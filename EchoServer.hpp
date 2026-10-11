#pragma once

#include <iostream>
#include <functional>
#include <unistd.h>
#include "Logger.hpp"
#include <cstdint> // uint16_t

#include <strings.h> // bzero

#include <sys/socket.h> // socket、bind
#include <netinet/in.h> // struct sockaddr_in 结构体
#include <arpa/inet.h>  // htons、inet_addr

using namespace NS_LOG_MODULE;
const uint16_t defaultport = 8888;
using callback_t = std::function<std::string (const std::string&)>;

enum
{
    SUCCESS,
    SOCKET_ERROR,
    BIND_ERROR,
    USAGE_ERROR
};

class UdpServer
{
public:
    UdpServer(callback_t cb, uint16_t port = defaultport)
        : _socketfd(-1),
          _cb(cb),
          _port(port)
    {}

    void Init() // 初始化依旧先建立网络通道，通过socket函数打开网络文件
    {
        // 1. 创建socket
        // 2. 填网络属性
        // 3. bind，将 IP 和 端口 写进内核
        _socketfd = socket(AF_INET, SOCK_DGRAM, 0);

        if (_socketfd < 0)
        {
            LOG(LogLevel::FATAL) << "create socket error";
            exit(SOCKET_ERROR);
        }

        LOG(LogLevel::INFO) << "create socket success";

        struct sockaddr_in local;
        bzero(&local, sizeof(local));                   // 清零，防脏值
        local.sin_family = AF_INET;                     // 协议家族
        // local.sin_addr.s_addr = inet_addr(_ip.c_str()); // sin_addr是一个结构体，要用它的成员变量s_addr来接收
        local.sin_port = htons(_port);                  // 端口：主机序->网络序
        local.sin_addr.s_addr = INADDR_ANY;             // 最佳实践，采用任意ip绑定

        int ret = bind(_socketfd, (struct sockaddr *)&local, sizeof(local));
        if (ret < 0)
        {
            LOG(LogLevel::FATAL) << "bind error";
            exit(BIND_ERROR);
        }
        LOG(LogLevel::INFO) << "bind success";
    }

    void Start()
    {
        char inbuffer[1024];

        while (true)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            ssize_t n = recvfrom(_socketfd, inbuffer, sizeof(inbuffer) - 1, 0, (struct sockaddr*)&peer, &len); // 一般读取类的都是返回实际读到的字节数

            if (n >= 0)
            {
                inbuffer[n] = 0;
                LOG(LogLevel::DEBUG) << "["<< inet_ntoa(peer.sin_addr) << ":" << ntohs(peer.sin_port) << "] " << "client say# " << inbuffer;

                // std::string echo_string = "server echo# ";
                // echo_string += inbuffer; // 不做回显了
                
                // 网络层只调用函数处理业务
                std::string result = _cb(std::string(inbuffer)); // 这里显示调用构造
                sendto(_socketfd, result.c_str(), result.size(), 0, (struct sockaddr*)&peer, len); // 将接收到的信息发回去，也就是回显
            }
            else
            {
                LOG(LogLevel::ERROR) << "recvfrom error";
            }
        }
    }

    ~UdpServer()
    {
        close(_socketfd);
    }

private:
    int _socketfd;
    uint16_t _port;
    callback_t _cb;
};