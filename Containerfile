FROM ubuntu:26.04

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        software-properties-common \
        lsb-release \
        ca-certificates \
        gpg \
        wget && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Add Kitware repository for latest CMake
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc | \
        gpg --dearmor -o /usr/share/keyrings/kitware-archive-keyring.gpg && \
    echo "deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main" \
        > /etc/apt/sources.list.d/kitware.list && \
    apt-get update && \
    apt-get install -y --no-install-recommends kitware-archive-keyring && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        ninja-build \
        g++-15 \
        clang-format \
        clang-tidy \
        cppcheck \
        valgrind \
        git \
        curl \
        wget \
        vim \
        tree \
        astyle \
        strace \
        gdb && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

ENV CC=/usr/bin/gcc-15 \
    CXX=/usr/bin/g++-15

RUN echo "Tool versions:" && \
    cmake --version && \
    g++-15 --version && \
    clang-format --version && \
    clang-tidy --version && \
    cppcheck --version && \
    valgrind --version && \
    git --version

CMD ["/bin/bash"]
