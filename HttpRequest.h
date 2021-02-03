//
// Created by xiaohun on 2021/2/2.
//

#ifndef WEB_SERVER_HTTPREQUEST_H
#define WEB_SERVER_HTTPREQUEST_H
#include <string>

class HttpRequest {
public:
    int cfd;
    std::string requestType;
    std::string fileName;
    std::string protocal;
};


#endif //WEB_SERVER_HTTPREQUEST_H
