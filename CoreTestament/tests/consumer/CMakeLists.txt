cmake_minimum_required(VERSION 3.23)

project(CoreTestamentConsumer LANGUAGES CXX)

find_package(CoreTestament CONFIG REQUIRED)

add_executable(CoreTestamentConsumer main.cpp)
target_link_libraries(CoreTestamentConsumer PRIVATE CoreTestament::CoreTestament)

enable_testing()
add_test(NAME CoreTestamentConsumerRuns COMMAND CoreTestamentConsumer)
