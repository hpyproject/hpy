from pycparser import c_ast
from .autogenfile import AutoGenFile
from .parse import toC

class autogen_hpyslot_h(AutoGenFile):
    PATH = 'hpy/devel/include/hpy/autogen_hpyslot.h'

    def generate(self):
        lines = []
        w = lines.append
        w('typedef enum {')
        for slot in self.api.hpyslots:
            w(f'    {slot.name} = {slot.value},')
        w('} HPySlot_Slot;')
        w('')
        for slot in self.api.hpyslots:
            w(f'#define _HPySlot_SIG__{slot.name} {slot.hpyfunc}')
        return '\n'.join(lines)

class cpython_autogen_hpytype_checkslot_h(AutoGenFile):
    PATH = 'hpy/devel/include/hpy/cpython/autogen_hpytype_checkslot.h'

    def generate(self):
        lines = []
        w = lines.append

        w('_HPy_UNUSED static inline int')
        w('_HPyType_CheckSlot(HPy type, HPyDef *value)')
        w('{')
        w('    PyTypeObject *t = (PyTypeObject *) _h2py(type);')
        w('    void *tmp;')
        w('    char msg[64];')
        w('    switch (value->slot.slot) {')
        for slot in self.api.hpyslots:
            # have fast-paths for number methods; more could be added if beneficial
            if slot.name.startswith('HPy_nb_'):
                w(f'    case {slot.name}:')
                # remove prefix 'HPy_'
                py_slot = slot.name[4:]
                assert py_slot.startswith('nb_')
                w(f'            return t->tp_as_number != NULL && (void*) t->tp_as_number->{py_slot} == (void*) value->slot.cpy_trampoline;')
        w('    default:')
        w('        tmp = PyType_GetSlot(t, (int) value->slot.slot);')
        w('        if (tmp == NULL && PyErr_Occurred()) {')
        w('            PyErr_Clear();')
        w('            snprintf(msg, sizeof(msg), "Unsupported slot in HPyType_CheckSlot: %d", value->slot.slot);')
        w('            Py_FatalError(msg);')
        w('        }')
        w('        return tmp == (void*) value->slot.cpy_trampoline;')
        w('    }')
        w('}')
        return '\n'.join(lines)

class universal_autogen_hpytype_checkslot_h(cpython_autogen_hpytype_checkslot_h):
    PATH = 'hpy/universal/src/autogen_hpytype_checkslot.h'
