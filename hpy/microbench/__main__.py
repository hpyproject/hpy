"""
Usage: microbench [options]

Options:
  -k --match SUBSTRING     Run only the benchmarks matching SUBSTRING
"""
import time
from dataclasses import dataclass
import docopt
from hpy.microbench.microbench import Microbench

@dataclass
class Benchmark:
    name: str
    meth: object

class Timer(object):

    def __init__(self):
        self.start = None
        self.stop = None

    def __enter__(self):
        self.start = time.time()

    def __exit__(self, etype, evalue, tb):
        self.stop = time.time()

    def __str__(self):
        if self.start is None:
            return '[NOT STARTED]'
        if self.stop is None:
            return '[IN PROGRESS]'
        usec = (self.stop - self.start) * 1000
        return f'{usec:.2f} us'


class Runner:

    def __init__(self, match):
        self.match  = match
        self.instance = Microbench()

    def get_benchmarks(self):
        names = [x for x in dir(self.instance) if x.startswith('bench_')]
        benchmarks = [Benchmark(name, getattr(self.instance, name))
                      for name in names]
        benchmarks.sort(key = lambda b: b.meth.__code__.co_firstlineno)
        return benchmarks


    def run(self):
        benchmarks = self.get_benchmarks()
        L = max([len(bench.name) for bench in benchmarks])
        for bench in benchmarks:
            print(f'{bench.name:{L}}', end='', flush=True)
            if self.match in bench.name:
                timer = Timer()
                bench.meth(timer)
                outcome = str(timer)
            else:
                outcome = 'SKIP'
            #
            print(f'{outcome:>14}')


def main():
    args = docopt.docopt(__doc__)
    match = args['--match'] or ''
    runner = Runner(match)
    runner.run()

if __name__ == '__main__':
    main()
