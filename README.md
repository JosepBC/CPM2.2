# Commands

# Compile
```bash
make
```

Run with time
```bash
make time
```


# Zoo

## Tunel
```bash
ssh -NL 8283:zoo.lab.deim:22 <nif>@portal1-deim.urv.cat
```

## Connect
```bash
ssh -X -p 8283 <nif>@localhost
```

## Scp files
```bash
scp -P 8283 <file> <nif>@localhost:.
```

## Run script
```bash
salloc -p pop -N 8 -n 32 time -p mpirun -n ${total_procs} -npernode ${proc} ./build/par.out
```

Where machine can be:
* POP
* dos6