
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

#include <capnp/message.h>
#include <capnp/serialize-packed.h>

#include "gen/hello.capnp.h"

void write_hello(int fd)
{
    ::capnp::MallocMessageBuilder message; // 送信用メッセージを作るためのベースbuilderを用意

    MsgHello::Builder hello_msg_builder = message.initRoot<MsgHello>(); // capnpで定義したstruct MsgHello用のbuilderを生成
    hello_msg_builder.setHello("Hello world\n");                        // builderを使って、struct MsgHelloのメンバに値を設定

    writePackedMessageToFd(fd, message); // 作成したメッセージをfdへ送信

    return;
}

void print_hello(int fd)
{
    ::capnp::PackedFdMessageReader message(fd); // 受信メッセージを読みだすベースReaderを用意

    MsgHello::Reader hello_msg_reader = message.getRoot<MsgHello>(); // capnpで定義したstruct MsgHello用のを生成

    printf("%s", hello_msg_reader.getHello().cStr()); // struct MsgHelloのText型のメンバにC言語の文字列型のポインタを取得し、printfで出力

    return;
}

int main(void)
{
    int pipefd[2];
    int ret = pipe(pipefd); // パイプを作成
    if (ret != 0)
    {
        fprintf(stderr, "fail pipe()");
        return EXIT_FAILURE;
    }

    write_hello(pipefd[1]); // helloメッセージをパイプに書き込む
    print_hello(pipefd[0]); // helloメッセージをパイプから読み出し、printfで出力

    close(pipefd[0]); // 書き込み側のパイプをクローズ
    close(pipefd[1]); // 読み出し側のパイプをクローズ

    return EXIT_SUCCESS;
}
