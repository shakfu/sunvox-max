#cp ../../macos/lib_arm64/sunvox.dylib ./

INCLUDES=include
LIBDIRS=lib
CFLAGS="-m64"
LDFLAGS="-m64"
LIBS=-lstdc++ -lsunvox

NAME=test_static

compile =												\
	gcc -I$(INCLUDES) $(CFLAGS) -g -c $(1).c;           \
	gcc -L$(LIBDIRS) $(LDFLAGS) -o $(1) $(1).o $(LIBS);


TARGETS = test_static test1 test2 test3 test4 test5 test6 test7 test8

all:
	@$(foreach target,$(TARGETS),$(call compile,$(target)))


clean:
	@rm *.o
	@$(foreach target,$(TARGETS),rm -f $(target))
