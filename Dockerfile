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


RUN apt-get update && \
    apt-get install -y \
        build-essential \
        wget \
        libgmp-dev \
        libmpfr-dev \
        libmpc-dev \
        flex \
        bison \
        texinfo \
        && \
    cd /tmp && \
    wget https://gcc.gnu.org/pub/gcc/snapshots/LATEST-15/gcc-15-20250112.tar.xz && \
    tar -xf gcc-15-20250112.tar.xz && \
    cd gcc-15-20250112 && \
    ./contrib/download_prerequisites && \
    mkdir build && \
    cd build && \
    ../configure --enable-languages=c,c++ --disable-multilib && \
    make -j$(nproc) && \
    make install && \
    cd / && \
    rm -rf /tmp/gcc-15-20250112 && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

ENV CC=/usr/local/bin/gcc \
    CXX=/usr/local/bin/g++



RUN apt-get update && \
    apt-get install -y wget gnupg software-properties-common && \
    wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | gpg --dearmor -o /usr/share/keyrings/llvm-archive-keyring.gpg && \
    echo "deb [signed-by=/usr/share/keyrings/llvm-archive-keyring.gpg] https://apt.llvm.org/lunar/ llvm-toolchain-lunar main" > /etc/apt/sources.list.d/llvm.list && \
    apt-get update && \
    apt-get install -y clang-16 clang-tools-16 lld-16 libc++-16-dev libc++abi-16-dev && \
    update-alternatives --install /usr/bin/clang clang /usr/bin/clang-16 100 && \
    update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-16 100 && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

ENV CC=/usr/bin/clang
ENV CXX=/usr/bin/clang++

RUN apt-get update && \
    apt-get install -y wget gnupg software-properties-common && \
    apt-get remove -y $(dpkg -l | grep clang- | awk '{print $2}' | grep -v clang-19) && \
    apt-get autoremove -y && \    
    wget https://apt.llvm.org/llvm.sh && \
    chmod +x llvm.sh && \
    ./llvm.sh 19 && \
    apt-get install -y clang-tools-19 clang-format-19 && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*


#ENV CC=/usr/bin/gcc-15 \
#    CXX=/usr/bin/g++-15

# Optional: Uncomment to preload C++ system headers for modules
# RUN $CXX -std=c++23 -fmodules-ts -x c++-system-header iostream 
# RUN $CXX -std=c++23 -fmodules-ts -x c++-system-header string
# RUN $CXX -std=c++23 -fmodules-ts -x c++-system-header stdexcept
# RUN $CXX -std=c++23 -fmodules-ts -x c++-system-header functional
# RUN $CXX -std=c++23 -fmodules-ts -x c++-system-header vector

CMD ["/bin/bash"]
