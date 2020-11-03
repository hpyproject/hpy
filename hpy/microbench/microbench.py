import time

class BenchModule:

    def bench_a(self, timer):
        pass

    def bench_b(self, timer):
        pass

    def bench_foobar(self, timer):
        with timer:
            time.sleep(0.4)



class BenchType:

    def bench_foo(self, timer):
        pass
