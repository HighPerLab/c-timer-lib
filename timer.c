#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>

#include "timer.h"

/** Functions **/

/* Function
 *  internal function to return a human understandable meaning for the
 *  status returns.
 *
 *  @param: the return status values
 *
 *  @return: the human understandable string meaning of the status value
 */
inline
char * error_num(int status)
{
    char * tmp;
    switch(status)
    {
        case OK:
            tmp = (char *) "status is OK";
            break;
        case NOT_ALLOCATED:
            tmp = (char *) "variable not allocated!";
            break;
        case CLOCK_FAILED:
            tmp = (char *) "clock not available!";
            break;
        default:
            tmp = (char *) "Unknown status number!";
            break;
    }
    return tmp;
}

/* Function
 *  internal function that interprets the clock enum and returns the
 *  appropriate system clock.
 *
 *  @param ck: clock enum
 *
 *  @return: clockid_t system clock
 */
inline
clockid_t set_clock(clock_e ck)
{
    clockid_t clock;
    switch(ck)
    {
        case rtc:
            clock = CLOCK_REALTIME_COARSE;
            break;
        case mono:
            clock = CLOCK_MONOTONIC;
            break;
        case monoc:
            clock = CLOCK_MONOTONIC_COARSE;
            break;
        case monor:
            clock = CLOCK_MONOTONIC_RAW;
            break;
        case cpup:
            clock = CLOCK_PROCESS_CPUTIME_ID;
            break;
        case cput:
            clock = CLOCK_THREAD_CPUTIME_ID;
            break;
        case monob:
#ifdef CLOCK_BOOTTIME
            clock = CLOCK_BOOTTIME;
            break;
#endif
        default:
            ERROR("Invalid CLOCK value, using CLOCK_REALTIME");
        case rt:
            clock = CLOCK_REALTIME;
            break;
    }
    return clock;
}

/* Function
 *  internal function that calculates the difference between two struct
 *  timespecs and returns a struct timespec containing the difference.
 *
 *  @param start: the fist measured time point
 *  @param end: the final or last measured time point
 *
 *  @return: struct timespec The difference between start and end
 */
inline
struct timespec diff_timespec(struct timespec end, struct timespec begin)
{
    struct timespec result = begin;
    /* Perform the carry for the later subtraction. */
    if (end.tv_nsec < begin.tv_nsec) {
        int nsec = (begin.tv_nsec - end.tv_nsec) / 1000000000 + 1;
        result.tv_nsec -= 1000000000 * nsec;
        result.tv_sec += nsec;
    }
    if (end.tv_nsec - begin.tv_nsec > 1000000000) {
        int nsec = (end.tv_nsec - begin.tv_nsec) / 1000000000;
        result.tv_nsec += 1000000000 * nsec;
        result.tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.
     *             tv_usec is certainly positive. */
    result.tv_sec = end.tv_sec - result.tv_sec;
    result.tv_nsec = end.tv_nsec - result.tv_nsec;

    return result;
}

/* 
 * XXX: This is a bit of unnecessary toil, however it does the job...
 * An inline solution would be more practical... but I can't
 * remember how to do it with a struct...
 */
int set_timespec(struct timespec ** tims, long sec, long nsec)
{
    *tims = (struct timespec *) malloc(sizeof(struct timespec));
    CHECK(!*tims, "Failed to allocate timespec!");

    (*tims)->tv_sec = sec;
    (*tims)->tv_nsec = nsec;
    return OK;

error:
    return NOT_ALLOCATED;
}

/* Function
 *  internal function to interpret the unit enum and return the
 *  appropriate time unit.
 *
 *  @param unit: enum time unit
 *
 *  @return: string The time unit
 */
inline
char * print_unit(unit_e unit)
{   
    switch(unit)
    {
        default:
            ERROR("Invalid UNIT value, using seconds (s)");
        case 0:
            return (char *) "s";
            break;
        case 1:
            return (char *) "ms";
            break;
        case 2:
            return (char *) "us";
            break;
        case 3:
            return (char *) "ns";
            break;
    }
    return (char *) "null";
}

/* Function
 *  create interval variable, which means to allocate the underlying
 *  structure.
 *
 *  @param tmp: the address of the interval to be allocated
 *  @param name: the name of the interval
 *  @param ck: clock enum
 *  @param ut: unit enum
 *
 *  @return: status code
 */
int create_interval(interval_t ** tmp, char * name, clock_e ck, unit_e ut)
{
    *tmp = (interval_t *) malloc(sizeof(interval_t));
    CHECK(!*tmp, "Unable to create interval %s!", name);
    (*tmp)->name = name;
    (*tmp)->clock = ck;
    (*tmp)->unit = ut;
    return OK;
    
error:
    if(*tmp) free(*tmp);
    return NOT_ALLOCATED;
}

/* Function
 *  get the current time and save its value in the appropriate pointer
 *
 *  @param clock: the system clock to be used
 *  @param time: the timespec structure that holds the time
 *
 *  @return: either OK, or error status from clock_gettime
 */
inline
int get_time(clockid_t clock, struct timespec * time)
{
    int ret;
    ret = clock_gettime(clock, time);
    CHECK(ret, "Failed to get start time!");
    return OK;

error:
    return ret;
}

/* Function
 *  set the start field of the interval with the current time
 *
 *  @param tmp: the interval
 *
 *  @return: either OK, or error status from clock_gettime
 */
int start(interval_t * tmp)
{
    return get_time(set_clock(tmp->clock), &(tmp->start));
}

/* Function
 *  set the stop field of the interval with the current time
 *
 *  @param tmp: the interval
 *
 *  @return: either OK, or error status from clock_gettime
 */
int stop(interval_t * tmp)
{
    return get_time(set_clock(tmp->clock), &(tmp->stop));
}

/* Function
 *  compute the elapsed time from the interval
 *
 *  @param tmp: the interval
 *  @param ut: unit enum
 *
 *  @return: the elapsed time in the global time unit
 */
inline
double elapsed_interval(interval_t * tmp, unit_e ut)
{
    double time = 0.0;
    unit_e unit;
    struct timespec diff;

    diff = diff_timespec(tmp->stop, tmp->start);
    unit = 0 <= ut && ut < unit_check ? ut : tmp->unit;
    switch(unit)
    {
        default:
            ERROR("Invalid UNIT value, using seconds (s)");
        case s:
            time = (double) diff.tv_sec + NANO_TO_SEC(diff.tv_nsec);
            break;
        case ms:
            time = (double) SEC_TO_MSEC(diff.tv_sec) + NANO_TO_MSEC(diff.tv_nsec);
            break;
        case us:
            time = (double) SEC_TO_MCSEC(diff.tv_sec) + NANO_TO_MCSEC(diff.tv_nsec);
            break;
        case ns:
            time = (double) SEC_TO_NSEC(diff.tv_sec) + diff.tv_nsec;
            break;
    }
    return time;
}

/* Function
 *  this function prints out the elapsed time(s) from the given interval(s). It supports
 *  several different print formats.
 *
 *  @param num: the number of intervals to be printed
 *  @param ...: the interval(s)
 */
inline
void print_results(int num, ...)
{
    va_list vl;
    char * names[num];
    double values[num];
    unit_e units[num];

    va_start(vl, num);
    for(int i = 0; i < num; i++)
    {
        interval_t * time = va_arg(vl, interval_t *);
        units[i] = time->unit;
        names[i] = (time->name == NULL) ? NULL : strdup(time->name);
        values[i] = elapsed_interval(time, none);
    }
    va_end(vl);

    for(int i = 0; i < num; i++)
    {
        printf("%s: %.3f %s\n", names[i], values[i], print_unit(units[i]));
        free(names[i]);
    }
} 

/* Function
 *  this function prints out the elapsed time(s) from the given interval(s).
 *  Print out is in a CSV compatible format.
 *
 *  @param comment: comment character that precedes the headers
 *  @param num: the number of intervals to be printed
 *  @param ...: the interval(s)
 */
inline
void print_results_csv(char * comment, int num, ...)
{
    va_list vl;
    char * names[num];
    double values[num];
    unit_e units[num];

    va_start(vl, num);
    for(int i = 0; i < num; i++)
    {
        interval_t * time = va_arg(vl, interval_t *);
        units[i] = time->unit;
        names[i] = strdup(time->name);
        values[i] = elapsed_interval(time, none);
    }
    va_end(vl);

    printf("%s ", comment);
    for(int i = 0; i < num; i++)
    {
        printf("%s (%s)", names[i], print_unit(units[i]));
        free(names[i]);
        if(i < num - 1) printf(", ");
    }
    printf("\n");
    for(int i = 0; i < num; i++)
    {
        printf("%.3f", values[i]);
        if(i < num - 1) printf(", ");
    }
    printf("\n");
}
