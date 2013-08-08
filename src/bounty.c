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

// Someone piss you off?  Put a bounty on his ass!  (this will let j00 doo it)

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


/*finishing borlak's bounty code. - pip 6/16/01
*/

void do_bounty(CHAR_DATA * ch, char *argument)
{
	char buf[MSL];
	CHAR_DATA *victim;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	long amount = 0;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if(arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char("Syntax: bounty [playername] [qp amount]\n\r", ch);
		return;
	}

	if((victim = get_char_world(ch, arg1)) == 0)
	{
		send_to_char("You can only place a bounty when a player is online.\n\r", ch);
		return;
	}
	if(IS_NPC(victim))
	{
		send_to_char("Kill your own mobs.\n\r", ch);
		return;
	}
	if(IS_IMMORTAL(victim))
	{
		send_to_char("Don't waste your money on them.  They are IMMORTAL!\n\r", ch);
		return;
	}

	if(victim == ch)
	{
		send_to_char("They are too stupid to bother with, keep your quest points!\n\r", ch);
		return;
	}

	if(!is_number(arg2))
	{
		send_to_char("[qp amount] must be in the form of a number.\n\r", ch);
		do_bounty(ch, "");
		return;
	}

	amount = atoi(arg2);

	if(amount < 50 && !IS_IMMORTAL(ch))
	{
		send_to_char("You must at least put 50 qp on a bounty.\n\r", ch);
		return;
	}

	if(amount > ch->pcdata->quest && !IS_IMMORTAL(ch))
	{
		sprintf(buf, "You need %li more qp to place that bounty.\n\r", amount - ch->pcdata->quest);
		send_to_char(buf, ch);
		return;
	}

	sprintf(buf, "You place a bounty of %li on %s's head.\n\r", amount, victim->name);
	send_to_char(buf, ch);
	sprintf(buf, "{R%s has just placed a %li qp bounty on %s's head!{x\n\r", ch->name, amount, victim->name);
	do_info(char_list, buf);
	victim->bounty += amount;
	if(!IS_IMMORTAL(ch))
		ch->pcdata->quest -= amount;
	sprintf(buf, "{W%s just placed a %li qp bounty on your head, run!{x\n\r", ch->name, amount);
	send_to_char(buf, victim);
	return;
}



// THIS IS THE END OF THE FILE THANKS
