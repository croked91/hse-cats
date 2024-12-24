cmake_minimum_required(VERSION 3.20)

project(cats LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic")

set(SOURCES
    src/addr_info_wrapper.cpp
    src/collage_creator.cpp
    src/download.cpp
    src/socket_wrapper.cpp
    src/thread_pool.cpp
    src/thread_safe_flag.cpp
    src/thread_safe_map.cpp
    src/utils.cpp
    src/upload.cpp
    main.cpp
)

set(HEADERS
    include/addr_info_wrapper.h
    include/collage_creator.h
    include/download.h
    include/socket_wrapper.h
    include/thread_pool.h
    include/thread_safe_flag.h
    include/thread_safe_map.h
    include/utils.h
    include/upload.h
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ..)

include_directories(include)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE pthread ${OpenCV_LIBS})
