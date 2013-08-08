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
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

#define	VT_CLEAR	"\033[2J"
#define VT_SCROLL	"\033[%d;%dr"
#define VT_POSITION	"\033[%d;%dH"

#define VT_WHITE	"\033[1m"
#define VT_BLINK	"\033[5m"
#define VT_BLACK	"\033[30m"
#define VT_RED		"\033[31m"
#define VT_GREEN	"\033[32m"
#define VT_YELLOW	"\033[33m"
#define VT_BLUE		"\033[34m"
#define VT_MAGENTA	"\033[35m"
#define VT_CYAN		"\033[36m"
#define VT_BLACKB	"\033[40m"
#define VT_REDB		"\033[41m"
#define VT_GREENB	"\033[42m"
#define VT_YELLOWB	"\033[43m"
#define VT_BLUEB	"\033[44m"
#define VT_MAGENTAB	"\033[45m"
#define VT_CYANB	"\033[46m"
#define VT_WHITEB	"\033[47m"

void init_vt(CHAR_DATA *ch)
{
	char buf[MAX_STRING_LENGTH];
	int x, y;

	if(ch->rows < 24 || ch->rows > 200)
	{
		send_to_char("Setting you to 24 rows (minimum).  If you do not have 24 rows, get a life.\n\r",ch);
		ch->rows = 24;
	}
	if(ch->cols < 80 || ch->cols > 200)
	{
		send_to_char("Setting you to 80 columns (minimum).  If you do not have 80 columns, get a life\n\r",ch);
		ch->cols = 80;
	}

	buf[0] = '\0';
	sprintf(buf,VT_CLEAR); send_to_char(buf,ch);
	sprintf(buf,"\033[12;24r"); send_to_char(buf,ch);
	sprintf(buf,"\033[0;0H"); send_to_char(buf,ch);

	sprintf(buf,"%s",VT_BLUEB);
	for(x=0;x < ch->rows;x++)
	{
		for(y=0;y < ch->cols;y++)
		{
			sprintf(buf+strlen(buf),"%c",219);
		}
		sprintf(buf+strlen(buf),"\n\r");
	}
	send_to_char(buf,ch);
}
