# RESPONDv2.0

[![CMake](https://github.com/SyndemicsLab/RESPONDSimulationv2/actions/workflows/cmake.yml/badge.svg)](https://github.com/SyndemicsLab/RESPONDSimulationv2/actions/workflows/cmake.yml)

`RESPONDv2.0` is a complete rewrite of the RESPOND model, first created by the [Syndemics Lab](https://www.syndemicslab.org) in the late 2010s, with the goals of improving the readability and maintainability of the model, improving the execution speed, and making the model calibration process easier.

While the original model was built using a combination of the R and C++ programming languages, the core of this rewrite is purely C++, instead planning to expose language bindings so users can work with the model using their language of choice.

## Dependency Management

We utilize [Conan 2](https://docs.conan.io/2/) to build and manage dependencies. For a list of dependencies please check the [`Conanfile`](conanfile.py). Conan can be installed via python's package manager, `pip`. Once installed, running the [build script](`scripts/build.sh`) installs all dependencies and builds the executables.

## What's New?

### Simulation Function Structure

One of the largest changes to the model, from a programming perspective, is the approach taken for representing the simulation population. In the original RESPOND model, this was done through a large, five-dimensional array that could not easily account for new population demographics/strata and required multiplication to be performed in serial.

This new implementation uses Eigen objects both to speed up the simulation through parallel computation of matrix multiplication and to make the programming more directly follow the underlying mathematical logic.

The State Tensor, known in the original as `cohort` (described above), is a three-dimensional array whose axes are

- Treatment/Intervention Status
- Opioid Use Disorder (OUD) Status
- Demographics (e.g. age, sex)

This enables flexibility in the number of combinations of intervention, opioid use, and demographics classifiers used in the model. The State Tensor is then wrapped in a vector to capture all timesteps.

## Building RESPOND

If on **Unix** and building from source:

1. Clone the repository to your local machine.
2. Install the [DataManagement repository](https://github.com/SyndemicsLab/DataManagement/tree/main)
3. Once DataManagement is installed run the following script to build:

```bash
./scripts/build.sh
```

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
./bin/respond <input_start> <input_end>
```
