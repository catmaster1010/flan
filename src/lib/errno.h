#ifndef errno_h
#define errno_h

#include <errno.h>

#define errno (get_current_thread()->errno)

#endif
