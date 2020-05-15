A simple project to benchmark between sorted binary search array, hash map and tree map with multi keys in C++. The result is:

```
    SIZE       BA      MAP     HASH
      1K     0.15     0.67     0.22
     10K     1.60     8.49     2.48
    100K    13.64    77.21    31.08
      1M   165.98   956.82   537.98
     10M  1945.18 12358.15 13615.49
```

The `SIZE` represents the data length for each epoch, followed by the running durations in milliseconds. The binary sorted array is about 6 times faster than tree map and 7 times faster than hash map.