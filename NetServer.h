//
// Created by xiaohun on 2021/2/2.
//

#ifndef WEB_SERVER_NETSERVER_H
#define WEB_SERVER_NETSERVER_H
#include <ev++.h>
#include <memory>
#include "HttpRequest.h"
#include "HttpResponse.h"

class NetServer {
public:
    NetServer();

    NetServer(const char *address, int port = 8888) ;

    ~NetServer();

    void start();

    void accept_cb(ev::io &watcher, int revents);

    void serv_read_cb(ev::io &watcher, int revents);

    std::shared_ptr<HttpRequest> getHttpRequest(char *requestData);

    void serv_action(std::shared_ptr<HttpRequest> httpRequest);
private:
    char address[16] = { 0 };
    int port;
    ev::default_loop loop;
    ev::io accept_io;
    ev::io serv_io;
};


#endif //WEB_SERVER_NETSERVER_H
