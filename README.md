# RESPONDSimulationv2

[![Ubuntu Build](https://github.com/SyndemicsLab/RESPONDSimulationv2/actions/workflows/build-and-test.yml/badge.svg)](https://github.com/SyndemicsLab/RESPONDSimulationv2/actions/workflows/build-and-test.yml)

[![Ubuntu Tests](https://github.com/SyndemicsLab/RESPONDSimulationv2/actions/workflows/build-and-test.yml/badge.svg)](https://github.com/SyndemicsLab/RESPONDSimulationv2/actions/workflows/build-and-test.yml)

[![Windows Build](https://github.com/SyndemicsLab/RESPONDSimulationv2/actions/workflows/build-and-test.yml/badge.svg)](https://github.com/SyndemicsLab/RESPONDSimulationv2/actions/workflows/build-and-test.yml)

[![Windows Tests](https://github.com/SyndemicsLab/RESPONDSimulationv2/actions/workflows/build-and-test.yml/badge.svg)](https://github.com/SyndemicsLab/RESPONDSimulationv2/actions/workflows/build-and-test.yml)

`RESPONDv2.0` is a complete rewrite of the RESPOND model, first created by the [Syndemics Lab](https://www.syndemicslab.org) in the late 2010s, with the goals of improving the readability and maintainability of the model, improving the execution speed, and making the model calibration process easier.

While the original model was built using a combination of the R and C++ programming languages, the core of this rewrite is purely C++ with various language bindings so users can work with the model using their language of choice.

## Dependency Management

Dependency management is done primarily through CMake files and the `FetchContent_Declare` methodology. If you wish to maintain local installations, core dependencies include:

- [DataManagement](https://github.com/SyndemicsLab/DataManagement)
- [Eigen](https://gitlab.com/libeigen/eigen)
- [spdlog](https://github.com/gabime/spdlog)

For python bindings we require:

- [Pybind11](https://pybind11.readthedocs.io/en/stable/basics.html)
- [Scikit-Build-Core](https://scikit-build-core.readthedocs.io/en/latest/)

For tests we require:

- [GoogleTest](https://github.com/google/googletest)

## Installation

To install the C++ executable (a list of cmake variable options can be found [here](cmake/options.cmake)):

```shell
git clone https://github.com/SyndemicsLab/RESPONDSimulationv2.git
cd RESPONDSimulationv2/build/
cmake .. -DCMAKE_BUILD_TYPE=Release -DRESPOND_BUILD_EXECUTABLE=ON -DRESPOND_BUILD_PIC=ON -DRESPOND_INSTALL=ON
cmake --build . --target install --config Release
```

To build the Python Bindings into a OS specific wheel (we are working on deploying/testing multi-platform):

```shell
git clone https://github.com/SyndemicsLab/RESPONDSimulationv2.git
cd RESPONDSimulationv2
pipx run build
```

and the wheel/distribution zip are placed in a directory called `dist/` found in the root of the project.

## What's New?

### Simulation Function Structure

One of the largest changes to the model, from a programming perspective, is the approach taken for representing the simulation population. In the original RESPOND model, this was done through a large, five-dimensional array that could not easily account for new population demographics/strata and required multiplication to be performed in serial.

This new implementation uses Eigen objects both to speed up the simulation through parallel computation of matrix multiplication and to make the programming more directly follow the underlying mathematical logic.

The State Tensor, known in the original as `cohort` (described above), is a three-dimensional array whose axes are

- Treatment/Intervention Status
- Opioid Use Disorder (OUD) Status
- Demographics (e.g. age, sex)

This enables flexibility in the number of combinations of intervention, opioid use, and demographics classifiers used in the model. The State Tensor is then wrapped in a vector to capture all timesteps.

## Running RESPOND

After building RESPOND, running the model requires a set of input files. The input files required are:

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
