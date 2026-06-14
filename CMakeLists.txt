cmake_minimum_required(VERSION 3.10.0)
project(CHMIEmulator VERSION 0.1.0 LANGUAGES C CXX)

include_directories(.)
include_directories(Commands)

add_executable(CHMIEmulator 
    main.cpp
) 

 