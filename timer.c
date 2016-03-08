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
 *  status returns. TODO: needs to be used... ;)
 *
 *  @param: the return status values
 *  @return: the humand understanble string meaning of the status value
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
 *  internal function that interprets the TIMERCLOCK macro and returns the
 *  appropriate system clock.
 *
 *  @return: clockid_t System Clock
 */
inline
clockid_t set_clock()
{
    clockid_t clock;
    switch(TIMERCLOCK)
    {
        default:
            ERROR("Invalid TIMERCLOCK value, using CLOCK_REALTIME");
        case 0:
            clock = CLOCK_REALTIME;
            break;
        case 1:
            clock = CLOCK_REALTIME_COARSE;
            break;
        case 2:
            clock = CLOCK_MONOTONIC;
            break;
        case 3:
            clock = CLOCK_MONOTONIC_COARSE;
            break;
        case 4:
            clock = CLOCK_MONOTONIC_RAW;
            break;
        case 5:
            clock = CLOCK_BOOTTIME;
            break;
        case 6:
            clock = CLOCK_PROCESS_CPUTIME_ID;
            break;
        case 7:
            clock = CLOCK_THREAD_CPUTIME_ID;
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
 * This is a bit of unnessicary toil, however it does the job...
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
 *  internal function to interpret the TIMERUNIT macro and return the
 *  appropriate time unit.
 *
 *  @return: string The time unit
 */
inline
char * unit()
{   
    char * s;
    switch(TIMERUNIT)
    {
        default:
            ERROR("Invalid TIMEUNIT value, using seconds (s)");
        case 0:
            s = (char *) "s";
            break;
        case 1:
            s = (char *) "ms";
            break;
        case 2:
            s = (char *) "us";
            break;
        case 3:
            s = (char *) "ns";
            break;
    }
    return s;
}

/* Function
 *  create interval variable, which means to allocate the underlying
 *  structure.
 *
 *  @param tmp: the address of the interval to be allocated
 *  @param name: the name of the interval
 *
 *  @return: status code
 */
int create_interval(interval ** tmp, char * name)
{
    *tmp = (interval *) malloc(sizeof(interval));
    CHECK(!*tmp, "Unable to create interval %s!", name);
    (*tmp)->name = name;
    return OK;
    
error:
    if(*tmp) free(*tmp);
    return NOT_ALLOCATED;
}

/* Function
 *  set the start field of the interval with the current time
 *
 *  @param tmp: the interval
 *
 *  @return: either OK, or error status from clock_gettime
 */
int start(interval * tmp)
{
    int ret;
    ret = clock_gettime(set_clock(), &(tmp->start));
    CHECK(ret, "Failed to get start time!");
    return OK;

error:
    return ret;
}

/* Function
 *  set the stop field of the interval with the current time
 *
 *  @param tmp: the interval
 *
 *  @return: either OK, or error status from clock_gettime
 */
int stop(interval * tmp)
{
    int ret;
    ret = clock_gettime(set_clock(), &(tmp->stop));
    CHECK(ret, "Failed to get stop time!");
    return OK;

error:
    return ret;
}

/* Function
 *  compute the elapsed time from the interval
 *
 *  @param tmp: the interval
 *
 *  @return: the elapsed time in the global time unit
 */
inline
double elapsed_interval(interval * tmp)
{
    double time = 0.0;
    struct timespec diff = diff_timespec(tmp->stop, tmp->start);
    switch(TIMERUNIT)
    {
        default:
            ERROR("Invalid TIMEUNIT value, using seconds (s)");
        case 0:
            time = (double) diff.tv_sec + NANO_TO_SEC(diff.tv_nsec);
            break;
        case 1:
            time = (double) SEC_TO_MSEC(diff.tv_sec) + NANO_TO_MSEC(diff.tv_nsec);
            break;
        case 2:
            time = (double) SEC_TO_MCSEC(diff.tv_sec) + NANO_TO_MCSEC(diff.tv_nsec);
            break;
        case 3:
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
    // Not used yet
    //char * units[num];

    va_start(vl, num);
    for(int i = 0; i < num; i++)
    {
        interval * time = va_arg(vl, interval *);
        names[i] = strdup(time->name);
        values[i] = elapsed_interval(time);
    }
    va_end(vl);

    for(int i = 0; i < num; i++)
    {
        printf("%s: %.3f %s\n", names[i], values[i], unit());
        free(names[i]);
    }
}
