FROM ubuntu:22.04 AS build

RUN apt-get update && apt-get -y --no-install-recommends install \
    build-essential \
    ca-certificates \
    gcc \
    python3 \
    pip \
    git \
    cmake 

WORKDIR /respond

COPY . /respond/

RUN ./scripts/build.sh -at Release

ENTRYPOINT [ "./bin/api" ]

LABEL Name=respondsimulationv2 Version=0.0.1
