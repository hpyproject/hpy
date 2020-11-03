"""
Usage: microbench [options]

Options:
  -k --match SUBSTRING     Run only the benchmarks matching SUBSTRING
"""
import time
from dataclasses import dataclass
import docopt
from hpy.microbench import microbench

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

    def collect(self):
        return self.collect_module(microbench)

    def collect_module(self, mod):
        benchmarks = []
        for name in dir(mod):
            obj = getattr(mod, name)
            if isinstance(obj, type) and obj.__name__.startswith('Bench'):
                benchmarks += self.collect_class(obj)
        return benchmarks

    def collect_class(self, cls):
        instance = cls()
        benchmarks = []
        for name in dir(instance):
            if not name.startswith('bench_'):
                continue
            fullname = f'{cls.__name__}::{name}'
            meth = getattr(instance, name)
            benchmarks.append(Benchmark(fullname, meth))
        #
        benchmarks.sort(key = lambda b: b.meth.__code__.co_firstlineno)
        return benchmarks

    def run(self):
        benchmarks = self.collect()
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
