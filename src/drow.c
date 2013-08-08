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

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "player.h"

/*
 * Local functions.
 */

void do_grant(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	long inpart = 0;
	long cost = 0;

	smash_tilde(argument);
	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DROW))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char("Syntax: Grant <person> <power>\n\r", ch);
		send_to_char("Drowfire (2500),  Darkness (7500),  Drowpoison (2500).\n\r", ch);
		send_to_char("Drowsight (5000),  Drowshield (5000),  Levitation (1000).\n\r", ch);
		send_to_char("Shadowwalk (10000),  Garotte (5000),  Spiderarms (7500).\n\r", ch);
		send_to_char("Drowhate (10000),  Spiderform (15000),  Web (5000).\n\r", ch);
		send_to_char("Dgarotte (2500),  Confuse (2500),\n\r", ch);
		send_to_char("Earthshatter (7500),  Speed (7500),  Toughskin (7500).\n\r", ch);
		send_to_char("Warrior (0), Mage (0), Cleric (0).\n\r", ch);
		return;
	}

	if((victim = get_char_world(ch, arg1)) == 0)
	{
		send_to_char("Nobody by that name.\n\r", ch);
		return;
	}

	if(IS_NPC(victim))
	{
		send_to_char("Not on NPC's.\n\r", ch);
		return;
	}

	if(!str_cmp(arg2, "mage") || !str_cmp(arg2, "warrior") || !str_cmp(arg2, "cleric"))
	{
		if(victim->pcdata->stats[UNI_GEN] < 3 && victim != ch)
		{
			send_to_char("Not on them!\n\r", ch);
			return;
		}

		if(!IS_CLASS(victim, CLASS_DROW))
		{
			send_to_char("Only on drows.\n\r", ch);
			return;
		}

		if(IS_SET(victim->special, SPC_DROW_WAR) ||
		   IS_SET(victim->special, SPC_DROW_MAG) || IS_SET(victim->special, SPC_DROW_CLE))
		{
			send_to_char("They already have a profession.\n\r", ch);
			return;
		}
		if(!str_cmp(arg2, "mage"))
			SET_BIT(victim->special, SPC_DROW_MAG);
		else if(!str_cmp(arg2, "cleric"))
			SET_BIT(victim->special, SPC_DROW_CLE);
		else if(!str_cmp(arg2, "warrior"))
			SET_BIT(victim->special, SPC_DROW_WAR);
		send_to_char("You give them a profession.\n\r", ch);
		send_to_char("You have been given a profession.\n\r", victim);
		save_char_obj(victim);
		return;
	}


	if(!str_cmp(arg2, "drowfire"))
	{
		inpart = DPOWER_DROWFIRE;
		cost = 2500;
	}
	else if(!str_cmp(arg2, "darkness"))
	{
		inpart = DPOWER_DARKNESS;
		cost = 7500;
	}
	else if(!str_cmp(arg2, "drowsight"))
	{
		inpart = DPOWER_DROWSIGHT;
		cost = 5000;
	}
	else if(!str_cmp(arg2, "spiderarms"))
	{
		inpart = DPOWER_ARMS;
		cost = 7500;
	}
	else if(!str_cmp(arg2, "web"))
	{
		inpart = DPOWER_WEB;
		cost = 5000;
	}
	else if(!str_cmp(arg2, "spiderform"))
	{
		inpart = DPOWER_SPIDERFORM;
		cost = 15000;
	}
	else if(!str_cmp(arg2, "drowhate"))
	{
		inpart = DPOWER_DROWHATE;
		cost = 10000;
	}
	else if(!str_cmp(arg2, "drowshield"))
	{
		inpart = DPOWER_DROWSHIELD;
		cost = 5000;
	}
	else if(!str_cmp(arg2, "levitation"))
	{
		inpart = DPOWER_LEVITATION;
		cost = 1000;
	}
	else if(!str_cmp(arg2, "shadowwalk"))
	{
		inpart = DPOWER_SHADOWWALK;
		cost = 10000;
	}
	else if(!str_cmp(arg2, "garotte"))
	{
		inpart = DPOWER_GAROTTE;
		cost = 5000;
	}
	else if(!str_cmp(arg2, "dgarotte"))
	{
		inpart = DPOWER_DGAROTTE;
		cost = 2500;
	}
	else if(!str_cmp(arg2, "drowpoison"))
	{
		inpart = DPOWER_DROWPOISON;
		cost = 2500;
	}
/*    else if (!str_cmp(arg2,"glamour"))
	{inpart = DPOWER_GLAMOUR; cost = 5000;} */
	else if(!str_cmp(arg2, "confuse"))
	{
		inpart = DPOWER_CONFUSE;
		cost = 2500;
	}
	else if(!str_cmp(arg2, "earthshatter"))
	{
		inpart = DPOWER_EARTHSHATTER;
		cost = 7500;
	}
	else if(!str_cmp(arg2, "speed"))
	{
		inpart = DPOWER_SPEED;
		cost = 7500;
	}
	else if(!str_cmp(arg2, "toughskin"))
	{
		inpart = DPOWER_TOUGHSKIN;
		cost = 7500;
	}
	else
	{
		send_to_char("Please select a power from:\n\r", ch);
		send_to_char("Drowfire (2500),  Darkness (7500),  Drowpoison  (1000).\n\r", ch);
		send_to_char("Drowsight (5000),  Drowshield (5000),  Levitation (1000).\n\r", ch);
		send_to_char("Shadowwalk (10000),  Garotte (5000),  Spiderarms (7500).\n\r", ch);
		send_to_char("Drowhate (10000),  Spiderform (15000),  Web (5000).\n\r", ch);
		send_to_char("Dgarotte (2500),  Confuse (2500),\n\r", ch);
		send_to_char("Earthshatter (7500),  Speed (7500),  Toughskin (7500).\n\r", ch);

		if(ch->pcdata->stats[UNI_GEN] < 3)
		{
			send_to_char("Warrior (0), Mage (0), Cleric (0).\n\r", ch);
		}
		return;
	}
	if(IS_SET(victim->pcdata->powers[1], inpart))
	{
		send_to_char("They have already got that power.\n\r", ch);
		return;
	}
	if(ch->pcdata->stats[DROW_POWER] < cost)
	{
		send_to_char("You have insufficient power to grant that gift.\n\r", ch);
		return;
	}
	SET_BIT(victim->pcdata->powers[1], inpart);
	ch->pcdata->stats[DROW_POWER] -= cost;
	if(victim != ch)
		send_to_char("You have been granted a gift from your matron!\n\r", victim);
	send_to_char("Ok.\n\r", ch);
	if(victim != ch)
		save_char_obj(ch);
	save_char_obj(victim);
	return;
}

void do_ddarkness(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	ROOM_INDEX_DATA *inroom;

	inroom = ch->in_room;

	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
/*
if (ch->power[DISC_VAMP_QUIE] < 6)
{
      send_to_char("You require level 6 Quietus to use Darkness of
Death.\n\r",ch);
return;
    }
 */
	if(ch->pcdata->condition[COND_THIRST] < 300)
	{
		send_to_char("You have insufficient blood.\n\r", ch);
		return;
	}
	ch->pcdata->condition[COND_THIRST] -= 300;
	SET_BIT(inroom->room_flags, ROOM_DARK);
	sprintf(buf, "A look of concentration passes over %s's face.", ch->name);
	act(buf, ch, 0, 0, TO_ROOM);
	send_to_char("A look of concentration passes over your face.\n\r", ch);
	sprintf(buf, "A complete darkness fills the room.\n\r");
	act(buf, ch, 0, 0, TO_ROOM);
	send_to_char(buf, ch);
	return;
}

void do_chaosblast(CHAR_DATA * ch, char *argument)
{

	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	long sn;
	long level;
	long spelltype;

	argument = one_argument(argument, arg);
	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DROW) || (!IS_SET(ch->special, SPC_DROW_MAG) && ch->pcdata->stats[UNI_GEN] > 2))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, arg)) == 0)
		if((victim = ch->fighting) == 0)
		{
			send_to_char("They aren't here.\n\r", ch);
			return;
		}
	if(ch->mana < 100)
	{
		send_to_char("You don't have enough mana.\n\r", ch);
		return;
	}

	if((sn = skill_lookup("chaos blast")) < 0)
		return;
	spelltype = skill_table[sn].target;
	level = ch->spl[spelltype] * 3 / 2;
	act("You concentrate your power on $N.", ch, 0, victim, TO_CHAR);
	act("$n concentrates $s power on you.", ch, 0, victim, TO_VICT);
	(*skill_table[sn].spell_fun) (sn, level, ch, victim);
	WAIT_STATE(ch, 12);
	ch->mana = ch->mana - 100;
	return;
}


void do_drowcreate(CHAR_DATA * ch, char *argument)
{
	OBJ_INDEX_DATA *pObjIndex;
	OBJ_DATA *obj;
	char arg[MAX_INPUT_LENGTH];
	long vnum = 0;

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DROW))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(arg[0] == '\0')
	{
		send_to_char("Please specify what kind of equipment you want to create.\n\r", ch);
		send_to_char("Sword, Piwafwi, Armor, Boots, Mask, Bracer. and now featuring the all new Whip!\n\r", ch);
		return;
	}
	if(!str_cmp(arg, "sword"))
		vnum = 29600;
	else if(!str_cmp(arg, "piwafwi"))
		vnum = 29601;
	else if(!str_cmp(arg, "armor"))
		vnum = 29602;
	else if(!str_cmp(arg, "boots"))
		vnum = 29603;
	else if(!str_cmp(arg, "mask"))
		vnum = 29604;
	else if(!str_cmp(arg, "bracer"))
		vnum = 29605;
	else if(!str_cmp(arg, "whip"))
		vnum = 29606;
	else
	{
		send_to_char("That is an invalid type.\n\r", ch);
		return;
	}
	if(ch->pcdata->stats[DROW_POWER] < 5000)
	{
		send_to_char("It costs 5000 points of power to create a piece of drow armour.\n\r", ch);
		return;
	}
	if(vnum == 0 || (pObjIndex = get_obj_index(vnum)) == 0)
	{
		send_to_char("Missing object, please inform KaVir.\n\r", ch);
		return;
	}
	ch->pcdata->stats[DROW_POWER] -= 5000;
	obj = create_object(pObjIndex, 50);
	obj_to_char(obj, ch);
	SET_BIT(obj->spectype, SITEM_DROW);
	act("$p appears in your hands.", ch, obj, 0, TO_CHAR);
	act("$p appears in $n's hands.", ch, obj, 0, TO_ROOM);
	return;
}

void do_favor(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DROW) || ch->pcdata->stats[UNI_GEN] > 2)
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(arg[0] == '\0')
	{
		send_to_char("Give Lloth's favor to whom?\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(IS_NPC(victim))
	{
		send_to_char("Not on NPC's.\n\r", ch);
		return;
	}

	if(ch == victim)
	{
		send_to_char("Not on yourself.\n\r", ch);
		return;
	}

	if(victim->level != LEVEL_AVATAR && !IS_IMMORTAL(victim))
	{
		send_to_char("Only on avatars.\n\r", ch);
		return;
	}

	if(!IS_IMMUNE(victim, IMM_DEMON))
	{
		send_to_char("Not on an unwilling person.\n\r", ch);
		return;
	}
/*    if (IS_CLASS(victim, CLASS_DROW))
	send_to_char("They are already a drow.\n\r", ch );
		return;
	} */
	if(IS_CLASS(victim, CLASS_DEMON) || IS_SET(victim->special, SPC_CHAMPION))
	{
		send_to_char("Not on a demon!\n\r", ch);
		return;
	}

	if(IS_CLASS(victim, CLASS_VAMPIRE))
	{
		send_to_char("Not on a vampire!\n\r", ch);
		return;
	}

	if(IS_CLASS(victim, CLASS_WEREWOLF))
	{
		send_to_char("Not on a werewolf!\n\r", ch);
		return;
	}
	if(IS_CLASS(victim, CLASS_HIGHLANDER))
	{
		send_to_char("Not on a highlander.\n\r", ch);
		return;
	}

	if(IS_CLASS(victim, CLASS_MONK))
	{
		send_to_char("Not on monks!\n\r", ch);
		return;
	}
	if(IS_CLASS(victim, CLASS_NINJA))
	{
		send_to_char("Not on a ninja.\n\r", ch);
		return;
	}
	if(ch->exp < 10000)
	{
		send_to_char("You cannot afford the 10,000 exp.\n\r", ch);
		return;
	}

	ch->exp = ch->exp - 10000;
	act("You make $N a drow!", ch, 0, victim, TO_CHAR);
	act("$n makes $N a drow!", ch, 0, victim, TO_NOTVICT);
	act("$n makes you a drow!", ch, 0, victim, TO_VICT);
	victim->pcdata->class = 0;
	victim->pcdata->class = CLASS_DROW;

	if(IS_CLASS(victim, CLASS_DROW))
	{
		victim->special = 0;
	}
	if(IS_CLASS(victim, CLASS_VAMPIRE))
		do_mortalvamp(victim, "");
	REMOVE_BIT(victim->act, PLR_HOLYLIGHT);
	REMOVE_BIT(victim->act, PLR_WIZINVIS);
	REMOVE_BIT(victim->special, SPC_SIRE);
	REMOVE_BIT(victim->special, SPC_ANARCH);
	victim->pcdata->stats[UNI_RAGE] = 0;
	victim->pcdata->stats[UNI_GEN] = ch->pcdata->stats[UNI_GEN] + 1;
	victim->special = 0;
	free_string(victim->morph);
	victim->morph = str_dup("");
	free_string(victim->clan);
	victim->clan = str_dup(ch->clan);

	free_string(victim->lord);
	if(ch->pcdata->stats[UNI_GEN] == 1)
		victim->lord = str_dup(ch->name);
	else
	{
		sprintf(buf, "%s %s", ch->lord, ch->name);
		victim->lord = str_dup(buf);
	}

	save_char_obj(ch);
	save_char_obj(victim);
	return;
}

void do_touch(CHAR_DATA * ch, char *argument)
{
	long sn;
	long level;
	long spelltype;

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DROW))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(ch->mana < 500)
	{
		send_to_char("You don't have enough mana.\n\r", ch);
		return;
	}

	if((sn = skill_lookup("llothbless")) < 0)
		return;
	spelltype = skill_table[sn].target;
	level = (long) (ch->spl[spelltype] * 0.5);
	(*skill_table[sn].spell_fun) (sn, level, ch, ch);
	WAIT_STATE(ch, 12);
	ch->mana = ch->mana - 500;
	return;
}

void do_drowfire(CHAR_DATA * ch, char *argument)
{

	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	long sn;
	long level;
	long spelltype;

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DROW) || !IS_SET(ch->pcdata->powers[1], DPOWER_DROWFIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(ch == victim)
	{
		send_to_char("Not on yourself.\n\r", ch);
		return;
	}

	if(ch->mana < 100)
	{
		send_to_char("You don't have enough mana.\n\r", ch);
		return;
	}

	if((sn = skill_lookup("drowfire")) < 0)
		return;
	spelltype = skill_table[sn].target;
	level = (long) (ch->spl[spelltype] * 0.5);
	(*skill_table[sn].spell_fun) (sn, level, ch, victim);
	WAIT_STATE(ch, 12);
	ch->mana = ch->mana - 100;
	return;
}
void do_heal(CHAR_DATA * ch, char *argument)
{
	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DROW) || (!IS_SET(ch->special, SPC_DROW_CLE) && ch->pcdata->stats[UNI_GEN] > 2))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(ch->mana < 100)
	{
		send_to_char("You don't have enough mana.\n\r", ch);
		return;
	}

	ch->mana = ch->mana - 100;
	ch->hit = ch->hit + ch->spl[BLUE_MAGIC] * 3 / 2;
	if(ch->hit > ch->max_hit)
		ch->hit = ch->max_hit;
	send_to_char("Lloth heals you.\n\r", ch);
	act("$n is healed by Lloth.", ch, 0, 0, TO_ROOM);
	WAIT_STATE(ch, 12);
	return;
}

void do_shadowwalk(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	ROOM_INDEX_DATA *location;
	CHAR_DATA *victim;

	one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DROW) || !IS_SET(ch->pcdata->powers[1], DPOWER_SHADOWWALK))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if((victim = get_char_world(ch, arg)) == 0)
	{
		send_to_char("Shadowwalk to whom?\n\r", ch);
		return;
	}

	location = victim->in_room;

	if(ch->move < 250)
	{
		send_to_char("You are too tired to go shadowwalking.\n\r", ch);
		return;
	}

	if(!IS_NPC(victim))
	{
		send_to_char("You failed.\n\r", ch);
		return;
	}

	if(!IS_SET(victim->in_room->room_flags, ROOM_DARK) && !IS_SET(victim->in_room->room_flags, ROOM_TOTAL_DARKNESS))
	{
		send_to_char("You failed.\n\r", ch);
		return;
	}

	act("You walk into the shadows.", ch, 0, 0, TO_CHAR);
	act("$n walks into the shadows.", ch, 0, 0, TO_ROOM);
	ch->move -= 250;
	char_from_room(ch);
	char_to_room(ch, location);
	do_look(ch, "auto");
	act("You walk out of the shadows.", ch, 0, 0, TO_CHAR);
	act("$n walks out of the shadows.", ch, 0, 0, TO_CHAR);

	return;
}

void do_drowhate(CHAR_DATA * ch, char *argument)
{
	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DROW) || !IS_SET(ch->pcdata->powers[1], DPOWER_DROWHATE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(IS_SET(ch->newbits, NEW_DROWHATE))
	{
		send_to_char("You calm your hatred.\n\r", ch);
		REMOVE_BIT(ch->newbits, NEW_DROWHATE);
	}
	else
	{
		send_to_char("You invoke your hatred for others.\n\r", ch);
		SET_BIT(ch->newbits, NEW_DROWHATE);
	}

	return;
}


void do_spiderform(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DROW) || !IS_SET(ch->pcdata->powers[1], DPOWER_SPIDERFORM))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}




	if(IS_POLYAFF(ch, POLY_SPIDERFORM))
	{
		REMOVE_BIT(ch->polyaff, POLY_SPIDERFORM);
		REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
		act("You transform into human form.", ch, 0, 0, TO_CHAR);
		act("$n's shrinks back into human form.", ch, 0, 0, TO_ROOM);

		ch->damroll = -50;
		ch->hitroll = -50;
		clear_stats(ch);
		free_string(ch->morph);
		ch->morph = str_dup("");
		return;
	}
	else if(IS_AFFECTED(ch, AFF_POLYMORPH))
	{
		send_to_char("You cant spider form when changed.\n\r", ch);
		return;
	}
	if(ch->stance[0] != -1)
		do_stance(ch, "");
	if(ch->mounted == IS_RIDING)
		do_dismount(ch, "");
	act("You transform into a giant spider", ch, 0, 0, TO_CHAR);
	act("$n's body grows and distorts into a giant spider.", ch, 0, 0, TO_ROOM);

	ch->pcdata->mod_str = 15;
	ch->pcdata->mod_dex = 15;
	SET_BIT(ch->polyaff, POLY_SPIDERFORM);
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	sprintf(buf, "%s a giant myrlochar", ch->name);
	free_string(ch->morph);
	ch->morph = str_dup(buf);
	ch->damroll = +50;
	ch->hitroll = +50;
	return;
}


/*
void do_spiderform( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];

	argument = one_argument(argument,arg);

	if (IS_NPC(ch)) return;

	if (!IS_CLASS(ch, CLASS_DROW) || !IS_SET(ch->pcdata->powers[1], DPOWER_SPIDERFORM))
	{send_to_char("Huh?\n\r", ch );
	return;}

	if (IS_AFFECTED(ch, AFF_POLYMORPH))
      {
	send_to_char("You can't spiderform while changed.\n\r", ch );
	return;
      }

      if (ch->cur_form != get_normal_form(ch))
      {
	sprintf(buf, "$n morphs back into %s.", GET_PROPER_NAME(ch));
        act(buf, ch, 0, 0, TO_ROOM);
        stc("You return to your normal form.\n\r", ch);
        set_form(ch, get_normal_form(ch) );
        WAIT_STATE(ch, 7);
        return;
      }

	if (ch->move < 500) {
	send_to_char("You don't have the energy to change.\n\r", ch );
	return;}


      if (ch->cur_form == get_normal_form(ch))
      {
	ch->move -= 500;

	act("You mutate into a giant spider.",ch,0,0,TO_CHAR);
        act("$n mutates into a giant spider.",ch,0,0,TO_ROOM);
        set_form(ch, FRM_DROWSPIDER);
	return;
    }

	return;
}
*/

void do_drows(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	char lord[MAX_INPUT_LENGTH];
	CHAR_DATA *gch;

	one_argument(argument, arg);

	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_DROW))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->stats[UNI_GEN] < 3)
		strcpy(lord, ch->name);
	else
		strcpy(lord, ch->lord);
	sprintf(buf, "The drow followers of %s:\n\r", lord);
	send_to_char(buf, ch);
	send_to_char("[      Name      ] [ Hits ] [ Mana ] [ Move ] [  Exp  ][   Power   ]\n\r", ch);
	for(gch = char_list; gch != 0; gch = gch->next)
	{
		if(IS_NPC(gch))
			continue;
		if(!IS_CLASS(gch, CLASS_DROW))
			continue;
		if(!str_cmp(ch->lord, lord) || !str_cmp(ch->name, lord))
		{
			sprintf(buf,
				"[%-16s] [%-6li] [%-6li] [%-6li] [%7li] [ %-9li ]\n\r",
				capitalize(gch->name),
				gch->hit, gch->mana, gch->move, gch->exp, gch->pcdata->stats[DROW_POWER]);
			send_to_char(buf, ch);
		}
	}
	return;
}


void do_drowsight(CHAR_DATA * ch, char *argument)
{
	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DROW) || !IS_SET(ch->pcdata->powers[1], DPOWER_DROWSIGHT))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(IS_SET(ch->act, PLR_HOLYLIGHT))
	{
		REMOVE_BIT(ch->act, PLR_HOLYLIGHT);
		send_to_char("Your senses return to normal.\n\r", ch);
	}
	else
	{
		SET_BIT(ch->act, PLR_HOLYLIGHT);
		send_to_char("Your senses increase to incredible proportions.\n\r", ch);
	}

	return;
}

void do_drowshield(CHAR_DATA * ch, char *argument)
{

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DROW) || !IS_SET(ch->pcdata->powers[1], DPOWER_DROWSHIELD))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(!IS_IMMUNE(ch, IMM_SHIELDED))
	{
		send_to_char("You shield your aura from those around you.\n\r", ch);
		SET_BIT(ch->immune, IMM_SHIELDED);
		return;
	}
	send_to_char("You stop shielding your aura.\n\r", ch);
	REMOVE_BIT(ch->immune, IMM_SHIELDED);


	return;
}


void do_drowpowers(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DROW))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	send_to_char("{cThe powers Lloth has bestowed upon you...{x\n\r", ch);
	send_to_char("{b-----------------------------------------{x\n\r", ch);

	if(IS_SET(ch->pcdata->powers[1], DPOWER_SPEED))
		send_to_char("You move quite quickly.\n\r", ch);

	if(IS_SET(ch->pcdata->powers[1], DPOWER_TOUGHSKIN))
		send_to_char("Your skin has been toughened.\n\r", ch);

	if(IS_SET(ch->pcdata->powers[1], DPOWER_DROWFIRE))
		send_to_char("You can call Drowfire upon your enemies.\n\r", ch);

	if(IS_SET(ch->pcdata->powers[1], DPOWER_DARKNESS))
		send_to_char("You can summon a globe of darkness.\n\r", ch);

	if(IS_SET(ch->pcdata->powers[1], DPOWER_DROWSIGHT))
		send_to_char("You can use enhanced sight.\n\r", ch);

	if(IS_SET(ch->pcdata->powers[1], DPOWER_LEVITATION))
		send_to_char("You can levitate in the air.\n\r", ch);

	if(IS_SET(ch->pcdata->powers[1], DPOWER_DROWSHIELD))
		send_to_char("You can shield your aura.\n\r", ch);

	if(IS_SET(ch->pcdata->powers[1], DPOWER_DROWPOISON))
		send_to_char("You have control over deadly poisons.\n\r", ch);

	if(IS_SET(ch->pcdata->powers[1], DPOWER_SHADOWWALK))
		send_to_char("You can walk through the shadows.\n\r", ch);

	if(IS_SET(ch->pcdata->powers[1], DPOWER_GAROTTE))
		send_to_char("You have mastered the art of garotte.\n\r", ch);

	if(IS_SET(ch->pcdata->powers[1], DPOWER_DGAROTTE))
		send_to_char("You have dark-enhanced garotte.\n\r", ch);

	if(IS_SET(ch->pcdata->powers[1], DPOWER_ARMS))
		send_to_char("You have extra spidery arms.\n\r", ch);

	if(IS_SET(ch->pcdata->powers[1], DPOWER_DROWHATE))
		send_to_char("You can invoke the hatred of the Drow.\n\r", ch);

	if(IS_SET(ch->pcdata->powers[1], DPOWER_SPIDERFORM))
		send_to_char("You can mutate into a large spider.\n\r", ch);

	if(IS_SET(ch->pcdata->powers[1], DPOWER_WEB))
		send_to_char("You can shoot a spidery web at opponents.\n\r", ch);

/*	if (IS_SET(ch->pcdata->powers[1], DPOWER_GLAMOUR))
	send_to_char("You can glamourosly change the appearance of items.\n\r", ch ); */

	if(IS_SET(ch->pcdata->powers[1], DPOWER_CONFUSE))
		send_to_char("You can confuse your enemies during battle.\n\r", ch);

	if(IS_SET(ch->pcdata->powers[1], DPOWER_EARTHSHATTER))
		send_to_char("You can shatter the earth under your feet.\n\r", ch);

	send_to_char("\n\r", ch);

	if(ch->pcdata->stats[UNI_GEN] == 1)
		send_to_char("You are Lloth's Avatar.\n\r", ch);

	if(ch->pcdata->stats[UNI_GEN] == 2)
		send_to_char("You are a Drow Matron.\n\r", ch);

	if(IS_SET(ch->special, SPC_DROW_WAR))
		send_to_char("You are a Drow Warrior.\n\r", ch);

	if(IS_SET(ch->special, SPC_DROW_MAG))
		send_to_char("You are a Drow Mage.\n\r", ch);

	if(IS_SET(ch->special, SPC_DROW_CLE))
		send_to_char("You are a Drow Cleric.\n\r", ch);

	sprintf(buf, "You have %li drow power points!.\n\r", ch->pcdata->stats[DROW_POWER]);
	send_to_char(buf, ch);

	sprintf(buf, "You have %li points of magic resistance.\n\r", ch->pcdata->stats[DROW_MAGIC]);
	send_to_char(buf, ch);

	if(weather_info.sunlight == SUN_DARK)
		send_to_char("You feel strong in the night.\n\r", ch);

	send_to_char("\n\r", ch);

	return;
}


void do_darkness(CHAR_DATA * ch, char *argument)
{
	bool blah = FALSE;

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DROW) || !IS_SET(ch->pcdata->powers[1], DPOWER_DARKNESS))
	{
		do_ddarkness(ch, argument);
		return;
	}

	if(IS_SET(ch->newbits, NEW_DARKNESS))
	{
		send_to_char("You banish your globe of darkness.\n\r", ch);
		act("The globe of darkness around $n disappears.", ch, 0, 0, TO_ROOM);
		REMOVE_BIT(ch->newbits, NEW_DARKNESS);
		if(ch->in_room != 0)
			if(IS_SET(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS))
				REMOVE_BIT(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS);
		return;
	}

	if(ch->mana < 500)
	{
		send_to_char("You don't have enough mana to summon the darkness.\n\r", ch);
		return;
	}

	send_to_char("You summon a globe of darkness.\n\r", ch);
	act("$n summons a globe of darkness.", ch, 0, 0, TO_ROOM);
	ch->mana -= 500;
	if(IS_SET(ch->extra, TIED_UP))
	{
		REMOVE_BIT(ch->extra, TIED_UP);
		blah = TRUE;
	}

	if(IS_AFFECTED(ch, AFF_WEBBED))
	{
		REMOVE_BIT(ch->affected_by, AFF_WEBBED);
		blah = TRUE;
	}

	if(blah)
		send_to_char("The darkness sets you free.\n\r", ch);

	SET_BIT(ch->newbits, NEW_DARKNESS);
	if(!IS_SET(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS))
		SET_BIT(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS);


	return;
}

void do_glamour(CHAR_DATA * ch, char *argument)
{

	char arg1[MAX_STRING_LENGTH];
	char arg2[MAX_STRING_LENGTH];
	OBJ_DATA *obj;

	argument = one_argument(argument, arg1);
	strcpy(arg2, argument);

	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_DROW) || !IS_SET(ch->pcdata->powers[1], DPOWER_GLAMOUR))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(arg1 == 0 || arg2 == 0)
	{
		send_to_char("Syntax: Glamour (item) (description)\n\r", ch);
		return;
	}

	if((obj = get_obj_carry(ch, arg1)) == 0)
	{
		send_to_char("You dont have that item.\n\r", ch);
		return;
	}

	if(strlen(arg2) > 40 || strlen(arg2) < 3)
	{
		send_to_char("From 3 to 40 characters please.\n\r", ch);
		return;
	}

	free_string(obj->name);
	obj->name = str_dup(arg2);
	free_string(obj->short_descr);
	obj->short_descr = str_dup(arg2);
	send_to_char("Ok.\n\r", ch);

	return;
}


void do_confuse(CHAR_DATA * ch, char *argument)
{

	CHAR_DATA *victim;

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DROW) || !IS_SET(ch->pcdata->powers[1], DPOWER_CONFUSE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if((victim = ch->fighting) == 0)
	{
		send_to_char("You are not fighting anyone.\n\r", ch);
		return;
	}

	if(ch->move < 75)
	{
		send_to_char("You need 75 move to confuse your opponent.\n\r", ch);
		return;
	}

	act("$n attempts to confuse you.", ch, 0, victim, TO_VICT);
	act("You attempt to confuse $N.", ch, 0, victim, TO_CHAR);
	act("$n attempts to confuse $N.", ch, 0, victim, TO_NOTVICT);

	ch->move -= 75;

	if(number_percent() > 25)
	{
		send_to_char("You failed.\n\r", ch);
		return;
	}

	else
	{
		do_flee(victim, "");
		WAIT_STATE(ch, 16);
		return;
	}

	return;
}


void do_earthshatter(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;
	long dam;
	long level;


	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DROW) || !IS_SET(ch->pcdata->powers[1], DPOWER_EARTHSHATTER))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(ch->mana < 150)
	{
		send_to_char("You need more mana.\n\r", ch);
		return;
	}

	level = ch->spl[PURPLE_MAGIC];
	ch->mana -= 150;

	send_to_char("You summon the power of the underworld, shattering the earth.\n\r", ch);
	act("$n causes the earth to shatter", ch, 0, 0, TO_ROOM);

	for(vch = ch->in_room->people; vch != 0; vch = vch_next)
	{
		vch_next = vch->next_in_room;

		if(vch == ch)
			continue;
		dam = dice(level, 7);
		if(saves_spell(level, vch))
			dam /= 2;
		damage(ch, vch, dam, skill_lookup("earthquake"));
	}
	WAIT_STATE(ch, 12);
	return;
}

// THIS IS THE END OF THE FILE THANKS
