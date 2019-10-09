CPUS := $(shell expr $(shell lscpu -b -eCPU | grep -v CPU | wc -l) '*' 2)

all: ./build/Makefile
	@make -C build -j$(CPUS)

run: all
	@cd build && ./cppblog -c ../config.js

./build/Makefile: build /usr/local/include/cppdb/backend.h /usr/local/include/cppcms/application.h
	@sudo apt-get install libgraphicsmagick++1-dev libmagick++-6.q16-dev libmagick++-dev discount libmarkdown2-dev
	@sh -c 'cd build && cmake ..'

build:
	@mkdir build

/usr/local/include/cppcms/application.h:
	@make cppcms

cppcms: lib/cppcms-1.2.1/build/Makefile
	@make -C lib/cppcms-1.2.1/build -j$(CPUS) && sudo make -C lib/cppcms-1.2.1/build install

lib/cppcms-1.2.1/build/Makefile: lib/cppcms-1.2.1/build/
	@sudo apt-get install cmake libpcre3-dev zlib1g-dev libgcrypt11-dev libicu-dev python
	@sh -c 'cd lib/cppcms-1.2.1/build && cmake ..'

lib/cppcms-1.2.1/build/: lib/cppcms-1.2.1/
	@mkdir lib/cppcms-1.2.1/build

lib/cppcms-1.2.1/:
	@tar -xvf lib/cppcms-1.2.1.tar.bz2 -C lib

/usr/local/include/cppdb/backend.h:
	@make cppdb

cppdb: lib/cppdb-0.3.1/build/Makefile
	@make -C lib/cppdb-0.3.1/build -j$(CPUS) && sudo make -C lib/cppdb-0.3.1/build install

lib/cppdb-0.3.1/build/Makefile: lib/cppdb-0.3.1/build/
	@sudo apt-get install libmysqlclient-dev libsqlite3-dev
	@sh -c 'cd lib/cppdb-0.3.1/build && cmake ..'

lib/cppdb-0.3.1/build/: lib/cppdb-0.3.1/
	@mkdir lib/cppdb-0.3.1/build

lib/cppdb-0.3.1/:
	@tar -xvf lib/cppdb-0.3.1.tar.bz2 -C lib

clean:
	@rm -vrf build lib/cppcms-1.2.1 lib/cppdb-0.3.1

clean-cppcms:
	@sudo rm -vrf /usr/local/include/booster /usr/local/include/cppcms /usr/local/bin/cppcms* /usr/local/lib/libcppcms*

clean-cppdb:
	@sudo rm -vrf rm -vrf /usr/local/include/cppdb /usr/local/lib/libcppdb*
