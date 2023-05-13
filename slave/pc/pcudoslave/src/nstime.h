/*
 *  file:     nstime.h
 *  brief:    simple 64-bit nanosecond timer for Linux and Windows
 *  created:  2023-05-13
 *  authors:  nvitya
 *  license:  public domain
*/

#ifndef __NSTIME_H_
#define __NSTIME_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define ONE_MICROSEC  1000
#define ONE_MILLISEC  1000000
#define ONE_SEC       1000000000

typedef long long  t_nstime;

#define nstime    nstime_sys

extern t_nstime   nstime_sys(void);
extern void       init_nstimer(t_nstime astarttime);

extern void       waitns(t_nstime wns);
extern void       ns_sleep_until(t_nstime wakeuptime);

#ifdef __cplusplus
}
#endif
#endif // __NSTIME_H_
