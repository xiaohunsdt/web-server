//
// Created by xiaohun on 2021/2/2.
//

#ifndef WEB_SERVER_HTTPRESPONSE_H
#define WEB_SERVER_HTTPRESPONSE_H
#include <string>

class HttpResponse {
public:
    void sendto(int cfd) const;
public:
    int code;
    std::string msg;
    std::string fileType;
    std::string fileName;
    int contentLength;
};


#endif //WEB_SERVER_HTTPRESPONSE_H
