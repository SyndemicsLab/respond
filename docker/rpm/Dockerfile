FROM fedora:latest AS builder

RUN dnf update && \
    dnf install -y epel-release && \
    dnf install -y cmake gcc-c++ make ninja rpm-build

WORKDIR /respond
COPY . /respond

RUN mkdir build && \
    cmake --workflow --preset package-release-gcc-rpm-shared-workflow 

FROM fedora:latest
COPY --from=builder /respond/build/shared/*.deb .