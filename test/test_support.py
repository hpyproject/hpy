from . import support

def test_expand_template():
    expanded = support.expand_template("""
        @EXPORT(f)
        @EXPORT(g)
        some more C stuff
        @INIT
    """, name='mytest')
    method_table = ['&f,', '&g,']
    methods = '\n        '.join(method_table)
    init_code = support.ExtensionTemplate.INIT_TEMPLATE % {
        'methods': methods,
        'legacy_methods': 'NULL',
        'name': 'mytest',
        'init_types': '',
    }
    assert expanded.rstrip() == f"""#include <hpy.h>

some more C stuff
{init_code}
""".rstrip()
