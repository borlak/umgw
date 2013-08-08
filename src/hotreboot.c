// merc hotreboot modified by borlak

#include <sys/types.h>
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

#define CH(descriptor)  ((descriptor)->original ? (descriptor)->original : (descriptor)->character)

#define HOTREBOOT_FILE	"../area/hotreboot"
#define OLDUPTIME_FILE	"../area/olduptime"
#define EXE_FILE	"../src/merc"

extern long port, control, rebootnum;
long olduptime = 0;


void prepare_players(void)
{
	DESCRIPTOR_DATA *d;

	for(d = descriptor_list; d; d = d->next)
	{
		if(d->connected != CON_PLAYING || !d->character)
			continue;

		stop_fighting(d->character, TRUE);
		d->character->position = POS_STANDING;
		do_restore(d->character, "self");
	}

	do_forceauto(char_list, "call all");
	do_autosave(char_list, "");
}


long get_olduptime(void)
{
	FILE *fp;
	long uptime;

	if((fp = fopen(OLDUPTIME_FILE, "r")) == 0)
		return 0;

	if(check_empty_file(fp))
		return 0;

	uptime = fread_number(fp);

	fclose(fp);

	return uptime;
}

void do_hotreboot(CHAR_DATA * ch, char *argument)
{
	FILE *fp;
	DESCRIPTOR_DATA *d, *d_next;
	char buf[100], buf2[100], buf3[100];

	fp = fopen(HOTREBOOT_FILE, "w");

	if(!fp)
	{
		send_to_char("hotreboot file not writeable, aborted.\n\r", ch);
		fpReserve = fopen(NULL_FILE, "r");
		perror("do_hotreboot:fopen");
		return;
	}

	if(argument && !str_cmp(argument,"INFINITE LOOP!"))
	{
		sprintf(buf, "[HotReboot by Godwars - Infinite Loop!]\n\r");
	}
	if(argument && argument[0] != '\0')
	{
		sprintf(buf, "[HotReboot by Godwars - Crash!]\n\r");
	}
	else
	{
		do_restore(ch, "all");
		prepare_players();
		write_kingdoms();
		sprintf(buf, "[HotReboot by %s - You will automatically reconnect]\n\r", ch->name);
	}

	/* For each playing descriptor, save its state */
	for(d = descriptor_list; d; d = d_next)
	{
		CHAR_DATA *och = CH(d);

		d_next = d->next;	/* We delete from the list , so need to save this */

		if(!d->character || d->connected != CON_PLAYING || och->level == 1)	/* drop those logging on */
		{
			write_to_descriptor(d->descriptor, "\n\rSorry, we are rebooting. Come back in a few minutes.\n\r",
					    0);
			close_socket2(d, FALSE);	/* throw'em out */
		}
		else
		{
			fprintf(fp, "%li %s %s\n", d->descriptor, och->name, d->host);
			save_char_obj(och);
			write_to_descriptor(d->descriptor, buf, 0);
		}
	}

	fprintf(fp, "-1\n\n");
	fclose(fp);

	if((fp = fopen(OLDUPTIME_FILE, "w")) != 0)
	{
		fprintf(fp, "%.0f\n", get_uptime());
		fclose(fp);
	}
	else
	{
		unlink(OLDUPTIME_FILE);
	}

	fclose(fpReserve);

	/* exec - descriptors are inherited */

	sprintf(buf, "%li", port);
	sprintf(buf2, "%li", control);
	sprintf(buf3, "%li", rebootnum);
#ifndef WIN32
	execl(EXE_FILE, "Merc", buf, "hotreboot", buf2, buf3, (char *) 0);
#endif
	/* Failed - sucessful exec will not return */

	perror("do_hotreboot: execl");
	send_to_char("hotreboot FAILED!\n\r", ch);

	/* Here you might want to reopen fpReserve */
	fpReserve = fopen(NULL_FILE, "r");
}

/* Recover from a hotreboot - load players */
void hotreboot_recover()
{
	DESCRIPTOR_DATA *d;
	FILE *fp;
	char name[100];
	char host[MSL];
	long desc;
	bool fOld;

	fp = fopen(HOTREBOOT_FILE, "r");

	if(!fp)			/* there are some descriptors open which will hang forever then ? */
	{
		perror("hotreboot_recover:fopen");
		exit(1);
	}

/*	unlink (HOTREBOOT_FILE);  In case something crashes - doesn't prevent reading	*/

	for(;;)
	{
		fscanf(fp, "%li %s %s\n", &desc, name, host);
		if(desc == -1)
			break;

		/* Write something, and check if it goes error-free */
		if(!write_to_descriptor(desc, "\n\rRestoring from hotreboot...\n\r", 0))
		{
#ifdef WIN32
			closesocket(desc);

#else /*  */
			close(desc);	/* nope */
#endif /*  */
			continue;
		}

		d = alloc_perm(sizeof(DESCRIPTOR_DATA));
		init_descriptor(d, desc);	/* set up various stuff */

		d->host = str_dup(host);
		d->next = descriptor_list;
		descriptor_list = d;
		d->connected = CON_HOTREBOOT_RECOVER;	/* -15, so close_socket frees the char */


		/* Now, find the pfile */

		fOld = load_char_obj(d, name);

		if(!fOld)	/* Player file not found?! */
		{
			write_to_descriptor(desc, "\n\rSomehow, your character was lost in the hotreboot. Reconnect\n\r",
					    0);
			close_socket2(d, FALSE);
		}
		else		/* ok! */
		{
			/* Just In Case */
			if(!d->character->in_room)
				d->character->in_room = get_room_index(ROOM_VNUM_TEMPLE);

			/* Insert in the char_list */
			d->character->next = char_list;
			char_list = d->character;

			char_to_room(d->character, d->character->in_room);
			do_look(d->character, "");
			act("$n appears from the netherworld!", d->character, 0, 0, TO_ROOM);
			d->connected = CON_PLAYING;
		}

	}

	fclose(fp);
	olduptime = get_olduptime();

}




// THIS IS THE END OF THE FILE THANKS
