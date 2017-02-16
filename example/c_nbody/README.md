# N-body

This example demonstrates a high performance gravitational particle interaction
([N-body simulation](http://en.wikipedia.org/wiki/N-body_simulation)).

### Compile and Run:

The code uses default definitions found in the top of the Makefile.
```
make
./main.x
```

### Notes:

One must take care to balance the work across cores with appropriate values for
the following command line args.

This code uses OpenSHMEM without an explicit bulk memory movement routine (i.e.
`shmem_put()`). It now uses remote direct memory access (RDMA) as a
demonstration and a software-defined cache in the ComputeAccel routine to
mitigate the cost of repeated remote access. This technique achieves ~95% of
the performance compared to optimized copying and extra buffer, but the added
benefit is that large copy buffers are not required and more particles can fit
on-chip.

The number of particles should be a multiple of 16 per core because of the loop
unrolling (x8) and software cache (16 particle chunks).

### Command Line Options:

`-n [on-chip number of particles]`
> This is the on-chip number of particles, which is limited by the available
> memory. Each particle uses 40 bytes. For example, 512 particles per core uses
> 20 KiB of data. Typically, 8192 may be used for Epiphany-III or up to 32768
> for Epiphany IV. Exceeding approximately 512 particles per core results in
> undefined behavior.

`-s [off-chip scale factor]`
> This value is the amount of off-chip blocks. Setting -n 4096 and -s 2, will
> perform a 8192 particle calculation. Use -s 1 (default) for a fully on-chip
> calculation. Because an on-chip work buffer must be used, 56 bytes per
> particle is used, so it further limits the total number of particles

`-i [number of time step iterations]`
> This value is the outer loop of the calculation, or the number of time steps
> for particle position updates.

`-d [number of cores]`
> This is the dimension of the 1D array of cores (typically 16 for Epiphany-III
> or 64 for Epiphany-IV). You may use less than the number of cores available
> but the behavior is undefined when using more.

`--validate`
> This performs the same computation on the host CPU (slowly) and compares the
> result. Take the listed number of errors with a grain of salt because order
> of operations matter.

`-h, --help`
> Lists help for commands

### Example Uses:

`./main.x`
> Uses default definitions found in the top of the Makefile

`./main.x -n 8192 -i 100 -d 16`
> Performs a 8192 particle calculation with each of the 16 cores getting a 512
> particle working set.

`./main.x -n 6400 -s 2 -i 100 -d 16`
> Performs a 12800 particle calculation with each of the 16 cores getting a 400
> particle working set.

`./main.x -n 256 -s 16 -i 100 -d 1 --validate`
> Performs a 4096 particle calculation with a single core and validates the
> result with the ARM CPU host.
