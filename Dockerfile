FROM ubuntu:22.04 AS build

RUN apt-get update && apt-get -y --no-install-recommends install \
    build-essential \
    ca-certificates \
    gcc \
    python3 \
    pip \
    cmake 

WORKDIR /respond

COPY src/ ./src/
COPY scripts/ ./scripts/
COPY lib/ ./lib/
COPY test/ ./test/

RUN ./scripts/build.sh -t Release

ENTRYPOINT [ "/bin/respond" ]

LABEL Name=respondsimulationv2 Version=0.0.1
