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
#else /*  */
#include <sys/types.h>
#endif /*  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "player.h"
void do_principles(CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);
	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_NINJA))

	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(arg1[0] == '\0' && arg2[0] == '\0')

	{
		sprintf(buf, "Principles: Sora (%li), Chikyu (%li), Ningenno (%li).\n\r", ch->pcdata->powers[NPOWER_SORA],
			ch->pcdata->powers[NPOWER_CHIKYU], ch->pcdata->powers[NPOWER_NINGENNO]);
		send_to_char(buf, ch);
		return;
	}
	if(arg2[0] == '\0')

	{
		if(!str_cmp(arg1, "sora"))

		{
			send_to_char("Sora - The finding, observing, and locating principle.\n\r\n\r", ch);
			if(ch->pcdata->powers[NPOWER_SORA] < 1)
				send_to_char("You have none of the Sora principles.\n\r", ch);
			if(ch->pcdata->powers[NPOWER_SORA] > 0)
				send_to_char("Mitsukeru -Locate- The scrying power to find enemies.\n\r", ch);
			if(ch->pcdata->powers[NPOWER_SORA] > 1)
				send_to_char("Koryou -Consider- The read aura power, learn about your enemies.\n\r", ch);
			if(ch->pcdata->powers[NPOWER_SORA] > 2)
				send_to_char("Kakusu -Hidden- Enhanced Hide.  Only other ninjas can see you.\n\r", ch);
			if(ch->pcdata->powers[NPOWER_SORA] > 3)
				send_to_char("Uro-Uro -Silent Walk- You leave no footprints behind.\n\r", ch);
			if(ch->pcdata->powers[NPOWER_SORA] > 4)
				send_to_char("Kanzuite -Aware- The truesight power.\n\r", ch);
			if(ch->pcdata->powers[NPOWER_SORA] > 5)
				send_to_char
					("Bomuzite -Sleep Gas- By mixing a potion, you can put everyone in a room to sleep.\n\r",
					 ch);
			return;
		}

		else if(!str_cmp(arg1, "chikyu"))

		{
			send_to_char("Chikyu - Preperation for battle.\n\r\n\r", ch);
			if(ch->pcdata->powers[NPOWER_CHIKYU] < 1)
				send_to_char("You have none of the Chikyu principles.\n\r", ch);
			if(ch->pcdata->powers[NPOWER_CHIKYU] > 0)
				send_to_char("Tsuyoku -Strength- Toughness.\n\r", ch);
			if(ch->pcdata->powers[NPOWER_CHIKYU] > 1)
				send_to_char("Songai -Damage- Enhanced damage.\n\r", ch);
			if(ch->pcdata->powers[NPOWER_CHIKYU] > 2)
				send_to_char("Isogu -Haste- Adds two extra attacks.\n\r", ch);
			if(ch->pcdata->powers[NPOWER_CHIKYU] > 3)
				send_to_char("Tsuiseki -Hunt- Fast hunting.\n\r", ch);
			if(ch->pcdata->powers[NPOWER_CHIKYU] > 4)
				send_to_char("Sakeru -Sonic Speed- Dodge more attacks.\n\r", ch);
			if(ch->pcdata->powers[NPOWER_CHIKYU] > 5)
				send_to_char("HaraKiri -Blood Power- Hurt yourself to gain  power.\n\r", ch);
			return;
		}

		else if(!str_cmp(arg1, "ningenno"))

		{
			send_to_char("Ningenno - The battle, attacking and getting away.\n\r\n\r", ch);
			if(ch->pcdata->powers[NPOWER_NINGENNO] < 1)
				send_to_char("You have none of the Ningenno principles.\n\r", ch);
			if(ch->pcdata->powers[NPOWER_NINGENNO] > 0)
				send_to_char("Tsume -Claw- IronClaw always worn on wrist to aid in hand-to-hand.\n\r", ch);
			if(ch->pcdata->powers[NPOWER_NINGENNO] > 1)
				send_to_char("Hakunetsu -First Strike- You get super backstabs.\n\r", ch);
			if(ch->pcdata->powers[NPOWER_NINGENNO] > 2)
				send_to_char("Mienaku -Vanish- Never fail flee.\n\r", ch);
			if(ch->pcdata->powers[NPOWER_NINGENNO] > 3)
				send_to_char
					("Shiroken -Throwing Star- Added attack per round, like headbutt for demons.\n\r",
					 ch);
			if(ch->pcdata->powers[NPOWER_NINGENNO] > 4)
				send_to_char("Dokuyaku -Poison- Adds poisons to the Shiroken\n\r", ch);
			if(ch->pcdata->powers[NPOWER_NINGENNO] > 5)
				send_to_char("Circle - circle behind your opponent and hit him during battle\n\r", ch);
			return;
		}
		sprintf(buf, "Principles: Sora (%li), Chikyu (%li), Ningenno (%li).\n\r", ch->pcdata->powers[NPOWER_SORA],
			ch->pcdata->powers[NPOWER_CHIKYU], ch->pcdata->powers[NPOWER_NINGENNO]);
		send_to_char(buf, ch);
		return;
	}
	if(!str_cmp(arg2, "improve"))

	{
		long improve;
		long cost;
		long max;

		if(!str_cmp(arg1, "sora"))
		{
			improve = NPOWER_SORA;
			max = 6;
		}

		else if(!str_cmp(arg1, "chikyu"))
		{
			improve = NPOWER_CHIKYU;
			max = 6;
		}

		else if(!str_cmp(arg1, "ningenno"))
		{
			improve = NPOWER_NINGENNO;
			max = 6;
		}

		else

		{
			send_to_char("Principles: Sora, Chikyu, Ningenno.\n\r", ch);
			return;
		}
		cost = (ch->pcdata->powers[improve] + 1) * 10;
		arg1[0] = UPPER(arg1[0]);
		if(ch->pcdata->powers[improve] >= max)

		{
			sprintf(buf, "You have already gained all the powers of the %s principle.\n\r", arg1);
			send_to_char(buf, ch);
			return;
		}
		if(cost > ch->practice)

		{
			sprintf(buf, "It costs you %li primal to improve your %s principle.\n\r", cost, arg1);
			send_to_char(buf, ch);
			return;
		}
		ch->pcdata->powers[improve] += 1;
		ch->practice -= cost;
		sprintf(buf, "You improve your ability in the %s principle.\n\r", arg1);
		send_to_char(buf, ch);
	}

	else
		send_to_char("To improve a principle, type: Principle <principle type> improve.\n\r", ch);
	return;
}
void do_michi(CHAR_DATA * ch, char *argument)
{
	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_NINJA))

	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(IS_CLASS(ch, CLASS_NINJA))

	{
		if(ch->pcdata->stats[UNI_RAGE] >= 50)

		{
			send_to_char("But you are already in the state of Michi.\n\r", ch);
			return;
		}
		if(ch->move < 1000)

		{
			send_to_char("But you don't have enough move to perform the michi.\n\r", ch);
			return;
		}
		send_to_char("You are gifted positive energy while performing the michi.\n\r", ch);
		act("$n is gifted positives energy while performing the michi.", ch, 0, 0, TO_ROOM);
		ch->pcdata->stats[UNI_RAGE] += 200;
		ch->move -= 1000;
		WAIT_STATE(ch, 12);
		return;
	}

	else
		send_to_char("But you are already in the state of Michi.\n\r", ch);
	return;
}
void do_discipline(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);
	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_NINJA))

	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(arg[0] == '\0')

	{
		send_to_char("Discipline whom?\n\r", ch);
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
	if(IS_IMMORTAL(victim))

	{
		send_to_char("Not on Immortals's.\n\r", ch);
		return;
	}
	if(ch == victim)

	{
		send_to_char("You cannot discipline yourself.\n\r", ch);
		return;
	}
	if(victim->lord != '\0')

	{
		send_to_char("They have already been disciplined.\n\r", ch);
		return;
	}
	if(victim->level != LEVEL_AVATAR && !IS_IMMORTAL(victim))

	{
		send_to_char("You can only teach avatars.\n\r", ch);
		return;
	}
	if(victim->pcdata->class != 0)
	{
		send_to_char("They already have a profession.\n\r", ch);
		return;
	}
	if(!IS_IMMUNE(victim, IMM_VAMPIRE))

	{
		send_to_char("You cannot discipline an unwilling person.\n\r", ch);
		return;
	}
	if(ch->exp < 100000)

	{
		send_to_char("You cannot afford the 100000 exp required to discipline them.\n\r", ch);
		return;
	}
	if(victim->exp < 100000)

	{
		send_to_char("They cannot afford the 100000 exp required to be disciplined from you.\n\r", ch);
		return;
	}
	ch->exp -= 100000;
	victim->exp -= 100000;
	act("You teach $N the ways of the ninja.", ch, 0, victim, TO_CHAR);
	act("$n teaches $N the ways of the ninja.", ch, 0, victim, TO_NOTVICT);
	act("$n teaches you the way of the ninja.", ch, 0, victim, TO_VICT);
	victim->pcdata->class = CLASS_NINJA;
	send_to_char("You are now a ninja.\n\r", victim);
	free_string(victim->lord);
	victim->lord = str_dup(ch->name);
	save_char_obj(ch);
	save_char_obj(victim);
	return;
}
void do_kakusu(CHAR_DATA * ch, char *argument)
{
	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_NINJA))

	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(IS_CLASS(ch, CLASS_NINJA) && ch->pcdata->powers[NPOWER_SORA] < 3)

	{
		send_to_char("You have not learned the Sora principle to 3.\n\r", ch);
		return;
	}
	if(ch->move < 500)

	{
		send_to_char("You don't have 500 move to activate your power.\n\r", ch);
		return;
	}
	if(ch->fighting && ch->pcdata->stats[UNI_GEN] != 1)

	{
		send_to_char("You can only use this power in combat if you are a Master Ninja.\n\r", ch);
		return;
	}
	if(IS_AFFECTED(ch, AFF_HIDE))

	{
		REMOVE_BIT(ch->affected_by, AFF_HIDE);
		act("$n appears from the shadows.", ch, 0, 0, TO_ROOM);
		send_to_char("You appear from the shadows.\n\r", ch);
	}

	else

	{
		act("$n disappears into the shadows.", ch, 0, 0, TO_ROOM);
		send_to_char("You disappear into the shadows.\n\r", ch);
		SET_BIT(ch->affected_by, AFF_HIDE);
		ch->move -= 500;
	}
	return;
}
void do_dice(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	long i;

	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_NINJA))

	{
		send_to_char("You are not a ninjaaaaaaaaaaaaaaa!\n\r", ch);
		return;
	}
	if(ch->pcdata->stats[UNI_GEN] > 2 || ch->pcdata->stats[UNI_GEN] < 1)

	{
		send_to_char("You must be an apprentice ninja to use the die of chance.\n\r", ch);
		return;
	}
	if((victim = ch->fighting) == 0)

	{
		send_to_char("You must be in combat to do this.\n\r", ch);
		return;
	}
	if(ch->move < 1001)

	{
		send_to_char("You need 1000 movement points to throw the die of chance.\n\r", ch);
		return;
	}
	ch->move = 50;
	act("$n throws the {cdie of chance{x!", ch, 0, victim, TO_ROOM);
	act("You throw the {cdie of chance{x!", ch, 0, victim, TO_CHAR);
	i = number_range(1, 600);
	if(i < 100)

	{
		act("The {cdie of chance{x lands on {GONE{x.", ch, 0, victim, TO_ROOM);
		act("The {cdie of chance{x lands on {GONE{x.", ch, 0, victim, TO_CHAR);
		victim->hit = victim->hit == 0 ? 1 : victim->hit / 2;
	}

	else if(i < 200)

	{
		act("The {cdie of chance{x lands on {GTWO{x.", ch, 0, victim, TO_ROOM);
		act("The {cdie of chance{x lands on {GTWO{x.", ch, 0, victim, TO_CHAR);
		ch->hit = ch->hit == 0 ? 1 : ch->hit / 2;
	}

	else if(i < 300)

	{
		act("The {cdie of chance{x lands on {GTHREE{x.", ch, 0, victim, TO_ROOM);
		act("The {cdie of chance{x lands on {GTHREE{x.", ch, 0, victim, TO_CHAR);
		do_stance(victim, "");
		WAIT_STATE(victim, 24);
	}

	else if(i < 400)

	{
		act("The {cdie of chance{x lands on {GFOUR{x.", ch, 0, victim, TO_ROOM);
		act("The {cdie of chance{x lands on {GFOUR{x.", ch, 0, victim, TO_CHAR);
		do_stance(ch, "");
		WAIT_STATE(ch, 24);
	}

	else if(i < 500)

	{
		act("The {cdie of chance{x lands on {GFIVE{x.", ch, 0, victim, TO_ROOM);
		act("The {cdie of chance{x lands on {GFIVE{x.", ch, 0, victim, TO_CHAR);
		ch->hit = ch->max_hit;
	}

	else if(i < 601)

	{
		act("The {cdie of chance{x lands on {GSIX{x.", ch, 0, victim, TO_ROOM);
		act("The {cdie of chance{x lands on {GSIX{x.", ch, 0, victim, TO_CHAR);
		victim->hit = victim->max_hit;
	}
	return;
}
void do_kanzuite(CHAR_DATA * ch, char *argument)
{
	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_NINJA))

	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(IS_CLASS(ch, CLASS_NINJA) && ch->pcdata->powers[NPOWER_SORA] < 5)

	{
		send_to_char("You have not learned the Sora principle to 5.\n\r", ch);
		return;
	}

	else if(ch->move < 500)

	{
		send_to_char("You don't have 500 move to increase your awareness.\n\r", ch);
		return;
	}
	if(IS_SET(ch->act, PLR_HOLYLIGHT))

	{
		REMOVE_BIT(ch->act, PLR_HOLYLIGHT);
		send_to_char("You're senses return to normal.\n\r", ch);
	}

	else

	{
		SET_BIT(ch->act, PLR_HOLYLIGHT);
		send_to_char("You're senses increase into incredible proportions.\n\r", ch);
		ch->move -= 500;
	}
	return;
}
void do_mienaku(CHAR_DATA * ch, char *argument)
{
	ROOM_INDEX_DATA *was_in;
	ROOM_INDEX_DATA *now_in;
	CHAR_DATA *victim;
	long attempt;

	if(IS_CLASS(ch, CLASS_NINJA) && ch->pcdata->powers[NPOWER_NINGENNO] < 3)

	{
		send_to_char("You have not learned the Ningenno principle to 3.\n\r", ch);
		return;
	}

	else if(!IS_CLASS(ch, CLASS_NINJA))

	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(IS_NPC(ch))
		return;
	if((victim = ch->fighting) == 0)

	{
		if(ch->position == POS_FIGHTING)
			ch->position = POS_STANDING;
		send_to_char("You aren't fighting anyone.\n\r", ch);
		return;
	}
	if(ch->move < 200)

	{
		send_to_char("You don't have enough movement points to flee.\n\r", ch);
		return;
	}
	if(IS_AFFECTED(ch, AFF_WEBBED))

	{
		send_to_char("You are unable to move with all this sticky webbing on.\n\r", ch);
		return;
	}
	was_in = ch->in_room;

	{
		EXIT_DATA *pexit;
		long door;

		send_to_char("You move to vanish from combat!\n\r", ch);
		for(attempt = 0; attempt < 6; attempt++)

		{
			door = number_door();
			if((pexit = was_in->exit[door]) == 0
			   || pexit->to_room == 0
			   || IS_SET(pexit->exit_info, EX_CLOSED)
			   || (IS_NPC(ch) && IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)))
				continue;
			move_char(ch, door);
			if((now_in = ch->in_room) == was_in)
				continue;

/* Use escape instead of flee so people know it's the ninja power */
			ch->in_room = was_in;
			act("$n has escaped!", ch, 0, 0, TO_ROOM);
			ch->in_room = now_in;
			if(!IS_NPC(ch))
				send_to_char("You escape from combat!\n\r", ch);

/*	if (victim->in_room == ch->in_room)
    {
    	SET_BIT(victim->extra,BLINDFOLDED);
    	act("You start to move at super speeds and blind $N.",ch,0,victim,TO_CHAR);
    	act("$n starts to move at super speeds and blinds $N.",ch,0,victim,TO_ROOM);
    	act("$n starts to move at super speeds and blinds you.",ch,0,victim,TO_VICT);

    	return;
    }
  */
			ch->move -= 200;
			stop_fighting(ch, TRUE);
			return;
		}
	}
	return;
}
void do_bomuzite(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg1[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg1);
	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_NINJA))

	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	else if(ch->pcdata->powers[NPOWER_SORA] < 6)

	{
		send_to_char("You have not learned the Sora principle to 6.\n\r", ch);
		return;
	}
	if(arg1[0] == '\0')

	{
		send_to_char("Bomb who?\n\r", ch);
		return;
	}
	if((victim = get_char_room(ch, arg1)) == 0)

	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}
	if(ch == victim)

	{
		send_to_char("Not on yourself!\n\r", ch);
		return;
	}
	if(ch->position == POS_FIGHTING)

	{
		send_to_char("Not while fighting!\n\r", ch);
		return;
	}
	if(ch->move < 500)

	{
		send_to_char("You don't have enough movement points.\n\r", ch);
		return;
	}
	if(victim->in_room == ch->in_room)

	{
		act("You toss your bomb onto the floor and put $N to sleep.", ch, 0, victim, TO_CHAR);
		act("$n tosses a bomb onto the floor.  You feel sleepy.", ch, 0, victim, TO_VICT);
		victim->position = POS_SLEEPING;
		ch->move -= 500;
		WAIT_STATE(ch, 24);
		return;
	}
	return;
}
void do_tsume(CHAR_DATA * ch, char *argument)
{
	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_NINJA) || ch->pcdata->powers[NPOWER_NINGENNO] < 1)
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(IS_VAMPAFF(ch, VAM_CLAWS))
	{
		send_to_char("You remove the IronClaws from your wrists.\n\r", ch);
		REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_CLAWS);
		return;
	}
	send_to_char("You attach IronClaws to your wrists.\n\r", ch);
	SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_CLAWS);
	return;
}
void do_hara_kiri(CHAR_DATA * ch, char *argument)
{
	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_NINJA) || ch->pcdata->powers[NPOWER_CHIKYU] < 6)
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[HARA_KIRI] > 0)
	{
		send_to_char("You are already experiencing the power of HaraKiri.\n\r", ch);
		return;
	}
	if(ch->hit < ch->max_hit / 10)
	{
		send_to_char("You are hurt too badly already.\n\r", ch);
		return;
	}
	ch->pcdata->powers[HARA_KIRI] = ch->hit / 500;
	if(ch->pcdata->powers[HARA_KIRI] < 5)
		ch->pcdata->powers[HARA_KIRI] = 5;
	ch->hit = 1;
	ch->mana = 1;
	ch->move = 1;
	send_to_char("You cut your finger and bleed profusely.\n\r", ch);
	act("$n cuts his finger and obtains the power of HaraKiri", ch, 0, 0, TO_ROOM);
	return;
}


// THIS IS THE END OF THE FILE THANKS
