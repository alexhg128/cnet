#include <stdio.h>

#include "app/app.h"

// Windows conditional
#ifdef _WIN32
    #define WINDOWS 1
    #define LINUX 0
    #define MACOS 0
#endif

// Linux
#ifdef __linux
    #define LINUX 1
    #define MACOS 0
    #define WINDOWS 0
#endif

// MacOS
#ifdef __APPLE__
    #define MACOS 1
    #define LINUX 0
    #define WINDOWS 0
#endif

int main() {

    printf("=== CNET SERVER ===\n\n");

    if(WINDOWS) {
        printf("Windows is not currently supported");
        return 0;
    }

    bootstrap_app();
}