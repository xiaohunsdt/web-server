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
        this->capacity = INIT_SIZE;
    }

    ~StringBuffer() {
        free(this->buffer);
    }

    bool append(char *str, int size) {
        if (this->capacity - this->size < size) {
            this->buffer = (char *)realloc(this->buffer, (this->capacity + size) * 2);
            this->capacity = (this->capacity + size) * 2;
            memset(this->buffer + this->size, 0, this->capacity - this->size);
        }

        memcpy(this->buffer + this->size, str, size);
        this->size += size;
    }

    std::string toString() const{
        return std::string(buffer);
    }

    char *getBuffer() const{
        return this->buffer;
    }

    int getLength() const{
        return this->size;
    }

private:
    char *buffer;
    int size;
    int capacity;
};