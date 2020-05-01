# C Lightweight Web Server

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

## Running

```
./bin/Server
```

## Testing

```
curl -X GET -D - localhost:8080
```