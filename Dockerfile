FROM ubuntu:24.04

# Install essential packages in one step
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        software-properties-common \
        lsb-release \
        build-essential \
        ca-certificates \
        gpg \
        wget \
        vim \
        ninja-build \
        tree && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Add Kitware repository
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc | \
        gpg --dearmor -o /usr/share/keyrings/kitware-archive-keyring.gpg && \
    echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ noble main' > /etc/apt/sources.list.d/kitware.list && \
    apt-get update && \
    apt-get install -y --no-install-recommends kitware-archive-keyring && \
    echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ noble-rc main' >> /etc/apt/sources.list.d/kitware.list && \
    apt-get update && \
    apt-get install -y --no-install-recommends cmake && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*
    
# Install CMake
RUN apt-get update && \
    apt-get install -y --no-install-recommends cmake && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Install G++
RUN apt-get update && \
    apt-get install -y --no-install-recommends g++-14 && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Install developer tools for formatting, analysis, performance measurement, and utilities
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        clang-format \
        clang-tidy \
        cppcheck \
        valgrind \
        git \
        curl \
        astyle \
        strace \
        gdb && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

RUN echo "Tool Versions:" && \
    cmake --version && \
    g++-14 --version && \
    clang-format --version && \
    clang-tidy --version && \
    cppcheck --version && \
    valgrind --version && \
    git --version && \
    astyle --version && \
    strace --version && \
    gdb --version
 
# Set compilers
ENV CC=/usr/bin/gcc-14 \
    CXX=/usr/bin/g++-14

# Optional: Uncomment to preload C++ system headers for modules
# RUN $CXX -std=c++23 -fmodules-ts -x c++-system-header iostream 
# RUN $CXX -std=c++23 -fmodules-ts -x c++-system-header string
# RUN $CXX -std=c++23 -fmodules-ts -x c++-system-header stdexcept
# RUN $CXX -std=c++23 -fmodules-ts -x c++-system-header functional
# RUN $CXX -std=c++23 -fmodules-ts -x c++-system-header vector

CMD ["/bin/bash"]
