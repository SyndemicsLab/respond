# RESPONDv2.0

`RESPONDv2.0` is a complete rewrite of the RESPOND model, first created by the [Syndemics Lab](https://www.syndemicslab.org) in the late 2010s, with the goals of improving the readability and maintainability of the model, improving the execution speed, and making the model calibration process easier.

While the original model was built using a combination of the R and C++ programming languages, the core of this rewrite is purely C++, instead planning to expose language bindings so users can work with the model using their language of choice.

## Dependencies

- [Boost](https://www.boost.org)
- [CMake](https://cmake.org) >= 3.19
- [Eigen](https://eigen.tuxfamily.org/index.php) >= 3.4.x
- [`fmt`](https://github.com/fmtlib/fmt) v9.0.0 or v9.1.0
- [`gcc`](https://gcc.gnu.org) v4.8.1 or newer
- [OpenMP](https://www.openmp.org)

### Tests

- [`gtest`](https://github.com/google/googletest) & `gmock`

### API

- [`Crow`](https://github.com/CrowCpp/Crow)

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

If on **Linux**:
1. Install conan2.0 via python and pip
2. Ensure > gcc 11 installed
3. Install cmake
4. Run the command:
```
$ ./debug-conanbuild.sh
$ src/respond inputfolder/ 1 2
```

If on **Windows**:
1. Run the command:
```
> debug-conanbuild.bat
> src/respond.exe inputfolder/ 1 2
```
