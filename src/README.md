# Notes on Memory Management

Within the context of the Epiphany architecture, `shmem_malloc()`,
`shmem_free()`, `shmem_realloc()`, and `shmem_align()` should` not be used.
Instead, the lower-level `shmemx_sbrk()`, and `shmemx_brk()` should be used
to acquire and release memory resources. The operations should also be
symmetric for each processing element.

If you must use the malloc/free/realloc/align routines, you must adhere to
the following rules:

1. `shmem_free()` must be called in the reverse order of allocation
2. `shmem_realloc()` can only be used on the last (re)allocated pointer
3. `shmem_align()` alignment must be a power of 2 greater than 8 (default is 8)

Failure to follow these rules will result in undefined behavior.

# Extensions

As mentioned above, `shmemx_sbrk()` and `shmemx_brk()` should be used for
memory management.

Compile-time options include:

| Option                   | Description                                    |
|--------------------------|------------------------------------------------|
| `SHMEM_HEAP_START` `#`   | minimum local address of SHMEM heap            |
| `SHMEM_USE_UNSAFE`       | use performance features at your own peril     |
| `SHMEM_USE_WAND_BARRIER` | hardware accelerated `shmem_barrier_all()`     |
| `SHMEM_USE_IPI_GET`      | `shmem_*_get()` use inter-processor interrupts |
| `SHMEM_USE_HEADER_ONLY`  | feature avoids linking with -lshmem; enables \ |
|                          | higher performance, smaller program footprint  |

# Building

This directory contains all of the OpenSHMEM routines and other supplemental
routines. Separate files for each routine are required to reduce the total
binary size when compiling with the library. You may install the shmem.h header
and libshmem.a library wherever you like but shmem.h must be expanded first or
take all include all code. The test codes within the [../test](../test)
directory will use the relative path to the header and library during
compilation.

Choose the makefile for the SDK you'll be developing.  The default with `make`
targets the COPRTHR-2 SDK.  Alternatively, you can build and install for the
native eSDK with:
```
make -f Makefile.esdk
sudo make -f Makefile.esdk install
```

The install expands shmem.h to include all code with the compile-time options
you selected.

Then test and benchmark the code in the test directory. This is optional and
presently works only with the COPRTHR-2 SDK. You make use `make run` or `make
check`

You may also run the example codes.  The directory name prefixes of `c_` and
`e_` are for use with the COPRTHR-2 SDK and eSDK, respectively.
