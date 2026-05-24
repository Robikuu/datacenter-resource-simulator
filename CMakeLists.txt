cmake_minimum_required(VERSION 4.1)
project(ProiectOOP)

set(CMAKE_CXX_STANDARD 20)

add_executable(ProiectOOP
    main.cpp
    utils.cpp
    IObject.cpp
    IP.cpp
    ResursaCloud.cpp
    Server.cpp
    ServerDedicat.cpp
    ServerVirtual.cpp
    MasinaVirtuala.cpp
    Datacenter.cpp
    Logger.cpp
    FabricaServer.cpp
    ClientCloud.cpp
)
