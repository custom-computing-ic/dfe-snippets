# dfe-snippets

`dfe-snippets` is a collection of building blocks for projects using Maxeler Dataflow Engines (DFEs).

## Requires

1. MaxCompiler 2013+
2. Compatible [Maxeler](http://www.maxeler.com/products/) hardware
3. python 2.7 (for running scripts)

## Structure

Implementations are grouped by concept (e.g. Summation, Infrastructure, Sorting) as shown below:

```
scripts/          ---> Useful scripts
concept1/
  implementation1/
     src/          ---> source code
     build         ---> build directory
```

## Use Scripts

To use the scripts, first add the directory to your PATH:

```
PATH_TO_DFE_SNIPPETS="~/path/to/dfe-snippets"
export PATH=${PATH}:"${PATH_TO_DFE_SNIPPETS}/scripts"
```

Note that python scripts require __python 2.7__ (check your version with `python -v`).

### create.py

Use `create.py -h` for a list of available options.

To create and run a __new dfe-snippet__ from the snippets directory run:
```
./create.py <ProjectName> -c <ProjectConcept>
cd <ProjectConcept>/<ProjectName>/build && make runsim
```

To create an external, __stand-alone project__ run:

```
./create.py <ProjectName>
cd <ProjectName>/build && make runsim
```
