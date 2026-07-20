FROM ubuntu:26.04

ENV DEBIAN_FRONTEND=noninteractive

# General tools, including the prerequisites for bootstrapping the Kitware
# apt repo below. g++-16 is the latest compiler available for Ubuntu 26.04
# LTS.
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        software-properties-common \
        lsb-release \
        ca-certificates \
        gpg \
        wget \
        curl \
        build-essential \
        ninja-build \
        g++-16 \
        clang \
        libclang-rt-dev \
        clang-format \
        clang-tidy \
        cppcheck \
        valgrind \
        gcovr \
        git \
        vim \
        tree \
        strace \
        gdb && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Bootstrap the Kitware apt repository and install cmake from it, since it's
# newer than the CMake version in Ubuntu's own archive.
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc | \
        gpg --dearmor -o /usr/share/keyrings/kitware-archive-keyring.gpg && \
    echo "deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main" \
        > /etc/apt/sources.list.d/kitware.list && \
    apt-get update && \
    apt-get install -y --no-install-recommends \
        kitware-archive-keyring \
        cmake && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# build-essential also pulls in Ubuntu's default gcc/g++; point the unversioned
# compiler names at g++-16 so local builds and the generic GCC
# toolchain use the selected compiler consistently.
RUN ln -sf /usr/bin/gcc-16 /usr/bin/gcc && \
    ln -sf /usr/bin/g++-16 /usr/bin/g++ && \
    ln -sf /usr/bin/gcc-16 /usr/bin/cc && \
    ln -sf /usr/bin/g++-16 /usr/bin/c++

ENV CC=/usr/bin/gcc-16 \
    CXX=/usr/bin/g++-16

RUN echo "Tool versions:" && \
    cmake --version && \
    g++-16 --version && \
    clang-format --version && \
    clang-tidy --version && \
    cppcheck --version && \
    valgrind --version && \
    git --version

CMD ["/bin/bash"]
