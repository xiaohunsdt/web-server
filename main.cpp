#include <iostream>
#include <unistd.h>
#include <ctype.h>
#include "NetServer.h"

int main() {
    NetServer netServer;
    netServer.start();
    return 0;
}
