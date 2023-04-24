FROM ubuntu:22.04 AS build

RUN apt-get update && apt-get -y --no-install-recommends install \
    build-essential \
    git \
    ca-certificates \
    gcc \
    clang \
    cmake \
    gdb \
    wget

RUN apt-get install -y libboost-all-dev \
    libgtest-dev \
    libeigen3-dev \
    libgmock-dev \
    libfmt-dev

WORKDIR /respond

COPY / /respond

WORKDIR /respond/build

RUN rm -rf *
RUN cmake -DCMAKE_BUILD_TYPE=Debug ..
RUN cmake ..
RUN make

CMD [ "test/respondTest" ]
# CMD [ "src/respond" ]