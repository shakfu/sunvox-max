

.PHONY: all build setup clean sign

all: clean build sign


build:
	@echo "building"
	@mkdir -p build && cd build && cmake -GXcode .. && cmake --build . --config Release

clean:
	@rm -rf build
	@rm -rf externals


setup:
	git submodule init
	git submodule update
	ln -s $(shell pwd) "$(HOME)/Documents/Max 9/Packages/$(shell basename `pwd`)"



sign:
	@echo "signing"
	@codesign -s "-" -f externals/sunvox\~.mxo/Contents/MacOS/sunvox~


