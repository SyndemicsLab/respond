# RESPOND

In 2018, the [Syndemics Lab](https://www.syndemicslab.org) created the <b>R</b>esearching <b>E</b>ffective <b>S</b>trategies to <b>P</b>revent <b>O</b>pioid <b>D</b>eath (RESPOND) simulation model as a tool through which populations in jurisdictions experiencing high-risk opioid use and their care cascades could be studied.

The structure of `RESPOND` can be broken down into the following pieces:

1. Population Dynamics
2. Opioid Use Disorder Progression
3. Care Delivery
4. Mortality

These parameter sets were designed and made via expert opinion and general consensus. Future work plans to experiment with the general structure accuracy.

[![CMake](https://github.com/SyndemicsLab/RESPONDSimulationv2/actions/workflows/cmake.yml/badge.svg)](https://github.com/SyndemicsLab/RESPONDSimulationv2/actions/workflows/cmake.yml)

## Implementation Details

The original implementation of `RESPOND` was written in a combination of two programming languages--R and C++. `RESPONDv2.0` is a complete rewrite of `RESPOND`, written purely in C++, with the goals of improving the readability and maintainability of the model, improving the execution speed, and making the model calibration process easier.

## Dependencies

<center>
| Library                                        | Minimum Version |
| ---------------------------------------------- | --------------- |
| [`gcc`](https://gcc.gnu.org)                   | 4.8.1           |
| [Boost](https://www.boost.org)                 | -               |
| [Eigen](https://eigen.tuxfamily.org/index.php) | 3.4             |
| [OpenMP](https://www.openmp.org)               | -               |
| [CMake](https://cmake.org)                     | 3.19            |
</center>

## Build Instructions

TODO

## What's New?

### Simulation Function Structure

One of the largest changes to the model, from a programming perspective, is the approach taken for representing the simulation population. In the original RESPOND model, this was done through a large, five-dimensional array that could not easily account for new population demographics/strata and required multiplication to be performed in serial.

This new implementation uses Eigen objects both to speed up the simulation through parallel computation of matrix multiplication and to make the programming more directly follow the underlying mathematical logic.

The State Tensor, known in the original as `cohort` (described above), is a three-dimensional array whose axes are

- Treatment/Intervention Status
- Opioid Use Disorder (OUD) Status
- Demographics (e.g. age, sex)

This enables flexibility in the number of combinations of intervention, opioid use, and demographics classifiers used in the model. The State Tensor is then wrapped in a vector to capture all timesteps.
