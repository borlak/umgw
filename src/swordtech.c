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

// swordtech for highlanders

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


void swordtech(CHAR_DATA * ch, long bit)
{
	AFFECT_DATA af;

	if(!ch || IS_NPC(ch))
		return;

	af.type = gsn_swordtech;
	af.duration = 3 + ch->pcdata->stats[UNI_GEN];
	af.modifier = 0;
	af.location = 0;
	af.bitvector = bit;
	affect_to_char(ch, &af);
}


void do_swordtech(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *victim = 0;
	OBJ_DATA *obj;
	AFFECT_DATA af;
	long chance;

	if(!ch || IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_HIGHLANDER))
	{
		send_to_char("You are not one of the chosen.\n\r", ch);
		return;
	}

	if(!argument || argument[0] == '\0')
	{
		send_to_char("What type of swordtech manuever do you want to attempt?\n\r", ch);
		send_to_char("Types are: fury, disarm, berserk, stun, decapitate, concentrate\n\r", ch);
		return;
	}

	if((obj = get_eq_char(ch, WEAR_WIELD)) == 0 || obj->pIndexData->vnum != 29695)
	{
		send_to_char("You must be wielding your Katana to do a swordtech manuever.\n\r", ch);
		return;
	}

	if(is_affected(ch, gsn_swordtech))
	{
		send_to_char("You are not ready for another swordtech manuever yet!\n\r", ch);
		return;
	}

	if(ch->move < 850)
	{
		send_to_char("You need 850 movement points to do a swordtech manuever!\n\r", ch);
		return;
	}

	ch->move -= 600;

	if(ch->pcdata->stats[UNI_GEN] == 1)
		chance = 55;
	else if(ch->pcdata->stats[UNI_GEN] == 2)
		chance = 35;
	else
		chance = 0;

	chance += 35;

	act("{G$n holds $p in front of $m and a spiral of silver energy swirls around $s body.{x", ch, obj, 0, TO_ROOM);
	act("{GYou holds $p in front of you and a spiral of silver energy swirls around your body.{x",
	    ch, obj, 0, TO_CHAR);

	if(chance < number_percent())
	{
		af.type = gsn_swordtech;
		af.duration = 3 + ch->pcdata->stats[UNI_GEN];
		af.modifier = 0;
		af.location = 0;
		af.bitvector = 0;
		affect_to_char(ch, &af);

		act("{gYou fail the manuever and the energy leaves you.{x", ch, 0, 0, TO_CHAR);
		act("{g$n fails the manuever and the energy leaves $m.{x", ch, 0, 0, TO_ROOM);
		return;
	}

	if(!str_prefix(argument, "berserk")
	   || !str_prefix(argument, "bezerk") || !str_prefix(argument, "beserk") || !str_prefix(argument, "berzerk"))
	{
		af.type = gsn_swordtech;
		af.duration = 5 + ((3 - ch->pcdata->stats[UNI_GEN]) * 2);
		af.modifier = 0;
		af.location = 0;
		af.bitvector = STECH_BERSERK;
		affect_to_char(ch, &af);
		act("You go berserk!", ch, 0, 0, TO_CHAR);
		act("$n goes berserk!", ch, 0, 0, TO_ROOM);
		return;
	}

	if(!str_prefix(argument, "concentrate"))
	{
		af.type = gsn_swordtech;
		af.duration = 5 + ((3 - ch->pcdata->stats[UNI_GEN]) * 2);
		af.modifier = 0;
		af.location = 0;
		af.bitvector = STECH_CONCENTRATE;
		affect_to_char(ch, &af);
		act("A deep calm overcomes you and you begin examining your opponent very carefully.", ch, 0, 0,
		    TO_CHAR);
		act("$n appears to get very calm and more alertive.", ch, 0, 0, TO_ROOM);
		return;
	}

	if(!str_prefix(argument, "decapitate"))
	{
		if((victim = ch->fighting) == 0)
		{
			send_to_char("You have to be fighting to do this manuever.\n\r", ch);
			return;
		}
		if(IS_NPC(victim) || number_percent() < 50)
		{
			act("You bring your sword up for a masterful swing and......miss horribly.", ch, 0, victim,
			    TO_CHAR);
			act("$n brings $s sword up for a masterful swing and......misses $N's head by a mile.", ch, 0,
			    victim, TO_NOTVICT);
			act("$n brings $s sword up for a masterful swing and......misses your head by a mile.", ch, 0,
			    victim, TO_VICT);
			act("You drop your weapons!", ch, 0, 0, TO_CHAR);
			act("$n drops $s weapons!", ch, 0, 0, TO_ROOM);
			obj_from_char(obj);
			obj_to_room(obj, ch->in_room);
			if((obj = get_eq_char(ch, WEAR_HOLD)) != 0)
			{
				obj_from_char(obj);
				obj_to_room(obj, ch->in_room);
			}
			return;
		}
		act("You bring your sword up for a masterful swing and whack $N's head clean off!", ch, 0, victim,
		    TO_CHAR);
		act("$n brings $s sword up for a masterful swing and whacks $N's head clean off!", ch, 0, victim,
		    TO_NOTVICT);
		act("$n brings $s sword up for a masterful swing and......you are now missing a head?!", ch, 0, victim,
		    TO_VICT);
		sprintf(buf, "{B%s swordtech decapitates %s!  Ooooh that's gotta hurt!{X", ch->name, victim->name);
		do_info(ch, buf);
		behead(victim);
		return;
	}

	if(!str_prefix(argument, "stun"))
	{
		if((victim = ch->fighting) == 0)
		{
			send_to_char("You have to be fighting to do this manuever.\n\r", ch);
			return;
		}
		act("You knock the hell out of $N's head with the side of your sword!", ch, 0, victim, TO_CHAR);
		act("$n knocks the hell out of $N's head with the side of $s sword!", ch, 0, victim, TO_NOTVICT);
		act("$n knocks the hell out of your head with the side of $s sword!", ch, 0, victim, TO_VICT);
		WAIT_STATE(victim, number_range(24, 60));
		return;
	}

	if(!str_prefix(argument, "fury"))
	{
		if((victim = ch->fighting) == 0)
		{
			send_to_char("You have to be fighting to do this manuever.\n\r", ch);
			return;
		}
		af.type = gsn_swordtech;
		af.duration = 3 + (ch->pcdata->stats[UNI_GEN] * 2);
		af.modifier = APPLY_STR;
		af.location = 0;
		af.bitvector = STECH_FURY;
		affect_to_char(ch, &af);
		act("You attack in a wild fury!", ch, 0, 0, TO_CHAR);
		act("$n attacks in a wild fury!", ch, 0, 0, TO_ROOM);
		multi_hit(ch, ch->fighting, TYPE_UNDEFINED);
		multi_hit(ch, ch->fighting, TYPE_UNDEFINED);
		multi_hit(ch, ch->fighting, TYPE_UNDEFINED);
		return;
	}

	if(!str_prefix(argument, "disarm"))
	{
		if((victim = ch->fighting) == 0 && !ch->in_room)
		{
			send_to_char("You have to be fighting to do this manuever.\n\r", ch);
			return;
		}
		if(IS_NPC(victim))
		{
			send_to_char("Not on NPC's.\n\r", ch);
			return;
		}
		if((obj = get_eq_char(victim, WEAR_WIELD)) == 0)
		{
			send_to_char("They aren't wielding a weapon!\n\r", ch);
			return;
		}
		obj_from_char(obj);
		obj_to_room(obj, ch->in_room);
		af.type = gsn_swordtech;
		af.duration = 5 + (ch->pcdata->stats[UNI_GEN] * 2);
		af.modifier = 0;
		af.location = 0;
		af.bitvector = STECH_DISARM;
		affect_to_char(ch, &af);
		act("$n disarms YOU!", ch, 0, victim, TO_VICT);
		act("$n disarms $N!", ch, 0, victim, TO_NOTVICT);
		act("You disarm $N!", ch, 0, victim, TO_CHAR);
		return;
	}

	send_to_char("Nothing happens.\n\r", ch);
}

// THIS IS THE END OF THE FILE THANKS
