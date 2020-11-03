import time

class Microbench:

    def bench_a(self, timer):
        pass

    def bench_b(self, timer):
        pass

    def bench_foobar(self, timer):
        with timer:
            time.sleep(0.4)




