/****************************************************************************
* AC Mud written by Michael "borlaK" Morrison,            borlaK@borlak.org *
*                                                                           *
* Read ../doc/licence.txt for the terms of use.  One of the terms of use is *
* not to remove these headers.                                              *
****************************************************************************/

/*
operating system dependant stuff
*/
#if defined(WIN32)
#define vsnprintf	_vsnprintf
#define FD_SETSIZE 64		// this is the max # of sockets that can connect!
#include <winsock2.h>
#include <sys/timeb.h>
#include <stdio.h>
#endif /*  */

#if !defined(WIN32)
#include <sys/time.h>
#include <pthread.h>
#include <netinet/in.h>
#endif /*  */
