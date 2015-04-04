all:
	mkdir -p dll
	make -C src
clean:
	make -C src clean
