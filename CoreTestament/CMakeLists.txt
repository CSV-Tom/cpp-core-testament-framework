cmake_minimum_required(VERSION 3.31.2)

project(CoreTestamentFramework LANGUAGES CXX VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_library(${PROJECT_NAME} STATIC)

target_sources(
        ${PROJECT_NAME}
    PRIVATE
        src/core/Internal/InternalTest.cpp
        src/core/Internal/InternalSuite.cpp
        src/core/Internal/InternalRegistry.cpp        
        src/core/Runner.cpp
        src/core/Suite.cpp
        src/core/Test.cpp

        
    PUBLIC
FILE_SET HEADERS
        BASE_DIRS include
)



target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wshadow
	#-H
	#-s
	-O3
)

#add_subdirectory(tests)

add_executable(TestRunner)

target_sources(TestRunner
    PRIVATE
        tests/mainTest.cpp
)


target_compile_options(TestRunner PRIVATE
    -Wall
   # -fmodules-ts
)

target_link_libraries(TestRunner PRIVATE CoreTestamentFramework)

add_test(NAME AllTests COMMAND TestRunner)


# MyLibrary/
# ├── CMakeLists.txt           # Haupt-CMake-Konfigurationsdatei
# ├── include/                 # Öffentliche Header für die Bibliothek
# │   ├── MyLibrary/           # Namespace-Struktur
# │   │   ├── PublicHeader1.h  # Beispiel für einen öffentlichen Header
# │   │   ├── PublicHeader2.h  # Beispiel für einen öffentlichen Header
# │   │   └── Config.h         # Öffentliche Konfigurationsdatei
# ├── src/                     # Quellcode der Bibliothek
# │   ├── CMakeLists.txt       # CMake-Konfiguration für die Bibliothek
# │   ├── InternalModule1.cpp  # Interne Implementierung
# │   ├── InternalModule2.cpp  # Interne Implementierung
# │   └── SharedImplementation.cpp # Gemeinsame Implementierung
# ├── modules/                 # C++20/23-Module (optional)
# │   ├── PublicModule.ixx     # Exportiertes Modul
# │   ├── InternalModule.ixx   # Nicht exportiertes Modul
# ├── tests/                   # Testcode
# │   ├── CMakeLists.txt       # CMake-Konfiguration für Tests
# │   ├── UnitTest1.cpp        # Beispiel für einen Unit-Test
# │   └── UnitTest2.cpp        # Beispiel für einen weiteren Test
# └── examples/                # Beispielanwendungen
#     ├── Example1.cpp         # Erste Beispielanwendung
#     ├── Example2.cpp         # Zweite Beispielanwendung
#     └── CMakeLists.txt       # CMake-Konfiguration für Beispiele
