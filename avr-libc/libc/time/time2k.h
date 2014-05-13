/*
 * (C)2012 Michael Duane Rice All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer. Redistributions in binary
 * form must reproduce the above copyright notice, this list of conditions
 * and the following disclaimer in the documentation and/or other materials
 * provided with the distribution. Neither the name of the copyright holders
 * nor the names of contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* $Id$ */

/** \file */

/**    \defgroup avr_time <time.h>: Time
    \code #include <time.h> \endcode
    <h3>Introduction to the Time functions</h3>
    This file declares the time functions implemented in \c avr-libc.

    The implementation aspires to conform with ISO/IEC 9899 (C90). However, due to limitations of the
    target processor and the nature of its development environment, a practical implementation must
    of necessity deviate from the standard.



    Section 7.23.2.1 clock()
    The type clock_t, the macro CLOCKS_PER_SEC, and the function clock() are not implemented. We
    consider these items belong to operating system code, or to application code when no operating
    system is present.

    Section 7.23.2.3 mktime()
    The standard specifies that mktime() should return (time2k_t) -1, if the time cannot be represented.
    This implementation always returns a 'best effort' representation.

    Section 7.23.2.4 time()
    The standard specifies that time() should return (time2k_t) -1, if the time is not available.
    Since the application must initialize the time system, this functionality is not implemented.

    Section 7.23.2.2, difftime()
    Due to the lack of a 64 bit double, the function difftime() returns a long integer. In most cases
    this change will be invisible to the user, handled automatically by the compiler.

    Section 7.23.1.4 struct tm
    Per the standard, struct tm->tm_isdst is greater than zero when Daylight Saving time is in effect.
    This implementation further specifies that, when positive, the value of tm_isdst represents
    the amount time is advanced during Daylight Saving time.

    Section 7.23.3.5 strftime()
    Only the 'C' locale is supported, therefore the modifiers 'E' and 'O' are ignored.
    The 'Z' conversion is also ignored, due to the lack of time zone name.

    In addition to the above departures from the standard, there are some behaviors which are different
    from what is often expected, though allowed under the standard.

    There is no 'platform standard' method to obtain the current time, time zone, or
    daylight savings 'rules' in the AVR environment. Therefore the application must initialize
    the time system with this information. The functions set_zone(), set_dst(), and
    set_system_time() are provided for initialization. Once initialized, system time is maintained by
    calling the function system_tick() at one second intervals.

    Though not specified in the standard, it is often expected that time2k_t is a signed integer
    representing an offset in seconds from Midnight Jan 1 1970... i.e. 'Unix time'. This implementation
    uses an unsigned 32 bit integer offset from Midnight Jan 1 2000. The use of this 'epoch' helps to
    simplify the conversion functions, while the 32 bit value allows time to be properly represented
    until Tue Feb 7 06:28:15 2136 UTC. The macros UNIX_OFFSET and NTP_OFFSET are defined to assist in
    converting to and from Unix and NTP time stamps.

    Unlike desktop counterparts, it is impractical to implement or maintain the 'zoneinfo' database.
    Therefore no attempt is made to account for time zone, daylight saving, or leap seconds in past dates.
    All calculations are made according to the currently configured time zone and daylight saving 'rule'.

    In addition to C standard functions, re-entrant versions of ctime(), asctime(), gmtime() and
    localtime() are provided which, in addition to being re-entrant, have the property of claiming
    less permanent storage in RAM. An additional time conversion, isotime() and its re-entrant version,
    uses far less storage than either ctime() or asctime().

    Along with the usual smattering of utility functions, such as is_leap_year(), this library includes
    a set of functions related the sun and moon, as well as sidereal time functions.
*/

#ifndef TIME2K_H
#define TIME2K_H

#ifdef __cplusplus
extern          "C" {
#endif

#include <inttypes.h>
#include <stdlib.h>
#include <time.h>

    /** \ingroup avr_time */
    /* @{ */

    /**
        time2k_t represents seconds elapsed from Midnight, Jan 1 2000 UTC (the Y2K 'epoch').
        Its range allows this implementation to represent time up to Tue Feb 7 06:28:15 2136 UTC.
    */
    typedef uint32_t time2k_t;

    /**
    The time function returns the systems current time stamp.
    If timer is not a null pointer, the return value is also assigned to the object it points to.
    */
    time2k_t          time2k(time2k_t *timer);

    /**
    The difftime function returns the difference between two binary time stamps,
    time1 - time0.
    */
    int32_t         difftime(time2k_t time1, time2k_t time0);

    /**
    This function 'compiles' the elements of a broken-down time structure, returning a binary time stamp.
    The elements of timeptr are interpreted as representing Local Time.

    The original values of the tm_wday and tm_yday elements of the structure are ignored,
    and the original values of the other elements are not restricted to the ranges stated for struct tm.

    On successful completion, the values of all elements of timeptr are set to the appropriate range.
    */
    time2k_t          mktime2k(struct tm * timeptr);

    /**
    This function 'compiles' the elements of a broken-down time structure, returning a binary time stamp.
    The elements of timeptr are interpreted as representing UTC.

    The original values of the tm_wday and tm_yday elements of the structure are ignored,
    and the original values of the other elements are not restricted to the ranges stated for struct tm.

    Unlike mktime(), this function DOES NOT modify the elements of timeptr.
    */
    time2k_t          mk_gmtime2k(const struct tm * timeptr);

    /**
    The gmtime function converts the time stamp pointed to by timer into broken-down time,
    expressed as UTC.
    */
    struct tm      *gmtime2k(const time2k_t * timer);

    /**
        Re entrant version of gmtime().
    */
    void            gmtime2k_r(const time2k_t * timer, struct tm * timeptr);

    /**
    The localtime function converts the time stamp pointed to by timer into broken-down time,
    expressed as Local time.
    */
    struct tm      *localtime2k(const time2k_t * timer);

    /**
        Re entrant version of localtime().
    */
    void            localtime2k_r(const time2k_t * timer, struct tm * timeptr);


    /**
        The ctime function is equivalent to asctime(localtime(timer))
    */
    char           *ctime2k(const time2k_t * timer);

    /**
        Re entrant version of ctime().
    */
    void            ctime2k_r(const time2k_t * timer, char *buf);

    /**
        Specify the Daylight Saving function.

        The Daylight Saving function should examine its parameters to determine whether
        Daylight Saving is in effect, and return a value appropriate for tm_isdst.

        Working examples for the USA and the EU are available..

            \code #include <util/eu_dst.h>\endcode
            for the European Union, and
            \code #include <util/usa_dst.h>\endcode
            for the United States

        If a Daylight Saving function is not specified, the system will ignore Daylight Saving.
    */
    void            set_dst2k(int (*) (const time2k_t *, int32_t *));

    /**
        Initialize the system time. Examples are...

        From a Clock / Calendar type RTC:
        \code
        struct tm rtc_time;

        read_rtc(&rtc_time);
        rtc_time.tm_isdst = 0;
        set_system_time( mktime(&rtc_time) );
        \endcode

        From a Network Time Protocol time stamp:
        \code
        set_system_time(ntp_timestamp - NTP_OFFSET);
        \endcode

        From a UNIX time stamp:
         \code
        set_system_time(unix_timestamp - UNIX_OFFSET);
        \endcode

    */
    void            set_system_time(time2k_t timestamp);


    /**
        Computes the difference between apparent solar time and mean solar time.
        The returned value is in seconds.
    */
    int16_t         equation_of_time2k(const time2k_t * timer);

    /**
        Computes the amount of time the sun is above the horizon, at the location of the observer.

        NOTE: At observer locations inside a polar circle, this value can be zero during the winter,
        and can exceed ONE_DAY during the summer.

        The returned value is in seconds.
    */
    int32_t         daylight_seconds2k(const time2k_t * timer);

    /**
        Computes the time of solar noon, at the location of the observer.
    */
    time2k_t          solar_noon2k(const time2k_t * timer);

    /**
        Return the time of sunrise, at the location of the observer. See the note about daylight_seconds().
    */
    time2k_t          sun_rise2k(const time2k_t * timer);

    /**
        Return the time of sunset, at the location of the observer. See the note about daylight_seconds().
    */
    time2k_t          sun_set2k(const time2k_t * timer);

    /** Returns the declination of the sun in radians. */
    double          solar_declination2k(const time2k_t * timer);

    /**
        Returns an approximation to the phase of the moon.
        The sign of the returned value indicates a waning or waxing phase.
        The magnitude of the returned value indicates the percentage illumination.
    */
    int8_t          moon_phase2k(const time2k_t * timer);

    /**
        Returns Greenwich Mean Sidereal Time, as seconds into the sidereal day.
        The returned value will range from 0 through 86399 seconds.
    */
    unsigned long   gm_sidereal2k(const time2k_t * timer);

    /**
        Returns Local Mean Sidereal Time, as seconds into the sidereal day.
        The returned value will range from 0 through 86399 seconds.
    */
    unsigned long   lm_sidereal2k(const time2k_t * timer);

    /* @} */
#ifdef __cplusplus
}
#endif

#endif              /* TIME_H  */
