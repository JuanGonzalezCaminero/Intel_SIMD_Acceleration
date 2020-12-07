# Intel SIMD Acceleration
One of my Master's Degree assignments, consisting on improving the performance obtained by the Intel C Compiler on an image sharpening algorithm 
using Intel's SSE and AVX SIMD instructions.

My solution is based on the fact that the best performance, in this case, is achieved by reusing the data loaded to the processor's registers as 
much as we can (Based on results from Valgrind's Cachegrind). With this algorithm, we can use any given number loaded from the image in three 
consecutive iterations of the loops. 

Using this, I unrolled the processing of 8 columns and two rows from the image, loading each row once to 256-bit AVX registers, and using it three 
times. This solution uses 17 AVX registers, one more than the processor has. Unrolling more iterations would mean we wouldn't be able to keep all 
the data in the CPU, having to store and reload from cache multiple times.

This solution, executed with the test image provided on an Intel Xeon E5-2697 v2, achieves times 50 to 55% faster than the original.

These programs can be compiled and executed using GCC, however the results are much better when using the Intel C Compiler.

## Future work
This approach, unrolling multiple iterations, means we could theoretically reuse part of the operations from one iteration to the next. However, 
this also means we have to explicitly separate a sum and a multiplication between AVX registers. If this operations are made at the same time, 
the compiler is able to optimize them to a FMA operation, more efficient than doing it separately. This means that, even if we skip part of the 
multiplications, the end result is slower than doing all the operations using FMA. However, this wasn't tested thoroughly and it is possible 
there is a viable implementation of this concept.
