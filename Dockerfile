FROM gcc:latest

RUN apt-get update && \
    apt-get install -y cmake && \
    apt-get install -y ninja-build && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /respond
COPY . /respond

RUN mkdir build && \
    cmake --preset debug-gcc-linux-static-config && \
    cmake --workflow --preset test-debug-gcc-linux-static-workflow