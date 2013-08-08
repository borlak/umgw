#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


#define PING_FILE "../ping/PING"

void player_ping(CHAR_DATA * ch)
{
	char buf[256];

	if(!ch || !ch->desc || !ch->desc->host)
		return;

	sprintf(buf, "ping -c 1 %s > %s%s &", ch->desc->host, PING_FILE, ch->name);

	//system(buf);
}


void ping_players(void)
{
	DESCRIPTOR_DATA *d;

	for(d = descriptor_list; d; d = d->next)
	{
		if(!d->character || d->connected != CON_PLAYING)
			continue;

		player_ping(d->character);
	}
}


void get_player_pings(void)
{
	DESCRIPTOR_DATA *d;
	char buf[MAX_STRING_LENGTH];
	char pbuf[2048];
	long ping;
	long i, x, length;
	FILE *fp;

	fclose(fpReserve);

	for(d = descriptor_list; d; d = d->next)
	{
		if(!d->character || d->connected != CON_PLAYING)
			continue;

		sprintf(buf, "%s%s", PING_FILE, d->character->name);

		if((fp = fopen(buf, "r")) == 0)
		{
			d->ping = -1;
			continue;
		}

		length = fread(buf, 1, MAX_STRING_LENGTH - 1, fp);
		buf[length] = '\0';

		for(x = 0; x < length; x++)
		{
			if(buf[x] == 't' && buf[x + 1] == 'i')
				break;
		}

		if(x >= length)
		{
			d->ping = -1;
			fclose(fp);
			continue;
		}

		for(x = x + 5, i = 0; buf[x] != '.' && buf[x] != '\0'; x++, i++)
			pbuf[i] = buf[x];

		pbuf[i] = '\0';
		ping = atoi(pbuf);
//              if( !str_cmp(d->character->name,"pip") )
 //                     d->ping = number_range(1,900);
   //            else
		d->ping = ping == 0 ? -1 : ping;
		fclose(fp);
	}

	fpReserve = fopen(NULL_FILE, "r");
}








// THIS IS THE END OF THE FILE THANKS
