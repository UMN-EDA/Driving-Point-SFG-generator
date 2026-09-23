#!/usr/bin/env python3
"""Build and run the preserved driving-point SFG generators in isolated folders."""
import argparse
import csv
import hashlib
import json
import math
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import time
from datetime import datetime, timezone

ROOT = Path(__file__).resolve().parent
ENGINES = {
    'ct': (1, 'readCkt3.cpp'),
    'sc': (2, 'readCkt3_SC_1.7.cpp'),
    'latest': (3, 'readCkt3_SC_1.8_NEW.cpp'),
}


def examples():
    return sorted((ROOT / 'examples').rglob('*.isc'))


def resolve_input(value):
    path = Path(value).expanduser()
    if path.is_file():
        return path.resolve()
    package_path = ROOT / path
    if package_path.is_file():
        return package_path.resolve()
    matches = [p for p in examples() if value in (p.name, p.stem)]
    if len(matches) == 1:
        return matches[0]
    raise ValueError('Input not found or ambiguous: {}. Use --list-examples.'.format(value))


def parse_input(path):
    # Normalize formatting only. Component labels and circuit connectivity are preserved.
    lines = []
    for raw in path.read_text().splitlines():
        line = raw.split('//', 1)[0].split('#', 1)[0].strip()
        if line:
            lines.append(line)
    if len(lines) < 4 or lines[-1] != 'END':
        raise ValueError('Input must contain a header, VI/VO connections, and a final END.')
    if not lines[0].isdigit():
        raise ValueError('First line must be a node count or phase count.')
    phased = len(lines) > 1 and lines[1].isdigit()
    phases = int(lines[0]) if phased else 1
    count = int(lines[1] if phased else lines[0])
    if phases < 1 or count < 1:
        raise ValueError('Phase and node counts must be positive.')
    records = lines[2 if phased else 1:-1]
    has_transistors = False
    vi = vo = 0
    for line in records:
        fields = line.split('_')
        if line.startswith(('M', 'P')):
            has_transistors = True
            if len(fields) != 4 or not re.fullmatch(r'[MP][A-Za-z0-9]+', fields[0]):
                raise ValueError('Expected transistor_GATE_DRAIN_SOURCE: ' + line)
            nodes = fields[1:]
        else:
            if len(fields) != 3:
                raise ValueError('Expected SOURCE_ELEMENT_DESTINATION: ' + line)
            src, element, dst = fields
            if not re.fullmatch(r'(?:1|OP[+-]|[CG][A-Za-z0-9]*|S[0-9]+)', element):
                raise ValueError('Unsupported element label: ' + element)
            if element.startswith('S') and not 1 <= int(element[1:]) <= phases:
                raise ValueError('Switch phase outside declared phase count: ' + line)
            vi += src == 'VI'
            vo += dst == 'VO'
            if (src == 'VI' or dst == 'VO') and element != '1':
                raise ValueError('VI/VO connections must have unit weight: ' + line)
            nodes = [n for n in (src, dst) if n not in ('VI', 'VO')]
            if src == 'VO' or dst == 'VI':
                raise ValueError('Use VI as source and VO as destination: ' + line)
        if any(not n.isdigit() or not 0 <= int(n) <= count for n in nodes):
            raise ValueError('Node outside 0..{}: {}'.format(count, line))
    if vi != 1 or vo != 1:
        raise ValueError('Exactly one VI connection and one VO connection are required.')
    if not records[0].startswith('VI_1_') or not records[1].endswith('_1_VO'):
        raise ValueError('Place the VI connection first and the VO connection second.')
    return {'phases': phases, 'nodes': count, 'phased_header': phased,
            'transistors': has_transistors, 'records': records}


def select_engine(info, requested):
    if requested == 'auto':
        return 'latest' if info['transistors'] else 'sc' if info['phased_header'] else 'ct'
    if requested == 'ct' and (info['transistors'] or info['phases'] != 1 or
                              any('_S' in r for r in info['records'])):
        raise ValueError('The ct engine does not support switches or transistors.')
    if requested == 'sc' and info['transistors']:
        raise ValueError('The sc engine does not support transistors; use latest.')
    return requested


def normalized_input(info, engine):
    header = [str(info['nodes'])]
    if engine != 'ct':
        header.insert(0, str(info['phases']))
    return '\n'.join(header + info['records'] + ['END']) + '\n'


def build(engine):
    number, filename = ENGINES[engine]
    compiler = shutil.which(os.environ.get('CXX', 'g++'))
    if not compiler:
        raise ValueError('C++ compiler not found. Install g++ or set CXX to a compiler path.')
    source = ROOT / 'src/driver.cpp'
    legacy = ROOT / 'src/legacy' / filename
    flags = ['-std=c++11', '-O0', '-DSFG_ENGINE=' + str(number)]
    version = subprocess.run([compiler, '--version'], stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT, text=True, check=True).stdout
    digest = hashlib.sha256(source.read_bytes() + legacy.read_bytes() +
                            repr([compiler, version, flags]).encode()).hexdigest()
    directory = ROOT / 'build'
    directory.mkdir(exist_ok=True)
    binary = directory / ('sfg-' + engine + '-' + digest[:16])
    command = [compiler] + flags + [str(source), '-o', str(binary)]
    if not binary.exists():
        temporary = binary.with_name(binary.name + '.tmp-' + str(os.getpid()))
        compile_command = command[:-1] + [str(temporary)]
        print('Building {} engine...'.format(engine), flush=True)
        with (directory / ('build-' + engine + '.log')).open('w') as log:
            result = subprocess.run(compile_command, stdout=log, stderr=subprocess.STDOUT)
        if result.returncode:
            if temporary.exists():
                temporary.unlink()
            raise ValueError('Build failed; see ' + str(directory / ('build-' + engine + '.log')))
        temporary.replace(binary)
    return binary, {'compiler': version.splitlines()[0], 'command': [Path(compiler).name] + flags + ['src/driver.cpp', '-o', str(binary.relative_to(ROOT))],
                    'command_directory': 'package root',
                    'build_sha256': digest,
                    'legacy_source': 'src/legacy/' + filename,
                    'legacy_sha256': hashlib.sha256(legacy.read_bytes()).hexdigest()}


def read_graph(path):
    with path.open(newline='') as handle:
        edges = list(csv.DictReader(handle, delimiter='\t'))
    for edge in edges:
        if set(edge) != {'source', 'weight', 'destination'} or None in edge.values():
            raise ValueError('Malformed graph export: ' + str(path))
    return edges


def write_summary(output, metadata):
    graph = output / 'graph.tsv'
    if graph.exists():
        edges = read_graph(graph)
        nodes = set()
        if (output / 'nodes.tsv').exists():
            with (output / 'nodes.tsv').open() as handle:
                nodes.update(row['node'] for row in csv.DictReader(handle, delimiter='\t'))
        nodes.update(e[k] for e in edges for k in ('source', 'destination'))
        metadata.update(node_count=len(nodes), edge_count=len(edges))
        (output / 'graph.json').write_text(json.dumps({'nodes': sorted(nodes), 'edges': edges}, indent=2)+'\n')
    for name in ('paths', 'loops'):
        file = output / (name + '.tsv')
        if file.exists():
            metadata[name + '_count'] = len(file.read_text().splitlines())
    parts = [output / name for name in ('paths.tsv', 'loops.tsv') if (output / name).exists()]
    if parts:
        (output / 'all_paths.txt').write_text(''.join(p.read_text() for p in parts))
    (output / 'run.json').write_text(json.dumps(metadata, indent=2) + '\n')


def run(args):
    path = resolve_input(args.input)
    info = parse_input(path)
    engine = select_engine(info, args.engine)
    binary, build_info = build(engine)
    python_analysis = args.mode in ('paths', 'loops', 'analysis') and args.analysis_backend == 'python'
    if python_analysis:
        try:
            import networkx
        except ImportError:
            raise ValueError('Python analysis requires networkx; install requirements.txt.')
    if not args.no_plot:
        os.environ.setdefault('MPLCONFIGDIR', str(ROOT / '.cache/matplotlib'))
        try:
            import matplotlib
            import networkx
        except ImportError as error:
            raise ValueError('Install requirements.txt or use --no-plot: ' + str(error))
    if args.output:
        output = Path(args.output).expanduser().resolve()
    else:
        stamp = datetime.now().strftime('%Y%m%d-%H%M%S-%f')
        output = ROOT / 'runs' / (path.stem + '-' + stamp)
    if output.exists() and any(output.iterdir()):
        raise ValueError('Output directory is not empty; choose a new --output: ' + str(output))
    output.mkdir(parents=True, exist_ok=True)
    shutil.copy2(path, output / 'input.original.isc')
    (output / 'input.isc').write_text(normalized_input(info, engine))
    metadata = {
        'input_name': path.name, 'input_sha256': hashlib.sha256(path.read_bytes()).hexdigest(),
        'engine': engine, 'mode': args.mode, 'phases': info['phases'],
        'started_utc': datetime.now(timezone.utc).isoformat(),
        'status': 'running', 'timeout_seconds': args.timeout,
        'analysis_backend': 'legacy' if args.mode == 'transfer' else args.analysis_backend if args.mode != 'graph' else None,
        'max_walks': args.max_walks, **build_info,
    }
    write_summary(output, metadata)
    started = time.monotonic()
    # No shell: filenames are passed as arguments and all outputs stay in the run directory.
    # Disable core dumps because experimental legacy circuits can crash.
    def child_limits():
        import resource
        resource.setrlimit(resource.RLIMIT_CORE, (0, 0))
    try:
        with (output / 'generator.log').open('w') as log:
            result = subprocess.run([str(binary), 'input.isc', 'graph' if python_analysis else args.mode], cwd=str(output),
                                    stdout=log, stderr=subprocess.STDOUT,
                                    timeout=args.timeout, preexec_fn=child_limits if os.name == 'posix' else None)
        metadata['return_code'] = result.returncode
        metadata['status'] = 'success' if result.returncode == 0 else 'failed'
    except subprocess.TimeoutExpired:
        metadata['status'] = 'timeout'
    if metadata['status'] == 'success' and python_analysis:
        try:
            with (output / 'analysis.log').open('w') as log:
                result = subprocess.run([sys.executable, str(ROOT / 'analyze_graph.py'), str(output),
                                         '--mode', args.mode, '--max-walks', str(args.max_walks)],
                                        stdout=log, stderr=subprocess.STDOUT, timeout=args.timeout)
            metadata['analysis_return_code'] = result.returncode
            if result.returncode:
                metadata['status'] = 'analysis_failed'
        except subprocess.TimeoutExpired:
            metadata['status'] = 'analysis_timeout'
    metadata['elapsed_seconds'] = round(time.monotonic() - started, 3)
    write_summary(output, metadata)
    if not args.no_plot and (output / 'graph.tsv').exists():
        try:
            from plot_graph import render
            render(output)
            metadata['plot_status'] = 'success'
        except Exception as error:
            metadata['plot_status'] = 'failed'
            metadata['plot_error'] = str(error)
        write_summary(output, metadata)
    if args.count is not None:
        combined = output / 'all_paths.txt'
        if combined.exists():
            print('{}: {} substring occurrences'.format(args.count, combined.read_text().count(args.count)))
        else:
            print('No path/loop export available to count.')
    print('Engine: {} | status: {}'.format(engine, metadata['status']))
    print('Results: ' + str(output))
    if 'node_count' in metadata:
        print('Nodes: {} | edges: {} | paths: {} | loops: {}'.format(
            metadata['node_count'], metadata['edge_count'], metadata.get('paths_count', 'not requested'),
            metadata.get('loops_count', 'not requested')))
    if metadata['status'] != 'success':
        print('The run did not complete. See generator.log, analysis.log (if present), and run.json.', file=sys.stderr)
        return 1
    if metadata.get('plot_status') == 'failed':
        print('Plotting failed: ' + metadata['plot_error'], file=sys.stderr)
        return 1
    return 0


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('input', nargs='?', help='Example name, .isc filename, or path to a custom circuit')
    parser.add_argument('--list-examples', action='store_true', help='List bundled inputs and selected engines')
    parser.add_argument('--engine', choices=['auto'] + list(ENGINES), default='auto')
    parser.add_argument('--mode', choices=['graph', 'paths', 'loops', 'analysis', 'transfer'], default='graph',
                        help='transfer also invokes the original symbolic TF routines')
    parser.add_argument('--analysis-backend', choices=['python', 'legacy'], default='python',
                        help='Path/loop backend; transfer always uses the original C++ routines')
    parser.add_argument('--max-walks', type=int, default=100000, help='Path+cycle export limit for Python analysis')
    parser.add_argument('--output', help='New or empty output directory (default: timestamped runs/ folder)')
    parser.add_argument('--no-plot', action='store_true', help='Skip plotting; requires only Python standard library and C++')
    parser.add_argument('--timeout', type=float, default=60, help='Maximum seconds for the native generator')
    parser.add_argument('--count', metavar='TEXT', help='Count a substring in the exported paths and loops')
    args = parser.parse_args()
    if args.list_examples:
        for path in examples():
            info = parse_input(path)
            print('{:<46} {:<7} {}'.format(path.stem, select_engine(info, 'auto'), path.relative_to(ROOT)))
        return 0
    if not args.input:
        parser.error('Provide an example/path, or use --list-examples.')
    if args.max_walks < 1:
        parser.error('--max-walks must be positive.')
    if not math.isfinite(args.timeout) or args.timeout <= 0:
        parser.error('--timeout must be a finite positive number.')
    try:
        return run(args)
    except (ValueError, OSError, subprocess.SubprocessError) as error:
        print('Error: ' + str(error), file=sys.stderr)
        return 2


if __name__ == '__main__':
    sys.exit(main())
