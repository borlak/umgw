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

void do_notravel(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(IS_IMMUNE(ch, IMM_TRAVEL))
	{
		REMOVE_BIT(ch->immune, IMM_TRAVEL);
		send_to_char("You can now be the target of travel or burrow.\n\r", ch);
		return;
	}
	else if(!IS_IMMUNE(ch, IMM_TRAVEL))
	{
		SET_BIT(ch->immune, IMM_TRAVEL);
		send_to_char("You can no longer be the target of travel or burrow.\n\r", ch);
		return;
	}
	else
		send_to_char("Excuse you?\n\r", ch);
	return;
}


/*
void do_warps( CHAR_DATA *ch, char *argument )
{
    char lin   [MAX_STRING_LENGTH];


    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch, CLASS_DEMON))
    {
    send_to_char("Huh?\n\r",ch);
    return;
    }
    sprintf( lin,
"-=(**)=-=(**)=-=(**)=-=(**)=-=(**)=-=(**)=-=(**)=-=(**)=-=(**)=-=(**)=-=(**)=-\n\r");
    send_to_char( lin,ch);
    send_to_char(
"                      -=(**)=- Attained Warps -=(**)=-\n\r",ch);
send_to_char(lin,ch);
if (IS_SET(ch->warp, WARP_CBODY      )) send_to_char("Your body is protected by a indestructable crystal shell.\n\r",ch);
if (IS_SET(ch->warp, WARP_SBODY      )) send_to_char("Your skin is as hard as steel.\n\r",ch);
if (IS_SET(ch->warp, WARP_STRONGARMS )) send_to_char("Your arms are incredibly strong.\n\r",ch);
if (IS_SET(ch->warp, WARP_STRONGLEGS )) send_to_char("Your legs are incredibly strong.\n\r",ch);
if (IS_SET(ch->warp, WARP_VENOMTONG  )) send_to_char("Your tongue is long and venomous.\n\r",ch);
if (IS_SET(ch->warp, WARP_SPIKETAIL  )) send_to_char("Your tail fires deadly spikes during combat.\n\r",ch);
if (IS_SET(ch->warp, WARP_BADBREATH  )) send_to_char("Your breath is putrid and deadly.\n\r",ch);
if (IS_SET(ch->warp, WARP_QUICKNESS  )) send_to_char("You have incredible speed.\n\r",ch);
if (IS_SET(ch->warp, WARP_STAMINA    )) send_to_char("You have increased stamina, reducing the damage you take.\n\r",ch);
if (IS_SET(ch->warp, WARP_HUNT       )) send_to_char("Your heightened senses enable you to hunt people.\n\r",ch);
if (IS_SET(ch->warp, WARP_DEVOUR     )) send_to_char("You have the ability to devour your opponents, sending them into the pits of Hell.\n\r",ch);
if (IS_SET(ch->warp, WARP_TERROR     )) send_to_char("Your features are so horrid that they may stun those who look at you.\n\r",ch);
if (IS_SET(ch->warp, WARP_REGENERATE )) send_to_char("Your body has the ability to regenerate incredibly fast.\n\r",ch);
if (IS_SET(ch->warp, WARP_STEED      )) send_to_char("Your mounts transform into hideous Demons.\n\r",ch);
if (IS_SET(ch->warp, WARP_WEAPON     )) send_to_char("You have the power to transform into a deadly battle axe.\n\r",ch);
if (IS_SET(ch->warp, WARP_INFIRMITY  )) send_to_char("Your body has been afflicted by a terrible infirmity.\n\r",ch);
if (IS_SET(ch->warp, WARP_GBODY      )) send_to_char("Your skin is made of a fragile glass.\n\r",ch);
if (IS_SET(ch->warp, WARP_SCARED     )) send_to_char("You are incredibly scared of combat.\n\r",ch);
if (IS_SET(ch->warp, WARP_MAGMA      )) send_to_char("Your body is composed of deadly magma.\n\r",ch);
if (IS_SET(ch->warp, WARP_WEAK       )) send_to_char("Your muscles are severely weakened.\n\r",ch);
if (IS_SET(ch->warp, WARP_SLOW       )) send_to_char("Your body moves very slowly.\n\r",ch);
if (IS_SET(ch->warp, WARP_VULNER     )) send_to_char("Your skin is very vulnerable to magic.\n\r",ch);
if (IS_SET(ch->warp, WARP_SHARDS     )) send_to_char("Your skin is covered with shards of ice.\n\r",ch);
if (IS_SET(ch->warp, WARP_WINGS      )) send_to_char("A pair of leathery wings protrude from your back.\n\r",ch);
if (IS_SET(ch->warp, WARP_CLUMSY     )) send_to_char("You are incredibly clumsy, enabling you to be disarmed.\n\r",ch);
if (IS_SET(ch->warp, WARP_STUPID     )) send_to_char("Your intelligence is extremely low, preventing you from casting spells.\n\r",ch);
if (IS_SET(ch->warp, WARP_SPOON      )) send_to_char("There is a spoon stuck on your ear.\n\r",ch);
if (IS_SET(ch->warp, WARP_FORK       )) send_to_char("You have a fork stuck in your nose.\n\r",ch);
if (IS_SET(ch->warp, WARP_KNIFE      )) send_to_char("You have a knife hanging out of your ear.\n\r",ch);
if (IS_SET(ch->warp, WARP_SALADBOWL  )) send_to_char("Your head is made out of a salad bowl.\n\r",ch);
if (ch->warp < 1) send_to_char("You haven't obtained any warp powers.\n\r",ch);
send_to_char(lin,ch);
return;
}


void do_redeem( CHAR_DATA *ch, char *argument )
{

    long           warpnum = number_range(1,19);
    long	      newwarp = 0;
    if (IS_NPC(ch)) return;

	if ( !IS_CLASS(ch, CLASS_DEMON))
	{
	send_to_char("Huh?\n\r", ch);
	return;
	}

    if (ch->pcdata->stats[DEMON_TOTAL] < 10000 ||
	ch->pcdata->stats[DEMON_CURRENT] < 10000)
    {
	send_to_char("You need 10000 demon points to redeem
warps!\n\r",ch);
	return;
    }
    {
             if (warpnum == 1) newwarp = WARP_CBODY;
        else if (warpnum == 2) newwarp = WARP_SBODY;
	else if (warpnum == 3) newwarp = WARP_STRONGARMS;
	else if (warpnum == 4) newwarp = WARP_STRONGLEGS;
	else if (warpnum == 5) newwarp = WARP_VENOMTONG;
	else if (warpnum == 6) newwarp = WARP_SPIKETAIL;
	else if (warpnum == 7) newwarp = WARP_BADBREATH;
	else if (warpnum == 8) newwarp = WARP_QUICKNESS;
	else if (warpnum == 9) newwarp = WARP_STAMINA;
	else if (warpnum == 11) newwarp = WARP_HUNT;
	else if (warpnum == 12) newwarp = WARP_DEVOUR;
	else if (warpnum == 13) newwarp = WARP_TERROR;
	else if (warpnum == 14) newwarp = WARP_REGENERATE;
	else if (warpnum == 15) newwarp = WARP_STEED;
	else if (warpnum == 16) newwarp = WARP_WEAPON;
        else if (warpnum == 17) newwarp = WARP_SHARDS;
	else if (warpnum == 18) newwarp = WARP_WINGS;
	else if (warpnum == 19) newwarp = WARP_MAGMA;
    }

	if (ch->warp==12877823)
	newwarp = 0;

	if (IS_SET(ch->warp, newwarp) && ch->warp != 12877823)
    {
      do_redeem(ch,"");
	return;
    }

    {
	if (IS_SET(ch->warp, WARP_INFIRMITY))
	{
	   REMOVE_BIT(ch->warp, WARP_INFIRMITY);
	   SET_BIT(ch->warp, newwarp);
	   ch->pcdata->stats[DEMON_CURRENT] -= 10000;
	   send_to_char("You pray to Satan and a warp is cleansed.\n\r",ch);
    	   save_char_obj(ch);
    	   return;
	}
	else if (IS_SET(ch->warp, WARP_GBODY))
	{
	   REMOVE_BIT(ch->warp, WARP_GBODY);
	   SET_BIT(ch->warp, newwarp);
	   ch->pcdata->stats[DEMON_CURRENT] -= 10000;
	   send_to_char("You pray to Satan and a warp is cleansed.\n\r",ch);
    	   save_char_obj(ch);
    	   return;
	}
	else if (IS_SET(ch->warp, WARP_SCARED))
	{
	   REMOVE_BIT(ch->warp, WARP_SCARED);
	   SET_BIT(ch->warp, newwarp);
	   ch->pcdata->stats[DEMON_CURRENT] -= 10000;
	   send_to_char("You pray to Satan and a warp is cleansed.\n\r",ch);
    	   save_char_obj(ch);
    	   return;
	}
	else if (IS_SET(ch->warp, WARP_WEAK))
	{
	   REMOVE_BIT(ch->warp, WARP_WEAK);
	   SET_BIT(ch->warp, newwarp);
	   ch->pcdata->stats[DEMON_CURRENT] -= 10000;
	   send_to_char("You pray to Satan and a warp is cleansed.\n\r",ch);
    	   save_char_obj(ch);
    	   return;
	}
	else if (IS_SET(ch->warp, WARP_SLOW))
	{
	   REMOVE_BIT(ch->warp, WARP_SLOW);
	   SET_BIT(ch->warp, newwarp);
	   ch->pcdata->stats[DEMON_CURRENT] -= 10000;
	   send_to_char("You pray to Satan and a warp is cleansed.\n\r",ch);
    	   save_char_obj(ch);
    	   return;
	}
	else if (IS_SET(ch->warp, WARP_VULNER))
	{
	   REMOVE_BIT(ch->warp, WARP_VULNER);
	   SET_BIT(ch->warp, newwarp);
	   ch->pcdata->stats[DEMON_CURRENT] -= 10000;
	   send_to_char("You pray to Satan and a warp is cleansed.\n\r",ch);
    	   save_char_obj(ch);
    	   return;
	}
	else if (IS_SET(ch->warp, WARP_CLUMSY))
	{
	   REMOVE_BIT(ch->warp, WARP_CLUMSY);
	   SET_BIT(ch->warp, newwarp);
	   ch->pcdata->stats[DEMON_CURRENT] -= 10000;
	   send_to_char("You pray to Satan and a warp is cleansed.\n\r",ch);
    	   save_char_obj(ch);
    	   return;
	}
	else if (IS_SET(ch->warp, WARP_STUPID))
	{
	   REMOVE_BIT(ch->warp, WARP_STUPID);
	   SET_BIT(ch->warp, newwarp);
	   ch->pcdata->stats[DEMON_CURRENT] -= 10000;
	   send_to_char("You pray to Satan and a warp is cleansed.\n\r",ch);
    	   save_char_obj(ch);
    	   return;
	}
	else if (IS_SET(ch->warp, WARP_SPOON))
	{
	   REMOVE_BIT(ch->warp, WARP_SPOON);
	   SET_BIT(ch->warp, newwarp);
	   ch->pcdata->stats[DEMON_CURRENT] -= 10000;
	   send_to_char("You pray to Satan and a warp is cleansed.\n\r",ch);
    	   save_char_obj(ch);
    	   return;
	}
	else if (IS_SET(ch->warp, WARP_FORK))
	{
	   REMOVE_BIT(ch->warp, WARP_FORK);
	   SET_BIT(ch->warp, newwarp);
	   ch->pcdata->stats[DEMON_CURRENT] -= 10000;
	   send_to_char("You pray to Satan and a warp is cleansed.\n\r",ch);
    	   save_char_obj(ch);
    	   return;
	}
	else if (IS_SET(ch->warp, WARP_KNIFE))
	{
	   REMOVE_BIT(ch->warp, WARP_KNIFE);
	   SET_BIT(ch->warp, newwarp);
	   ch->pcdata->stats[DEMON_CURRENT] -= 10000;
	   send_to_char("You pray to Satan and a warp is cleansed.\n\r",ch);
    	   save_char_obj(ch);
    	   return;
	}
      else send_to_char("You have no evil warps to redeem!\n\r",ch);
    }
  return;
}


void do_obtain( CHAR_DATA *ch, char *argument )
{
    long       newwarp = 0;
    long       warpnum = number_range(1,30);

    if (IS_NPC(ch)) return;

	if ( !IS_CLASS(ch, CLASS_DEMON))
	{
	send_to_char("Huh?\n\r", ch);
	return;
	}

    if (ch->pcdata->stats[DEMON_TOTAL] < 15000 ||
	ch->pcdata->stats[DEMON_CURRENT] < 15000)
    {
	send_to_char("You need 15000 demon points to obtain a new warp!\n\r",ch);
	return;
    }

        if (ch->warpcount >= 18)
        {
	    send_to_char("You have already obtained as many warps as possible.\n\r",ch);
	    return;
	}

             if (warpnum == 1) newwarp = WARP_CBODY;
        else if (warpnum == 2) newwarp = WARP_SBODY;
	else if (warpnum == 3) newwarp = WARP_STRONGARMS;
	else if (warpnum == 4) newwarp = WARP_STRONGLEGS;
	else if (warpnum == 5) newwarp = WARP_VENOMTONG;
	else if (warpnum == 6) newwarp = WARP_SPIKETAIL;
	else if (warpnum == 7) newwarp = WARP_BADBREATH;
	else if (warpnum == 8) newwarp = WARP_QUICKNESS;
	else if (warpnum == 9) newwarp = WARP_STAMINA;
	else if (warpnum == 11) newwarp = WARP_HUNT;
	else if (warpnum == 12) newwarp = WARP_DEVOUR;
	else if (warpnum == 13) newwarp = WARP_TERROR;
	else if (warpnum == 14) newwarp = WARP_REGENERATE;
	else if (warpnum == 15) newwarp = WARP_STEED;
	else if (warpnum == 16) newwarp = WARP_WEAPON;
	else if (warpnum == 17) newwarp = WARP_INFIRMITY;
	else if (warpnum == 18) newwarp = WARP_GBODY;
	else if (warpnum == 19) newwarp = WARP_SCARED;
	else if (warpnum == 20) newwarp = WARP_MAGMA;
	else if (warpnum == 21) newwarp = WARP_WEAK;
	else if (warpnum == 22) newwarp = WARP_SLOW;
	else if (warpnum == 23) newwarp = WARP_VULNER;
	else if (warpnum == 24) newwarp = WARP_SHARDS;
	else if (warpnum == 25) newwarp = WARP_WINGS;
	else if (warpnum == 26) newwarp = WARP_CLUMSY;
	else if (warpnum == 27) newwarp = WARP_STUPID;
	else if (warpnum == 28) newwarp = WARP_SPOON;
	else if (warpnum == 29) newwarp = WARP_FORK;
	else if (warpnum == 30) newwarp = WARP_KNIFE;

    if (IS_SET(ch->warp, newwarp))
    {
      do_obtain(ch,"");
	return;
    }

    SET_BIT(ch->warp, newwarp);
    ch->pcdata->stats[DEMON_CURRENT] -= 15000;
    ch->warpcount += 1;
    send_to_char("You have obtained a new warp!\n\r",ch);
    save_char_obj(ch);
    return;
}

*/
void do_inpart(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
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

	if(!IS_CLASS(ch, CLASS_DEMON))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char("Syntax: Inpart <person> <power>\n\r", ch);
		send_to_char(" Fangs        [ 2500] Claws        [ 2500] Tail        [ 5000]\n\r", ch);
		send_to_char(" Horns        [ 2500] Hooves       [ 1500]\n\r", ch);
		send_to_char(" NightSight   [ 3000] Wings        [ 1000] Might       [ 7500]\n\r", ch);
		send_to_char(" Toughness    [ 7500] Speed        [ 7500] Travel      [ 1500]\n\r", ch);
		send_to_char(" Scry         [ 7500] Truesight    [ 7500] Move        [  500]\n\r", ch);
		send_to_char(" Leap         [  500] Magic        [ 1000] Lifespan    [  100]\n\r", ch);
		send_to_char(" Shield       [20000] Longsword    [    0] Shortsword  [    0]\n\r", ch);
		send_to_char(" Immolate     [ 2500] Inferno      [20000] Caust       [ 3000]\n\r", ch);
		send_to_char(" Unnerve      [ 5000] Freezeweapon [ 3000] Demonform   [25000]\n\r", ch);
		send_to_char(" Leech	      [15000] Blink        [25000] \n\r", ch);
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

	if(!IS_CLASS(victim, CLASS_DEMON) && str_cmp(ch->name, "Pip"))
	{
		send_to_char("Stop cheating people.  Only on demons.\n\r", ch);
		if(victim->pcdata->class == CLASS_MONK)
		{
			sprintf(buf, "%s was a monk trying to be inparted by %s. CHEAT CHEAT CHEAT.\n\r", victim->name,
				ch->name);
			log_string(buf);
			return;
		}
		return;
	}
	if(victim->level != LEVEL_AVATAR || (victim != ch && !IS_SET(victim->special, SPC_CHAMPION)))
	{
		send_to_char("Only on a champion.\n\r", ch);
		return;
	}

/*    if ( victim != ch && str_cmp(victim->lord, ch->name) &&
	str_cmp(victim->lord, ch->lord) && strlen(victim->lord) > 1 )
    {
	send_to_char( "They are not your champion.\n\r", ch );
	return;
    }*/

	if(!str_cmp(arg2, "longsword"))
	{
		send_to_char("You have been granted the power to transform into a demonic longsword!\n\r", victim);
		send_to_char("You grant them the power to transform into a demonic longsword.\n\r", ch);
		victim->pcdata->powers[DPOWER_OBJ_VNUM] = 29662;
		save_char_obj(victim);
		return;
	}
	if(!str_cmp(arg2, "shortsword"))
	{
		send_to_char("You have been granted the power to transform into a demonic shortsword!\n\r", victim);
		send_to_char("You grant them the power to transform into a demonic shortsword.\n\r", ch);
		victim->pcdata->powers[DPOWER_OBJ_VNUM] = 29663;
		save_char_obj(victim);
		return;
	}

	if(!str_cmp(arg2, "fangs"))
	{
		inpart = DEM_FANGS;
		cost = 2500;
	}
	else if(!str_cmp(arg2, "immolate"))
	{
		inpart = DEM_IMMOLATE;
		cost = 2500;
	}
	else if(!str_cmp(arg2, "inferno"))
	{
		inpart = DEM_INFERNO;
		cost = 20000;
	}
	else if(!str_cmp(arg2, "caust"))
	{
		inpart = DEM_CAUST;
		cost = 3000;
	}
	else if(!str_cmp(arg2, "freezeweapon"))
	{
		inpart = DEM_FREEZEWEAPON;
		cost = 3000;
	}
	else if(!str_cmp(arg2, "unnerve"))
	{
		inpart = DEM_UNNERVE;
		cost = 5000;
	}
	else if(!str_cmp(arg2, "claws"))
	{
		inpart = DEM_CLAWS;
		cost = 2500;
	}
	else if(!str_cmp(arg2, "horns"))
	{
		inpart = DEM_HORNS;
		cost = 2500;
	}
	else if(!str_cmp(arg2, "demonform"))
	{
		inpart = DEM_FORM;
		cost = 25000;
	}
	else if(!str_cmp(arg2, "tail"))
	{
		inpart = DEM_TAIL;
		cost = 5000;
	}
	else if(!str_cmp(arg2, "hooves"))
	{
		inpart = DEM_HOOVES;
		cost = 1500;
	}
	else if(!str_cmp(arg2, "shield"))
	{
		inpart = DEM_SHIELD;
		cost = 20000;
	}
	else if(!str_cmp(arg2, "nightsight"))
	{
		inpart = DEM_EYES;
		cost = 3000;
	}
	else if(!str_cmp(arg2, "wings"))
	{
		inpart = DEM_WINGS;
		cost = 1000;
	}
	else if(!str_cmp(arg2, "might"))
	{
		inpart = DEM_MIGHT;
		cost = 7500;
	}
	else if(!str_cmp(arg2, "toughness"))
	{
		inpart = DEM_TOUGH;
		cost = 7500;
	}
	else if(!str_cmp(arg2, "speed"))
	{
		inpart = DEM_SPEED;
		cost = 7500;
	}
	else if(!str_cmp(arg2, "travel"))
	{
		inpart = DEM_TRAVEL;
		cost = 1500;
	}
	else if(!str_cmp(arg2, "scry"))
	{
		inpart = DEM_SCRY;
		cost = 7500;
	}
	else if(!str_cmp(arg2, "move"))
	{
		inpart = DEM_MOVE;
		cost = 500;
	}
	else if(!str_cmp(arg2, "leap"))
	{
		inpart = DEM_LEAP;
		cost = 500;
	}
	else if(!str_cmp(arg2, "magic"))
	{
		inpart = DEM_MAGIC;
		cost = 1000;
	}
	else if(!str_cmp(arg2, "truesight"))
	{
		inpart = DEM_TRUESIGHT;
		cost = 7500;
	}
	else if(!str_cmp(arg2, "leech"))
	{
		inpart = DEM_LEECH;
		cost = 15000;
	}
	else if(!str_cmp(arg2, "blink"))
	{
		inpart = DEM_BLINK;
		cost = 15000;
	}
	else if(!str_cmp(arg2, "lifespan"))
	{
		inpart = DEM_LIFESPAN;
		cost = 100;
	}
	else
	{
		do_inpart(ch, "");
		return;
	}
	if(IS_DEMPOWER(victim, inpart))
	{
		send_to_char("They have already got that power.\n\r", ch);
		return;
	}
	if(ch->pcdata->stats[DEMON_TOTAL] < cost || ch->pcdata->stats[DEMON_CURRENT] < cost)
	{
		send_to_char("You have insufficient power to inpart that gift.\n\r", ch);
		return;
	}
	SET_BIT(victim->pcdata->powers[DPOWER_FLAGS], inpart);
	ch->pcdata->stats[DEMON_CURRENT] -= cost;
	if(victim != ch)
		send_to_char("You have been granted a demonic gift from your patron!\n\r", victim);
	send_to_char("Ok.\n\r", ch);
	if(victim != ch)
		save_char_obj(ch);
	save_char_obj(victim);
	return;
}

void do_demonarmour(CHAR_DATA * ch, char *argument)
{
	OBJ_INDEX_DATA *pObjIndex;
	OBJ_DATA *obj;
	char arg[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	long vnum = 0;

	argument = one_argument(argument, arg);
	argument = one_argument(argument, arg2);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DEMON))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(arg[0] == '\0')
	{
		send_to_char("Command: demonarmor <black/red> <piece>.\n\r", ch);
		stc("For weapons: demona <red> <longsword/shortsword>.\n\r", ch);
		return;
	}

	if(arg2[0] == '\0')
	{
		send_to_char
			("Please specify which piece of demon armor you wish to make: Ring Collar\n\rPlate Helmet Leggings Boots Gauntlets Sleeves Cape Belt Bracer Visor.\n\r",
			 ch);
		return;
	}

	if(!str_cmp(arg2, "ring") && !str_cmp(arg, "black"))
		vnum = 29650;
	else if(!str_cmp(arg2, "collar") && !str_cmp(arg, "black"))
		vnum = 29651;
	else if(!str_cmp(arg2, "plate") && !str_cmp(arg, "black"))
		vnum = 29652;
	else if(!str_cmp(arg2, "helmet") && !str_cmp(arg, "black"))
		vnum = 29653;
	else if(!str_cmp(arg2, "leggings") && !str_cmp(arg, "black"))
		vnum = 29654;
	else if(!str_cmp(arg2, "boots") && !str_cmp(arg, "black"))
		vnum = 29655;
	else if(!str_cmp(arg2, "gauntlets") && !str_cmp(arg, "black"))
		vnum = 29656;
	else if(!str_cmp(arg2, "sleeves") && !str_cmp(arg, "black"))
		vnum = 29657;
	else if(!str_cmp(arg2, "cape") && !str_cmp(arg, "black"))
		vnum = 29658;
	else if(!str_cmp(arg2, "belt") && !str_cmp(arg, "black"))
		vnum = 29659;
	else if(!str_cmp(arg2, "bracer") && !str_cmp(arg, "black"))
		vnum = 29660;
	else if(!str_cmp(arg2, "visor") && !str_cmp(arg, "black"))
		vnum = 29661;
	else if(!str_cmp(arg2, "ring") && !str_cmp(arg, "red"))
		vnum = 27650;
	else if(!str_cmp(arg2, "collar") && !str_cmp(arg, "red"))
		vnum = 27651;
	else if(!str_cmp(arg2, "plate") && !str_cmp(arg, "red"))
		vnum = 27652;
	else if(!str_cmp(arg2, "helmet") && !str_cmp(arg, "red"))
		vnum = 27653;
	else if(!str_cmp(arg2, "leggings") && !str_cmp(arg, "red"))
		vnum = 27654;
	else if(!str_cmp(arg2, "boots") && !str_cmp(arg, "red"))
		vnum = 27655;
	else if(!str_cmp(arg2, "gauntlets") && !str_cmp(arg, "red"))
		vnum = 27656;
	else if(!str_cmp(arg2, "sleeves") && !str_cmp(arg, "red"))
		vnum = 27657;
	else if(!str_cmp(arg2, "cape") && !str_cmp(arg, "red"))
		vnum = 27658;
	else if(!str_cmp(arg2, "belt") && !str_cmp(arg, "red"))
		vnum = 27659;
	else if(!str_cmp(arg2, "bracer") && !str_cmp(arg, "red"))
		vnum = 27660;
	else if(!str_cmp(arg2, "visor") && !str_cmp(arg, "red"))
		vnum = 27661;
	else if(!str_cmp(arg2, "longsword") && !str_cmp(arg, "red"))
		vnum = 29662;
	else if(!str_cmp(arg2, "shortsword") && !str_cmp(arg, "red"))
		vnum = 29663;
	else
	{
		send_to_char
			("Please specify which piece of demon armor you wish to make: Ring Collar\n\rPlate Helmet Leggings Boots Gauntlets Sleeves Cape Belt Bracer Visor.\n\r",
			 ch);
		return;
	}
	if((ch->pcdata->stats[DEMON_TOTAL] < 5000 || ch->pcdata->stats[DEMON_CURRENT] < 5000) && !str_cmp(arg, "black"))
	{
		send_to_char("It costs 5000 points of power to create a piece of black demon armour.\n\r", ch);
		return;
	}
	else if((ch->pcdata->stats[DEMON_TOTAL] < 15000 || ch->pcdata->stats[DEMON_CURRENT] < 15000)
		&& !str_cmp(arg, "red"))
	{
		send_to_char("It costs 15000 points of power to create a piece of red demon armour.\n\r", ch);
		return;
	}

	if(vnum == 0 || (pObjIndex = get_obj_index(vnum)) == 0)
	{
		send_to_char("Missing object, please inform KaVir.\n\r", ch);
		return;
	}
	if(!str_cmp(arg, "black"))
	{
		ch->pcdata->stats[DEMON_TOTAL] -= 5000;
		ch->pcdata->stats[DEMON_CURRENT] -= 5000;
	}
	else if(!str_cmp(arg, "red"))
	{
		ch->pcdata->stats[DEMON_TOTAL] -= 15000;
		ch->pcdata->stats[DEMON_CURRENT] -= 15000;
	}
	obj = create_object(pObjIndex, 50);
	obj_to_char(obj, ch);
	SET_BIT(obj->spectype, SITEM_DEMONIC);
	act("$p appears in your hands in a blast of flames.", ch, obj, 0, TO_CHAR);
	act("$p appears in $n's hands in a blast of flames.", ch, obj, 0, TO_ROOM);
	return;
}

void do_travel(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(IS_CLASS(ch, CLASS_DEMON) || IS_SET(ch->special, SPC_CHAMPION))
	{
		if(!IS_DEMPOWER(ch, DEM_TRAVEL))
		{
			send_to_char("You haven't been granted the gift of travel.\n\r", ch);
			return;
		}
	}
	else
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if((victim = get_char_world(ch, arg)) == 0)
	{
		send_to_char("Nobody by that name.\n\r", ch);
		return;
	}

	if(IS_NPC(victim))
	{
		send_to_char("Not on NPC's.\n\r", ch);
		return;
	}

	if(IS_IMMUNE(victim, IMM_TRAVEL))
	{
		send_to_char("I don't think they want you to do that.\n\r", ch);
		return;
	}

	if(victim == ch)
	{
		send_to_char("Nothing happens.\n\r", ch);
		return;
	}

	if(victim->level != LEVEL_AVATAR || (!IS_SET(victim->special, SPC_CHAMPION) && !IS_CLASS(victim, CLASS_DEMON)))
	{
		send_to_char("Nothing happens.\n\r", ch);
		return;
	}
/*
    if ( IS_CLASS(victim, CLASS_DEMON) && str_cmp(ch->lord, victim->name) )
    {
	send_to_char( "Nothing happens.\n\r", ch );
	return;
    }
    else if ( IS_SET(victim->special, SPC_CHAMPION) &&
	(str_cmp(victim->lord, ch->name) && str_cmp(victim->lord, ch->lord) ))
    {
	send_to_char( "Nothing happens.\n\r", ch );
	return;
    }
*/
	if(victim->in_room == 0)
	{
		send_to_char("Nothing happens.\n\r", ch);
		return;
	}

/*
    if ( victim->position != POS_STANDING )
    {
	send_to_char( "You are unable to focus on their location.\n\r", ch );
	return;
    }
*/
	send_to_char("You sink into the ground.\n\r", ch);
	act("$n sinks into the ground.", ch, 0, 0, TO_ROOM);
	char_from_room(ch);
	char_to_room(ch, victim->in_room);
	do_look(ch, "");
	send_to_char("You rise up out of the ground.\n\r", ch);
	act("$n rises up out of the ground.", ch, 0, 0, TO_ROOM);
	return;
}

void do_horns(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE) && !IS_CLASS(ch, CLASS_DEMON) && !IS_SET(ch->special, SPC_CHAMPION))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(!IS_DEMPOWER(ch, DEM_HORNS) && IS_CLASS(ch, CLASS_DEMON))
	{
		send_to_char("You haven't been granted the gift of horns.\n\r", ch);
		return;
	}
	else if(IS_CLASS(ch, CLASS_VAMPIRE) && !IS_VAMPAFF(ch, VAM_VICISSITUDE))
	{
		send_to_char("You have not mastered the Vicissitude discipline.\n\r", ch);
		return;
	}

	if(IS_DEMAFF(ch, DEM_HORNS))
	{
		send_to_char("Your horns slide back into your head.\n\r", ch);
		act("$n's horns slide back into $s head.", ch, 0, 0, TO_ROOM);
		REMOVE_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_HORNS);
		return;
	}
	send_to_char("Your horns extend out of your head.\n\r", ch);
	act("A pair of pointed horns extend from $n's head.", ch, 0, 0, TO_ROOM);
	SET_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_HORNS);
	return;
}
void do_tail(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DEMON))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(!IS_DEMPOWER(ch, DEM_TAIL))
	{
		send_to_char("You haven't been granted the gift of a tail.\n\r", ch);
		return;
	}

	if(IS_DEMAFF(ch, DEM_TAIL))
	{
		send_to_char("Your tail slides back into your back.\n\r", ch);
		act("$n's tail slides back into $s back.", ch, 0, 0, TO_ROOM);
		REMOVE_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_TAIL);
		return;
	}

	send_to_char("Your tail slides out of your back side.\n\r", ch);
	act("A huge tail extends from $n's back.", ch, 0, 0, TO_ROOM);
	SET_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_TAIL);
	return;
}

/*moved*/
void do_hooves(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DEMON))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(!IS_DEMPOWER(ch, DEM_HOOVES))
	{
		send_to_char("You haven't been granted the gift of hooves.\n\r", ch);
		return;
	}

	if(IS_DEMAFF(ch, DEM_HOOVES))
	{
		send_to_char("Your hooves transform into feet.\n\r", ch);
		act("$n's hooves transform back into $s feet.", ch, 0, 0, TO_ROOM);
		REMOVE_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_HOOVES);
		return;
	}
	send_to_char("Your feet transform into hooves.\n\r", ch);
	act("$n's feet transform into hooves.", ch, 0, 0, TO_ROOM);
	SET_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_HOOVES);
	return;
}

void do_wings(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DEMON) && !IS_SET(ch->special, SPC_CHAMPION))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(!IS_DEMPOWER(ch, DEM_WINGS))
	{
		send_to_char("You haven't been granted the gift of wings.\n\r", ch);
		return;
	}

	if(arg[0] != '\0')
	{
		if(!IS_DEMAFF(ch, DEM_WINGS))
		{
			send_to_char("First you better get your wings out!\n\r", ch);
			return;
		}
		if(!str_cmp(arg, "unfold") || !str_cmp(arg, "u"))
		{
			if(IS_DEMAFF(ch, DEM_UNFOLDED))
			{
				send_to_char("But your wings are already unfolded!\n\r", ch);
				return;
			}
			send_to_char("Your wings unfold from behind your back.\n\r", ch);
			act("$n's wings unfold from behind $s back.", ch, 0, 0, TO_ROOM);
			SET_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_UNFOLDED);
			return;
		}
		else if(!str_cmp(arg, "fold") || !str_cmp(arg, "f"))
		{
			if(!IS_DEMAFF(ch, DEM_UNFOLDED))
			{
				send_to_char("But your wings are already folded!\n\r", ch);
				return;
			}
			send_to_char("Your wings fold up behind your back.\n\r", ch);
			act("$n's wings fold up behind $s back.", ch, 0, 0, TO_ROOM);
			REMOVE_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_UNFOLDED);
			return;
		}
		else
		{
			send_to_char("Do you want to FOLD or UNFOLD your wings?\n\r", ch);
			return;
		}
	}

	if(IS_DEMAFF(ch, DEM_WINGS))
	{
		if(IS_DEMAFF(ch, DEM_UNFOLDED))
		{
			send_to_char("Your wings fold up behind your back.\n\r", ch);
			act("$n's wings fold up behind $s back.", ch, 0, 0, TO_ROOM);
			REMOVE_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_UNFOLDED);
		}
		send_to_char("Your wings slide into your back.\n\r", ch);
		act("$n's wings slide into $s back.", ch, 0, 0, TO_ROOM);
		REMOVE_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_WINGS);
		return;
	}
	send_to_char("Your wings extend from your back.\n\r", ch);
	act("A pair of wings extend from $n's back.", ch, 0, 0, TO_ROOM);
	SET_BIT(ch->pcdata->powers[DPOWER_CURRENT], DEM_WINGS);
	return;
}

void do_lifespan(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DEMON) && !IS_SET(ch->special, SPC_CHAMPION))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(!IS_DEMPOWER(ch, DEM_LIFESPAN))
	{
		send_to_char("You haven't been granted the gift of lifespan.\n\r", ch);
		return;
	}

	if((obj = ch->pcdata->chobj) == 0)
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(obj->chobj == 0 || obj->chobj != ch)
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(!IS_HEAD(ch, LOST_HEAD))
	{
		send_to_char("You cannot change your lifespan in this form.\n\r", ch);
		return;
	}

	if(!str_cmp(arg, "l") || !str_cmp(arg, "long"))
		obj->timer = 0;
	else if(!str_cmp(arg, "s") || !str_cmp(arg, "short"))
		obj->timer = 1;
	else
	{
		send_to_char("Do you wish to have a long or short lifespan?\n\r", ch);
		return;
	}
	send_to_char("Ok.\n\r", ch);

	return;
}

void do_pact(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	bool can_sire = FALSE;

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DEMON) && !IS_SET(ch->special, SPC_CHAMPION))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(IS_CLASS(ch, CLASS_DEMON))
		can_sire = TRUE;

	if(!can_sire)
	{
		send_to_char("You are not able to make a pact.\n\r", ch);
		return;
	}

	if(arg[0] == '\0')
	{
		send_to_char("Make a pact with whom?\n\r", ch);
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
		send_to_char("You cannot make a pact with yourself.\n\r", ch);
		return;
	}

	if(victim->pcdata->class != ch->pcdata->class)
	{
		send_to_char("Only on demons!\n\r", ch);
		return;
	}

	if(victim->level != LEVEL_AVATAR && !IS_IMMORTAL(victim))
	{
		send_to_char("You can only make pacts with avatars.\n\r", ch);
		return;
	}

	if(!IS_IMMUNE(victim, IMM_DEMON))
	{
		send_to_char("You cannot make a pact with an unwilling person.\n\r", ch);
		return;
	}

	if(ch->exp < 666)
	{
		send_to_char("You cannot afford the 666 exp to make a pact.\n\r", ch);
		return;
	}

	if(!IS_EVIL(victim))
	{
		send_to_char("They must be evil!\n\r", ch);
		return;
	}

	ch->exp = ch->exp - 666;
	act("You make $N a demonic champion!", ch, 0, victim, TO_CHAR);
	act("$n makes $N a demonic champion!", ch, 0, victim, TO_NOTVICT);
	act("$n makes you a demonic champion!", ch, 0, victim, TO_VICT);
	SET_BIT(victim->special, SPC_CHAMPION);

	free_string(victim->lord);
	victim->lord = strdup(ch->name);
	free_string(victim->clan);
	victim->clan = strdup(ch->clan);

	if(IS_CLASS(victim, CLASS_VAMPIRE))
		do_mortalvamp(victim, "");
	REMOVE_BIT(victim->act, PLR_HOLYLIGHT);
	REMOVE_BIT(victim->act, PLR_WIZINVIS);
	victim->pcdata->stats[UNI_RAGE] = 0;

	free_string(victim->morph);

	save_char_obj(ch);
	save_char_obj(victim);
	return;
}

void do_offersoul(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_IMMUNE(ch, IMM_DEMON))
	{
/*
	send_to_char("That would be a very bad idea...\n\r",ch);
	return;
*/
		send_to_char("You will now allow demons to buy your soul.\n\r", ch);
		SET_BIT(ch->immune, IMM_DEMON);
		return;

	}
	send_to_char("You will no longer allow demons to buy your soul.\n\r", ch);
	REMOVE_BIT(ch->immune, IMM_DEMON);
	return;
}

void do_weaponform(CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;

	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_DEMON) && !IS_SET(ch->special, SPC_CHAMPION))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	else if(IS_AFFECTED(ch, AFF_POLYMORPH))
	{
		send_to_char("You cannot do this while polymorphed.\n\r", ch);
		return;
	}

/*
    if (IS_SET(ch->warp, WARP_WEAPON))
	ch->pcdata->powers[DPOWER_OBJ_VNUM] = OBJ_VNUM_KHORNE;
*/
	if(ch->pcdata->powers[DPOWER_OBJ_VNUM] < 1)
	{
		send_to_char("You don't have the ability to change into a weapon.\n\r", ch);
		return;
	}
	if((obj = create_object(get_obj_index(ch->pcdata->powers[DPOWER_OBJ_VNUM]), 60)) == 0)
	{
		send_to_char("You don't have the ability to change into a weapon.\n\r", ch);
		return;
	}
	if(IS_AFFECTED(ch, AFF_WEBBED))
	{
		send_to_char("Not with all this sticky webbing on.\n\r", ch);
		return;
	}

	if(ch->max_hit <= 0)	// code added by jedibird@lwgn.com 10-Nov-99
	{
		send_to_char("Not while morted!\n\r", ch);
	}

	if(ch->fight_timer > 0)
	{
		send_to_char("Not until your fight timer expires.\n\r", ch);
		return;
	}



	obj_to_room(obj, ch->in_room);
	act("$n transforms into $p and falls to the ground.", ch, obj, 0, TO_ROOM);
	act("You transform into $p and fall to the ground.", ch, obj, 0, TO_CHAR);
	ch->pcdata->obj_vnum = ch->pcdata->powers[DPOWER_OBJ_VNUM];
	obj->chobj = ch;
	ch->pcdata->chobj = obj;
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	SET_BIT(ch->extra, EXTRA_OSWITCH);
	free_string(ch->morph);
	ch->morph = str_dup(obj->short_descr);
	return;
}

void do_humanform(CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;

	if(IS_NPC(ch))
		return;
	if((obj = ch->pcdata->chobj) == 0)
	{
		send_to_char("You are already in human form.\n\r", ch);
		return;
	}

	ch->pcdata->obj_vnum = 0;
	obj->chobj = 0;
	ch->pcdata->chobj = 0;
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	REMOVE_BIT(ch->extra, EXTRA_OSWITCH);
	free_string(ch->morph);
	ch->morph = str_dup("");
	act("$p transforms into $n.", ch, obj, 0, TO_ROOM);
	act("Your reform your human body.", ch, obj, 0, TO_CHAR);
	extract_obj(obj);
	if(ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
	{
		char_from_room(ch);
		char_to_room(ch, get_room_index(ROOM_VNUM_HELL));
	}
	return;
}

void do_champions(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	char lord[MAX_INPUT_LENGTH];
	CHAR_DATA *gch;

	one_argument(argument, arg);

	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_DEMON) && !IS_SET(ch->special, SPC_CHAMPION))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(strlen(ch->lord) < 2 && !IS_CLASS(ch, CLASS_DEMON))
	{
		send_to_char("But you don't follow any demon!\n\r", ch);
		return;
	}

	if(IS_CLASS(ch, CLASS_DEMON))
		strcpy(lord, ch->name);
	else
		strcpy(lord, ch->lord);
	sprintf(buf, "The champions of %s:\n\r", lord);
	send_to_char(buf, ch);
	send_to_char("[      Name      ] [ Hits ] [ Mana ] [ Move ] [  Exp  ] [       Power]\n\r", ch);
	for(gch = char_list; gch != 0; gch = gch->next)
	{
		if(IS_NPC(gch))
			continue;
		if(!IS_CLASS(gch, CLASS_DEMON) && !IS_SET(gch->special, SPC_CHAMPION))
			continue;
		if(!str_cmp(ch->lord, lord) || !str_cmp(ch->name, lord))
		{
			sprintf(buf,
				"[%-16s] [%-6li] [%-6li] [%-6li] [%7li] [ %-9li%9li ]\n\r",
				capitalize(gch->name),
				gch->hit, gch->mana, gch->move,
				gch->exp, gch->pcdata->stats[DEMON_CURRENT], gch->pcdata->stats[DEMON_TOTAL]);
			send_to_char(buf, ch);
		}
	}
	return;
}


void do_eyespy(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	CHAR_DATA *familiar;

	if(IS_HEAD(ch, LOST_EYE_L) && IS_HEAD(ch, LOST_EYE_R))
	{
		send_to_char("But you don't have any more eyes to pluck out!\n\r", ch);
		return;
	}
	if(!IS_HEAD(ch, LOST_EYE_L) && number_range(1, 2) == 1)
	{
		act("You pluck out your left eyeball and throw it to the ground.", ch, 0, 0, TO_CHAR);
		act("$n plucks out $s left eyeball and throws it to the ground.", ch, 0, 0, TO_ROOM);
	}
	else if(!IS_HEAD(ch, LOST_EYE_R))
	{
		act("You pluck out your right eyeball and throw it to the ground.", ch, 0, 0, TO_CHAR);
		act("$n plucks out $s right eyeball and throws it to the ground.", ch, 0, 0, TO_ROOM);
	}
	else
	{
		act("You pluck out your left eyeball and throw it to the ground.", ch, 0, 0, TO_CHAR);
		act("$n plucks out $s left eyeball and throws it to the ground.", ch, 0, 0, TO_ROOM);
	}
	if((familiar = ch->pcdata->familiar) != 0)
	{
		make_part(ch, "eyeball");
		return;
	}

	victim = create_mobile(get_mob_index(MOB_VNUM_EYE));
	if(victim == 0)
	{
		send_to_char("Error - please inform KaVir.\n\r", ch);
		return;
	}

	char_to_room(victim, ch->in_room);

	ch->pcdata->familiar = victim;
	victim->wizard = ch;
	return;
}


/*
void do_gifts( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    long improve;
    long cost;
    long max;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (IS_NPC(ch)) return;

    if (!IS_CLASS(ch, CLASS_DEMON))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (arg1[0] == '\0' && arg2[0] == '\0')
    {
	sprintf(buf,"Gifts: Strength of Satan, Speed of Satan, Stake, Cone of fire, True Form\n\r");
	send_to_char(buf,ch);
	return;
    }
    if (arg2[0] == '\0')
    {
	if (!str_cmp(arg1,"Strength"))
	{
	    send_to_char("Strength of Satan: You are as strong as your creator.\n\r",ch);
	    return;
	}
	else if (!str_cmp(arg1,"Speed"))
	{
	    send_to_char("Speed: The Speed of satan you dodge most attacks wth incredible speed.\n\r",ch);
	    return;
	}
	else if (!str_cmp(arg1,"Stake"))
	{
	    send_to_char("Stake: You have the ability to make a silver stake.\n\r",ch);
	    return;
	}
	else if (!str_cmp(arg1,"Cone"))
	{
	    send_to_char("Cone: You have the power to breath a cone fo fire.\n\r",ch);
	    return;
	}
	else if (!str_cmp(arg1,"Form"))
	{
	    send_to_char("Form: The power to go into true demon form.\n\r",ch);
	    return;
	}

	sprintf(buf,"Gifts: Strength of Satan, Speed of Satan, Stake, Cone of fire, True Form\n\r");
        send_to_char(buf,ch);
	return;
    }
    if (!str_cmp(arg2,"improve"))
    {
	     if (!str_cmp(arg1,"strength"   )) {improve = DEM_STRENGTH;max=1;}
	else if (!str_cmp(arg1,"speed"   )) {improve = DEM_SPEEDY;max=1;}
	else if (!str_cmp(arg1,"stake"   )) {improve = DEM_STAKE;max=1;}
	else if (!str_cmp(arg1,"cone"    )) {improve = DEM_CONE;max=1;}
	else if (!str_cmp(arg1,"form" )) {improve = DEM_FORM;max=1;}
	else
	{
	    send_to_char("You can improve: Strength Speed Stake Cone Form\n\r",ch);
	    return;
	}
	cost = (ch->pcdata->powers[improve]+1) * 50;
	arg1[0] = UPPER(arg1[0]);
	if ( ch->pcdata->powers[improve] >= max )
	{
	    sprintf(buf,"You have already gained this gift.\n\r");
	    send_to_char(buf,ch);
	    return;
	}
	if ( cost > ch->practice )
	{
	    sprintf(buf,"It costs you %li primal to improve your gifts.\n\r", cost);
	    send_to_char(buf,ch);
	    return;
	}
       ch->pcdata->powers[improve] += 1;
       ch->practice -= cost;
       sprintf(buf,"You improve your gifts.\n\r");
       send_to_char(buf,ch);
      }
}
*/

void do_cone(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	long sn;
	long level;
	long spelltype;

	argument = one_argument(argument, arg);
	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DEMON))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(arg[0] == '\0')
	{
		send_to_char("Cone who?\n\r", ch);
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

	if((sn = skill_lookup("cone")) < 0)
		return;
	spelltype = skill_table[sn].target;
	level = ch->spl[spelltype] * 1;
	level = level * 1;
	act("You Blast $N with a cone of fire.", ch, 0, victim, TO_CHAR);
	act("$n Blasts you with a cone of fire.", ch, 0, victim, TO_VICT);
	(*skill_table[sn].spell_fun) (sn, level, ch, victim);
	WAIT_STATE(ch, 10);
	ch->mana = ch->mana - 100;
	return;
}

void do_dstake(CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DEMON))
	{

		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(60 > ch->practice)
	{
		send_to_char("It costs 60 points of primal to create a stake.\n\r", ch);
		return;
	}
	ch->practice -= 60;
	obj = create_object(get_obj_index(OBJ_VNUM_STAKE), 0);
	if(IS_SET(obj->quest, QUEST_ARTIFACT))
		REMOVE_BIT(obj->quest, QUEST_ARTIFACT);
	obj_to_char(obj, ch);
	act("A Stake appears in your hands in a flash of light.", ch, 0, 0, TO_CHAR);
	act("A Stake appears in $n's hands in a flash of light.", ch, 0, 0, TO_ROOM);
	return;
}

// THIS IS THE END OF THE FILE THANKS
