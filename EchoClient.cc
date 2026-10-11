#include <iostream>
#include <string>

#include <strings.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


void Usage(const std::string &process)
{
    std::cerr << "Usage:\n\t";
    std::cerr << process << " server_ip server_port";
}

int main(int argc, char *args[])
{
    if (argc != 3) // 输入的合法性检验
    {
        Usage(args[0]);
        exit(1);
    }

    std::string server_ip = args[1];      // 从输入中接收服务端ip
    uint16_t server_port = atoi(args[2]); // 接收port

    // 建立通道
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "客户端 socket 创建通道失败" << std::endl;
        exit(2);
    }

    // 建结构体，填网络属性
    struct sockaddr_in server;
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(server_ip.c_str());
    server.sin_port = htons(server_port);

    // 启动
    while (true)
    {
        std::string msg;
        std::cout << "Please Enter# ";  // 获取用户输入
        std::getline(std::cin, msg);

        socklen_t len = sizeof(server);
        ssize_t n = sendto(sockfd, msg.c_str(), msg.size(), 0, (struct sockaddr *)&server, len);

        if (n > 0)
        {
            char inbuffer[1024];
            struct sockaddr_in tmp;
            socklen_t tmplen = sizeof(tmp);
            ssize_t m = recvfrom(sockfd, inbuffer, sizeof(inbuffer) - 1, 0, (struct sockaddr *)&tmp, &tmplen);
            if (m > 0)
            {
                inbuffer[m] = 0;
                std::cout << inbuffer << std::endl;
            }
        }
        else
        {
            std::cerr << "客户端 sendto error" << std::endl;
        }
    }

    return 0;
}