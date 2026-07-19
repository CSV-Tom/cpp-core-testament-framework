cmake_minimum_required(VERSION 3.23)

project(CoreServicesConsumer LANGUAGES CXX)

find_package(CoreServices CONFIG REQUIRED)

add_executable(CoreServicesConsumer main.cpp)
target_link_libraries(CoreServicesConsumer PRIVATE CoreServices::CoreServices)

enable_testing()
add_test(NAME CoreServicesConsumerRuns COMMAND CoreServicesConsumer)
