# Managing the Data

The RESPOND model is packaged as a library, but for ease of use we do also provide a basic command line interface executable for users to run the model. To this end, we make use of command line arguments and required files/directory structures. In addition to the baseline required files, we have additional optional files used to calculate both the cost effectivness measures and the utility (quality of life) measurements.

## Required Inputs

The required inputs are listed below:

- `sim.conf`
- `all_types_overdose.csv`
- `background_mortality.csv`
- `block_init_effect.csv`
- `block_trans.csv`
- `entering_cohort.csv`
- `fatal_overdose.csv`
- `init_cohort.csv`
- `oud_trans.csv`
- `SMR.csv`

### `sim.conf`

The `sim.conf` file forms the backbone of the RESPOND model. It governs the general flow and the basic pre-defined single digit parameters in the model. This file is meant to be changed by the user to determine how the model should act, it is not calibrated and does not contain tabular data. The general structure follows a prototypical `.ini` file with a header describing a set of key-value pairs. An example section of the `sim.conf` is provided below:

```text
[simulation]
duration                     = 208
aging_interval               = 260
intervention_change_times    = 52, 104, 156, 208
```

### `all_types_overdose.csv`

Hehe

## Optional Input Tables

- `bg_utility.csv`
- `healthcare_utilization_cost.csv`
- `oud_utility.csv`
- `overdose_cost.csv`
- `pharmaceutical_cost.csv`
- `setting_utility.csv`
- `treatment_utilization_cost.csv`

Previous: [Installation](installation.md)

Next: [Run](run.md)
