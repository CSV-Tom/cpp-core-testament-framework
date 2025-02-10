cmake_minimum_required(VERSION 3.31.2)

project(CoreServices LANGUAGES CXX VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_library(${PROJECT_NAME} STATIC)


target_sources(${PROJECT_NAME}
    PRIVATE 
        src/ServiceLocator.cpp
    PUBLIC FILE_SET HEADERS
        BASE_DIRS include
        PATTERN *.hpp 
)


target_include_directories(${PROJECT_NAME}
    PUBLIC include
    PRIVATE src
)


option(BUILD_TESTING "Enable tests" ON)

if(BUILD_TESTING)
    include(CTest)
    add_executable(Test${PROJECT_NAME})
    target_sources(Test${PROJECT_NAME} PRIVATE tests/main.cpp)
    target_link_libraries(Test${PROJECT_NAME} PRIVATE ${PROJECT_NAME})
    add_test(NAME CoreServicesTests COMMAND Test${PROJECT_NAME})
endif()
