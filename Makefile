

.PHONY: all clean

all:
	@mkdir -p build && cd build && cmake .. && make

clean:
	@rm -rf build
	@rm -rf externals/*


setup:
	git submodule init
	git submodule update
	ln -s $(shell pwd) "$(HOME)/Documents/Max 8/Packages/$(shell basename `pwd`)"
