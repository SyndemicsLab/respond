# Running the Model

The RESPOND model, in addition to forming the basis for the Simdemics library, acts as a completely independent executable model. This model was designed to track state transitions to study opioid use disorder.

## Using the Executable

Running RESPOND is incredibly simple provided you use our packaged executable. If built using the "gcc-release" workflow outlined in the [Installation Section](installation.md) the following command runs the executable from the root of the repository on input folder 1:

```bash
./build/extras/executable/respond_exe /path/to/input/folders 1 1
```

## Arguments

The executable takes 3 straightforward positional arguments that govern the input folder location, the starting input folder and the end input folder inclusively. Thus, if you only have a single input folder titled `input1` located at `/home/usr/` you would provide the arguments: `/home/usr/ 1 1`. If you have multiple input folders (i.e. `input1`, `input2`, and `input3`) you would provide: `/home/usr/ 1 3`.

Previous: [Data](data.md)

Next: [Under the Hood](math.md)
