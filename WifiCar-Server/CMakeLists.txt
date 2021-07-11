cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -lwiringPi -lwiringPiDev -lpthread -llirc_client")
project(wifi)
add_executable(wifi pca9685.c basicDrive.c ultra.c wifi.c)
