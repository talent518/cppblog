CPUS := $(shell nproc)

all: ./build/Makefile
	@make -C build -j$(CPUS)

run: all
	@cd build && ./cppblog -c ../config.js

./build/Makefile: /usr/local/include/cppdb/backend.h /usr/local/include/cppcms/application.h
	@sudo apt-get install -y libgraphicsmagick++1-dev libmagick++-6.q16-dev libmagick++-dev discount libmarkdown2-dev
	@sh -c 'mkdir -p build;cd build && cmake ..'

/usr/local/include/cppcms/application.h:
	@make cppcms

cppcms: lib/cppcms/build/Makefile
	@make -C lib/cppcms/build -j$(CPUS) && sudo make -C lib/cppcms/build install

lib/cppcms/build/Makefile: lib/cppcms/build/
	@sudo apt-get install -y libpcre3-dev zlib1g-dev libgcrypt20-dev libicu-dev
	@sh -c 'cd lib/cppcms/build && cmake ..'

lib/cppcms/build/:
	@mkdir lib/cppcms/build

/usr/local/include/cppdb/backend.h:
	@make cppdb

cppdb: lib/cppdb/build/Makefile
	@make -C lib/cppdb/build -j$(CPUS) && sudo make -C lib/cppdb/build install

lib/cppdb/build/Makefile: lib/cppdb/build/
	@sudo apt-get install -y libmysqlclient-dev libsqlite3-dev
	@sh -c 'cd lib/cppdb/build && cmake ..'

lib/cppdb/build/:
	@mkdir lib/cppdb/build

clean:
	@rm -vrf build lib/cppcms/build lib/cppdb/build

clean-cppcms:
	@sudo rm -vrf /usr/local/include/booster /usr/local/include/cppcms /usr/local/bin/cppcms* /usr/local/lib/libcppcms*

clean-cppdb:
	@sudo rm -vrf rm -vrf /usr/local/include/cppdb /usr/local/lib/libcppdb*
