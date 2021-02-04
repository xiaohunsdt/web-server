//
// Created by xiaohun on 2021/2/2.
//

#include "HttpResponse.h"
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <utility>
#include "pub.h"
#include "wrap.h"
#include "utils.cpp"

HttpResponse::HttpResponse() {
    new(this)HttpResponse(200, "OK");
}

HttpResponse::HttpResponse(int code, std::string msg) : code(code), msg(std::move(msg)) {
    if (code == 404) {
        struct stat st;
        stat("404.html", &st);
        this->setFile("404.html", st.st_size);
    }
}

HttpResponse::~HttpResponse() {
    if (this->contentLength != 0) {
        free(this->content);
    }
}

void HttpResponse::setFile(const char *fileName, int len) {
    this->fileName = fileName;
    this->fileType = get_mime_type(fileName);

    //将文件放入content中
    StringBuffer stringBuffer;
    int fd = open(this->fileName.c_str(), O_RDONLY);
    if (fd < 0) {
        perror("open error!");
        return;
    }

    char buf[1024] = {0};
    int n;
    while (1) {
        n = read(fd, buf, sizeof(buf));
        if (n <= 0) {
            break;
        }
        stringBuffer.append(buf, n);
    }
    close(fd);

    this->setContent(stringBuffer.getBuffer(), stringBuffer.getLength());
}

void HttpResponse::setContent(const char *content, int len) {
    this->contentLength = len;
    this->content = (char *) malloc(len + 1);
    memcpy(this->content, content, len);
    this->content[len] = 0;
}

void HttpResponse::sendto(int cfd) const {
    char buf[1024] = {0};
    sprintf(buf, "HTTP/1.1 %d %s \r\n", this->code, this->msg.c_str());
    sprintf(buf + strlen(buf), "Content-Type:%s\r\n", this->fileType.c_str());
    if (this->contentLength > 0) {
        sprintf(buf + strlen(buf), "Content-Length:%d\r\n", this->contentLength);
    }

    strcat(buf, "\r\n");
    Write(cfd, buf, strlen(buf));

    if (this->contentLength > 0) {
        Write(cfd, this->content, this->contentLength);
    }
}
