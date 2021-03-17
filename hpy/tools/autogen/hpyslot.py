from pycparser import c_ast
from .autogenfile import AutoGenFile
from .parse import toC

class autogen_hpyslot_h(AutoGenFile):
    PATH = 'hpy/devel/include/common/autogen_hpyslot.h'

    def _type_slots(self):
        lines = []
        w = lines.append
        w('typedef enum {')
        for slot in self.api.hpyslots:
            w(f'    {slot.name} = {slot.value},')
        w('} HPySlot_Slot;')
        w('')
        for slot in self.api.hpyslots:
            w(f'#define _HPySlot_SIG__{slot.name} {slot.hpyfunc}')
        return lines

    def _module_slots(self):
        lines = []
        w = lines.append
        w('typedef enum {')
        for slot in self.api.hpymoduleslots:
            w(f'    {slot.name} = {slot.value},')
        w('} HPyModule_Slot;')
        w('')
        for slot in self.api.hpymoduleslots:
            w(f'#define _HPyModuleSlot_SIG__{slot.name} {slot.hpyfunc}')
        return lines

    def generate(self):
        lines = []
        lines.extend(self._type_slots())
        lines.append('')
        lines.extend(self._module_slots())
        return '\n'.join(lines)
