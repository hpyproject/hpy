from .autogenfile import AutoGenFile


class autogen_ctx_h(AutoGenFile):
    PATH = 'hpy/devel/include/autogen_ctx.h'

    ## struct _HPyContext_s {
    ##     int ctx_version;
    ##     HPy h_None;
    ##     ...
    ##     HPy (*ctx_Module_Create)(HPyContext ctx, HPyModuleDef *def);
    ##     ...
    ## }

    def generate(self):
        lines = []
        w = lines.append
        w('struct _HPyContext_s {')
        w('    int ctx_version;')
        for f in self.api.declarations:
            w('    %s;' % f.ctx_decl())
        w('};')
        return '\n'.join(lines)
