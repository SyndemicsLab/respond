# Installation

The RESPOND model is available to users through multiple methods. That being said, RESPOND has been built and tested on machines running ubuntu 24.04 and CI/CD tested on Windows machines. We recommend a basic installation either through building the source code or utilizing CMake's Fetch Content feature. We do plan to eventually work towards providing a debian package install as well.

## Dependencies

RESPOND has several dependencies that require being satistifed before installation. This is primarily a concern if you intend to build from source as FetchContent should generally manage the dependencies if installed that way. Our `FetchContent_Declare` statements all attempt to `find_package` before downloading so if you choose to make use of specific versions of these dependencies you simply install the dependencies before attempting to install RESPOND.

**Note:** DataManagement is a Syndemics Lab library and under heavy development. It is *NOT* recommended to attempt to install a separate version.

- [DataManagement](https://github.com/SyndemicsLab/DataManagement)
- [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page)
- [Spdlog](https://github.com/gabime/spdlog)

## Installing From Source

In order to install from source, clone the repository and run cmake. We provide workflows in the `CMakePresets.json` and encourage the use of them for easy building. Our primary workflow is built using the [GCC compiler](https://gcc.gnu.org/) and the [Ninja build system](https://ninja-build.org/).

```bash
git clone https://github.com/SyndemicsLab/respond.git
cd respond
cmake --workflow gcc-release
```

We also provide a debug version that is slower, but prints more to the logger to help debug problems:

```bash
cmake --workflow gcc-debug
```

## Installing With CMake

Since CMake 3.11, the `FetchContent` tool has been available to users. Early on in the design process, we made the decision to progress under the assumption that CMake will handle our dependencies and additional package managers can be installed as necessary and included in toolchain files. To install via `FetchContent` simply do:

```cmake
include(FetchContent)
FetchContent_Declare(
    respond
    GIT_REPOSITORY https://github.com/SyndemicsLab/respond.git
    GIT_TAG main
)
option(RESPOND_INSTALL "Enable install for respond project" ON)
option(RESPOND_BUILD_TESTS "Disable testing for respond" OFF)
FetchContent_MakeAvailable(respond)
```

This should produce a corresponding `respondConfig.cmake` file for CMake linking and installation.

Previous: [Home](index.md)

Next: [Data](data.md)
