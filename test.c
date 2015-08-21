#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "timer.h"

int main()
{
    interval * a;
    interval * b;
    interval * c;

    create_interval(&a, "Test 1");
    create_interval(&b, "Test 2");
    create_interval(&c, "Test 3");

    printf("Running '%s'\n", a->name);
    
    start(a);
    nanosleep((struct timespec[]){{1, 0}}, NULL);
    stop(a);
    
    printf("RAW:\n START: %lld.%.9ld\n END: %lld.%.9ld\n", (long long) a->start.tv_sec, a->start.tv_nsec, (long long) a->stop.tv_sec, a->stop.tv_nsec); 
    printf("OUT: %.9f %s\n", elapsed_interval(a), unit());
    printf("EXPECTED: 1 sec\n");
    
    printf("Running '%s'\n", b->name);
    
    start(b);
    nanosleep((struct timespec[]){{1, MILLI_TO_NSEC(500)}}, NULL);
    stop(b);
    
    printf("RAW:\n START: %lld.%.9ld\n END: %lld.%.9ld\n", (long long) b->start.tv_sec, b->start.tv_nsec, (long long) b->stop.tv_sec, b->stop.tv_nsec); 
    printf("OUT: %.9f %s\n", elapsed_interval(b), unit());
    
    printf("Running '%s'\n", c->name);
    printf("EXPECTED: 1.5 sec\n");
    
    start(c);
    nanosleep((struct timespec[]){{2, MILLI_TO_NSEC(756)}}, NULL);
    stop(c);
    
    printf("RAW:\n START: %lld.%.9ld\n END: %lld.%.9ld\n", (long long) c->start.tv_sec, c->start.tv_nsec, (long long) c->stop.tv_sec, c->stop.tv_nsec); 
    printf("OUT: %.9f %s\n", elapsed_interval(c), unit());
    printf("EXPECTED: 2.756 sec\n");
    
    printf("FINAL TEST\n");
    print_results(3, a, b, c);
   
    free(a);
    free(b);
    free(c);

    return EXIT_SUCCESS;
}
