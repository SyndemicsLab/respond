# Managing the Data

The RESPOND model is packaged as a library, but for ease of use we do also provide a basic command line interface executable for users to run the model. To this end, we make use of command line arguments and required files/directory structures. In addition to the baseline required files, we have additional optional files used to calculate both the cost effectiveness measures and the utility (quality of life) measurements.

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

### sim.conf

The `sim.conf` file forms the backbone of the RESPOND model. It governs the general flow and the basic pre-defined single digit parameters in the model. This file is meant to be changed by the user to determine how the model should act, it is not calibrated and does not contain tabular data. The general structure follows a prototypical `.ini` file with a header describing a set of key-value pairs. For RESPOND, there are 5 unique sets:

- simulation
- state
- demographic
- cost
- output

Each state governs a unique component of the model.

#### simulation

The simulation section governs the basic parameters of the model. An example is given below:

```ini
[simulation]
duration                     = 52           # The Duration of the Simulation
intervention_change_times    = 52           # The Timesteps to Expect Intervention Probability Changes
entering_sample_change_times = 52           # The Timesteps to Expect Entering Sample Count Changes
overdose_change_times        = 52           # The Timesteps to Expect Overdose Probability Changes
stratified_entering_cohort   = false        # Whether to Stratify the Entering Cohort by Behavior and Intervention
```

#### state

The state section governs the basic outline of the state matrix. The multiplication of the lengths of the values here gives us the size of the state space (i.e. 13 interventions and 4 behaviors = 13x4 = 52).

```ini
[state]
interventions = No_Treatment, Buprenorphine, Naltrexone, Methadone, Detox, Corrections, Residential, Post_Buprenorphine, Post_Naltrexone, Post_Methadone, Post_Detox, Post_Corrections, Post_Residential
behaviors     = Active_Noninjection, Active_Injection, Nonactive_Noninjection, Nonactive_Injection, Stimulants
```

#### demographic

This is a section that is being prepared for removal. As the model does not account for cohort demographics (we leave it to the provided data) this section should never change.

```ini
[demographic]
age_groups = 1_100 
race       = other 
sex        = other 
```

#### cost

This section governs the costing calculations as part of post-processing. The first parameter is always `cost_analysis` and flags whether or not to run cost calculations. By enabling costing, we know the model takes longer as all states must be recorded to calculate cost effectiveness at the given times. That being said, the example section:

```ini
[cost]
cost_analysis         = false               # Whether to calculate costs
cost_perspectives     = healthcare_system   # What perspective to use, must match with columns in the cost files
discount_rate         = 0.0025              # The discounting rate to apply to costs and utilities
reporting_interval    = 52                  # The interval length of which to calculate costs (i.e. 52 is a yearly reporting interval)
cost_category_outputs = false               # Whether to output costs in terms of their distinct categories
```

#### output

Finally, the output section determines what the general model outputs should record. Included in here are whether to write the inputs, whether to pivot the results to a long format array matching R's expected notation, and more.

```ini
[output]
per_intervention_predictions    = true      # Whether to provide outputs per intervention
general_outputs                 = true      # Whether to generate a "general" file containing key outcomes of interest
general_stats_output_timesteps  = 52        # What timesteps to save to the output file
write_calibrated_inputs         = false     # Whether to write the parameters used during this run
pivot_long                      = true      # Whether to pivot the table to long form as R expects (generally less human readable)
```

### Tabular Data

TODO

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
