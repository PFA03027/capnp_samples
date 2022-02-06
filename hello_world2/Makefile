
all: hello_world2.out

clean:
	-rm -fr hello_world2.out hello2_main.o gen

hello_world2.out : hello2_main.o  gen/hello_if.capnp.o
	g++ -o $@ $^ -lcapnp -lkj -lpthread -lcapnp-rpc -lkj-async

hello2_main.o : hello2_main.cpp
hello2_main.o : gen/hello_if.capnp.h

gen/hello_if.capnp.h gen/hello_if.capnp.c++ : hello_if.capnp
	mkdir -p gen
	capnp compile -oc++:gen $<

%.o : %.cpp
	g++ -o $@ -c $<

%.o : %.c++
	g++ -o $@ -c $<

