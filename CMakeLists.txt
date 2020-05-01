cmake_minimum_required(VERSION 2.8.12)
set(CMAKE_C_STANDARD 11)
SET(CMAKE_C_FLAGS -pthread)
project(Server)
find_package(Threads REQUIRED)

# Linux and MacOS
if(UNIX)
    ADD_EXECUTABLE(
        bin/Server
        src/main.c
        src/app/app.c
        src/app/http/http.c
        
        src/native/linux/networking.c
    )

    ADD_EXECUTABLE(
        bin/Client
        src/client.c

        src/native/linux/networking.c
    )
endif()

# Windows
if(WIN32)

endif()