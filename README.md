# C NET Web Server

A lightweight C Web Server by Alejandro Hahn

## Utilities needed

### CMake

```
sudo yum install cmake
```

### GCC/G++

```
sudo yum -y install gcc
sudo yum -y install gcc-c++
```

## Before compiling

```
mkdir bin

cmake .
```

## Compiling

```
make
```

## Compiling in debug mode

```
chmod +x ./compile_debug.sh
./compile_debug.sh
```

## Running

```
./bin/Server
```

## Testing

```
curl -X GET -D - localhost:8080
curl -X GET -D - localhost:8080/TC2025.pdf
curl -X GET -D - localhost:8080/TC2025.html
```