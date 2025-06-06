# FAQs

**Q: Where is the calibration procedure?**
> The calibration procedure is part of the `respondpy` library built and packaged as a separate library. That repository has a direct dependency on this codebase.

**Q: How do I add an intervention state?**
> Adding an intervention state requires 2 steps. Firstly, you must add the intervention to the `state.interventions` section in the `sim.conf`. The second step requires you add the data for the intervention to each file stratified by intervention (i.e. `all_types_overdose.csv`, `block_init_effect.csv`, `block_trans.csv`, `init_cohort.csv`, `oud_trans.csv`, and `SMR.csv`).

Previous: [Limitations](limitations.md)
