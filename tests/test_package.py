"""Contract and integration tests for the package, without changing legacy sources."""
import csv
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
from sfg import examples, parse_input, select_engine
from analyze_graph import analyze


class PackageTests(unittest.TestCase):
    def test_preserved_files(self):
        manifest=json.loads((ROOT/'docs/original_sha256.json').read_text())
        for name, digest in manifest.items():
            with self.subTest(file=name):
                self.assertEqual(hashlib.sha256((ROOT/name).read_bytes()).hexdigest(),digest)

    def test_example_families(self):
        expected={'continuous_time':'ct','switched_capacitor':'sc','transistor':'latest','experimental':'latest'}
        self.assertEqual(len(examples()),17)
        for path in examples():
            with self.subTest(example=path.name):
                self.assertEqual(select_engine(parse_input(path),'auto'),expected[path.parent.name])

    def test_parallel_edges_and_cycles(self):
        # Two VI->a and two a->VO edges give four paths. A self-loop adds one cycle.
        with tempfile.TemporaryDirectory() as tmp:
            directory=Path(tmp)
            (directory/'graph.tsv').write_text('source\tweight\tdestination\n-100\tA\ta\n-100\tB\ta\na\tC\t100\na\tD\t100\na\tL\ta\n')
            (directory/'nodes.tsv').write_text('node\tmagic\n-100\t0\na\t0\n100\t0\n')
            counts=analyze(directory)
            self.assertEqual(counts,{'paths_count':4,'loops_count':1})
            self.assertEqual(set((directory/'paths_edges.tsv').read_text().splitlines()),{'A\tC','A\tD','B\tC','B\tD'})
            self.assertEqual((directory/'loops.tsv').read_text(),'a\tL\ta\n')

    def test_walk_limit(self):
        with tempfile.TemporaryDirectory() as tmp:
            directory=Path(tmp)
            (directory/'graph.tsv').write_text('source\tweight\tdestination\n-100\tA\t100\n-100\tB\t100\n')
            (directory/'nodes.tsv').write_text('node\tmagic\n-100\t0\n100\t0\n')
            with self.assertRaisesRegex(ValueError,'limit'):
                analyze(directory,max_walks=1)

    def run_cli(self, *args, cwd=None):
        return subprocess.run([sys.executable,str(ROOT/'sfg.py'),*args],cwd=cwd,
                              stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True,timeout=120)

    def test_custom_input_and_output_isolation(self):
        with tempfile.TemporaryDirectory(prefix='sfg custom ') as tmp:
            directory=Path(tmp)
            custom=directory/'custom circuit.isc'
            custom.write_text('# One-pole RC\n2\nVI_1_1\n2_1_VO\n1_G1_2\n2_C1_0\nEND\n')
            result=self.run_cli(str(custom),'--mode','analysis','--no-plot','--output',str(directory/'out'),cwd=directory)
            self.assertEqual(result.returncode,0,result.stdout)
            metadata=json.loads((directory/'out/run.json').read_text())
            self.assertEqual(metadata['paths_count'],1)
            self.assertEqual(metadata['loops_count'],0)
            self.assertEqual(metadata['engine'],'ct')
            self.assertIn('(1/(G1+sC1))',(directory/'out/graph.tsv').read_text())
            self.assertFalse((directory/'graph.tsv').exists())
            original=(directory/'out/graph.tsv').read_bytes()
            repeated=self.run_cli(str(custom),'--no-plot','--output',str(directory/'out'))
            self.assertEqual(repeated.returncode,2)
            self.assertEqual((directory/'out/graph.tsv').read_bytes(),original)

    def test_mode_selection(self):
        with tempfile.TemporaryDirectory() as tmp:
            for mode in ('graph','paths','loops','transfer'):
                output=Path(tmp)/mode
                result=self.run_cli('1stRCLPF','--mode',mode,'--no-plot','--output',str(output))
                self.assertEqual(result.returncode,0,result.stdout)
                self.assertEqual((output/'paths.tsv').exists(),mode in ('paths','transfer'))
                self.assertEqual((output/'loops.tsv').exists(),mode in ('loops','transfer'))
                self.assertEqual((output/'transfer_function.txt').exists(),mode=='transfer')

    def test_invalid_input_rejected_before_execution(self):
        with tempfile.TemporaryDirectory() as tmp:
            input_path=Path(tmp)/'bad.isc'
            input_path.write_text('2\nVI_1_1\n2_1_VO\n1_G1_3\nEND\n')
            result=self.run_cli(str(input_path),'--no-plot','--output',str(Path(tmp)/'out'))
            self.assertEqual(result.returncode,2)
            self.assertIn('Node outside',result.stdout)
            self.assertFalse((Path(tmp)/'out').exists())

    def test_transistor_reference(self):
        with tempfile.TemporaryDirectory() as tmp:
            result=self.run_cli('5T_OTA_Integrator_tran','--mode','analysis','--no-plot','--output',tmp)
            self.assertEqual(result.returncode,0,result.stdout)
            metadata=json.loads((Path(tmp)/'run.json').read_text())
            self.assertEqual((metadata['edge_count'],metadata['paths_count'],metadata['loops_count']),(24,10,3))

    def test_switched_capacitor_analysis(self):
        with tempfile.TemporaryDirectory() as tmp:
            result=self.run_cli('FirstOrderSCFilter','--mode','analysis','--no-plot','--output',tmp)
            self.assertEqual(result.returncode,0,result.stdout)
            metadata=json.loads((Path(tmp)/'run.json').read_text())
            self.assertEqual((metadata['edge_count'],metadata['paths_count'],metadata['loops_count']),(22,20,12))

    def test_matplotlib_fallback(self):
        from plot_graph import fallback_plot
        with tempfile.TemporaryDirectory() as tmp:
            fallback_plot(Path(tmp),{'-100':'0','100':'0'},[
                {'id':'e001','source':'-100','destination':'100','weight':'1','label':'1'}], 'Simple graph')
            self.assertTrue((Path(tmp)/'graph.png').read_bytes().startswith(b'\x89PNG'))
            self.assertIn('<svg',(Path(tmp)/'graph.svg').read_text())


if __name__=='__main__':
    unittest.main()
