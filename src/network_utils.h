#pragma once
#include <vector>
#include <string>
#include <cstring>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <ifaddrs.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>
#endif

inline std::vector<std::string> get_local_ips() {
    std::vector<std::string> ips;

#ifdef _WIN32
    // Windows: getaddrinfo 사용
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        addrinfo hints, *res, *ptr;
        std::memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET; // IPv4
        hints.ai_socktype = SOCK_STREAM;

        if (getaddrinfo(hostname, nullptr, &hints, &res) == 0) {
            for (ptr = res; ptr != nullptr; ptr = ptr->ai_next) {
                char ip_str[INET_ADDRSTRLEN];
                sockaddr_in* ipv4 = (sockaddr_in*)ptr->ai_addr;
                if (inet_ntop(AF_INET, &(ipv4->sin_addr), ip_str, INET_ADDRSTRLEN)) {
                    ips.push_back(ip_str);
                }
            }
            freeaddrinfo(res);
        }
    }
#else
    // macOS / Linux: getifaddrs 사용
    struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == -1) return ips;

    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr || ifa->ifa_addr->sa_family != AF_INET) 
            continue;

        char ip_str[INET_ADDRSTRLEN];
        struct sockaddr_in* sa = (struct sockaddr_in*)ifa->ifa_addr;
        if (inet_ntop(AF_INET, &(sa->sin_addr), ip_str, INET_ADDRSTRLEN)) {
            std::string ip(ip_str);
            if (ip != "127.0.0.1") {
                ips.push_back(ip);
            }
        }
    }
    freeifaddrs(ifaddr);
#endif
    return ips;
}