# RESPOND: An Opioid Use Disorder State Transition Model

<a href="https://www.syndemicslab.org/hep-ce"><img align="right" src="Respond_logo.png" alt="Respond Logo" height="120" /></a>

This repository houses a complete rewrite of the original [RESPOND model](https://journals.plos.org/plosone/article?id=10.1371/journal.pone.0310763), first created by the [Syndemics Lab](https://www.syndemicslab.org) in 2018 with a focus on 3 primary goals:

1. Improve the Maintainability/Scalability of the Model
2. Improve the Overall Efficiency of the Model
3. Improve the Portability

## RESPONDv1

The [original RESPOND model](https://github.com/SyndemicsLab/RESPONDv1/tree/main) was built using a combination of the R and C++ programming languages. This proved computationally slow and required a very skilled developer at the core of the software knowing all of the intricate details. As such, it proved incredibly difficult to onboard new engineers, analysts, and researchers to the model. Worse yet, in order to make changes to the model, all requirements inevitably filtered back to the software engineer. Following the software "inversion of control" paradigm, this refactoring focused on abstracting the model to it's core components and allowing users to customize it to their needs rather than a general rigid structure.

## CMake

RESPOND makes full use of the CMake build system. It is a common tool used throughout the C++ user-base and we utilize it for dependency management, linking, and testing. As C++ has poor package management, we intentionally decided to move our focus away from tools such as conan and vcpkg and stay with pure CMake. Not to say we would never publish with such package managers, but it is not a core focus of the refactor/engineering team.

We natively support 4 different build workflows with the `CMakePresets.json` file. They are:

1. `gcc-release`
2. `gcc-debug`
3. `gcc-release-cluster`
4. `gcc-debug-cluster`

Unless you are explicitly using a linux based computing cluster, we highly recommend choosing one of the first two build processes. In the future, we do intend to expand to additional compilers and operating systems beyond GCC and Linux.

Overall, we make use of 11 custom CMake variables. They are found in the [options.cmake file](cmake/options.cmake) and all are set accordingly in the `CMakePresets.json`.

## Dependencies

We make abundant use of the CMake `FetchContent` feature released in CMake 3.11. We utilize features added in CMake 3.24 to check if the package is previously installed, so the minimum required version of CMake is **3.24**.

The required dependencies are:

- [DataManagement](https://github.com/SyndemicsLab/DataManagement)
- [Eigen](https://gitlab.com/libeigen/eigen)
- [spdlog](https://github.com/gabime/spdlog)

For tests we require:

- [GoogleTest](https://github.com/google/googletest)

## Build

This is, by nature, a C++ library. This means that the default behavior is not to provide an executable for the model, but rather a set of callable functions to design your own model. However, we have a distinct use case for an executable and building a model, and as such we provide the ability to build and install this executable. This is directly controlled via the `RESPOND_BUILD_EXECUTABLE` variable.

### Local

If you would like to clone and build this locally, it is a relatively straightforward process:

```shell
git clone https://github.com/SyndemicsLab/RESPONDSimulationv2.git
cd RESPONDSimulationv2
cmake --workflow --preset gcc-release
```

And then the model is build and installed. Our default location is a build directory in the repository, but the CMake Install Directory can be pointed to wherever the user desires.

### Fetch Content

If you would like to make use of Fetch Content to extract the library:

```cmake
include(FetchContent)
FetchContent_Declare(
    respond
    GIT_REPOSITORY https://github.com/SyndemicsLab/RESPONDSimulationv2.git
    GIT_TAG main
)
option(RESPOND_INSTALL "Enable install for respond project" ON)
option(RESPOND_BUILD_TESTS "Disable testing for RESPOND" OFF)
FetchContent_MakeAvailable(respond)
```

### Script

If you would prefer a single, all in one, script. Our team has developed a script that works on any linux environment and finds packages wherever available. As such, the user needs simply run:

```shell
./tools/build.sh
```

## Running the RESPOND Executable

If you choose to use our provided executable, running the model requires a set of input files. The input files required are:

1. `all_types_overdose.csv`
2. `background_mortality.csv`
3. `block_init_effect.csv`
4. `block_trans.csv`
5. `entering_cohort.csv`
6. `fatal_overdose.csv`
7. `init_cohort.csv`
8. `oud_trans.csv`
9. `sim.conf`
10. `SMR.csv`

These are all put into a folder titled `input<number>` where the number is replaced with the ID of the input. Then, after these folders are created and RESPOND is built we simply run the command:

```bash
./build/extras/respond_exe <input_start> <input_end>
```

Next: [Installation](installation.md)
