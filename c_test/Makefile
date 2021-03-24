CC = gcc
INCLUDE=-I.. -I../hpy/devel/include -I../hpy/debug/src/include
CFLAGS = -O0 -UNDEBUG -g -Wall -Werror -Wfatal-errors $(INCLUDE) -DHPY_UNIVERSAL_ABI

test: test_debug_handles
	./test_debug_handles

test_debug_handles: test_debug_handles.o ../hpy/debug/src/dhqueue.o
	$(CC) -o $@ $^

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

