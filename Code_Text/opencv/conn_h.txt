#ifndef CONN_H
#define CONN_H

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")  // 윈속 라이브러리 링크

class ESP8266Client {
public:
    ESP8266Client(const std::string& ip, int port);
    ~ESP8266Client();

    bool sendData(const std::string& data);
    std::string receiveData();

private:
    void initializeWinsock();
    void cleanupWinsock();
    SOCKET sock;
};

#endif // ESP8266_CLIENT_H
