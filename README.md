# RESPOND: An Opioid Use Disorder State Transition Model

<a href="https://www.syndemicslab.org/respond"><img align="right" src="https://github.com/SyndemicsLab/.github/blob/main/profile/images/RESPOND.png" alt="RESPOND Logo" height="120" /></a>

[![Docs](https://github.com/SyndemicsLab/respond/actions/workflows/deploy-docs.yml/badge.svg)](https://github.com/SyndemicsLab/respond/actions/workflows/deploy-docs.yml)
[![Tests](https://github.com/SyndemicsLab/respond/actions/workflows/test-ubuntu.yml/badge.svg)](https://github.com/SyndemicsLab/respond/actions/workflows/test-ubuntu.yml)
[![Coverage](https://github.com/SyndemicsLab/respond/actions/workflows/coverage.yml/badge.svg)](https://github.com/SyndemicsLab/respond/actions/workflows/coverage.yml)

This is the home of the [RESPOND model](https://syndemicslab.github.io/respond)[1], first created by the [Syndemics Lab](https://www.syndemicslab.org) in 2018, now rewritten with a focus on four primary goals:

1. Improve the Maintainability/Scalability of the Model
2. Improve the Overall Efficiency of the Model
3. Improve the Portability
4. Improve testing

## RESPONDv1

The [original RESPOND model](https://github.com/SyndemicsLab/RESPONDv1/tree/main) was built using a combination of the R and C++ programming languages. This proved computationally slow, and it required a very skilled developer who actively remembered all of the details at the core of the software. As such, it proved incredibly difficult to onboard new engineers, analysts, and researchers to the model. Worse yet, in order to make changes to the model, all requirements inevitably filtered back to the software engineer. Following the software "inversion of control" paradigm, this refactoring focused on abstracting the model to its core components and allowing users to customize it to their needs rather than holding to a general rigid structure.

## Releases
As of **2025-11-17**, this repository is going through a refactor to transition from being a C++ library to a framework used through language bindings. Release [v0.3.0](https://github.com/SyndemicsLab/respond/releases/tag/v0.3.0) is the last version that includes the executable.

## CMake

RESPOND makes full use of the CMake build system. It is a common tool used throughout the C++ user-base and we utilize it for dependency management, linking, and testing. As C++ has poor package management, we intentionally decided to move our focus away from tools such as conan and vcpkg and stay with pure CMake. Not to say we would never publish with such package managers, but it is not a core focus of the refactor/engineering team.

We natively support 4 different build workflows with the `CMakePresets.json` file. They are:

1. `gcc-release`
2. `gcc-debug`
3. `gcc-release-cluster`
4. `gcc-debug-cluster`

Unless you are explicitly using a Linux based computing cluster, we highly recommend choosing one of the first two build processes. In the future, we do intend to expand to additional compilers and operating systems beyond GCC and Linux.

Overall, we make use of 11 custom CMake variables. They are found in the [options.cmake file](cmake/options.cmake) and all are set accordingly in the `CMakePresets.json`.

## Dependencies

We make abundant use of the CMake `FetchContent` feature released in CMake 3.11. We utilize features added in CMake 3.24 to check if the package is previously installed, so the minimum required version of CMake is **3.24**.

The required dependencies are:

- [DataManagement](https://github.com/SyndemicsLab/DataManagement)
- [Eigen](https://gitlab.com/libeigen/eigen)
- [spdlog](https://github.com/gabime/spdlog)

Building tests is optional, but when doing so it requires:
- [GoogleTest](https://github.com/google/googletest)

### Local

If you would like to clone and build this locally, it is a relatively straightforward process:

```shell
git clone https://github.com/SyndemicsLab/respond.git
cd respond
cmake --workflow --preset gcc-release
```

And then the model is build and installed. Our default location is a build directory in the repository, but the CMake Install Directory can be pointed to wherever the user desires.

### FetchContent

If you would like to make use of Fetch Content to extract the library:

```cmake
include(FetchContent)
FetchContent_Declare(
    respond
    GIT_REPOSITORY https://github.com/SyndemicsLab/respond.git
    GIT_TAG main
)
option(RESPOND_INSTALL "Enable install for respond project" ON)
option(RESPOND_BUILD_TESTS "Disable testing for RESPOND" OFF)
FetchContent_MakeAvailable(respond)
```

### Script

If you would prefer a single, all in one, script. Our team has developed a script that works on any Linux environment and finds packages wherever available. As such, the user need simply run:

```shell
./tools/build.sh
```

## References
1. Madushani RWMA, Wang J, Weitz M, Linas BP, White LF, Chrysanthopoulou SA (2025) Empirical calibration of a simulation model of opioid use disorder. PLoS ONE 20(3): e0310763. https://doi.org/10.1371/journal.pone.0310763
