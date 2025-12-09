FROM gcc:latest AS builder

RUN apt-get update && \
    apt-get install -y cmake && \
    apt-get install -y ninja-build && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /respond
COPY . /respond

RUN mkdir build && \
    cmake --workflow --preset package-release-gcc-debian-shared-workflow 

FROM debian:stable-slim
COPY --from=builder /respond/build/shared/*.deb .