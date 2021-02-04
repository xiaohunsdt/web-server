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
#include <dirent.h>

#include "wrap.h"
#include "NetServer.h"
#include "utils.cpp"
#include "pub.h"

NetServer::NetServer() {
    new(this)NetServer("0.0.0.0", 8888);
}

NetServer::NetServer(const char *address, int port) : port(port) {
    memcpy(this->address, address, strlen(address));

    //更改工作路径
    char path[255] = {0};
    sprintf(path, "%s%s", getenv("HOME"), "/Desktop/web-server/webapp");
    chdir(path);
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
                strdecode(fileName, fileName);
                httpRequest->fileName = fileName + 1;
                httpRequest->protocal = protocal;
                break;
            }
        }
        token = strtok(NULL, "\n");
        index++;
    }

    if (httpRequest->fileName.empty() || httpRequest->fileName == "/" || httpRequest->fileName == "\\") {
        httpRequest->fileName = "index.html";
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
    HttpResponse *httpResponse;
    struct stat st;
    if (stat(httpRequest->fileName.c_str(), &st) < 0) {
        httpResponse = new HttpResponse(404, "Not Found");
    } else {
        httpResponse = new HttpResponse();
        if (S_ISREG(st.st_mode)) {
            httpResponse->setFile(httpRequest->fileName.c_str(), st.st_size);
        } else if (S_ISDIR(st.st_mode)) {
            struct dirent **namelist;
            StringBuffer stringBuffer;
            int n;
            n = scandir(httpRequest->fileName.c_str(), &namelist, nullptr, alphasort);
            if (n < 0) {
                perror("scandir");
            } else {
                char buffer[512];
                while (n--) {
                    memset(buffer, 0, sizeof(buffer));
                    if (namelist[n]->d_type == DT_DIR) {
                        sprintf(buffer, "<li><a href=%s/>%s</a></li>", namelist[n]->d_name, namelist[n]->d_name);
                    } else {
                        sprintf(buffer, "<li><a href=%s/%s>%s</a></li>", httpRequest->fileName.c_str(),
                                namelist[n]->d_name, namelist[n]->d_name);
                    }

                    stringBuffer.append(buffer, sizeof(buffer));
                    free(namelist[n]);
                }
                free(namelist);
                httpResponse->setContent(stringBuffer.getBuffer(), stringBuffer.getLength());
                httpResponse->fileType = get_mime_type(".html");
            }
        }
    }

    httpResponse->sendto(httpRequest->cfd);
}

void NetServer::start() {
    struct sigaction __action;
    sigemptyset(&__action.sa_mask);
    __action.sa_flags = 0;
    __action.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &__action, NULL);

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

