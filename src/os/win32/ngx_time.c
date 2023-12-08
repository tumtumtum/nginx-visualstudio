
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>


void
ngx_gettimeofday(struct timeval *tp)
{
    uint64_t  intervals;
    FILETIME  ft;

    GetSystemTimeAsFileTime(&ft);

    /*
     * A file time is a 64-bit value that represents the number
     * of 100-nanosecond intervals that have elapsed since
     * January 1, 1601 12:00 A.M. UTC.
     *
     * Between January 1, 1970 (Epoch) and January 1, 1601 there were
     * 134744 days,
     * 11644473600 seconds or
     * 11644473600,000,000,0 100-nanosecond intervals.
     *
     * See also MSKB Q167296.
     */
    /*
    * 时间换算
    1s【秒】 = 1000ms【毫秒】
  1ms【毫秒】 = 1000μs【微秒】
    1μs【微秒】 = 1000ns【纳秒】
  1ns 【纳秒】= 1000ps【皮秒】

    /*
    windows有很多关于时间的函数都是使用FILETIME结构，这个结构代表了从1601年1月1日开始到现在的100纳秒间隔是多少。而其他平台则更多使用unix时间轴，从1970年1月1日00:00:00到现在的经过的秒数。
    当需要转换两个时间戳时,你需要的就是： 116444736000000000:
    1s = 1 000 000 000 ns = 10 000 000 * 100 ns  
    1s内就有10 000 000个100毫微秒间隔。
    [10000000s *60 * 60 ] h * 24 * 【365 [days] * (1970-1601)[gap] + 89[leap days 闰年的时间]】 =  116444736000000000ns

    */
    intervals = ((uint64_t) ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    intervals -= 116444736000000000;

    tp->tv_sec = (long) (intervals / 10000000);
    tp->tv_usec = (long) ((intervals % 10000000) / 10);
}


void
ngx_libc_localtime(time_t s, struct tm *tm)
{
    struct tm  *t;

    t = localtime(&s);
    *tm = *t;
}


void
ngx_libc_gmtime(time_t s, struct tm *tm)
{
    struct tm  *t;

    t = gmtime(&s);
    *tm = *t;
}


ngx_int_t
ngx_gettimezone(void)
{
    u_long                 n;
    TIME_ZONE_INFORMATION  tz;

    n = GetTimeZoneInformation(&tz);

    switch (n) {

    case TIME_ZONE_ID_UNKNOWN:
        return -tz.Bias;

    case TIME_ZONE_ID_STANDARD:
        return -(tz.Bias + tz.StandardBias);

    case TIME_ZONE_ID_DAYLIGHT:
        return -(tz.Bias + tz.DaylightBias);

    default: /* TIME_ZONE_ID_INVALID */
        return 0;
    }
}
