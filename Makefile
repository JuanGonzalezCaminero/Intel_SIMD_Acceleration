#CC = gcc
.PHONY: check

CFLAGS= -O3

all: 
	icc -O3 -o sharpen_t sharpen_t.c ctimer.c
	icc -O3 -o sharpen_t_AVX sharpen_t_AVX.c ctimer.c
	#icc -O3 -g -o testing testing.c
	icc -O0 -g -o testing testing.c

all_gcc: 
	gcc -O3 -o sharpen_t sharpen_t.c ctimer.c
	gcc -O3 -mavx -o sharpen_t_AVX sharpen_t_AVX.c ctimer.c
	#icc -O3 -g -o testing testing.c
	gcc -O0 -g -mavx -o testing testing.c

debug:
	icc -O0 -g -o sharpen_t_AVX sharpen_t_AVX.c ctimer.c

run:
	./sharpen_t sunset.ppm
	./sharpen_t_AVX sunset.ppm

run_avx:
	./sharpen_t_AVX sunset.ppm

run_testing:
	./testing

ref:
	./sharpen_t sunset.ppm
	mv sharpen.ppm reference.ppm

check:
	./sharpen_t_AVX sunset.ppm
	./check reference.ppm sharpen.ppm