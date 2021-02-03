//
// Created by xiaohun on 2021/2/3.
//

#include <string>
#include <cstring>

#define  INIT_SIZE 10

class StringBuffer {
public:
    StringBuffer() : size(0) {
        this->buffer = (char *) malloc(INIT_SIZE);
        memset(this->buffer, 0, INIT_SIZE);

        //预留一个\0位
        this->capacity = INIT_SIZE;
    }

    ~StringBuffer() {
        free(this->buffer);
    }

    bool append(char *str, int size) {
        if (this->capacity - this->size < size) {
            realloc(this->buffer, (this->capacity + size) * 2);
            this->capacity = (this->capacity + size) * 2;
            memset(this->buffer + this->size, 0, this->capacity - this->size);
        }

        memcpy(this->buffer + this->size, str, size);
        this->size += size;
    }

    std::string toString() {
        return std::string(buffer);
    }

    char *getBuffer() {
        return this->buffer;
    }

private:
    char *buffer;
    int size;
    int capacity;
};