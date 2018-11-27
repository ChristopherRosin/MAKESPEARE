# MAKESPEARE/benchmarks

This has the newly-created input/output data for the benchmarks used in the MAKESPEARE AAAI paper, in the format required by MAKESPEARE.  These are gzip-compressed files, and two have been split due to github file size limits.  To prepare these files for use:

```
cat fast-sort.tsv.gz.* > fast-sort.tsv.gz
cat binary-search.tsv.gz.* > binary-search.tsv.gz
gzip -d *.gz
```

Note this doesn't include the 7 Established Benchmarks for which input/output data was previously available; for those see the conversion script in MAKESPEARE/scripts.  
