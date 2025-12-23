# Under the Hood

The RESPOND model is built on the idea that we can model opioid use disorder as a state transition model. We also maintain the Markov properties such that the RESPOND model is a completely Markovian model. Represented mathematically, this means that the next step depends exclusively on the current conditions and the transition probabilities as shown in equation 1.

$$
x_{t+1} = M_{t}^{T}x_{t}
$$

When we assume $x_{t}$ is able to represent the state of the cohort at timestep $t$, this means that the model is concerned with representing the transition matrix $M$. RESPOND represents this transition matrix as a series of sequential operations. They are:

1. Entering Cohort Transitions
2. Intervention Transitions
3. Behavior Transitions
4. Overdose Transitions
5. Background Mortality Transitions

**Note:** A large deviation from v1, RESPOND now considers the "Entering Cohort" and "Death" counts as states in the state matrix. This is required in order to allow for the entering cohort transition to function as part of the transition matrix and not exist as an extra component added into equation 1.

## Entering Cohort Transitions

Entering cohort transitions is the first step in the transition operation. Its actually extremely straightforward, it is a direct movement of people from the "entering cohort" state into the no treatment state.

$x = e(x, t)$

## Intervention Transitions

$x = \Iota(x, t)$

## Behavior Transitions

$x = \Beta(x, t)$

## Overdose Transitions

$x = \Omega(x, t)$

## Background Mortality Transitions

$x = \Mu(x, t)$

Previous: [Run](run.md)

Next: [Limitations](limitations.md)
