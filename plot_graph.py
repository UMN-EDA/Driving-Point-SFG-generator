#!/usr/bin/env python3
"""Render a packaged SFG run as PNG, SVG, DOT, and a standalone HTML report."""
import argparse
import csv
import html
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys


def quote(value):
    return json.dumps(str(value))


def short_node(node):
    return node.replace('.000000', '')


def render(directory):
    directory = Path(directory).resolve()
    with (directory / 'graph.tsv').open() as handle:
        edges = list(csv.DictReader(handle, delimiter='\t'))
    nodes = {}
    with (directory / 'nodes.tsv').open() as handle:
        for row in csv.DictReader(handle, delimiter='\t'):
            nodes[row['node']] = row['magic']
    for edge in edges:
        for key in ('source', 'destination'):
            nodes.setdefault(edge[key], '0')
    metadata = json.loads((directory / 'run.json').read_text())
    name = metadata['input_name']
    label_map = []
    dot = ['digraph SFG {', 'graph [rankdir=LR, bgcolor="white", pad=0.35, nodesep=0.35, ranksep=0.8];',
           'node [shape=ellipse, style=filled, fontname="Helvetica", fontsize=11, color="#64748b"];',
           'edge [fontname="Helvetica", fontsize=9, color="#64748b", arrowsize=0.7];']
    for node in sorted(nodes):
        color = '#ffedd5' if node.startswith('-') and node != '-100' else '#dbeafe'
        if node in ('-100', '100', '-1000'):
            color = '#dcfce7'
        label = 'VI' if node == '-100' else 'VO' if node in ('100', '-1000') else short_node(node)
        dot.append('{} [label={}, fillcolor={}];'.format(quote(node), quote(label), quote(color)))
    for index, edge in enumerate(edges, 1):
        identifier = 'e{:03d}'.format(index)
        weight = edge['weight']
        label = weight if len(weight) <= 22 else identifier
        label_map.append(dict(id=identifier, label=label, **edge))
        dot.append('{} -> {} [label={}, tooltip={}];'.format(
            quote(edge['source']), quote(edge['destination']), quote(label), quote(weight)))
    dot.append('}')
    (directory / 'graph.dot').write_text('\n'.join(dot)+'\n')
    with (directory / 'edge_legend.tsv').open('w', newline='') as handle:
        writer = csv.DictWriter(handle, fieldnames=['id', 'label', 'source', 'weight', 'destination'], delimiter='\t')
        writer.writeheader()
        writer.writerows(label_map)
    executable = shutil.which('dot')
    if executable:
        for fmt in ('svg', 'png'):
            subprocess.run([executable, '-T'+fmt, str(directory / 'graph.dot'),
                            '-o', str(directory / ('graph.'+fmt))],
                           check=True, timeout=60, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    else:
        fallback_plot(directory, nodes, label_map, name)
    esc = html.escape
    legend = ''.join('<tr><td>{}</td><td>{}</td><td>{}</td><td><code>{}</code></td></tr>'.format(
        esc(e['id']), esc(short_node(e['source'])), esc(short_node(e['destination'])), esc(e['weight']))
        for e in label_map)
    metrics = ' · '.join('{}: {}'.format(label, metadata.get(key, 'not requested')) for label, key in
                         [('Nodes', 'node_count'), ('Edges', 'edge_count'), ('Paths', 'paths_count'), ('Loops', 'loops_count')])
    available = ['graph.tsv', 'nodes.tsv', 'graph.json', 'graph.dot', 'edge_legend.tsv',
                 'paths.tsv', 'loops.tsv', 'path_gains.txt', 'loop_gains.txt', 'all_paths.txt', 'transfer_function.txt', 'generator.log', 'run.json']
    links = ' · '.join('<a href="{0}">{0}</a>'.format(f) for f in available if (directory/f).exists())
    walks = ''
    for kind in ('paths', 'loops'):
        file = directory / (kind+'.tsv')
        if file.exists():
            walks += '<details><summary>{}</summary><pre>{}</pre></details>'.format(kind.title(), esc(file.read_text()))
    document = '''<!doctype html>
<html lang="en"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>{name} — signal-flow graph</title>
<style>
body {{font:16px/1.5 system-ui,sans-serif;color:#172033;max-width:1400px;margin:36px auto;padding:0 24px;background:#f8fafc}}
h1 {{margin-bottom:4px}} .card {{background:white;border:1px solid #e2e8f0;border-radius:12px;padding:20px;margin:20px 0}}
a {{color:#1d4ed8}} object {{width:100%;min-height:420px;max-height:900px}} table {{border-collapse:collapse;width:100%}}
th,td {{padding:10px;border-bottom:1px solid #e2e8f0;text-align:left}} code {{overflow-wrap:anywhere}} pre {{overflow:auto}}
</style><h1>{name}</h1><p>Driving-point signal-flow graph · engine: {engine} · mode: {mode} · status: {status}</p>
<p>{metrics}</p><div class="card"><object data="graph.svg" type="image/svg+xml"><img src="graph.png" alt="Signal-flow graph"></object>
<p><a href="graph.svg">Open full SVG</a> · <a href="graph.png">PNG image</a></p>
<p>Green: input/output. Orange: auxiliary nodes. Blue: circuit nodes. Long gains use edge IDs; exact expressions appear below.</p></div>
<div class="card"><h2>Edge gains</h2><table><thead><tr><th>ID</th><th>Source</th><th>Destination</th><th>Gain</th></tr></thead><tbody>{legend}</tbody></table></div>
<div class="card"><h2>Paths and loops</h2>{walks}</div><p>{links}</p>
<p>Generated with the driving-point signal-flow graph toolkit.</p></html>
'''.format(name=esc(name), engine=esc(metadata['engine']), mode=esc(metadata['mode']),
           status=esc(metadata['status']), metrics=esc(metrics), legend=legend, walks=walks, links=links)
    (directory / 'report.html').write_text(document)


def fallback_plot(directory, nodes, edges, name):
    # Graphviz is optional. Keep this backend headless and preserve parallel edges.
    os.environ.setdefault('MPLCONFIGDIR', str(directory / '.matplotlib'))
    import matplotlib
    matplotlib.use('Agg')
    import matplotlib.pyplot as plt
    import networkx as nx
    graph = nx.MultiDiGraph()
    graph.add_nodes_from(sorted(nodes))
    for edge in edges:
        graph.add_edge(edge['source'], edge['destination'])
    positions = nx.spring_layout(graph, seed=19, iterations=200)
    size = min(24, max(10, len(nodes)**0.5*2))
    fig, ax = plt.subplots(figsize=(size, size*.75))
    colors = ['#dcfce7' if n in ('-100','100','-1000') else '#ffedd5' if n.startswith('-') and n != '-100' else '#dbeafe' for n in graph]
    nx.draw_networkx_nodes(graph, positions, node_color=colors, node_size=950, ax=ax)
    nx.draw_networkx_labels(graph, positions, labels={n: short_node(n) for n in graph}, font_size=8, ax=ax)
    nx.draw_networkx_edges(graph, positions, arrows=True, arrowsize=15,
                           connectionstyle='arc3,rad=0.12', node_size=950, ax=ax)
    # Edge IDs are unambiguous even when long gain expressions would overlap.
    pairs = {}
    for edge in edges:
        pairs.setdefault((edge['source'],edge['destination']), []).append(edge['id'])
    for (source, target), identifiers in pairs.items():
        x = (positions[source][0]+positions[target][0])/2
        y = (positions[source][1]+positions[target][1])/2
        ax.text(x,y,','.join(identifiers),fontsize=7,color='#334155',bbox=dict(facecolor='white',alpha=.75,edgecolor='none'))
    ax.set_title(name)
    ax.axis('off')
    fig.tight_layout()
    for fmt in ('png','svg'):
        fig.savefig(str(directory / ('graph.'+fmt)), dpi=150, bbox_inches='tight')
    plt.close(fig)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('run_directory', type=Path)
    args = parser.parse_args()
    try:
        render(args.run_directory)
    except (OSError, ValueError, ImportError, subprocess.SubprocessError) as error:
        print('Plot error: '+str(error),file=sys.stderr)
        return 1
    print('Report: '+str(args.run_directory.resolve()/'report.html'))
    return 0


if __name__ == '__main__':
    sys.exit(main())
