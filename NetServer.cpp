//
// Created by xiaohun on 2021/2/2.
//
#include <iostream>
#include <memory>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include "wrap.h"
#include "NetServer.h"
#include "utils.cpp"

NetServer::NetServer() {
    new(this)NetServer("0.0.0.0", 8888);
}

NetServer::NetServer(const char *address, int port) : port(port) {
    memcpy(this->address, address, strlen(address));
}

NetServer::~NetServer() {

}

std::shared_ptr<HttpRequest> NetServer::getHttpRequest(char *requestData) {
    assert(strlen(requestData) > 0);
//    std::cout << requestData << std::endl;
    auto httpRequest = std::make_shared<HttpRequest>();

    int index = 1;
    char *token = strtok(requestData, "\n");
    while (token) {
//        std::cout << token << std::endl;
        switch (index) {
            case 1 : {
                char requestType[16] = {0};
                char fileName[255] = {0};
                char protocal[16] = {0};
                sscanf(token, "%[^ ] %[^ ] %[^\r]", requestType, fileName, protocal);
//                std::cout << requestType << "--" << fileName << "--" << protocal << std::endl;
                httpRequest->requestType = requestType;
                httpRequest->fileName = fileName;
                httpRequest->protocal = protocal;
                break;
            }
        }
        token = strtok(NULL, "\n");
        index++;
    }

    return httpRequest;
}

void NetServer::accept_cb(ev::io &watcher, int revents) {
    int cfd = Accept4(watcher.fd, NULL, NULL, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (cfd < 0)
        std::cout << "invaild cfd!!" << std::endl;

    serv_io.set<NetServer, &NetServer::serv_read_cb>(this);
    serv_io.start(cfd, ev::READ);
}

void NetServer::serv_read_cb(ev::io &watcher, int revents) {
    StringBuffer stringBuffer;
    char buffer[10];
    int nready;
    while (1) {
        nready = Read(watcher.fd, buffer, sizeof(buffer));
        if (nready == -1) {
            break;
        } else if (nready == 0) {
            close(watcher.fd);
            watcher.stop();
            return;
        } else if (nready < -1) {
            std::cout << strerror(errno) << std::endl;
            this->loop.unloop(ev::ALL);
            return;
        }

        // 已读完缓存数据,添加到数据区
        stringBuffer.append(buffer, nready);
    }

    auto httpRequest = this->getHttpRequest(stringBuffer.getBuffer());
    httpRequest->cfd = watcher.fd;
//    std::cout << httpRequest->requestType << "--" << httpRequest->fileName << "--" << httpRequest->protocal<< std::endl;
    this->serv_action(httpRequest);
}

void NetServer::serv_action(std::shared_ptr<HttpRequest> httpRequest) {
    struct stat st;
    HttpResponse *httpResponse;
    if (stat(httpRequest->fileName.c_str(), &st) < 0) {
        httpResponse = new HttpResponse(404,"NOT FOUND");
    }
}

void NetServer::start() {
    int lfd = Socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    int reuse = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    struct sockaddr_in sockaddrIn = {0};
    inet_pton(AF_INET, this->address, &sockaddrIn.sin_addr.s_addr);
    sockaddrIn.sin_port = htons(port);
    Bind(lfd, (struct sockaddr *) &sockaddrIn, sizeof(sockaddrIn));

    Listen(lfd, 128);

    accept_io.set<NetServer, &NetServer::accept_cb>(this);
    accept_io.start(lfd, ev::READ);
//    this->loop.run(EVRUN_NOWAIT);
    this->loop.run();
}

