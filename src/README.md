# Known Issues

1. Use COPRTHR host callback API for SHMEM memory routines
2. Fix `shmem_info_get_name()`
3. The `[f]collect[32|64]` routines are broken
4. For the atomic fetch and set operations, do these need to use locks?

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
| `SHMEM_USE_WAND_BARRIER` | hardware accelerated `shmem_barrier_all()`     |
| `SHMEM_USE_IPI_GET`      | `shmem_*_get()` use inter-processor interrupts |

# Building

The ARL OpenSHMEM for Epiphany package may be extracted to any location. This
directory contains all of the OpenSHMEM routines and other supplemental
routines. Separate files for each routine are required to reduce the total
binary size when compiling with the library. You may install the shmem.h header
and libshmem.a library wherever you like. The test codes within the
[../test](../test) directory will use the relative path to the header and
library during compilation.

To build the library, first unpack the archive:
```
cd src
```
Choose the makefile for the SDK you'll be developing (eSDK or COPRTHR-2).
Alternatively, you can copy the Makefile.esdk or Makefile.coprthr file to
Makefile and type `make`.
```
make -f Makefile.[esdk|coprthr]
```
Then test and benchmark the code. This is optional and presently works only
with the COPRTHR-2 SDK:
```
cd ../test
make
./run_tests.sh
```
