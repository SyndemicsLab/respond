# RESPONDv2.0

`RESPONDv2.0` is a complete rewrite of the RESPOND model, first created by the [Syndemics Lab](https://www.syndemicslab.org) in the late 2010s, with the goals of improving the readability and maintainability of the model, improving the execution speed, and making the model calibration process easier.

While the original model was built using a combination of the R and C++ programming languages, the core of this rewrite is purely C++, instead exposing language bindings so users can work with the model using their language of choice.

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

## What's New?

### Simulation Function Structure

One of the largest changes to the model, from a programming perspective, is the approach taken for representing the simulation population. In the original RESPOND model, this was done through a large, five-dimensional array that could not easily account for new population demographics/strata and required multiplication to be performed in serial.

This new implementation uses Eigen objects both to speed up the simulation through parallel computation of matrix multiplication and to make the programming more directly follow the underlying mathematical logic.

The State Tensor, known in the original as `cohort` (described above), is a three-dimensional array whose axes are

- Treatment/Intervention Status
- Opioid Use Disorder (OUD) Status
- Demographics (e.g. age, sex)

This enables flexibility in the number of combinations of intervention, opioid use, and demographics classifiers used in the model. The State Tensor is then wrapped in a vector to capture all timesteps.

### Terminology

Some specialized language is sometimes necessary to discuss the `RESPOND` model. In the course of its use, the terminology has become somewhat overlapping. We attempt to address some of this here:

<center>
| Original Term | New Term     | Definition                                                                                                                                 |
| ------------- | ------------ | ------------------------------------------------------------------------------------------------------------------------------------------ |
| Block         | Intervention | A sub-group of the population representing the group undergoing some treatment or lack thereof, i.e. "No Treatment", "Buprenorphine", etc. |
| Strategy      | Input Set    | A singular input folder representing a specific population of interest in a `RESPOND` study                                                |
</center>

## Mathematics

Overall, RESPOND operates as a compartmental deterministic model. As such, it can be explained by a discrete equation. To derive this equation we must break apart the simulation step process into individual operations and variables. Each variable is outlined in the table below.

$$
\begin{array}{ |l|l|l| }
\hline
 \textbf{Variable} & \textbf{Description} & \textbf{Shape or Size} \\\\
 \hline
 \mathbf{I} & \text{Intervention Vector} & N_I \\\\
 \hline
 \mathbf{U} & \text{OUD State Vector} & N_U \\\\
 \hline
 \mathbf{D} & \text{Combination Demographic Vector} & N_D \\\\
 \hline
 \mathbf{D_x} & \text{Individual Demographic Vector} & N_{D_x} \\\\
 \hline
 \mathit{T} & \text{Number of Treatments} & \frac{N_I - 1}{2} \\\\
 \hline
 \mathbf{S} &  \text{State Matrix} & N_I \times N_U \times N_D \\\\
 \hline
 \mathbf{E} & \text{Entering Sample Matrix} & N_I \times N_U \times N_D \\\\
 \hline
 \mathbf{Z_I} & \text{Intervention Transition Matrix} & N_I^2 \times N_U \times N_D \\\\
 \hline
 \mathbf{\Beta} & \text{Intervention Initialization Effect} & N_I \times N_U \times N_D \\\\
 \hline
 \mathbf{Z_U} & \text{OUD Transition Matrix} & N_I \times N_U^2 \times N_D \\\\
 \hline
 \mathbf{O} & \text{Overdose Chance Matrix} & N_I \times N_U \times N_D \\\\
 \hline
 \mathbf{M} & \text{Mortality Chance Matrix} & N_I \times N_U \times N_D \\\\
 \hline
 t & \text{Timestep} & \textrm{Scalar} \\\\
 \hline
 \Delta t & \text{Change in Time} & \textrm{Scalar} \\\\
 \hline
\end{array}
$$

Now, a single step is composed of 5 different operations:

1. Samples Enter the Simulation
2. OUD Transitions Occur
3. Intervention Transitions Occur
   1. Anything that changes intervention has the opportunity to change OUD again.
4. Overdoses Occur
5. Mortalities Occur

The Entering Samples can be modeled via:
$$
\begin{align}
    \mathbf{\textrm{Shape}(\Phi_e)} =  N_I \times N_U \times N_D \quad | \quad
    \mathbf{\Phi_e} = \mathbf{S} + \mathbf{E}
\end{align}
$$

OUD Transitions are modeled by:
$$
\begin{align}
    \mathbf{\textrm{Shape}(\Phi_a)} =  N_I \times N_U \times N_D \quad | \quad
    \mathbf{\Phi_a} &= \sum_{\alpha \in \mathbf{A}}{\mathbf{\Phi_e}_{\alpha} \cdot \mathbf{Z_I}_{\alpha:\alpha+N_I}}
\end{align}
$$

Intervention Transitions are modeled by:
$$
\begin{align}
    \mathbf{\textrm{Shape}(\Phi_u)} =  N_I \times N_U \times N_D \quad | \quad
    \mathbf{\Phi_u} &= \sum_{\beta \in \mathbf{U}}{\mathbf{\Phi_a}_{\beta} \cdot
    \mathbf{Z_U}_{\beta:\beta+N_U}}
\end{align}
$$

The values that change intervention have the opportunity to once again switch OUD states via the Intervention Initialization Effect Rate:
$$
\begin{align}
    \mathbf{\textrm{Shape}(\Beta)} = N_I \times N_U \times N_D \quad | \quad \mathbf{\Phi_u} = \Beta \cdot \Phi_u 
\end{align}
$$

Overdoses are modeled by:
$$
\begin{align}
    \mathbf{\textrm{Shape}(\Phi_o)} =  N_I \times N_U \times N_D \quad | \quad
    \mathbf{\Phi_o} = \mathbf{\Phi_u} \cdot \mathbf{O}
\end{align}
$$

Finally, mortality is modeled by:
$$
\begin{align}
    \mathbf{\textrm{Shape}(\Phi_m)} =  N_I \times N_U \times N_D \quad | \quad
    \mathbf{\Phi_m} = \mathbf{\Phi_u} \cdot \mathbf{M}
\end{align}
$$

<!-- Test -->
<!-- $$ -->
<!-- \begin{array}{ |l|l|l| } -->
<!-- \hline -->
<!--  \textbf{Section} & \textbf{Shape} & \textbf{Definition} \\\\ -->
<!--  \hline -->
<!--  Foo & Bar & Baz \\\\ -->
<!--  \hline -->
<!--  Foo & Bar & Baz \\\\ -->
<!--  \hline -->
<!--  Foo & Bar & Baz \\\\ -->
<!--  \hline -->
<!--  Foo & Bar & Baz \\\\ -->
<!--  \hline -->
<!-- \end{array} -->
<!-- $$ -->

Thus the new step becomes:
$$
\begin{align}
    \mathbf{S_{t+1}} = \mathbf{\Phi_u} - \mathbf{\Phi_m}
\end{align}
$$

Altogether this becomes:
$$
    \mathbf{S_{t+1}} = \Bigg(\Beta \cdot \sum_{\beta \in \mathbf{U}}
    {\Big(\sum_{\alpha \in \mathbf{A}}
    {(\mathbf{S_t} + \mathbf{E})_{\alpha} \cdot \mathbf{Z_I}_{\alpha:\alpha+N_I}}
    \Big)_{\beta} \cdot \mathbf{Z_U}_{\beta:\beta+N_U}}\Bigg) - \Bigg(\Big(\sum_{\beta \in \mathbf{U}}
    {\Big(\sum_{\alpha \in \mathbf{A}}
    {(\mathbf{S_t} + \mathbf{E})_{\alpha} \cdot \mathbf{Z_I}_{\alpha:\alpha+N_I}}
    \Big)_{\beta} \cdot
    \mathbf{Z_U}_{\beta:\beta+N_U}}\Big) \cdot \Beta \cdot \mathbf{M}\Bigg)
$$
