
all: hello_world3.out

clean:
	-rm -fr hello_world3.out hello3_main.o gen

hello_world3.out : hello3_main.o  gen/hello_if.capnp.o
	g++ -o $@ $^ -lcapnp -lkj -lpthread -lcapnp-rpc -lkj-async

hello3_main.o : hello3_main.cpp
hello3_main.o : gen/hello_if.capnp.h

gen/hello_if.capnp.h gen/hello_if.capnp.c++ : hello_if.capnp
	mkdir -p gen
	capnp compile -oc++:gen $<

%.o : %.cpp
	g++ -o $@ -c $<

%.o : %.c++
	g++ -o $@ -c $<

