# Running the Model

Running RESPOND is incredibly simple, provided you use our packaged executable. If built using the "gcc-release" workflow the following command runs the executable from the root of the repository on input folder 1:

```bash
./build/extras/executable/hep_ce /path/to/input/folders 1 1
```

## Arguments

As you can tell, the executable takes 3 positional arguments. They're actually quite simplistic and straightforward. They govern the input folder location, the starting input folder and the end input folder inclusively. Thus, if you only have a single input folder titled `input1` located at `/home/usr/` you would provide the arguments: `/home/usr/ 1 1`. If you have multiple input folders (i.e. `input1`, `input2`, and `input3`) you would provide: `/home/usr/ 1 3`.

Previous: [Data](data.md)

Next: [Limitations](limitations.md)
