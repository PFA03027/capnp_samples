
all: big_msg.out

clean:
	-rm -fr big_msg.out big_msg_main.o  gen


big_msg.out : big_msg_main.o  gen/big_msg.capnp.o
	g++ -o $@ $^ -lcapnp -lkj -lpthread

big_msg_main.o : big_msg_main.cpp
big_msg_main.o : gen/big_msg.capnp.h

gen/big_msg.capnp.h gen/big_msg.capnp.c++ : big_msg.capnp
	mkdir -p gen
	capnp compile -oc++:gen $<

%.o : %.cpp
	g++ -o $@ -c $<

%.o : %.c++
	g++ -o $@ -c $<

