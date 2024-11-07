cmake_minimum_required(VERSION 3.22.1)
set(CMAKE_CXX_STANDARD 11)

project(P2-RC)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(Sources
    main.cpp
    Node.cpp
    Connection.cpp
    Utils.cpp
)

set(Headers
    Node.h
    Connection.h
    Utils.h
)

find_library(SCTP_LIB libsctp.so)
#find_library(THREAD_LIB libpthread.so)


add_executable(${PROJECT_NAME} ${Sources} ${Headers})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SCTP_LIB})
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
#target_link_libraries(${PROJECT_NAME} PRIVATE ${THREAD_LIB})
