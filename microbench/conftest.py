import time
import pytest

class Timer:

    def __init__(self, nodeid):
        self.nodeid = nodeid
        self.start = None
        self.stop = None

    def __enter__(self):
        if self.start is not None:
            raise ValueError('You cannot use "with timer:" more than once')
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

    def has_timing(self):
        return self.start is not None and self.stop is not None


class TimerSession:

    def __init__(self):
        self.timers = {} # nodeid -> Timer

    def new_timer(self, nodeid):
        timer = Timer(nodeid)
        self.timers[nodeid] = timer
        return timer

    def get_timer(self, nodeid):
        return self.timers.get(nodeid)


@pytest.fixture
def timer(request):
    return request.config._timersession.new_timer(request.node.nodeid)

def pytest_configure(config):
    config._timersession = TimerSession()

@pytest.hookimpl(hookwrapper=True)
def pytest_report_teststatus(report, config):
    outcome = yield
    category, letter, word = outcome.get_result()
    timer = config._timersession.get_timer(report.nodeid)
    if category == 'passed' and timer:
        if timer.has_timing():
            word = str(timer)
        else:
            word = (str(timer), {'red': True})
        outcome.force_result((category, letter, word))
