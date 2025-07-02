#include "conn.h"

// Winsock 초기화
void ESP8266Client::initializeWinsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed!" << std::endl;
        exit(1);
    }
}

// Winsock 정리
void ESP8266Client::cleanupWinsock() {
    closesocket(sock);
    WSACleanup();
}

// 생성자: 소켓 초기화 및 서버 연결
ESP8266Client::ESP8266Client(const std::string& ip, int port) {
    initializeWinsock();

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed!" << std::endl;
        WSACleanup();
        exit(1);
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address / Address not supported" << std::endl;
        cleanupWinsock();
        exit(1);
    }

    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed! Error code: " << WSAGetLastError() << std::endl;
        cleanupWinsock();
        exit(1);
    }

    std::cout << "Connected to ESP8266 server at " << ip << ":" << port << std::endl;
}

// 소멸자: 소켓 정리
ESP8266Client::~ESP8266Client() {
    cleanupWinsock();
}

// 데이터 전송
bool ESP8266Client::sendData(const std::string& data) {
    int bytesSent = send(sock, data.c_str(), data.length(), 0);
    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "Send failed! Error code: " << WSAGetLastError() << std::endl;
        return false;
    }
    std::cout << "Sent: " << data << " (" << bytesSent << " bytes)" << std::endl;
    return true;
}

// 데이터 수신
std::string ESP8266Client::receiveData() {
    char buffer[1024];
    int bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';  // 문자열 끝 처리
        return std::string(buffer);
    }
    return "";
}
