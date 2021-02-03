//
// Created by xiaohun on 2021/2/2.
//

#include "HttpResponse.h"
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void HttpResponse::sendto(int cfd) const {
    char buf[1024] = {0};
    sprintf(buf, "HTTP/1.1 %d %s \r\n", this->code, this->msg.c_str());
    sprintf(buf + strlen(buf), "Content-Type:%s\r\n", this->fileType.c_str());
    if (this->contentLength > 0) {
        sprintf(buf + strlen(buf), "Content-Length:%d\r\n", this->contentLength);
    }

    strcat(buf, "\r\n");

    if (!this->fileName.empty()) {
        int fd = open(this->fileName.c_str(), O_RDONLY);
        if (fd < 0) {
            perror("open error!");
            return;
        }
        char buf[1024];
        int n;
        while (1) {
            n = read(fd, buf, sizeof(buf));
            if(n <= 0){
                break;
            }
            write(cfd,buf,n);
        }
    }


}
