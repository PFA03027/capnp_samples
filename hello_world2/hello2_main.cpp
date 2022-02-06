
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

#define NUM_OF_ASK 10000
#define NUM_OF_CLIENTS 100

class AskHelloImpl final : public AskHello::Server
{
    // Implementation of the Calculator Cap'n Proto interface.

public:
    kj::Promise<void> ask(AskContext context) override
    {
        // printf("server get a question: %s\n", context.getParams().getQuestion().cStr());

        context.getResults().setResponse("Hello world");

        return kj::READY_NOW;
    }
};

void ask_hello_server(void)
{
    // Set up a server.
    capnp::EzRpcServer server(kj::heap<AskHelloImpl>(), UNIX_DOMAIN_SOCKET_PATH_ADDR);

    // Write the port number to stdout, in case it was chosen automatically.
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
    kj::NEVER_DONE.wait(waitScope);

    return;
}

void ask_hello_client(void)
{
    capnp::EzRpcClient client(UNIX_DOMAIN_SOCKET_PATH_ADDR);
    AskHello::Client askhello_client = client.getMain<AskHello>();

    // Keep an eye on `waitScope`.  Whenever you see it used is a place where we
    // stop and wait for the server to respond.  If a line of code does not use
    // `waitScope`, then it does not block!
    auto &waitScope = client.getWaitScope();

    for (int i = 0; i < NUM_OF_ASK; i++)
    {

        // Set up the request.
        auto request = askhello_client.askRequest();
        request.setQuestion("How are you ?");

        // Send it, which returns a promise for the result (without blocking).
        auto askPromise = request.send();

        // Now that we've sent all the requests, wait for the response.  Until this
        // point, we haven't waited at all!
        auto response = askPromise.wait(waitScope);
        KJ_ASSERT(response.getResponse() == "Hello world");
    }

    printf("PASS\n");

    return;
}

int main(void)
{
    unlink(UNIX_DOMAIN_SOCKET_PATH);

    std::thread server_thread(ask_hello_server);
    sleep(1); // should wait unitil server start completion

    std::thread client_threads[NUM_OF_CLIENTS];

    for (int i = 0; i < NUM_OF_CLIENTS; i++)
    {
        client_threads[i] = std::thread(ask_hello_client);
    }

    for (int i = 0; i < NUM_OF_CLIENTS; i++)
    {
        if (client_threads[i].joinable())
        {
            client_threads[i].join();
        }
    }

    if (server_thread.joinable())
    {
        server_thread.join();
    }

    return EXIT_SUCCESS;
}
