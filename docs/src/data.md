# Managing the Data

The RESPOND model is packaged as a library, but for ease of use we do also provide a basic command line interface executable for users to run the model. To this end, we make use of command line arguments and required files/directory structures. In addition to the baseline required files, we have additional optional files used to calculate both the cost effectiveness measures and the utility (quality of life) measurements.

## Required Inputs

The required inputs are listed below:

- `sim.conf`
- `inputs.db`

### sim.conf

The `sim.conf` file forms the backbone of the RESPOND model. It governs the general flow and the basic pre-defined single digit parameters in the model. This file is meant to be changed by the user to determine how the model should act, it is not calibrated and does not contain tabular data. The general structure follows a prototypical `.ini` file with a header describing a set of key-value pairs. For RESPOND, there are 5 unique sets:

- simulation
- state
- demographic
- cost
- output

Each set governs a unique component of the model. An example can be found in the [examples folder](https://github.com/SyndemicsLab/respond/blob/main/extras/examples/sim.conf).

#### simulation

The simulation section governs the basic parameters of the model. An example is given below:

```ini
[simulation]
duration                     = 52           # The duration of the simulation
intervention_change_times    = 52           # The timesteps to expect intervention probability changes
entering_sample_change_times = 52           # The timesteps to expect entering sample count changes
overdose_change_times        = 52           # The timesteps to expect overdose probability changes
stratified_entering_cohort   = false        # Whether to stratify the entering cohort by behavior and intervention
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
cost_analysis         = false                       # Whether to calculate costs
cost_perspectives     = healthcare_system, societal # What perspective to use, must match with columns in the cost files
discount_rate         = 0.0025                      # The discounting rate to apply to costs and utilities
reporting_interval    = 52                          # The interval length of which to calculate costs (i.e. 52 is a yearly reporting interval)
cost_category_outputs = false                       # Whether to output costs in terms of their distinct categories
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

The tabular data is now stored in a SQLite database titled `inputs.db`. The database schema can be created via the file `create_db.sql` found alongside the `sim.conf` in the [examples folder of the repository]((https://github.com/SyndemicsLab/respond/blob/main/extras/examples)). The schema consists of 13 tables:

- background_mortality
- behavior
- behavior_transition
- cohort
- cohort_demographics
- demographics
- initial_population
- intervention
- intervention_transition
- overdose
- overdose_fatality
- population_change
- smr

## Cost Effectiveness Inputs

After running the model, we have post processing operations that can be applied to the final state to calculate cost effectiveness metrics. These have traditionally been governed by the following CSVs:

- `bg_utility.csv`
- `healthcare_utilization_cost.csv`
- `oud_utility.csv`
- `overdose_cost.csv`
- `pharmaceutical_cost.csv`
- `setting_utility.csv`
- `treatment_utilization_cost.csv`

### File Breakdown

Below are a breakdown of each file:

#### healthcare_utilization_cost.csv

```ini
block
agegrp
race
sex
oud
healthcare_system
```

#### overdose_cost.csv

```ini
X
healthcare_system
```

#### pharmaceutical_cost.csv

```ini
block
healthcare_system
```

#### setting_utility.csv

```ini
block
utility
```

#### treatment_utilization_cost.csv

```ini
block
healthcare_system
```

Previous: [Installation](installation.md)

Next: [Run](run.md)
