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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


char *const where_name[] = {
	"[Light         ] ",
	"[On Finger     ] ",
	"[On Finger     ] ",
	"[Around Neck   ] ",
	"[Around Neck   ] ",
	"[On Body       ] ",
	"[On Head       ] ",
	"[On Legs       ] ",
	"[On Feet       ] ",
	"[On Hands      ] ",
	"[On Arms       ] ",
	"[Off Hand      ] ",
	"[Around Body   ] ",
	"[Around Waist  ] ",
	"[Around Wrist  ] ",
	"[Around Wrist  ] ",
	"[Right Hand    ] ",
	"[Left Hand     ] ",
	"[On Face       ] ",
	"[Left Scabbard ] ",
	"[Right Scabbard] ",
	"[Third Arm     ] ",
	"[Fourth Arm    ] "
};



/*
 * Local functions.
 */
char *format_obj_to_char args((OBJ_DATA * obj, CHAR_DATA * ch, bool fShort));
void show_char_to_char_0 args((CHAR_DATA * victim, CHAR_DATA * ch));
void show_char_to_char_1 args((CHAR_DATA * victim, CHAR_DATA * ch));
void show_char_to_char args((CHAR_DATA * list, CHAR_DATA * ch));
bool check_blind args((CHAR_DATA * ch));

void evil_eye args((CHAR_DATA * victim, CHAR_DATA * ch));
void check_left_arm args((CHAR_DATA * ch, CHAR_DATA * victim));
void check_right_arm args((CHAR_DATA * ch, CHAR_DATA * victim));
void check_left_leg args((CHAR_DATA * ch, CHAR_DATA * victim));
void check_right_leg args((CHAR_DATA * ch, CHAR_DATA * victim));

void obj_score args((CHAR_DATA * ch, OBJ_DATA * obj));

// global number of players, for login
long gCur = 0;
long gCurMax = 0;


char *class_name(long class)
{
	switch (class)
	{
	case CLASS_HIGHLANDER:
		return "Highlander";
	case CLASS_DROW:
		return "Drow";
	case CLASS_DEMON:
		return "Demon";
	case CLASS_NINJA:
		return "Ninja";
	case CLASS_MONK:
		return "Monk";
	case CLASS_VAMPIRE:
		return "Vampire";
	case CLASS_WEREWOLF:
		return "Werewolf";
	case 0:
		return "Unknown";
	default:
		return "Hybrid";
	}
	return "Bugged-As-Shit";
}

char *format_obj_to_char(OBJ_DATA * obj, CHAR_DATA * ch, bool fShort)
{
	static char buf[MAX_STRING_LENGTH];

	buf[0] = '\0';
	if(IS_SET(obj->quest, QUEST_ARTIFACT))
		strcat(buf, "(Artifact) ");
	else if(IS_SET(obj->quest, QUEST_RELIC))
		strcat(buf, "({RRelic{x) ");

	if(obj->points > 0 && obj->points < 1001)
		strcat(buf, "({YLegendary{x) ");
	if(obj->points > 1000)
		strcat(buf, "(Mythical) ");
	if(IS_SET(obj->quest, ITEM_EQUEST))
		strcat(buf, "(Priceless) ");
	if(IS_SET(obj->quest, QUEST_ANCIENT))
		strcat(buf, "({DAncient{X) ");
	if(IS_OBJ_STAT(obj, ITEM_INVIS))
		strcat(buf, "({WInvis{x) ");
	if(IS_AFFECTED(ch, AFF_DETECT_EVIL) && !IS_OBJ_STAT(obj, ITEM_ANTI_GOOD) && IS_OBJ_STAT(obj, ITEM_ANTI_EVIL))
		strcat(buf, "({BBlue Aura{x) ");
	else if(IS_AFFECTED(ch, AFF_DETECT_EVIL) && IS_OBJ_STAT(obj, ITEM_ANTI_GOOD) && !IS_OBJ_STAT(obj, ITEM_ANTI_EVIL))
		strcat(buf, "({RRed Aura{x) ");
	else if(IS_AFFECTED(ch, AFF_DETECT_EVIL)
		&& IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)
		&& !IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_OBJ_STAT(obj, ITEM_ANTI_EVIL))
		strcat(buf, "({YYellow Aura{x) ");
	if(IS_AFFECTED(ch, AFF_DETECT_MAGIC) && IS_OBJ_STAT(obj, ITEM_MAGIC))
		strcat(buf, "({GMagical{x) ");
	if(IS_OBJ_STAT(obj, ITEM_GLOW))
		strcat(buf, "({MGlowing{x) ");
	if(IS_OBJ_STAT(obj, ITEM_HUM))
		strcat(buf, "({CHumming{x) ");
	if(IS_OBJ_STAT(obj, ITEM_SHADOWPLANE) && obj->in_room != 0 && !IS_AFFECTED(ch, AFF_SHADOWPLANE))
		strcat(buf, "({kShadowplane{x) ");
	if(!IS_OBJ_STAT(obj, ITEM_SHADOWPLANE) && obj->in_room != 0 && IS_AFFECTED(ch, AFF_SHADOWPLANE))
		strcat(buf, "(Normal plane) ");

	if(fShort)
	{
		if(obj->short_descr != 0)
			strcat(buf, obj->short_descr);
		if(obj->condition < 100)
			strcat(buf, " (damaged)");
	}
	else
	{
		if(obj->description != 0)
			strcat(buf, obj->description);
	}

	return buf;
}



long char_hitroll(CHAR_DATA * ch)
{
/* mog */

	long hr = GET_HITROLL(ch);

	if(!IS_NPC(ch))
	{
		if(IS_CLASS(ch, CLASS_VAMPIRE) && ch->pcdata->stats[UNI_RAGE] > 0)
			hr += ch->pcdata->stats[UNI_RAGE];
		else if(IS_CLASS(ch, CLASS_NINJA) && ch->pcdata->stats[UNI_RAGE] > 0)
			hr += ch->pcdata->stats[UNI_RAGE];
		else if(IS_CLASS(ch, CLASS_NINJA) &&
			ch->pcdata->powers[NPOWER_CHIKYU] >= 6 && ch->pcdata->powers[HARA_KIRI] > 0)
			hr += 100;
		else if(IS_SET(ch->special, SPC_WOLFMAN) && ch->pcdata->stats[UNI_RAGE] > 0)
			hr += (long) (ch->pcdata->stats[UNI_RAGE] * .5);
		else if(IS_CLASS(ch, CLASS_DEMON) && ch->pcdata->stats[DEMON_POWER] > 0)
			hr += ((ch->pcdata->stats[DEMON_POWER]) * ch->pcdata->stats[DEMON_POWER]);
		else if(IS_SET(ch->special, SPC_CHAMPION) && ch->pcdata->stats[DEMON_POWER] > 0)
			hr += ((ch->pcdata->stats[DEMON_POWER]) * ch->pcdata->stats[DEMON_POWER]);
		else if(IS_CLASS(ch, CLASS_HIGHLANDER) && (get_eq_char(ch, WEAR_WIELD) != 0))
		{
			long wpn = ch->wpn[1];

			if(wpn >= 500)
				hr += 400;
			else
				hr += wpn;

			if(ch->pcdata->stats[UNI_GEN] == 2)
				hr += 100;
			if(ch->pcdata->stats[UNI_GEN] == 1)
				hr += 250;

			if(IS_SWORDTECH(ch, STECH_BERSERK))
				hr /= 2;
			if(IS_SWORDTECH(ch, STECH_CONCENTRATE))
				hr *= 2;

		}
	}

	if(IS_CLASS(ch, CLASS_WEREWOLF) && ch->pcdata->stats[UNI_RAGE] > 0)
		hr += (long) (ch->pcdata->stats[UNI_RAGE] * .5);

	if(IS_CLASS(ch, CLASS_DEMON))
		hr += ch->pcdata->stats[DEMON_CURRENT] / 10000;

	return hr;
}

long char_damroll(CHAR_DATA * ch)
{
	long dr = GET_DAMROLL(ch);

	if(!IS_NPC(ch))
	{

		if(IS_CLASS(ch, CLASS_NINJA) && ch->pcdata->powers[NPOWER_CHIKYU] >= 6 && ch->pcdata->powers[HARA_KIRI] > 0)
			dr += 50;
		else if(IS_CLASS(ch, CLASS_VAMPIRE) && ch->pcdata->stats[UNI_RAGE] > 0)
			dr += ch->pcdata->stats[UNI_RAGE];
		else if(IS_CLASS(ch, CLASS_NINJA) && ch->pcdata->stats[UNI_RAGE] > 0)
			dr += ch->pcdata->stats[UNI_RAGE];
		else if(IS_SET(ch->special, SPC_WOLFMAN) && ch->pcdata->stats[UNI_RAGE] > 0)
			dr += (long) (ch->pcdata->stats[UNI_RAGE] * .5);
		else if(IS_CLASS(ch, CLASS_DEMON) && ch->pcdata->stats[DEMON_POWER] > 0)
			dr += ((ch->pcdata->stats[DEMON_POWER]) * ch->pcdata->stats[DEMON_POWER]);
		else if(IS_SET(ch->special, SPC_CHAMPION) && ch->pcdata->stats[DEMON_POWER] > 0)
			dr += ((ch->pcdata->stats[DEMON_POWER]) * ch->pcdata->stats[DEMON_POWER]);
		else if(IS_CLASS(ch, CLASS_HIGHLANDER) && (get_eq_char(ch, WEAR_WIELD) != 0))
		{
			long wpn = ch->wpn[1];

			if(wpn >= 500)
				dr += 400;
			else
				dr += wpn;

			if(ch->pcdata->stats[UNI_GEN] == 2)
				dr += 100;
			if(ch->pcdata->stats[UNI_GEN] == 1)
				dr += 250;

			if(IS_SWORDTECH(ch, STECH_BERSERK))
				dr *= 2;
			if(IS_SWORDTECH(ch, STECH_CONCENTRATE))
				dr /= 2;
		}
	}

	if(IS_CLASS(ch, CLASS_WEREWOLF) && ch->pcdata->stats[UNI_RAGE] > 0)
		dr += (long) (ch->pcdata->stats[UNI_RAGE] * .5);

	if(IS_CLASS(ch, CLASS_DEMON))
		dr += ch->pcdata->stats[DEMON_CURRENT] / 10000;

	return dr;
}

long char_ac(CHAR_DATA * ch)
{
	long a_c = GET_AC(ch);

	if(!IS_NPC(ch))
	{
		if(IS_CLASS(ch, CLASS_HIGHLANDER) && (get_eq_char(ch, WEAR_WIELD) != 0))
		{
//              long wpn = ch->pcdata->powers[HPOWER_WPNSKILL];
			long wpn = ch->wpn[1];

			a_c -= wpn;

			if(ch->pcdata->stats[UNI_GEN] == 2)
				a_c -= 350;
			if(ch->pcdata->stats[UNI_GEN] == 1)
				a_c -= 600;
		}
	}

	if(IS_SWORDTECH(ch, STECH_BERSERK))
		return 0;
	if(IS_SWORDTECH(ch, STECH_CONCENTRATE))
		a_c *= 2;

	return a_c;
}

/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char(OBJ_DATA * list, CHAR_DATA * ch, bool fShort, bool fShowNothing)
{
	char buf[MAX_STRING_LENGTH];
	char **prgpstrShow;
	long *prgnShow;
	char *pstrShow;
	OBJ_DATA *obj;
	long nShow;
	long iShow;
	long count;
	bool fCombine;

	if(ch->desc == 0)
		return;

	/*
	 * Alloc space for output lines.
	 */
	count = 0;
	for(obj = list; obj != 0; obj = obj->next_content)
		count++;
	prgpstrShow = alloc_mem(count * sizeof(char *), "show_list_to_char:prgpstrShow");
	prgnShow = alloc_mem(count * sizeof(long), "show_list_to_char:prgnShow");
	nShow = 0;

	/*
	 * Format the list of objects.
	 */
	for(obj = list; obj != 0; obj = obj->next_content)
	{
		if(!IS_NPC(ch) && ch->pcdata->chobj != 0 && obj->chobj != 0 && obj->chobj == ch)
			continue;
		if(obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj))
		{
			pstrShow = format_obj_to_char(obj, ch, fShort);
			fCombine = FALSE;

			if(IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE))
			{
				/*
				 * Look for duplicates, case sensitive.
				 * Matches tend to be near end so run loop backwords.
				 */
				for(iShow = nShow - 1; iShow >= 0; iShow--)
				{
					if(!strcmp(prgpstrShow[iShow], pstrShow))
					{
						prgnShow[iShow]++;
						fCombine = TRUE;
						break;
					}
				}
			}

			/*
			 * Couldn't combine, or didn't want to.
			 */
			if(!fCombine)
			{
				prgpstrShow[nShow] = str_dup(pstrShow);
				prgnShow[nShow] = 1;
				nShow++;
			}
		}
	}

	/*
	 * Output the formatted list.
	 */
	for(iShow = 0; iShow < nShow; iShow++)
	{
		if(IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE))
		{
			if(prgnShow[iShow] != 1)
			{
				sprintf(buf, "(%2li) ", prgnShow[iShow]);
				send_to_char(buf, ch);
			}
			else
			{
				send_to_char("     ", ch);
			}
		}
		send_to_char(prgpstrShow[iShow], ch);
		send_to_char("\n\r", ch);
		free_string(prgpstrShow[iShow]);
	}

	if(fShowNothing && nShow == 0)
	{
		if(IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE))
			send_to_char("     ", ch);
		send_to_char("Nothing.\n\r", ch);
	}

	/*
	 * Clean up.
	 */
	free_mem(prgpstrShow, count * sizeof(char *));
	free_mem(prgnShow, count * sizeof(long));

	return;
}



void show_char_to_char_0(CHAR_DATA * victim, CHAR_DATA * ch)
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char buf3[MAX_STRING_LENGTH];
	char buf4[MAX_STRING_LENGTH];
	char mount2[MAX_STRING_LENGTH];
	CHAR_DATA *mount;

	buf[0] = '\0';
	buf2[0] = '\0';
	buf3[0] = '\0';

	if(!IS_NPC(victim) && victim->pcdata->chobj != 0)
		return;

	if((mount = victim->mount) != 0 && IS_SET(victim->mounted, IS_MOUNT))
		return;

	if(IS_HEAD(victim, LOST_HEAD) && IS_AFFECTED(victim, AFF_POLYMORPH))
		strcat(buf, "     ");
	else
	{
		if(!IS_NPC(victim) && victim->desc == 0)
			strcat(buf, "(Link-Dead) ");
		if(IS_AFFECTED(victim, AFF_INVISIBLE))
			strcat(buf, "(Invis) ");
		if(IS_AFFECTED(victim, AFF_HIDE))
			strcat(buf, "(Hide) ");
		if(IS_AFFECTED(victim, AFF_CHARM))
			strcat(buf, "(Charmed) ");
		if(IS_AFFECTED(victim, AFF_PASS_DOOR) || IS_AFFECTED(victim, AFF_ETHEREAL))
			strcat(buf, "(Translucent) ");
		if(IS_AFFECTED(victim, AFF_FAERIE_FIRE))
			strcat(buf, "(Pink Aura) ");
		if(IS_EVIL(victim) && IS_AFFECTED(ch, AFF_DETECT_EVIL))
			strcat(buf, "(Red Aura) ");
		if(IS_AFFECTED(victim, AFF_SANCTUARY))
			strcat(buf, "(White Aura) ");
		if(IS_AFFECTED(victim, AFF_DROWFIRE))
			strcat(buf, "(Drowfire) ");
	}
	if(IS_AFFECTED(ch, AFF_SHADOWPLANE) && !IS_AFFECTED(victim, AFF_SHADOWPLANE))
		strcat(buf, "(Normal plane) ");
	else if(!IS_AFFECTED(ch, AFF_SHADOWPLANE) && IS_AFFECTED(victim, AFF_SHADOWPLANE))
		strcat(buf, "(Shadowplane) ");
	/* Vampires and werewolves can recognise each other - KaVir */
	if(!IS_NPC(victim) && !IS_NPC(ch) && IS_CLASS(victim, CLASS_VAMPIRE) &&
	   IS_HERO(victim) && IS_HERO(ch) &&
	   (IS_CLASS(ch, CLASS_VAMPIRE) || IS_CLASS(ch, CLASS_WEREWOLF)
	    || IS_CLASS(ch, CLASS_HIGHLANDER) || IS_CLASS(ch, CLASS_DEMON)
	    || IS_SET(ch->special, SPC_CHAMPION) || IS_CLASS(ch, CLASS_DROW)
	    || IS_CLASS(ch, CLASS_NINJA) || IS_CLASS(ch, CLASS_MONK)))
		strcat(buf, "(Vampire) ");
	else if(!IS_NPC(victim) && !IS_NPC(ch) && IS_CLASS(victim, CLASS_WEREWOLF)
		&& IS_HERO(victim) && IS_HERO(ch) &&
		(IS_CLASS(ch, CLASS_VAMPIRE) || IS_CLASS(ch, CLASS_WEREWOLF)
		 || IS_CLASS(ch, CLASS_HIGHLANDER) || IS_CLASS(ch, CLASS_DEMON)
		 || IS_SET(ch->special, SPC_CHAMPION) || IS_CLASS(ch, CLASS_DROW)
		 || IS_CLASS(ch, CLASS_NINJA) || IS_CLASS(ch, CLASS_MONK) ))
		strcat(buf, "(Werewolf) ");
	else if(!IS_NPC(victim) && !IS_NPC(ch) && IS_CLASS(victim, CLASS_DEMON)
		&& IS_HERO(victim) && IS_HERO(ch) && IS_SET(victim->special, SPC_DEMON_LORD)
		&& (IS_CLASS(ch, CLASS_VAMPIRE) || IS_CLASS(ch, CLASS_WEREWOLF)
		    || IS_CLASS(ch, CLASS_HIGHLANDER) || IS_CLASS(ch, CLASS_DEMON)
		    || IS_SET(ch->special, SPC_CHAMPION) || IS_CLASS(ch, CLASS_DROW)
		    || IS_CLASS(ch, CLASS_NINJA) || IS_CLASS(ch, CLASS_MONK)))
		strcat(buf, "(Demon Lord) ");
	else if(!IS_NPC(victim) && !IS_NPC(ch) && IS_SET(victim->special, SPC_CHAMPION)
		&& IS_HERO(victim) && IS_HERO(ch) &&
		(IS_CLASS(ch, CLASS_VAMPIRE) || IS_CLASS(ch, CLASS_WEREWOLF)
		 || IS_CLASS(ch, CLASS_HIGHLANDER) || IS_CLASS(ch, CLASS_DEMON)
		 || IS_SET(ch->special, SPC_CHAMPION) || IS_CLASS(ch, CLASS_DROW)
		 || IS_CLASS(ch, CLASS_NINJA) || IS_CLASS(ch, CLASS_MONK)))
	{
		if(IS_SET(victim->special, SPC_SIRE) || IS_SET(victim->special, SPC_PRINCE))
			strcat(buf, "(Demon) ");
		else
			strcat(buf, "(Champion) ");
	}
	else if(!IS_NPC(victim) && !IS_NPC(ch) && IS_CLASS(victim, CLASS_HIGHLANDER)
		&& IS_HERO(victim) && IS_HERO(ch) &&
		(IS_CLASS(ch, CLASS_VAMPIRE) || IS_CLASS(ch, CLASS_WEREWOLF)
		 || IS_CLASS(ch, CLASS_HIGHLANDER) || IS_CLASS(ch, CLASS_DEMON)
		 || IS_SET(ch->special, SPC_CHAMPION) || IS_CLASS(ch, CLASS_DROW)
		 || IS_CLASS(ch, CLASS_NINJA) || IS_CLASS(ch, CLASS_MONK)))
		strcat(buf, "(Highlander) ");
	else if(!IS_NPC(victim) && !IS_NPC(ch) && IS_CLASS(victim, CLASS_DROW)
		&& IS_HERO(victim) && IS_HERO(ch) &&
		(IS_CLASS(ch, CLASS_VAMPIRE) || IS_CLASS(ch, CLASS_WEREWOLF)
		 || IS_CLASS(ch, CLASS_HIGHLANDER) || IS_CLASS(ch, CLASS_DEMON)
		 || IS_SET(ch->special, SPC_CHAMPION) || IS_CLASS(ch, CLASS_DROW)
		 || IS_CLASS(ch, CLASS_NINJA) || IS_CLASS(ch, CLASS_MONK)))
		strcat(buf, "(Drow) ");
	else if(!IS_NPC(victim) && !IS_NPC(ch) && IS_CLASS(victim, CLASS_MONK)
		&& IS_HERO(victim) && IS_HERO(ch) &&
		(IS_CLASS(ch, CLASS_VAMPIRE) || IS_CLASS(ch, CLASS_WEREWOLF)
		 || IS_CLASS(ch, CLASS_HIGHLANDER) || IS_CLASS(ch, CLASS_DEMON)
		 || IS_SET(ch->special, SPC_CHAMPION) || IS_CLASS(ch, CLASS_DROW)
		 || IS_CLASS(ch, CLASS_NINJA) || IS_CLASS(ch, CLASS_MONK)))
		strcat(buf, "(Monk) ");
	else if(!IS_NPC(victim) && !IS_NPC(ch) && IS_CLASS(victim, CLASS_NINJA)
		&& IS_HERO(victim) && IS_HERO(ch) &&
		(IS_CLASS(ch, CLASS_VAMPIRE) || IS_CLASS(ch, CLASS_WEREWOLF)
		 || IS_CLASS(ch, CLASS_HIGHLANDER) || IS_CLASS(ch, CLASS_DEMON)
		 || IS_SET(ch->special, SPC_CHAMPION) || IS_CLASS(ch, CLASS_DROW)
		 || IS_CLASS(ch, CLASS_NINJA) || IS_CLASS(ch, CLASS_MONK)))
		strcat(buf, "(Ninja) ");
	if(!IS_NPC(ch) && IS_VAMPAFF(ch, VAM_AUSPEX) &&
	   !IS_NPC(victim) && IS_CLASS(victim, CLASS_VAMPIRE) && IS_VAMPAFF(victim, VAM_DISGUISED))
	{
		strcat(buf, "(");
		strcat(buf, victim->name);
		strcat(buf, ") ");
	}

	if(IS_AFFECTED(victim, AFF_FLAMING))
	{
		if(IS_NPC(victim))
			sprintf(buf2, "\n\r...%s is engulfed in blazing flames!", victim->short_descr);
		else if(!IS_NPC(victim) && IS_AFFECTED(victim, AFF_POLYMORPH))
			sprintf(buf2, "\n\r...%s is engulfed in blazing flames!", victim->morph);
		else
			sprintf(buf2, "\n\r...%s is engulfed in blazing flames!", victim->name);
	}

	if(IS_AFFECTED(victim, AFF_DROWFIRE))
	{
		if(IS_NPC(victim))
			sprintf(buf2, "\n\r...%s is engulfed in magical flames!", victim->short_descr);
		else if(!IS_NPC(victim) && IS_AFFECTED(victim, AFF_POLYMORPH))
			sprintf(buf2, "\n\r...%s is engulfed in magical flames!", victim->morph);
		else
			sprintf(buf2, "\n\r...%s is engulfed in magical flames!", victim->name);
	}

	if(!IS_NPC(victim) && IS_HEAD(victim, LOST_HEAD) && IS_AFFECTED(victim, AFF_POLYMORPH))
	{
		if(IS_EXTRA(victim, GAGGED) && IS_EXTRA(victim, BLINDFOLDED))
			sprintf(buf3, "...%s is gagged and blindfolded!", victim->morph);
		else if(IS_EXTRA(victim, GAGGED))
			sprintf(buf3, "...%s is gagged!", victim->morph);
		else if(IS_EXTRA(victim, BLINDFOLDED))
			sprintf(buf3, "...%s is blindfolded!", victim->morph);
	}

	if(IS_HEAD(victim, LOST_HEAD) && IS_AFFECTED(victim, AFF_POLYMORPH))
	{
		strcat(buf, victim->morph);
		strcat(buf, " is lying here.");
		strcat(buf, buf2);
		strcat(buf, buf3);
		strcat(buf, "\n\r");
		buf[5] = UPPER(buf[5]);
		send_to_char(buf, ch);
		return;
	}

	if(IS_EXTRA(victim, TIED_UP))
	{
		if(IS_NPC(victim))
			sprintf(buf3, "\n\r...%s is tied up", victim->short_descr);
		else if(!IS_NPC(victim) && IS_AFFECTED(victim, AFF_POLYMORPH))
			sprintf(buf3, "\n\r...%s is tied up", victim->morph);
		else
			sprintf(buf3, "\n\r...%s is tied up", victim->name);
		if(IS_EXTRA(victim, GAGGED) && IS_EXTRA(victim, BLINDFOLDED))
			strcat(buf3, ", gagged and blindfolded!");
		else if(IS_EXTRA(victim, GAGGED))
			strcat(buf3, " and gagged!");
		else if(IS_EXTRA(victim, BLINDFOLDED))
			strcat(buf3, " and blindfolded!");
		else
			strcat(buf3, "!");
	}

	if(IS_AFFECTED(victim, AFF_WEBBED))
	{
		if(IS_NPC(victim))
			sprintf(buf4, "\n\r...%s is coated in a sticky web.", victim->short_descr);
		else if(!IS_NPC(victim) && IS_AFFECTED(victim, AFF_POLYMORPH))
			sprintf(buf4, "\n\r...%s is coated in a sticky web.", victim->morph);
		else
			sprintf(buf4, "\n\r...%s is coated in a sticky web.", victim->name);
		strcat(buf3, buf4);
	}

	if(!IS_NPC(victim) && IS_AFFECTED(victim, AFF_POLYMORPH))
		strcat(buf, victim->morph);
	else if(victim->position == POS_STANDING && victim->long_descr[0] != '\0' && (mount = victim->mount) == 0)
	{
		strcat(buf, victim->long_descr);
		send_to_char(buf, ch);
		if(IS_NPC(ch) || !IS_SET(ch->act, PLR_BRIEF))
		{
			if(IS_ITEMAFF(victim, ITEMA_SHOCKSHIELD))
				act("...$N is surrounded by a crackling shield of lightning.", ch, 0, victim, TO_CHAR);
			if(IS_ITEMAFF(victim, ITEMA_FIRESHIELD))
				act("...$N is surrounded by a burning shield of fire.", ch, 0, victim, TO_CHAR);
			if(IS_ITEMAFF(victim, ITEMA_ICESHIELD))
				act("...$N is surrounded by a shimmering shield of ice.", ch, 0, victim, TO_CHAR);
			if(IS_ITEMAFF(victim, ITEMA_ACIDSHIELD))
				act("...$N is surrounded by a bubbling shield of acid.", ch, 0, victim, TO_CHAR);
			if(IS_ITEMAFF(victim, ITEMA_CHAOSSHIELD))
				act("...$N is surrounded by a swirling shield of chaos.", ch, 0, victim, TO_CHAR);
			if(IS_ITEMAFF(victim, ITEMA_REFLECT))
				act("...$N is surrounded by a flickering shield of darkness.", ch, 0, victim, TO_CHAR);
		}
		return;
	}
	else
		strcat(buf, PERS(victim, ch));

	if((mount = victim->mount) != 0 && victim->mounted == IS_RIDING)
	{
		if(IS_NPC(mount))
			sprintf(mount2, " is here riding %s", mount->short_descr);
		else
			sprintf(mount2, " is here riding %s", mount->name);
		strcat(buf, mount2);
		if(victim->position == POS_FIGHTING)
		{
			strcat(buf, ", fighting ");
			if(victim->fighting == 0)
				strcat(buf, "thin air??");
			else if(victim->fighting == ch)
				strcat(buf, "YOU!");
			else if(victim->in_room == victim->fighting->in_room)
			{
				strcat(buf, PERS(victim->fighting, ch));
				strcat(buf, ".");
			}
			else
				strcat(buf, "somone who left??");
		}
		else
			strcat(buf, ".");
	}
	else if(victim->position == POS_STANDING && IS_AFFECTED(victim, AFF_FLYING))
		strcat(buf, " is hovering here");
	else if(victim->position == POS_STANDING && (!IS_NPC(victim) && (IS_VAMPAFF(victim, VAM_FLYING))))
		strcat(buf, " is hovering here");
	else if(victim->position == POS_STANDING && (!IS_NPC(victim) &&
						     IS_CLASS(victim, CLASS_DROW)
						     && IS_SET(victim->pcdata->powers[1], DPOWER_LEVITATION)))
		strcat(buf, " is hovering here");
	else
	{
		switch (victim->position)
		{
		case POS_DEAD:
			strcat(buf, " is DEAD!!");
			break;
		case POS_MORTAL:
			strcat(buf, " is mortally wounded.");
			break;
		case POS_INCAP:
			strcat(buf, " is incapacitated.");
			break;
		case POS_STUNNED:
			strcat(buf, " is lying here stunned.");
			break;
		case POS_SLEEPING:
			strcat(buf, " is sleeping here.");
			break;
		case POS_RESTING:
			strcat(buf, " is resting here.");
			break;
		case POS_MEDITATING:
			strcat(buf, " is meditating here.");
			break;
		case POS_SITTING:
			strcat(buf, " is sitting here.");
			break;
		case POS_STANDING:
			if(!IS_NPC(victim) && victim->stance[0] == STANCE_NORMAL)
				strcat(buf, " is here, crouched in a fighting stance.");
			else if(!IS_NPC(victim) && victim->stance[0] == STANCE_VIPER)
				strcat(buf, " is here, crouched in a viper fighting stance.");
			else if(!IS_NPC(victim) && victim->stance[0] == STANCE_CRANE)
				strcat(buf, " is here, crouched in a crane fighting stance.");
			else if(!IS_NPC(victim) && victim->stance[0] == STANCE_CRAB)
				strcat(buf, " is here, crouched in a crab fighting stance.");
			else if(!IS_NPC(victim) && victim->stance[0] == STANCE_MONGOOSE)
				strcat(buf, " is here, crouched in a mongoose fighting stance.");
			else if(!IS_NPC(victim) && victim->stance[0] == STANCE_BULL)
				strcat(buf, " is here, crouched in a bull fighting stance.");
			else if(!IS_NPC(victim) && victim->stance[0] == STANCE_MANTIS)
				strcat(buf, " is here, crouched in a mantis fighting stance.");
			else if(!IS_NPC(victim) && victim->stance[0] == STANCE_DRAGON)
				strcat(buf, " is here, crouched in a dragon fighting stance.");
			else if(!IS_NPC(victim) && victim->stance[0] == STANCE_TIGER)
				strcat(buf, " is here, crouched in a tiger fighting stance.");
			else if(!IS_NPC(victim) && victim->stance[0] == STANCE_MONKEY)
				strcat(buf, " is here, crouched in a monkey fighting stance.");
			else if(!IS_NPC(victim) && victim->stance[0] == STANCE_SWALLOW)
				strcat(buf, " is here, crouched in a swallow fighting stance.");
			else
				strcat(buf, " is here.");
			break;
		case POS_FIGHTING:
			strcat(buf, " is here, fighting ");
			if(victim->fighting == 0)
				strcat(buf, "thin air??");
			else if(victim->fighting == ch)
				strcat(buf, "YOU!");
			else if(victim->in_room == victim->fighting->in_room)
			{
				strcat(buf, PERS(victim->fighting, ch));
				strcat(buf, ".");
			}
			else
				strcat(buf, "somone who left??");
			break;
		}
	}

	strcat(buf, buf2);
	strcat(buf, buf3);
	strcat(buf, "\n\r");
	buf[0] = UPPER(buf[0]);
	send_to_char(buf, ch);

	if(!IS_NPC(ch) && IS_SET(ch->act, PLR_BRIEF))
		return;

	if(IS_ITEMAFF(victim, ITEMA_SHOCKSHIELD))
		act("...$N is surrounded by a crackling shield of lightning.", ch, 0, victim, TO_CHAR);
	if(IS_ITEMAFF(victim, ITEMA_FIRESHIELD))
		act("...$N is surrounded by a burning shield of fire.", ch, 0, victim, TO_CHAR);
	if(IS_ITEMAFF(victim, ITEMA_ICESHIELD))
		act("...$N is surrounded by a shimmering shield of ice.", ch, 0, victim, TO_CHAR);
	if(IS_ITEMAFF(victim, ITEMA_ACIDSHIELD))
		act("...$N is surrounded by a bubbling shield of acid.", ch, 0, victim, TO_CHAR);
	if(IS_ITEMAFF(victim, ITEMA_CHAOSSHIELD))
		act("...$N is surrounded by a swirling shield of chaos.", ch, 0, victim, TO_CHAR);
	if(IS_ITEMAFF(victim, ITEMA_REFLECT))
		act("...$N is surrounded by a flickering shield of darkness.", ch, 0, victim, TO_CHAR);
	return;
}



void evil_eye(CHAR_DATA * victim, CHAR_DATA * ch)
{
	char buf[MAX_STRING_LENGTH];
	long tabletype;

	return;

	if(IS_NPC(victim))
		return;
	if(victim == ch)
		return;
	if(ch->level != 3 || victim->level != 3)
		return;
	if(!IS_VAMPAFF(victim, VAM_DOMINATE))
		return;
	if(victim->powertype != 0 && strlen(victim->powertype) > 1)
	{
		sprintf(buf, "\n\r%s\n\r", victim->powertype);
		send_to_char(buf, ch);
	}
	if(IS_SET(victim->spectype, EYE_SELFACTION) && victim->poweraction != 0)
		interpret(victim, victim->poweraction);
	if(IS_SET(victim->spectype, EYE_ACTION) && victim->poweraction != 0)
		interpret(ch, victim->poweraction);
	if(IS_SET(victim->spectype, EYE_SPELL) && victim->specpower > 0)
	{
		tabletype = skill_table[victim->specpower].target;
		(*skill_table[victim->specpower].spell_fun) (victim->specpower, victim->spl[tabletype], victim, ch);
	}
	return;
}


void show_char_to_char_1(CHAR_DATA * victim, CHAR_DATA * ch)
{
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;
	long iWear;
	long percent;
	bool found;

	if(can_see(victim, ch))
	{
		act("$n looks at you.", ch, 0, victim, TO_VICT);
		act("$n looks at $N.", ch, 0, victim, TO_NOTVICT);
	}

	if(!IS_NPC(ch) && IS_HEAD(victim, LOST_HEAD))
	{
		act("$N is lying here.", ch, 0, victim, TO_CHAR);
		return;
	}
	if(victim->description[0] != '\0')
	{
		send_to_char(victim->description, ch);
	}
	else
	{
		act("You see nothing special about $M.", ch, 0, victim, TO_CHAR);
	}

	if(victim->max_hit > 0)
		percent = (100 * victim->hit) / victim->max_hit;
	else
		percent = -1;

	strcpy(buf, PERS(victim, ch));

	if(percent >= 100)
		strcat(buf, " is in perfect health.\n\r");
	else if(percent >= 90)
		strcat(buf, " is slightly scratched.\n\r");
	else if(percent >= 80)
		strcat(buf, " has a few bruises.\n\r");
	else if(percent >= 70)
		strcat(buf, " has some cuts.\n\r");
	else if(percent >= 60)
		strcat(buf, " has several wounds.\n\r");
	else if(percent >= 50)
		strcat(buf, " has many nasty wounds.\n\r");
	else if(percent >= 40)
		strcat(buf, " is bleeding freely.\n\r");
	else if(percent >= 30)
		strcat(buf, " is covered in blood.\n\r");
	else if(percent >= 20)
		strcat(buf, " is leaking guts.\n\r");
	else if(percent >= 10)
		strcat(buf, " is almost dead.\n\r");
	else
		strcat(buf, " is DYING.\n\r");

	buf[0] = UPPER(buf[0]);
	send_to_char(buf, ch);

	if(!IS_NPC(victim))
	{
		if(IS_AFFECTED(victim, AFF_INFRARED) || IS_VAMPAFF(victim, VAM_NIGHTSIGHT))
			act("$N's eyes are glowing bright red.", ch, 0, victim, TO_CHAR);
		if(IS_AFFECTED(victim, AFF_FLYING))
			act("$N is hovering in the air.", ch, 0, victim, TO_CHAR);
		if(!IS_NPC(victim) && IS_CLASS(victim, CLASS_DROW) && IS_SET(victim->pcdata->powers[1], DPOWER_LEVITATION))
			act("$N is hovering in the air.", ch, 0, victim, TO_CHAR);
		if(IS_VAMPAFF(victim, VAM_FANGS))
			act("$N has a pair of long, pointed fangs.", ch, 0, victim, TO_CHAR);
		if(!IS_NPC(victim) && IS_CLASS(victim, CLASS_VAMPIRE) && IS_VAMPAFF(victim, VAM_CLAWS))
			act("$N has razer sharp claws protruding from under $S finger nails.", ch, 0, victim, TO_CHAR);
		else if(!IS_NPC(victim) && IS_VAMPAFF(victim, VAM_CLAWS))
			act("$N has razer sharp talons extending from $S fingers.", ch, 0, victim, TO_CHAR);
		if(!IS_NPC(victim) && (IS_CLASS(victim, CLASS_DEMON) || IS_SET(victim->special, SPC_CHAMPION)))
		{
			if(IS_DEMAFF(victim, DEM_HORNS))
				act("$N has a pair of pointed horns extending from $S head.", ch, 0, victim, TO_CHAR);
			if(IS_DEMAFF(victim, DEM_HOOVES))
				act("$N has a huge pair of hooves in place of his feet.", ch, 0, victim, TO_CHAR);
			if(IS_DEMAFF(victim, DEM_TAIL))
				act("$N has a huge tail extended from $S back side.", ch, 0, victim, TO_CHAR);
			if(IS_DEMAFF(victim, DEM_WINGS))
			{
				if(IS_DEMAFF(victim, DEM_UNFOLDED))
					act("$N has a pair of batlike wings spread out from behind $S back.", ch, 0,
					    victim, TO_CHAR);
				else
					act("$N has a pair of batlike wings folded behind $S back.", ch, 0, victim,
					    TO_CHAR);
			}
		}
	}
	found = FALSE;
	for(iWear = 0; iWear < MAX_WEAR; iWear++)
	{
		if((obj = get_eq_char(victim, iWear)) != 0 && can_see_obj(ch, obj))
		{
			if(!found)
			{
				send_to_char("\n\r", ch);
				act("$N is using:", ch, 0, victim, TO_CHAR);
				found = TRUE;
			}
			send_to_char(where_name[iWear], ch);
			if(IS_NPC(ch) || ch->pcdata->chobj == 0 || ch->pcdata->chobj != obj)
			{
				send_to_char(format_obj_to_char(obj, ch, TRUE), ch);
				send_to_char("\n\r", ch);
			}
			else
				send_to_char("you\n\r", ch);
		}
	}

	if(victim != ch && !IS_NPC(ch) && !IS_HEAD(victim, LOST_HEAD) && number_percent() < ch->pcdata->learned[gsn_peek])
	{
		send_to_char("\n\rYou peek at the inventory:\n\r", ch);
		show_list_to_char(victim->carrying, ch, TRUE, TRUE);
	}

	return;
}



void show_char_to_char(CHAR_DATA * list, CHAR_DATA * ch)
{
	CHAR_DATA *rch;

	for(rch = list; rch != 0; rch = rch->next_in_room)
	{
		if(rch == ch)
			continue;

		if(IS_IMMORTAL(ch) || IS_IMMORTAL(rch))
		{
			if(!IS_NPC(rch) && IS_SET(rch->act, PLR_WIZINVIS) && get_trust(ch) < get_trust(rch))
				continue;
		}
		else
		{
			if(!IS_NPC(rch)
			   && (IS_SET(rch->act, PLR_WIZINVIS)
			       || IS_ITEMAFF(rch, ITEMA_VANISH))
			   && !IS_SET(ch->act, PLR_HOLYLIGHT) && !IS_ITEMAFF(ch, ITEMA_VISION))
				continue;
		}

		if(!IS_NPC(rch) && IS_HEAD(rch, LOST_HEAD))
			continue;

		if(!IS_NPC(rch) && IS_EXTRA(rch, EXTRA_OSWITCH))
			continue;

		if(can_see(ch, rch))
			show_char_to_char_0(rch, ch);
		else if(room_is_dark(ch->in_room)
			&& (IS_AFFECTED(rch, AFF_INFRARED) || (!IS_NPC(rch) && IS_VAMPAFF(rch, VAM_NIGHTSIGHT))))
		{
			send_to_char("You see glowing red eyes watching YOU!\n\r", ch);
		}
	}

	return;
}



bool check_blind(CHAR_DATA * ch)
{
	if(!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT) && !IS_AFFECTED(ch, AFF_TOTALBLIND))
		return TRUE;

	if(IS_ITEMAFF(ch, ITEMA_VISION) && !IS_AFFECTED(ch, AFF_TOTALBLIND))
		return TRUE;

	if(IS_HEAD(ch, LOST_EYE_L) && IS_HEAD(ch, LOST_EYE_R))
	{
		send_to_char("You have no eyes to see with!\n\r", ch);
		return FALSE;
	}

	if(IS_EXTRA(ch, BLINDFOLDED))
	{
		send_to_char("You can't see a thing through the blindfold!\n\r", ch);
		return FALSE;
	}

	if(IS_AFFECTED(ch, AFF_BLIND) && !IS_AFFECTED(ch, AFF_SHADOWSIGHT))
	{
		send_to_char("You can't see a thing!\n\r", ch);
		return FALSE;
	}

	if(IS_AFFECTED(ch, AFF_TOTALBLIND) && !IS_IMMORTAL(ch))
	{
		send_to_char("You can't see through this shroud of darkness!\n\r", ch);
		return FALSE;
	}
	return TRUE;
}

/*
void do_random(CHAR_DATA *ch, char *argument)
{
char arg[MIL];
char arg2[MIL];

      long cow = 0;
        struct timeval          now_time;
        double                  counttime;
        long days, hours, minutes, seconds;
        long totsec;
	long start = 0;
	long end = 0;

gettimeofday(&now_time,0);
totsec = (time_t)now_time.tv_sec;
counttime = (946706400 - totsec);
if (counttime < 0)
{
        counttime = (totsec - 946706400);
        cow = 1;
}

days = counttime/86400;
counttime -= 86400*days;

hours = counttime/3600;
counttime -= 3600*hours;

minutes = counttime/60;
counttime -= 60*minutes;

seconds = counttime;

stc("Thank you for using Jason's random number generator.\n\r",ch);

argument = one_argument(argument,arg);
one_argument(argument,arg2);

if (arg[0] == '\0')
{
	stc("no range currently will return only a 0 or a 1.  But it's random!\n\r",ch);
	stc("The number is: ",ch);
	if (seconds % 2 == 0)
		stc("1.\n\r",ch);
	else
		stc("0.\n\r",ch);
	return;
}

if (arg2[0] == '\0')
{
	stc("To be a range, you must have two numbers.\n\r",ch);
	return;
}

if (!is_number(arg) || !is_number(arg2))
{
	stc("Syntax is random <number> <number>.\n\r",ch);
	return;
}

start = atoi(arg);
end = atoi(arg2);

if (start == end)
{
	sprintf(buf,"The number is %li.\n\r",start);
	stc(buf,ch);
	return;
}


// nifty loop to find a random number, like a bubble sort or something?
number = ((start + end)/2);
o


return;


}
*/



#define NEWS_FILE "news"

// Fri Oct  1 09:56:08 1999
// 123456789012345678901234

char *format_argument_for_changes(char *argument)
{
	static char buf[MAX_STRING_LENGTH];
	const char *standard_format = "\n\r{x{x               ";
	long length;
	long i, ibuf, nextline;

	if(!argument || argument[0] == '\0')
		return 0;

	if((length = strlen(argument)) > (MAX_STRING_LENGTH - (length / 80 * 26)))
		return 0;

	memset(buf,0,MAX_STRING_LENGTH);

	for(i = 0, ibuf = 0, nextline = 0; i < length; i++, nextline++)
	{
		if(nextline >= 60 && argument[i] == ' ')
		{
			strcat(&buf[ibuf], standard_format);
			ibuf += strlen(standard_format);
			nextline = 0;
			continue;
		}
		buf[ibuf++] = argument[i];
	}
	buf[ibuf] = '\0';

	return buf;
}



void do_news(CHAR_DATA * ch, char *argument)
{
	FILE *fp;
	char buf[MAX_STRING_LENGTH];
	char news[156];
	char *strtime;
	long length;
	bool num = FALSE;
	bool old = FALSE;
	const char *seperator =
		"\n\r{Y--------------------------------------------------------------------------------{X\n\r";

	extern time_t current_time;

	if(!ch || IS_NPC(ch))
		return;

	fclose(fpReserve);

	if(!IS_IMMORTAL(ch) || !argument || argument[0] == '\0' || (argument && argument[0] != '\0' && is_number(argument)))
	{
		if(argument && argument[0] != '\0' && is_number(argument))
			num = TRUE;

		if(num)
		{
			old = TRUE;
			sprintf(buf, "%s%s.txt", NEWS_FILE, argument);
		}
		else
		{
			sprintf(buf, "%s.txt", NEWS_FILE);
		}

		if((fp = fopen(buf, "r")) == 0)
		{
			send_to_char("No changes listed.\n\r", ch);
			fpReserve = fopen(NULL_FILE, "r");

			send_to_char("{cLatest news files from oldest to newest: ", ch);
			for(num = 1; num < 12; num++)
			{
				sprintf(buf, "news%li.txt", num);
				if((fp = fopen(buf, "r")) == 0)
					break;
				else
				{
					sprintf(news, "%li ", num);
					send_to_char(news, ch);
					fclose(fp);
				}
			}
			send_to_char("\n\rType (news #) to read them.{x\n\r", ch);

			return;
		}

		length = fread(buf, 1, MAX_STRING_LENGTH - 1 - strlen(seperator), fp);
		buf[length] = '\0';

		if(ch->pcdata->changesplace > length)	// new changes file
			ch->pcdata->changesplace = length;

		if(!num && ch->pcdata->changesplace != length)
		{
			char befbuf[MAX_STRING_LENGTH];
			char aftbuf[MAX_STRING_LENGTH];
			long i, k;

			for(i = 0; i < ch->pcdata->changesplace; i++)
				befbuf[i] = buf[i];
			befbuf[i] = '\0';

			for(k = 0; i < (long) strlen(buf); k++, i++)
				aftbuf[k] = buf[i];
			aftbuf[k] = '\0';

			sprintf(buf, "%s%s%s", befbuf, seperator, aftbuf);
			ch->pcdata->changesplace = length;
		}

		send_to_char(buf, ch);
		fclose(fp);

		send_to_char("{cLatest news files from oldest to newest: ", ch);
		for(num = 1; num < 12; num++)
		{
			sprintf(buf, "news%li.txt", num);
			if((fp = fopen(buf, "r")) == 0)
				break;
			else
			{
				sprintf(news, "%li ", num);
				send_to_char(news, ch);
				fclose(fp);
			}
		}
		send_to_char("\n\rType (news #) to read them.{x\n\r", ch);

		if(!old && length >= MAX_STRING_LENGTH - 500 - (long) strlen(seperator))
		{
			char rename[256];

			log_string("{RCHANGES {WFILE {BIS {DTOO {GDAMN {YBIG!!!!!{x  Creating new news file!");

			sprintf(rename, "mv %s.txt %s", NEWS_FILE, buf);
			system(rename);
		}

		fpReserve = fopen(NULL_FILE, "r");
		return;
	}

	sprintf(buf, "%s.txt", NEWS_FILE);

	if((fp = fopen(buf, "a")) == 0)
	{
		send_to_char("Error opening file.\n\r", ch);
		fpReserve = fopen(NULL_FILE, "r");
		return;
	}
	
	strtime = ctime(&current_time);
	strtime[10] = '\0';

	if((argument = format_argument_for_changes(argument)) == 0)
	{
		send_to_char("Formatting argument had a problem.\n\r", ch);
		fpReserve = fopen(NULL_FILE, "r");
		return;
	}

	fprintf(fp, "\r- {C%s{x - %s\n\r\n\r", strtime, argument);
	fclose(fp);
	fpReserve = fopen(NULL_FILE, "r");
	do_info(ch, "News added.");
	return;
}


void do_rows(CHAR_DATA * ch, char *argument)
{
	if(!argument || !is_number(argument))
	{
		send_to_char("Usage: rows <how many rows you got, duh>\n\r",ch);
		return;
	}
	ch->rows = atoi(argument);
	send_to_char("Mmk.\n\r",ch);
	init_vt(ch);
}


void do_cols(CHAR_DATA * ch, char *argument)
{
	if(!argument || !is_number(argument))
	{
		send_to_char("Usage: cols <how many columns you got, duh>\n\r",ch);
		return;
	}
	ch->cols = atoi(argument);
	send_to_char("Mmk.\n\r",ch);
	init_vt(ch);
}



void do_look(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	EXIT_DATA *pexit;
	CHAR_DATA *victim;
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;
	CHAR_DATA *wizard;
	OBJ_DATA *obj;
	OBJ_DATA *portal;
	OBJ_DATA *portal_next;
	ROOM_INDEX_DATA *pRoomIndex;
	ROOM_INDEX_DATA *location;
	char *pdesc;
	long door;
	bool found;

	if(ch->desc == 0 && (wizard = ch->wizard) == 0)
		return;

	if(ch->in_room == 0)
		return;

	if(ch->position < POS_SLEEPING)
	{
		send_to_char("You can't see anything but stars!\n\r", ch);
		return;
	}

	if(ch->position == POS_SLEEPING)
	{
		send_to_char("You can't see anything, you're sleeping!\n\r", ch);
		return;
	}

	if(!check_blind(ch))
		return;

	if(!IS_NPC(ch) && IS_SET(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS) &&
	   !IS_ITEMAFF(ch, ITEMA_VISION) && !IS_IMMORTAL(ch) &&
	   !(IS_CLASS(ch, CLASS_DROW) && IS_SET(ch->act, PLR_HOLYLIGHT)))
	{
		send_to_char("It is pitch black ... \n\r", ch);
		return;
	}

	if(!IS_NPC(ch)
	   && !IS_SET(ch->act, PLR_HOLYLIGHT)
	   && !IS_ITEMAFF(ch, ITEMA_VISION)
	   && !IS_VAMPAFF(ch, VAM_NIGHTSIGHT)
	   && !IS_AFFECTED(ch, AFF_SHADOWPLANE)
	   && !(ch->in_room != 0 && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT
		&& !IS_NPC(ch) && ch->pcdata->chobj != 0
		&& ch->pcdata->chobj->in_obj != 0) && room_is_dark(ch->in_room))
	{
		send_to_char("It is pitch black ... \n\r", ch);
		show_char_to_char(ch->in_room->people, ch);
		return;
	}

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if(arg1[0] == '\0' || !str_cmp(arg1, "auto"))
	{
		/* 'look' or 'look auto' */

		draw_map(ch);

		if(ch->in_room != 0 && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT
		   && !IS_NPC(ch) && ch->pcdata->chobj != 0 && ch->pcdata->chobj->in_obj != 0)
			act("$p", ch, ch->pcdata->chobj->in_obj, 0, TO_CHAR);
		else if(IS_AFFECTED(ch, AFF_SHADOWPLANE))
			send_to_char("The shadow plane\n\r", ch);
		else
		{
			sprintf(buf, "{C%s{x\n\r", ch->in_room->name);
			send_to_char(buf, ch);
		}

		if(!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOEXIT))
			do_exits(ch, "auto");

		if(ch->in_room != 0 && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT
		   && !IS_NPC(ch) && ch->pcdata->chobj != 0 && ch->pcdata->chobj->in_obj != 0)
		{
			act("You are inside $p.", ch, ch->pcdata->chobj->in_obj, 0, TO_CHAR);
			show_list_to_char(ch->pcdata->chobj->in_obj->contains, ch, FALSE, FALSE);
		}
		else if((arg1[0] == '\0' || !str_cmp(arg1, "auto")) && IS_AFFECTED(ch, AFF_SHADOWPLANE))
			send_to_char("You are standing in complete darkness.\n\r", ch);
		else if((!IS_NPC(ch) && !IS_SET(ch->act, PLR_BRIEF)) && (arg1[0] == '\0' || !str_cmp(arg1, "auto")))
		{
			send_to_char(ch->in_room->description, ch);
			if(ch->in_room->blood == 1000)
				sprintf(buf, "{RYou notice that the room is completely drenched in blood.{x\n\r");
			else if(ch->in_room->blood > 750)
				sprintf(buf,
					"{RYou notice that there is a very large amount of blood around the room.{x\n\r");
			else if(ch->in_room->blood > 500)
				sprintf(buf, "{RYou notice that there is a large quantity of blood around the room.{x\n\r");
			else if(ch->in_room->blood > 250)
				sprintf(buf, "{RYou notice a fair amount of blood on the floor.{x\n\r");
			else if(ch->in_room->blood > 100)
				sprintf(buf, "{RYou notice several blood stains on the floor.{x\n\r");
			else if(ch->in_room->blood > 50)
				sprintf(buf, "{RYou notice a few blood stains on the floor.{x\n\r");
			else if(ch->in_room->blood > 25)
				sprintf(buf, "{RYou notice a couple of blood stains on the floor.{x\n\r");
			else if(ch->in_room->blood > 0)
				sprintf(buf, "{RYou notice a few drops of blood on the floor.{x\n\r");
			else
				sprintf(buf, "{RYou notice nothing special in the room.{x\n\r");
			if(ch->in_room->blood > 0)
				send_to_char(buf, ch);
		}

		show_list_to_char(ch->in_room->contents, ch, FALSE, FALSE);
		show_char_to_char(ch->in_room->people, ch);
		return;
	}

	if(!str_cmp(arg1, "i") || !str_cmp(arg1, "in"))
	{
		/* 'look in' */
		if(arg2[0] == '\0')
		{
			send_to_char("Look in what?\n\r", ch);
			return;
		}

		if((obj = get_obj_here(ch, arg2)) == 0)
		{
			send_to_char("You do not see that here.\n\r", ch);
			return;
		}

		switch (obj->item_type)
		{
		default:
			send_to_char("That is not a container.\n\r", ch);
			break;

		case ITEM_PORTAL:
		case ITEM_WGATE:
			pRoomIndex = get_room_index(obj->value[0]);
			location = ch->in_room;
			if(pRoomIndex == 0)
			{
				send_to_char("It doesn't seem to lead anywhere.\n\r", ch);
				return;
			}
			if((obj->value[2] == 1 || obj->value[2] == 3) && obj->item_type == ITEM_PORTAL)
			{
				send_to_char("It seems to be closed.\n\r", ch);
				return;
			}

			char_from_room(ch);
			char_to_room(ch, pRoomIndex);

			found = FALSE;
			for(portal = ch->in_room->contents; portal != 0; portal = portal_next)
			{
				portal_next = portal->next_content;
				if((obj->value[0] == portal->value[3]) && (obj->value[3] == portal->value[0]))
				{
					found = TRUE;
					if(obj->item_type == ITEM_WGATE)
					{
						stc("You peer through the magical rift...\n\r", ch);
					}

					if(IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
					   !IS_SET(portal->extra_flags, ITEM_SHADOWPLANE))
					{
						REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
						do_look(ch, "auto");
						SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
						break;
					}
					else if(!IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
						IS_SET(portal->extra_flags, ITEM_SHADOWPLANE))
					{
						SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
						do_look(ch, "auto");
						REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
						break;
					}
					else
					{
						do_look(ch, "auto");
						break;
					}
				}
			}
			char_from_room(ch);
			char_to_room(ch, location);
			break;

		case ITEM_DRINK_CON:
			if(obj->value[1] <= 0)
			{
				send_to_char("It is empty.\n\r", ch);
				break;
			}

			if(obj->value[1] < obj->value[0] / 5)
				sprintf(buf, "There is a little %s liquid left in it.\n\r",
					liq_table[obj->value[2]].liq_color);
			else if(obj->value[1] < obj->value[0] / 4)
				sprintf(buf, "It contains a small about of %s liquid.\n\r",
					liq_table[obj->value[2]].liq_color);
			else if(obj->value[1] < obj->value[0] / 3)
				sprintf(buf, "It's about a third full of %s liquid.\n\r",
					liq_table[obj->value[2]].liq_color);
			else if(obj->value[1] < obj->value[0] / 2)
				sprintf(buf, "It's about half full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
			else if(obj->value[1] < obj->value[0])
				sprintf(buf, "It is almost full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
			else if(obj->value[1] == obj->value[0])
				sprintf(buf, "It's completely full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
			else
				sprintf(buf, "Somehow it is MORE than full of %s liquid.\n\r",
					liq_table[obj->value[2]].liq_color);
			send_to_char(buf, ch);
			break;

		case ITEM_CONTAINER:
		case ITEM_CORPSE_NPC:
		case ITEM_CORPSE_PC:
			if(IS_SET(obj->value[1], CONT_CLOSED))
			{
				send_to_char("It is closed.\n\r", ch);
				break;
			}

			act("$p contains:", ch, obj, 0, TO_CHAR);
			show_list_to_char(obj->contains, ch, TRUE, TRUE);
			break;
		}
		return;
	}

	if((victim = get_char_room(ch, arg1)) != 0)
	{
		show_char_to_char_1(victim, ch);
		evil_eye(victim, ch);
		return;
	}

	for(vch = char_list; vch != 0; vch = vch_next)
	{
		vch_next = vch->next;
		if(vch->in_room == 0)
			continue;
		if(vch->in_room == ch->in_room)
		{
			if(!IS_NPC(vch) && !str_cmp(arg1, vch->morph))
			{
				show_char_to_char_1(vch, ch);
				evil_eye(vch, ch);
				return;
			}
			continue;
		}
	}

	if(!IS_NPC(ch) && ch->pcdata->chobj != 0 && ch->pcdata->chobj->in_obj != 0)
	{
		obj = get_obj_in_obj(ch, arg1);
		if(obj != 0)
		{
			send_to_char(obj->description, ch);
			send_to_char("\n\r", ch);
			return;
		}
	}

	for(obj = ch->carrying; obj != 0; obj = obj->next_content)
	{
		if(!IS_NPC(ch) && ch->pcdata->chobj != 0 && obj->chobj != 0 && obj->chobj == ch)
			continue;
		if(can_see_obj(ch, obj))
		{
			pdesc = get_extra_descr(arg1, obj->extra_descr);
			if(pdesc != 0)
			{
				send_to_char(pdesc, ch);
				return;
			}

			pdesc = get_extra_descr(arg1, obj->pIndexData->extra_descr);
			if(pdesc != 0)
			{
				send_to_char(pdesc, ch);
				return;
			}
		}

		if(is_name(arg1, obj->name))
		{
			send_to_char(obj->description, ch);
			send_to_char("\n\r", ch);
			return;
		}
	}

	for(obj = ch->in_room->contents; obj != 0; obj = obj->next_content)
	{
		if(!IS_NPC(ch) && ch->pcdata->chobj != 0 && obj->chobj != 0 && obj->chobj == ch)
			continue;
		if(can_see_obj(ch, obj))
		{
			pdesc = get_extra_descr(arg1, obj->extra_descr);
			if(pdesc != 0)
			{
				send_to_char(pdesc, ch);
				return;
			}

			pdesc = get_extra_descr(arg1, obj->pIndexData->extra_descr);
			if(pdesc != 0)
			{
				send_to_char(pdesc, ch);
				return;
			}
		}

		if(is_name(arg1, obj->name))
		{
			send_to_char(obj->description, ch);
			send_to_char("\n\r", ch);
			return;
		}
	}

	pdesc = get_extra_descr(arg1, ch->in_room->extra_descr);
	if(pdesc != 0)
	{
		send_to_char(pdesc, ch);
		return;
	}

	if(!str_cmp(arg1, "n") || !str_cmp(arg1, "north"))
		door = 0;
	else if(!str_cmp(arg1, "e") || !str_cmp(arg1, "east"))
		door = 1;
	else if(!str_cmp(arg1, "s") || !str_cmp(arg1, "south"))
		door = 2;
	else if(!str_cmp(arg1, "w") || !str_cmp(arg1, "west"))
		door = 3;
	else if(!str_cmp(arg1, "u") || !str_cmp(arg1, "up"))
		door = 4;
	else if(!str_cmp(arg1, "d") || !str_cmp(arg1, "down"))
		door = 5;
	else
	{
		send_to_char("You do not see that here.\n\r", ch);
		return;
	}

	/* 'look direction' */
	if((pexit = ch->in_room->exit[door]) == 0)
	{
		send_to_char("Nothing special there.\n\r", ch);
		return;
	}
/*
    if ( pexit->description != 0 && pexit->description[0] != '\0' )
	send_to_char( pexit->description, ch );
    else
	send_to_char( "Nothing special there.\n\r", ch );
*/

	if(pexit->keyword != 0 && pexit->keyword[0] != '\0' && pexit->keyword[0] != ' ')
	{
		if(IS_SET(pexit->exit_info, EX_CLOSED))
		{
			act("The $d is closed.", ch, 0, pexit->keyword, TO_CHAR);
		}
		else if(IS_SET(pexit->exit_info, EX_ISDOOR))
		{
			act("The $d is open.", ch, 0, pexit->keyword, TO_CHAR);
			if((pexit = ch->in_room->exit[door]) == 0)
				return;
			if((pRoomIndex = pexit->to_room) == 0)
				return;
			location = ch->in_room;
			char_from_room(ch);
			char_to_room(ch, pRoomIndex);
			do_look(ch, "auto");
			char_from_room(ch);
			char_to_room(ch, location);
		}
		else
		{
			if((pexit = ch->in_room->exit[door]) == 0)
				return;
			if((pRoomIndex = pexit->to_room) == 0)
				return;
			location = ch->in_room;
			char_from_room(ch);
			char_to_room(ch, pRoomIndex);
			do_look(ch, "auto");
			char_from_room(ch);
			char_to_room(ch, location);
		}
	}
	else
	{
		if((pexit = ch->in_room->exit[door]) == 0)
			return;
		if((pRoomIndex = pexit->to_room) == 0)
			return;
		location = ch->in_room;
		char_from_room(ch);
		char_to_room(ch, pRoomIndex);
		do_look(ch, "auto");
		char_from_room(ch);
		char_to_room(ch, location);
	}

	return;
}



void do_examine(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;

	one_argument(argument, arg);

	if(arg[0] == '\0')
	{
		send_to_char("Examine what?\n\r", ch);
		return;
	}

	do_look(ch, arg);

	if((obj = get_obj_here(ch, arg)) != 0)
	{
		if(obj->condition >= 100)
			sprintf(buf, "You notice that %s is in perfect condition.\n\r", obj->short_descr);
		else if(obj->condition >= 75)
			sprintf(buf, "You notice that %s is in good condition.\n\r", obj->short_descr);
		else if(obj->condition >= 50)
			sprintf(buf, "You notice that %s is in average condition.\n\r", obj->short_descr);
		else if(obj->condition >= 25)
			sprintf(buf, "You notice that %s is in poor condition.\n\r", obj->short_descr);
		else
			sprintf(buf, "You notice that %s is in awful condition.\n\r", obj->short_descr);
		send_to_char(buf, ch);
		switch (obj->item_type)
		{
		default:
			break;

		case ITEM_DRINK_CON:
		case ITEM_CONTAINER:
		case ITEM_CORPSE_NPC:
		case ITEM_CORPSE_PC:
			send_to_char("When you look inside, you see:\n\r", ch);
			sprintf(buf, "in %s", arg);
			do_look(ch, buf);
		}
	}

	return;
}



/*
 * Thanks to Zrin for auto-exit part.
 */
void do_exits(CHAR_DATA * ch, char *argument)
{
	extern char *const dir_name[];
	char buf[MAX_STRING_LENGTH];
	EXIT_DATA *pexit;
	bool found;
	bool fAuto;
	long door;

	buf[0] = '\0';
	fAuto = !str_cmp(argument, "auto");

	if(!check_blind(ch))
		return;

	strcpy(buf, fAuto ? "[{WExits{x:" : "Obvious exits:\n\r");

	found = FALSE;
	for(door = 0; door <= 5; door++)
	{
		if((pexit = ch->in_room->exit[door]) != 0
		   && pexit->to_room != 0 && !IS_SET(pexit->exit_info, EX_CLOSED))
		{
			found = TRUE;
			if(fAuto)
			{
				strcat(buf, " ");
				strcat(buf, dir_name[door]);
				strcat(buf, "{x");
			}
			else
			{
				sprintf(buf + strlen(buf), "%-5s - %s\n\r",
					capitalize(dir_name[door]),
					room_is_dark(pexit->to_room) ? "Too dark to tell" : pexit->to_room->name);
			}
		}
	}

	if(!found)
		strcat(buf, fAuto ? " none" : "None.\n\r");

	if(fAuto)
		strcat(buf, "]\n\r");

	send_to_char(buf, ch);
	return;
}


float pkill_factor(CHAR_DATA * ch)
{
	float rating = 0;
	float pkill, pdeath, age;
	static const float hundred = 100.0, twohundred = 200.0, five = 5.0, fourthousand = 4000.0;

	if(ch->pkill == 0 && ch->pdeath == 0)
		return 0;

	pkill = (float) UMAX(1, ch->pkill);
	pdeath = (float) UMAX(1, ch->pdeath);
	age = (float) UMAX(1, (2 * ((ch->played + (long) (current_time - ch->logon))) / 7200));
	rating += (float) (fourthousand * ((((pkill / age) * hundred) + (pkill / pdeath / five * hundred)) / twohundred));
// 4000* (((pk/hours * 100) + (pk/pd/5*100)) / 200)

	return rating;
}


long player_rating(CHAR_DATA * ch)
{
	OBJ_DATA *obj;
	long rating = 0;
	long i;

	if(IS_NPC(ch))
		return 0;

	for(i = 0; i < 10; i++)
		rating += UMIN(ch->stance[i], 200);

	if(IS_CLASS(ch, CLASS_HIGHLANDER))
		for(i = 0; i < 12; i++)
			rating += (UMIN(ch->wpn[i] == 0 ? 1 : ch->wpn[i], 1000)) / 5;
	else
		for(i = 0; i < 12; i++)
			rating += UMIN(ch->wpn[i], 200);

	for(i = 0; i < 4; i++)
		rating += UMIN(ch->spl[i], 240);

	for(obj = ch->carrying; obj; obj = obj->next_content)
	{
		if(obj->wear_loc < 0 && obj->points > 0)
			continue;

		rating += obj->points / 5;
	}

	if(ch->pcdata->stats[UNI_GEN] == 1)
		rating += 3000;
	if(ch->pcdata->stats[UNI_GEN] == 2)
		rating += 1500;

	rating += ch->max_hit / 6;
	rating += ch->max_move / 18;
	rating += ch->max_mana / 36;
	rating += ch->mkill == 0 ? 1 : ch->mkill / 70;
	return rating;
}



void do_score(CHAR_DATA * ch, char *argument)
{
	long x = 0;
	char buf[MAX_STRING_LENGTH];
	char ss1[MAX_STRING_LENGTH];
	char ss2[MAX_STRING_LENGTH];
	long a_c = char_ac(ch);
	char nametit[MSL];

	nametit[0] = '\0';

	sprintf(buf, "%s%s\n\r", ch->name, IS_NPC(ch) ? "" : ch->pcdata->title);

	if(strlen(buf) < 70)
	{
		for(x = 0; x < (long) (70 - strlen(buf)) / 2; x++)
			strcat(nametit, " ");
		strcat(nametit, buf);
		send_to_char(nametit, ch);
	}
	else
		send_to_char(buf, ch);

	if(!IS_NPC(ch) && (IS_EXTRA(ch, EXTRA_OSWITCH) || IS_HEAD(ch, LOST_HEAD)))
	{
		obj_score(ch, ch->pcdata->chobj);
		return;
	}
	sprintf(buf,
		"You are %s%s.  You have been playing for %li hours.\n\r",
		ch->name, IS_NPC(ch) ? "" : ch->pcdata->title, (get_age(ch) - 17) * 2);
	send_to_char(buf, ch);
	if(!IS_NPC(ch))
		birth_date(ch, TRUE);
	if(!IS_NPC(ch) && IS_EXTRA(ch, EXTRA_PREGNANT))
		birth_date(ch, FALSE);

	if(get_trust(ch) != ch->level)
	{
		sprintf(buf, "You are trusted at level %li.\n\r", get_trust(ch));
		send_to_char(buf, ch);
	}

	sprintf(buf,
		"You have %li/%li hit, %li/%li mana, %li/%li movement, %li primal energy.\n\r",
		ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move, ch->max_move, ch->practice);
	send_to_char(buf, ch);

	sprintf(buf,
		"You are carrying %li/%li items with weight %li/%li kg.\n\r",
		ch->carry_number, can_carry_n(ch), ch->carry_weight, can_carry_w(ch));
	send_to_char(buf, ch);

	sprintf(buf,
		"Str: %li  Int: %li  Wis: %li  Dex: %li  Con: %li.\n\r",
		get_curr_str(ch), get_curr_int(ch), get_curr_wis(ch), get_curr_dex(ch), get_curr_con(ch));
	send_to_char(buf, ch);

	sprintf(buf, "You have scored %li exp, and have %li gold coins.\n\r", ch->exp, ch->gold);
	send_to_char(buf, ch);

	if(!IS_NPC(ch) && (IS_CLASS(ch, CLASS_DEMON) || IS_SET(ch->special, SPC_CHAMPION)))
	{
		sprintf(buf,
			"You have %li out of %li points of demonic power stored.\n\r",
			ch->pcdata->stats[DEMON_CURRENT], ch->pcdata->stats[DEMON_TOTAL]);
		send_to_char(buf, ch);
	}

	if(!IS_NPC(ch) && IS_CLASS(ch, CLASS_DROW))
	{
		sprintf(buf, "You have %li drow power points!.\n\r", ch->pcdata->stats[DROW_POWER]);
		send_to_char(buf, ch);
		sprintf(buf, "You have %li points of magic resistance.\n\r", ch->pcdata->stats[DROW_MAGIC]);
		send_to_char(buf, ch);
	}

	sprintf(buf,
		"Autoexit: %s.  Autoloot: %s.  Autosac: %s.\n\r",
		(!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOEXIT)) ? "yes" : "no",
		(!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOLOOT)) ? "yes" : "no",
		(!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOSAC)) ? "yes" : "no");
	send_to_char(buf, ch);

	sprintf(buf, "Wimpy set to %li hit points.", ch->wimpy);
	if(!IS_NPC(ch) && IS_EXTRA(ch, EXTRA_DONE))
		strcat(buf, "  You are no longer a virgin.\n\r");
	else
		strcat(buf, "\n\r");
	send_to_char(buf, ch);

	if(!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
		send_to_char("You are drunk.\n\r", ch);
	if(!IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] == 0)
		send_to_char("You are thirsty.\n\r", ch);
	if(!IS_NPC(ch) && ch->pcdata->condition[COND_FULL] == 0)
		send_to_char("You are hungry.\n\r", ch);
	if(!IS_NPC(ch) && ch->pcdata->stage[0] >= 100)
		send_to_char("You are feeling extremely horny.\n\r", ch);
	else if(!IS_NPC(ch) && ch->pcdata->stage[0] >= 50)
		send_to_char("You are feeling pretty randy.\n\r", ch);
	else if(!IS_NPC(ch) && ch->pcdata->stage[0] >= 1)
		send_to_char("You are feeling rather kinky.\n\r", ch);

	if(!IS_NPC(ch) && ch->pcdata->stage[1] > 0 && ch->position == POS_STANDING)
	{
		send_to_char("You are having sexual intercourse.\n\r", ch);
		if(!IS_NPC(ch) && (ch->pcdata->stage[2] + 25) >= ch->pcdata->stage[1])
			send_to_char("You are on the verge of having an orgasm.\n\r", ch);
	}
	else
		switch (ch->position)
		{
		case POS_DEAD:
			send_to_char("You are DEAD!!\n\r", ch);
			break;
		case POS_MORTAL:
			send_to_char("You are MORTALLY WOUNDED.\n\r", ch);
			break;
		case POS_INCAP:
			send_to_char("You are incapacitated.\n\r", ch);
			break;
		case POS_STUNNED:
			send_to_char("You are stunned.\n\r", ch);
			break;
		case POS_SLEEPING:
			send_to_char("You are sleeping.\n\r", ch);
			break;
		case POS_RESTING:
			send_to_char("You are resting.\n\r", ch);
			break;
		case POS_MEDITATING:
			send_to_char("You are meditating.\n\r", ch);
			break;
		case POS_SITTING:
			send_to_char("You are sitting.\n\r", ch);
			break;
		case POS_STANDING:
			send_to_char("You are standing.\n\r", ch);
			break;
		case POS_FIGHTING:
			send_to_char("You are fighting.\n\r", ch);
			break;
		}

	if(ch->level >= 0)
	{
		sprintf(buf, "AC: %li.  ", a_c);
		send_to_char(buf, ch);
	}

	send_to_char("You are ", ch);
	if(a_c >= 101)
		send_to_char("naked!\n\r", ch);
	else if(a_c >= 80)
		send_to_char("barely clothed.\n\r", ch);
	else if(a_c >= 60)
		send_to_char("wearing clothes.\n\r", ch);
	else if(a_c >= 40)
		send_to_char("slightly armored.\n\r", ch);
	else if(a_c >= 20)
		send_to_char("somewhat armored.\n\r", ch);
	else if(a_c >= 0)
		send_to_char("armored.\n\r", ch);
	else if(a_c >= -50)
		send_to_char("well armored.\n\r", ch);
	else if(a_c >= -100)
		send_to_char("strongly armored.\n\r", ch);
	else if(a_c >= -250)
		send_to_char("heavily armored.\n\r", ch);
	else if(a_c >= -500)
		send_to_char("superbly armored.\n\r", ch);
	else if(a_c >= -749)
		send_to_char("divinely armored.\n\r", ch);
	else
		send_to_char("ultimately armored!\n\r", ch);

	sprintf(buf, "Hitroll: %li.  Damroll: %li.  Damcap: %li.  ", char_hitroll(ch), char_damroll(ch), ch->damcap[0]);
	send_to_char(buf, ch);

	if(!IS_NPC(ch) && IS_CLASS(ch, CLASS_VAMPIRE))
	{
		sprintf(buf, "Blood: %li.\n\r", ch->pcdata->condition[COND_THIRST]);
		send_to_char(buf, ch);

		sprintf(buf, "Beast: %li.  ", ch->beast);
		send_to_char(buf, ch);
		if(ch->beast < 0)
			send_to_char("You are a cheat!\n\r", ch);
		else if(ch->beast == 0)
			send_to_char("You have attained Golconda!\n\r", ch);
		else if(ch->beast <= 5)
			send_to_char("You have almost reached Golconda!\n\r", ch);
		else if(ch->beast <= 10)
			send_to_char("You are nearing Golconda!\n\r", ch);
		else if(ch->beast <= 15)
			send_to_char("You have great control over your beast.\n\r", ch);
		else if(ch->beast <= 20)
			send_to_char("Your beast has little influence over your actions.\n\r", ch);
		else if(ch->beast <= 30)
			send_to_char("You are in control of your beast.\n\r", ch);
		else if(ch->beast <= 40)
			send_to_char("You are able to hold back the beast.\n\r", ch);
		else if(ch->beast <= 60)
			send_to_char("You are constantly struggling for control of your beast.\n\r", ch);
		else if(ch->beast <= 75)
			send_to_char("Your beast has great control over your actions.\n\r", ch);
		else if(ch->beast <= 90)
			send_to_char("The power of the beast overwhelms you.\n\r", ch);
		else if(ch->beast <= 99)
			send_to_char("You have almost lost your battle with the beast!\n\r", ch);
		else
			send_to_char("The beast has taken over!\n\r", ch);

	}
	else if(ch->level >= 0)
		send_to_char("\n\r", ch);

	if(ch->level >= 0)
	{
		sprintf(buf, "Alignment: %li.  ", ch->alignment);
		send_to_char(buf, ch);
	}

	send_to_char("You are ", ch);
	if(ch->alignment > 900)
		send_to_char("angelic.\n\r", ch);
	else if(ch->alignment > 700)
		send_to_char("saintly.\n\r", ch);
	else if(ch->alignment > 350)
		send_to_char("good.\n\r", ch);
	else if(ch->alignment > 100)
		send_to_char("kind.\n\r", ch);
	else if(ch->alignment > -100)
		send_to_char("neutral.\n\r", ch);
	else if(ch->alignment > -350)
		send_to_char("mean.\n\r", ch);
	else if(ch->alignment > -700)
		send_to_char("evil.\n\r", ch);
	else if(ch->alignment > -900)
		send_to_char("demonic.\n\r", ch);
	else
		send_to_char("satanic.\n\r", ch);

	if(!IS_NPC(ch) && ch->level >= 0)
	{
		sprintf(buf, "Status: %li.  ", ch->pcdata->status);
		send_to_char(buf, ch);
	}

	if(!IS_NPC(ch))
	{
		if(ch->pkill == 0)
			sprintf(ss1, "no players");
		else if(ch->pkill == 1)
			sprintf(ss1, "%li player", ch->pkill);
		else
			sprintf(ss1, "%li players", ch->pkill);
		if(ch->pdeath == 0)
			sprintf(ss2, "no players");
		else if(ch->pdeath == 1)
			sprintf(ss2, "%li player", ch->pdeath);
		else
			sprintf(ss2, "%li players", ch->pdeath);
		sprintf(buf, "\n\rYou have killed %s and have been killed by %s.\n\r", ss1, ss2);
		send_to_char(buf, ch);
		if(ch->mkill == 0)
			sprintf(ss1, "no mobs");
		else if(ch->mkill == 1)
			sprintf(ss1, "%li mob", ch->mkill);
		else
			sprintf(ss1, "%li mobs", ch->mkill);
		if(ch->mdeath == 0)
			sprintf(ss2, "no mobs");
		else if(ch->mdeath == 1)
			sprintf(ss2, "%li mob", ch->mdeath);
		else
			sprintf(ss2, "%li mobs", ch->mdeath);
		sprintf(buf, "You have killed %s and have been killed by %s.\n\r", ss1, ss2);
		send_to_char(buf, ch);
	}

	if(!IS_NPC(ch) && ch->pcdata->quest > 0)
	{
		if(ch->pcdata->quest == 1)
			sprintf(buf, "You have a single quest point.\n\r");
		else
			sprintf(buf, "You have %li quest points.\n\r", ch->pcdata->quest);
		send_to_char(buf, ch);
	}

	if(IS_AFFECTED(ch, AFF_HIDE))
		send_to_char("You are keeping yourself hidden from those around you.\n\r", ch);

	if(!IS_NPC(ch))
	{
		if(IS_CLASS(ch, CLASS_WEREWOLF) && ch->pcdata->powers[WPOWER_SILVER] > 0)
		{
			sprintf(buf, "You have attained %li points of silver tolerance.\n\r",
				ch->pcdata->powers[WPOWER_SILVER]);
			send_to_char(buf, ch);
		}
		if(IS_CLASS(ch, CLASS_VAMPIRE) && ch->pcdata->stats[UNI_RAGE] > 0)
		{
			sprintf(buf, "The beast is in control of your actions:  Affects Hitroll and Damroll by +%li.\n\r",
				ch->pcdata->stats[UNI_RAGE]);
			send_to_char(buf, ch);
		}
		else if(IS_SET(ch->special, SPC_WOLFMAN) && ch->pcdata->stats[UNI_RAGE] > 0)
		{
			sprintf(buf, "You are raging:  Affects Hitroll and Damroll by +%li.\n\r",
				ch->pcdata->stats[UNI_RAGE]);
			send_to_char(buf, ch);
		}
		else if(IS_CLASS(ch, CLASS_NINJA) && ch->pcdata->stats[UNI_RAGE] > 0)
		{
			sprintf(buf, "Michi:  Affects Hitroll and Damroll by +%li.\n\r", ch->pcdata->stats[UNI_RAGE]);
			send_to_char(buf, ch);
		}
		else if(IS_CLASS(ch, CLASS_DEMON) && ch->pcdata->stats[DEMON_POWER] > 0)
		{
			sprintf(buf, "You are wearing demonic armour:  Affects Hitroll and Damroll by +%li.\n\r",
				((ch->pcdata->stats[DEMON_POWER]) * ch->pcdata->stats[DEMON_POWER]));
			send_to_char(buf, ch);
		}
		else if(IS_SET(ch->special, SPC_CHAMPION) && ch->pcdata->stats[DEMON_POWER] > 0)
		{
			sprintf(buf, "You are wearing demonic armour:  Affects Hitroll and Damroll by +%li.\n\r",
				((ch->pcdata->stats[DEMON_POWER]) * ch->pcdata->stats[DEMON_POWER]));
			send_to_char(buf, ch);
		}
		send_to_char("You can view your affects via the 'affect' command.\n\r", ch);
	}

	return;
}


char *const day_name[] = {
	"gaint cows", "mudding with one hand", "unF", "mud sex", "chinese checkers",
	"royal mccheese nigga", "graphics suck"
};

char *const month_name[] = {

	"the azns", "", "the niggas", "the h4x0rs",
	"the crackers", "the biatches", "it", "breakfast", "the cheese",
	"pancakes", "the glue", "wednesday", "the door", "the calendar",
	"the large eyebrows", "hair", "the grass"
};


double get_uptime(void)
{
	struct timeval now_time;
	double uptime;
	extern struct timeval boot_time;
	extern long olduptime;

	get_time(&now_time);
	uptime = difftime((time_t) now_time.tv_sec, (time_t) boot_time.tv_sec);

	if(olduptime)
		uptime += olduptime;

	return uptime;
}


char *uptime(void)
{
	static char strtime[256];
	static long daymessage = 0;
	double uptime;
	long days, hours, minutes, seconds;

	uptime = get_uptime();

	days = (long) uptime / 86400;
	uptime -= 86400 * days;

	hours = (long) uptime / 3600;
	uptime -= 3600 * hours;

	minutes = (long) uptime / 60;
	uptime -= 60 * minutes;

	seconds = (long) uptime;

	sprintf(strtime, "Godwars has been up %li day%s, %li hour%s, %li minute%s and %li second%s.",
		days, days == 1 ? "" : "s",
		hours, hours == 1 ? "" : "s", minutes, minutes == 1 ? "" : "s", seconds, seconds == 1 ? "" : "s");

	if(days > daymessage)
	{
		daymessage = days;
		do_info(char_list, strtime);
	}

	return strtime;
}

void do_uptime(CHAR_DATA * ch, char *argument)
{
	if(!ch)
		return;

	send_to_char(uptime(), ch);
	send_to_char("\n\r", ch);
	return;
}

char *countdown(void)
{
	long cow = 0;
	static char countdown_time[256];
	struct timeval now_time;
	double counttime;
	long days, hours, minutes, seconds;
	long totsec;

	get_time(&now_time);
	totsec = (time_t) now_time.tv_sec;
	counttime = (946706400 - totsec);
	if(counttime < 0)
	{
		counttime = (totsec - 946706400);
		cow = 1;
	}

	days = (long) (counttime / 86400);
	counttime -= 86400 * days;

	hours = (long) (counttime / 3600);
	counttime -= 3600 * hours;

	minutes = (long) (counttime / 60);
	counttime -= 60 * minutes;

	seconds = (long) (counttime);

	if(cow != 1)
	{
		sprintf(countdown_time,
			"There are %li day%s, %li hour%s, %li minute%s and %li second%s left until the Year 2000.\n\r", days,
			days == 1 ? "" : "s", hours, hours == 1 ? "" : "s", minutes, minutes == 1 ? "" : "s", seconds,
			seconds == 1 ? "" : "s");
	}
	else
	{
		sprintf(countdown_time, "The Year 2000 arrived %li day%s, %li hour%s, %li minute%s and %li second%s ago.\n\r",
			days, days == 1 ? "" : "s",
			hours, hours == 1 ? "" : "s", minutes, minutes == 1 ? "" : "s", seconds, seconds == 1 ? "" : "s");
	}


	return countdown_time;
}


void do_countdown(CHAR_DATA * ch, char *argument)
{
	struct timeval now_time;
	char buf[MAX_STRING_LENGTH];
	long totsec;

	get_time(&now_time);
	totsec = (time_t) now_time.tv_sec;

	send_to_char(countdown(), ch);
	if(IS_IMMORTAL(ch))
	{
		sprintf(buf, "total seconds = %li\n\r", totsec);
		send_to_char(buf, ch);
	}
	return;

}

void do_time(CHAR_DATA * ch, char *argument)
{
	extern char str_boot_time[];
	char buf[MAX_STRING_LENGTH];
	char *suf;
	long day;

	day = time_info.day + 1;

	if(day > 4 && day < 20)
		suf = "th";
	else if(day % 10 == 1)
		suf = "st";
	else if(day % 10 == 2)
		suf = "nd";
	else if(day % 10 == 3)
		suf = "rd";
	else
		suf = "th";

	sprintf(buf,
		"It is %li o'clock %s, Day of %s, %li%s the Month of %s.\n\rGod Wars started up at %s\rThe system time is %s\r",
		(time_info.hour % 12 == 0) ? 12 : time_info.hour % 12,
		time_info.hour >= 12 ? "pm" : "am",
		day_name[day % 7], day, suf, month_name[time_info.month], str_boot_time, (char *) ctime(&current_time));

	send_to_char(buf, ch);
	send_to_char(uptime(), ch);
	send_to_char("\n\r", ch);
	do_countdown(ch, "");
	return;
}



void do_weather(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];

	static char *const sky_look[4] = {
		"cloudless",
		"cloudy",
		"rainy",
		"lit by flashes of lightning"
	};

	if(!IS_OUTSIDE(ch))
	{
		send_to_char("You can't see the weather indoors.\n\r", ch);
		return;
	}

	sprintf(buf, "The sky is %s and %s.\n\r",
		sky_look[weather_info.sky],
		weather_info.change >= 0 ? "a warm southerly breeze blows" : "a cold northern gust blows");
	send_to_char(buf, ch);
	return;
}



void do_help(CHAR_DATA * ch, char *argument)
{
	char argall[MAX_INPUT_LENGTH];
	char argone[MAX_INPUT_LENGTH];
	HELP_DATA *pHelp;

	if(argument[0] == '\0')
		argument = "summary";

	/*
	 * Tricky argument handling so 'help a b' doesn't match a.
	 */
	argall[0] = '\0';
	while(argument[0] != '\0')
	{
		argument = one_argument(argument, argone);
		if(argall[0] != '\0')
			strcat(argall, " ");
		strcat(argall, argone);
	}

	for(pHelp = help_first; pHelp != 0; pHelp = pHelp->next)
	{
		if(pHelp->level > get_trust(ch))
			continue;

		if(is_name(argall, pHelp->keyword))
		{
			if(pHelp->level >= 0 && str_cmp(argall, "imotd"))
			{
				send_to_char(pHelp->keyword, ch);
				send_to_char("\n\r", ch);
			}

			/*
			 * Strip leading '.' to allow initial blanks.
			 */
			if(pHelp->text[0] == '.')
				send_to_char(pHelp->text + 1, ch);
			else
				send_to_char(pHelp->text, ch);
			return;
		}
	}

	send_to_char("No help on that word.\n\r", ch);
	return;

/* why won't this work borlak?
   works fine for established players... but
   if a newbie types it, it crashes, or if a
   person that just saved types it, it crashes.
   but once a person has used it (after crash)
   they can.


    if ((fp = fopen("needhelp.txt","a")) == 0)
    {
	bug("needhelp.txt update - unable to open file for writing!",0);
	send_to_char("No help on that word.\n\r",ch);
//	fclose(fp);
    	return;
    }
    fprintf(fp,"ADD HELP FILE FOR: %s :END\n\r",argall);
    fclose(fp);
    send_to_char( "Added to ../area/needhelp.txt\n\r",ch);
    send_to_char( "We will add a help for this when we get time, providing this help name is valid.\n\r",ch);
    send_to_char( "No help on that word.\n\r", ch );
    return;
*/

}


long col_str_len(char *txt)
{
	long pos, len;

	len = 0;

	for(pos = 0; txt[pos] != '\0'; pos++)
	{
		if(txt[pos] == '{')
			pos++;
		else
			len++;

		continue;
	}

	return len;
}

void cent_to_char(char *txt, CHAR_DATA * ch)
{
	long len, pos;
	char buf[MAX_STRING_LENGTH];

	len = (80 - col_str_len(txt)) / 2;
	for(pos = 0; pos < len; pos++)
	{
		buf[pos] = ' ';
	}
	buf[pos] = '\0';
	send_to_char(buf, ch);
	send_to_char(txt, ch);
	send_to_char("\n\r", ch);
}

void divide_to_char(CHAR_DATA * ch)
{
	send_to_char("-------------------------------------------------------------------------------\r\n", ch);
}

void divide2_to_char(CHAR_DATA * ch)
{
	send_to_char("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\r\n", ch);
}

void divide3_to_char(CHAR_DATA * ch)
{
	send_to_char("===============================================================================\r\n", ch);
}

void divide4_to_char(CHAR_DATA * ch)
{
	send_to_char("-=[**]=-=[**]=-=[**]=-=[**]=-=[**]=-=[***]=-=[**]=-=[**]=-=[**]=-=[**]=-=[**]=-\r\n", ch);
}

void divide5_to_char(CHAR_DATA * ch)
{
	cent_to_char("-=[***********]=-------------=[***********]=-", ch);
}

void divide6_to_char(CHAR_DATA * ch)
{
	cent_to_char("-    -   -  - - -- ---====*====--- -- - -  -   -    -", ch);
}

void banner_to_char(char *txt, CHAR_DATA * ch)
{
	char buf[MAX_STRING_LENGTH];
	long loop, wdth, ln;

	ln = strlen(txt);
	if(ln > 16)
	{
		sprintf(buf, "-=[**]=-=[**]=-=[**]=-=[                               ]=-=[**]=-=[**]=-=[**]=-");
		wdth = (31 - ln) / 2 + 20;
	}
	else
	{
		sprintf(buf, "-=[**]=-=[**]=-=[**]=-=[**]=-=[                ]=-=[**]=-=[**]=-=[**]=-=[**]=-");
		wdth = (16 - ln) / 2 + 32;
	}
	for(loop = 0; loop < ln; loop++)
		buf[loop + wdth + 22] = txt[loop];
	cent_to_char(buf, ch);
}

void banner2_to_char(char *txt, CHAR_DATA * ch)
{
	char buf[MAX_STRING_LENGTH];
	long loop, wdth, ln;

	ln = strlen(txt);
	if(ln > 16)
	{
		sprintf(buf, "    -   -  - - -- ---===                               ===--- -- - -  -   -\r\n");
		wdth = (31 - ln) / 2 + 24;
	}
	else
	{
		sprintf(buf, "     -    -   -  - - -- ---====                ====--- -- - -  -   -    -\r\n");
		wdth = (16 - ln) / 2 + 32;
	}
	for(loop = 0; loop < ln; loop++)
		buf[loop + wdth + 6] = txt[loop];
	send_to_char(buf, ch);
}


char *ping_rating(CHAR_DATA * ch)
{
	long ping;

	if(!ch || !ch->desc || ch->desc->connected != CON_PLAYING)
		return "{D";

	ping = ch->desc->ping;

	if(ping >= 0 && ping < 100)
		return "{G";
	else if(ping < 200)
		return "{g";
	else if(ping < 300)
		return "{Y";
	else if(ping < 400)
		return "{y";
	else if(ping < 500)
		return "{R";
	else
		return "{r";

	return "{D";
}


void do_rating(CHAR_DATA * ch, char *argument)
{
	DESCRIPTOR_DATA *d;
	char buf[MAX_STRING_LENGTH];
	long num, total, average = 0;

	buf[0] = '\0';

	sprintf(buf, "Rating, PK Factor, Name\n\r");

	for(d = descriptor_list, num = 0, total = 0; d; d = d->next)
	{
		if(!d->character || d->connected != CON_PLAYING || IS_IMMORTAL(d->character))
			continue;

		average = player_rating(d->character);
		sprintf(buf + strlen(buf), "[%-5li] [%-4.0f] %s\n\r",
			average, pkill_factor(d->character), d->character->name);
		total += average;
		num++;
	}

	if(num && total)
		average = total / num;

	sprintf(buf + strlen(buf), "\n\r%li players on with an average rating of %li.\n\r\n\r", num, average);
	send_to_char(buf, ch);
}



void do_ping(CHAR_DATA * ch, char *argument)
{
	DESCRIPTOR_DATA *d;
	char buf[MAX_STRING_LENGTH];
	long num, total, average = 0;

	buf[0] = '\0';

	for(d = descriptor_list, num = 0, total = 0; d; d = d->next)
	{
		if(!d->character || !d->host || d->connected != CON_PLAYING)
			continue;

		sprintf(buf + strlen(buf), "[%s%li{x] %s\n\r", ping_rating(d->character), d->ping, d->character->name);
		if(d->ping != -1)
			total += d->ping;
		num++;
	}

	if(num && total)
		average = total / num;

	sprintf(buf + strlen(buf), "\n\r%li players on with an average ping of %li.\n\r\n\r", num, average);
	send_to_char(buf, ch);
}

/*
 * New 'who' command originally by Alander of Rivers of Mud.
 */
void do_who(CHAR_DATA * ch, char *argument)
{
	long tot;
	char buf[MAX_STRING_LENGTH];
	char bbuf[MSL];
	char arg[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char mort[MAX_STRING_LENGTH];
	char avat[MAX_STRING_LENGTH];
	char a1[MSL];
	char a2[MSL];
	char a3[MSL];
	char a4[MSL];
	char a5[MSL];
	char a6[MSL];
	char a7[MSL];
	char a0[MSL];
	char immo[MAX_STRING_LENGTH];
	DESCRIPTOR_DATA *d;
	long iLevelLower;
	long iLevelUpper;
	long nNumber;
	long mor, ava, imm;
	bool fClassRestrict;
	bool fImmortalOnly;
	bool showall = FALSE;
	long id = 0;
	long ping = 0;

	if(IS_NPC(ch))
		return;

	/*
	 * Set default arguments.
	 */
	iLevelLower = 0;
	iLevelUpper = MAX_LEVEL + 1;
	fClassRestrict = FALSE;
	fImmortalOnly = FALSE;

	/*
	 * Parse arguments.
	 */
	nNumber = 0;

	argument = one_argument(argument, arg);
	if(arg[0] != '\0' && is_number(arg))
	{
		id = atoi(arg);
	}
	else if(arg[0] != '\0' && !str_cmp(arg, "all") && IS_IMMORTAL(ch))
	{
		showall = TRUE;
	}

	/*
	 * Now show matching chars.
	 */
	gCur = 0;
	tot = 0;
	buf[0] = '\0';
	mort[0] = '\0';
	avat[0] = '\0';
	immo[0] = '\0';
	a1[0] = '\0';
	a2[0] = '\0';
	a3[0] = '\0';
	a4[0] = '\0';
	a5[0] = '\0';
	a6[0] = '\0';
	a7[0] = '\0';
	a0[0] = '\0';

	mor = ava = imm = 0;
	for(d = descriptor_list; d != 0; d = d->next)
	{
		CHAR_DATA *wch;
		char const *class;
		char const *race;

		/*
		 * Check for match against restrictions.
		 * Don't use trust as that exposes trusted mortals.
		 */
		if(d->connected == CON_PLAYING)
			tot++;

		if(d->connected != CON_PLAYING || (!can_see(ch, d->character) && (!IS_SET(ch->act, PLR_WATCHER))))
			continue;

		wch = (d->original != 0) ? d->original : d->character;
		if(wch->level < iLevelLower
		   || wch->level > iLevelUpper
		   || (fImmortalOnly && wch->level < LEVEL_IMMORTAL) || (fClassRestrict && wch->level != LEVEL_HERO))
			continue;

		if(id > 0 && get_kingdom(wch->pcdata->kingdom)->id != id)
			continue;
		gCur++;

		/*
		 * Figure out what to print for class.
		 */
		class = " ";
		race = " ";

		switch (wch->pcdata->class)
		{
		default:
			race = "None";
			break;
		case CLASS_VAMPIRE:
			race = "Vampire";
			break;
		case CLASS_WEREWOLF:
			race = "Werewolf";
			break;
		case CLASS_MONK:
			race = "Monk";
			break;
		case CLASS_NINJA:
			race = "Ninja";
			break;
		case CLASS_DEMON:
			race = "Demon";
			break;
		case CLASS_HIGHLANDER:
			race = "Highlander";
			break;
		case CLASS_BARD:
			race = "Bard";
			break;
		case CLASS_DROW:
			race = "Drow";
			break;
		}

		if((IS_HEAD(wch, LOST_HEAD) || IS_EXTRA(wch, EXTRA_OSWITCH)) && wch->pcdata->chobj != 0)
		{
			if(wch->pcdata->chobj->pIndexData->vnum == 12)
				race = " A Head ";
			else if(wch->pcdata->chobj->pIndexData->vnum == 30005)
				race = "A  Brain";
			else
				race = " Object ";
		}


		switch (wch->level)
		{
		default:
			break;
		case MAX_LEVEL - 0:
			class = " Implementor ";
			break;
		case MAX_LEVEL - 1:
			class = "  HighJudge  ";
			break;
		case MAX_LEVEL - 2:
			class = "    Judge    ";
			break;
		case MAX_LEVEL - 3:
			class = "  Enforcer   ";
			break;
		case MAX_LEVEL - 4:
			class = " Quest Maker ";
			break;
		case MAX_LEVEL - 5:
			class = "   Builder   ";
			break;
		case MAX_LEVEL - 6:
		case MAX_LEVEL - 7:
		case MAX_LEVEL - 8:
		case MAX_LEVEL - 9:
			switch (wch->sex)
			{
			case SEX_MALE:
				if(wch->pcdata->status <= 0)
					class = "   Peasant   ";
				else if(wch->pcdata->status <= 10)
					class = "    Knight   ";
				else if(wch->pcdata->status <= 15)
					class = "    Lord     ";
				else if(wch->pcdata->status <= 20)
					class = "    Baron    ";
				else if(wch->pcdata->status <= 30)
					class = "     Sir     ";
				else if(wch->pcdata->status <= 40)
					class = "    Master   ";
				else if(wch->pcdata->status <= 50)
					class = "   Viscount  ";
				else if(wch->pcdata->status <= 60)
					class = "    Count    ";
				else if(wch->pcdata->status <= 70)
					class = "    Duke     ";
				else if(wch->pcdata->status <= 79)
					class = "    Prince   ";
				else if(wch->pcdata->status >= 80)
					class = "    King     ";
				break;
			default:
				if(wch->pcdata->status <= 0)
					class = "   Peasant   ";
				else if(wch->pcdata->status <= 10)
					class = "    Knight   ";
				else if(wch->pcdata->status <= 15)
					class = "    Lady     ";
				else if(wch->pcdata->status <= 20)
					class = "   Baroness  ";
				else if(wch->pcdata->status <= 30)
					class = "     Sir     ";
				else if(wch->pcdata->status <= 40)
					class = "   Mistress  ";
				else if(wch->pcdata->status <= 50)
					class = " Viscountess ";
				else if(wch->pcdata->status <= 60)
					class = "   Countess  ";
				else if(wch->pcdata->status <= 70)
					class = "   Duchess   ";
				else if(wch->pcdata->status <= 79)
					class = "   Princess  ";
				else if(wch->pcdata->status >= 80)
					class = "    Queen    ";
				break;
			}
			break;
		case MAX_LEVEL - 10:
			class = "   Mortal    ";
			break;
		case MAX_LEVEL - 11:
			class = "   Mortal    ";
			break;
		case MAX_LEVEL - 12:
			class = "   Newbie    ";
			break;
		}

		if(!str_cmp(wch->name, "Aria"))
			class = "   Goddess   ";

		if(wch->bounty > 0)
			sprintf(bbuf, "[%li {GBOUNTY{x]", wch->bounty);

		ping = wch->desc->ping;
		if( ping > 900 )
			ping = 900;
		if( ping < 100 )
			ping = 100;

		if(showall)
		{
			if(wch->level > 6)
				continue;

			sprintf(a0 + strlen(a0), "[%s][%s%-4li{x] %-12s  {GH:%-5li  {RD:%-4li  {BR:%s{x\n\r",
				class,
				ping_rating(wch), wch->desc ? wch->desc->ping : -1,
				wch->name, wch->hit, wch->damcap[DAM_CAP], wch->in_room ? wch->in_room->name : "Somewhere");
			ava++;
		}
		else
		{
			if(wch->level > 6)
			{
				sprintf(immo + strlen(immo), "%s%s{x%s{x%s{x\n\r",
					wch->pcdata->krank ? wch->pcdata->krank[0] == '\0' ? "" : wch->pcdata->krank : "",
					wch->pcdata->krank ? wch->pcdata->krank[0] == '\0' ? "" : " " : "",
					wch->name, wch->pcdata->title);
				imm++;
			}
			else if(wch->level > 2 && wch->level < 7)
			{
				if(wch->pcdata->status >= 0)
				{
					sprintf(a0 + strlen(a0),
						"[%s][%s%li{X] %s%s%s%s%s%s{x{W%s{X of the %s Kingdom. [%s%s{x] %s{x\n\r",
						class,
						ping_rating(wch),
						ping/100,
						IS_SET(wch->flag2, EXTRA_AFK) ? "{R<AFK> {x" : "",
						wch->in_room->vnum >= 21500 ? wch->in_room->vnum <=
						21849 ? "({RRedlight{x) " : "" : "", is_affected(wch,
												 gsn_godgift) ?
						"({CBlessed{x) " : "", is_affected(wch,
										   gsn_godcurse) ? "({rCursed{x) " : "",
						wch->pcdata->krank ? wch->pcdata->krank[0] ==
						'\0' ? "" : wch->pcdata->krank : "",
						wch->pcdata->krank ? wch->pcdata->krank[0] == '\0' ? "" : " " : "",
						wch->name, get_kingdom(wch->pcdata->kingdom)->name,
						wch->pcdata->stats[UNI_GEN] == 3 ? "" : wch->pcdata->stats[UNI_GEN] ==
						2 ? "{B" : wch->pcdata->stats[UNI_GEN] == 1 ? "{C" : "", race,
						wch->bounty > 0 ? bbuf : "");
				}
				ava++;
			}
			else
			{
				sprintf(mort + strlen(mort), "[%s] %s%s%s the mortal.\n\r",
					class,
					is_affected(wch, gsn_godgift) ? "({CBlessed{x) " : "",
					is_affected(wch, gsn_godcurse) ? "({RCursed{x) " : "", wch->name);
				mor++;
			}
		}
	}

	stc("------------------------------------------------------------------------------\n\r", ch);
	if(tourn_on())
	{
		if(!tourn_started())
			send_to_char("{WA tournament is about to start, type {Rtournament join{W to join.{x\n\r", ch);
		else
			send_to_char("{WA tournament is in progress.{x\n\r", ch);
	}

	if(imm > 0)
	{
		if(imm > 1)
			sprintf(buf,
				"----------------------------- %2li {GImmortals{x online. ---------------------------\n\r",
				imm);
		else
			sprintf(buf,
				"----------------------------- one {GImmortal{x online. ---------------------------\n\r");
		stc(buf, ch);
		stc(immo, ch);
	}

	if(ava > 0)
	{
		if(ava > 1)
			sprintf(buf,
				"------------------------------- %2li {YAvatars{x online. ---------------------------\n\r",
				ava);
		else
			sprintf(buf,
				"------------------------------- one {YAvatar{x online. ---------------------------\n\r");
		stc(buf, ch);
		stc(a0, ch);
	}

	if(mor > 0)
	{
		if(mor > 1)
			sprintf(buf,
				"------------------------------- %2li {BMortals{x online. ---------------------------\n\r",
				mor);
		else
			sprintf(buf,
				"------------------------------- one {BMortal{x online. ---------------------------\n\r");
		stc(buf, ch);
		stc(mort, ch);
	}

	if(gCur > gCurMax)
		gCurMax = gCur;

	if(tot == 1)
		sprintf(buf2, "You are the only {Wvisible{x player connected!\n\r");
	else
		sprintf(buf2,
			"There are a total of %li {Wvisible{x players connected.  Max since reboot: %li\n\rThere are a total of %li {Winvisible{x players connected.\n\r",
			gCur, gCurMax, tot - gCur);
	send_to_char("------------------------------------------------------------------------------\n\r", ch);
	send_to_char(buf2, ch);
	send_to_char("------------------------------------------------------------------------------\n\r", ch);

	return;
}



void do_inventory(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	OBJ_DATA *portal;
	OBJ_DATA *portal_next;
	ROOM_INDEX_DATA *pRoomIndex;
	ROOM_INDEX_DATA *location;
	bool found;

	if(!IS_NPC(ch) && IS_HEAD(ch, LOST_HEAD))
	{
		send_to_char("You are not a container.\n\r", ch);
		return;
	}
	else if(!IS_NPC(ch) && IS_EXTRA(ch, EXTRA_OSWITCH))
	{
		if(!IS_NPC(ch) && (obj = ch->pcdata->chobj) == 0)
		{
			send_to_char("You are not a container.\n\r", ch);
			return;
		}
		switch (obj->item_type)
		{
		default:
			send_to_char("You are not a container.\n\r", ch);
			break;

		case ITEM_PORTAL:
			pRoomIndex = get_room_index(obj->value[0]);
			location = ch->in_room;
			if(pRoomIndex == 0)
			{
				send_to_char("You don't seem to lead anywhere.\n\r", ch);
				return;
			}
			char_from_room(ch);
			char_to_room(ch, pRoomIndex);

			found = FALSE;
			for(portal = ch->in_room->contents; portal != 0; portal = portal_next)
			{
				portal_next = portal->next_content;
				if((obj->value[0] == portal->value[3]) && (obj->value[3] == portal->value[0]))
				{
					found = TRUE;
					if(IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
					   !IS_SET(portal->extra_flags, ITEM_SHADOWPLANE))
					{
						REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
						do_look(ch, "auto");
						SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
						break;
					}
					else if(!IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
						IS_SET(portal->extra_flags, ITEM_SHADOWPLANE))
					{
						SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
						do_look(ch, "auto");
						REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
						break;
					}
					else
					{
						do_look(ch, "auto");
						break;
					}
				}
			}
			char_from_room(ch);
			char_to_room(ch, location);
			break;

		case ITEM_DRINK_CON:
			if(obj->value[1] <= 0)
			{
				send_to_char("You are empty.\n\r", ch);
				break;
			}
			if(obj->value[1] < obj->value[0] / 5)
				sprintf(buf, "There is a little %s liquid left in you.\n\r",
					liq_table[obj->value[2]].liq_color);
			else if(obj->value[1] < obj->value[0] / 4)
				sprintf(buf, "You contain a small about of %s liquid.\n\r",
					liq_table[obj->value[2]].liq_color);
			else if(obj->value[1] < obj->value[0] / 3)
				sprintf(buf, "You're about a third full of %s liquid.\n\r",
					liq_table[obj->value[2]].liq_color);
			else if(obj->value[1] < obj->value[0] / 2)
				sprintf(buf, "You're about half full of %s liquid.\n\r",
					liq_table[obj->value[2]].liq_color);
			else if(obj->value[1] < obj->value[0])
				sprintf(buf, "You are almost full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
			else if(obj->value[1] == obj->value[0])
				sprintf(buf, "You're completely full of %s liquid.\n\r",
					liq_table[obj->value[2]].liq_color);
			else
				sprintf(buf, "Somehow you are MORE than full of %s liquid.\n\r",
					liq_table[obj->value[2]].liq_color);
			send_to_char(buf, ch);
			break;

		case ITEM_CONTAINER:
		case ITEM_CORPSE_NPC:
		case ITEM_CORPSE_PC:
			act("$p contain:", ch, obj, 0, TO_CHAR);
			show_list_to_char(obj->contains, ch, TRUE, TRUE);
			break;
		}
		return;
	}
	send_to_char("You are carrying:\n\r", ch);
	show_list_to_char(ch->carrying, ch, TRUE, TRUE);
	return;
}



void do_equipment(CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;
	long iWear;
	bool found;

	send_to_char("You are using:\n\r", ch);
	found = FALSE;
	for(iWear = 0; iWear < MAX_WEAR; iWear++)
	{
		if((obj = get_eq_char(ch, iWear)) == 0)
			continue;

		send_to_char(where_name[iWear], ch);
		if(can_see_obj(ch, obj))
		{
			send_to_char(format_obj_to_char(obj, ch, TRUE), ch);
			send_to_char("\n\r", ch);
		}
		else
		{
			send_to_char("something.\n\r", ch);
		}
		found = TRUE;
	}

	if(!found)
		send_to_char("Nothing.\n\r", ch);

	return;
}



void do_compare(CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	OBJ_DATA *obj1;
	OBJ_DATA *obj2;
	long value1;
	long value2;
	char *msg;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);
	if(arg1[0] == '\0')
	{
		send_to_char("Compare what to what?\n\r", ch);
		return;
	}

	if((obj1 = get_obj_carry(ch, arg1)) == 0)
	{
		send_to_char("You do not have that item.\n\r", ch);
		return;
	}

	if(arg2[0] == '\0')
	{
		for(obj2 = ch->carrying; obj2 != 0; obj2 = obj2->next_content)
		{
			if(obj2->wear_loc != WEAR_NONE
			   && can_see_obj(ch, obj2)
			   && obj1->item_type == obj2->item_type && (obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0)
				break;
		}

		if(obj2 == 0)
		{
			send_to_char("You aren't wearing anything comparable.\n\r", ch);
			return;
		}
	}
	else
	{
		if((obj2 = get_obj_carry(ch, arg2)) == 0)
		{
			send_to_char("You do not have that item.\n\r", ch);
			return;
		}
	}

	msg = 0;
	value1 = 0;
	value2 = 0;

	if(obj1 == obj2)
	{
		msg = "You compare $p to itself.  It looks about the same.";
	}
	else if(obj1->item_type != obj2->item_type)
	{
		msg = "You can't compare $p and $P.";
	}
	else
	{
		switch (obj1->item_type)
		{
		default:
			msg = "You can't compare $p and $P.";
			break;

		case ITEM_ARMOR:
			value1 = obj1->value[0];
			value2 = obj2->value[0];
			break;

		case ITEM_WEAPON:
			value1 = obj1->value[1] + obj1->value[2];
			value2 = obj2->value[1] + obj2->value[2];
			break;
		}
	}

	if(msg == 0)
	{
		if(value1 == value2)
			msg = "$p and $P look about the same.";
		else if(value1 > value2)
			msg = "$p looks better than $P.";
		else
			msg = "$p looks worse than $P.";
	}

	act(msg, ch, obj1, obj2, TO_CHAR);
	return;
}



void do_credits(CHAR_DATA * ch, char *argument)
{
	do_help(ch, "diku");
	return;
}



void do_wizlist(CHAR_DATA * ch, char *argument)
{
	do_help(ch, "wizlist");
	return;
}



void do_where(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	DESCRIPTOR_DATA *d;
	bool found;

	one_argument(argument, arg);

	if(arg[0] == '\0')
	{
		sprintf(buf, "Area: %s\n\r", ch->in_room->area->name);
		send_to_char(buf, ch);
		send_to_char("Players near you:\n\r", ch);
		found = FALSE;
		for(d = descriptor_list; d != 0; d = d->next)
		{
			if(d->connected == CON_PLAYING
			   && (victim = d->character) != 0
			   && !IS_NPC(victim)
			   && victim->in_room != 0
			   && victim->in_room->area == ch->in_room->area
			   && victim->pcdata->chobj == 0 && can_see(ch, victim))
			{
				found = TRUE;
				sprintf(buf, "%-28s %s\n\r", victim->name, victim->in_room->name);
				send_to_char(buf, ch);
			}
		}
		if(!found)
			send_to_char("None\n\r", ch);
	}
	else
	{
		found = FALSE;
		for(victim = char_list; victim != 0; victim = victim->next)
		{
			if(victim->in_room != 0
			   && victim->in_room->area == ch->in_room->area
			   && !IS_AFFECTED(victim, AFF_HIDE)
			   && !IS_AFFECTED(victim, AFF_SNEAK) && can_see(ch, victim) && is_name(arg, victim->name))
			{
				found = TRUE;
				sprintf(buf, "Area: %s\n\r", victim->in_room->area->name);
				send_to_char(buf, ch);
				sprintf(buf, "%-28s %s\n\r", PERS(victim, ch), victim->in_room->name);
				send_to_char(buf, ch);
				break;
			}
		}
		if(!found)
			act("You didn't find any $T.", ch, 0, arg, TO_CHAR);
	}

	return;
}




void do_consider(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	char *msg;
	long diff;
	long overall;
	long con_hit;
	long con_dam;
	long con_ac;
	long con_hp;

	one_argument(argument, arg);
	overall = 0;

	if(arg[0] == '\0')
	{
		send_to_char("Consider killing whom?\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They're not here.\n\r", ch);
		return;
	}

	act("You examine $N closely, looking for $S weaknesses.", ch, 0, victim, TO_CHAR);
	act("$n examine $N closely, looking for $S weaknesses.", ch, 0, victim, TO_NOTVICT);
	act("$n examines you closely, looking for your weaknesses.", ch, 0, victim, TO_VICT);

	if(!IS_NPC(victim))
		do_skill(ch, victim->name);

	if(!IS_NPC(victim) && IS_CLASS(victim, CLASS_VAMPIRE) && IS_EXTRA(victim, EXTRA_FAKE_CON))
	{
		con_hit = victim->pcdata->fake_hit;
		con_dam = victim->pcdata->fake_dam;
		con_ac = victim->pcdata->fake_ac;
		con_hp = victim->pcdata->fake_hp;
	}
	else
	{
		con_hit = char_hitroll(victim);
		con_dam = char_damroll(victim);
		con_ac = char_ac(victim);
		con_hp = victim->hit;
	}
	if(con_hp < 1)
		con_hp = 1;

	diff = victim->level - ch->level + con_hit - char_hitroll(ch);
	if(diff <= -35)
	{
		msg = "You are FAR more skilled than $M.";
		overall = overall + 3;
	}
	else if(diff <= -15)
	{
		msg = "$E is not as skilled as you are.";
		overall = overall + 2;
	}
	else if(diff <= -5)
	{
		msg = "$E doesn't seem quite as skilled as you.";
		overall = overall + 1;
	}
	else if(diff <= 5)
	{
		msg = "You are about as skilled as $M.";
	}
	else if(diff <= 15)
	{
		msg = "$E is slightly more skilled than you are.";
		overall = overall - 1;
	}
	else if(diff <= 35)
	{
		msg = "$E seems more skilled than you are.";
		overall = overall - 2;
	}
	else
	{
		msg = "$E is FAR more skilled than you.";
		overall = overall - 3;
	}
	act(msg, ch, 0, victim, TO_CHAR);

	diff = victim->level - ch->level + con_dam - char_damroll(ch);
	if(diff <= -35)
	{
		msg = "You are FAR more powerful than $M.";
		overall = overall + 3;
	}
	else if(diff <= -15)
	{
		msg = "$E is not as powerful as you are.";
		overall = overall + 2;
	}
	else if(diff <= -5)
	{
		msg = "$E doesn't seem quite as powerful as you.";
		overall = overall + 1;
	}
	else if(diff <= 5)
	{
		msg = "You are about as powerful as $M.";
	}
	else if(diff <= 15)
	{
		msg = "$E is slightly more powerful than you are.";
		overall = overall - 1;
	}
	else if(diff <= 35)
	{
		msg = "$E seems more powerful than you are.";
		overall = overall - 2;
	}
	else
	{
		msg = "$E is FAR more powerful than you.";
		overall = overall - 3;
	}
	act(msg, ch, 0, victim, TO_CHAR);

	diff = ch->hit * 100 / con_hp;
	if(diff <= 10)
	{
		msg = "$E is currently FAR healthier than you are.";
		overall = overall - 3;
	}
	else if(diff <= 50)
	{
		msg = "$E is currently much healthier than you are.";
		overall = overall - 2;
	}
	else if(diff <= 75)
	{
		msg = "$E is currently slightly healthier than you are.";
		overall = overall - 1;
	}
	else if(diff <= 125)
	{
		msg = "$E is currently about as healthy as you are.";
	}
	else if(diff <= 200)
	{
		msg = "You are currently slightly healthier than $M.";
		overall = overall + 1;
	}
	else if(diff <= 500)
	{
		msg = "You are currently much healthier than $M.";
		overall = overall + 2;
	}
	else
	{
		msg = "You are currently FAR healthier than $M.";
		overall = overall + 3;
	}
	act(msg, ch, 0, victim, TO_CHAR);

	diff = con_ac - char_ac(ch);
	if(diff <= -100)
	{
		msg = "$E is FAR better armoured than you.";
		overall = overall - 3;
	}
	else if(diff <= -50)
	{
		msg = "$E looks much better armoured than you.";
		overall = overall - 2;
	}
	else if(diff <= -25)
	{
		msg = "$E looks better armoured than you.";
		overall = overall - 1;
	}
	else if(diff <= 25)
	{
		msg = "$E seems about as well armoured as you.";
	}
	else if(diff <= 50)
	{
		msg = "You are better armoured than $M.";
		overall = overall + 1;
	}
	else if(diff <= 100)
	{
		msg = "You are much better armoured than $M.";
		overall = overall + 2;
	}
	else
	{
		msg = "You are FAR better armoured than $M.";
		overall = overall + 3;
	}
	act(msg, ch, 0, victim, TO_CHAR);

	diff = overall;
	if(diff <= -11)
		msg = "Conclusion: $E would kill you in seconds.";
	else if(diff <= -7)
		msg = "Conclusion: You would need a lot of luck to beat $M.";
	else if(diff <= -3)
		msg = "Conclusion: You would need some luck to beat $N.";
	else if(diff <= 2)
		msg = "Conclusion: It would be a very close fight.";
	else if(diff <= 6)
		msg = "Conclusion: You shouldn't have a lot of trouble defeating $M.";
	else if(diff <= 10)
		msg = "Conclusion: $N is no match for you.  You can easily beat $M.";
	else
		msg = "Conclusion: $E wouldn't last more than a few seconds against you.";
	act(msg, ch, 0, victim, TO_CHAR);

	return;
}



void set_title(CHAR_DATA * ch, char *title)
{
	char buf[MAX_STRING_LENGTH];

	if(IS_NPC(ch))
	{
		bug("Set_title: NPC.", 0);
		return;
	}

	if(isalpha(title[0]) || isdigit(title[0]))
	{
		buf[0] = ' ';
		strcpy(buf + 1, title);
	}
	else
	{
		strcpy(buf, title);
	}

	free_string(ch->pcdata->title);
	ch->pcdata->title = str_dup(buf);
	return;
}



void do_title(CHAR_DATA * ch, char *argument)
{
	if(IS_NPC(ch))
		return;

	if(argument[0] == '\0')
	{
		send_to_char("Change your title to what?\n\r", ch);
		return;
	}

	if(!IS_IMMORTAL(ch) && strlen(argument) > 50)
		argument[50] = '\0';

	smash_tilde(argument);
	set_title(ch, argument);
	send_to_char("Ok.\n\r", ch);
}


void do_afk(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];

	if(IS_NPC(ch))
		return;

	if(IS_SET(ch->flag2, EXTRA_AFK))
	{
		free_string(ch->pcdata->title);
		ch->pcdata->title = str_dup(ch->short_descr);
		free_string(ch->short_descr);
		ch->short_descr = 0;
		send_to_char("You are no longer AFK.\n\r", ch);
		sprintf(buf, "{R%s is no longer AFK!{x", ch->name);
		do_info(ch, buf);
		REMOVE_BIT(ch->flag2, EXTRA_AFK);
		return;
	}

	else if(!IS_SET(ch->flag2, EXTRA_AFK))
	{
		free_string(ch->short_descr);
		ch->short_descr = str_dup(ch->pcdata->title);
		free_string(ch->pcdata->title);
		ch->pcdata->title = str_dup("{R..IS AFK{x");
		send_to_char("You are now AFK.\n\r", ch);
		sprintf(buf, "{R%s is now AFK!{x", ch->name);
		do_info(ch, buf);
		SET_BIT(ch->flag2, EXTRA_AFK);
		WAIT_STATE(ch, 25);
		return;
	}

	else
		return;

}



void do_email(CHAR_DATA * ch, char *argument)
{
	if(IS_NPC(ch))
		return;

	if(argument[0] == '\0')
	{
		send_to_char("What do you wish to set your email address to?\n\r", ch);
		return;
	}

	if(strlen(argument) > 70)
	{
		send_to_char("Email listing limited to 70 characters.\n\r", ch);
		return;
	}

	smash_tilde(argument);

	if(!ch->pcdata->email || ch->pcdata->email[0] != '\0')
		free_string(ch->pcdata->email);

	ch->pcdata->email = str_dup(argument);
	send_to_char("Ok.\n\r", ch);
	save_char_obj(ch);
}



void do_description(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];

	if(argument[0] != '\0')
	{
		buf[0] = '\0';
		smash_tilde(argument);
		if(argument[0] == '+')
		{
			if(ch->description != 0)
				strcat(buf, ch->description);
			argument++;
			while(isspace(*argument))
				argument++;
		}

		if(strlen(buf) + strlen(argument) >= MAX_STRING_LENGTH - 2)
		{
			send_to_char("Description too long.\n\r", ch);
			return;
		}

		strcat(buf, argument);
		strcat(buf, "\n\r");
		free_string(ch->description);
		ch->description = str_dup(buf);
	}

	send_to_char("Your description is:\n\r", ch);
	send_to_char(ch->description ? ch->description : "(None).\n\r", ch);
	return;
}



void do_report(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;
	char buf[MAX_STRING_LENGTH];
	char hit_str[MAX_INPUT_LENGTH];
	char mana_str[MAX_INPUT_LENGTH];
	char move_str[MAX_INPUT_LENGTH];
	char mhit_str[MAX_INPUT_LENGTH];
	char mmana_str[MAX_INPUT_LENGTH];
	char mmove_str[MAX_INPUT_LENGTH];
	char exp_str[MAX_INPUT_LENGTH];

	sprintf(hit_str, "%li", ch->hit);
	COL_SCALE(hit_str, ch, ch->hit, ch->max_hit);
	sprintf(mana_str, "%li", ch->mana);
	COL_SCALE(mana_str, ch, ch->mana, ch->max_mana);
	sprintf(move_str, "%li", ch->move);
	COL_SCALE(move_str, ch, ch->move, ch->max_move);
	sprintf(exp_str, "%li", ch->exp);
	COL_SCALE(exp_str, ch, ch->exp, 1000);
	sprintf(mhit_str, "{C%li{x", ch->max_hit);
	sprintf(mmana_str, "{C%li{x", ch->max_mana);
	sprintf(mmove_str, "{C%li{x", ch->max_move);
	sprintf(buf,
		"You report: %s/%s hp %s/%s mana %s/%s mv %s xp.\n\r",
		hit_str, mhit_str, mana_str, mmana_str, move_str, mmove_str, exp_str);

	send_to_char(buf, ch);

	for(vch = char_list; vch != 0; vch = vch_next)
	{
		vch_next = vch->next;
		if(vch == 0)
			continue;
		if(vch == ch)
			continue;
		if(vch->in_room == 0)
			continue;
		if(vch->in_room != ch->in_room)
			continue;
		sprintf(hit_str, "%li", ch->hit);
		COL_SCALE(hit_str, vch, ch->hit, ch->max_hit);
		sprintf(mana_str, "%li", ch->mana);
		COL_SCALE(mana_str, vch, ch->mana, ch->max_mana);
		sprintf(move_str, "%li", ch->move);
		COL_SCALE(move_str, vch, ch->move, ch->max_move);
		sprintf(exp_str, "%li", ch->exp);
		COL_SCALE(exp_str, vch, ch->exp, 1000);
		sprintf(mhit_str, "{C%li{x", ch->max_hit);
		sprintf(mmana_str, "{C%li{x", ch->max_mana);
		sprintf(mmove_str, "{C%li{x", ch->max_move);
		if(!IS_NPC(ch) && IS_AFFECTED(ch, AFF_POLYMORPH))
			sprintf(buf, "%s reports: %s/%s hp %s/%s mana %s/%s mv %s xp.\n\r",
				ch->morph, hit_str, mhit_str, mana_str, mmana_str, move_str, mmove_str, exp_str);
		else
			sprintf(buf, "%s reports: %s/%s hp %s/%s mana %s/%s mv %s xp.\n\r",
				IS_NPC(ch) ? capitalize(ch->short_descr) : ch->name,
				hit_str, mhit_str, mana_str, mmana_str, move_str, mmove_str, exp_str);
		buf[0] = UPPER(buf[0]);
		send_to_char(buf, vch);
	}
	return;
}



void do_practice(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	long sn;

	if(IS_NPC(ch))
		return;

	if(argument[0] == '\0')
	{
		long col;

		col = 0;
		for(sn = 0; sn < MAX_SKILL; sn++)
		{
			if(skill_table[sn].name == 0)
				break;
			if(ch->level < skill_table[sn].skill_level)
				continue;
			sprintf(buf, "%18s %3li%%  ", skill_table[sn].name, ch->pcdata->learned[sn]);
			send_to_char(buf, ch);
			if(++col % 3 == 0)
				send_to_char("\n\r", ch);
		}

		if(col % 3 != 0)
			send_to_char("\n\r", ch);

		sprintf(buf, "You have %li exp left.\n\r", ch->exp);
		send_to_char(buf, ch);
	}
	else
	{
		if(!IS_AWAKE(ch))
		{
			send_to_char("In your dreams, or what?\n\r", ch);
			return;
		}

		if(ch->exp <= 0)
		{
			send_to_char("You have no exp left.\n\r", ch);
			return;
		}

		if((sn = skill_lookup(argument)) < 0 || (!IS_NPC(ch) && ch->level < skill_table[sn].skill_level))
		{
			send_to_char("You can't practice that.\n\r", ch);
			return;
		}


		if(ch->pcdata->learned[sn] >= SKILL_ADEPT)
		{
			sprintf(buf, "You are already an adept of %s.\n\r", skill_table[sn].name);
			send_to_char(buf, ch);
		}
		else if(ch->pcdata->learned[sn] == 0 && ch->exp < 5000)
		{
			sprintf(buf, "You need 5000 exp to learn %s.\n\r", skill_table[sn].name);
			send_to_char(buf, ch);
		}
		else
		{
			ch->exp -= 5000;
			ch->pcdata->learned[sn] = SKILL_ADEPT;
			act("You are now an adept of $T.", ch, 0, skill_table[sn].name, TO_CHAR);
		}
	}
	return;
}



/*
 * 'Wimpy' originally by Dionysos.
 */
void do_wimpy(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	long wimpy;

	one_argument(argument, arg);

	if(arg[0] == '\0')
		wimpy = ch->max_hit / 5;
	else
		wimpy = atoi(arg);

	if(wimpy < 0)
	{
		send_to_char("Your courage exceeds your wisdom.\n\r", ch);
		return;
	}

	if(wimpy > ch->max_hit)
	{
		send_to_char("Such cowardice ill becomes you.\n\r", ch);
		return;
	}

	ch->wimpy = wimpy;
	sprintf(buf, "Wimpy set to %li hit points.\n\r", wimpy);
	send_to_char(buf, ch);
	return;
}



void do_password(CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char *pArg;
	char *pwdnew;
	char *p;
	char cEnd;

	if(IS_NPC(ch))
		return;

	/*
	 * Can't use one_argument here because it smashes case.
	 * So we just steal all its code.  Bleagh.
	 */
	pArg = arg1;
	while(isspace(*argument))
		argument++;

	cEnd = ' ';
	if(*argument == '\'' || *argument == '"')
		cEnd = *argument++;

	while(*argument != '\0')
	{
		if(*argument == cEnd)
		{
			argument++;
			break;
		}
		*pArg++ = *argument++;
	}
	*pArg = '\0';

	pArg = arg2;
	while(isspace(*argument))
		argument++;

	cEnd = ' ';
	if(*argument == '\'' || *argument == '"')
		cEnd = *argument++;

	while(*argument != '\0')
	{
		if(*argument == cEnd)
		{
			argument++;
			break;
		}
		*pArg++ = *argument++;
	}
	*pArg = '\0';

	if(arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char("Syntax: password <old> <new>.\n\r", ch);
		return;
	}

	if(strcmp(arg1, ch->pcdata->pwd) && strcmp(crypt(arg1, ch->pcdata->pwd), ch->pcdata->pwd))
	{
		WAIT_STATE(ch, 40);
		send_to_char("Wrong password.  Wait 10 seconds.\n\r", ch);
		return;
	}

	if(strlen(arg2) < 5)
	{
		send_to_char("New password must be at least five characters long.\n\r", ch);
		return;
	}

	/*
	 * No tilde allowed because of player file format.
	 */
	pwdnew = crypt(arg2, ch->name);
	for(p = pwdnew; *p != '\0'; p++)
	{
		if(*p == '~')
		{
			send_to_char("New password not acceptable, try again.\n\r", ch);
			return;
		}
	}

	free_string(ch->pcdata->pwd);
	ch->pcdata->pwd = str_dup(pwdnew);
	if(!IS_EXTRA(ch, EXTRA_NEWPASS))
		SET_BIT(ch->extra, EXTRA_NEWPASS);
	save_char_obj(ch);
	if(ch->desc != 0 && ch->desc->connected == CON_PLAYING)
		send_to_char("Ok.\n\r", ch);
	return;
}



void do_socials(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	long iSocial;
	long col;

	col = 0;

	for(iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++)
	{
		sprintf(buf, "%-12s", social_table[iSocial].name);
		send_to_char(buf, ch);
		if(++col % 6 == 0)
			send_to_char("\n\r", ch);
	}

	if(col % 6 != 0)
		send_to_char("\n\r", ch);

	return;
}



void do_xsocials(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	long iSocial;
	long col;

	col = 0;

	for(iSocial = 0; xsocial_table[iSocial].name[0] != '\0'; iSocial++)
	{
		sprintf(buf, "%-12s", xsocial_table[iSocial].name);
		send_to_char(buf, ch);
		if(++col % 6 == 0)
			send_to_char("\n\r", ch);
	}

	if(col % 6 != 0)
		send_to_char("\n\r", ch);

	return;
}



void do_spells(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	long sn;
	long col;

	col = 0;
	for(sn = 0; sn < MAX_SKILL && skill_table[sn].name != 0; sn++)
	{
		sprintf(buf, "%-12s", skill_table[sn].name);
		send_to_char(buf, ch);
		if(++col % 6 == 0)
			send_to_char("\n\r", ch);
	}

	if(col % 6 != 0)
		send_to_char("\n\r", ch);
	return;
}



/*
 * Contributed by Alander.
 */
void do_commands(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	long cmd;
	long col;

	col = 0;
	for(cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++)
	{
		if(cmd_table[cmd].level == 0 && cmd_table[cmd].level <= get_trust(ch))
		{
			sprintf(buf, "%-12s", cmd_table[cmd].name);
			send_to_char(buf, ch);
			if(++col % 6 == 0)
				send_to_char("\n\r", ch);
		}
	}

	if(col % 6 != 0)
		send_to_char("\n\r", ch);
	return;
}



void do_channels(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	one_argument(argument, arg);

	if(IS_NPC(ch))
		return;
	if(arg[0] == '\0')
	{
		if(!IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE))
		{
			send_to_char("You are silenced.\n\r", ch);
			return;
		}

		send_to_char("Channels:", ch);

		send_to_char(!IS_SET(ch->deaf, CHANNEL_CHAT) ? " +CHAT" : " -chat", ch);

#if 0
		send_to_char(!IS_SET(ch->deaf, CHANNEL_HACKER) ? " +HACKER" : " -hacker", ch);
#endif

		if(IS_IMMORTAL(ch))
		{
			send_to_char(!IS_SET(ch->deaf, CHANNEL_IMMTALK) ? " +IMMTALK" : " -immtalk", ch);
		}

		send_to_char(!IS_SET(ch->deaf, CHANNEL_MUSIC) ? " +MUSIC" : " -music", ch);

		send_to_char(!IS_SET(ch->deaf, CHANNEL_QUESTION) ? " +QUESTION" : " -question", ch);

		send_to_char(!IS_SET(ch->deaf, CHANNEL_SHOUT) ? " +SHOUT" : " -shout", ch);

		if(IS_IMMORTAL(ch))
		{
			send_to_char(!IS_SET(ch->deaf, CHANNEL_LOG) ? " +LOG" : " -log", ch);
		}

		send_to_char(!IS_SET(ch->deaf, CHANNEL_INFO) ? " +INFO" : " -info", ch);

		send_to_char(!IS_SET(ch->deaf, CHANNEL_TELL) ? " +TELL" : " -tell", ch);

		send_to_char(!IS_SET(ch->deaf, CHANNEL_KTALK) ? " +KTALK" : " -ktalk", ch);

		send_to_char(".\n\r", ch);
	}
	else
	{
		bool fClear;
		long bit;

		if(arg[0] == '+')
			fClear = TRUE;
		else if(arg[0] == '-')
			fClear = FALSE;
		else
		{
			send_to_char("Channels -channel or +channel?\n\r", ch);
			return;
		}

/*	     if ( !str_cmp( arg+1, "auction"  ) ) bit = CHANNEL_AUCTION;*/
		if(!str_cmp(arg + 1, "chat"))
			bit = CHANNEL_CHAT;
#if 0
		else if(!str_cmp(arg + 1, "hacker"))
			bit = CHANNEL_HACKER;
#endif
		else if(!str_cmp(arg + 1, "immtalk"))
			bit = CHANNEL_IMMTALK;
		else if(!str_cmp(arg + 1, "music"))
			bit = CHANNEL_MUSIC;
		else if(!str_cmp(arg + 1, "question"))
			bit = CHANNEL_QUESTION;
		else if(!str_cmp(arg + 1, "shout"))
			bit = CHANNEL_SHOUT;
		else if(!str_cmp(arg + 1, "yell"))
			bit = CHANNEL_YELL;
		else if(IS_IMMORTAL(ch) && !str_cmp(arg + 1, "log"))
			bit = CHANNEL_LOG;
		else if(!str_cmp(arg + 1, "info"))
			bit = CHANNEL_INFO;
		else if(!str_cmp(arg + 1, "tell"))
			bit = CHANNEL_TELL;
		else if(!str_cmp(arg + 1, "ktalk"))
			bit = CHANNEL_KTALK;
		else
		{
			send_to_char("Set or clear which channel?\n\r", ch);
			return;
		}

		if(fClear)
			REMOVE_BIT(ch->deaf, bit);
		else
			SET_BIT(ch->deaf, bit);

		send_to_char("Ok.\n\r", ch);
	}

	return;
}



/*
 * Contributed by Grodyn.
 */
void do_config(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	if(IS_NPC(ch))
		return;

	one_argument(argument, arg);

	if(arg[0] == '\0')
	{
		send_to_char("[ Keyword  ] Option\n\r", ch);

		send_to_char(IS_SET(ch->act, PLR_ANSI)
			     ? "[+ANSI     ] You have ansi colour on.\n\r"
			     : "[-ansi     ] You have ansi colour off.\n\r", ch);

		send_to_char(IS_SET(ch->act, PLR_AUTOEXIT)
			     ? "[+AUTOEXIT ] You automatically see exits.\n\r"
			     : "[-autoexit ] You don't automatically see exits.\n\r", ch);

		send_to_char(IS_SET(ch->act, PLR_AUTOLOOT)
			     ? "[+AUTOLOOT ] You automatically loot corpses.\n\r"
			     : "[-autoloot ] You don't automatically loot corpses.\n\r", ch);

		send_to_char(IS_SET(ch->act, PLR_AUTOSAC)
			     ? "[+AUTOSAC  ] You automatically sacrifice corpses.\n\r"
			     : "[-autosac  ] You don't automatically sacrifice corpses.\n\r", ch);

		send_to_char(IS_SET(ch->act, PLR_BLANK)
			     ? "[+BLANK    ] You have a blank line before your prompt.\n\r"
			     : "[-blank    ] You have no blank line before your prompt.\n\r", ch);

		send_to_char(IS_SET(ch->act, PLR_BRIEF)
			     ? "[+BRIEF    ] You see brief descriptions.\n\r"
			     : "[-brief    ] You see long descriptions.\n\r", ch);

		send_to_char(IS_SET(ch->act, PLR_COMBINE)
			     ? "[+COMBINE  ] You see object lists in combined format.\n\r"
			     : "[-combine  ] You see object lists in single format.\n\r", ch);

		send_to_char(IS_SET(ch->act, PLR_PROMPT)
			     ? "[+PROMPT   ] You have a prompt.\n\r" : "[-prompt   ] You don't have a prompt.\n\r", ch);

		send_to_char(IS_SET(ch->act, PLR_TELNET_GA)
			     ? "[+TELNETGA ] You receive a telnet GA sequence.\n\r"
			     : "[-telnetga ] You don't receive a telnet GA sequence.\n\r", ch);

		send_to_char(IS_SET(ch->act, PLR_SILENCE) ? "[+SILENCE  ] You are silenced.\n\r" : "", ch);

		send_to_char(!IS_SET(ch->act, PLR_NO_EMOTE) ? "" : "[-emote    ] You can't emote.\n\r", ch);

		send_to_char(!IS_SET(ch->act, PLR_NO_TELL) ? "" : "[-tell     ] You can't use 'tell'.\n\r", ch);
	}
	else
	{
		bool fSet;
		long bit;

		if(arg[0] == '+')
			fSet = TRUE;
		else if(arg[0] == '-')
			fSet = FALSE;
		else
		{
			send_to_char("Config -option or +option?\n\r", ch);
			return;
		}

		if(!str_cmp(arg + 1, "ansi"))
			bit = PLR_ANSI;
		else if(!str_cmp(arg + 1, "autoexit"))
			bit = PLR_AUTOEXIT;
		else if(!str_cmp(arg + 1, "autoloot"))
			bit = PLR_AUTOLOOT;
		else if(!str_cmp(arg + 1, "autosac"))
			bit = PLR_AUTOSAC;
		else if(!str_cmp(arg + 1, "blank"))
			bit = PLR_BLANK;
		else if(!str_cmp(arg + 1, "brief"))
			bit = PLR_BRIEF;
		else if(!str_cmp(arg + 1, "combine"))
			bit = PLR_COMBINE;
		else if(!str_cmp(arg + 1, "prompt"))
			bit = PLR_PROMPT;
		else if(!str_cmp(arg + 1, "telnetga"))
			bit = PLR_TELNET_GA;
		else
		{
			send_to_char("Config which option?\n\r", ch);
			return;
		}

		if(fSet)
			SET_BIT(ch->act, bit);
		else
			REMOVE_BIT(ch->act, bit);

		send_to_char("Ok.\n\r", ch);
	}

	return;
}

void do_ansi(CHAR_DATA * ch, char *argument)
{
	if(IS_NPC(ch))
		return;
	if(IS_SET(ch->act, PLR_ANSI))
		do_config(ch, "-ansi");
	else
		do_config(ch, "+ansi");
	return;
}

void do_autoexit(CHAR_DATA * ch, char *argument)
{
	if(IS_NPC(ch))
		return;
	if(IS_SET(ch->act, PLR_AUTOEXIT))
		do_config(ch, "-autoexit");
	else
		do_config(ch, "+autoexit");
	return;
}

void do_autoloot(CHAR_DATA * ch, char *argument)
{
	if(IS_NPC(ch))
		return;
	if(IS_SET(ch->act, PLR_AUTOLOOT))
		do_config(ch, "-autoloot");
	else
		do_config(ch, "+autoloot");
	return;
}

void do_autosac(CHAR_DATA * ch, char *argument)
{
	if(IS_NPC(ch))
		return;
	if(IS_SET(ch->act, PLR_AUTOSAC))
		do_config(ch, "-autosac");
	else
		do_config(ch, "+autosac");
	return;
}

void do_blank(CHAR_DATA * ch, char *argument)
{
	if(IS_NPC(ch))
		return;
	if(IS_SET(ch->act, PLR_BLANK))
		do_config(ch, "-blank");
	else
		do_config(ch, "+blank");
	return;
}

void do_brief(CHAR_DATA * ch, char *argument)
{
	if(IS_NPC(ch))
		return;
	if(IS_SET(ch->act, PLR_BRIEF))
		do_config(ch, "-brief");
	else
		do_config(ch, "+brief");
	return;
}

void do_diagnose(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	long teeth = 0;
	long ribs = 0;
	CHAR_DATA *victim;

	argument = one_argument(argument, arg);

	if(arg == '\0')
	{
		send_to_char("Who do you wish to diagnose?\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("Nobody here by that name.\n\r", ch);
		return;
	}
	act("$n examines $N carefully, diagnosing $S injuries.", ch, 0, victim, TO_NOTVICT);
	act("$n examines you carefully, diagnosing your injuries.", ch, 0, victim, TO_VICT);
	act("Your diagnoses of $N reveals the following...", ch, 0, victim, TO_CHAR);
	send_to_char("--------------------------------------------------------------------------------\n\r", ch);
	if((victim->loc_hp[0] + victim->loc_hp[1] + victim->loc_hp[2] +
	    victim->loc_hp[3] + victim->loc_hp[4] + victim->loc_hp[5] + victim->loc_hp[6]) == 0)
	{
		act("$N has no apparent injuries.", ch, 0, victim, TO_CHAR);
		send_to_char("--------------------------------------------------------------------------------\n\r", ch);
		return;
	}
	/* Check head */
	if(IS_HEAD(victim, LOST_EYE_L) && IS_HEAD(victim, LOST_EYE_R))
		act("$N has lost both of $S eyes.", ch, 0, victim, TO_CHAR);
	else if(IS_HEAD(victim, LOST_EYE_L))
		act("$N has lost $S left eye.", ch, 0, victim, TO_CHAR);
	else if(IS_HEAD(victim, LOST_EYE_R))
		act("$N has lost $S right eye.", ch, 0, victim, TO_CHAR);
	if(IS_HEAD(victim, LOST_EAR_L) && IS_HEAD(victim, LOST_EAR_R))
		act("$N has lost both of $S ears.", ch, 0, victim, TO_CHAR);
	else if(IS_HEAD(victim, LOST_EAR_L))
		act("$N has lost $S left ear.", ch, 0, victim, TO_CHAR);
	else if(IS_HEAD(victim, LOST_EAR_R))
		act("$N has lost $S right ear.", ch, 0, victim, TO_CHAR);
	if(IS_HEAD(victim, LOST_NOSE))
		act("$N has lost $S nose.", ch, 0, victim, TO_CHAR);
	else if(IS_HEAD(victim, BROKEN_NOSE))
		act("$N has got a broken nose.", ch, 0, victim, TO_CHAR);
	if(IS_HEAD(victim, BROKEN_JAW))
		act("$N has got a broken jaw.", ch, 0, victim, TO_CHAR);
	if(IS_HEAD(victim, LOST_HEAD))
	{
		act("$N has had $S head cut off.", ch, 0, victim, TO_CHAR);
		if(IS_BLEEDING(victim, BLEEDING_HEAD))
			act("...Blood is spurting from the stump of $S neck.", ch, 0, victim, TO_CHAR);
	}
	else
	{
		if(IS_BODY(victim, BROKEN_NECK))
			act("$N has got a broken neck.", ch, 0, victim, TO_CHAR);
		if(IS_BODY(victim, CUT_THROAT))
		{
			act("$N has had $S throat cut open.", ch, 0, victim, TO_CHAR);
			if(IS_BLEEDING(victim, BLEEDING_THROAT))
				act("...Blood is pouring from the wound.", ch, 0, victim, TO_CHAR);
		}
	}
	if(IS_HEAD(victim, BROKEN_SKULL))
		act("$N has got a broken skull.", ch, 0, victim, TO_CHAR);
	if(IS_HEAD(victim, LOST_TOOTH_1))
		teeth += 1;
	if(IS_HEAD(victim, LOST_TOOTH_2))
		teeth += 2;
	if(IS_HEAD(victim, LOST_TOOTH_4))
		teeth += 4;
	if(IS_HEAD(victim, LOST_TOOTH_8))
		teeth += 8;
	if(IS_HEAD(victim, LOST_TOOTH_16))
		teeth += 16;
	if(teeth > 0)
	{
		sprintf(buf, "$N has had %li teeth knocked out.", teeth);
		act(buf, ch, 0, victim, TO_CHAR);
	}
	if(IS_HEAD(victim, LOST_TONGUE))
		act("$N has had $S tongue ripped out.", ch, 0, victim, TO_CHAR);
	if(IS_HEAD(victim, LOST_HEAD))
	{
		send_to_char("--------------------------------------------------------------------------------\n\r", ch);
		return;
	}
	/* Check body */
	if(IS_BODY(victim, BROKEN_RIBS_1))
		ribs += 1;
	if(IS_BODY(victim, BROKEN_RIBS_2))
		ribs += 2;
	if(IS_BODY(victim, BROKEN_RIBS_4))
		ribs += 4;
	if(IS_BODY(victim, BROKEN_RIBS_8))
		ribs += 8;
	if(IS_BODY(victim, BROKEN_RIBS_16))
		ribs += 16;
	if(ribs > 0)
	{
		sprintf(buf, "$N has got %li broken ribs.", ribs);
		act(buf, ch, 0, victim, TO_CHAR);
	}
	if(IS_BODY(victim, BROKEN_SPINE))
		act("$N has got a broken spine.", ch, 0, victim, TO_CHAR);
	/* Check arms */
	check_left_arm(ch, victim);
	check_right_arm(ch, victim);
	check_left_leg(ch, victim);
	check_right_leg(ch, victim);
	send_to_char("--------------------------------------------------------------------------------\n\r", ch);
	return;
}

void check_left_arm(CHAR_DATA * ch, CHAR_DATA * victim)
{
	char buf[MAX_STRING_LENGTH];
	char finger[10];
	long fingers = 0;

	if(IS_ARM_L(victim, LOST_ARM) && IS_ARM_R(victim, LOST_ARM))
	{
		act("$N has lost both of $S arms.", ch, 0, victim, TO_CHAR);
		if(IS_BLEEDING(victim, BLEEDING_ARM_L) && IS_BLEEDING(victim, BLEEDING_ARM_R))
			act("...Blood is spurting from both stumps.", ch, 0, victim, TO_CHAR);
		else if(IS_BLEEDING(victim, BLEEDING_ARM_L))
			act("...Blood is spurting from the left stump.", ch, 0, victim, TO_CHAR);
		else if(IS_BLEEDING(victim, BLEEDING_ARM_R))
			act("...Blood is spurting from the right stump.", ch, 0, victim, TO_CHAR);
		return;
	}
	if(IS_ARM_L(victim, LOST_ARM))
	{
		act("$N has lost $S left arm.", ch, 0, victim, TO_CHAR);
		if(IS_BLEEDING(victim, BLEEDING_ARM_L))
			act("...Blood is spurting from the stump.", ch, 0, victim, TO_CHAR);
		return;
	}
	if(IS_ARM_L(victim, BROKEN_ARM) && IS_ARM_R(victim, BROKEN_ARM))
		act("$N arms are both broken.", ch, 0, victim, TO_CHAR);
	else if(IS_ARM_L(victim, BROKEN_ARM))
		act("$N's left arm is broken.", ch, 0, victim, TO_CHAR);
	if(IS_ARM_L(victim, LOST_HAND) && IS_ARM_R(victim, LOST_HAND) && !IS_ARM_R(victim, LOST_ARM))
	{
		act("$N has lost both of $S hands.", ch, 0, victim, TO_CHAR);
		if(IS_BLEEDING(victim, BLEEDING_HAND_L) && IS_BLEEDING(victim, BLEEDING_HAND_R))
			act("...Blood is spurting from both stumps.", ch, 0, victim, TO_CHAR);
		else if(IS_BLEEDING(victim, BLEEDING_HAND_L))
			act("...Blood is spurting from the left stump.", ch, 0, victim, TO_CHAR);
		else if(IS_BLEEDING(victim, BLEEDING_HAND_R))
			act("...Blood is spurting from the right stump.", ch, 0, victim, TO_CHAR);
		return;
	}
	if(IS_ARM_L(victim, LOST_HAND))
	{
		act("$N has lost $S left hand.", ch, 0, victim, TO_CHAR);
		if(IS_BLEEDING(victim, BLEEDING_HAND_L))
			act("...Blood is spurting from the stump.", ch, 0, victim, TO_CHAR);
		return;
	}
	if(IS_ARM_L(victim, LOST_FINGER_I))
		fingers += 1;
	if(IS_ARM_L(victim, LOST_FINGER_M))
		fingers += 1;
	if(IS_ARM_L(victim, LOST_FINGER_R))
		fingers += 1;
	if(IS_ARM_L(victim, LOST_FINGER_L))
		fingers += 1;
	if(fingers == 1)
		sprintf(finger, "finger");
	else
		sprintf(finger, "fingers");
	if(fingers > 0 && IS_ARM_L(victim, LOST_THUMB))
	{
		sprintf(buf, "$N has lost %li %s and $S thumb from $S left hand.", fingers, finger);
		act(buf, ch, 0, victim, TO_CHAR);
	}
	else if(fingers > 0)
	{
		sprintf(buf, "$N has lost %li %s from $S left hand.", fingers, finger);
		act(buf, ch, 0, victim, TO_CHAR);
	}
	else if(IS_ARM_L(victim, LOST_THUMB))
	{
		sprintf(buf, "$N has lost the thumb from $S left hand.");
		act(buf, ch, 0, victim, TO_CHAR);
	}
	fingers = 0;
	if(IS_ARM_L(victim, BROKEN_FINGER_I) && !IS_ARM_L(victim, LOST_FINGER_I))
		fingers += 1;
	if(IS_ARM_L(victim, BROKEN_FINGER_M) && !IS_ARM_L(victim, LOST_FINGER_M))
		fingers += 1;
	if(IS_ARM_L(victim, BROKEN_FINGER_R) && !IS_ARM_L(victim, LOST_FINGER_R))
		fingers += 1;
	if(IS_ARM_L(victim, BROKEN_FINGER_L) && !IS_ARM_L(victim, LOST_FINGER_L))
		fingers += 1;
	if(fingers == 1)
		sprintf(finger, "finger");
	else
		sprintf(finger, "fingers");
	if(fingers > 0 && IS_ARM_L(victim, BROKEN_THUMB) && !IS_ARM_L(victim, LOST_THUMB))
	{
		sprintf(buf, "$N has broken %li %s and $S thumb on $S left hand.", fingers, finger);
		act(buf, ch, 0, victim, TO_CHAR);
	}
	else if(fingers > 0)
	{
		sprintf(buf, "$N has broken %li %s on $S left hand.", fingers, finger);
		act(buf, ch, 0, victim, TO_CHAR);
	}
	else if(IS_ARM_L(victim, BROKEN_THUMB) && !IS_ARM_L(victim, LOST_THUMB))
	{
		sprintf(buf, "$N has broken the thumb on $S left hand.");
		act(buf, ch, 0, victim, TO_CHAR);
	}
	return;
}

void check_right_arm(CHAR_DATA * ch, CHAR_DATA * victim)
{
	char buf[MAX_STRING_LENGTH];
	char finger[10];
	long fingers = 0;

	if(IS_ARM_L(victim, LOST_ARM) && IS_ARM_R(victim, LOST_ARM))
		return;
	if(IS_ARM_R(victim, LOST_ARM))
	{
		act("$N has lost $S right arm.", ch, 0, victim, TO_CHAR);
		if(IS_BLEEDING(victim, BLEEDING_ARM_R))
			act("...Blood is spurting from the stump.", ch, 0, victim, TO_CHAR);
		return;
	}
	if(!IS_ARM_L(victim, BROKEN_ARM) && IS_ARM_R(victim, BROKEN_ARM))
		act("$N's right arm is broken.", ch, 0, victim, TO_CHAR);
	else if(IS_ARM_L(victim, LOST_ARM) && IS_ARM_R(victim, BROKEN_ARM))
		act("$N's right arm is broken.", ch, 0, victim, TO_CHAR);
	if(IS_ARM_L(victim, LOST_HAND) && IS_ARM_R(victim, LOST_HAND))
		return;
	if(IS_ARM_R(victim, LOST_HAND))
	{
		act("$N has lost $S right hand.", ch, 0, victim, TO_CHAR);
		if(IS_BLEEDING(victim, BLEEDING_HAND_R))
			act("...Blood is spurting from the stump.", ch, 0, victim, TO_CHAR);
		return;
	}
	if(IS_ARM_R(victim, LOST_FINGER_I))
		fingers += 1;
	if(IS_ARM_R(victim, LOST_FINGER_M))
		fingers += 1;
	if(IS_ARM_R(victim, LOST_FINGER_R))
		fingers += 1;
	if(IS_ARM_R(victim, LOST_FINGER_L))
		fingers += 1;
	if(fingers == 1)
		sprintf(finger, "finger");
	else
		sprintf(finger, "fingers");
	if(fingers > 0 && IS_ARM_R(victim, LOST_THUMB))
	{
		sprintf(buf, "$N has lost %li %s and $S thumb from $S right hand.", fingers, finger);
		act(buf, ch, 0, victim, TO_CHAR);
	}
	else if(fingers > 0)
	{
		sprintf(buf, "$N has lost %li %s from $S right hand.", fingers, finger);
		act(buf, ch, 0, victim, TO_CHAR);
	}
	else if(IS_ARM_R(victim, LOST_THUMB))
	{
		sprintf(buf, "$N has lost the thumb from $S right hand.");
		act(buf, ch, 0, victim, TO_CHAR);
	}
	fingers = 0;
	if(IS_ARM_R(victim, BROKEN_FINGER_I) && !IS_ARM_R(victim, LOST_FINGER_I))
		fingers += 1;
	if(IS_ARM_R(victim, BROKEN_FINGER_M) && !IS_ARM_R(victim, LOST_FINGER_M))
		fingers += 1;
	if(IS_ARM_R(victim, BROKEN_FINGER_R) && !IS_ARM_R(victim, LOST_FINGER_R))
		fingers += 1;
	if(IS_ARM_R(victim, BROKEN_FINGER_L) && !IS_ARM_R(victim, LOST_FINGER_L))
		fingers += 1;
	if(fingers == 1)
		sprintf(finger, "finger");
	else
		sprintf(finger, "fingers");
	if(fingers > 0 && IS_ARM_R(victim, BROKEN_THUMB) && !IS_ARM_R(victim, LOST_THUMB))
	{
		sprintf(buf, "$N has broken %li %s and $S thumb on $S right hand.", fingers, finger);
		act(buf, ch, 0, victim, TO_CHAR);
	}
	else if(fingers > 0)
	{
		sprintf(buf, "$N has broken %li %s on $S right hand.", fingers, finger);
		act(buf, ch, 0, victim, TO_CHAR);
	}
	else if(IS_ARM_R(victim, BROKEN_THUMB) && !IS_ARM_R(victim, LOST_THUMB))
	{
		sprintf(buf, "$N has broken the thumb on $S right hand.");
		act(buf, ch, 0, victim, TO_CHAR);
	}
	return;
}

void check_left_leg(CHAR_DATA * ch, CHAR_DATA * victim)
{
	char buf[MAX_STRING_LENGTH];
	char toe[10];
	long toes = 0;

	if(IS_LEG_L(victim, LOST_LEG) && IS_LEG_R(victim, LOST_LEG))
	{
		act("$N has lost both of $S legs.", ch, 0, victim, TO_CHAR);
		if(IS_BLEEDING(victim, BLEEDING_LEG_L) && IS_BLEEDING(victim, BLEEDING_LEG_R))
			act("...Blood is spurting from both stumps.", ch, 0, victim, TO_CHAR);
		else if(IS_BLEEDING(victim, BLEEDING_LEG_L))
			act("...Blood is spurting from the left stump.", ch, 0, victim, TO_CHAR);
		else if(IS_BLEEDING(victim, BLEEDING_LEG_R))
			act("...Blood is spurting from the right stump.", ch, 0, victim, TO_CHAR);
		return;
	}
	if(IS_LEG_L(victim, LOST_LEG))
	{
		act("$N has lost $S left leg.", ch, 0, victim, TO_CHAR);
		if(IS_BLEEDING(victim, BLEEDING_LEG_L))
			act("...Blood is spurting from the stump.", ch, 0, victim, TO_CHAR);
		return;
	}
	if(IS_LEG_L(victim, BROKEN_LEG) && IS_LEG_R(victim, BROKEN_LEG))
		act("$N legs are both broken.", ch, 0, victim, TO_CHAR);
	else if(IS_LEG_L(victim, BROKEN_LEG))
		act("$N's left leg is broken.", ch, 0, victim, TO_CHAR);
	if(IS_LEG_L(victim, LOST_FOOT) && IS_LEG_R(victim, LOST_FOOT))
	{
		act("$N has lost both of $S feet.", ch, 0, victim, TO_CHAR);
		if(IS_BLEEDING(victim, BLEEDING_FOOT_L) && IS_BLEEDING(victim, BLEEDING_FOOT_R))
			act("...Blood is spurting from both stumps.", ch, 0, victim, TO_CHAR);
		else if(IS_BLEEDING(victim, BLEEDING_FOOT_L))
			act("...Blood is spurting from the left stump.", ch, 0, victim, TO_CHAR);
		else if(IS_BLEEDING(victim, BLEEDING_FOOT_R))
			act("...Blood is spurting from the right stump.", ch, 0, victim, TO_CHAR);
		return;
	}
	if(IS_LEG_L(victim, LOST_FOOT))
	{
		act("$N has lost $S left foot.", ch, 0, victim, TO_CHAR);
		if(IS_BLEEDING(victim, BLEEDING_FOOT_L))
			act("...Blood is spurting from the stump.", ch, 0, victim, TO_CHAR);
		return;
	}
	if(IS_LEG_L(victim, LOST_TOE_A))
		toes += 1;
	if(IS_LEG_L(victim, LOST_TOE_B))
		toes += 1;
	if(IS_LEG_L(victim, LOST_TOE_C))
		toes += 1;
	if(IS_LEG_L(victim, LOST_TOE_D))
		toes += 1;
	if(toes == 1)
		sprintf(toe, "toe");
	else
		sprintf(toe, "toes");
	if(toes > 0 && IS_LEG_L(victim, LOST_TOE_BIG))
	{
		sprintf(buf, "$N has lost %li %s and $S big toe from $S left foot.", toes, toe);
		act(buf, ch, 0, victim, TO_CHAR);
	}
	else if(toes > 0)
	{
		sprintf(buf, "$N has lost %li %s from $S left foot.", toes, toe);
		act(buf, ch, 0, victim, TO_CHAR);
	}
	else if(IS_LEG_L(victim, LOST_TOE_BIG))
	{
		sprintf(buf, "$N has lost the big toe from $S left foot.");
		act(buf, ch, 0, victim, TO_CHAR);
	}
	toes = 0;
	if(IS_LEG_L(victim, BROKEN_TOE_A) && !IS_LEG_L(victim, LOST_TOE_A))
		toes += 1;
	if(IS_LEG_L(victim, BROKEN_TOE_B) && !IS_LEG_L(victim, LOST_TOE_B))
		toes += 1;
	if(IS_LEG_L(victim, BROKEN_TOE_C) && !IS_LEG_L(victim, LOST_TOE_C))
		toes += 1;
	if(IS_LEG_L(victim, BROKEN_TOE_D) && !IS_LEG_L(victim, LOST_TOE_D))
		toes += 1;
	if(toes == 1)
		sprintf(toe, "toe");
	else
		sprintf(toe, "toes");
	if(toes > 0 && IS_LEG_L(victim, BROKEN_TOE_BIG) && !IS_LEG_L(victim, LOST_TOE_BIG))
	{
		sprintf(buf, "$N has broken %li %s and $S big toe from $S left foot.", toes, toe);
		act(buf, ch, 0, victim, TO_CHAR);
	}
	else if(toes > 0)
	{
		sprintf(buf, "$N has broken %li %s on $S left foot.", toes, toe);
		act(buf, ch, 0, victim, TO_CHAR);
	}
	else if(IS_LEG_L(victim, BROKEN_TOE_BIG) && !IS_LEG_L(victim, LOST_TOE_BIG))
	{
		sprintf(buf, "$N has broken the big toe on $S left foot.");
		act(buf, ch, 0, victim, TO_CHAR);
	}
	return;
}

void check_right_leg(CHAR_DATA * ch, CHAR_DATA * victim)
{
	char buf[MAX_STRING_LENGTH];
	char toe[10];
	long toes = 0;

	if(IS_LEG_L(victim, LOST_LEG) && IS_LEG_R(victim, LOST_LEG))
		return;
	if(IS_LEG_R(victim, LOST_LEG))
	{
		act("$N has lost $S right leg.", ch, 0, victim, TO_CHAR);
		if(IS_BLEEDING(victim, BLEEDING_LEG_R))
			act("...Blood is spurting from the stump.", ch, 0, victim, TO_CHAR);
		return;
	}
	if(!IS_LEG_L(victim, BROKEN_LEG) && IS_LEG_R(victim, BROKEN_LEG))
		act("$N's right leg is broken.", ch, 0, victim, TO_CHAR);
	if(IS_LEG_L(victim, LOST_FOOT) && IS_LEG_R(victim, LOST_FOOT))
		return;
	if(IS_LEG_R(victim, LOST_FOOT))
	{
		act("$N has lost $S right foot.", ch, 0, victim, TO_CHAR);
		if(IS_BLEEDING(victim, BLEEDING_FOOT_R))
			act("...Blood is spurting from the stump.", ch, 0, victim, TO_CHAR);
		return;
	}
	if(IS_LEG_R(victim, LOST_TOE_A))
		toes += 1;
	if(IS_LEG_R(victim, LOST_TOE_B))
		toes += 1;
	if(IS_LEG_R(victim, LOST_TOE_C))
		toes += 1;
	if(IS_LEG_R(victim, LOST_TOE_D))
		toes += 1;
	if(toes == 1)
		sprintf(toe, "toe");
	else
		sprintf(toe, "toes");
	if(toes > 0 && IS_LEG_R(victim, LOST_TOE_BIG))
	{
		sprintf(buf, "$N has lost %li %s and $S big toe from $S right foot.", toes, toe);
		act(buf, ch, 0, victim, TO_CHAR);
	}
	else if(toes > 0)
	{
		sprintf(buf, "$N has lost %li %s from $S right foot.", toes, toe);
		act(buf, ch, 0, victim, TO_CHAR);
	}
	else if(IS_LEG_R(victim, LOST_TOE_BIG))
	{
		sprintf(buf, "$N has lost the big toe from $S right foot.");
		act(buf, ch, 0, victim, TO_CHAR);
	}
	toes = 0;
	if(IS_LEG_R(victim, BROKEN_TOE_A) && !IS_LEG_R(victim, LOST_TOE_A))
		toes += 1;
	if(IS_LEG_R(victim, BROKEN_TOE_B) && !IS_LEG_R(victim, LOST_TOE_B))
		toes += 1;
	if(IS_LEG_R(victim, BROKEN_TOE_C) && !IS_LEG_R(victim, LOST_TOE_C))
		toes += 1;
	if(IS_LEG_R(victim, BROKEN_TOE_D) && !IS_LEG_R(victim, LOST_TOE_D))
		toes += 1;
	if(toes == 1)
		sprintf(toe, "toe");
	else
		sprintf(toe, "toes");
	if(toes > 0 && IS_LEG_R(victim, BROKEN_TOE_BIG) && !IS_LEG_R(victim, LOST_TOE_BIG))
	{
		sprintf(buf, "$N has broken %li %s and $S big toe on $S right foot.", toes, toe);
		act(buf, ch, 0, victim, TO_CHAR);
	}
	else if(toes > 0)
	{
		sprintf(buf, "$N has broken %li %s on $S right foot.", toes, toe);
		act(buf, ch, 0, victim, TO_CHAR);
	}
	else if(IS_LEG_R(victim, BROKEN_TOE_BIG) && !IS_LEG_R(victim, LOST_TOE_BIG))
	{
		sprintf(buf, "$N has broken the big toe on $S right foot.");
		act(buf, ch, 0, victim, TO_CHAR);
	}
	return;
}

void obj_score(CHAR_DATA * ch, OBJ_DATA * obj)
{
	char buf[MAX_STRING_LENGTH];
	AFFECT_DATA *paf;
	long itemtype;

	if(!ch || !obj)
		return;

	sprintf(buf, "You are %s.\n\r", obj->short_descr);
	send_to_char(buf, ch);

	sprintf(buf, "Type %s, Extra flags %s.\n\r", item_type_name(obj), extra_bit_name(obj->extra_flags));
	send_to_char(buf, ch);

	sprintf(buf, "You weigh %li pounds and are worth %li gold coins.\n\r", obj->weight, obj->cost);
	send_to_char(buf, ch);

	if(obj->questmaker != 0 && strlen(obj->questmaker) > 1 && obj->questowner != 0 && strlen(obj->questowner) > 1)
	{
		sprintf(buf, "You were created by %s, and are owned by %s.\n\r", obj->questmaker, obj->questowner);
		send_to_char(buf, ch);
	}
	else if(obj->questmaker != 0 && strlen(obj->questmaker) > 1)
	{
		sprintf(buf, "You were created by %s.\n\r", obj->questmaker);
		send_to_char(buf, ch);
	}
	else if(obj->questowner != 0 && strlen(obj->questowner) > 1)
	{
		sprintf(buf, "You are owned by %s.\n\r", obj->questowner);
		send_to_char(buf, ch);
	}

	switch (obj->item_type)
	{
	case ITEM_SCROLL:
	case ITEM_POTION:
		sprintf(buf, "You contain level %li spells of:", obj->value[0]);
		send_to_char(buf, ch);

		if(obj->value[1] >= 0 && obj->value[1] < MAX_SKILL)
		{
			send_to_char(" '", ch);
			send_to_char(skill_table[obj->value[1]].name, ch);
			send_to_char("'", ch);
		}

		if(obj->value[2] >= 0 && obj->value[2] < MAX_SKILL)
		{
			send_to_char(" '", ch);
			send_to_char(skill_table[obj->value[2]].name, ch);
			send_to_char("'", ch);
		}

		if(obj->value[3] >= 0 && obj->value[3] < MAX_SKILL)
		{
			send_to_char(" '", ch);
			send_to_char(skill_table[obj->value[3]].name, ch);
			send_to_char("'", ch);
		}

		send_to_char(".\n\r", ch);
		break;

	case ITEM_QUEST:
		sprintf(buf, "Your quest point value is %li.\n\r", obj->value[0]);
		send_to_char(buf, ch);
		break;

	case ITEM_WAND:
	case ITEM_STAFF:
		sprintf(buf, "You have %li(%li) charges of level %li", obj->value[1], obj->value[2], obj->value[0]);
		send_to_char(buf, ch);

		if(obj->value[3] >= 0 && obj->value[3] < MAX_SKILL)
		{
			send_to_char(" '", ch);
			send_to_char(skill_table[obj->value[3]].name, ch);
			send_to_char("'", ch);
		}

		send_to_char(".\n\r", ch);
		break;

	case ITEM_WEAPON:
		sprintf(buf, "You inflict %li to %li damage in combat (average %li).\n\r",
			obj->value[1], obj->value[2], (obj->value[1] + obj->value[2]) / 2);
		send_to_char(buf, ch);

		if(obj->value[0] >= 1000)
			itemtype = obj->value[0] - ((obj->value[0] / 1000) * 1000);
		else
			itemtype = obj->value[0];

		if(itemtype > 0)
		{
			if(obj->level < 10)
				sprintf(buf, "You are a minor spell weapon.\n\r");
			else if(obj->level < 20)
				sprintf(buf, "You are a lesser spell weapon.\n\r");
			else if(obj->level < 30)
				sprintf(buf, "You are an average spell weapon.\n\r");
			else if(obj->level < 40)
				sprintf(buf, "You are a greater spell weapon.\n\r");
			else if(obj->level < 50)
				sprintf(buf, "You are a major spell weapon.\n\r");
			else
				sprintf(buf, "You are a supreme spell weapon.\n\r");
			send_to_char(buf, ch);
		}

		if(itemtype == 1)
			sprintf(buf, "You are dripping with corrosive acid.\n\r");
		else if(itemtype == 4)
			sprintf(buf, "You radiate an aura of darkness.\n\r");
		else if(itemtype == 30)
			sprintf(buf, "You are the bane of all evil.\n\r");
		else if(itemtype == 34)
			sprintf(buf, "You drink the souls of your victims.\n\r");
		else if(itemtype == 37)
			sprintf(buf, "You have been tempered in hellfire.\n\r");
		else if(itemtype == 48)
			sprintf(buf, "You crackle with sparks of lightning.\n\r");
		else if(itemtype == 53)
			sprintf(buf, "You are dripping with a dark poison.\n\r");
		else if(itemtype > 0)
			sprintf(buf, "You have been imbued with the power of %s.\n\r", skill_table[itemtype].name);
		if(itemtype > 0)
			send_to_char(buf, ch);

		if(obj->value[0] >= 1000)
			itemtype = obj->value[0] / 1000;
		else
			break;

		if(itemtype == 4 || itemtype == 1)
			sprintf(buf, "You radiate an aura of darkness.\n\r");
		else if(itemtype == 27 || itemtype == 2)
			sprintf(buf, "You allow your wielder to see invisible things.\n\r");
		else if(itemtype == 39 || itemtype == 3)
			sprintf(buf, "You grant your wielder the power of flight.\n\r");
		else if(itemtype == 45 || itemtype == 4)
			sprintf(buf, "You allow your wielder to see in the dark.\n\r");
		else if(itemtype == 46 || itemtype == 5)
			sprintf(buf, "You render your wielder invisible to the human eye.\n\r");
		else if(itemtype == 52 || itemtype == 6)
			sprintf(buf, "You allow your wielder to walk through solid doors.\n\r");
		else if(itemtype == 54 || itemtype == 7)
			sprintf(buf, "You protect your wielder from evil.\n\r");
		else if(itemtype == 57 || itemtype == 8)
			sprintf(buf, "You protect your wielder in combat.\n\r");
		else if(itemtype == 9)
			sprintf(buf, "You allow your wielder to walk in complete silence.\n\r");
		else if(itemtype == 10)
			sprintf(buf, "You surround your wielder with a shield of lightning.\n\r");
		else if(itemtype == 11)
			sprintf(buf, "You surround your wielder with a shield of fire.\n\r");
		else if(itemtype == 12)
			sprintf(buf, "You surround your wielder with a shield of ice.\n\r");
		else if(itemtype == 13)
			sprintf(buf, "You surround your wielder with a shield of acid.\n\r");
		else if(itemtype == 14)
			sprintf(buf, "You protect your wielder from attacks from DarkBlade clan guardians.\n\r");
		else if(itemtype == 15)
			sprintf(buf, "You surround your wielder with a shield of chaos.\n\r");
		else if(itemtype == 16)
			sprintf(buf, "You regenerate the wounds of your wielder.\n\r");
		else if(itemtype == 17)
			sprintf(buf, "You enable your wielder to move at supernatural speed.\n\r");
		else if(itemtype == 18)
			sprintf(buf, "You can slice through armour without difficulty.\n\r");
		else if(itemtype == 19)
			sprintf(buf, "You protect your wielder from player attacks.\n\r");
		else if(itemtype == 20)
			sprintf(buf, "You surround your wielder with a shield of darkness.\n\r");
		else if(itemtype == 21)
			sprintf(buf, "You grant your wielder superior protection.\n\r");
		else if(itemtype == 22)
			sprintf(buf, "You grant your wielder supernatural vision.\n\r");
		else if(itemtype == 23)
			sprintf(buf, "You make your wielder fleet-footed.\n\r");
		else if(itemtype == 24)
			sprintf(buf, "You conceal your wielder from sight.\n\r");
		else if(itemtype == 25)
			sprintf(buf, "You invoke the power of your wielders beast.\n\r");
		else
			sprintf(buf, "You are bugged...please report it.\n\r");
		if(itemtype > 0)
			send_to_char(buf, ch);
		break;

	case ITEM_ARMOR:
		sprintf(buf, "Your armor class is %li.\n\r", obj->value[0]);
		send_to_char(buf, ch);
		if(obj->value[3] < 1)
			break;
		if(obj->value[3] == 4 || obj->value[3] == 1)
			sprintf(buf, "You radiate an aura of darkness.\n\r");
		else if(obj->value[3] == 27 || obj->value[3] == 2)
			sprintf(buf, "You allow your wearer to see invisible things.\n\r");
		else if(obj->value[3] == 39 || obj->value[3] == 3)
			sprintf(buf, "You grant your wearer the power of flight.\n\r");
		else if(obj->value[3] == 45 || obj->value[3] == 4)
			sprintf(buf, "You allow your wearer to see in the dark.\n\r");
		else if(obj->value[3] == 46 || obj->value[3] == 5)
			sprintf(buf, "You render your wearer invisible to the human eye.\n\r");
		else if(obj->value[3] == 52 || obj->value[3] == 6)
			sprintf(buf, "You allow your wearer to walk through solid doors.\n\r");
		else if(obj->value[3] == 54 || obj->value[3] == 7)
			sprintf(buf, "You protect your wearer from evil.\n\r");
		else if(obj->value[3] == 57 || obj->value[3] == 8)
			sprintf(buf, "You protect your wearer in combat.\n\r");
		else if(obj->value[3] == 9)
			sprintf(buf, "You allow your wearer to walk in complete silence.\n\r");
		else if(obj->value[3] == 10)
			sprintf(buf, "You surround your wearer with a shield of lightning.\n\r");
		else if(obj->value[3] == 11)
			sprintf(buf, "You surround your wearer with a shield of fire.\n\r");
		else if(obj->value[3] == 12)
			sprintf(buf, "You surround your wearer with a shield of ice.\n\r");
		else if(obj->value[3] == 13)
			sprintf(buf, "You surround your wearer with a shield of acid.\n\r");
		else if(obj->value[3] == 14)
			sprintf(buf, "You protect your wearer from attacks from DarkBlade clan guardians.\n\r");
		else if(obj->value[3] == 15)
			sprintf(buf, "You surround your wielder with a shield of chaos.\n\r");
		else if(obj->value[3] == 16)
			sprintf(buf, "You regenerate the wounds of your wielder.\n\r");
		else if(obj->value[3] == 17)
			sprintf(buf, "You enable your wearer to move at supernatural speed.\n\r");
		else if(obj->value[3] == 18)
			sprintf(buf, "You can slice through armour without difficulty.\n\r");
		else if(obj->value[3] == 19)
			sprintf(buf, "You protect your wearer from player attacks.\n\r");
		else if(obj->value[3] == 20)
			sprintf(buf, "You surround your wearer with a shield of darkness.\n\r");
		else if(obj->value[3] == 21)
			sprintf(buf, "You grant your wearer superior protection.\n\r");
		else if(obj->value[3] == 22)
			sprintf(buf, "You grant your wearer supernatural vision.\n\r");
		else if(obj->value[3] == 23)
			sprintf(buf, "You make your wearer fleet-footed.\n\r");
		else if(obj->value[3] == 24)
			sprintf(buf, "You conceal your wearer from sight.\n\r");
		else if(obj->value[3] == 25)
			sprintf(buf, "You invoke the power of your wearers beast.\n\r");
		else
			sprintf(buf, "You are bugged...please report it.\n\r");
		if(obj->value[3] > 0)
			send_to_char(buf, ch);
		break;
	}

	for(paf = obj->pIndexData->affected; paf != 0; paf = paf->next)
	{
		if(paf->location != APPLY_NONE && paf->modifier != 0)
		{
			sprintf(buf, "You affect %s by %li.\n\r", affect_loc_name(paf->location), paf->modifier);
			send_to_char(buf, ch);
		}
	}

	for(paf = obj->affected; paf != 0; paf = paf->next)
	{
		if(paf->location != APPLY_NONE && paf->modifier != 0)
		{
			sprintf(buf, "You affect %s by %li.\n\r", affect_loc_name(paf->location), paf->modifier);
			send_to_char(buf, ch);
		}
	}
	return;
}

/* Do_prompt from Morgenes from Aldara Mud */
void do_prompt(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];

	buf[0] = '\0';
	if(IS_NPC(ch))
		return;

	if(argument[0] == '\0')
	{
		do_help(ch, "prompt");
		return;
	}

	if(!strcmp(argument, "on"))
	{
		if(IS_EXTRA(ch, EXTRA_PROMPT))
			send_to_char("But you already have customised prompt on!\n\r", ch);
		else
		{
			send_to_char("Ok.\n\r", ch);
			SET_BIT(ch->extra, EXTRA_PROMPT);
		}
		return;
	}
	else if(!strcmp(argument, "off"))
	{
		if(!IS_EXTRA(ch, EXTRA_PROMPT))
			send_to_char("But you already have customised prompt off!\n\r", ch);
		else
		{
			send_to_char("Ok.\n\r", ch);
			REMOVE_BIT(ch->extra, EXTRA_PROMPT);
		}
		return;
	}
	else if(!strcmp(argument, "clear"))
	{
		free_string(ch->prompt);
		ch->prompt = str_dup("");
		return;
	}
	else
	{
		if(strlen(argument) > 50)
			argument[50] = '\0';
		smash_tilde(argument);
		strcat(buf, argument);
	}

	free_string(ch->prompt);
	ch->prompt = str_dup(buf);
	send_to_char("Ok.\n\r", ch);
	return;
}

/* Do_prompt from Morgenes from Aldara Mud */
void do_cprompt(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];

	buf[0] = '\0';
	if(IS_NPC(ch))
		return;

	if(argument[0] == '\0')
	{
		do_help(ch, "cprompt");
		return;
	}

	if(!strcmp(argument, "clear"))
	{
		free_string(ch->cprompt);
		ch->cprompt = str_dup("");
		return;
	}
	else
	{
		if(strlen(argument) > 50)
			argument[50] = '\0';
		smash_tilde(argument);
		strcat(buf, argument);
	}

	free_string(ch->cprompt);
	ch->cprompt = str_dup(buf);
	send_to_char("Ok.\n\r", ch);
	return;
}

/*put in by kitiyara while working on pk ratio or sumfin
void do_leader(CHAR_DATA *ch, char *argument)
{
	long pk;
	long pd;
	float ratio;



}
*/

bool check_empty_file(FILE * fp)
{
	char k;
	long i = 0;
	long whitespace = 0;

	if(!fp)
		return TRUE;

	do
	{
		k = fgetc(fp);
		if(isspace(k))
			whitespace++;
		i++;
	} while(k != EOF);

	rewind(fp);

	if(i < 2 || i == whitespace)
		return TRUE;

	return FALSE;
}


void do_finger(CHAR_DATA * ch, char *argument)
{
	char strsave[MAX_INPUT_LENGTH];
	char *buf;
	char buf2[MAX_INPUT_LENGTH];
	FILE *fp;
	long num;
	long num2;
	long extra;

	if(IS_NPC(ch))
		return;

	if(!check_parse_name(argument))
	{
		send_to_char("Thats an illegal name.\n\r", ch);
		return;
	}

	if(!char_exists(TRUE, argument))
	{
		send_to_char("That player doesn't exist.\n\r", ch);
		return;
	}

	fclose(fpReserve);
	sprintf(strsave, "%sbackup/%s", PLAYER_DIR, capitalize(argument));
	if((fp = fopen(strsave, "r")) != 0)
	{
		if(check_empty_file(fp))
		{
			send_to_char("That file is bugged, deleting.\n\r", ch);
			fclose(fp);
			fpReserve = fopen(NULL_FILE, "r");
			sprintf(strsave, "rm %sbackup/%s", PLAYER_DIR, capitalize(argument));
			system(strsave);
			return;
		}

		send_to_char("--------------------------------------------------------------------------------\n\r", ch);
		buf = fread_string(fp);
		send_to_char(buf, ch);
		send_to_char(" ", ch);
		buf = fread_string(fp);
		send_to_char(buf, ch);
		send_to_char(".\n\r", ch);
		send_to_char("{x--------------------------------------------------------------------------------\n\r", ch);
		send_to_char("Last connected from ", ch);
		buf = fread_string(fp);
		send_to_char(buf, ch);
		send_to_char(" at ", ch);
		buf = fread_string(fp);
		send_to_char(buf, ch);
		send_to_char(".\n\r", ch);
		send_to_char("--------------------------------------------------------------------------------\n\r", ch);
		extra = fread_number(fp);
		num = fread_number(fp);
		send_to_char("Sex: ", ch);
		buf = fread_string(fp);
		if(num == SEX_MALE)
			send_to_char("Male. ", ch);
		else if(num == SEX_FEMALE)
		{
			send_to_char("Female. ", ch);
			if(IS_SET(extra, EXTRA_PREGNANT))
				other_age(ch, extra, TRUE, buf);
		}
		else
			send_to_char("None. ", ch);
		buf = fread_string(fp);
		other_age(ch, extra, FALSE, buf);
		num = fread_number(fp);
		num2 = fread_number(fp);
		switch (num)
		{
		default:
			send_to_char("They are mortal, ", ch);
			break;
		case LEVEL_AVATAR:
		case LEVEL_APPRENTICE:
		{
			if(num2 <= 10)
				send_to_char("They are an Avatar, ", ch);
			else if(num2 < 15)
				send_to_char("They are an Immortal, ", ch);
			else if(num2 < 20)
				send_to_char("They are a Godling, ", ch);
			else if(num2 < 30)
				send_to_char("They are a Demigod, ", ch);
			else if(num2 < 40)
				send_to_char("They are a Lesser God, ", ch);
			else if(num2 < 50)
				send_to_char("They are a Greater God, ", ch);
			else if(num2 < 60)
				send_to_char("They are a Supreme God, ", ch);
			else
				send_to_char("They are a Ultimate God, ", ch);
			break;
		}
		case LEVEL_BUILDER:
			send_to_char("They are a Builder, ", ch);
			break;
		case LEVEL_QUESTMAKER:
			send_to_char("They are a Quest Maker, ", ch);
			break;
		case LEVEL_ENFORCER:
			send_to_char("They are an Enforcer, ", ch);
			break;
		case LEVEL_JUDGE:
			send_to_char("They are a Judge, ", ch);
			break;
		case LEVEL_HIGHJUDGE:
			send_to_char("They are a High Judge, ", ch);
			break;
		case LEVEL_IMPLEMENTOR:
			send_to_char("They are an Implementor, ", ch);
			break;
		}
		num = fread_number(fp);
		if(num > 0)
			num2 = (2 * (num / 7200));
		else
			num2 = 0;
		sprintf(buf2, "and have been playing for %li hours.\n\r", num2);
		send_to_char(buf2, ch);
		buf = fread_string(fp);
		if(strlen(buf) > 2)
		{
			if(IS_SET(extra, EXTRA_MARRIED))
				sprintf(buf2, "They are married to %s.\n\r", buf);
			else
				sprintf(buf2, "They are engaged to %s.\n\r", buf);
			send_to_char(buf2, ch);
		}
		num = fread_number(fp);
		num2 = fread_number(fp);
		sprintf(buf2, "Player kills: %li, Player Deaths: %li.\n\r", num, num2);
		send_to_char(buf2, ch);
		num = fread_number(fp);
		num2 = fread_number(fp);
		sprintf(buf2, "Mob kills: %li, Mob Deaths: %li.\n\r", num, num2);
		send_to_char(buf2, ch);
/*
	num=fread_number(fp);
	num2=fread_number(fp);
	sprintf(buf2,"Tournament wins: %li, Tournament Losses: %li\n\r",num,num2);
	send_to_char(buf2,ch);
*/
		send_to_char("--------------------------------------------------------------------------------\n\r", ch);
		buf = fread_string(fp);
		if(strlen(buf) > 0)
		{
			sprintf(buf2, "Email: %s\n\r", buf);
			send_to_char(buf2, ch);
			send_to_char("--------------------------------------------------------------------------------\n\r",
				     ch);
		}
	}
	else
	{
		bug("Do_finger: fopen", 0);
	}
	fclose(fp);
	fpReserve = fopen(NULL_FILE, "r");
	return;
}


void do_findvnums(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	long vnum;
	long amount;
	long current;
	static long lasttime;
	long time;
	long start;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if(!arg1 || arg1[0] == '\0' || !is_number(arg1))
	{
		send_to_char("How many available vnums do you need?\n\r", ch);
		return;
	}

	if(arg2 && arg2[0] != '\0' && is_number(arg2))
	{
		start = atoi(arg2);
		if((start >= MAX_VNUM || start < 2000) && !IS_IMMORTAL(ch))
		{
			sprintf(buf, "Range is from 2000 to %d.\n\r", MAX_VNUM);
			send_to_char(buf, ch);
			return;
		}
	}
	else
	{
		start = 2000;
	}

	amount = atoi(arg1);

	if(amount > 500)
	{
		send_to_char("More than 500 rooms?  Talk to borlak\n\r", ch);
		return;
	}

	time = (long) get_uptime();

	if(time - lasttime < 5)
	{
		send_to_char("This command can only be used every five seconds.\n\r", ch);
		return;
	}

	lasttime = time;

	for(vnum = start, current = 0; vnum < MAX_VNUM; vnum++)
	{
		if(get_room_index(vnum) != 0 || get_obj_index(vnum) != 0 || get_mob_index(vnum) != 0)
		{
			current = 0;
			continue;
		}

		if(++current == amount)
		{
			sprintf(buf, "You can use the vnums from %li to %li.\n\r", vnum - amount + 1, vnum);
			send_to_char(buf, ch);
			return;
		}
	}

	send_to_char("No vnums were found!\n\r", ch);
	return;
}

void do_prayer(CHAR_DATA * ch, char *argument)
{
	if(IS_NPC(ch))
		return;
	if(ch->in_room == 0 || ch->in_room->vnum != 3054)

	{
		send_to_char("You can only get a restore at the Temple Altar of Midgaard.\n\r", ch);
		return;
	}
	if(ch->level > 2)

	{
		send_to_char("Only for mortals.\n\r", ch);
		return;
	}
	do_restore(ch, "self");
	send_to_char("God answers you, you are restored.\n\r", ch);
	return;
}
void do_apply(CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg1);
	if(IS_NPC(ch))
		send_to_char("Only Players can be Quest Trusted.\n\r", ch);
	if(arg1[0] == '\0')

	{
		send_to_char("Usage: APPLY QUEST\n\r", ch);
		return;
	}
	if(!str_cmp(arg1, "quest"))

	{
		if(IS_EXTRA(ch, EXTRA_TRUSTED))

		{
			send_to_char("You are already Quest Trusted\n\r", ch);
			return;
		}
		send_to_char("Your application for Questing Trust has been recieved\n\r", ch);
		send_to_char("and is being processed.....\n\r\n\r", ch);
		if((ch->pcdata->score[SCORE_NUM_QUEST] >= 2) && (ch->pcdata->score[SCORE_QUEST] >= 100))

		{
			send_to_char("You have been blessed with Questing Trust.\n\r", ch);
			SET_BIT(ch->extra, EXTRA_TRUSTED);
			return;
		}

		else

		{
			send_to_char("Your Application has been Denied!\n\r", ch);
			send_to_char("You need to complete 2 quests,\n\r", ch);
			send_to_char("Must have 100 qp on hand.\n\r", ch);
			return;
		}
	}
}


// THIS IS THE END OF THE FILE THANKS
