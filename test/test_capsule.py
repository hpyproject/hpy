"""
NOTE: these tests are also meant to be run as PyPy "applevel" tests.

This means that global imports will NOT be visible inside the test
functions. In particular, you have to "import pytest" inside the test in order
to be able to use e.g. pytest.raises (which on PyPy will be implemented by a
"fake pytest module")
"""
import pytest
from .support import HPyTest, DefaultExtensionTemplate

class CapsuleTemplate(DefaultExtensionTemplate):

    def DEFINE_SomeObject(self):
        return """
            typedef struct {
                int value;
                char message[];
            } SomeObject;
        """

    def DEFINE_Capsule_New(self, destructor="NULL"):
        return """
            #include <string.h>

            #define CAPSULE_NAME "some_capsule"

            HPyDef_METH(Capsule_New, "capsule_new", capsule_new_impl, HPyFunc_VARARGS)
            static HPy capsule_new_impl(HPyContext *ctx, HPy self, HPy *args, HPy_ssize_t nargs)
            {
                int value;
                char *message;
                size_t n_message;
                if (!HPyArg_Parse(ctx, NULL, args, nargs, "is", &value, &message))
                {
                    return HPy_NULL;
                }
                n_message = strlen(message) + 1;
                SomeObject *pointer = (SomeObject *) malloc(sizeof(SomeObject) + n_message * sizeof(char));
                pointer->value = value;
                strncpy(pointer->message, message, n_message);
                return HPyCapsule_New(ctx, pointer, CAPSULE_NAME, (HPyCapsule_Destructor) %s);
            }
        """ % destructor

    def DEFINE_Payload_Free(self):
        return """
            #include <string.h>

            HPyDef_METH(Payload_Free, "payload_free", payload_free_impl, HPyFunc_O)
            static HPy payload_free_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                void *pointer = HPyCapsule_GetPointer(ctx, arg, CAPSULE_NAME);
                if (pointer != NULL)
                {
                    free(pointer);
                }
                return HPy_Dup(ctx, ctx->h_None);
            }
        """

    def DEFINE_Capsule_GetName(self):
        return """
            HPyDef_METH(Capsule_GetName, "capsule_getname", capsule_getname_impl, HPyFunc_O)
            static HPy capsule_getname_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                const char *name = HPyCapsule_GetName(ctx, arg);
                return HPyUnicode_FromString(ctx, name);
            }
        """

    def DEFINE_Capsule_GetPointer(self):
        return """
            HPyDef_METH(Capsule_GetPointer, "capsule_getpointer", capsule_get_payload_impl, HPyFunc_O)
            static HPy capsule_get_payload_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                SomeObject *pointer = (SomeObject *) HPyCapsule_GetPointer(ctx, arg, CAPSULE_NAME);
                HPy value = HPyLong_FromLong(ctx, pointer->value);
                HPy message = HPyUnicode_FromString(ctx, pointer->message);
                HPy result = HPyTuple_Pack(ctx, 2, value, message);
                HPy_Close(ctx, value);
                HPy_Close(ctx, message);
                return result;
            }
        """

class TestHPyCapsule(HPyTest):

    ExtensionTemplate = CapsuleTemplate

    def test_capsule_new(self):
        mod = self.make_module("""
            @DEFINE_SomeObject
            @DEFINE_Capsule_New
            @DEFINE_Capsule_GetName
            @DEFINE_Payload_Free

            @EXPORT(Capsule_New)
            @EXPORT(Capsule_GetName)
            @EXPORT(Payload_Free)

            @INIT
        """)
        p = mod.capsule_new(789, "Hello, World!")
        try:
            assert mod.capsule_getname(p) == "some_capsule"
        finally:
            # since HPy's capsule API does not allow a destructor, we need to
            # manually free the payload to avoid a memleak
            mod.payload_free(p)

    def test_capsule_getpointer(self):
        mod = self.make_module("""
            @DEFINE_SomeObject
            @DEFINE_Capsule_New
            @DEFINE_Capsule_GetPointer
            @DEFINE_Payload_Free

            @EXPORT(Capsule_New)
            @EXPORT(Capsule_GetPointer)
            @EXPORT(Payload_Free)

            @INIT
        """)
        ival = 789
        message = "Hello, World!"
        p = mod.capsule_new(ival, message)
        try:
            assert mod.capsule_getpointer(p) == (ival, message)
        finally:
            # since HPy's capsule API does not allow a destructor, we need to
            # manually free the payload to avoid a memleak
            mod.payload_free(p)

    def test_legacy_capsule_compat(self):
        import pytest
        mod = self.make_module("""
            #include <Python.h>

            static int dummy = 123;

            HPyDef_METH(Create_pycapsule, "create_pycapsule", create_pycapsule_impl, HPyFunc_O)
            static HPy create_pycapsule_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                HPy_ssize_t n;
                const char *name = HPyUnicode_AsUTF8AndSize(ctx, arg, &n);
                const char *name_copy = (const char *) malloc(n * sizeof(char));
                strncpy(name_copy, name, n);
                PyObject *legacy_caps = PyCapsule_New(&dummy, name_copy);
                HPy res = HPy_FromPyObject(ctx, legacy_caps);
                Py_DECREF(legacy_caps);
                return res;
            }

            HPyDef_METH(Capsule_get, "get", get_impl, HPyFunc_O)
            static HPy get_impl(HPyContext *ctx, HPy self, HPy arg)
            {
                const char *name = HPyCapsule_GetName(arg);
                HPy h_name = HPyUnicode_FromString(ctx, name);
                int *ptr = (int *) HPyCapsule_GetPointer(ctx, arg, name);
                HPy h_value = HPyLong_FromLong(ctx, *ptr);
                HPy res = HPyTuple_Pack(ctx, 2, h_name, h_value);
                HPy_Close(ctx, h_name);
                HPy_Close(ctx, h_value);
                return res;
            }

            @EXPORT(Create_pycapsule)
            @EXPORT(Capsule_get)

            @INIT
        """)
        name = "legacy_capsule"
        p = mod.create_pycapsule(name)
        assert mod.get(p) == (name, 123)

    def test_capsule_new_with_destructor(self):
        mod = self.make_module("""
            static void my_destructor(const char *name, void *pointer, void *context);

            @DEFINE_SomeObject
            @DEFINE_Capsule_New(my_destructor)
            @DEFINE_Capsule_GetName
            @DEFINE_Payload_Free

            static int pointer_freed = 0;

            static void my_destructor(const char *name, void *pointer, void *context)
            {
                free(pointer);
                pointer_freed = 1;
            }

            HPyDef_METH(Pointer_freed, "pointer_freed", pointer_freed_impl, HPyFunc_NOARGS)
            static HPy pointer_freed_impl(HPyContext *ctx, HPy self)
            {
                return HPyBool_FromLong(ctx, pointer_freed);
            }


            @EXPORT(Capsule_New)
            @EXPORT(Capsule_GetName)
            @EXPORT(Pointer_freed)

            @INIT
        """)
        p = mod.capsule_new(789, "Hello, World!")
        assert mod.capsule_getname(p) == "some_capsule"
        del p
        assert mod.pointer_freed()
