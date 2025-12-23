# Researching Effective Strategies to Prevent Opioid Use Disorder

<a href="https://www.syndemicslab.org/respond"><img align="right" src="Respond_logo.png" alt="Respond Logo" height="120" /></a>

This repository houses a complete rewrite of the original [RESPOND model](https://journals.plos.org/plosone/article?id=10.1371/journal.pone.0310763), first created by the [Syndemics Lab](https://www.syndemicslab.org) in 2018 with a focus on 3 primary goals:

1. Improve the maintainability and scalability of the model.
2. Improve the overall efficiency of the model, in both runtime and memory usage.
3. Allow for more data flexibility in the model.

## RESPONDv1

The [original RESPOND model](https://github.com/SyndemicsLab/RESPONDv1/tree/main) was built using a combination of the R and C++ programming languages. This proved computationally slow and required a very skilled developer at the core of the software knowing all of the intricate details. As such, it proved incredibly difficult to onboard new engineers, analysts, and researchers to the model. In addition, in order to make changes to the model, all requirements inevitably filtered back to the software engineer. Following the software "inversion of control" paradigm, this refactoring focused on abstracting the model to its core components and allowing users to customize it to their needs rather than a general rigid structure.

## CMake

RESPOND makes full use of the CMake build system. It is a common tool used throughout the C++ user-base and we utilize it for dependency management, linking, and testing. As C++ has poor package management, we intentionally decided to move our focus away from tools such as conan and vcpkg and stay with pure CMake. Not to say we would never publish with such package managers, but it is not a core focus of the refactor/engineering team.

Currently our CMake supports 10 different workflows. They are named using the following convention: `<exetype>-<versiontype>-<os>-<shared/static>-workflow`. Please consult the CMakePresets.json or run `cmake --workflow --list-presets` to see the entire list.

### Fetch Content

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

## Dependencies

As mentioned above, we utilize the  CMake `FetchContent` feature released in CMake 3.11. Additionally, we utilize features added in CMake 3.24 to check if the package is previously installed, so the minimum required version of CMake is **3.24**.

The required dependencies are:

- [Eigen](https://gitlab.com/libeigen/eigen)
- [spdlog](https://github.com/gabime/spdlog)

For tests we require:

- [GoogleTest](https://github.com/google/googletest)

## Install Packages

As part of our automated workflow, we provide several different installable packages as part of each release. They can be found on each [tagged release on our GitHub](https://github.com/SyndemicsLab/respond/tags). Currently we provide RPM and Debian installers for static and shared libraries, and a NSIS windows static library installer. For a step-by-step procedure of building the package from source, please check the [installation documentation](installation.md).

Next: [Motivation](motivation.md)
