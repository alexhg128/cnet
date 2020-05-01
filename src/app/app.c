/**
 * Main app module
 */

#include "app.h"

#include <stdio.h>
#include "http/http.h"

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
    #include "../native/networking.h"
#endif

// MacOS
#ifdef __APPLE__
    #define MACOS 1
    #define LINUX 0
    #define WINDOWS 0
    #include "../native/networking.h"
#endif

void bootstrap_app() {
    start_socket(receive, not_available);
}