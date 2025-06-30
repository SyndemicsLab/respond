# Limitations

Currently the RESPOND model and overall Simdemics Modeling Library are in heavy development. This means they have A LOT of current limitations. Any requests should be directed to the issues page on the github repository and the maintainers will work on them as permits. That being said, some readily identified limitations that have been identified are:

## Library

The code base started as a specific model and many of the library limitations are due to old code and notations being maintained. That being said, this is an area of continuous improvement so watch for updates:

- DataLoaders currently only support RESPOND
- The RESPOND model currently requires DataLoaders
- DataLoaders make use of the csv file structure
- Specific file names are required for DataLoaders
- Column names must be exact
- Behavior and intervention names must be underscored instead of spaces or dashes
- The model cannot be run on a GPU
- Windows hangs during unit tests
- There is no generalized linux build
- Installation requires adding the respond cmake folder path to the `CMAKE_PREFIX_PATH`

## Data

Currently, there are a lot of limitations in the required data structure.

- `sim.conf` timestep lists must contain the duration as the final value in the list
- All oud columns should be renamed to behaviors
- The Demographic structure still exists in the data without any impact to the model

Previous: [Under the Hold](math.md)

Next: [FAQs](faq.md)
