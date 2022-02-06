

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

#include <future>

#include <capnp/message.h>
#include <capnp/serialize-packed.h>

#include "gen/big_msg.capnp.h"

#define TEST_DATA_SIZE  (10*1024*1024)
// #define TEST_DATA_SIZE  (60*1024)

int write_big_msg(int fd)
{
    ::capnp::MallocMessageBuilder message;

    BigDataMsg::Builder big_msg_builder = message.initRoot<BigDataMsg>();
    auto big_data_builder = big_msg_builder.initBigdata(TEST_DATA_SIZE);

    uint8_t count = 0;
    for( auto& e : big_data_builder) {
        e = count;
        count++;
    }

    writePackedMessageToFd(fd, message);

    return 1;
}

void print_big_msg(int fd)
{
    ::capnp::PackedFdMessageReader message(fd);

    BigDataMsg::Reader big_msg_reader = message.getRoot<BigDataMsg>();

    auto big_data_reader = big_msg_reader.getBigdata();
    
    if( big_data_reader.size() != TEST_DATA_SIZE) {
        fprintf( stderr, "data size mismatch error!\n");
        return;
    }
    uint8_t count = 0;
    for( auto& e : big_data_reader) {
        if( e != count ) {
            fprintf( stderr, "data mismatch error!\n");
            return;
        }
        count++;
    }

    printf("OK!\n");
    return;
}

int main(void)
{
    int pipefd[2];
    int ret = pipe(pipefd);
    if(ret != 0) {
        fprintf( stderr, "fail pipe()\n");
        return EXIT_FAILURE;
    }

    auto future_write = std::async(std::launch::async, write_big_msg, pipefd[1]);   // OK, even if big data
    // write_big_msg(pipefd[1]);    // NG, in case of big data

    print_big_msg(pipefd[0]);

    close(pipefd[0]);
    close(pipefd[1]);

    return EXIT_SUCCESS;
}
