#!/usr/bin/env python3
"""Run every bundled example and write an execution-status index."""
import argparse
import html
import json
from pathlib import Path
import subprocess
import sys
from datetime import datetime
from sfg import ROOT, examples


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output-root', type=Path, default=None)
    parser.add_argument('--mode', choices=['graph','paths','loops','analysis','transfer'], default='graph')
    parser.add_argument('--analysis-backend', choices=['python','legacy'], default='python')
    parser.add_argument('--no-plot', action='store_true')
    parser.add_argument('--timeout', type=float, default=60)
    parser.add_argument('--skip-existing', action='store_true', help='Reuse completed run metadata in the output root')
    args = parser.parse_args()
    output = (args.output_root or ROOT/'runs'/('examples-'+datetime.now().strftime('%Y%m%d-%H%M%S-%f'))).resolve()
    if output.exists() and any(output.iterdir()) and not args.skip_existing:
        parser.error('Output root is not empty; use a new directory or --skip-existing.')
    output.mkdir(parents=True, exist_ok=True)
    rows = []
    for path in examples():
        directory = output/path.stem
        summary = directory/'run.json'
        if not (args.skip_existing and summary.exists()):
            command = [sys.executable, str(ROOT/'sfg.py'), str(path), '--output', str(directory),
                       '--mode', args.mode, '--timeout', str(args.timeout), '--analysis-backend', args.analysis_backend]
            if args.no_plot:
                command.append('--no-plot')
            result = subprocess.run(command)
            if result.returncode and not summary.exists():
                rows.append({'input_name':path.name,'status':'wrapper_failed'})
                continue
        info = json.loads(summary.read_text())
        if info['mode'] != args.mode or (args.mode in ('paths','loops','analysis') and info.get('analysis_backend') != args.analysis_backend):
            parser.error('Existing run settings differ: '+str(directory))
        rows.append({k:info.get(k) for k in ['input_name','engine','mode','analysis_backend','status','plot_status',
                                            'node_count','edge_count','paths_count','loops_count','elapsed_seconds']})
    (output/'index.json').write_text(json.dumps(rows,indent=2)+'\n')
    entries = []
    for row in rows:
        stem = Path(row['input_name']).stem
        report = stem+'/report.html' if (output/stem/'report.html').exists() else stem+'/run.json'
        entries.append('<tr><td><a href="{}">{}</a></td>{}</tr>'.format(html.escape(report),html.escape(stem),
            ''.join('<td>{}</td>'.format(html.escape(str(row.get(k,'—')))) for k in
                    ['engine','status','node_count','edge_count','paths_count','loops_count'])))
    (output/'index.html').write_text('''<!doctype html><html lang="en"><meta charset="utf-8">
<title>SFG example results</title><style>body{font:16px system-ui;max-width:1200px;margin:40px auto;padding:20px}table{border-collapse:collapse;width:100%}td,th{padding:12px;border-bottom:1px solid #ddd;text-align:left}a{color:#1d4ed8}</style>
<h1>SFG example results</h1><p>Browse the generated graph, circuit data, and analysis outputs for each example.</p>
<table><tr><th>Example</th><th>Engine</th><th>Status</th><th>Nodes</th><th>Edges</th><th>Paths</th><th>Loops</th></tr>'''+''.join(entries)+'</table></html>\n')
    failed = sum(r['status'] != 'success' or r.get('plot_status') == 'failed' for r in rows)
    print('{} examples; {} failed. Index: {}'.format(len(rows),failed,output/'index.html'))
    return 1 if failed else 0


if __name__ == '__main__':
    sys.exit(main())
