cmake_minimum_required(VERSION 3.28)
project(prj_evrp)

set(CMAKE_CXX_STANDARD 17)

add_executable(prj_evrp main.cpp
        Instance.cpp
        Instance.h
        Node.cpp
        Node.h
        Utils.cpp
        Utils.h
        Search.cpp
        Search.h
        instance_factory/Evrptwprpl.cpp
        instance_factory/Evrptwprpl.h
        instance_factory/Factory.cpp
        instance_factory/Factory.h
        instance_factory/Vrppl.cpp
        instance_factory/Vrppl.h
        Sequence.cpp
        Sequence.h
        Config.cpp
        Config.h
        Stats.cpp
        Stats.h
        Solution.cpp
        Solution.h)
