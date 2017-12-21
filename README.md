ARL OpenSHMEM for Epiphany
======

OpenSHMEM is a standardized interface to enable portable applications for
partitioned global address space (PGAS) architectures. This repository includes
the *ARL OpenSHMEM for Epiphany* implementation for optimized one-sided
communication, atomic, and collective operations for the
[Adapteva](http://adapteva.com) Epiphany microarchitecture on the [Parallella
board](http://parallella.org) (see [./src](./src)). Also included are
microbenchmarks (see [./example](./example)) and test codes (see
[./test](./test)). These codes require the COPRTHR-2 SDK. The library may be
used without COPRTHR-2 and has been tested with the Parabuntu 2016.11.1
release.

The OpenSHMEM implementation is based on the [OpenSHMEM 1.4
Specification](http://openshmem.org/site/sites/default/site\_files/OpenSHMEM-1.4.pdf)

The software was developed at the US Army Research Laboratory, Computational
Sciences Division, Computing Architectures Branch.

Prerequisites
-----

Code has been tested on the
[2016.11.1](https://github.com/parallella/parabuntu/releases) and
[2015.1](http://ftp.parallella.org/ubuntu/dists/trusty/image/) Parallella
images, but please use the former.

You must download and install the [COPRTHR-2
SDK](http://www.browndeertechnology.com/coprthr2.htm) to be able to run the
[./test](./test) and most [./example](./example) codes. The library is free for
non-commercial use.

Although it's not required to build the library, a
[Makefile](./src/Makefile.esdk) is provided if you're using the vendor-provided
eSDK.

Using
-----

You may type `make` to build all directories, or `make [src|example|test]`
to build individual subdirectories. Running `make run` will run example codes
and a selection of test codes. Running `make check` will run all of the test
codes with various parameters. There is no installation, but you should link to
the _libshmem.a_ library and use the appropriate include path for _shmem.h_.

If you're not using the COPRTHR-2 SDK, you can build the library for the eSDK:
`cd ./src; make -f Makefile.esdk`

Citation
-----

If you are using this work in an academic publication, please include the
following reference in your paper:

*J. Ross and D. Richie, "An OpenSHMEM Implementation for the Adapteva Epiphany
Coprocessor," OpenSHMEM and Related Technologies. Enhancing OpenSHMEM for
Hybrid Environments, vol. 10007, pp. 146-159, Dec. 2016,
doi:10.1007/978-3-319-50995-2_10*
