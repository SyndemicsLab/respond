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

RUN pip install conan

COPY ./src /respond/src
COPY conanfile.py /respond/conanfaile.py
WORKDIR /respond/

RUN if ([ ! -d build ]); then \
    cd .. \
    fi \ 
    if ([ -f lib/Matrixify.cpython-311-x86_64-linux-gnu.so ]); then \
    rm -rf lib/Matrixify.cpython-311-x86_64-linux-gnu.so \
    fi \
    if ([ -f lib/Simulation.cpython-311-x86_64-linux-gnu.so ]); then \
    rm -rf lib/Simulation.cpython-311-x86_64-linux-gnu.so \ 
    fi \
    ([[ -d build ]] && rm -rf build/*) || mkdir build \
    rm -rf build/* \
    rm -rf bin/* \
    conan install . --build=missing --settings=build_type=Release \
    cd build \
    source Release/generators/conanbuild.sh \
    cmake .. -DCMAKE_TOOLCHAIN_FILE=Release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DBUILD_TESTS=OFF -DBUILD_PYBINDINGS=ON \
    cmake --build . \
    source Release/generators/deactivate_conanbuild.sh 

ENTRYPOINT [ "/bin/respond" ]


# RUN apt-get install -y libasio-dev
# WORKDIR /respond/libs
# RUN git clone https://github.com/CrowCpp/Crow.git
# WORKDIR /respond/libs/Crow/build
# RUN cmake -DCROW_BUILD_EXAMPLES=OFF -DCROW_BUILD_TESTS=OFF ..
# RUN make install 

# WORKDIR /respond/build

# RUN rm -rf *
# RUN cmake -DCMAKE_BUILD_TYPE=Debug ..
# RUN make

# CMD [ "test/respondTest" ]
# CMD [ "src/respond", "TestInput/", "2" ]