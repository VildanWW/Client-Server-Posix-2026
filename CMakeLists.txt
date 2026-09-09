cmake_minimum_required (VERSION 3.8)
project ("Client-Server-Posix")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_package(PostgreSQL REQUIRED)

add_executable(server_app 
    Server/main.cpp 
    Server/Server.cpp 
    Server/Server.h
    Server/UserSession.h
    Server/UserSession.cpp
    Server/DatabaseManager.h
    Server/DatabaseManager.cpp
)

target_include_directories(server_app PRIVATE ${PostgreSQL_INCLUDE_DIRS})

target_link_libraries(server_app PRIVATE Threads::Threads ${PostgreSQL_LIBRARIES})
