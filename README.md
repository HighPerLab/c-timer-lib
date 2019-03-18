C Timer Header
--------------

Created for a research project to simplify the process of measuring runtimes.

This library encapsulates the `clock_gettime` function and provides a
timing structure to measure time intervals. A few nice functions are provided
that can start and stop the timer and return the elapsed time in units of
seconds, milliseconds, microseconds, and nanoseconds.

The developer also has the option to select the system clock to be used and
also activate debugging facilities.

## Note for GCC <= 4.4

For use with GCC <= 4.4, remember to compile with `-std=gnu99` or there will be
errors regarding using the `clock_gettime` function.

# License

This work is unlicensed and part of the public domain, as per
(The Unlicense)[https://unlicense.org/] license.
