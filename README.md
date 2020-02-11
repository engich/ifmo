# ifmo

### Building
To build an executable in **build** folder:

    mkdir build
    cd build
    cmake ../
    cmake --build .
    
### Executing
To execute binary:

    ./w_model [Options]
    
Where Options are of the form **-n[=value]** or **--Name[=value]**

Lists of options:

General:

    -h, --help :    Prints this message (optional, default: 0)
    --stopOnUnknownParam :  Stop if unkown param entered (optional, default: 1)
    -S, --seed :    Random number seed (optional, default: 1581386493)

Persistence:

    -o, --out :     Output file of the sampling (optional, default: out.dat)
    --status :      Status file (optional, default: ./cmake-build-debug/w_model.status)

Representation:

    -V, --vecSize : Genotype size (optional, default: 12)
    -n, --nbSol :   Number of adaptive walks (optional, default: 100)
    -D, --dumm :    Dummy parameter of w-model transformation (optional, default: 0)
    -N, --neut :    Neutrality parameter of w-model transformation (optional, default: 0)
    -E, --epis :    Epistasis parameter of w-model transformation (optional, default: 0)
    -R, --rugg :    Ruggedness parameter of w-model transformation (optional, default: 0)

### Example

    ./w_model -V=12, -n=100, -D=1, -N=2, -E=3, -R=4
