# RESPOND v2.0
RESPOND v2.0 is a re-write of the original RESPOND model first proposed by our lab several years ago. While successful, it was generally slow and not pheasible to expand upon. Therefore, in order to make the codebase more modular and generally speed up the model, we have decided to refactor the project into RESPOND v2.0.

### Environment Dependencies
- C++11
- GTest
- GMock
- Boost
- Eigen >= 3.4.x
- OpenMP
- CMake >= 3.19

### Inputs
Previously, RESPOND managed all inputs in a massive 5 dimensional array. This meant for any changes in parameters we had to completely restructure the initial array. In addition, the array did element wise multiplication in serial slowing down the simulation substantially. 

This is being changed so that the input matrix is simply 3 dimensions

- x: Treatment State
- y: OUD State
- z: Demographic Vector

This allows for flexibility in the number of treatment states, OUD states, and demographic combinations. In addition, since there is no transition between demographics, we can run the z dimension in parallel across the entire simulation. 

