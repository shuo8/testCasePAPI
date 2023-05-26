It consists of 3 loops. The first two are for data initialization. The third one is used to generate data cache misses in L1 layer.

You can adjust the parameter “stride” to change the behaviour of the third loop. Theoretically, when “stride” is smaller than some threshold, cache misses should be small. Otherwise, it should be big. The threshold depends on the sizes of  each data element and cache line.

In the attached code, each element of “matrix” is int64, which is 8 bytes. Cache line in most architecture is 64 bytes. When “stride” <= 8, cache miss rate should be stride/8. When “stride” > 8, cache miss rate should be close to 100%. The actual results are also impacted by other factors, such as cache prefetch.

Normally, we try “stride” with a number of power of 2, like 1, 2, 4, 8, 16, 32 …. When I tried with stride=256, each data access triggers one data miss. The loop generates 8192*8192 L1 data cache misses by default config.

You can use PAPI to profile this case and compare results returned by PAPI with theoretical analysis. You can also change data type to floats and collect the number of floating point operations using performance counters ( depending on whether your computer supports floating point counters. Wiener in RCC does have this feature).

To do this, you need to make your work a bit flexible to allow users to set interested events. I think you can do it either using a configure file or environment variables.