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
 */
#ifndef __TIMER_HEADER_GUARD__
#define __TIMER_HEADER_GUARD__

#if __cplusplus
extern "C" {
#endif

/** Global types **/

/* Enum of time units */
typedef enum
{
    s,   // seconds
    ms,  // milli-seconds
    us,  // micro-seconds
    ns,  // nano-seconds
    unit_check, // used for enum check
    none // use the intervals unit
} unit_e;

/* Enum of system clocks */
typedef enum
{
    rt,
 /* - CLOCK_REALTIME
  *       System-wide clock that measures real (i.e., wall-clock) time. Setting
  *       this clock requires appropriate privileges. This clock is affected by
  *       discontinuous jumps in the system time (e.g., if the system
  *       administrator manually changes the clock), and by the incremental
  *       adjustments performed by adjtime(3) and NTP.
  */
    rtc,
 /* - CLOCK_REALTIME_COARSE   (Linux only!)
  *       A faster but less precise version of CLOCK_REALTIME. Use when you
  *       need very fast, but not fine-grained timestamps. 
  */
    mono,
 /* - CLOCK_MONOTONIC         (Linux only!)
  *       Clock that cannot be set and represents monotonic time since some
  *       unspecified starting point. This clock is not affected by
  *       discontinuous jumps in the system time (e.g., if the system
  *       administrator manually changes the clock), but is affected by the
  *       incremental adjustments performed by adjtime(3) and NTP.
  */
    monoc,
 /* - CLOCK_MONOTONIC_COARSE  (Linux only!)
  *       A faster but less precise version of CLOCK_MONOTONIC. Use when you
  *       need very fast, but not fine-grained timestamps. 
  */
    monor,
 /* - CLOCK_MONOTONIC_RAW     (Linux only!)
  *       Similar to CLOCK_MONOTONIC, but provides access to a raw
  *       hardware-based time that is not subject to NTP adjustments or the
  *       incremental adjustments performed by adjtime(3).
  */
    monob,
 /* - CLOCK_BOOTTIME          (Linux only!)
  *       Identical to CLOCK_MONOTONIC, except it also includes any time that
  *       the system is suspended. This allows applications to get a
  *       suspend-aware monotonic clock without having to deal with the
  *       complications of CLOCK_REALTIME, which may have discontinuities if
  *       the time is changed using settimeofday(2).
  */
    cpup,
 /* - CLOCK_PROCESS_CPUTIME_ID
  *       High-resolution per-process timer from the CPU.
  */
    cput
 /* - CLOCK_THREAD_CPUTIME_ID
  *       Thread-specific CPU-time clock. 
  */
} clock_e;

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
    struct timespec elapsed;
    clockid_t clockid;
    unit_e unit;
    clock_e clock;
} interval_t;

/** Declarations **/

char * error_num(int status);
clockid_t set_clock();
struct timespec diff_timespec(struct timespec a, struct timespec b);
int set_timespec(struct timespec ** tims, long sec, long nsec);
char * print_unit(unit_e unit);
int create_interval(interval_t ** tmp, char * name, clock_e ck, unit_e ut);
int get_time(clockid_t clock, struct timespec * time);
int start(interval_t * tmp);
int stop(interval_t * tmp);
double elapsed_interval(interval_t * tmp, unit_e ut);
void print_results(int num, ...);
void print_results_csv(char * comment, int num, ...);

#if __cplusplus
}
#endif

#endif
