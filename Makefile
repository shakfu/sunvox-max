

.PHONY: all build setup clean sign

all: clean build sign


build:
	@echo "building"
	@mkdir -p build && cd build && cmake .. && make

clean:
	@rm -rf build
	@rm -rf externals/*


setup:
	git submodule init
	git submodule update
	ln -s $(shell pwd) "$(HOME)/Documents/Max 8/Packages/$(shell basename `pwd`)"



sign:
	@echo "signing"
	@codesign -s "-" -f externals/sunvox\~.mxo/Contents/MacOS/sunvox~
	@codesign -s "-" -f externals/sunvox\~.mxo/Contents/Frameworks/libsunvox.dylib


