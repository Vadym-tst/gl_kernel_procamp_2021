#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <math.h>

#define NAME_BUF_SIZE			(16)
#define CALLOC_ELEMENT_SIZE		(1)
#define MAX_STEPS				(64)
#define NANOSEC_IN_1_SEC		(1000000000L)

enum {
	MALLOC,
	CALLOC,
	ALLOCA,
	NUM_OF_ALLOC_FUNC,
};

struct allocator{
	void *(*allocation_method)(size_t size);
	void (*deallocation_method)(void *p);
	const char name[NAME_BUF_SIZE];
};

static inline void *_alloca(size_t num)
{
	return alloca(num);
}

static inline void *_calloc(size_t num)
{
	return calloc(num, CALLOC_ELEMENT_SIZE);
}

static inline void _free(void *p) { }

static inline void get_time(struct timespec *p_time)
{
	int result = clock_gettime(CLOCK_REALTIME, p_time);

	if (result != 0) {
		printf("Error %d: can't get current time\n", result);
	}
}

static inline void get_time_diff(struct timespec *since, struct timespec *till,
									struct timespec *result)
{
	result->tv_sec = till->tv_sec - since->tv_sec;
	result->tv_nsec = till->tv_nsec - since->tv_nsec;
	if (result->tv_nsec < 0) {
		--result->tv_sec;
		result->tv_nsec += NANOSEC_IN_1_SEC;
	}
}

int main()
{
	struct timespec start, stop, result;
	int step = 0;
	void *pointer;
	const struct allocator allocators[] = { {malloc, free, "malloc"},
											{_calloc, free, "calloc"},
											{_alloca, _free, "alloca"} };
	while(step <= MAX_STEPS) {
		for (int i = 0; i < NUM_OF_ALLOC_FUNC; i++) {
			get_time(&start);
			pointer = allocators[i].allocation_method((size_t)pow(2, step));
			get_time(&stop);
			get_time_diff(&start, &stop, &result);
			printf("%s: 2^%d bytes allocation\ttime is %lu.%09lu s\n",
					allocators[i].name, step, result.tv_sec, result.tv_nsec);
			get_time(&start);
			allocators[i].deallocation_method(pointer);
			get_time(&stop);
			get_time_diff(&start, &stop, &result);
			printf("%s: 2^%d bytes deallocation\ttime is %lu.%09lu s\n",
					allocators[i].name, step, result.tv_sec, result.tv_nsec);
		}
		step++;
	}
	printf("\nMEMORY ALLOCATION TIME MEASURING TEST FINIFHED");

	return 0;
}
