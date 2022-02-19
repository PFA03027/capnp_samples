
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

#include <thread>

#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include <capnp/ez-rpc.h>
#include <kj/debug.h>

#include "gen/hello_if.capnp.h"

#define UNIX_DOMAIN_SOCKET_PATH "/tmp/capnp.hello2_main.socket"
#define UNIX_DOMAIN_SOCKET_PATH_ADDR "unix:" UNIX_DOMAIN_SOCKET_PATH

class AskHelloImpl final : public AskHello::Server
{
    // Implementation of the AskHello Cap'n Proto interface.
    // Cap'n ProtoのAskHello I/Fの実装クラス

public:
    // ask I/Fの呼び出しを受けるコールバック関数の実装
    kj::Promise<void> ask(AskContext context) override
    {
        // クライアント側からの呼び出し時の引数と、返り値を保持するクラスをこの関数の引数contextで受け取る

        // 呼び出し引数の内容を表示
        printf("server get a question: %s\n", context.getParams().getQuestion().cStr());

        context.getResults().setResponse("Hello world"); // 返り値をcontextに書き込む。

        return kj::READY_NOW;
    }
};

void ask_hello_server(void)
{
    // Set up a server.
    // Cap'n protoで定義したAskHello I/Fの実装型AskHelloImplを、rpcサーバーに渡し、サーバーを起動
    capnp::EzRpcServer server(kj::heap<AskHelloImpl>(), UNIX_DOMAIN_SOCKET_PATH_ADDR);

    // Write the port number to stdout, in case it was chosen automatically.
    // ポート番号を取得し、UNIXドメインソケット、あるいはTCP/IPによってprintfの内容を切り替える
    auto &waitScope = server.getWaitScope();
    uint port = server.getPort().wait(waitScope);
    if (port == 0)
    {
        // The address format "unix:/path/to/socket" opens a unix domain socket,
        // in which case the port will be zero.
        printf("Listening on Unix socket...\n");
    }
    else
    {
        printf("Listening on port %d...\n", port);
    }

    // Run forever, accepting connections and handling requests.
    // AskHelloImplの受信処理無限ループに入る。
    kj::NEVER_DONE.wait(waitScope);

    return;
}

void ask_hello_client(void)
{
    capnp::EzRpcClient client(UNIX_DOMAIN_SOCKET_PATH_ADDR);       // サーバーへ接続するベースclientを用意する。
    AskHello::Client askhello_client = client.getMain<AskHello>(); // ベースclientからAskHelloのI/F用clientを生成する。

    // Keep an eye on `waitScope`.
    // Whenever you see it used is a place where we stop and wait for the server to respond.
    // If a line of code does not use `waitScope`, then it does not block!
    // waitScopeを見てみてください。
    // これが使用されている場所では、一度止まってサーバーからレスポンスが戻ってくるのを待ちます。
    // waitScopeがない場所では、ブロックしません。
    auto &waitScope = client.getWaitScope();

    // Set up the request.
    // AskHelloのI/F用clientから、Cap'n protoで定義したask I/F用オブジェクトを生成する。
    auto request = askhello_client.askRequest();
    request.setQuestion("How are you ?"); // Cap'n protoで定義したask I/Fの引数を書き込む。

    // Send it, which returns a promise for the result (without blocking).
    // ask I/Fの要求をサーバーへ送信。戻り値として、promiseを受け取る。また、送信はブロックされません。
    auto askPromise = request.send();

    // Now that we've sent all the requests, wait for the response.
    // Until this point, we haven't waited at all!
    // この時点で、すべての要求を送り終わったので、レスポンスを待つ。
    // なお、この場所までは、いっさい待ち状態にならない。
    auto response = askPromise.wait(waitScope);         // サーバーからのレスポンスを受け取る。
    KJ_ASSERT(response.getResponse() == "Hello world"); // レスポンスが"Hello world"となっていることを検証

    printf("%s\n", response.getResponse().cStr());
    printf("PASS\n");

    return;
}

int main(void)
{
    unlink(UNIX_DOMAIN_SOCKET_PATH); // UNIXドメインソケットのopenエラーを避けるために、一旦削除

    std::thread server_thread(ask_hello_server); // サーバー側スレッドを起動
    sleep(1);                                    // サーバースレッドが起動し、UNIXドメインソケットの準備が終わるまで、待機する。

    std::thread client_threads;

    client_threads = std::thread(ask_hello_client); // クライアントスレッドを起動
    client_threads.join();                          // クライアントスレッドの終了同期

    if (server_thread.joinable())
    {
        server_thread.join(); // サーバースレッドの終了同期。。。ですが、今は戻ってきません。
    }

    return EXIT_SUCCESS;
}
