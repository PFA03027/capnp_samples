
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

#include <capnp/message.h>
#include <capnp/serialize-packed.h>

#include "gen/hello.capnp.h"

void write_hello(int fd)
{
    ::capnp::MallocMessageBuilder message;

    MsgHello::Builder hello_msg_builder = message.initRoot<MsgHello>();
    hello_msg_builder.setHello("Hello world\n");

    writePackedMessageToFd(fd, message);

    return;
}

void print_hello(int fd)
{
    ::capnp::PackedFdMessageReader message(fd);

    MsgHello::Reader hello_msg_reader = message.getRoot<MsgHello>();
    
    printf("%s", hello_msg_reader.getHello().cStr());

    return;
}

int main(void)
{
    int pipefd[2];
    int ret = pipe(pipefd);
    if(ret != 0) {
        fprintf( stderr, "fail pipe()");
        return EXIT_FAILURE;
    }

    write_hello(pipefd[1]);
    print_hello(pipefd[0]);

    close(pipefd[0]);
    close(pipefd[1]);

    return EXIT_SUCCESS;
}
