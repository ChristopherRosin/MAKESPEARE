# MAKESPEARE/scripts

converter-synthesis-suite.pl is a Perl script for converting the 7 "Established Benchmarks" used in the paper to the format required by MAKESPEARE.  These benchmarks were described in this paper:

*Helmuth, T., and Spector, L. 2015b. General program synthesis benchmark suite. In GECCO, 1039–1046.*

and the authors of this benchmark suite provided CSV files here:

<https://github.com/thelmuth/Program-Synthesis-Benchmark-Data>

which were used in the MAKESPEARE paper.  As an example, to convert the Collatz Numbers CSV from the above github repository, to the TSV format required by MAKESPEARE:

```
perl converter-synthesis-suite.pl < examples-collatz-numbers.csv > collatz-numbers.tsv
```

The script has only been tested to work with the 7 Established Benchmarks used in the MAKESPEARE paper.
