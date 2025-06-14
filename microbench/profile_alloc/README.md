# Profile allocation

```sh
pip install vmprof
sudo apt install libunwind-dev
python -m vmprof -o tmp_output.log profile_alloc.py
vmprofshow tmp_output.log tree
# for PyPy
pip install vmprof-firefox-converter@git+https://github.com/paugier/vmprof-firefox-converter@packaging
vmprofconvert -run profile_alloc.py cpy
# or
vmprofconvert -run profile_alloc.py
```

Example of (truncated) output (/path/to/pypy was /home/me/.local/share/uv/python/pypy-3.11.11-linux-x86_64-gnu):

```text
 97.4% |. main  97.4%  profile_alloc.py:9
  1.2% |.|. pypy_g_execute_frame  1.2%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  1.2% |.|.|. pypy_g_ll_portal_runner__Unsigned_Bool_pypy_interpreter  100.0%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  1.0% |.|.|.|. pypy_g_portal_12  86.7%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  1.0% |.|.|.|.|. pypy_g_handle_bytecode__AccessDirect_None  100.0%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  1.0% |.|.|.|.|.|. pypy_g_dispatch_bytecode__AccessDirect_None  100.0%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  1.0% |.|.|.|.|.|.|. pypy_g_opcode_impl_for_mul__AccessDirect_star_2  100.0%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  1.0% |.|.|.|.|.|.|.|. pypy_g_binop_mul_impl  100.0%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  1.0% |.|.|.|.|.|.|.|.|. pypy_g__call_binop_impl____mul_____rmul__  100.0%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  1.0% |.|.|.|.|.|.|.|.|.|. pypy_g_get_and_call_function__star_1  100.0%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  1.0% |.|.|.|.|.|.|.|.|.|.|. pypy_g_funccall__star_2  100.0%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  1.0% |.|.|.|.|.|.|.|.|.|.|.|. pypy_g_BuiltinCode2_fastcall_2.part.0  100.0%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  1.0% |.|.|.|.|.|.|.|.|.|.|.|.|. pypy_g_fastfunc_descr_mul_2_6  100.0%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  1.0% |.|.|.|.|.|.|.|.|.|.|.|.|.|. pypy_g_ObjectListStrategy_mul  100.0%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
[...]
 87.4% |.|. pypy_g__finish_create_instance  89.7%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
 49.1% |.|.|. pypy_g_IncrementalMiniMarkGC_malloc_fixed_or_varsize_no  56.2%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
 47.7% |.|.|.|. pypy_g_IncrementalMiniMarkGC_external_malloc  97.2%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  2.1% |.|.|.|.|. malloc  4.4%  /usr/lib/x86_64-linux-gnu/libc.so.6:0
 35.9% |.|.|.|.|. pypy_g_IncrementalMiniMarkGC_minor_collection_with_majo.part.0  75.3%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
 35.2% |.|.|.|.|.|. pypy_g_IncrementalMiniMarkGC_major_collection_step  97.9%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
 14.6% |.|.|.|.|.|.|. pypy_g_IncrementalMiniMarkGC_visit_all_objects_step  41.7%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  2.2% |.|.|.|.|.|.|.|. pypy_g_trace__gc_callback__collect_ref_rec  15.3%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
[...]
  4.2% |.|.|.|.|. pypy_g_ll_dict_setitem__DICTPtr_Address_Address  8.7%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
[...]
 34.5% |.|.|. pypy_g_W_TupleObjectUserDictWeakrefable__hpy_set_raw_st  39.5%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
 28.2% |.|.|.|. pypy_g_AbstractAttribute_write  81.9%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
 26.7% |.|.|.|.|. pypy_g_Terminator__write_terminator  94.5%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
 24.4% |.|.|.|.|.|. pypy_g__reorder_and_add__v512___simple_call__function__  91.6%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  8.1% |.|.|.|.|.|.|. pypy_g_W_HPyObjectUserDictWeakrefable__set_mapdict_incr  33.1%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  8.0% |.|.|.|.|.|.|.|. pypy_g_IncrementalMiniMarkGC_collect_and_reserve  99.0%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  8.0% |.|.|.|.|.|.|.|.|. pypy_g_IncrementalMiniMarkGC__minor_collection  100.0%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  4.8% |.|.|.|.|.|.|.|.|.|. pypy_g_IncrementalMiniMarkGC_collect_oldrefs_to_nursery  59.6%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  3.5% |.|.|.|.|.|.|.|.|.|.|. pypy_g_trace__gc_callback__trace_drag_out  72.6%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
[...]
  2.5% |.|.|.|.|.|.|.|.|.|. pypy_g_IncrementalMiniMarkGC_collect_cardrefs_to_nurser  31.7%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  1.7% |.|.|.|.|.|.|.|.|.|.|. pypy_g_trace_partial__gc_callback__trace_drag_out  66.7%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
[...]
 14.2% |.|.|.|.|.|.|. pypy_g_AbstractAttribute__find_branch_to_move_into  58.0%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
 11.3% |.|.|.|.|.|.|.|. pypy_g_IncrementalMiniMarkGC_collect_and_reserve  79.9%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
 11.3% |.|.|.|.|.|.|.|.|. pypy_g_IncrementalMiniMarkGC__minor_collection  100.0%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  8.2% |.|.|.|.|.|.|.|.|.|. pypy_g_IncrementalMiniMarkGC_collect_oldrefs_to_nursery  72.1%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  6.3% |.|.|.|.|.|.|.|.|.|.|. pypy_g_trace__gc_callback__trace_drag_out  77.4%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  1.2% |.|.|.|.|.|.|.|.|.|.|.|. pypy_g_ArenaCollection_malloc  18.3%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
[...]
  1.6% |.|.|.|.|.|.|.|.|.|. pypy_g_IncrementalMiniMarkGC_collect_cardrefs_to_nurser  14.3%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  1.1% |.|.|.|.|.|.|.|.|.|.|. pypy_g_trace_partial__gc_callback__trace_drag_out  66.7%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
[...]
  2.2% |.|.|.|.|.|.|.|. pypy_g_ll_dict_get__dicttablePtr_tuple2Ptr_GCREFPtr  15.2%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
[...]
  4.4% |.|.|.|. pypy_g_IncrementalMiniMarkGC_collect_and_reserve  12.8%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  4.4% |.|.|.|.|. pypy_g_IncrementalMiniMarkGC__minor_collection  100.0%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  3.4% |.|.|.|.|.|. pypy_g_IncrementalMiniMarkGC_collect_oldrefs_to_nursery  77.2%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  2.5% |.|.|.|.|.|.|. pypy_g_trace__gc_callback__trace_drag_out  75.0%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
[...]
  2.2% |.|.|. pypy_g_W_TupleObjectUserDictWeakrefable__hpy_get_raw_st  2.6%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
  1.5% |.|.|.|. pypy_g_AbstractAttribute_read  69.0%  /path/to/pypy/bin/libpypy3.11-c.so.debug:0
```
