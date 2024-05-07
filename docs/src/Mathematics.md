# Mathematics
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
 \mathbf{\xi} & \text{Intervention Initialization Effect} & N_I \times N_U \times N_D \\\\
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
    \mathbf{\textrm{Shape}(\xi)} = N_I \times N_U \times N_D \quad | \quad \mathbf{\Phi_u} = \xi \cdot \Phi_{\textrm{u}}
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

Thus the new step becomes:
$$
\begin{align}
    \mathbf{S_{t+1}} = \mathbf{\Phi_u} - \mathbf{\Phi_m}
\end{align}
$$

Altogether this becomes:
$$
    \mathbf{S_{t+1}} = \Bigg(\xi \cdot \sum_{\beta \in \mathbf{U}}
    {\Big(\sum_{\alpha \in \mathbf{A}}
    {(\mathbf{S_t} + \mathbf{E})_{\alpha} \cdot \mathbf{Z_I}_{\alpha:\alpha+N_I}}
    \Big)_{\beta} \cdot \mathbf{Z_U}_{\beta:\beta+N_U}}\Bigg) - \Bigg(\Big(\sum_{\beta \in \mathbf{U}}
    {\Big(\sum_{\alpha \in \mathbf{A}}
    {(\mathbf{S_t} + \mathbf{E})_{\alpha} \cdot \mathbf{Z_I}_{\alpha:\alpha+N_I}}
    \Big)_{\beta} \cdot
    \mathbf{Z_U}_{\beta:\beta+N_U}}\Big) \cdot \xi \cdot \mathbf{M}\Bigg)
$$
