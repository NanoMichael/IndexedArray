A simple project to benchmark between sorted binary search array, hash map and tree map with multi keys in C++. 

Compiled with `g++ 7.5.0`.

With build-type `Debug`, the result is:

```
 SIZE       BA      MAP     HASH
   1K     0.09     0.69     0.21
  10K     1.07     9.79     2.31
 100K    11.90   123.60    33.51
   1M   142.81  1512.49   479.21
  10M  1648.69 18656.31 12088.24
```

The `SIZE` represents the data length for each epoch, with 3 elements for each item generated randomly, followed by the running durations in milliseconds.

With build-type `Release`, the result is:

```
 SIZE       BA      MAP     HASH
   1K     0.04     0.05     0.01
  10K     0.47     0.59     0.21
 100K     4.45     9.23     5.60
   1M    49.91   127.81   100.36
  10M   538.66  1409.72  6595.97
```