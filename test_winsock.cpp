#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        std::cerr << "Failed.\n";
        return 1;
    }
    std::cout << "Winsock2 initialized successfully!\n";
    WSACleanup();
    return 0;
}
