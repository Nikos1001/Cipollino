
#include "../common/common.h"
#include "../../libs/sockpp/include/sockpp/tcp_acceptor.h"
#include <thread>

void test(sockpp::tcp_socket sock) {
    printf("New connection!\n");
    uint64_t x[10] = {11, 22, 33};
    sock.write_n(x, sizeof(x));
    sock.write_n(x, sizeof(x));
    ssize_t n;
    char buf[512];
    while((n = sock.read(buf, sizeof(buf))) > 0) {
        buf[511] = '\0';
    }
    printf("Connection closed :(\n");
}

int main() {
    printf("Hello from the server!\n");

    sockpp::tcp_acceptor acc(2000);
    if(!acc) {
        errPrint(acc.last_error_str().c_str());
        exit(-1);
    }

    while(true) {
        sockpp::tcp_socket sock = acc.accept();
        std::thread t(test, std::move(sock));
        t.detach();
    }

    return 0;
}
