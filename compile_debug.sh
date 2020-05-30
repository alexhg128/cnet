#/bin/bash

gcc src/main.c src/app/http/http.c src/app/app.c src/native/linux/networking.c -pthread  -g -ggdb -o ./bin/Server