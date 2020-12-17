#include <time.h>

struct rusage {
  struct timeval ru_utime;	/* user time used */
  struct timeval ru_stime;	/* system time used */
  long ru_maxrss;		/* integral max resident set size */
  long ru_ixrss;		/* integral shared text memory size */
  long ru_idrss;		/* integral unshared data size */
  long ru_isrss;		/* integral unshared stack size */
  long ru_minflt;		/* page reclaims */
  long ru_majflt;		/* page faults */
  long ru_nswap;		/* swaps */
  long ru_inblock;		/* block input operations */
  long ru_oublock;		/* block output operations */
  long ru_msgsnd;		/* messages sent */
  long ru_msgrcv;		/* messages received */
  long ru_nsignals;		/* signals received */
  long ru_nvcsw;		/* voluntary context switches */
  long ru_nivcsw;		/* involuntary context switches */
};

/// STUB ///
int getrusage(int _who, struct rusage *_rusage) {};
