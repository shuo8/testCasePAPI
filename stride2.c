#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <papi.h>
#define SIZE_DEFAULT 8192
#define STRIDE_DEFAULT 256
/*return the duration in seconds */

double CalElapsedTime(struct timeval *tv_begin, struct timeval *tv_end)
{
    double ela_time;
    long ela_secs;
    long ela_usecs;
    if (tv_end->tv_usec >= tv_begin->tv_usec)
    {
        ela_secs = tv_end->tv_sec - tv_begin->tv_sec;
        ela_usecs = tv_end->tv_usec - tv_begin->tv_usec;
    }
    else
    {
        ela_secs = tv_end->tv_sec - tv_begin->tv_sec - 1;
        ela_usecs = tv_end->tv_usec - tv_begin->tv_usec + 1000000;
    }
    ela_usecs += ela_secs * 1000000;
    ela_time = ((double)ela_usecs) / 1000000;
    return ela_time;
}

void print_usage(char **argv) // PAPI_reset(eventset);
{
    printf("Usage:  %s -s size -d stride\n", *argv);
}

int main(int argc, char *argv[])
{
    int retval;
    retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT)
    {
        fprintf(stderr, "Error initializing PAPI! %s\n",
                PAPI_strerror(retval));
        return 0;
    }

    int eventset = PAPI_NULL;
    retval = PAPI_create_eventset(&eventset);
    if (retval != PAPI_OK)
    {
        fprintf(stderr, "Error creating eventset! %s\n",
                PAPI_strerror(retval));
    }

    retval = PAPI_add_named_event(eventset, "PAPI_L1_DCM");
    if (retval != PAPI_OK)
    {
        fprintf(stderr, "Error adding PAPI_L1_DCM: %s\n",
                PAPI_strerror(retval));
    } // PAPI_reset(eventset);
    long long count;
    long long total = 0;
    PAPI_reset(eventset);
    int length = SIZE_DEFAULT;
    int width = SIZE_DEFAULT;
    int stride = STRIDE_DEFAULT;
    int i;
    int j;
    int64_t *matrix;
    int64_t *matrix_fill;
    int iteration = 0;
    int cha;
    struct timeval begin;
    struct timeval end;
    double duration;
    int64_t temp = 0;
    while ((cha = getopt(argc, argv, "s:d:h")) != -1)
        switch (cha)
        {
        case 's':
            length = (atol(optarg));
            width = length;
            break;
        case 'd':
            stride = atoi(optarg);
            break;
        case 'h':;
        default:
            print_usage(argv);
            exit(0);
        }
    if (length <= 0 || width <= 0)
    {
        printf("ERROR: Invalid matrix width : %d, length: %d\n", width, length);
        exit(1);
    }
    printf("width:%d, length: %d, stride:%d\n", width, length, stride);
    matrix = ((int64_t *)(aligned_alloc(64, sizeof(int64_t) * length * width)));
    matrix_fill = ((int64_t *)(aligned_alloc(64, sizeof(int64_t) * length * width)));
    // initialization
    for (j = 0; j < width; j++)
    {
        for (i = 0; i < length; i++)
        {
            matrix[i * width + j] = (i * j);
        }
    }
    // clear cache
    for (j = 0; j < width; j++)
    {
        for (i = 0; i < length; i++)
        {
            matrix_fill[i * width + j] = (i * j);
        }
    }
    iteration = length * width / stride;
    gettimeofday(&begin, (void *)0);
    PAPI_start(eventset);
    for (j = 0; j < stride; j++)
    {
        for (i = 0; i < iteration; i++)
        {
            temp = +matrix[i * stride + j];
        }
    }
    PAPI_stop(eventset, &count);
    printf("PAPI_L1_DCM: %lld \n", count);
    PAPI_cleanup_eventset(eventset);
    PAPI_destroy_eventset(&eventset);
    gettimeofday(&end, (void *)0);
    duration = CalElapsedTime(&begin, &end);
    printf("Time: %f with the execution, result: %ld\n", duration, temp);
    if (matrix != ((void *)0))
        free(matrix);
    if (matrix_fill != ((void *)0))
        free(matrix_fill);
    return 0;
}