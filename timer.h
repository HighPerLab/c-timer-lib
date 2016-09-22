/* ***************************************************************************
 * This header file provides a rudimentary timing facility for C/C++ based
 * applications
 *
 * Makes use of the POSIX standard derived timing mechanisms found within the
 * C/C++ standard library. Specifically we use the `clock_gettime` function.
 *
 * This header file was designed for use for the accurate profiling of
 * applications in the context of benchmarking. It was created as part of a
 * masters project to investigate the performance portability between OpenCL
 * and Single-Assignment C (http://www.sac-home.org/) programming models.
 *
 * Created by Hans-Nikolai Viessmann (C) 2015 - 2016
 *
 * Following macros are available to manipulate verbose output, set time unit,
 * and specify the clock to be used:
 *  -   TIMERVER -> 0 (off) 1 (print errors: default) 2 (debug)
 *  -  TIMERUNIT -> 0 (sec: default) 1 (ms) 2 (us) 3 (ns)
 *  - TIMERCLOCK -> many, see the macro for options...
 *
 * Possible Extension:
 *  - Integrate units into intervals (useful?)
 */
#ifndef __TIMER_HEADER_GUARD__
#define __TIMER_HEADER_GUARD__

#if __cplusplus
extern "C" {
#endif

#ifndef TIMERVER
/* Verbosity level, the following values are considered valid:
 * - 0 : verbosity off
 * - 1 : error messages
 * - 2 : debug
 */
#define TIMERVER 1
#endif

#ifndef TIMERUNIT
/* Based upon SI units, the following values are considered valid:
 * - 0 : seconds 
 * - 1 : milli-seconds
 * - 2 : micro-seconds
 * - 3 : nano-seconds
 */
#warning TIMEUNIT set to nano-seconds (ns)
#define TIMERUNIT 3
#endif

#ifndef TIMERCLOCK
/* The following values are considered valid (descriptions from the man
 * page of clock_getres(2)):
 * - 0 : CLOCK_REALTIME
 *       System-wide clock that measures real (i.e., wall-clock) time. Setting
 *       this clock requires appropriate privileges. This clock is affected by
 *       discontinuous jumps in the system time (e.g., if the system
 *       administrator manually changes the clock), and by the incremental
 *       adjustments performed by adjtime(3) and NTP. 
 * - 1 : CLOCK_REALTIME_COARSE   (Linux only!)
 *       A faster but less precise version of CLOCK_REALTIME. Use when you
 *       need very fast, but not fine-grained timestamps. 
 * - 2 : CLOCK_MONOTONIC         (Linux only!)
 *       Clock that cannot be set and represents monotonic time since some
 *       unspecified starting point. This clock is not affected by
 *       discontinuous jumps in the system time (e.g., if the system
 *       administrator manually changes the clock), but is affected by the
 *       incremental adjustments performed by adjtime(3) and NTP. 
 * - 3 : CLOCK_MONOTONIC_COARSE  (Linux only!)
 *       A faster but less precise version of CLOCK_MONOTONIC. Use when you
 *       need very fast, but not fine-grained timestamps. 
 * - 4 : CLOCK_MONOTONIC_RAW     (Linux only!)
 *       Similar to CLOCK_MONOTONIC, but provides access to a raw
 *       hardware-based time that is not subject to NTP adjustments or the
 *       incremental adjustments performed by adjtime(3).
 * - 5 : CLOCK_BOOTTIME          (Linux only!)
 *       Identical to CLOCK_MONOTONIC, except it also includes any time that
 *       the system is suspended. This allows applications to get a
 *       suspend-aware monotonic clock without having to deal with the
 *       complications of CLOCK_REALTIME, which may have discontinuities if
 *       the time is changed using settimeofday(2). 
 * - 6 : CLOCK_PROCESS_CPUTIME_ID
 *       High-resolution per-process timer from the CPU. 
 * - 7 : CLOCK_THREAD_CPUTIME_ID
 *       Thread-specific CPU-time clock. 
 */
#warning TIMERCLOCK set to CLOCK_MONOTONIC
#define TIMERCLOCK 2
#endif

/** Macros for verbosity **/

#if TIMERVER > 0
#define ERROR(message, ...) \
    fprintf(stderr, \
            " [ERROR] Timer: (%s:%d) " message "\n" \
            , __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define ERROR(message, ...)
#endif

#if TIMERVER > 1
#define DEBUG(message, ...) \
    fprintf(stderr, \
            " [DEBUG] Timer: (%s:%d) " message "\n" \
            , __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define DEBUG(message, ...)
#endif

#define CHECK(cond, message, ...) \
    if((cond)) { ERROR(message, ##__VA_ARGS__); goto error; }

/** Error related **/

#define OK 0
#define NOT_ALLOCATED -1
#define CLOCK_FAILED -2

/** Time conversions **/

#define NANO_TO_SEC(time) (time / 1000000000.0)
#define NANO_TO_MSEC(time) (time / 1000000.0)
#define NANO_TO_MCSEC(time) (time / 1000.0)

#define MICRO_TO_SEC(time) (time / 1000000.0)
#define MICRO_TO_MSEC(time) (time / 1000.0)
#define MICRO_TO_NSEC(time) (time * 1000.0)

#define MILLI_TO_SEC(time) (time / 1000.0)
#define MILLI_TO_MCSEC(time) (time * 1000.0)
#define MILLI_TO_NSEC(time) (time * 1000000.0)

#define SEC_TO_MSEC(time) (time * 1000.0)
#define SEC_TO_MCSEC(time) (time * 1000000.0)
#define SEC_TO_NSEC(time) (time * 1000000000.0)

/** Global types **/

/* Datatype
 *  struct interval -> 
 *   - name -> string
 *   - start -> struct timespec
 *   - stop -> struct timespec
 */
typedef struct
{
    char * name;
    struct timespec start;
    struct timespec stop;
} interval;

/** Declarations **/

char * error_num(int status);
clockid_t set_clock();
struct timespec diff_timespec(struct timespec a, struct timespec b);
int set_timespec(struct timespec ** tims, long sec, long nsec);
char * unit();
int create_interval(interval ** tmp, char * name);
int start(interval * tmp);
int stop(interval * tmp);
double elapsed_interval(interval * tmp);
void print_results(int num, ...);

#if __cplusplus
}
#endif

#endif
