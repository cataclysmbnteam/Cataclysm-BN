---
title: Unit tests
---

Check [testing](../../dev/guides/testing.md) for details.

There is a suite of tests built into the source tree at tests/ You should run the test suite after
ANY change to the game source. An ordinary invocation of `make` will build the test executable at
tests/cata_test, and it can be invoked like any ordinary executable, or via `make check`. With no
arguments it will run the entire test suite. With `--help` it will print a number of invocation
options you can use to adjust its operation.

```sh
$ make
... compilation details ...
$ tests/cata_test
Starting the actual test at Fri Nov  9 04:37:03 2018
===============================================================================
All tests passed (1324684 assertions in 94 test cases)
Ended test at Fri Nov  9 04:37:45 2018
The test took 41.772 seconds
```

I recommend habitually invoking make like `make YOUR BUILD OPTIONS && make check`.
