#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "timer.h"

#ifndef UNITS
#define UNITS s
#endif

int main()
{
    interval_t * a;
    interval_t * b;
    interval_t * c;

    create_interval(&a, "Test 1", mono, UNITS);
    create_interval(&b, "Test 2", mono, UNITS);
    create_interval(&c, "Test 3", mono, UNITS);

    printf("Running '%s'\n", a->name);
    
    start(a);
    nanosleep((struct timespec[]){{1, 0}}, NULL);
    stop(a);
    
    printf("RAW:\n START: %lld.%.9ld\n END: %lld.%.9ld\n", (long long) a->start.tv_sec, a->start.tv_nsec, (long long) a->stop.tv_sec, a->stop.tv_nsec); 
    printf("OUT: %.9f %s\n", elapsed_interval(a, -1), print_unit(a->unit));
    printf("EXPECTED: 1 sec\n");
    
    printf("Running '%s'\n", b->name);
    
    start(b);
    nanosleep((struct timespec[]){{1, MILLI_TO_NSEC(500)}}, NULL);
    stop(b);
    
    printf("RAW:\n START: %lld.%.9ld\n END: %lld.%.9ld\n", (long long) b->start.tv_sec, b->start.tv_nsec, (long long) b->stop.tv_sec, b->stop.tv_nsec); 
    printf("OUT: %.9f %s\n", elapsed_interval(b, -1), print_unit(b->unit));
    
    printf("Running '%s'\n", c->name);
    printf("EXPECTED: 1.5 sec\n");
    
    start(c);
    nanosleep((struct timespec[]){{2, MILLI_TO_NSEC(756)}}, NULL);
    stop(c);
    
    printf("RAW:\n START: %lld.%.9ld\n END: %lld.%.9ld\n", (long long) c->start.tv_sec, c->start.tv_nsec, (long long) c->stop.tv_sec, c->stop.tv_nsec); 
    printf("OUT: %.9f %s\n", elapsed_interval(c, -1), print_unit(c->unit));
    printf("EXPECTED: 2.756 sec\n");
    
    printf("FINAL TEST\n");
    print_results(3, a, b, c);
    print_results_csv("#", 3, a, b, c);
   
    free(a);
    free(b);
    free(c);

    return EXIT_SUCCESS;
}
