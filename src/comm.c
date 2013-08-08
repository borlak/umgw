/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Thanks to abaddon for proof-reading our comm.c and pointing out bugs.  *
 *  Any remaining bugs are, of course, our work, not his.  :)              *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/*
 * This file contains all of the OS-dependent stuff:
 *   startup, signals, BSD sockets for tcp/ip, i/o, timing.
 *
 * The data flow for input is:
 *
 * The data flow for output is:
 *
 * The OS-dependent functions are Read_from_descriptor and Write_to_descriptor.
 * -- Furey  26 Jan 1993
 */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include "merc.h"

#ifndef WIN32
#include <unistd.h>
#endif

/*
 * Malloc debugging stuff.
 */
#if defined(MALLOC_DEBUG)
#include <malloc.h>
extern long malloc_debug args((long));
extern long malloc_verify args((void));
#endif

/*
 * Signal handling.
 * Apollo has a problem with __attribute(atomic) in signal.h,
 *   I dance around it.
 */
#include <signal.h>
#ifndef WIN32
#include <arpa/inet.h>
#include <sys/resource.h>	/* for RLIMIT_NOFILE */
#include <netdb.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/telnet.h>
const char echo_off_str[] = { IAC, WILL, TELOPT_ECHO, '\0' };
const char echo_on_str[] = { IAC, WONT, TELOPT_ECHO, '\0' };
const char go_ahead_str[] = { IAC, GA, '\0' };
#else
#include <sys/types.h>
#include <fcntl.h>
#include <ctype.h>
const char echo_off_str[] = { '\0' };
const char echo_on_str[] = { '\0' };
const char go_ahead_str[] = { '\0' };
#endif


/*
 * Global variables.
 */
DESCRIPTOR_DATA *descriptor_free;	/* Free list for descriptors    */
DESCRIPTOR_DATA *descriptor_list;	/* All open descriptors         */
DESCRIPTOR_DATA *d_next;	/* Next descriptor in loop      */
FILE *fpReserve;		/* Reserved file handle         */
bool god;			/* All new chars are gods!      */
bool merc_down;			/* Shutdown                     */
bool wizlock;			/* Game is wizlocked            */
long reboot_time = -1;		/* take a guess         */
char str_boot_time[MAX_INPUT_LENGTH];
char crypt_pwd[MAX_INPUT_LENGTH];
time_t current_time;		/* Time of this pulse           */
struct timeval boot_time;
extern long hackified;
extern long crazy;


/* Colour scale char list - Calamar */
char *scale[SCALE_COLS] = {
	L_RED,
	L_BLUE,
	L_GREEN,
	YELLOW
};

void game_loop_unix args((long control));
long init_socket args((long port));
void new_descriptor args((long control));
bool read_from_descriptor args((DESCRIPTOR_DATA * d));
bool write_to_descriptor args((long desc, char *txt, long length));


/*
 * Other local functions (OS-independent).
 */
bool check_reconnect args((DESCRIPTOR_DATA * d, char *name, bool fConn));
bool check_kickoff args((DESCRIPTOR_DATA * d, char *name, bool fConn));
bool check_playing args((DESCRIPTOR_DATA * d, char *name));
int main args((int argc, char **argv));
void nanny args((DESCRIPTOR_DATA * d, char *argument));
bool process_output args((DESCRIPTOR_DATA * d, bool fPrompt));
void read_from_buffer args((DESCRIPTOR_DATA * d));
void stop_idling args((CHAR_DATA * ch));
void bust_a_prompt args((DESCRIPTOR_DATA * d));
void check_cheater args((CHAR_DATA * ch));

long control;
long port;
long rebootnum;
long cpu_update = 0;
long need_update;

void exit_handler(int signo)
{
	static struct timeval last_signal_time;
	struct timeval time;

	get_time(&time);

	switch (signo)
	{
	case SIGINT:
	case SIGILL:
	case SIGFPE:
	case SIGSEGV:
#ifndef WIN32
	case SIGPIPE:
	case SIGTRAP:
#endif
		do_hotreboot(char_list, "CRASH!!!");
		break;
#ifndef WIN32
	case SIGVTALRM:	// check if the mud is looping
		if(last_signal_time.tv_sec > 0 && (time.tv_sec - last_signal_time.tv_sec) < 15)
		{
			log_string("Mud in infinite loop, rebooting!");
			do_hotreboot(char_list, "INFINITE LOOP!");
		}
		break;
#endif
	}
	get_time(&last_signal_time);
}


int main(int argc, char **argv)
{
	struct timeval now_time;
	bool fhotreboot = FALSE;

#ifndef WIN32
	struct itimerval itime;
	struct timeval time;
#endif

	/*
	 * Memory debugging if needed.
	 */
#if defined(MALLOC_DEBUG)
	malloc_debug(2);
#endif

#ifdef RLIMIT_NOFILE
#ifndef min
# define min(a,b)     (((a) < (b)) ? (a) : (b))
#endif
	{
		struct rlimit rlp;

		(void) getrlimit(RLIMIT_NOFILE, &rlp);
		rlp.rlim_cur = min(rlp.rlim_max, FD_SETSIZE);
		(void) setrlimit(RLIMIT_NOFILE, &rlp);
	}
#endif

	/*
	 * Init time and encryption.
	 */
	get_time(&now_time);
	current_time = (time_t) now_time.tv_sec;
	strcpy(str_boot_time, ctime(&current_time));
	strcpy(crypt_pwd, "Don't bother.");

	if((fpReserve = fopen(NULL_FILE, "r")) == 0)
	{
		perror(NULL_FILE);
		exit(1);
	}

	/*
	 * Get the port number.
	 */
	port = 5666;
	if(argc > 1)
	{
		if(!is_number(argv[1]))
		{
			fprintf(stderr, "Usage: %s [port #]\n", argv[0]);
			exit(1);
		}
		else if((port = atoi(argv[1])) <= 1024)
		{
			fprintf(stderr, "Port number must be above 1024.\n");
			exit(1);
		}

		if(argv[2] && argv[2][0])
		{
			fhotreboot = TRUE;
			control = atoi(argv[3]);
			rebootnum = atoi(argv[4]);
			if(++rebootnum == 200)
				exit(1);
		}
		else
		{
			rebootnum = 0;
			fhotreboot = FALSE;
		}
	}

/* signal handlers .. borlak */
	signal(SIGINT, exit_handler);
	signal(SIGSEGV, exit_handler);
	signal(SIGFPE, exit_handler);
	signal(SIGILL, exit_handler);
#ifndef WIN32
	signal(SIGTRAP, exit_handler);
	signal(SIGPIPE, exit_handler);
	signal(SIGVTALRM, exit_handler);
// infinite loop checking
	time.tv_sec = 5;
	time.tv_usec = 0;
	itime.it_interval = time;
	itime.it_value = time;
	if(setitimer(ITIMER_VIRTUAL, &itime, 0) < 0)
		log_string("Error starting setitimer.");
#endif

	/*
	 * Run the game.
	 */
#ifdef WIN32
	{
		WORD wVersionRequested;
		WSADATA wsaData;

		wVersionRequested = MAKEWORD(1, 1);

		if((WSAStartup(wVersionRequested, &wsaData)) != 0)
		{
			perror("error starting winsock");
			exit(1);
		}

		if(LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
		{
			perror("No suitable winsock DLL found, aborting.");
			exit(1);
			return 0;
		}
	}
#endif

	if(!fhotreboot)
		control = init_socket(port);
	boot_db(fhotreboot);

	sprintf(log_buf, "Godwars is ready to rock on port %li.", port);
	log_string(log_buf);


	game_loop_unix(control);
#ifndef WIN32
	close(control);
#else
	closesocket(control);
#endif
	write_kingdoms();
	/*
	 * That's all, folks.
	 */
	log_string("Normal termination of game.");
	exit(0);
	return 0;
}


long init_socket(long port)
{
	static struct sockaddr_in sa_zero;
	struct sockaddr_in sa;
	long x;
	long fd;

	if((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Init_socket: socket");
		exit(1);
	}

	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &x, sizeof(x)) < 0)
	{
		perror("Init_socket: SO_REUSEADDR");
#ifndef WIN32
		exit(1);
#endif
	}
/*
#if defined(SO_DONTLINGER) && !defined(SYSV)
    {
	struct	linger	ld;


	ld.l_onoff  = 1;
	ld.l_linger = 1000;

	if ( setsockopt( fd, SOL_SOCKET, SO_DONTLINGER,
	(char *) &ld, sizeof(ld) ) < 0 )
	{
	    perror( "Init_socket: SO_DONTLINGER" );
	    close( fd );
	    exit( 1 );
	}
    }
#endif
*/
	sa = sa_zero;
	sa.sin_family = AF_INET;
	sa.sin_port = htons((short) port);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(fd, (struct sockaddr *) &sa, sizeof(sa)) < 0)
	{
		perror("Init_socket: bind");
#ifndef WIN32
		close(fd);
#else
		closesocket(fd);
#endif
		exit(1);
	}

	if(listen(fd, 3) < 0)
	{
		perror("Init_socket: listen");
#ifndef WIN32
		close(fd);
#else
		closesocket(fd);
#endif
		exit(1);
	}

	return fd;
}

void excessive_cpu(long blx)
{
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;

	for(vch = char_list; vch != 0; vch = vch_next)
	{
		vch_next = vch->next;

		if(!IS_NPC(vch))
		{
			send_to_char("Mud frozen: Autosave and quit.  The mud will reboot in 2 seconds.\n\r", vch);
			interpret(vch, "quit");
		}
	}
	exit(1);
}

void game_loop_unix(long control)
{
	static struct timeval null_time;
	struct timeval last_time;
	unsigned long stall_socket;
	fd_set stall_set;

//DESCRIPTOR_DATA *d;
#ifndef WIN32
	signal(SIGPIPE, SIG_IGN);
#endif
	get_time(&last_time);
	get_time(&boot_time);
	current_time = (time_t) last_time.tv_sec;

	stall_socket = socket(AF_INET, SOCK_STREAM, 0);

	/* Main loop */
	while(!merc_down)
	{
		fd_set in_set;
		fd_set out_set;
		fd_set exc_set;
		DESCRIPTOR_DATA *d;
		long maxdesc;

#if defined(MALLOC_DEBUG)
		if(malloc_verify() != 1)
			abort();
#endif

		/* Update the CPU checker */
		need_update = 2;

		/*
		 * Poll all active descriptors.
		 */
		FD_ZERO(&in_set);
		FD_ZERO(&out_set);
		FD_ZERO(&exc_set);
		FD_SET((unsigned long) control, &in_set);
		maxdesc = control;
/* kavirpoint
	maxdesc	= control * 2;
*/
		for(d = descriptor_list; d != 0; d = d->next)
		{
			maxdesc = UMAX(maxdesc, d->descriptor);
			FD_SET((unsigned long) d->descriptor, &in_set);
			FD_SET((unsigned long) d->descriptor, &out_set);
			FD_SET((unsigned long) d->descriptor, &exc_set);
		}

		null_time.tv_sec = 0;
		null_time.tv_usec = 0;
		if(select(maxdesc + 1, &in_set, &out_set, &exc_set, &null_time) < 0)
		{
			perror("Game_loop: select: poll");
			exit(1);
		}

		/*
		 * New connection?
		 */
		if(FD_ISSET(control, &in_set))
			new_descriptor(control);

		/*
		 * Kick out the freaky folks.
		 */
		for(d = descriptor_list; d != 0; d = d_next)
		{
			d_next = d->next;
			if(FD_ISSET(d->descriptor, &exc_set))
			{
				FD_CLR((unsigned long) d->descriptor, &in_set);
				FD_CLR((unsigned long) d->descriptor, &out_set);
				if(d->character)
					save_char_obj(d->character);
				d->outtop = 0;
				close_socket2(d, FALSE);
			}
			else
			{
				if(d->connected != CON_PLAYING)
				{
					if(++d->login_time > (PULSE_PER_SECOND * 60 * 3))
					{
						sprintf(log_buf, "%s@%s took too long to login, closing connection.",
							d->character ? d->character->name : "Someone",
							d->host ? d->host : "Somewhere");
						log_string(log_buf);
						write_to_descriptor(d->descriptor,
								    "\n\r\n\rYou have taken too long to login, good-bye.\n\r",
								    0);
						if(d->character)
							save_char_obj(d->character);
						d->outtop = 0;
						close_socket2(d, FALSE);
					}

					if(++d->wait > PULSE_PER_SECOND * 25)
					{
						sprintf(log_buf, "%s@%s was idle too long, closing connection.",
							d->character ? d->character->name : "Someone",
							d->host ? d->host : "Somewhere");
/* why we have this anyway? beats me
						if (str_cmp(d->host,"63.11.161.72") && str_cmp(d->host,"172.146.87.230") && 
str_cmp(d->host,"24.181.130.223")) 
								// put login script here.
							log_string(log_buf);
*/					
	write_to_descriptor(d->descriptor,
								    "\n\r\n\rYou have been idle too long, good-bye.\n\r",
								    0);
						if(d->character)
							save_char_obj(d->character);
						d->outtop = 0;
						close_socket2(d, FALSE);
					}
					else if(d->wait == PULSE_PER_SECOND * 15)
					{
						write_to_descriptor(d->descriptor,
								    "\n\rYou have been idle and will automatically be disconnected in 10 seconds.\n\r",
								    0);
					}
				}
			}
		}

		/*
		 * Process input.
		 */
		for(d = descriptor_list; d != 0; d = d_next)
		{
			d_next = d->next;
			d->fcommand = FALSE;

			if(FD_ISSET(d->descriptor, &in_set))
			{
				if(d->character != 0)
					d->character->timer = 0;
				if(!read_from_descriptor(d))
				{
					FD_CLR((unsigned long) d->descriptor, &out_set);
					if(d->character != 0)
						save_char_obj(d->character);
					d->outtop = 0;
					close_socket2(d, FALSE);
					continue;
				}
			}

			if(d->character != 0 && d->character->wait > 0)
			{
				--d->character->wait;
				continue;
			}

			read_from_buffer(d);
			if(d->incomm[0] != '\0')
			{
				d->fcommand = TRUE;
				d->wait = 0;
				stop_idling(d->character);

				if(d->connected == CON_PLAYING)
					interpret(d->character, d->incomm);
				else
					nanny(d, d->incomm);

				d->incomm[0] = '\0';
			}
		}



		/*
		 * Autonomous game motion.
		 */
		update_handler();



		/*
		 * Output.
		 */
		for(d = descriptor_list; d != 0; d = d_next)
		{
			d_next = d->next;

			if((d->fcommand || d->outtop > 0) && FD_ISSET(d->descriptor, &out_set))
			{
				if(!process_output(d, TRUE))
				{
					if(d->character != 0)
						save_char_obj(d->character);
					d->outtop = 0;
					close_socket2(d, FALSE);
				}
			}
		}



		/*
		 * Synchronize to a clock.
		 * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
		 * Careful here of signed versus unsigned arithmetic.
		 */
		{
			struct timeval now_time;
			long secDelta;
			long usecDelta;

			get_time(&now_time);
			usecDelta = ((long) last_time.tv_usec) - ((long) now_time.tv_usec) + 1000000 / PULSE_PER_SECOND;
			secDelta = ((long) last_time.tv_sec) - ((long) now_time.tv_sec);
			while(usecDelta < 0)
			{
				usecDelta += 1000000;
				secDelta -= 1;
			}

			while(usecDelta >= 1000000)
			{
				usecDelta -= 1000000;
				secDelta += 1;
			}

			if(secDelta > 0 || (secDelta == 0 && usecDelta > 0))
			{
				struct timeval stall_time;

				stall_time.tv_usec = usecDelta;
				stall_time.tv_sec = secDelta;
				FD_ZERO(&stall_set);
				FD_SET(stall_socket, &stall_set);
				if(select(0, 0, 0, &stall_set, &stall_time) < 0)
				{
					perror("Game_loop: select: stall");
					exit(1);
				}
			}
		}

		get_time(&last_time);
		current_time = (time_t) last_time.tv_sec;

		/* check for automatic reboot
		   if( reboot_time > -1 )
		   {
		   if(char_list)
		   {
		   if (reboot_time / 60 == 5)
		   do_echo(char_list,"REBOOT COUNTDOWN: 5 minutes left.\n\r");
		   if (reboot_time / 60 == 4)
		   do_echo(char_list,"REBOOT COUNTDOWN: 4 minutes left.\n\r");
		   if (reboot_time / 60 == 3)
		   do_echo(char_list,"REBOOT COUNTDOWN: 3 minutes left.\n\r");
		   if (reboot_time / 60 == 2)
		   do_echo(char_list,"REBOOT COUNTDOWN: 2 minutes left.\n\r");
		   if (reboot_time / 60 == 1)
		   do_echo(char_list,"REBOOT COUNTDOWN: 1 minute left.\n\r");
		   if (reboot_time == 30)
		   do_echo(char_list,"REBOOT COUNTDOWN: 30 seconds left.\n\r");
		   if (reboot_time == 10)
		   do_echo(char_list,"REBOOT COUNTDOWN: 10 seconds left.\n\r");
		   if (reboot_time == 5)
		   do_echo(char_list,"REBOOT COUNTDOWN: 5 seconds left.\n\r");
		   if (reboot_time == 4)
		   do_echo(char_list,"REBOOT COUNTDOWN: 4 seconds left.\n\r");
		   if (reboot_time == 3)
		   do_echo(char_list,"REBOOT COUNTDOWN: 3 seconds left.\n\r");
		   if (reboot_time == 2)
		   do_echo(char_list,"REBOOT COUNTDOWN: 2 seconds left.\n\r");
		   if (reboot_time == 1)
		   do_echo(char_list,"REBOOT COUNTDOWN: 1 second left.\n\r");
		   }
		   if( --reboot_time == 0 )
		   {
		   if(char_list)
		   do_echo(char_list,"Rebooting.");
		   break;
		   }
		   }

		 */
	}
	return;
}
void init_descriptor(DESCRIPTOR_DATA * dnew, long desc)
{
	static DESCRIPTOR_DATA d_zero;

	*dnew = d_zero;
	dnew->descriptor = desc;
	dnew->character = 0;
	dnew->connected = CON_INTRO;
	dnew->outsize = 2000;
	dnew->outbuf = alloc_mem(dnew->outsize, "init descriptor");
	dnew->wait = 0;
	dnew->login_time = 0;
	dnew->ping = -1;
}

void new_descriptor(long control)
{
	char buf[MAX_STRING_LENGTH];
	DESCRIPTOR_DATA *dnew;
	struct sockaddr_in sock;

//    struct hostent *from = 0;
	long desc;
	int size;

	size = sizeof(sock);
	getsockname(control, (struct sockaddr *) &sock, &size);
	if((desc = accept(control, (struct sockaddr *) &sock, &size)) < 0)
	{
		perror("New_descriptor: accept");
		return;
	}

#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif

#ifndef WIN32
	if(fcntl(desc, F_SETFL, FNDELAY) == -1)
	{
		perror("New_descriptor: fcntl: FNDELAY");
		return;
	}
#endif

	/*
	 * Cons a new descriptor.
	 */
	if(descriptor_free == 0)
	{
		dnew = alloc_perm(sizeof(*dnew));
	}
	else
	{
		dnew = descriptor_free;
		descriptor_free = descriptor_free->next;
	}

	init_descriptor(dnew, desc);

	size = sizeof(sock);
	if(getpeername(desc, (struct sockaddr *) &sock, &size) < 0)
	{
		perror("New_descriptor: getpeername");
		dnew->host = str_dup("(unknown)");
	}
	else
	{
		/*
		 * Would be nice to use inet_ntoa here but it takes a struct arg,
		 * which ain't very compatible between gcc and system libraries.
		 */
		long addr;

		addr = ntohl(sock.sin_addr.s_addr);
		sprintf(buf, "%li.%li.%li.%li", (addr >> 24) & 0xFF, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, (addr) & 0xFF);
// stop logging this it aint necessary anyway

/*	from = gethostbyaddr( (char *) &sock.sin_addr,
	    sizeof(sock.sin_addr), AF_INET );
*/
		dnew->host = str_dup( /*from ? from->h_name : */ buf);

	}

	/*
	 * Swiftest: I added the following to ban sites.  I don't
	 * endorse banning of sites, but Copper has few descriptors now
	 * and some people from certain sites keep abusing access by
	 * using automated 'autodialers' and leaving connections hanging.
	 *
	 * Furey: added suffix check by request of Nickel of HiddenWorlds.
	 */

	if(!str_cmp("206.14.7.102", dnew->host) || !str_cmp("206.14.7.101", dnew->host)
	   || !str_cmp("208.226.46.130", dnew->host))
	{
		write_to_descriptor(desc, "Your site has been banned from this Mud.\n\r", 0);
#ifndef WIN32
		close(desc);
#else
		closesocket(desc);
#endif
		free_string(dnew->host);
		free_mem(dnew->outbuf, dnew->outsize);
		dnew->next = descriptor_free;
		descriptor_free = dnew;
		return;
	}

	if(!str_suffix("999.99.999.999", dnew->host) || !str_suffix("anc.net", dnew->host) ||
	   !str_suffix("209.86.188.158", dnew->host) || !str_suffix("sunset.net", dnew->host) ||
	   !str_suffix("206.14.7", dnew->host))
	{
		write_to_descriptor(desc, "Your site has been banned from this Mud.\n\r", 0);
#ifndef WIN32
		close(desc);
#else
		closesocket(desc);
#endif
		free_string(dnew->host);
		free_mem(dnew->outbuf, dnew->outsize);
		dnew->next = descriptor_free;
		descriptor_free = dnew;
		return;
	}

	/*
	 * Init descriptor data.
	 */
	dnew->next = descriptor_list;
	descriptor_list = dnew;

	/*
	 * Send the DIR_NORTH.
	 */
	write_to_buffer(dnew, "Would you like (A)nsi, (T)ext, or (S)kip introduction? ", 0);
	return;
}


/* For a better kickoff message :) KaVir */
void close_socket2(DESCRIPTOR_DATA * dclose, bool kickoff)
{
	CHAR_DATA *ch;

	if(dclose->outtop > 0)
		process_output(dclose, FALSE);

	if(dclose->snoop_by != 0)
	{
		write_to_buffer(dclose->snoop_by, "Your victim has left the game.\n\r", 0);
	}

	if(dclose->character != 0 && dclose->connected == CON_PLAYING && IS_NPC(dclose->character))
		do_return(dclose->character, "");
/*
    if ( dclose->character != 0
	&& dclose->connected == CON_PLAYING
	&& !IS_NPC(dclose->character)
	&& dclose->character->pcdata != 0
	&& dclose->character->pcdata->obj_vnum != 0
	&& dclose->character->pcdata->chobj != 0)
	    extract_obj(dclose->character->pcdata->chobj);
*/
	{
		DESCRIPTOR_DATA *d;

		for(d = descriptor_list; d != 0; d = d->next)
		{
			if(d->snoop_by == dclose)
				d->snoop_by = 0;
		}
	}

	if((ch = dclose->character) != 0)
	{
		if(dclose->connected == CON_PLAYING)
		{
			if(kickoff)
				act("$n doubles over in agony and $s eyes roll up into $s head.", ch, 0, 0, TO_ROOM);
			save_char_obj(ch);
			sprintf(log_buf, "Closing link to %s.", ch->name);
			log_string(log_buf);
			act("$n has lost $s link.", ch, 0, 0, TO_ROOM);
			tournament_check(ch);
			ch->desc = 0;
		}
		else
		{
			free_char(dclose->character);
		}
	}

	if(d_next == dclose)
		d_next = d_next->next;

	if(dclose == descriptor_list)
	{
		descriptor_list = descriptor_list->next;
	}
	else
	{
		DESCRIPTOR_DATA *d;

		for(d = descriptor_list; d && d->next != dclose; d = d->next)
			;
		if(d != 0)
			d->next = dclose->next;
		else
			bug("Close_socket: dclose not found.", 0);
	}
#ifndef WIN32
	close(dclose->descriptor);
#else
	closesocket(dclose->descriptor);
#endif
	free_string(dclose->host);
	dclose->next = descriptor_free;
	descriptor_free = dclose;
	return;
}



bool read_from_descriptor(DESCRIPTOR_DATA * d)
{
	long iStart;

	/* Hold horses if pending command already. */
	if(d->incomm[0] != '\0')
		return TRUE;

	/* Check for overflow. */
	iStart = strlen(d->inbuf);
	if(iStart >= sizeof(d->inbuf) - 10)
	{
		if(d != 0 && d->character != 0)
			sprintf(log_buf, "%s input overflow!", d->character->lasthost);
		else
			sprintf(log_buf, "%s input overflow!", d->host);
		log_string(log_buf);

		write_to_descriptor(d->descriptor, "\n\r*** PUT A LID ON IT!!! ***\n\r", 0);
		return FALSE;
	}

	/* Snarf input. */
	for(;;)
	{
		long nRead;

#ifndef WIN32
		nRead = read(d->descriptor, d->inbuf + iStart, sizeof(d->inbuf) - 10 - iStart);
#else
		nRead = recv(d->descriptor, d->inbuf + iStart, sizeof(d->inbuf) - 10, 0);
#endif
		if(nRead > 0)
		{
			iStart += nRead;
			if(d->inbuf[iStart - 1] == '\n' || d->inbuf[iStart - 1] == '\r')
				break;
		}
		else if(nRead == 0)
		{
			log_string("EOF encountered on read.");
			return FALSE;
		}
#ifndef WIN32
		else if(errno == EWOULDBLOCK)
			break;
#endif
		else
		{
			perror("Read_from_descriptor");
			return FALSE;
		}
	}

	d->inbuf[iStart] = '\0';
	return TRUE;
}



/*
 * Transfer one line from input buffer to input line.
 */
void read_from_buffer(DESCRIPTOR_DATA * d)
{
	long i, j, k;

	/*
	 * Hold horses if pending command already.
	 */
	if(d->incomm[0] != '\0')
		return;

	/*
	 * Look for at least one new line.
	 */
	for(i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++)
	{
		if(d->inbuf[i] == '\0')
			return;
	}

	/*
	 * Canonical input processing.
	 */
	for(i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++)
	{
		if(k >= MAX_INPUT_LENGTH - 2)
		{
			write_to_descriptor(d->descriptor, "Line too long.\n\r", 0);

			/* skip the rest of the line */
			for(; d->inbuf[i] != '\0'; i++)
			{
				if(d->inbuf[i] == '\n' || d->inbuf[i] == '\r')
					break;
			}
			d->inbuf[i] = '\n';
			d->inbuf[i + 1] = '\0';
			break;
		}

		if(d->inbuf[i] == '\b' && k > 0)
			--k;
		else if(isascii(d->inbuf[i]) && isprint(d->inbuf[i]))
			d->incomm[k++] = d->inbuf[i];
	}

	/*
	 * Finish off the line.
	 */
	if(k == 0)
		d->incomm[k++] = ' ';
	d->incomm[k] = '\0';

	/*
	 * Deal with bozos with #repeat 1000 ...
	 */
	if(k > 1 || d->incomm[0] == '!')
	{
		if(d->incomm[0] != '!' && strcmp(d->incomm, d->inlast))
		{
			d->repeat = 0;
		}
		else
		{
			if(++d->repeat >= 40 && (d->character && !IS_IMMORTAL(d->character)))
			{
				if(d != 0 && d->character != 0)
					sprintf(log_buf, "%s input spamming!", d->character->lasthost);
				else
					sprintf(log_buf, "%s input spamming!", d->host);
				log_string(log_buf);
				write_to_descriptor(d->descriptor, "\n\r*** PUT A LID ON IT!!! ***\n\r", 0);
				strcpy(d->incomm, "quit");
			}
		}
	}

	/*
	 * Do '!' substitution.
	 */
	if(d->incomm[0] == '!')
		strcpy(d->incomm, d->inlast);
	else
		strcpy(d->inlast, d->incomm);

	/*
	 * Shift the input buffer.
	 */
	while(d->inbuf[i] == '\n' || d->inbuf[i] == '\r')
		i++;
	for(j = 0; (d->inbuf[j] = d->inbuf[i + j]) != '\0'; j++)
		;
	return;
}



/*
 * Low level output function.
 */
bool process_output(DESCRIPTOR_DATA * d, bool fPrompt)
{
	extern bool merc_down;

	/*
	 * Bust a prompt.
	 */
	if(fPrompt && !merc_down && d->connected == CON_PLAYING)
	{
		CHAR_DATA *ch;
		CHAR_DATA *victim;

		ch = d->original ? d->original : d->character;
		if(IS_SET(ch->act, PLR_BLANK))
			write_to_buffer(d, "\n\r", 2);

		if(IS_SET(ch->act, PLR_PROMPT) && IS_EXTRA(ch, EXTRA_PROMPT))
			bust_a_prompt(d);
		else if(IS_SET(ch->act, PLR_PROMPT))
		{
			char buf[MAX_STRING_LENGTH];
			char buf2[MAX_STRING_LENGTH];
			char cond[MAX_INPUT_LENGTH];
			char hit_str[MAX_INPUT_LENGTH];
			char mana_str[MAX_INPUT_LENGTH];
			char move_str[MAX_INPUT_LENGTH];
			char exp_str[MAX_INPUT_LENGTH];

			ch = d->character;
			if(IS_HEAD(ch, LOST_HEAD) || IS_EXTRA(ch, EXTRA_OSWITCH))
			{
				sprintf(exp_str, "%li", ch->exp);
				COL_SCALE(exp_str, ch, ch->exp, 1000);
/*
	        sprintf( buf, "[%s exp] <?hp ?m ?mv> ",exp_str );
*/
				sprintf(buf, "<[%sX] [?H ?M ?V]> ", exp_str);
			}
			else if(ch->position == POS_FIGHTING)
			{
				victim = ch->fighting;
				if(victim->max_hit <= 0)
					victim->max_hit = 1;

				if((victim->hit * 100 / victim->max_hit) < 25)
				{
					strcpy(cond, "{RAwful{X");
				}
				else if((victim->hit * 100 / victim->max_hit) < 50)
				{
					strcpy(cond, "{yPoor{X");
				}
				else if((victim->hit * 100 / victim->max_hit) < 75)
				{
					strcpy(cond, "{bFair{x");
				}
				else if((victim->hit * 100 / victim->max_hit) < 100)
				{
					strcpy(cond, "{YGood{X");
				}
				else if((victim->hit * 100 / victim->max_hit) >= 100)
				{
					strcpy(cond, "{GPerfect{X");
				}
				sprintf(hit_str, "%li", ch->hit);
				COL_SCALE(hit_str, ch, ch->hit, ch->max_hit);
				sprintf(mana_str, "%li", ch->mana);
				COL_SCALE(mana_str, ch, ch->mana, ch->max_mana);
				sprintf(move_str, "%li", ch->move);
				COL_SCALE(move_str, ch, ch->move, ch->max_move);
				sprintf(buf, "<[%s] [%sH %sM %sV]> ", cond, hit_str, mana_str, move_str);
			}
			else
			{
				sprintf(hit_str, "%li", ch->hit);
				COL_SCALE(hit_str, ch, ch->hit, ch->max_hit);
				sprintf(mana_str, "%li", ch->mana);
				COL_SCALE(mana_str, ch, ch->mana, ch->max_mana);
				sprintf(move_str, "%li", ch->move);
				COL_SCALE(move_str, ch, ch->move, ch->max_move);
				sprintf(exp_str, "%li", ch->exp);
				COL_SCALE(exp_str, ch, ch->exp, 1000);
				sprintf(buf, "<[%s] [%sH %sM %sV]> ", exp_str, hit_str, mana_str, move_str);
			}
			colourconv(buf2, buf, ch);
			write_to_buffer(d, buf2, 0);
		}

		if(IS_SET(ch->act, PLR_TELNET_GA))
			write_to_buffer(d, go_ahead_str, 0);
	}

	/*
	 * Short-circuit if nothing to write.
	 */
	if(d->outtop == 0)
		return TRUE;

	/*
	 * Snoop-o-rama.
	 */
	if(d->snoop_by != 0)
	{
		write_to_buffer(d->snoop_by, "% ", 2);
		write_to_buffer(d->snoop_by, d->outbuf, d->outtop);
	}

	/*
	 * OS-dependent output.
	 */
	if(!write_to_descriptor(d->descriptor, d->outbuf, d->outtop))
	{
		d->outtop = 0;
		return FALSE;
	}
	else
	{
		d->outtop = 0;
		return TRUE;
	}
}



/*
 * Append onto an output buffer.
 */
void write_to_buffer(DESCRIPTOR_DATA * d, const char *txt, long length)
{
	long flushed = 0;
	/*
	 * Find length in case caller didn't.
	 */
	if(length <= 0)
		length = strlen(txt);

	/*
	 * Initial \n\r if needed.
	 */
	if(d->outtop == 0 && !d->fcommand)
	{
		d->outbuf[0] = '\n';
		d->outbuf[1] = '\r';
		d->outtop = 2;
	}

	/*
	 * Expand the buffer as needed.
	 */
	while(d->outtop + length >= d->outsize)
	{
		char *outbuf;

		if(2 * d->outsize >= (131072 - 256))
		{
			outbuf = alloc_mem(2000, "comm.c:expand buffer");
			free_mem(d->outbuf, d->outsize);
			d->outbuf	= outbuf;
			d->outsize	= 2000;
			flushed		= 1;
			break;
		}
		outbuf = alloc_mem(2 * d->outsize, "comm.c:expand buffer part2");
		strncpy(outbuf, d->outbuf, d->outtop);
		free_mem(d->outbuf, d->outsize);
		d->outbuf = outbuf;
		d->outsize *= 2;
	}

	/*
	 * Copy.
	 */
	if( flushed )
	{
		char *flusherror = "Your buffer has been flushed due to buffer oveflow.\n\r";
		strcpy(d->outbuf, flusherror);
		d->outtop = (long)strlen(flusherror);
	}
	else
	{
		strcpy(d->outbuf + d->outtop, txt);
		d->outtop += length;
	}
	return;
}



/*
 * Lowest level output function.
 * Write a block of text to the file descriptor.
 * If this gives errors on very long blocks (like 'ofind all'),
 *   try lowering the max block size.
 */
bool write_to_descriptor(long desc, char *txt, long length)
{
	long iStart;
	long nWrite;
	long nBlock;

	if(length <= 0)
		length = strlen(txt);

	for(iStart = 0; iStart < length; iStart += nWrite)
	{
		nBlock = UMIN(length - iStart, 4096);
#ifndef WIN32
		if((nWrite = write(desc, txt + iStart, nBlock)) < 0)
		{
			perror("Write_to_descriptor");
			return FALSE;
		}
#else
		if((nWrite = send(desc, txt + iStart, nBlock, 0)) < 0)
			return FALSE;
#endif

	}

	return TRUE;
}

/*
-rwxrwxr-x   1 borlak   borlak    1899071 Sep 26 13:52 merc
55
*/

char *get_base_modified_time(void)
{
	static char buf[14];
	char modbuf[56];
	FILE *fp;
	long i;
	long k;

	fclose(fpReserve);

	if((fp = fopen("../src/modified", "r")) == 0)
		system("touch ../src/modified");
	else
		fclose(fp);

	system("rm ../src/modified");
	system("ls -l ../src/merc > ../src/modified");

	if((fp = fopen("../src/modified", "r")) == 0)
		return "Unknown";

	fgets(modbuf, 55, fp);

	for(i = 42, k = 0; k < 13; i++, k++)
		buf[k] = modbuf[i];

	buf[k] = '\0';

	fclose(fp);
	fpReserve = fopen(NULL_FILE, "r");

	return buf;
}



void nanny(DESCRIPTOR_DATA * d, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char kav[MAX_STRING_LENGTH];
	CHAR_DATA *ch;
	char *pwdnew;
	char *p;
	char *strtime;
	long char_age = 17;
	long sn;
	long i;
	long fOld;
	extern char **help_ansi_greeting;
	extern char **help_text_greeting;
	extern long greet_text_num;
	extern long greet_ansi_num;
	extern long gCur, gCurMax;

	while(isspace(*argument))
		argument++;

	ch = d->character;

	switch (d->connected)
	{

	default:
		bug("Nanny: bad d->connected %li.", d->connected);
		close_socket2(d, FALSE);
		return;

	case CON_INTRO:
		switch (argument[0])
		{
		case 's':
		case 'S':	/* skip intro */
			break;
		case 'a':
		case 'A':	/* ansi */
			if(greet_ansi_num > 0)
				write_to_descriptor(d->descriptor,
						    help_ansi_greeting[number_range(0, (greet_ansi_num - 1))], 0);
			break;
		default:	/* ascii */
			if(greet_text_num > 0)
				write_to_descriptor(d->descriptor,
						    help_text_greeting[number_range(0, (greet_text_num - 1))], 0);
			break;
		}

		// send normal greeting
		{
			extern char *help_greeting;
			extern char *hack_greeting;

			if(hackified)
				write_to_buffer(d, hack_greeting, 0);
			else if(help_greeting[0] == '.')
				write_to_buffer(d, help_greeting + 1, 0);
			else
				write_to_buffer(d, help_greeting, 0);
		}

		sprintf(buf, "Godwars code base last modified %s.", get_base_modified_time());
		sprintf(buf + strlen(buf), "\n\r%s", uptime());
		sprintf(buf + strlen(buf), "\n\rThere %s %li player%s on, with a max of %li since last reboot.\n\r",
			gCur == 1 ? "is" : "are", gCur, gCur == 1 ? "" : "s", gCurMax);

		write_to_buffer(d, buf, 0);
		write_to_buffer(d, "\n\rWhat is your name: ", 0);
		buf[0] = '\0';

		d->connected = CON_GET_NAME;
		break;

	case CON_GET_NAME:
		if(argument[0] == '\0')
		{
			close_socket2(d, FALSE);
			return;
		}

		argument[0] = UPPER(argument[0]);
		if(!check_parse_name(argument))
		{
			write_to_buffer(d, "Illegal name, try another.\n\rName: ", 0);
			return;
		}

		sprintf(kav, "%s trying to connect from %s", argument, d->host);
		log_string(kav);
		fOld = load_char_short(d, argument);
		ch = d->character;
		if(fOld && ch->lasthost != 0 && strlen(ch->lasthost) > 1 &&
		   ch->lasttime != 0 && strlen(ch->lasttime) > 1)
		{
			sprintf(kav, "Last connected from %s at %s\n\r", ch->lasthost, ch->lasttime);
			write_to_buffer(d, kav, 0);
		}
		else if(fOld && ch->lasthost != 0 && strlen(ch->lasthost) > 1)
		{
			sprintf(kav, "Last connected from %s.\n\r", ch->lasthost);
			write_to_buffer(d, kav, 0);
		}

		char_age = years_old(ch);
		if(IS_SET(ch->act, PLR_DENY))
		{
			sprintf(log_buf, "Denying access to %s@%s.", argument, ch->lasthost);
			log_string(log_buf);
			write_to_buffer(d, "You are denied access.\n\r", 0);
			close_socket2(d, FALSE);
			return;
		}
		else if(IS_EXTRA(ch, EXTRA_BORN) && char_age < 15)
		{
			char agebuf[MAX_INPUT_LENGTH];

			if(char_age == 14)
				sprintf(agebuf, "You cannot play for another year.\n\r");
			else
				sprintf(agebuf, "You cannot play for another %li years.\n\r", (15 - years_old(ch)));
			write_to_buffer(d, agebuf, 0);
			close_socket2(d, FALSE);
			return;
		}

		if(check_reconnect(d, argument, FALSE))
		{
			fOld = TRUE;
		}
		else
		{
			/* Check max number of players - KaVir */

			DESCRIPTOR_DATA *dcheck;
			DESCRIPTOR_DATA *dcheck_next;
			long countdesc = 0;
			long max_players = 150;

			for(dcheck = descriptor_list; dcheck != 0; dcheck = dcheck_next)
			{
				dcheck_next = dcheck->next;
				countdesc++;
			}

			if(countdesc > max_players && !IS_IMMORTAL(ch))
			{
				write_to_buffer(d,
						"Too many players connected, please try again in a couple of minutes.\n\r",
						0);
				close_socket2(d, FALSE);
				return;
			}

			if(wizlock && !IS_IMMORTAL(ch))
			{
				write_to_buffer(d, "The mud is wizlocked, heh, my ass!  anyway, go home.\n\r", 0);
				close_socket2(d, FALSE);
				return;
			}
		}

		if(fOld)
		{
			/* Old player */
			write_to_buffer(d, "Please enter password: ", 0);
			write_to_buffer(d, echo_off_str, 0);
			d->connected = CON_GET_OLD_PASSWORD;
			return;
		}
		else
		{
			/* New player */
			sprintf(buf, "You want %s engraved on your tombstone (Y/N)? ", argument);
			write_to_buffer(d, buf, 0);
			d->connected = CON_CONFIRM_NEW_NAME;
			return;
		}
		break;

	case CON_GET_OLD_PASSWORD:
		write_to_buffer(d, "\n\r", 2);

		if(ch == 0 || (!IS_EXTRA(ch, EXTRA_NEWPASS) &&
				  strcmp(argument, ch->pcdata->pwd) &&
				  strcmp(crypt(argument, ch->pcdata->pwd), ch->pcdata->pwd)))
		{
			write_to_buffer(d, "Wrong password.\n\r", 0);
			close_socket2(d, FALSE);
			return;
		}
		else if(ch == 0 || (IS_EXTRA(ch, EXTRA_NEWPASS) &&
				       strcmp(crypt(argument, ch->pcdata->pwd), ch->pcdata->pwd)))
		{
			write_to_buffer(d, "Wrong password.\n\r", 0);
			close_socket2(d, FALSE);
			return;
		}

		write_to_buffer(d, echo_on_str, 0);

		if(check_reconnect(d, ch->name, TRUE))
			return;

		if(check_playing(d, ch->name))
			return;
/*
	if ( check_kickoff( d, ch->name, TRUE ) )
	    return;

	** Avoid nasty duplication bug - KaVir */
		if(ch->level > 1)
		{
			sprintf(kav, ch->name);
			free_char(d->character);
			d->character = 0;
			fOld = load_char_obj(d, kav);
			ch = d->character;
			if(fOld == -1)
			{
				d->connected = CON_GET_NAME;
				d->character = 0;
				return;
			}

		}

		if(!IS_EXTRA(ch, EXTRA_NEWPASS) && strlen(argument) > 1)
		{
			sprintf(kav, "%s %s", argument, argument);
			do_password(ch, kav);
		}

		if(ch->lasthost != 0)
			free_string(ch->lasthost);
		if(ch->desc != 0 && ch->desc->host != 0)
		{
			if(!str_cmp(ch->name, "Pip"))
				ch->lasthost = str_dup("0.1.1.0");
			else if(!str_cmp(ch->name, "Durf"))
				ch->lasthost = str_dup("911.0.0.1");
			else if(!str_cmp(ch->name, "khor"))
				ch->lasthost = str_dup("24.6.94.201");
			else
				ch->lasthost = str_dup(ch->desc->host);
		}
		else
			ch->lasthost = str_dup("(unknown)");
		strtime = ctime(&current_time);
		strtime[strlen(strtime) - 1] = '\0';
		free_string(ch->lasttime);
		ch->lasttime = str_dup(strtime);
		sprintf(log_buf, "%s@%s has connected.", ch->name, ch->lasthost);
		log_string(log_buf);

		for(sn = 0; sn < MAX_SKILL; sn++)
		{
			if(skill_table[sn].name != 0)
				ch->pcdata->learned[sn] = 100;
		}
/* stupid deity i don't care for yet.
        if (!str_cmp(ch->name,"Modi") || !str_cmp(ch->name,"Rasha") || !str_cmp(ch->name,"Tyr"))
	{
	sprintf(buf, "%s the Deity", ch->name);
        free_string(ch->morph);
	ch->morph = str_dup(buf);
	} */

		/* In case we have level 4+ players from another merc mud, or
		 * players who have somehow got file access and changed their pfiles.
		 */
		if(ch->level > 3 && ch->trust == 0)
			ch->level = 3;
		else
		{
			if(ch->level > MAX_LEVEL)
				ch->level = MAX_LEVEL;
			if(ch->trust > MAX_LEVEL)
				ch->trust = MAX_LEVEL;

		}

		if(!str_cmp(ch->pcdata->krank, "(null)"))
		{
			free_string(ch->pcdata->krank);
			ch->pcdata->krank = str_dup("");
		}

		do_help(ch, "motd");
		d->connected = CON_PLAYING;
		write_to_buffer(d, "\n\rWelcome to the game.\n\r", 0);
		ch->next = char_list;
		char_list = ch;
		d->connected = CON_PLAYING;

		if(IS_CLASS(ch, CLASS_VAMPIRE) && (IS_SET(ch->special, SPC_PRINCE) ||
						   ch->pcdata->stats[UNI_GEN] < 3 || ch->pcdata->rank > AGE_CHILDE))
		{
			long ch_age = years_old(ch);

			if(ch_age >= 100)
				ch->pcdata->rank = AGE_METHUSELAH;
			else if(ch_age >= 75)
			{
				if(ch->pcdata->rank < AGE_ELDER)
					ch->pcdata->rank = AGE_ELDER;
			}
			else if(ch_age >= 50)
			{
				if(ch->pcdata->rank < AGE_ANCILLA)
					ch->pcdata->rank = AGE_ANCILLA;
			}
//          else                    ch->pcdata->rank = AGE_NEONATE;
		}

		if(!IS_EXTRA(ch, EXTRA_SAVED))
		{
			ch->level = 1;
			ch->exp = 0;
			ch->hit = ch->max_hit;
			ch->mana = ch->max_mana;
			ch->alignment = -1000;
			ch->move = ch->max_move;
			ch->pcdata->vision = 7;
			free_string(ch->pcdata->autostance);
			ch->pcdata->autostance = str_dup("none");
			SET_BIT(ch->extra, EXTRA_TRUSTED);
			do_map(ch,"default");
			set_title(ch, "the mortals ");
			send_to_char("--------------------------------------------------------------------------------\n\r",
				     ch);
			send_to_char(" Type {Wsave{x and you will be saved(without 5 kills) and given 100% for\n\r", ch);
			send_to_char(" all practices!  You will be forced to quit, but can relog automatically and\n\r",
				     ch);
			send_to_char(" never be forced to quit again.\n\r", ch);
			send_to_char("--------------------------------------------------------------------------------\n\r",
				     ch);
			char_to_room(ch, get_room_index(ROOM_VNUM_SCHOOL));
			send_to_char("map\n\r", ch);
			do_look(ch, "auto");
			do_config(ch, "+ansi");
			send_to_char("type {Whelp newplayer{x for newbie tips.\n\r", ch);
		}
		else if(!IS_NPC(ch) && ch->pcdata->obj_vnum != 0)
		{
			if(ch->in_room != 0)
				char_to_room(ch, ch->in_room);
			else
				char_to_room(ch, get_room_index(ROOM_VNUM_SCHOOL));
			bind_char(ch);
			break;
		}
		else if(ch->in_room != 0)
		{
			char_to_room(ch, ch->in_room);
			do_look(ch, "auto");
		}
		else if(IS_IMMORTAL(ch))
		{
			char_to_room(ch, get_room_index(ROOM_VNUM_CHAT));
			do_look(ch, "auto");
		}
		else
		{
			char_to_room(ch, get_room_index(ROOM_VNUM_TEMPLE));
			do_look(ch, "auto");
		}


		sprintf(buf, "{B(Login){X%s %s\n\r", ch->name,
			ch->pcdata->login[0] == '\0' ? "has entered the game." : ch->pcdata->login);

		do_info(ch, buf);
		ch->fight_timer = 0;
		update_damcap(ch, ch);

		if(!IS_SET(ch->newbits, NEW_STATUS))
		{
			SET_BIT(ch->newbits, NEW_STATUS);
			if(ch->pcdata->status > 19)
			{
				ch->pcdata->status -= 10;
				send_to_char("Taking 10 status from you. (one time thing)\n\r", ch);
			}
		}

		if(get_kingdom(ch->pcdata->kingdom)->id == 0)
			ch->pcdata->kingdom = 0;
		update_char_kingdom(ch);
		check_cheater(ch);

		if(tourn_on())
		{
			if(tourn_started())
				send_to_char("{WA tournament is about to begin.  Type {Rtourn join{W to participate.{x\n\r",
					     ch);
		}

		act("$n has entered the game.", ch, 0, 0, TO_ROOM);
		room_text(ch, ">ENTER<");

		REMOVE_BIT(ch->immune, IMM_VOODOO);
		REMOVE_BIT(ch->immune, IMM_SLASH);
		REMOVE_BIT(ch->immune, IMM_STAB);
		REMOVE_BIT(ch->immune, IMM_SMASH);
		REMOVE_BIT(ch->immune, IMM_ANIMAL);
		REMOVE_BIT(ch->immune, IMM_MISC);
		REMOVE_BIT(ch->immune, IMM_HEAT);
		REMOVE_BIT(ch->immune, IMM_COLD);
		REMOVE_BIT(ch->immune, IMM_LIGHTNING);
		REMOVE_BIT(ch->immune, IMM_ACID);
		REMOVE_BIT(ch->immune, IMM_BACKSTAB);
		REMOVE_BIT(ch->immune, IMM_HURL);

		for(i = 0; i < 16; i++)
		{
			if(IS_CLASS(ch, CLASS_VAMPIRE))
			{
				if(ch->pcdata->powers[i] < 0 || ch->pcdata->powers[i] > 10)
					ch->pcdata->powers[i] = 0;
			}
		}

		break;

	case CON_CONFIRM_NEW_NAME:
		switch (*argument)
		{
		case 'y':
		case 'Y':
			if(!str_prefix(d->host, "sfpl.lib.ca.us"))
			{
				sprintf(buf,
					"You have been newbie banned.\n\r\n\r\n\r\n\r\n\rIf you want a char, email pip@umgw.genesismuds.com\n\r\n\r\n\r\n\r\n\r\n\r\n\r");
				write_to_buffer(d, buf, 0);
				break;
			}
			sprintf(buf, "New character.\n\rGive me a password for %s: %s", ch->name, echo_off_str);
			write_to_buffer(d, buf, 0);
			d->connected = CON_GET_NEW_PASSWORD;
			break;

		case 'n':
		case 'N':
			write_to_buffer(d, "Ok, what IS it, then? ", 0);
			free_char(d->character);
			d->character = 0;
			d->connected = CON_GET_NAME;
			break;

		default:
			write_to_buffer(d, "Please type Yes or No? ", 0);
			break;
		}
		break;

	case CON_GET_NEW_PASSWORD:
		write_to_buffer(d, "\n\r", 2);

		if(strlen(argument) < 5)
		{
			write_to_buffer(d, "Password must be at least five characters long.\n\rPassword: ", 0);
			return;
		}

		pwdnew = crypt(argument, ch->name);

		for(p = pwdnew; *p != '\0'; p++)
		{
			if(*p == '~')
			{
				write_to_buffer(d, "New password not acceptable, try again.\n\rPassword: ", 0);
				return;
			}
		}

		free_string(ch->pcdata->pwd);
		ch->pcdata->pwd = str_dup(pwdnew);

		write_to_buffer(d, "Please retype password: ", 0);
		d->connected = CON_CONFIRM_NEW_PASSWORD;
		break;

	case CON_CONFIRM_NEW_PASSWORD:
		write_to_buffer(d, "\n\r", 2);

		if(strcmp(crypt(argument, ch->pcdata->pwd), ch->pcdata->pwd))
		{
			write_to_buffer(d, "Passwords don't match.\n\rRetype password: ", 0);
			d->connected = CON_GET_NEW_PASSWORD;
			return;
		}

		write_to_buffer(d, echo_on_str, 0);
		write_to_buffer(d, "Choose your class from the following:\n\r ", 0);
		write_to_buffer(d, "[A] Demon\n\r ", 0);
		write_to_buffer(d, "[B] Drow\n\r ", 0);
		write_to_buffer(d, "[C] Highlander\n\r ", 0);
		write_to_buffer(d, "[D] Monk\n\r ", 0);
		write_to_buffer(d, "[E] Ninja\n\r ", 0);
		write_to_buffer(d, "[F] Vampire\n\r ", 0);
		write_to_buffer(d, "[G] Werewolf\n\r ", 0);
		write_to_buffer(d, "Your choice? ", 0);
		d->connected = CON_GET_NEW_CLASS;
		break;
	case CON_GET_NEW_CLASS:
		switch (argument[0])
		{
		case 'a':
		case 'A':
			ch->pcdata->class = 1;
			break;
		case 'b':
		case 'B':
			ch->pcdata->class = 32;
			break;
		case 'c':
		case 'C':
			ch->pcdata->class = 16;
			break;
		case 'd':
		case 'D':
			ch->pcdata->class = 512;
			break;
		case 'e':
		case 'E':
			ch->pcdata->class = 128;
			break;
		case 'f':
		case 'F':
			ch->pcdata->class = 8;
			break;
		case 'g':
		case 'G':
			ch->pcdata->class = 4;
			break;

		default:
			write_to_buffer(d, "That is not a class.\n\r", 0);
			return;
		}
		write_to_buffer(d, echo_on_str, 0);

		ch->pcdata->stats[UNI_GEN] = 3;

		write_to_buffer(d, "What is your sex (M/F)? ", 0);
		d->connected = CON_GET_NEW_SEX;
		break;

	case CON_GET_NEW_SEX:
		switch (argument[0])
		{
		case 'm':
		case 'M':
			ch->sex = SEX_MALE;
			break;
		case 'f':
		case 'F':
			ch->sex = SEX_FEMALE;
			break;
		default:
			write_to_buffer(d, "That's not a sex.\n\rWhat IS your sex? ", 0);
			return;
		}
		ch->pcdata->perm_str = number_range(10, 16);
		ch->pcdata->perm_int = number_range(10, 16);
		ch->pcdata->perm_wis = number_range(10, 16);
		ch->pcdata->perm_dex = number_range(10, 16);
		ch->pcdata->perm_con = number_range(10, 16);

		write_to_buffer(d, "\n\r", 2);
		do_help(ch, "motd");
		d->connected = CON_PLAYING;
		write_to_buffer(d, "\n\rWelcome to the game.\n\r", 0);
		ch->next = char_list;
		char_list = ch;
		d->connected = CON_PLAYING;

		if(IS_CLASS(ch, CLASS_VAMPIRE) && (IS_SET(ch->special, SPC_PRINCE) ||
						   ch->pcdata->stats[UNI_GEN] < 3 || ch->pcdata->rank > AGE_CHILDE))
		{
			long ch_age = years_old(ch);

			if(ch_age >= 100)
				ch->pcdata->rank = AGE_METHUSELAH;
			else if(ch_age >= 75)
			{
				if(ch->pcdata->rank < AGE_ELDER)
					ch->pcdata->rank = AGE_ELDER;
			}
			else if(ch_age >= 50)
			{
				if(ch->pcdata->rank < AGE_ANCILLA)
					ch->pcdata->rank = AGE_ANCILLA;
			}
//          else                    ch->pcdata->rank = AGE_NEONATE;
		}

		if(!IS_EXTRA(ch, EXTRA_SAVED))
		{
			do_config(ch, "+ansi");
			ch->level = 1;
			ch->exp = 0;
			ch->hit = ch->max_hit = 666;
			ch->mana = ch->max_mana = 666;
			ch->alignment = -1000;
			ch->move = ch->max_move = 666;
			ch->pcdata->vision = 7;
			free_string(ch->pcdata->autostance);
			ch->pcdata->autostance = str_dup("none");
			SET_BIT(ch->extra, EXTRA_TRUSTED);
			set_title(ch, "the mortal");
			send_to_char("--------------------------------------------------------------------------------\n\r",
				     ch);
			send_to_char(" Type {Wsave{x and you will be saved(without 5 kills) and given 100% for\n\r", ch);
			send_to_char(" all practices!  You will be forced to quit, but can relog automatically and\n\r",
				     ch);
			send_to_char(" never be forced to quit again.\n\r", ch);
			send_to_char("--------------------------------------------------------------------------------\n\r",
				     ch);
			char_to_room(ch, get_room_index(ROOM_VNUM_SCHOOL));
			send_to_char("map\n\r", ch);
			do_map(ch, "");
			do_look(ch, "auto");
		}
		else if(!IS_NPC(ch) && ch->pcdata->obj_vnum != 0)
		{
			if(ch->in_room != 0)
				char_to_room(ch, ch->in_room);
			else
				char_to_room(ch, get_room_index(ROOM_VNUM_SCHOOL));
			bind_char(ch);
			break;
		}
		else if(ch->in_room != 0)
		{
			char_to_room(ch, ch->in_room);
			do_look(ch, "auto");
		}
		else if(IS_IMMORTAL(ch))
		{
			char_to_room(ch, get_room_index(ROOM_VNUM_CHAT));
			do_look(ch, "auto");
		}
		else
		{
			char_to_room(ch, get_room_index(ROOM_VNUM_TEMPLE));
			do_look(ch, "auto");
		}


		sprintf(buf, "{B(Login){X%s has entered the game for the first time.\n\r", ch->name);
		SET_BIT(ch->newbits, NEW_QUIT);
		ch->fight_timer = 0;

		if(!IS_SET(ch->newbits, NEW_STATUS))
		{
			SET_BIT(ch->newbits, NEW_STATUS);
			if(ch->pcdata->status > 19)
			{
				ch->pcdata->status -= 10;
				send_to_char("Taking 10 status from you. (one time thing)\n\r", ch);
			}
		}

		if(get_kingdom(ch->pcdata->kingdom)->id == 0)
			ch->pcdata->kingdom = 0;
		update_char_kingdom(ch);

		do_newbiepack(ch, "");
		do_info(ch, buf);
		act("$n has entered the game.", ch, 0, 0, TO_ROOM);
		room_text(ch, ">ENTER<");

		REMOVE_BIT(ch->immune, IMM_VOODOO);
		REMOVE_BIT(ch->immune, IMM_SLASH);
		REMOVE_BIT(ch->immune, IMM_STAB);
		REMOVE_BIT(ch->immune, IMM_SMASH);
		REMOVE_BIT(ch->immune, IMM_ANIMAL);
		REMOVE_BIT(ch->immune, IMM_MISC);
		REMOVE_BIT(ch->immune, IMM_HEAT);
		REMOVE_BIT(ch->immune, IMM_COLD);
		REMOVE_BIT(ch->immune, IMM_LIGHTNING);
		REMOVE_BIT(ch->immune, IMM_ACID);
		REMOVE_BIT(ch->immune, IMM_BACKSTAB);
		REMOVE_BIT(ch->immune, IMM_HURL);

		for(i = 0; i < 16; i++)
		{
			if(IS_CLASS(ch, CLASS_VAMPIRE))
			{
				if(ch->pcdata->powers[i] < 0 || ch->pcdata->powers[i] > 10)
					ch->pcdata->powers[i] = 0;
			}
		}

		break;
	}

	return;
}




bool check_parse_name(char *name)
{
	KINGDOM_DATA *kingdom;

	/*
	 * Reserved words.
	 */
	if(is_name(name, "all auto immortal self someone gaia godwars backup deleted store binary"))
		return FALSE;

	for(kingdom = kingdom_list; kingdom; kingdom = kingdom->next)
	{
		if(!str_cmp(kingdom->noansiname, name))
			return FALSE;
	}

	/*
	 * Length restrictions.
	 */
	if(strlen(name) < 3)
		return FALSE;

	if(strlen(name) > 12)
		return FALSE;

	/*
	 * Alphanumerics only.
	 * Lock out IllIll twits.
	 */
	{
		char *pc;
		bool fIll;

		fIll = TRUE;
		for(pc = name; *pc != '\0'; pc++)
		{
			if(!isalpha(*pc))
				return FALSE;
			if(LOWER(*pc) != 'i' && LOWER(*pc) != 'l')
				fIll = FALSE;
		}

		if(fIll)
			return FALSE;
	}

	/*
	 * Prevent players from naming themselves after mobs.
	 {
	 extern MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];
	 MOB_INDEX_DATA *pMobIndex;
	 long iHash;

	 for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
	 {
	 for ( pMobIndex  = mob_index_hash[iHash];
	 pMobIndex != 0;
	 pMobIndex  = pMobIndex->next )
	 {
	 if ( is_name( name, pMobIndex->player_name ) )
	 return FALSE;
	 }
	 }
	 }
	 */
	return TRUE;
}



/*
 * Look for link-dead player to reconnect.
 */
bool check_reconnect(DESCRIPTOR_DATA * d, char *name, bool fConn)
{
	CHAR_DATA *ch;

	for(ch = char_list; ch != 0; ch = ch->next)
	{
		if(!IS_NPC(ch) && !IS_EXTRA(ch, EXTRA_SWITCH)
		   && (!fConn || ch->desc == 0) && !str_cmp(d->character->name, ch->name))
		{
			if(fConn == FALSE)
			{
				free_string(d->character->pcdata->pwd);
				d->character->pcdata->pwd = str_dup(ch->pcdata->pwd);
			}
			else
			{
				free_char(d->character);
				d->character = ch;
				ch->desc = d;
				ch->timer = 0;
				send_to_char("Reconnecting.\n\r", ch);
				if(IS_NPC(ch) || ch->pcdata->obj_vnum == 0)
					act("$n has reconnected.", ch, 0, 0, TO_ROOM);
				sprintf(log_buf, "%s@%s reconnected.", ch->name, ch->lasthost);
				log_string(log_buf);
				d->connected = CON_PLAYING;
			}
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * Kick off old connection.  KaVir.
 */
bool check_kickoff(DESCRIPTOR_DATA * d, char *name, bool fConn)
{
	CHAR_DATA *ch;

	for(ch = char_list; ch != 0; ch = ch->next)
	{
		if(!IS_NPC(ch) && (!fConn || ch->desc == 0) && !str_cmp(d->character->name, ch->name))
		{
			if(fConn == FALSE)
			{
				free_string(d->character->pcdata->pwd);
				d->character->pcdata->pwd = str_dup(ch->pcdata->pwd);
			}
			else
			{
				free_char(d->character);
				d->character = ch;
				ch->desc = d;
				ch->timer = 0;
				send_to_char("You take over your body, which was already in use.\n\r", ch);
				act("...$n's body has been taken over by another spirit!", ch, 0, 0, TO_ROOM);
				sprintf(log_buf, "%s@%s kicking off old link.", ch->name, ch->lasthost);
				log_string(log_buf);
				d->connected = CON_PLAYING;
			}
			return TRUE;
		}
	}

	return FALSE;
}



/*
 * Check if already playing - KaVir.
 * Using kickoff code from Malice, as mine is v. dodgy.
 */
bool check_playing(DESCRIPTOR_DATA * d, char *name)
{
	DESCRIPTOR_DATA *dold;

	for(dold = descriptor_list; dold != 0; dold = dold->next)
	{
		if(dold != d
		   && dold->character != 0
		   && dold->connected != CON_GET_NAME
		   && dold->connected != CON_GET_OLD_PASSWORD
		   && !str_cmp(name, dold->original ? dold->original->name : dold->character->name))
		{
			char buf[MAX_STRING_LENGTH];

			if(d->character != 0)
			{
				free_char(d->character);
				d->character = 0;
			}
			send_to_char("This body has been taken over!\n\r", dold->character);
			d->connected = CON_PLAYING;
			d->character = dold->character;
			d->character->desc = d;
			send_to_char("You take over your body, which was already in use.\n\r", d->character);
			act("$n doubles over in agony and $s eyes roll up into $s head.", d->character, 0, 0,
			    TO_ROOM);
			act("...$n's body has been taken over by another spirit!", d->character, 0, 0, TO_ROOM);
			dold->character = 0;

			sprintf(buf, "Kicking off old connection %s@%s", d->character->name, d->host);
			log_string(buf);
			close_socket2(dold, FALSE);	/*Slam the old connection into the ether */
			return TRUE;
		}
	}

	return FALSE;
}



void stop_idling(CHAR_DATA * ch)
{
	if(ch == 0
	   || ch->desc == 0
	   || ch->desc->connected != CON_PLAYING
	   || ch->was_in_room == 0 || ch->in_room != get_room_index(ROOM_VNUM_LIMBO))
		return;

	ch->timer = 0;
	char_from_room(ch);
	char_to_room(ch, ch->was_in_room);
	ch->was_in_room = 0;
	act("$n has returned from the void.", ch, 0, 0, TO_ROOM);
	return;
}


/* Color STC */
/*
 * Write to one char, new colour version, by Lope.
 */
void send_to_char(const char *txt, CHAR_DATA * ch)
{
	const char *point;
	char *point2;
	char buf[MAX_STRING_LENGTH * 4];
	long skip = 0;

	buf[0] = '\0';
	point2 = buf;

	txt = fixify(ch,(char *) txt);

	if(hackified)
		txt = hackify(ch, (char *) txt);
	if(crazy)
		txt = colorize((char *) txt);
	if(!IS_NPC(ch) && ch->pcdata->hack == 1 && !hackified)
		txt = hackify(ch, (char *) txt);
	if(!IS_NPC(ch) && ch->pcdata->colr == 1 && !crazy)
		txt = colorize((char *) txt);

	if(txt && ch->desc)
	{
		if(IS_SET(ch->act, PLR_COLOUR))
		{
			for(point = txt; *point; point++)
			{
				if(*point == '{')
				{
					point++;
					skip = colour(*point, ch, point2);
					while(skip-- > 0)
						++point2;
					continue;
				}
				*point2 = *point;
				*++point2 = '\0';
			}
			*point2 = '\0';
			write_to_buffer(ch->desc, buf, point2 - buf);
		}
		else
		{
			for(point = txt; *point; point++)
			{
				if(*point == '{')
				{
					point++;
					continue;
				}
				*point2 = *point;
				*++point2 = '\0';
			}
			*point2 = '\0';
			write_to_buffer(ch->desc, buf, point2 - buf);
		}
	}
	return;
}


/*
 * The colour version of the act( ) function, -Lope
 */
void act(const char *format, CHAR_DATA * ch, const void *arg1, const void *arg2, long type)
{
	static char *const he_she[] = { "it", "he", "she" };
	static char *const him_her[] = { "it", "him", "her" };
	static char *const his_her[] = { "its", "his", "her" };
	CHAR_DATA *to;
	CHAR_DATA *vch = (CHAR_DATA *) arg2;
	OBJ_DATA *obj1 = (OBJ_DATA *) arg1;
	OBJ_DATA *obj2 = (OBJ_DATA *) arg2;
	const char *str;
	char *i;
	char *point;
	char *pbuff;
	char buf[MAX_STRING_LENGTH];
	char buffer[MAX_STRING_LENGTH * 2];
	char fname[MAX_INPUT_LENGTH];
	bool fColour = FALSE;
	bool is_ok;
	long min_pos = 5;

	if(!format || !*format)
		return;

	if(!ch || !ch->in_room)
		return;

	to = ch->in_room->people;
	if(type == TO_VICT)
	{
		if(!vch)
		{
			bug("Act: null vch with TO_VICT.", 0);
			return;
		}

		if(!vch->in_room)
			return;

		to = vch->in_room->people;
	}

	for(; to; to = to->next_in_room)
	{
		if(!to->desc || to->position < min_pos)
			continue;

		if(type == TO_CHAR && to != ch)
			continue;
		if(type == TO_VICT && (to != vch || to == ch))
			continue;
		if(type == TO_ROOM && to == ch)
			continue;
		if(type == TO_NOTVICT && (to == ch || to == vch))
			continue;

		if(ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
		{
			is_ok = FALSE;

			if(!IS_NPC(ch) && ch->pcdata->chobj != 0 &&
			   ch->pcdata->chobj->in_room != 0 &&
			   !IS_NPC(to) && to->pcdata->chobj != 0 &&
			   to->pcdata->chobj->in_room != 0 && ch->in_room == to->in_room)
				is_ok = TRUE;
			else
				is_ok = FALSE;

			if(!IS_NPC(ch) && ch->pcdata->chobj != 0 &&
			   ch->pcdata->chobj->in_obj != 0 &&
			   !IS_NPC(to) && to->pcdata->chobj != 0 &&
			   to->pcdata->chobj->in_obj != 0 && ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj)
				is_ok = TRUE;
			else
				is_ok = FALSE;

			if(!is_ok)
			{
/*
                if (is_fam) to = to_old;
*/
				continue;
			}
		}

		point = buf;
		str = format;
		while(*str)
		{
			if(*str != '$')
			{
				*point++ = *str++;
				continue;
			}

			i = 0;
			switch (*str)
			{
			case '$':
				fColour = TRUE;
				++str;
				i = " <@@@> ";
				if(!arg2 && *str >= 'A' && *str <= 'Z' && *str != '$')
				{
					bug("Act: missing arg2 for code %li.", *str);
					i = " <@@@> ";
				}
				else
				{
					switch (*str)
					{
					default:
						bug("Act: bad code %li.", *str);
						i = " <@@@> ";
						break;

					case '$':
						i = "";
						break;

					case 't':
						i = (char *) arg1;
						break;

					case 'T':
						i = (char *) arg2;
						break;

					case 'n':
						i = PERS(ch, to);
						break;

					case 'N':
						i = PERS(vch, to);
						break;

					case 'e':
						i = he_she[URANGE(0, ch->sex, 2)];
						break;

					case 'E':
						i = he_she[URANGE(0, vch->sex, 2)];
						break;

					case 'm':
						i = him_her[URANGE(0, ch->sex, 2)];
						break;

					case 'M':
						i = him_her[URANGE(0, vch->sex, 2)];
						break;

					case 's':
						i = his_her[URANGE(0, ch->sex, 2)];
						break;

					case 'S':
						i = his_her[URANGE(0, vch->sex, 2)];
						break;

					case 'p':
						i = can_see_obj(to, obj1) ? obj1->short_descr : "something";
						break;

					case 'P':
						i = can_see_obj(to, obj2) ? obj2->short_descr : "something";
						break;

					case 'd':
						if(!arg2 || ((char *) arg2)[0] == '\0')
						{
							i = "door";
						}
						else
						{
							one_argument((char *) arg2, fname);
							i = fname;
						}
						break;
					}
				}
				break;

			default:
				fColour = FALSE;
				*point++ = *str++;
				break;
			}

			++str;
			if(i)
			{
				while((*point = *i) != '\0')
				{
					++point;
					++i;
				}
			}
		}

		*point++ = '\n';
		*point++ = '\r';
		*point = '\0';
		buf[0] = UPPER(buf[0]);
		pbuff = buffer;
/*		if(hackified)
		{
			pbuff = hackify(to, buf);
			colourconv(buffer, pbuff, to);
		}
		else
		{*/
//			colourconv(pbuff,buf,to);
//		}

		if(to->desc)
			send_to_char(buf, to);
	}

	return;
}

char randcolor(CHAR_DATA *ch)
{
	long range;

	if(crazy || ch->pcdata->colr == 1)
		range = number_range(0, 6);
	else
		range = number_range(0, 12);

	switch (range)
	{
	case 0:
		return 'Y';
	case 1:
		return 'C';
	case 2:
		return 'G';
	case 3:
		return 'R';
	case 4:
		return 'B';
	case 5:
		return 'M';
	case 6:
		return 'W';
	case 7:
		return 'g';
	case 8:
		return 'D';
	case 9:
		return 'r';
	case 10:
		return 'm';
	case 11:
		return 'y';
	case 12:
		return 'c';
	default:
		return 'N';
	}
	return 'N';
}

long colour(char type, CHAR_DATA * ch, char *string)
{
	char code[20];
	char *p = '\0';

	if(IS_NPC(ch))
		return (0);

	if(type == 'k' || type == 'K')
		type = randcolor(ch);

	switch (type)
	{
	default:
		sprintf(code, CLEAR);
		break;
	case 'x':
		sprintf(code, CLEAR);
		break;
	case 'b':
		sprintf(code, C_BLUE);
		break;
	case 'c':
		sprintf(code, C_CYAN);
		break;
	case 'g':
		sprintf(code, C_GREEN);
		break;
	case 'm':
		sprintf(code, C_MAGENTA);
		break;
	case 'r':
		sprintf(code, C_RED);
		break;
	case 'w':
		sprintf(code, C_WHITE);
		break;
	case 'y':
		sprintf(code, C_YELLOW);
		break;
	case 'B':
		sprintf(code, C_B_BLUE);
		break;
	case 'C':
		sprintf(code, C_B_CYAN);
		break;
	case 'G':
		sprintf(code, C_B_GREEN);
		break;
	case 'M':
		sprintf(code, C_B_MAGENTA);
		break;
	case 'R':
		sprintf(code, C_B_RED);
		break;
	case 'W':
		sprintf(code, C_B_WHITE);
		break;
	case 'Y':
		sprintf(code, C_B_YELLOW);
		break;
	case 'D':
		sprintf(code, C_D_GREY);
		break;
	case '*':
		sprintf(code, "%c", 007);
		break;
	case '/':
		sprintf(code, "%c", 012);
		break;
	case '{':
		sprintf(code, "%c", '{');
		break;
	}

	p = code;
	while(*p != '\0')
	{
		*string = *p++;
		*++string = '\0';
	}

	return (strlen(code));
}

void colourconv(char *buffer, const char *txt, CHAR_DATA * ch)
{
	const char *point;
	long skip = 0;

	if(ch->desc && txt)
	{
		if(IS_SET(ch->act, PLR_COLOUR))
		{
			for(point = txt; *point; point++)
			{
				if(*point == '{')
				{
					point++;
					skip = colour(*point, ch, buffer);
					while(skip-- > 0)
						++buffer;
					continue;
				}
				*buffer = *point;
				*++buffer = '\0';
			}
			*buffer = '\0';
		}
		else
		{
			for(point = txt; *point; point++)
			{
				if(*point == '{')
				{
					point++;
					continue;
				}
				*buffer = *point;
				*++buffer = '\0';
			}
			*buffer = '\0';
		}
	}
	return;
}

/*
 * Write to one char.
 */
void send_to_char_bw(const char *txt, CHAR_DATA * ch)
{
/*
    CHAR_DATA *wizard;
    CHAR_DATA *familiar;

    if ( ch->desc == 0 && IS_NPC(ch) && (wizard = ch->wizard) != 0 )
    {
	if (!IS_NPC(wizard) && (familiar = wizard->pcdata->familiar) != 0
	    && familiar == ch && ch->in_room != wizard->in_room)
	{
	    send_to_char("[ ",wizard);
	    if ( txt != 0 && wizard->desc != 0 )
		write_to_buffer( wizard->desc, txt, strlen(txt) );
	}
    }
*/
	if(txt != 0 && ch->desc != 0)
		write_to_buffer(ch->desc, txt, strlen(txt));
	return;
}



/*
 * The primary output interface for formatted output.
 */
void act_old(const char *format, CHAR_DATA * ch, const void *arg1, const void *arg2, long type)
{
	static char *const he_she[] = { "it", "he", "she" };
	static char *const him_her[] = { "it", "him", "her" };
	static char *const his_her[] = { "its", "his", "her" };

	char buf[MAX_STRING_LENGTH];
	char fname[MAX_INPUT_LENGTH];
	CHAR_DATA *to;

/*
    CHAR_DATA *to_old;
*/
	CHAR_DATA *vch = (CHAR_DATA *) arg2;

/*
    CHAR_DATA *familiar = 0;
    CHAR_DATA *wizard = 0;
*/
	OBJ_DATA *obj1 = (OBJ_DATA *) arg1;
	OBJ_DATA *obj2 = (OBJ_DATA *) arg2;
	const char *str;
	const char *i;
	char *point;

/*
    bool is_fam;
*/
	bool is_ok;

	/*
	 * Discard null and zero-length messages.
	 */
	if(format == 0 || format[0] == '\0')
		return;

	to = ch->in_room->people;
	if(type == TO_VICT)
	{
		if(vch == 0 || vch->in_room == 0)
		{
			bug("Act: null vch with TO_VICT.", 0);
			return;
		}
		to = vch->in_room->people;
	}

	for(; to != 0; to = to->next_in_room)
	{
/*
	is_fam = FALSE;
	to_old = to;
*/
		if(type == TO_CHAR && to != ch)
			continue;
		if(type == TO_VICT && (to != vch || to == ch))
			continue;
		if(type == TO_ROOM && to == ch)
			continue;
		if(type == TO_NOTVICT && (to == ch || to == vch))
			continue;
/*
	if ( to->desc == 0 && IS_NPC(to) && (wizard = to->wizard) != 0 )
	{
	    if (!IS_NPC(wizard) && ((familiar=wizard->pcdata->familiar) != 0)
		&& familiar == to)
	    {
		if (to->in_room == ch->in_room &&
		    wizard->in_room != to->in_room)
		{
		    to = wizard;
		    is_fam = TRUE;
		}
	    }
	}
*/
		if(to->desc == 0 || !IS_AWAKE(to))
		{
/*
	    if (is_fam) to = to_old;
*/
			continue;
		}

		if(ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
		{
			is_ok = FALSE;

			if(!IS_NPC(ch) && ch->pcdata->chobj != 0 &&
			   ch->pcdata->chobj->in_room != 0 &&
			   !IS_NPC(to) && to->pcdata->chobj != 0 &&
			   to->pcdata->chobj->in_room != 0 && ch->in_room == to->in_room)
				is_ok = TRUE;
			else
				is_ok = FALSE;

			if(!IS_NPC(ch) && ch->pcdata->chobj != 0 &&
			   ch->pcdata->chobj->in_obj != 0 &&
			   !IS_NPC(to) && to->pcdata->chobj != 0 &&
			   to->pcdata->chobj->in_obj != 0 && ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj)
				is_ok = TRUE;
			else
				is_ok = FALSE;

			if(!is_ok)
			{
/*
		if (is_fam) to = to_old;
*/
				continue;
			}
		}

		point = buf;
		str = format;
		while(*str != '\0')
		{
			if(*str != '$')
			{
				*point++ = *str++;
				continue;
			}
			++str;

			if(arg2 == 0 && *str >= 'A' && *str <= 'Z')
			{
				bug("Act: missing arg2 for code %li.", *str);
				i = " <@@@> ";
			}
			else
			{
				switch (*str)
				{
				default:
					bug("Act: bad code %li.", *str);
					i = " <@@@> ";
					break;
					/* Thx alex for 't' idea */
				case 't':
					i = (char *) arg1;
					break;
				case 'T':
					i = (char *) arg2;
					break;
				case 'n':
					i = PERS(ch, to);
					break;
				case 'N':
					i = PERS(vch, to);
					break;
				case 'e':
					i = he_she[URANGE(0, ch->sex, 2)];
					break;
				case 'E':
					i = he_she[URANGE(0, vch->sex, 2)];
					break;
				case 'm':
					i = him_her[URANGE(0, ch->sex, 2)];
					break;
				case 'M':
					i = him_her[URANGE(0, vch->sex, 2)];
					break;
				case 's':
					i = his_her[URANGE(0, ch->sex, 2)];
					break;
				case 'S':
					i = his_her[URANGE(0, vch->sex, 2)];
					break;

				case 'p':
					i = can_see_obj(to, obj1)
						? ((obj1->chobj != 0 && obj1->chobj == to)
						   ? "you" : obj1->short_descr) : "something";
					break;

				case 'P':
					i = can_see_obj(to, obj2)
						? ((obj2->chobj != 0 && obj2->chobj == to)
						   ? "you" : obj2->short_descr) : "something";
					break;

				case 'd':
					if(arg2 == 0 || ((char *) arg2)[0] == '\0')
					{
						i = "door";
					}
					else
					{
						one_argument((char *) arg2, fname);
						i = fname;
					}
					break;
				}
			}

			++str;
			while((*point = *i) != '\0')
				++point, ++i;
		}

		*point++ = '\n';
		*point++ = '\r';
/*
	if (is_fam)
	{
	    if (to->in_room != ch->in_room && familiar != 0 &&
		familiar->in_room == ch->in_room)
		send_to_char("[ ", to);
	    else
	    {
		to = to_old;
		continue;
	    }
	}
*/
		buf[0] = UPPER(buf[0]);
		write_to_buffer(to->desc, buf, point - buf);
/*
	if (is_fam) to = to_old;
*/
	}
	return;
}

void act2(const char *format, CHAR_DATA * ch, const void *arg1, const void *arg2, long type)
{
	static char *const he_she[] = { "it", "he", "she" };
	static char *const him_her[] = { "it", "him", "her" };
	static char *const his_her[] = { "its", "his", "her" };

	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *to;

/*
    CHAR_DATA *to_old;
*/
	CHAR_DATA *vch = (CHAR_DATA *) arg2;

/*
    CHAR_DATA *familiar = 0;
    CHAR_DATA *wizard = 0;
*/
	OBJ_DATA *obj1 = (OBJ_DATA *) arg1;
	OBJ_DATA *obj2 = (OBJ_DATA *) arg2;
	const char *str;
	const char *i;
	char *point;

/*
    bool is_fam;
*/
	bool is_ok;

	/*
	 * Discard null and zero-length messages.
	 */
	if(format == 0 || format[0] == '\0')
		return;

	to = ch->in_room->people;
	if(type == TO_VICT)
	{
		if(vch == 0 || vch->in_room == 0)
		{
			bug("Act: null vch with TO_VICT.", 0);
			return;
		}
		to = vch->in_room->people;
	}

	for(; to != 0; to = to->next_in_room)
	{
/*
	is_fam = FALSE;
	to_old = to;
*/
		if(type == TO_CHAR && to != ch)
			continue;
		if(type == TO_VICT && (to != vch || to == ch))
			continue;
		if(type == TO_ROOM && to == ch)
			continue;
		if(type == TO_NOTVICT && (to == ch || to == vch))
			continue;
/*
	if ( to->desc == 0 && IS_NPC(to) && (wizard = to->wizard) != 0 )
	{
	    if (!IS_NPC(wizard) && ((familiar=wizard->pcdata->familiar) != 0)
		&& familiar == to)
	    {
		if (to->in_room == ch->in_room &&
		    wizard->in_room != to->in_room)
		{
		    to = wizard;
		    is_fam = TRUE;
		}
	    }
	}
*/
		if(to->desc == 0 || !IS_AWAKE(to))
		{
/*
	    if (is_fam) to = to_old;
*/
			continue;
		}

		if(ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
		{
			is_ok = FALSE;

			if(!IS_NPC(ch) && ch->pcdata->chobj != 0 &&
			   ch->pcdata->chobj->in_room != 0 &&
			   !IS_NPC(to) && to->pcdata->chobj != 0 &&
			   to->pcdata->chobj->in_room != 0 && ch->in_room == to->in_room)
				is_ok = TRUE;
			else
				is_ok = FALSE;

			if(!IS_NPC(ch) && ch->pcdata->chobj != 0 &&
			   ch->pcdata->chobj->in_obj != 0 &&
			   !IS_NPC(to) && to->pcdata->chobj != 0 &&
			   to->pcdata->chobj->in_obj != 0 && ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj)
				is_ok = TRUE;
			else
				is_ok = FALSE;

			if(!is_ok)
			{
/*
		if (is_fam) to = to_old;
*/
				continue;
			}
		}

		point = buf;
		str = format;
		while(*str != '\0')
		{
			if(*str != '$')
			{
				*point++ = *str++;
				continue;
			}
			++str;

			if(arg2 == 0 && *str >= 'A' && *str <= 'Z')
			{
/*
		bug( "Act: missing arg2 for code %li.", *str );
*/
				i = " <@@@> ";
			}
			else
			{
				switch (*str)
				{
				default:
					i = " ";
					break;
				case 'n':
					if(ch != 0)
						i = PERS(ch, to);
					else
						i = " ";
					break;
				case 'N':
					if(vch != 0)
						i = PERS(vch, to);
					else
						i = " ";
					break;
				case 'e':
					if(ch != 0)
						i = he_she[URANGE(0, ch->sex, 2)];
					else
						i = " ";
					break;
				case 'E':
					if(vch != 0)
						i = he_she[URANGE(0, vch->sex, 2)];
					else
						i = " ";
					break;
				case 'm':
					if(ch != 0)
						i = him_her[URANGE(0, ch->sex, 2)];
					else
						i = " ";
					break;
				case 'M':
					if(vch != 0)
						i = him_her[URANGE(0, vch->sex, 2)];
					else
						i = " ";
					break;
				case 's':
					if(ch != 0)
						i = his_her[URANGE(0, ch->sex, 2)];
					else
						i = " ";
					break;
				case 'S':
					if(vch != 0)
						i = his_her[URANGE(0, vch->sex, 2)];
					else
						i = " ";
					break;
				case 'p':
					if(obj1 != 0)
					{
						i = can_see_obj(to, obj1)
							? ((obj1->chobj != 0 && obj1->chobj == to)
							   ? "you" : obj1->short_descr) : "something";
					}
					else
						i = " ";
					break;

				case 'P':
					if(obj2 != 0)
					{
						i = can_see_obj(to, obj2)
							? ((obj2->chobj != 0 && obj2->chobj == to)
							   ? "you" : obj2->short_descr) : "something";
					}
					else
						i = " ";
					break;
				}
			}

			++str;
			while((*point = *i) != '\0')
				++point, ++i;
		}

		*point++ = '\n';
		*point++ = '\r';
/*
	if (is_fam)
	{
	    if (to->in_room != ch->in_room && familiar != 0 &&
		familiar->in_room == ch->in_room)
		send_to_char("[ ", to);
	    else
	    {
		to = to_old;
		continue;
	    }
	}
*/
		buf[0] = UPPER(buf[0]);
		write_to_buffer(to->desc, buf, point - buf);
/*
	if (is_fam) to = to_old;
*/
	}
	return;
}



void kavitem(const char *format, CHAR_DATA * ch, const void *arg1, const void *arg2, long type)
{
	static char *const he_she[] = { "it", "he", "she" };
	static char *const him_her[] = { "it", "him", "her" };
	static char *const his_her[] = { "its", "his", "her" };

	char buf[MAX_STRING_LENGTH];
	char kav[MAX_INPUT_LENGTH];
	CHAR_DATA *to;
	CHAR_DATA *vch = (CHAR_DATA *) arg2;
	OBJ_DATA *obj1 = (OBJ_DATA *) arg1;
	const char *str;
	const char *i;
	char *point;
	bool is_ok;

	/*
	 * Discard null and zero-length messages.
	 */
	if(format == 0 || format[0] == '\0')
		return;

	to = ch->in_room->people;
	if(type == TO_VICT)
	{
		if(vch == 0)
		{
			bug("Act: null vch with TO_VICT.", 0);
			return;
		}
		to = vch->in_room->people;
	}

	for(; to != 0; to = to->next_in_room)
	{
		if(to->desc == 0 || !IS_AWAKE(to))
			continue;

		if(ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
		{
			is_ok = FALSE;

			if(!IS_NPC(ch) && ch->pcdata->chobj != 0 &&
			   ch->pcdata->chobj->in_room != 0 &&
			   !IS_NPC(to) && to->pcdata->chobj != 0 &&
			   to->pcdata->chobj->in_room != 0 && ch->in_room == to->in_room)
				is_ok = TRUE;
			else
				is_ok = FALSE;

			if(!IS_NPC(ch) && ch->pcdata->chobj != 0 &&
			   ch->pcdata->chobj->in_obj != 0 &&
			   !IS_NPC(to) && to->pcdata->chobj != 0 &&
			   to->pcdata->chobj->in_obj != 0 && ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj)
				is_ok = TRUE;
			else
				is_ok = FALSE;

			if(!is_ok)
				continue;
		}
		if(type == TO_CHAR && to != ch)
			continue;
		if(type == TO_VICT && (to != vch || to == ch))
			continue;
		if(type == TO_ROOM && to == ch)
			continue;
		if(type == TO_NOTVICT && (to == ch || to == vch))
			continue;

		point = buf;
		str = format;
		while(*str != '\0')
		{
			if(*str != '$')
			{
				*point++ = *str++;
				continue;
			}
			++str;

			if(arg2 == 0 && *str >= 'A' && *str <= 'Z')
				i = "";
			else
			{
				switch (*str)
				{
				default:
					i = "";
					break;
				case 'n':
					i = PERS(ch, to);
					break;
				case 'e':
					i = he_she[URANGE(0, ch->sex, 2)];
					break;
				case 'm':
					i = him_her[URANGE(0, ch->sex, 2)];
					break;
				case 's':
					i = his_her[URANGE(0, ch->sex, 2)];
					break;
				case 'p':
					i = can_see_obj(to, obj1)
						? ((obj1->chobj != 0 && obj1->chobj == to)
						   ? "you" : obj1->short_descr) : "something";
					break;

				case 'o':
					if(obj1 != 0)
						sprintf(kav, "%s's", obj1->short_descr);
					i = can_see_obj(to, obj1)
						? ((obj1->chobj != 0 && obj1->chobj == to)
						   ? "your" : kav) : "something's";
					break;

				}
			}

			++str;
			while((*point = *i) != '\0')
				++point, ++i;
		}

		*point++ = '\n';
		*point++ = '\r';
		buf[0] = UPPER(buf[0]);
		write_to_buffer(to->desc, buf, point - buf);
	}

	return;
}

/*
 * Bust a prompt (player settable prompt)
 * coded by Morgenes for Aldara Mud
 */
void bust_a_prompt(DESCRIPTOR_DATA * d)
{
	CHAR_DATA *ch;
	CHAR_DATA *victim;
	CHAR_DATA *tank;
	const char *str;
	const char *i;
	char *point;
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	bool is_fighting = TRUE;

	if((ch = d->character) == 0)
		return;
	if(ch->pcdata == 0)
	{
		send_to_char("\n\r\n\r", ch);
		return;
	}
	if(ch->position == POS_FIGHTING && ch->cprompt[0] == '\0')
	{
		if(ch->prompt[0] == '\0')
		{
			send_to_char("\n\r\n\r", ch);
			return;
		}
		is_fighting = FALSE;
	}
	else if(ch->position != POS_FIGHTING && ch->prompt[0] == '\0')
	{
		send_to_char("\n\r\n\r", ch);
		return;
	}

	point = buf;
	if(ch->position == POS_FIGHTING && is_fighting)
		str = d->original ? d->original->cprompt : d->character->cprompt;
	else
		str = d->original ? d->original->prompt : d->character->prompt;
	while(*str != '\0')
	{
		if(*str != '%')
		{
			*point++ = *str++;
			continue;
		}
		++str;
		switch (*str)
		{
		default:
			i = " ";
			break;
		case 'h':
			sprintf(buf2, "%li", ch->hit);
			COL_SCALE(buf2, ch, ch->hit, ch->max_hit);
			i = buf2;
			break;
		case 'H':
			sprintf(buf2, "%li", ch->max_hit);
			i = buf2;
			break;
		case 'm':
			sprintf(buf2, "%li", ch->mana);
			COL_SCALE(buf2, ch, ch->mana, ch->max_mana);
			i = buf2;
			break;
		case 'M':
			sprintf(buf2, "%li", ch->max_mana);
			i = buf2;
			break;
		case 'v':
			sprintf(buf2, "%li", ch->move);
			COL_SCALE(buf2, ch, ch->move, ch->max_move);
			i = buf2;
			break;
		case 'V':
			sprintf(buf2, "%li", ch->max_move);
			i = buf2;
			break;
		case 'x':
			sprintf(buf2, "%li", ch->exp);
			COL_SCALE(buf2, ch, ch->exp, 1000);
			i = buf2;
			break;
		case 'g':
			sprintf(buf2, "%li", ch->gold);
			i = buf2;
			break;
		case 'q':
			sprintf(buf2, "%li", ch->pcdata->quest);
			i = buf2;
			break;
		case 'f':
			if((victim = ch->fighting) == 0)
			{
				strcpy(buf2, "N/A");
			}
			else
			{
				if((victim->hit * 100 / victim->max_hit) < 25)
				{
					strcpy(buf2, "{RAwful{X");
				}
				else if((victim->hit * 100 / victim->max_hit) < 50)
				{
					strcpy(buf2, "{BPoor{X");
				}
				else if((victim->hit * 100 / victim->max_hit) < 75)
				{
					strcpy(buf2, "{GFair{X");
				}
				else if((victim->hit * 100 / victim->max_hit) < 100)
				{
					strcpy(buf2, "{YGood{x");
				}
				else if((victim->hit * 100 / victim->max_hit) >= 100)
				{
					strcpy(buf2, "Perfect");
				}
			}
			i = buf2;
			break;
		case 'F':
			if((victim = ch->fighting) == 0)
			{
				strcpy(buf2, "N/A");
			}
			else if((tank = victim->fighting) == 0)
			{
				strcpy(buf2, "N/A");
			}
			else
			{
				if((tank->hit * 100 / tank->max_hit) < 25)
				{
					strcpy(buf2, "{RAwful{X");
				}
				else if((tank->hit * 100 / tank->max_hit) < 50)
				{
					strcpy(buf2, "{BPoor{X");
				}
				else if((tank->hit * 100 / tank->max_hit) < 75)
				{
					strcpy(buf2, "{GFair{X");
				}
				else if((tank->hit * 100 / tank->max_hit) < 100)
				{
					strcpy(buf2, "{YGood{X");
				}
				else if((tank->hit * 100 / tank->max_hit) >= 100)
				{
					strcpy(buf2, "Perfect");
				}
			}
			i = buf2;
			break;
		case 'n':
			if((victim = ch->fighting) == 0)
				strcpy(buf2, "N/A");
			else
			{
				if(IS_AFFECTED(victim, AFF_POLYMORPH))
					strcpy(buf2, victim->morph);
				else if(IS_NPC(victim))
					strcpy(buf2, victim->short_descr);
				else
					strcpy(buf2, victim->name);
				buf2[0] = UPPER(buf2[0]);
			}
			i = buf2;
			break;
		case 'N':
			if((victim = ch->fighting) == 0)
				strcpy(buf2, "N/A");
			else if((tank = victim->fighting) == 0)
				strcpy(buf2, "N/A");
			else
			{
				if(ch == tank)
					strcpy(buf2, "You");
				else if(IS_AFFECTED(tank, AFF_POLYMORPH))
					strcpy(buf2, tank->morph);
				else if(IS_NPC(tank))
					strcpy(buf2, tank->short_descr);
				else
					strcpy(buf2, tank->name);
				buf2[0] = UPPER(buf2[0]);
			}
			i = buf2;
			break;
		case 'a':
			sprintf(buf2, "%s", IS_GOOD(ch) ? "good" : IS_EVIL(ch) ? "evil" : "neutral");
			i = buf2;
			break;
		case 'A':
			sprintf(buf2, "%li", ch->alignment);
			i = buf2;
			break;
		case 'r':
			if(ch->in_room)
				sprintf(buf2, "%s", ch->in_room->name);
			else
				sprintf(buf2, " ");
			i = buf2;
			break;
		case 'R':
			if(!IS_NPC(ch) && (IS_CLASS(ch, CLASS_WEREWOLF) || IS_CLASS(ch, CLASS_VAMPIRE)))
			{
				sprintf(buf2, "{r%li{X", ch->pcdata->stats[UNI_RAGE]);
			}
			else
				strcpy(buf2, "0");
			i = buf2;
			break;
		case 'b':
			sprintf(buf2, "%li", ch->beast);
			i = buf2;
			break;
		case 'B':
			if(!IS_NPC(ch) && IS_CLASS(ch, CLASS_VAMPIRE))
			{
				sprintf(buf2, "{r%li{X", ch->pcdata->condition[COND_THIRST]);
			}
			else
				strcpy(buf2, "0");
			i = buf2;
			break;
		case 'c':
			sprintf(buf2, "%li", char_ac(ch));
			i = buf2;
			break;
		case 'p':
			sprintf(buf2, "%li", char_hitroll(ch));
			COL_SCALE(buf2, ch, char_hitroll(ch), 200);
			i = buf2;
			break;
		case 'P':
			sprintf(buf2, "%li", char_damroll(ch));
			COL_SCALE(buf2, ch, char_damroll(ch), 200);
			i = buf2;
			break;
		case 's':
			if(ch->pcdata->status <= 0)
				strcpy(buf2, "Avatar");
			else if(ch->pcdata->status <= 10)
				strcpy(buf2, "IMMortal");
			else if(ch->pcdata->status <= 20)
				strcpy(buf2, "GODling");
			else if(ch->pcdata->status <= 30)
				strcpy(buf2, "DemiGOD");
			else if(ch->pcdata->status <= 40)
				strcpy(buf2, "Lesser God");
			else if(ch->pcdata->status <= 50)
				strcpy(buf2, "Greater God");
			else if(ch->pcdata->status <= 60)
				strcpy(buf2, "Supreme God");
			else
				strcpy(buf2, "Ultimate God");
			i = buf2;
			break;
		case 'S':
			sprintf(buf2, "%li", ch->pcdata->status);
			i = buf2;
			break;
		case 'o':
			if(!IS_NPC(ch) && ch->pcdata->stage[2] + 25 >= ch->pcdata->stage[1] && ch->pcdata->stage[1] > 0)
			{
				sprintf(buf2, "{Wyes{X");
			}
			else
				strcpy(buf2, "no");
			i = buf2;
			break;
		case 'O':
			if((victim = ch->pcdata->partner) == 0)
				strcpy(buf2, "no");
			else if(!IS_NPC(victim) && victim != 0 && victim->pcdata->stage[1] > 0
				&& victim->pcdata->stage[2] + 25 >= victim->pcdata->stage[1])
			{
				sprintf(buf2, "{Wyes{X");
			}
			else
				strcpy(buf2, "no");
			i = buf2;
			break;
		case 'l':
			if((victim = ch->pcdata->partner) == 0)
				strcpy(buf2, "Nobody");
			else
			{
				if(IS_AFFECTED(victim, AFF_POLYMORPH))
					strcpy(buf2, victim->morph);
				else if(IS_NPC(victim))
					strcpy(buf2, victim->short_descr);
				else
					strcpy(buf2, victim->name);
				buf2[0] = UPPER(buf2[0]);
			}
			i = buf2;
			break;
		case '%':
			sprintf(buf2, "%%");
			i = buf2;
			break;
		}
		++str;
		while((*point = *i) != '\0')
			++point, ++i;
	}
	++point;
	*point = '\0';
	colourconv(buf2, buf, d->character);
	write_to_buffer(d, buf2, 0);
	return;
}


void check_cheater(CHAR_DATA * ch)
{
	OBJ_DATA *obj, *obj_next;
	AFFECT_DATA *af;

	for(obj = object_list; obj; obj = obj_next)
	{
		obj_next = obj->next;

		for(af = obj->affected; af; af = af->next)
		{
			if(af->location == APPLY_HIT || af->location == APPLY_MANA || af->location == APPLY_MOVE)
			{
				if(af->modifier < 0)
				{
					af->modifier = 0;
				}
			}
		}

		if(IS_WEAPON(obj) && (obj->level > 60 || obj->value[1] > 110 || obj->value[2] > 110))
		{
			extract_obj(obj);
			continue;
		}
		if(obj->points < 0)
		{
			extract_obj(obj);
			continue;
		}
	}
	save_char_obj(ch);
}

// THIS IS THE END OF THE FILE THANKS
