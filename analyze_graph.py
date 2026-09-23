#!/usr/bin/env python3
"""Enumerate paths and cycles from the unchanged generator's directed multigraph."""
import argparse
import csv
from contextlib import ExitStack
import itertools
import json
from pathlib import Path
import sys


def analyze(directory, mode='analysis', max_walks=100000):
    import networkx as nx
    directory = Path(directory)
    with (directory/'graph.tsv').open() as handle:
        edges = list(csv.DictReader(handle, delimiter='\t'))
    graph = nx.DiGraph()
    adjacency = {}
    for index, edge in enumerate(edges, 1):
        source, target = edge['source'], edge['destination']
        graph.add_edge(source, target)
        adjacency.setdefault((source,target), []).append((index,edge['weight']))
    with (directory/'nodes.tsv').open() as handle:
        graph.add_nodes_from(row['node'] for row in csv.DictReader(handle,delimiter='\t'))
    target = '-1000' if '-1000' in graph else '100'
    if '-100' not in graph or target not in graph:
        raise ValueError('Graph must contain the legacy VI and VO node identifiers.')
    counts = {}
    total = 0
    for kind, closed in [('paths',False),('loops',True)]:
        if mode not in ('analysis',kind):
            continue
        walks = nx.simple_cycles(graph) if closed else nx.all_simple_paths(graph,'-100',target)
        with ExitStack() as stack:
            files = {suffix:stack.enter_context((directory/(kind+suffix)).open('w'))
                     for suffix in ('.tsv','_nodes.tsv','_edges.tsv','_edge_ids.tsv')}
            gains = stack.enter_context((directory/('path_gains.txt' if kind=='paths' else 'loop_gains.txt')).open('w'))
            count = 0
            for raw_nodes in walks:
                nodes = list(raw_nodes)
                # Canonicalize the starting point of cycles for readable, stable identifiers.
                if closed:
                    start = min(range(len(nodes)),key=lambda i:nodes[i])
                    nodes = nodes[start:]+nodes[:start]
                pairs = list(zip(nodes,nodes[1:]))
                if closed:
                    pairs.append((nodes[-1],nodes[0]))
                for selected in itertools.product(*(adjacency[pair] for pair in pairs)):
                    total += 1
                    if total > max_walks:
                        raise ValueError('Walk limit exceeded; partial exports retained. Increase --max-walks.')
                    count += 1
                    ids = [str(e[0]) for e in selected]
                    weights = [e[1] for e in selected]
                    walk = []
                    for i, weight in enumerate(weights):
                        walk.extend([nodes[i],weight])
                    walk.append(nodes[0] if closed else nodes[-1])
                    files['.tsv'].write('\t'.join(walk)+'\n')
                    files['_nodes.tsv'].write('\t'.join(nodes)+'\n')
                    files['_edges.tsv'].write('\t'.join(weights)+'\n')
                    files['_edge_ids.tsv'].write('\t'.join(ids)+'\n')
                    gains.write('*'.join('('+w+')' for w in weights if w != '1') or '1')
                    gains.write('\n')
            counts[kind+'_count'] = count
    (directory/'python_analysis.json').write_text(json.dumps({
        'backend':'networkx', 'networkx_version':nx.__version__,
        'semantics':'Simple node paths/cycles expanded over each distinct parallel edge; no symbolic simplification.',
        **counts},indent=2)+'\n')
    return counts


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('directory',type=Path)
    parser.add_argument('--mode',choices=['paths','loops','analysis'],default='analysis')
    parser.add_argument('--max-walks',type=int,default=100000)
    args=parser.parse_args()
    if args.max_walks < 1:
        parser.error('--max-walks must be positive.')
    try:
        print(json.dumps(analyze(args.directory,args.mode,args.max_walks)))
    except (ImportError,ValueError,OSError) as error:
        print('Analysis error: '+str(error),file=sys.stderr)
        return 1
    return 0


if __name__=='__main__':
    sys.exit(main())
