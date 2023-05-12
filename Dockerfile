FROM ubuntu:22.04 AS build

RUN apt-get update && apt-get -y --no-install-recommends install \
    build-essential \
    ca-certificates \
    gcc \
    git \
    gdb \
    cmake 

RUN apt-get install -y libboost-all-dev \
    libgtest-dev \
    libeigen3-dev \
    libgmock-dev \
    libfmt-dev

WORKDIR /respond

COPY / /respond

RUN apt-get install -y libasio-dev
WORKDIR /respond/libs
RUN git clone https://github.com/CrowCpp/Crow.git
WORKDIR /respond/libs/Crow/build
RUN cmake -DCROW_BUILD_EXAMPLES=OFF -DCROW_BUILD_TESTS=OFF ..
RUN make install 

WORKDIR /respond/build

RUN rm -rf *
RUN cmake -DCMAKE_BUILD_TYPE=Debug ..
RUN make

# CMD [ "test/respondTest" ]
CMD [ "src/respond", "TestInput/input1", "1" ]