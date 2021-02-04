//
// Created by xiaohun on 2021/2/2.
//

#ifndef WEB_SERVER_HTTPRESPONSE_H
#define WEB_SERVER_HTTPRESPONSE_H

#include <string>

class HttpResponse {
public:
    HttpResponse();

    HttpResponse(int code, std::string msg);

    ~HttpResponse();

    void setFile(const char *fileName, int len);

    void setContent(const char *content, int len);

    void sendto(int cfd) const;

public:
    int code;
    std::string msg;
    std::string fileType;
    std::string fileName;
    char *content;
    int contentLength = 0;
};


#endif //WEB_SERVER_HTTPRESPONSE_H
