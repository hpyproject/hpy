class Foo:
    """"""

    def __len__(self):
        return 42

    def __getitem__(self, key):
        return None

    def noargs(
        self,
    ):
        """"""

    def onearg(self, arg):
        """"""

    def varargs(self, *args):
        """"""

    def call_with_tuple(self, func, *args):
        """"""
        if len(args) != 1:
            raise TypeError("call_with_tuple requires two arguments")
        return func(*args[0])

    def call_with_tuple_and_dict(self, func, *args):
        """"""
        return func(args[0][0], args[1]["b"])

    def allocate_int(
        self,
    ):
        return 2048

    def allocate_tuple(self):
        return (2048, 2049)


def noargs():
    """"""


def onearg(arg):
    """"""


def varargs(*args):
    """"""


def call_with_tuple(func, *args):
    """"""
    if len(args) != 1:
        raise TypeError("call_with_tuple requires two arguments")
    return func(*args[0])


def call_with_tuple_and_dict(func, *args):
    """"""
    return func(args[0][0], args[1]["b"])


def allocate_int():
    return 2048


def allocate_tuple():
    return (2048, 2049)


HTFoo = Foo
