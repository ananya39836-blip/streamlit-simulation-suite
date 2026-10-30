#define _WIN32_WINNT 0x0600
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <algorithm>

// ============================================================================
// NATIVE MULTI-THREADED MODERN C++ WEB SERVER (WINSOCK2)
// Serves Question 1, 2, and 3 Treatises, 3D Simulators, & Mission Control
// Strictly Modern C++ | Zero-Hang Multi-Threaded Architecture
// ============================================================================

std::string getMimeType(const std::string& path) {
    if (path.rfind(".html") != std::string::npos) return "text/html; charset=utf-8";
    if (path.rfind(".json") != std::string::npos) return "application/json; charset=utf-8";
    if (path.rfind(".pdf")  != std::string::npos) return "application/pdf";
    if (path.rfind(".docx") != std::string::npos) return "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    if (path.rfind(".bmp")  != std::string::npos) return "image/bmp";
    if (path.rfind(".ply")  != std::string::npos) return "application/octet-stream";
    if (path.rfind(".css")  != std::string::npos) return "text/css";
    if (path.rfind(".js")   != std::string::npos) return "application/javascript";
    return "application/octet-stream";
}

std::vector<char> readFileBytes(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) return {};
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    file.read(buffer.data(), size);
    return buffer;
}

std::string urlDecode(const std::string& in) {
    std::string out;
    for (size_t i = 0; i < in.length(); ++i) {
        if (in[i] == '%' && i + 2 < in.length()) {
            int val = 0;
            std::istringstream iss(in.substr(i + 1, 2));
            if (iss >> std::hex >> val) {
                out += static_cast<char>(val);
                i += 2;
            } else {
                out += in[i];
            }
        } else if (in[i] == '+') {
            out += ' ';
        } else {
            out += in[i];
        }
    }
    return out;
}

void handleClient(SOCKET clientSocket, const std::string& workspace) {
    // Set socket timeout to prevent any hanging connections
    DWORD timeoutMs = 4000;
    setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeoutMs, sizeof(timeoutMs));
    setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeoutMs, sizeof(timeoutMs));

    char recvBuffer[4096];
    int bytesReceived = recv(clientSocket, recvBuffer, sizeof(recvBuffer) - 1, 0);
    if (bytesReceived <= 0) {
        closesocket(clientSocket);
        return;
    }
    recvBuffer[bytesReceived] = '\0';

    std::string request(recvBuffer);
    std::istringstream reqStream(request);
    std::string method, uri, protocol;
    reqStream >> method >> uri >> protocol;

    // Strip query parameters for static routing
    std::string cleanUri = uri;
    size_t qPos = cleanUri.find('?');
    if (qPos != std::string::npos) {
        cleanUri = cleanUri.substr(0, qPos);
    }

    std::cout << "[HTTP Server] " << method << " " << uri << std::endl;

    std::string filePath;
    std::string disposition = "";
    bool isJsonApi = false;

    if (cleanUri == "/" || cleanUri == "/index.html" || cleanUri == "/all-in-one" || cleanUri == "/app" || cleanUri.empty()) {
        filePath = workspace + "\\index.html";
    } else if (cleanUri == "/spacecraft" || cleanUri == "/space" || cleanUri == "/spacecraft_3d_simulation.html" || cleanUri == "/flight_deck") {
        filePath = workspace + "\\spacecraft_3d_simulation.html";
    } else if (cleanUri == "/freefire" || cleanUri == "/ff" || cleanUri == "/freefire_3d_simulation.html") {
        filePath = workspace + "\\freefire_3d_simulation.html";
    } else if (cleanUri == "/mission" || cleanUri == "/control" || cleanUri == "/mission_control.html") {
        filePath = workspace + "\\spacecraft_3d_simulation.html";
    } else if (cleanUri == "/lidar" || cleanUri == "/lidar.html" || cleanUri == "/lidar_studio.html") {
        filePath = workspace + "\\lidar_studio.html";
    } else if (cleanUri == "/nasa" || cleanUri == "/nasa.html" || cleanUri == "/nasa_mission_control.html") {
        filePath = workspace + "\\nasa_mission_control.html";
    } else if (cleanUri == "/tesseract" || cleanUri == "/tesseract_4d_lidar.html") {
        filePath = workspace + "\\tesseract_4d_lidar.html";
    } else if (cleanUri == "/api/treatises") {
        filePath = workspace + "\\treatises_catalog.json";
        isJsonApi = true;
    } else if (cleanUri == "/api/q2_treatises") {
        filePath = workspace + "\\q2_treatises_catalog.json";
        isJsonApi = true;
    } else if (cleanUri == "/api/q2_execution_results") {
        filePath = workspace + "\\q02_execution_results.json";
        isJsonApi = true;
    } else if (cleanUri == "/api/q3_treatises") {
        filePath = workspace + "\\q3_treatises_catalog.json";
        isJsonApi = true;
    } else if (cleanUri == "/api/q3_execution_results") {
        filePath = workspace + "\\q03_execution_results.json";
        isJsonApi = true;
    } else if (cleanUri == "/api/q4_treatises") {
        filePath = workspace + "\\q4_treatises_catalog.json";
        isJsonApi = true;
    } else if (cleanUri == "/api/q4_execution_results") {
        filePath = workspace + "\\q04_execution_results.json";
        isJsonApi = true;
    } else if (cleanUri.rfind("/download", 0) == 0 && uri.find("file=") != std::string::npos) {
        size_t fPos = uri.find("file=");
        std::string rawFilename = uri.substr(fPos + 5);
        size_t ampPos = rawFilename.find('&');
        if (ampPos != std::string::npos) rawFilename = rawFilename.substr(0, ampPos);
        std::string filename = urlDecode(rawFilename);
        if (filename.find("..") != std::string::npos || filename.find('/') != std::string::npos || filename.find('\\') != std::string::npos) {
            std::string badReq = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
            send(clientSocket, badReq.c_str(), badReq.length(), 0);
            closesocket(clientSocket);
            return;
        }
        filePath = workspace + "\\" + filename;
        disposition = "Content-Disposition: attachment; filename=\"" + filename + "\"\r\n";
    } else {
        std::string pathPart = cleanUri;
        if (!pathPart.empty() && pathPart[0] == '/') pathPart = pathPart.substr(1);
        filePath = workspace + "\\" + pathPart;
    }

    std::vector<char> body = readFileBytes(filePath);

    if (body.empty()) {
        std::string notFound = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: 13\r\nConnection: close\r\n\r\n404 Not Found";
        send(clientSocket, notFound.c_str(), notFound.length(), 0);
        closesocket(clientSocket);
        return;
    }

    std::ostringstream responseHeaders;
    responseHeaders << "HTTP/1.1 200 OK\r\n"
                    << "Content-Type: " << getMimeType(filePath) << "\r\n"
                    << "Content-Length: " << body.size() << "\r\n"
                    << "Access-Control-Allow-Origin: *\r\n"
                    << "Connection: close\r\n";
    if (!disposition.empty()) {
        responseHeaders << disposition;
    }
    responseHeaders << "\r\n";

    std::string headerStr = responseHeaders.str();
    send(clientSocket, headerStr.c_str(), headerStr.length(), 0);

    // Send body in chunks
    size_t totalSent = 0;
    while (totalSent < body.size()) {
        int sent = send(clientSocket, body.data() + totalSent, std::min<size_t>(body.size() - totalSent, 65536), 0);
        if (sent <= 0) break;
        totalSent += sent;
    }

    closesocket(clientSocket);
}

struct ClientParam {
    SOCKET sock;
    std::string workspace;
};

DWORD WINAPI ClientThreadProc(LPVOID lpParam) {
    ClientParam* p = reinterpret_cast<ClientParam*>(lpParam);
    if (p) {
        handleClient(p->sock, p->workspace);
        delete p;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    std::string workspace = "C:\\Users\\HP\\.gemini\\antigravity\\scratch\\reality_treatises";
    int port = 8080;

    std::cout << "========================================================================\n";
    std::cout << "   MULTI-THREADED C++ WEB SERVER (WINSOCK2 / HTTP 1.1) - PORT 8080      \n";
    std::cout << "========================================================================\n";

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        return 1;
    }

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "socket() failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    int opt = 1;
    setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "bind() failed on port " << port << ": " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "listen() failed: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    std::cout << ">> Reality Web Server successfully deployed and listening on:\n";
    std::cout << "   http://localhost:" << port << "/\n";
    std::cout << "   http://127.0.0.1:" << port << "/\n";
    std::cout << ">> Multi-threaded client handling active.\n";
    std::cout << "------------------------------------------------------------------------\n";

    while (true) {
        sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(listenSocket, (sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket != INVALID_SOCKET) {
            ClientParam* p = new ClientParam{ clientSocket, workspace };
            HANDLE hThread = CreateThread(NULL, 0, ClientThreadProc, p, 0, NULL);
            if (hThread) {
                CloseHandle(hThread);
            } else {
                handleClient(clientSocket, workspace);
                delete p;
            }
        }
    }

    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
