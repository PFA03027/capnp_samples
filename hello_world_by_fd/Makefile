
all: hello_world.out

clean:
	-rm -fr hello_world.out hello_main.o  gen


hello_world.out : hello_main.o  gen/hello.capnp.o
	g++ -o $@ $^ -lcapnp -lkj

hello_main.o : hello_main.cpp
hello_main.o : gen/hello.capnp.h

gen/hello.capnp.h gen/hello.capnp.c++ : hello.capnp
	mkdir -p gen
	capnp compile -oc++:gen $<

%.o : %.cpp
	g++ -o $@ -c $<

%.o : %.c++
	g++ -o $@ -c $<

