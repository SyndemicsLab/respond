# Running the Model

The RESPOND model can be used in multiple ways:

1. **As a C++ library** - Integrate into your own C++ projects
2. **Via Python package** - Use the higher-level [respondpy](https://github.com/SyndemicsLab/respondpy) interface
3. **Standalone executable** - Legacy support for direct command-line execution

## Using RESPOND as a C++ Library

For C++ developers, RESPOND can be integrated directly into projects. See the [C++ API Guide](api-guide.md) for:

- Complete API reference
- Usage examples
- Memory management details
- Design patterns and best practices

### Basic Example

```cpp
#include <respond/simulation.hpp>
#include <respond/model.hpp>

int main() {
    // Create a simulation
    respond::Simulation sim("my_logger");
    
    // Create and configure a model
    auto model = respond::Model::Create("population_model", "my_logger");
    
    // Set initial state
    Eigen::VectorXd initial_state(50);
    initial_state.setZero();
    initial_state(0) = 1000;  // 1000 individuals in state 0
    model->SetState(initial_state);
    
    // Add transitions
    auto transition = respond::TransitionFactory::CreateTransition(
        "behavior", "my_logger");
    model->AddTransition(transition);
    
    // Add model to simulation
    sim.AddModel(model);
    
    // Run simulation for 52 timesteps
    for (int t = 0; t < 52; ++t) {
        sim.Run();
    }
    
    // Extract results
    auto histories = sim.GetModelHistories();
    
    return 0;
}
```

For complete examples and API documentation, see the [C++ API Guide](api-guide.md).

## Using the Standalone Executable

Legacy support for standalone execution is available in release [v0.3.0](https://github.com/SyndemicsLab/respond/releases/tag/v0.3.0).

### Command-Line Usage

To run the executable (requires legacy release):

```bash
./respond_exe /path/to/input/folders 1 1
```

### Arguments

The executable takes 3 positional arguments:

1. **Input folder path**: Directory containing simulation configuration and data
2. **Start folder index**: First input folder number (e.g., 1 for `input1`)
3. **End folder index**: Last input folder number (inclusive)

### Examples

```bash
# Run single input folder
./respond_exe /home/user/data 1 1

# Run folders input1, input2, input3
./respond_exe /home/user/data 1 3

# Run folders input5 through input10
./respond_exe /home/user/data 5 10
```

### Output

The executable produces output files in the same directory as the input data.

## Python Integration

For a higher-level interface, use the [respondpy](https://pypi.org/project/respondpy/) Python package:

```python
import respondpy as respond

# Configure and run simulations
sim = respond.Simulation(config_path)
results = sim.run()
```

See the [respondpy documentation](https://github.com/SyndemicsLab/respondpy) for details.

Previous: [Data](data.md)

Next: [Under the Hood](math.md)
