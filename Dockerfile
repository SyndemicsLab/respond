FROM ubuntu:22.04 AS build

RUN apt-get update && apt-get -y --no-install-recommends install \
    build-essential \
    ca-certificates \
    gcc \
    gdb \
    cmake 

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
RUN make

# CMD [ "test/respondTest" ]
CMD [ "src/respond", "TestInput/input1", "1" ]