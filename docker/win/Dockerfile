# Install MinGW-w64 (using MSYS2 for convenience)
# This is a multi-stage approach to keep the final image smaller
# Stage 1: Build the MSYS2 installer
FROM mcr.microsoft.com/windows/servercore:ltsc2019 AS msys2-builder
WORKDIR /msys2
COPY --from=mcr.microsoft.com/windows/nanoserver:ltsc2019 \
    C:/Windows/System32/msiexec.exe \
    C:/Windows/System32/msiexec.exe
RUN powershell -Command "Invoke-WebRequest -Uri 'https://repo.msys2.org/distrib/x86_64/msys2-x86_64-latest.exe' -OutFile 'msys2-installer.exe'"
RUN msiexec.exe /i msys2-installer.exe /qn

# Stage 2: Install MinGW and CMake in the main image
FROM mcr.microsoft.com/windows/servercore:ltsc2019 AS respond-builder
WORKDIR /respond
# Copy the MSYS2 installation from the builder stage
COPY --from=msys2-builder /msys2/msys64 /msys64

# Add MSYS2/MinGW to the PATH
ENV PATH="C:\\msys64\\usr\\bin;C:\\msys64\\mingw64\\bin;${PATH}"

# Install MinGW-w64 toolchain, CMake, and Ninja using pacman
RUN pacman -Syu --noconfirm
RUN pacman -S --noconfirm mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja

# Copy your C++ library source code into the container
COPY . /respond/

# Create a build directory
RUN mkdir build && \
    cmake --workflow --preset package-release-nsis-shared-workflow

FROM mcr.microsoft.com/windows/servercore:ltsc2019
COPY --from=respond-builder /respond/build/shared/*.exe .
