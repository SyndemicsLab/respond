# Installation

The RESPOND library can be installed via the executable packages built during the GitHub Actions workflow, building the Docker images, or by installing and building the source code. Our CMake is also designed to allow for usage with the `FetchContent` process for easy integration with any CMake projects. For library versions in different programming languages, please view the `respondpy` library.

## Dependencies

While we do handle our own dependency management, either via package installation or source building, we believe it is important to inform users of the external projects they will be installing alongside RESPOND. Our package has 2 core dependencies (`Eigen` and `spdlog`) and an additional dependency for the testing executable (`gtest`).

[Eigen](https://libeigen.gitlab.io/documentation/) is a widely used C++ linear algebra library. It allows for rapid development of matrix operations as well as converting matrices between GPU and CPU tensors. As RESPOND is a Markov model, we are able to provide speedups through linear algebra techniques and Eigen is the core allowing those speedups.

The [spdlog](https://github.com/gabime/spdlog) library is how we are able to write logs during runtime. We log various warnings and errors that could potentially arrive during runtime such as mathematical limitations of data and the assumptions we make if we are able to smooth over issues. By default, we attempt to resolve a few simple issues (i.e. if no data is found for transitions we assume everyone is retained in the same state) but this information is always logged to the output via the spdlog library.

## Installing From Packages

We distribute the executable installers through our tagged builds. They can be found [on our GitHub page](https://github.com/SyndemicsLab/respond/tags).

## Installing From Source

In order to install the library from source, clone the repository and run CMake. We provide workflows in the `CMakePresets.json` and encourage the use of them for easy building.

### Building without Packaging or Testing

```bash
git clone https://github.com/SyndemicsLab/respond.git
cd respond
cmake --preset test-debug
cmake --build --preset test-debug
```

### Testing

```bash
git clone https://github.com/SyndemicsLab/respond.git
cd respond
cmake --workflow --preset test-debug-shared
```

## Fetch Content

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

Previous: [Motivation](motivation.md)

Next: [Data](data.md)
