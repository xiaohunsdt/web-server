cmake_minimum_required(VERSION 3.17)
project(web_server)

set(CMAKE_CXX_STANDARD 14)

link_libraries(ev)

add_executable(web_server main.cpp pub.h pub.cpp log.h log.cpp
        NetServer.cpp NetServer.h
        wrap.h wrap.cpp HttpRequest.cpp HttpRequest.h
        HttpResponse.cpp HttpResponse.h utils.cpp
        )

target_link_libraries(web_server -lev)