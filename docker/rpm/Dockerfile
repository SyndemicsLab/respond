FROM fedora:latest AS builder

RUN dnf update -y
RUN dnf install -y cmake gcc-c++ make ninja rpm-build git

WORKDIR /respond
COPY . /respond

RUN mkdir build && \
    cmake --workflow --preset package-release-gcc-rpm-shared-workflow 

FROM fedora:latest
COPY --from=builder /respond/build/shared/*.rpm .