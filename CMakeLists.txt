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
        Sequence.h
        Config.cpp
        Config.h
        Stats.cpp
        Stats.h
        Solution.cpp
        Solution.h
        SearchDeltaCalc.cpp
        Route.cpp
        Route.h
        Counter.h
        SearchPersister.cpp
        SearchPropagator.cpp
        SearchValidator.cpp
        SeachLocalSearch.cpp
        SearchIG.cpp
)
