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
#include <string.h>
#include <time.h>
#include "merc.h"



/*
 * Local functions.
 */
long hit_gain args((CHAR_DATA * ch));
long mana_gain args((CHAR_DATA * ch));
long move_gain args((CHAR_DATA * ch));
void mobile_update args((void));
void weather_update args((void));
void char_update args((void));
void obj_update args((void));
void aggr_update args((void));
void ww_update args((void));
void embrace_update args((void));
void spec_update args((void));
void second_update args((void));

extern void vote_update(void);
extern void bard_update(void);
extern void trivia_update(void);

bool reset_mud = FALSE;
extern char *const dir_name[];

bool check_safe(CHAR_DATA * ch)
{
	ROOM_INDEX_DATA *room;
	EXIT_DATA *exit;
	long i, r;

	if(!ch || IS_NPC(ch) || IS_IMMORTAL(ch))
		return FALSE;

	if((room = ch->in_room) == 0)
		return FALSE;

	if(IS_AFFECTED(ch, AFF_SAFE))
		return FALSE;

	if(IS_SET(room->room_flags, ROOM_SAFE))
		ch->pcdata->safe_wait++;

	if(ch->in_room->vnum == 2 && ch->desc != 0 && !IS_NPC(ch) && ch->level < 2)
	{
		send_to_char("You cannot sit at temple long as an avatar.\n\r", ch);
		send_to_char("A powerful force hurls you from the room.\n\r", ch);
		act("$n is hurled from the room by a powerful force.", ch, 0, 0, TO_ROOM);
		ch->position = POS_STUNNED;
		char_from_room(ch);
		char_to_room(ch, get_room_index(ROOM_VNUM_TEMPLE));
		act("$n appears in the room, and falls to the ground stunned.", ch, 0, 0, TO_ROOM);
	}

	if(ch->pcdata->safe_wait > 15)
	{
		for(i = 0; i < 10; i++)
		{
			r = number_door();
			if((exit = room->exit[r]) != 0 && exit->to_room != 0)
			{
				send_to_char("You have been safe enough...moving you.\n\r", ch);
				act("$n has been safe enough...$e dissapears $d!", ch, dir_name[r], 0, TO_ROOM);
				char_from_room(ch);
				char_to_room(ch, exit->to_room);
				do_look(ch, "auto");
				ch->fight_timer = 10;
				ch->pcdata->safe_wait = 0;
				break;
			}
		}
		return TRUE;
	}

	return FALSE;
}


void gain_exp(CHAR_DATA * ch, long gain)
{
	CHAR_DATA *mount = 0;
	CHAR_DATA *master = 0;

	if(IS_NPC(ch) && (mount = ch->mount) != 0 && !IS_NPC(mount))
	{
		if((master = ch->master) == 0 || master != mount)
			mount->exp += (gain * number_range(1, 6));
	}

	if(!IS_NPC(ch))
		ch->exp += (gain * number_range(1, 6));
	return;
}



/*
 * Regeneration stuff.
 */
long hit_gain(CHAR_DATA * ch)
{
	long gain;
	long conamount;

	if(IS_NPC(ch))
	{
		gain = number_range(ch->level / 2, ch->level * 2);
	}
	else
	{
		if(IS_CLASS(ch, CLASS_VAMPIRE))
			return 0;
		gain = number_range(300, 500);

		if((conamount = (get_curr_con(ch) + 1)) > 1)
		{
			switch (ch->position)
			{
			case POS_MEDITATING:
				gain *= (long) (conamount * 0.5);
				break;
			case POS_SLEEPING:
				gain *= (long) conamount;
				break;
			case POS_RESTING:
				gain *= (long) (conamount * 0.5);
				break;
			}
		}

		if(ch->pcdata->condition[COND_FULL] == 0 && !IS_HERO(ch))
			gain = (long) (gain * 0.5);

		if(ch->pcdata->condition[COND_THIRST] == 0 && !IS_HERO(ch))
			gain = (long) (gain * 0.5);

	}

	if(!IS_NPC(ch) && (IS_AFFECTED(ch, AFF_POISON) || IS_AFFECTED(ch, AFF_FLAMING)))
		gain = (long) (gain * 0.25);

	return UMIN(gain, ch->max_hit - ch->hit);
}



long mana_gain(CHAR_DATA * ch)
{
	long gain;
	long intamount;

	if(IS_NPC(ch))
	{
		gain = ch->level;
	}
	else
	{
		if(IS_CLASS(ch, CLASS_VAMPIRE))
			return 0;
		gain = number_range(10, 20);

		if((intamount = (get_curr_int(ch) + 1)) > 1)
		{
			switch (ch->position)
			{
			case POS_MEDITATING:
				gain *= (long) (intamount * 10.0);
				break;
			case POS_SLEEPING:
				gain *= intamount;
				break;
			case POS_RESTING:
				gain *= (long) (intamount * 0.5);
				break;
			}
		}

		if(!IS_HERO(ch) && ch->pcdata->condition[COND_THIRST] == 0)
			gain = (long) (gain * 0.5);

	}

	if(IS_AFFECTED(ch, AFF_POISON) || IS_AFFECTED(ch, AFF_FLAMING))
		gain = (long) (gain * 0.25);

	return UMIN(gain, ch->max_mana - ch->mana);
}



long move_gain(CHAR_DATA * ch)
{
	long gain;
	long dexamount;

	if(IS_NPC(ch))
	{
		gain = ch->level;
	}
	else
	{
		if(IS_CLASS(ch, CLASS_VAMPIRE))
			return 0;
		gain = number_range(10, 20);

		if((dexamount = (get_curr_dex(ch) + 1)) > 1)
		{
			switch (ch->position)
			{
			case POS_MEDITATING:
				gain *= (long) (dexamount * (ch->level * 10));
				break;
			case POS_SLEEPING:
				gain *= dexamount;
				break;
			case POS_RESTING:
				gain *= (long) (dexamount * 0.5);
				break;
			}
		}

		if(!IS_HERO(ch) && ch->pcdata->condition[COND_THIRST] == 0)
			gain = (long) (gain * 0.5);
	}

	if(IS_AFFECTED(ch, AFF_POISON) || IS_AFFECTED(ch, AFF_FLAMING))
		gain = (long) (gain * 0.25);

	return UMIN(gain, ch->max_move - ch->move);
}



void gain_condition(CHAR_DATA * ch, long iCond, long value)
{
	long condition;

	if(value == 0 || IS_NPC(ch))
		return;

	if(!IS_NPC(ch) && IS_HERO(ch) && !IS_CLASS(ch, CLASS_VAMPIRE) && iCond != COND_DRUNK)
		return;

	condition = ch->pcdata->condition[iCond];
	if(!IS_NPC(ch) && !IS_CLASS(ch, CLASS_VAMPIRE))
		ch->pcdata->condition[iCond] = URANGE(0, condition + value, 48);
	else
	{
		if(ch->pcdata->stats[UNI_GEN] == 2)
		{
			ch->pcdata->condition[iCond] = URANGE(0, condition + value, 1000);

			if(ch->pcdata->condition[iCond] > 30000)
				ch->pcdata->condition[iCond] = 30000;
		}
		else if(ch->pcdata->stats[UNI_GEN] == 3)
			ch->pcdata->condition[iCond] = URANGE(0, condition + value, 666);
		else if(ch->pcdata->stats[UNI_GEN] == 4)
			ch->pcdata->condition[iCond] = URANGE(0, condition + value, 500);
		else if(ch->pcdata->stats[UNI_GEN] >= 5)
			ch->pcdata->condition[iCond] = URANGE(0, condition + value, 400);
		else if(ch->pcdata->stats[UNI_GEN] == 6)
			ch->pcdata->condition[iCond] = URANGE(0, condition + value, 333);
		else if(ch->pcdata->stats[UNI_GEN] == 7)
			ch->pcdata->condition[iCond] = URANGE(0, condition + value, 285);
		else if(ch->pcdata->stats[UNI_GEN] == 8)
			ch->pcdata->condition[iCond] = URANGE(0, condition + value, 250);
		else if(ch->pcdata->stats[UNI_GEN] == 9)
			ch->pcdata->condition[iCond] = URANGE(0, condition + value, 222);
		else if(ch->pcdata->stats[UNI_GEN] == 10)
			ch->pcdata->condition[iCond] = URANGE(0, condition + value, 200);
		else if(ch->pcdata->stats[UNI_GEN] == 11)
			ch->pcdata->condition[iCond] = URANGE(0, condition + value, 181);
		else if(ch->pcdata->stats[UNI_GEN] == 12)
			ch->pcdata->condition[iCond] = URANGE(0, condition + value, 166);
		else if(ch->pcdata->stats[UNI_GEN] == 13)
			ch->pcdata->condition[iCond] = URANGE(0, condition + value, 153);

	}

	if(ch->pcdata->condition[iCond] == 0)
	{
		switch (iCond)
		{
		case COND_FULL:
			if(!IS_CLASS(ch, CLASS_VAMPIRE))
			{
				send_to_char("You are REALLY hungry.\n\r", ch);
				act("You hear $n's stomach rumbling.", ch, 0, 0, TO_ROOM);
			}
			break;

		case COND_THIRST:
			if(!IS_CLASS(ch, CLASS_VAMPIRE))
				send_to_char("You are REALLY thirsty.\n\r", ch);
			else if(ch->hit > 0)
			{
				send_to_char("You are DYING from lack of blood!\n\r", ch);
				act("$n gets a hungry look in $s eyes.", ch, 0, 0, TO_ROOM);
				ch->hit = ch->hit - number_range(2, 5);
				if(number_percent() <= ch->beast && ch->beast > 0)
					vamp_rage(ch);
				if(!IS_VAMPAFF(ch, VAM_FANGS))
					do_fangs(ch, "");
/*
    if ( IS_SET(ch->polyaff, POLY_ZULO) )
    {
        act( "You transform into human form.", ch, 0, 0, TO_CHAR );
        act( "$n transforms into human form.", ch, 0, 0, TO_ROOM );
        do_dragonform( ch, "auto" );
        REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
        REMOVE_BIT(ch->polyaff, POLY_ZULO);
        REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_CHANGED);
        clear_stats(ch);
        free_string( ch->morph );
        ch->morph = str_dup( "" );
        return;
    }
*/
			}
			break;

		case COND_DRUNK:
			if(condition != 0)
				send_to_char("You are sober.\n\r", ch);
			break;
		}
	}
	else if(ch->pcdata->condition[iCond] < 10)
	{
		switch (iCond)
		{
		case COND_FULL:
			if(!IS_CLASS(ch, CLASS_VAMPIRE))
				send_to_char("You feel hungry.\n\r", ch);
			break;

		case COND_THIRST:
			if(!IS_CLASS(ch, CLASS_VAMPIRE))
				send_to_char("You feel thirsty.\n\r", ch);
			else
			{
				send_to_char("You crave blood.\n\r", ch);
				if(number_range(1, 1000) <= ch->beast && ch->beast > 0)
					vamp_rage(ch);
				if(number_percent() > (ch->pcdata->condition[COND_THIRST] + 75)
				   && !IS_VAMPAFF(ch, VAM_FANGS))
					do_fangs(ch, "");
			}
			break;
		}
	}

	return;
}


void spec_update(void)
{
	SPEC_OBJ_DATA *spec;
	SPEC_OBJ_DATA *spec_next;

	for(spec = spec_obj_list; spec; spec = spec_next)
	{
		spec_next = spec->next_world;

		/* specs with timer of -1 go on forever...
		   if( spec->timer != -1 && --spec->timer == 0
		   &&  (*spec->spec_fun)(spec, 0, 0, 0, UPDATE_END) == TRUE)
		   {
		   if( spec == beginning )
		   spec_prev = beginning = spec->next;
		   else
		   spec_prev->next      = spec->next;

		   spec->next   = spec_obj_free;
		   spec_obj_free        = spec;
		   continue;
		   }
		   else
		   { */
		/* false means the spec terminated itself       */
		/* dont want to point to null spec (spec_prev)  */
		/*      if( (*spec->spec_fun)(spec, 0, 0, 0, UPDATE_TIMER) == FALSE )
		   {
		   if( beginning == spec )
		   beginning = spec_next;

		   continue;
		   }
		   } */
	}

}




/*
 * Mob autonomous action.
 * This function takes 25% to 35% of ALL Merc cpu time.
 * -- Furey
 */
void mobile_update(void)
{
	CHAR_DATA *ch;
	CHAR_DATA *ch_next;
	EXIT_DATA *pexit;
	long door;

	/* Examine all mobs. */
	for(ch = char_list; ch != 0; ch = ch_next)
	{
		ch_next = ch->next;

		if(ch->in_room == 0)
			continue;
/*
	if ( ch->hunting != 0 && ch->hunting != '\0' &&
	    strlen(ch->hunting) > 1 )
	{
	    check_hunt( ch );
	    continue;
	}
*/
		if(!IS_NPC(ch))
		{
			if(ch->pcdata->condition[COND_DRUNK] > 10 && number_range(1, 10) == 1)
			{
				send_to_char("You hiccup loudly.\n\r", ch);
				act("$n hiccups.", ch, 0, 0, TO_ROOM);
			}
			if(ch->pcdata->stage[0] > 0 || ch->pcdata->stage[2] > 0)
			{
				CHAR_DATA *vch;

				if(ch->pcdata->stage[1] > 0 && ch->pcdata->stage[2] >= 225)
				{
					ch->pcdata->stage[2] += 1;
					if((vch = ch->pcdata->partner) != 0 &&
					   !IS_NPC(vch) && vch->pcdata->partner == ch &&
					   ((vch->pcdata->stage[2] >= 200 && vch->sex == SEX_FEMALE) ||
					    (ch->pcdata->stage[2] >= 200 && ch->sex == SEX_FEMALE)))
					{
						if(ch->in_room != vch->in_room)
							continue;
						if(vch->pcdata->stage[2] >= 225 &&
						   ch->pcdata->stage[2] >= 225 &&
						   vch->pcdata->stage[2] < 240 && ch->pcdata->stage[2] < 240)
						{
							ch->pcdata->stage[2] = 240;
							vch->pcdata->stage[2] = 240;
						}
						if(ch->sex == SEX_MALE && vch->pcdata->stage[2] >= 240)
						{
							act("You thrust deeply between $N's warm, damp thighs.", ch, 0,
							    vch, TO_CHAR);
							act("$n thrusts deeply between your warm, damp thighs.", ch, 0,
							    vch, TO_VICT);
							act("$n thrusts deeply between $N's warm, damp thighs.", ch, 0,
							    vch, TO_NOTVICT);
							if(vch->pcdata->stage[2] > ch->pcdata->stage[2])
								ch->pcdata->stage[2] = vch->pcdata->stage[2];
						}
						else if(ch->sex == SEX_FEMALE && vch->pcdata->stage[2] >= 240)
						{
							act("You squeeze your legs tightly around $N, moaning loudly.", ch,
							    0, vch, TO_CHAR);
							act("$n squeezes $s legs tightly around you, moaning loudly.", ch,
							    0, vch, TO_VICT);
							act("$n squeezes $s legs tightly around $N, moaning loudly.", ch,
							    0, vch, TO_NOTVICT);
							if(vch->pcdata->stage[2] > ch->pcdata->stage[2])
								ch->pcdata->stage[2] = vch->pcdata->stage[2];
						}
					}
					if(ch->pcdata->stage[2] >= 250)
					{
						if((vch = ch->pcdata->partner) != 0 &&
						   !IS_NPC(vch) && vch->pcdata->partner == ch &&
						   ch->in_room == vch->in_room)
						{
							vch->pcdata->stage[2] = 250;
							if(ch->sex == SEX_MALE)
							{
								stage_update(ch, vch, 2);
								stage_update(vch, ch, 2);
							}
							else
							{
								stage_update(vch, ch, 2);
								stage_update(ch, vch, 2);
							}
							ch->pcdata->stage[0] = 0;
							vch->pcdata->stage[0] = 0;
							if(!IS_EXTRA(ch, EXTRA_EXP))
							{
								send_to_char
									("Congratulations on achieving a simultanious orgasm!  Recieve 100000 exp!\n\r",
									 ch);
								SET_BIT(ch->extra, EXTRA_EXP);
								ch->exp += 100000;
							}
							if(!IS_EXTRA(vch, EXTRA_EXP))
							{
								send_to_char
									("Congratulations on achieving a simultanious orgasm!  Recieve 100000 exp!\n\r",
									 vch);
								SET_BIT(vch->extra, EXTRA_EXP);
								vch->exp += 100000;
							}
						}
					}
				}
				else
				{
					if(ch->pcdata->stage[0] > 0 && ch->pcdata->stage[2] < 1 &&
					   ch->position != POS_RESTING)
					{
						if(ch->pcdata->stage[0] > 1)
							ch->pcdata->stage[0] -= 1;
						else
							ch->pcdata->stage[0] = 0;
					}
					else if(ch->pcdata->stage[2] > 0 && ch->pcdata->stage[0] < 1)
					{
						if(ch->pcdata->stage[2] > 10)
							ch->pcdata->stage[2] -= 10;
						else
							ch->pcdata->stage[2] = 0;
						if(ch->sex == SEX_MALE && ch->pcdata->stage[2] == 0)
							send_to_char("You feel fully recovered.\n\r", ch);
					}
				}
			}

/* Ninja Michi - Loki */
			if(!IS_NPC(ch) && IS_CLASS(ch, CLASS_NINJA) && IS_HERO(ch))
			{
				if(ch->position == POS_FIGHTING && ch->pcdata->stats[UNI_RAGE] > 0
				   && ch->pcdata->stats[UNI_RAGE] < 200 && !IS_ITEMAFF(ch, ITEMA_RAGER))
					ch->pcdata->stats[UNI_RAGE] += 1;
				else if(ch->position == POS_MEDITATING && ch->pcdata->stats[UNI_RAGE] > 0
					&& ch->pcdata->stats[UNI_RAGE] < 200 && !IS_ITEMAFF(ch, ITEMA_RAGER))
					ch->pcdata->stats[UNI_RAGE] -= 20;
				else if(ch->pcdata->stats[UNI_RAGE] > 0 && !IS_ITEMAFF(ch, ITEMA_RAGER))
					ch->pcdata->stats[UNI_RAGE] -= 1;
				if(ch->pcdata->stats[UNI_RAGE] < 1)
					continue;
			}

			if(!IS_NPC(ch) && IS_CLASS(ch, CLASS_VAMPIRE) && IS_HERO(ch))
			{
				if(ch->position == POS_FIGHTING && ch->pcdata->stats[UNI_RAGE] > 0
				   && ch->pcdata->stats[UNI_RAGE] < 25 && !IS_ITEMAFF(ch, ITEMA_RAGER))
					ch->pcdata->stats[UNI_RAGE] += 1;
				else if(ch->pcdata->stats[UNI_RAGE] > 0 && !IS_ITEMAFF(ch, ITEMA_RAGER))
					ch->pcdata->stats[UNI_RAGE] -= 1;
				if(ch->pcdata->stats[UNI_RAGE] < 1)
					continue;
				if(ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
					werewolf_regen(ch);
				if(ch->loc_hp[6] > 0)
				{
					long sn = skill_lookup("clot");

					(*skill_table[sn].spell_fun) (sn, ch->level, ch, ch);
				}
				else
				{
					if((ch->loc_hp[0] + ch->loc_hp[1] + ch->loc_hp[2] +
					    ch->loc_hp[3] + ch->loc_hp[4] + ch->loc_hp[5]) != 0)
						reg_mend(ch);
				}
			}
			else if(!IS_NPC(ch) && IS_CLASS(ch, CLASS_WEREWOLF) && IS_HERO(ch))
			{
				if(ch->position == POS_FIGHTING && !IS_ITEMAFF(ch, ITEMA_RAGER))
				{
					if(ch->pcdata->stats[UNI_RAGE] < 300)
						ch->pcdata->stats[UNI_RAGE] += number_range(5, 10);
					if(ch->pcdata->stats[UNI_RAGE] < 300 && ch->pcdata->powers[WPOWER_WOLF] > 3)
						ch->pcdata->stats[UNI_RAGE] += number_range(5, 10);
					if(!IS_SET(ch->special, SPC_WOLFMAN) && ch->pcdata->stats[UNI_RAGE] >= 100)
						do_werewolf(ch, "");
				}
				else if(ch->pcdata->stats[UNI_RAGE] > 0 && !IS_ITEMAFF(ch, ITEMA_RAGER))
				{
					ch->pcdata->stats[UNI_RAGE] -= 1;
					if(ch->pcdata->stats[UNI_RAGE] < 100)
						do_unwerewolf(ch, "");
				}
				if(ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
					werewolf_regen(ch);
				if(IS_CLASS(ch, CLASS_WEREWOLF) && ch->position == POS_SLEEPING
				   && ch->pcdata->powers[WPOWER_BEAR] > 3 && ch->hit > 0)
				{
					if(ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
						werewolf_regen(ch);
					if(ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
						werewolf_regen(ch);
					if(ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
						werewolf_regen(ch);
					if(ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
						werewolf_regen(ch);
				}
				if(ch->loc_hp[6] > 0)
				{
					long sn = skill_lookup("clot");

					(*skill_table[sn].spell_fun) (sn, ch->level, ch, ch);
				}
				else
				{
					if((ch->loc_hp[0] + ch->loc_hp[1] + ch->loc_hp[2] +
					    ch->loc_hp[3] + ch->loc_hp[4] + ch->loc_hp[5]) != 0)
						reg_mend(ch);
				}
			}
			else if(!IS_CLASS(ch, CLASS_DEMON) || (IS_CLASS(ch, CLASS_DEMON) && IN_HELL(ch)))
			{
				if(ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
					werewolf_regen(ch);
				if(ch->hit > 0)
				{
					if(ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
						werewolf_regen(ch);
					if(ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
						werewolf_regen(ch);
					if(ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
						werewolf_regen(ch);
					if(ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
						werewolf_regen(ch);
				}
				if(ch->loc_hp[6] > 0)
				{
					long sn = skill_lookup("clot");

					(*skill_table[sn].spell_fun) (sn, ch->level, ch, ch);
				}
				else
				{
					if((ch->loc_hp[0] + ch->loc_hp[1] + ch->loc_hp[2] +
					    ch->loc_hp[3] + ch->loc_hp[4] + ch->loc_hp[5]) != 0)
						reg_mend(ch);
				}
			}
			else if(IS_ITEMAFF(ch, ITEMA_REGENERATE) || (!IS_NPC(ch) &&
								     (IS_CLASS(ch, CLASS_HIGHLANDER))))
			{
				if(ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
					werewolf_regen(ch);
				if(ch->loc_hp[6] > 0)
				{
					long sn = skill_lookup("clot");

					(*skill_table[sn].spell_fun) (sn, ch->level, ch, ch);
				}
				else
				{
					if((ch->loc_hp[0] + ch->loc_hp[1] + ch->loc_hp[2] +
					    ch->loc_hp[3] + ch->loc_hp[4] + ch->loc_hp[5]) != 0)
						reg_mend(ch);
				}
			}
			continue;
		}

		if(IS_AFFECTED(ch, AFF_CHARM))
			continue;

		/* That's all for sleeping / busy monster */
		if(ch->position != POS_STANDING)
			continue;

		/* Scavenge */
		if(IS_SET(ch->act, ACT_SCAVENGER) && ch->in_room->contents != 0 && number_bits(2) == 0)
		{
			OBJ_DATA *obj;
			OBJ_DATA *obj_best;
			long max;

			max = 1;
			obj_best = 0;
			for(obj = ch->in_room->contents; obj; obj = obj->next_content)
			{
				if(CAN_WEAR(obj, ITEM_TAKE) && obj->cost > max)
				{
					obj_best = obj;
					max = obj->cost;
				}
			}

			if(obj_best)
			{
				obj_from_room(obj_best);
				obj_to_char(obj_best, ch);
				act("$n picks $p up.", ch, obj_best, 0, TO_ROOM);
				act("You pick $p up.", ch, obj_best, 0, TO_CHAR);
			}
		}

		/* Wander */
		if(!IS_SET(ch->act, ACT_SENTINEL)
		   && (door = number_bits(5)) <= 5
		   && (pexit = ch->in_room->exit[door]) != 0
		   && pexit->to_room != 0
		   && !IS_SET(pexit->exit_info, EX_CLOSED)
		   && !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
		   && (ch->hunting == 0 || strlen(ch->hunting) < 2)
		   && ((!IS_SET(ch->act, ACT_STAY_AREA) && ch->level < 900) || pexit->to_room->area == ch->in_room->area))
		{
			move_char(ch, door);
		}

		/* Flee */
		if(ch->hit < ch->max_hit / 2
		   && (door = number_bits(3)) <= 5
		   && (pexit = ch->in_room->exit[door]) != 0
		   && pexit->to_room != 0
		   && !IS_AFFECTED(ch, AFF_WEBBED)
		   && ch->level < 900
		   && !IS_SET(pexit->exit_info, EX_CLOSED) && !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB))
		{
			CHAR_DATA *rch;
			bool found;

			found = FALSE;
			for(rch = pexit->to_room->people; rch != 0; rch = rch->next_in_room)
			{
				if(!IS_NPC(rch))
				{
					found = TRUE;
					break;
				}
			}
			if(!found)
				move_char(ch, door);
		}

	}

	return;
}



/*
 * Update the weather.
 */
void weather_update(void)
{
	char buf[MAX_STRING_LENGTH];
	DESCRIPTOR_DATA *d;
	CHAR_DATA *ch = 0;
	long diff;
	bool char_up;

	buf[0] = '\0';

	switch (++time_info.hour)
	{
	case 5:
		weather_info.sunlight = SUN_LIGHT;
		strcat(buf, "The day has begun.\n\r");
		break;

	case 6:
		weather_info.sunlight = SUN_RISE;
		strcat(buf, "The sun rises in the east.\n\r");
		break;

	case 19:
		weather_info.sunlight = SUN_SET;
		strcat(buf, "The sun slowly disappears in the west.\n\r");
		break;

	case 20:
		weather_info.sunlight = SUN_DARK;
		strcat(buf, "The night has begun.\n\r");
		break;

	case 24:
		time_info.hour = 0;
		time_info.day++;
		for(d = descriptor_list; d != 0; d = d->next)
		{
			char_up = FALSE;
			if(d->connected == CON_PLAYING && (ch = d->character) != 0 && !IS_NPC(ch))
			{
				send_to_char("{YYou hear a clock in the distance strike midnight.{x\n\r", ch);
				if(IS_SET(ch->flag2, AFF2_ROT))
				{
					send_to_char("Your flesh feels better.\n\r", ch);
					REMOVE_BIT(ch->flag2, AFF2_ROT);

				}
				if(IS_SET(ch->in_room->room_flags, ROOM_SILENCE))
				{
					send_to_char("The silence leaves the room.\n\r", ch);
					act("The silence leaves the room.", ch, 0, 0, TO_ROOM);
					REMOVE_BIT(ch->in_room->room_flags, ROOM_SILENCE);

				}

				if(IS_SET(ch->in_room->room_flags, ROOM_FLAMING))
				{
					send_to_char("The flames in the room die down.\n\r", ch);
					act("The flames in the room die down.", ch, 0, 0, TO_ROOM);
					REMOVE_BIT(ch->in_room->room_flags, ROOM_FLAMING);
				}

				if(IS_CLASS(ch, CLASS_VAMPIRE) && ch->in_room && ch->in_room->vnum > 100)
				{
					if(ch->hit < ch->max_hit)
					{
						ch->hit = ch->max_hit;
						char_up = TRUE;
					}
					if(ch->mana < ch->max_mana)
					{
						ch->mana = ch->max_mana;
						char_up = TRUE;
					}
					if(ch->move < ch->max_move)
					{
						ch->move = ch->max_move;
						char_up = TRUE;
					}
					if(char_up)
						send_to_char
							("You feel the strength of the kindred flow through your veins!\n\r",
							 ch);
					ch->position = POS_STANDING;
				}
				if(IS_EXTRA(ch, EXTRA_POTENCY))
				{
					send_to_char("You feel your blood potency fade away.\n\r", ch);
					ch->pcdata->stats[UNI_GEN]++;
					REMOVE_BIT(ch->extra, EXTRA_POTENCY);
				}

				if(IS_SET(ch->newbits, NEW_TIDE))
				{
					REMOVE_BIT(ch->newbits, NEW_TIDE);
					send_to_char("The tide of vitae leaves you.\n\r", ch);
				}

			}
		}
		break;
	}

	if(time_info.day >= 35)
	{
		time_info.day = 0;
		time_info.month++;
	}

	if(time_info.month >= 17)
	{
		time_info.month = 0;
		time_info.year++;
	}

	/*
	 * Weather change.
	 */
	if(time_info.month >= 9 && time_info.month <= 16)
		diff = weather_info.mmhg > 985 ? -2 : 2;
	else
		diff = weather_info.mmhg > 1015 ? -2 : 2;

	weather_info.change += diff * dice(1, 4) + dice(2, 6) - dice(2, 6);
	weather_info.change = UMAX(weather_info.change, -12);
	weather_info.change = UMIN(weather_info.change, 12);

	weather_info.mmhg += weather_info.change;
	weather_info.mmhg = UMAX(weather_info.mmhg, 960);
	weather_info.mmhg = UMIN(weather_info.mmhg, 1040);

	switch (weather_info.sky)
	{
	default:
		bug("Weather_update: bad sky %li.", weather_info.sky);
		weather_info.sky = SKY_CLOUDLESS;
		break;

	case SKY_CLOUDLESS:
		if(weather_info.mmhg < 990 || (weather_info.mmhg < 1010 && number_bits(2) == 0))
		{
			strcat(buf, "The sky is getting cloudy.\n\r");
			weather_info.sky = SKY_CLOUDY;
		}
		break;

	case SKY_CLOUDY:
		if(weather_info.mmhg < 970 || (weather_info.mmhg < 990 && number_bits(2) == 0))
		{
			strcat(buf, "It starts to rain.\n\r");
			weather_info.sky = SKY_RAINING;
		}

		if(weather_info.mmhg > 1030 && number_bits(2) == 0)
		{
			strcat(buf, "The clouds disappear.\n\r");
			weather_info.sky = SKY_CLOUDLESS;
		}
		break;

	case SKY_RAINING:
		if(weather_info.mmhg < 970 && number_bits(2) == 0)
		{
			strcat(buf, "Lightning flashes in the sky.\n\r");
			weather_info.sky = SKY_LIGHTNING;
		}

		if(weather_info.mmhg > 1030 || (weather_info.mmhg > 1010 && number_bits(2) == 0))
		{
			strcat(buf, "The rain stopped.\n\r");
			weather_info.sky = SKY_CLOUDY;
		}
		break;

	case SKY_LIGHTNING:
		if(weather_info.mmhg > 1010 || (weather_info.mmhg > 990 && number_bits(2) == 0))
		{
			strcat(buf, "The lightning has stopped.\n\r");
			weather_info.sky = SKY_RAINING;
			break;
		}
		break;
	}

	if(buf[0] != '\0')
	{
		for(d = descriptor_list; d != 0; d = d->next)
		{
			if(d->connected == CON_PLAYING && IS_OUTSIDE(d->character) && IS_AWAKE(d->character))
				send_to_char(buf, d->character);
		}
	}

	return;
}
void limbo_update(void)
{
	DESCRIPTOR_DATA *d;
	CHAR_DATA *ch;

	for(d = descriptor_list; d != 0; d = d->next)
	{
		if((ch = d->character) == 0 || IS_NPC(ch) || d->connected != CON_PLAYING)
		{
			continue;
		}
/*  taking this out until it becomes a problem again. pip
if (ch->in_room->vnum == 2)
{
act("A strange force hurls $n from the room.",ch,0,0,TO_ROOM);
send_to_char("A strange force hurls you from the room.\n\r",ch);
char_from_room(ch);
char_to_room(ch,get_room_index(ROOM_VNUM_TEMPLE));
do_look(ch,"auto");
ch->position = POS_STUNNED;
}
*/
	}
}

/*
void embrace_update( void )
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA       *ch;
    CHAR_DATA       *victim;
    char buf[MAX_STRING_LENGTH];
    for ( d = descriptor_list; d != 0; d = d->next )
      {
      if ((ch = d->character) == 0
        || IS_NPC(ch))
	{
            continue;
      }

	if (ch->embracing == 0)
		continue;

	if (!IS_CLASS(ch,CLASS_VAMPIRE))
	  stop_embrace(ch,0);

	victim=ch->embracing;

	if (get_char_world(ch,victim->name) ==0)
	{stop_embrace(ch,0);continue;}

	if (ch->in_room != victim->in_room)
	  stop_embrace(ch,victim);

      if (IS_NPC(victim) )
      {
	  ch->pcdata->condition[COND_THIRST] += number_range(40,50);
        act("$N falls to the ground lifeless.",ch,0,victim,TO_CHAR);
        act("$N falls to the ground lifeless.",ch,0,victim,TO_ROOM);
        stop_embrace(ch,victim);
        raw_kill(victim);
        return;
      }

	if (victim->pcdata->condition[COND_THIRST] < 0)
	  victim->pcdata->condition[COND_THIRST] = 0;

        ch->pcdata->condition[COND_THIRST] += number_range(30,40);
        sprintf(buf,"%s shudders in ecstacy as he drinks blood from %s's neck.\n\r",ch->name,victim->name);
	  act(buf,ch,0,0,TO_ROOM);
	  sprintf(buf,"You shudder in ecstacy as you drink blood from %s's neck.\n\r",victim->name);
	  send_to_char(buf,ch);
	  sprintf(buf,"You feel some of your life slip away as %s drinks from your neck.\n\r",ch->name);
	  send_to_char(buf,victim);
	  ch->pcdata->condition[COND_THIRST] += number_range(35,40);
	  victim->pcdata->condition[COND_THIRST] -= number_range(40,42);
	  if (ch->pcdata->condition[COND_THIRST] > 2000/ch->pcdata->stats[UNI_GEN])
	{ch->pcdata->condition[COND_THIRST]=2000/ch->pcdata->stats[UNI_GEN];
	  send_to_char("Your bloodlust is sated.\n\r",ch);}
	  if (victim->pcdata->condition[COND_THIRST] < 0)
	    victim->pcdata->condition[COND_THIRST]=0;

	  if (victim->pcdata->condition[COND_THIRST] ==0)
	  {
	    sprintf(buf,"You have been diablerized!\n\r");
	    send_to_char(buf,victim);
	    raw_kill(victim);
	    sprintf(buf,"%s's body falls lifeless to the ground.\n\r",victim->name);
	    act(buf,ch,0,0,TO_ROOM);
	    send_to_char(buf,ch);
    	    stop_embrace(ch,victim);
  	    if (victim->pcdata->stats[UNI_GEN] < ch->pcdata->stats[UNI_GEN] && ch->pcdata->stats[UNI_GEN] > 3)
  	    {
		sprintf(buf,"%s has been diablerized by %s.",victim->name,ch->name);
  		victim->pcdata->condition[COND_THIRST]=0;
  		do_info(char_list,buf);
	    }
  	else
  	{
  	  sprintf(buf,"%s has been diablerized by %s for no generation.", victim->name,ch->name);
  	  victim->pcdata->condition[COND_THIRST]=0;
  	  do_info(ch,buf);
  	}

}
continue;



      }
    return;
}
*/


/*
 * Update all chars, including mobs.
 * This function is performance sensitive.
 */
void char_update(void)
{
	CHAR_DATA *ch;
	CHAR_DATA *ch_next;
	CHAR_DATA *ch_save;
	CHAR_DATA *ch_quit;
	CHAR_DATA *wch = 0;
	bool is_obj = FALSE;
	bool drop_out = FALSE;
	time_t save_time;

	save_time = current_time;
	ch_save = 0;
	ch_quit = 0;
	for(ch = char_list; ch != 0; ch = ch_next)
	{
		AFFECT_DATA *paf;
		AFFECT_DATA *paf_next;

		ch_next = ch->next;

		if(!IS_NPC(ch) && (IS_HEAD(ch, LOST_HEAD) || IS_EXTRA(ch, EXTRA_OSWITCH)))
			is_obj = TRUE;
		else if(!IS_NPC(ch) && ch->pcdata->obj_vnum != 0)
		{
			is_obj = TRUE;
			SET_BIT(ch->extra, EXTRA_OSWITCH);
		}
		else
			is_obj = FALSE;
		/*
		 * Find dude with oldest save time.
		 */
		if(!IS_NPC(ch)
		   && (ch->desc == 0 || ch->desc->connected == CON_PLAYING)
		   && ch->level >= 2 && ch->save_time < save_time)
		{
			ch_save = ch;
			save_time = ch->save_time;
		}

		if(check_safe(ch))
			continue;

		if(!IS_NPC(ch))
		{
			if(ch->pcdata->safe_timer > 0)
				ch->pcdata->safe_timer--;
		}

		// check for misworn class equipment - pip. 9/14/01
		if (!IS_NPC(ch))
		{
			OBJ_DATA *obj;
			OBJ_DATA *obj_next;
			
			long cow = 0;			

			 for(obj = ch->carrying; obj != 0; obj = obj_next)
                	 {
                        	obj_next = obj->next_content;
				cow = 0;				

				
				if (obj->pIndexData->vnum >= 29709 && obj->pIndexData->vnum <= 29711)
				{
					SET_BIT(obj->spectype,SITEM_WW);
	                                REMOVE_BIT(obj->spectype,SITEM_MONK);
        	                        REMOVE_BIT(obj->spectype,SITEM_NINJA);
        	                        REMOVE_BIT(obj->spectype,SITEM_DROW);
        	                        REMOVE_BIT(obj->spectype,SITEM_DEMONIC);
					if (!IS_CLASS(ch,CLASS_WEREWOLF))
						cow = 1;
				}
		
				if (obj->pIndexData->vnum >= 29700 && obj->pIndexData->vnum <= 29702)
                                {
					REMOVE_BIT(obj->spectype,SITEM_WW);
                                	REMOVE_BIT(obj->spectype,SITEM_MONK);
                                	REMOVE_BIT(obj->spectype,SITEM_DROW);
                                	REMOVE_BIT(obj->spectype,SITEM_DEMONIC);
					SET_BIT(obj->spectype,SITEM_NINJA);
					if (!IS_CLASS(ch,CLASS_NINJA))
						cow = 1;
				}
				if (obj->pIndexData->vnum >= 29600 && obj->pIndexData->vnum <= 29606)
                                {
					REMOVE_BIT(obj->spectype,SITEM_WW);
                                	REMOVE_BIT(obj->spectype,SITEM_MONK);
                                	REMOVE_BIT(obj->spectype,SITEM_NINJA);
                                	REMOVE_BIT(obj->spectype,SITEM_DEMONIC);
					SET_BIT(obj->spectype,SITEM_DROW);
					if (!IS_CLASS(ch,CLASS_DROW))
						cow = 1;
				}
				if (obj->pIndexData->vnum >= 29706 && obj->pIndexData->vnum <= 29708)
                                {
					REMOVE_BIT(obj->spectype,SITEM_WW);
                                	REMOVE_BIT(obj->spectype,SITEM_NINJA);
                                	REMOVE_BIT(obj->spectype,SITEM_DROW);
                                	REMOVE_BIT(obj->spectype,SITEM_DEMONIC);
					SET_BIT(obj->spectype,SITEM_MONK);
					if (!IS_CLASS(ch,CLASS_MONK))
						cow = 1;
				}
				if (obj->pIndexData->vnum >= 29650 && obj->pIndexData->vnum <= 29663)
                                {
					REMOVE_BIT(obj->spectype,SITEM_WW);
                                	REMOVE_BIT(obj->spectype,SITEM_MONK);
                                	REMOVE_BIT(obj->spectype,SITEM_NINJA);
                                	REMOVE_BIT(obj->spectype,SITEM_DROW);
					SET_BIT(obj->spectype,SITEM_DEMONIC);
					if (!IS_CLASS(ch,CLASS_DEMON))
						cow = 1;
				}
				
				if (cow != 1)
					continue;
		
				if(obj->wear_loc != WEAR_NONE)
				{
					stc("We have fixed the class equipment bug, at your expense!\n\r",ch);
					unequip_char(ch,obj);
				}
                	}

		}		


		// drow hate
		if(!IS_NPC(ch) && IS_CLASS(ch, CLASS_DROW) && IS_SET(ch->newbits, NEW_DROWHATE) && number_percent() < 25)
		{
			if(ch->fighting && number_percent() < 50)
			{
				;
			}
			else
			{
				for(wch = ch->in_room->people; wch; wch = wch->next_in_room)
				{
					if(wch == ch || !can_see(ch, wch))
						continue;

					// attack a player if found!
					if(!IS_NPC(wch))
						break;

					// must attack something...
					if(wch->next_in_room == 0)
						break;
				}
			}

			if(wch)
			{
				// attack!
				send_to_char("Your hatred overcomes you.\n\r", ch);

				act("You lunge out in fury at $N!", ch, 0, wch, TO_CHAR);
				act("$n lunges out in a wild fury at you!", ch, 0, wch, TO_VICT);
				act("$n lunges out in a wild fury at $N!", ch, 0, wch, TO_NOTVICT);

				multi_hit(ch, wch, TYPE_UNDEFINED);
				continue;
			}
		}

		if(IS_CLASS(ch, CLASS_VAMPIRE) && ch->beast > 0 && ch->pcdata->condition[COND_THIRST] <= 15)
		{
			act("You bare your fangs and scream in rage from lack of blood.", ch, 0, 0, TO_CHAR);
			act("$n bares $s fangs and screams in rage.", ch, 0, 0, TO_ROOM);
			do_berserk(ch, "");
			do_beastlike(ch, "");
		}

		/* Add tick messages here..
		   if ( TIMER(ch, TIMER_WHATEVER) == 1 )
		   stc("Your whatever timer has worn off.\n\r",ch);
		 */
		if(!IS_NPC(ch))
		{
/* --> */ if(IS_CLASS(ch, CLASS_NINJA) && ch->pcdata->powers[NPOWER_CHIKYU] >= 6
	     && ch->pcdata->powers[HARA_KIRI] > 0)
			{
				if(ch->pcdata->powers[HARA_KIRI] == 1)
					send_to_char("Your HaraKiri wears off.\n\r", ch);
				ch->pcdata->powers[HARA_KIRI]--;
			}

		}

		if(ch->position > POS_STUNNED && !is_obj)
		{
			if(ch->hit < ch->max_hit)
			{
				ch->hit += hit_gain(ch);

				if(!IS_NPC(ch) && ch->hit >= ch->max_hit)
				{
					while(ch->pcdata->assist)
						remove_assist(ch, ch->pcdata->assist->ch);
				}
			}

			if(ch->mana < ch->max_mana)
				ch->mana += mana_gain(ch);

			if(ch->move < ch->max_move)
				ch->move += move_gain(ch);
		}

		if(ch->position == POS_STUNNED && !is_obj)
		{
			ch->hit = ch->hit + number_range(2, 4);
			update_pos(ch);
/*
            if (ch->position > POS_STUNNED)
            {
                act( "$n clambers back to $s feet.", ch, 0, 0, TO_ROOM );
                act( "You clamber back to your feet.", ch, 0, 0, TO_CHAR );
            }
*/
		}

		if(!IS_NPC(ch) && ch->level < LEVEL_IMMORTAL && !is_obj)
		{
			OBJ_DATA *obj;
			long blood;

			if(((obj = get_eq_char(ch, WEAR_WIELD)) != 0
			    && obj->item_type == ITEM_LIGHT
			    && obj->value[2] > 0)
			   || ((obj = get_eq_char(ch, WEAR_HOLD)) != 0
			       && obj->item_type == ITEM_LIGHT && obj->value[2] > 0))
			{
				if(--obj->value[2] == 0 && ch->in_room != 0)
				{
					--ch->in_room->light;
					act("$p goes out.", ch, obj, 0, TO_ROOM);
					act("$p goes out.", ch, obj, 0, TO_CHAR);
					extract_obj(obj);
				}
			}

			if(++ch->timer >= 32)
			{
				if(ch->was_in_room == 0 && ch->in_room != 0)
				{
					ch->was_in_room = ch->in_room;
					if(ch->fighting != 0)
						stop_fighting(ch, TRUE);
					act("$n disappears into the void.", ch, 0, 0, TO_ROOM);
					send_to_char("You disappear into the void.\n\r", ch);
					char_from_room(ch);
					char_to_room(ch, get_room_index(ROOM_VNUM_LIMBO));
				}
			}

			if(ch->timer > 30)
				ch_quit = ch;

			gain_condition(ch, COND_DRUNK, -1);
			if(!IS_CLASS(ch, CLASS_VAMPIRE))
			{
				gain_condition(ch, COND_FULL, -1);
				gain_condition(ch, COND_THIRST, -1);
			}
			else
			{
				blood = -1;
/* Vamps lose tons of blood when in zulo form - Vic */
				if(IS_POLYAFF(ch, POLY_ZULO))
					blood -= number_range(10, 25);
				if(ch->beast > 0)
				{
					if(IS_VAMPAFF(ch, VAM_CLAWS))
						blood -= number_range(1, 3);
					if(IS_VAMPAFF(ch, VAM_FANGS))
						blood -= 1;
					if(IS_VAMPAFF(ch, VAM_NIGHTSIGHT))
						blood -= 1;
					if(IS_VAMPAFF(ch, AFF_SHADOWSIGHT))
						blood -= number_range(1, 3);
					if(IS_SET(ch->act, PLR_HOLYLIGHT))
						blood -= number_range(1, 5);
					if(IS_VAMPAFF(ch, VAM_DISGUISED))
						blood -= number_range(5, 10);
					if(IS_VAMPAFF(ch, VAM_CHANGED))
						blood -= number_range(5, 10);
					if(IS_VAMPAFF(ch, IMM_SHIELDED))
						blood -= number_range(1, 3);
					if(IS_POLYAFF(ch, POLY_SERPENT))
						blood -= number_range(1, 3);
					if(ch->beast == 100)
						blood *= 2;
				}
				gain_condition(ch, COND_THIRST, blood);
			}
		}

		for(paf = ch->affected; paf; paf = paf_next)
		{
			if(!paf)
				return;

			paf_next = paf->next;
			if(paf->duration > 0)
				paf->duration--;
			else if(paf->duration < 0)
				;
			else
			{
				if(paf_next == 0 || paf_next->type != paf->type || paf_next->duration > 0)
				{
					if(paf->type > 0 && paf->type < MAX_SKILL && skill_table[paf->type].msg_off
					   && !is_obj)
					{
						send_to_char(skill_table[paf->type].msg_off, ch);
						send_to_char("\n\r", ch);
						if(IS_SET(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS))
							REMOVE_BIT(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS);
					}

				}
				affect_remove(ch, paf);
			}
		}

		/*
		 * Careful with the damages here,
		 *   MUST NOT refer to ch after damage taken,
		 *   as it may be lethal damage (on NPC).
		 */
		if(ch->loc_hp[6] > 0 && !is_obj && ch->in_room != 0)
		{
			long dam = 0;
			long minhit = 0;

			if(!IS_NPC(ch))
				minhit = -11;
			if(IS_BLEEDING(ch, BLEEDING_HEAD) && (ch->hit - dam) > minhit)
			{
				act("A spray of blood shoots from the stump of $n's neck.", ch, 0, 0, TO_ROOM);
				send_to_char("A spray of blood shoots from the stump of your neck.\n\r", ch);
				dam += number_range(20, 50);
			}
			if(IS_BLEEDING(ch, BLEEDING_THROAT) && (ch->hit - dam) > minhit)
			{
				act("Blood pours from the slash in $n's throat.", ch, 0, 0, TO_ROOM);
				send_to_char("Blood pours from the slash in your throat.\n\r", ch);
				dam += number_range(10, 20);
			}
			if(IS_BLEEDING(ch, BLEEDING_ARM_L) && (ch->hit - dam) > minhit)
			{
				act("A spray of blood shoots from the stump of $n's left arm.", ch, 0, 0, TO_ROOM);
				send_to_char("A spray of blood shoots from the stump of your left arm.\n\r", ch);
				dam += number_range(10, 20);
			}
			else if(IS_BLEEDING(ch, BLEEDING_HAND_L) && (ch->hit - dam) > minhit)
			{
				act("A spray of blood shoots from the stump of $n's left wrist.", ch, 0, 0, TO_ROOM);
				send_to_char("A spray of blood shoots from the stump of your left wrist.\n\r", ch);
				dam += number_range(5, 10);
			}
			if(IS_BLEEDING(ch, BLEEDING_ARM_R) && (ch->hit - dam) > minhit)
			{
				act("A spray of blood shoots from the stump of $n's right arm.", ch, 0, 0, TO_ROOM);
				send_to_char("A spray of blood shoots from the stump of your right arm.\n\r", ch);
				dam += number_range(10, 20);
			}
			else if(IS_BLEEDING(ch, BLEEDING_HAND_R) && (ch->hit - dam) > minhit)
			{
				act("A spray of blood shoots from the stump of $n's right wrist.", ch, 0, 0, TO_ROOM);
				send_to_char("A spray of blood shoots from the stump of your right wrist.\n\r", ch);
				dam += number_range(5, 10);
			}
			if(IS_BLEEDING(ch, BLEEDING_LEG_L) && (ch->hit - dam) > minhit)
			{
				act("A spray of blood shoots from the stump of $n's left leg.", ch, 0, 0, TO_ROOM);
				send_to_char("A spray of blood shoots from the stump of your left leg.\n\r", ch);
				dam += number_range(10, 20);
			}
			else if(IS_BLEEDING(ch, BLEEDING_FOOT_L) && (ch->hit - dam) > minhit)
			{
				act("A spray of blood shoots from the stump of $n's left ankle.", ch, 0, 0, TO_ROOM);
				send_to_char("A spray of blood shoots from the stump of your left ankle.\n\r", ch);
				dam += number_range(5, 10);
			}
			if(IS_BLEEDING(ch, BLEEDING_LEG_R) && (ch->hit - dam) > minhit)
			{
				act("A spray of blood shoots from the stump of $n's right leg.", ch, 0, 0, TO_ROOM);
				send_to_char("A spray of blood shoots from the stump of your right leg.\n\r", ch);
				dam += number_range(10, 20);
			}
			else if(IS_BLEEDING(ch, BLEEDING_FOOT_R) && (ch->hit - dam) > minhit)
			{
				act("A spray of blood shoots from the stump of $n's right ankle.", ch, 0, 0, TO_ROOM);
				send_to_char("A spray of blood shoots from the stump of your right ankle.\n\r", ch);
				dam += number_range(5, 10);
			}
			if(IS_HERO(ch))
			{
				ch->hit = ch->hit - dam;
				if(ch->hit < 1)
					ch->hit = 1;
			}
			else
				ch->hit = ch->hit - dam;
			update_pos(ch);
			ch->in_room->blood += dam;
			if(ch->in_room->blood > 1000)
				ch->in_room->blood = 1000;
			if(ch->hit <= -11 || (IS_NPC(ch) && ch->hit < 1))
			{


				do_killperson(ch, ch->name);
				drop_out = TRUE;
			}
		}

		if(IS_SET(ch->flag2, AFF2_ROT) && !is_obj && !drop_out)
		{
			long dam;

			if(IS_NPC(ch))
				continue;
			act("$n's flesh shrivels and tears.", ch, 0, 0, TO_ROOM);
			send_to_char("Your flesh shrivels and tears.\n\r", ch);
			dam = number_range(250, 500);
			ch->hit = ch->hit - dam;
			update_pos(ch);
			if(ch->hit < -10)
			{
				do_killperson(ch, ch->name);
				drop_out = TRUE;
			}
		}


		if(IS_AFFECTED(ch, AFF_FLAMING) && !is_obj && !drop_out && ch->in_room != 0)
		{
			long dam;

			if(!IS_NPC(ch) && IS_HERO(ch))
				continue;
			if(!IS_NPC(ch) && IS_IMMUNE(ch, IMM_HEAT) && !IS_CLASS(ch, CLASS_VAMPIRE))
				continue;
			act("$n's flesh burns and crisps.", ch, 0, 0, TO_ROOM);
			send_to_char("Your flesh burns and crisps.\n\r", ch);
			dam = number_range(50, 500);
			if(!IS_NPC(ch) && IS_IMMUNE(ch, IMM_HEAT))
				dam /= 2;
			if(!IS_NPC(ch) && IS_CLASS(ch, CLASS_VAMPIRE))
				dam *= 2;
			ch->hit = ch->hit - dam;
			update_pos(ch);
			if(ch->hit <= -11)
			{
				do_killperson(ch, ch->name);
				drop_out = TRUE;
			}
		}
		else if(IS_CLASS(ch, CLASS_VAMPIRE) && (!IS_AFFECTED(ch, AFF_SHADOWPLANE)) &&
			(!IS_NPC(ch) && !IS_IMMUNE(ch, IMM_SUNLIGHT)) && ch->in_room != 0 &&
			(!ch->in_room->sector_type == SECT_INSIDE) && !is_obj &&
			(!room_is_dark(ch->in_room)) && (weather_info.sunlight != SUN_DARK))
		{
			act("$n's flesh smolders in the sunlight!", ch, 0, 0, TO_ROOM);
			send_to_char("Your flesh smolders in the sunlight!\n\r", ch);
			/* This one's to keep Zarkas quiet ;) */
			if(IS_POLYAFF(ch, POLY_SERPENT))
				ch->hit = ch->hit - number_range(2, 4);
			else
				ch->hit = ch->hit - number_range(5, 10);
			update_pos(ch);
			if(ch->hit <= -11)
			{
				do_killperson(ch, ch->name);
				drop_out = TRUE;
			}
		}
		else if(IS_AFFECTED(ch, AFF_POISON) && !is_obj && !drop_out)
		{
			act("$n shivers and suffers.", ch, 0, 0, TO_ROOM);
			send_to_char("You shiver and suffer.\n\r", ch);
			damage(ch, ch, 2, gsn_poison);
		}
/*
	else if ( !IS_NPC( ch ) && ch->paradox[1] > 0 )
	{
	    if ( ch->paradox[1] > 2999 ) paradox( ch );
	    else if ( ch->paradox[2] == 0 && ch->paradox[1] > 0 )
	    {
		ch->paradox[1] --;
		ch->paradox[2] = PARADOX_TICK;
	    }
	    else ch->paradox[3] --;
	}
*/
		else if(ch->position == POS_INCAP && !is_obj && !drop_out)
		{
			if(ch->level > 0)
				ch->hit = ch->hit + number_range(2, 4);
			else
				ch->hit = ch->hit - number_range(1, 2);
			update_pos(ch);
			if(ch->position > POS_INCAP)
			{
				act("$n's wounds stop bleeding and seal up.", ch, 0, 0, TO_ROOM);
				send_to_char("Your wounds stop bleeding and seal up.\n\r", ch);
			}
			if(ch->position > POS_STUNNED)
			{
				act("$n clambers back to $s feet.", ch, 0, 0, TO_ROOM);
				send_to_char("You clamber back to your feet.\n\r", ch);
			}
		}
		else if(ch->position == POS_MORTAL && !is_obj && !drop_out)
		{
			drop_out = FALSE;
			/* if (ch->level > 0) */
			ch->hit = ch->hit + number_range(2, 4);
/*	    else
	    {
                ch->hit = ch->hit - number_range(1,2);
		if (!IS_NPC(ch) && (ch->hit <=-11))
		    do_killperson(ch,ch->name);
		drop_out = TRUE;
	    }*/
			if(!drop_out)
			{
				update_pos(ch);
				if(ch->position == POS_INCAP)
				{
					act("$n's wounds begin to close, and $s bones pop back into place.", ch, 0, 0,
					    TO_ROOM);
					send_to_char("Your wounds begin to close, and your bones pop back into place.\n\r",
						     ch);
				}
			}
		}
		else if(ch->position == POS_DEAD && !is_obj && !drop_out)
		{
			update_pos(ch);
			if(!IS_NPC(ch))
				do_killperson(ch, ch->name);
		}
		drop_out = FALSE;
	}

	/*
	 * Autosave and autoquit.
	 * Check that these chars still exist.
	 took out autoquit for now. - pip

	 if ( ch_save != 0 || ch_quit != 0 )
	 {
	 for ( ch = char_list; ch != 0; ch = ch_next )
	 {
	 ch_next = ch->next;
	 if ( ch == ch_save )
	 ;
	 //         save_char_obj( ch );
	 if ( ch == ch_quit && (IS_EXTRA(ch,EXTRA_SAVED)))
	 do_quit( ch, "" );
	 }
	 }
	 */

	return;
}


void quest(void)
{
	static const long rvnum[] = {
		109, 2210, 19192, 2396, 2429, 1477, 7900, 601,
		9103, 25008, 931, 9326, 9236, 2103, 2350
	};
	static const long ovnum[] = {
		200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
		210, 211
	};
	static const long mvnum[] = {
		200, 201
	};
	OBJ_DATA *obj;
	CHAR_DATA *ch;
	long rrandom;
	long orandom;
	long mrandom;

	rrandom = number_range(0, 14);
	orandom = number_range(0, 11);
	mrandom = number_range(0, 1);
	obj = create_object(get_obj_index(ovnum[orandom]), 100);
	ch = create_mobile(get_mob_index(mvnum[mrandom]));
	obj_to_char(obj, ch);
	char_to_room(ch, get_room_index(rvnum[rrandom]));
	SET_BIT(ch->special, SPC_NOFIND);
	SET_BIT(obj->quest, ITEM_EQUEST);
	obj->cost = 15;

	return;
}


/*
 * Update all objs.
 * This function is performance sensitive.
 */
void obj_update(void)
{
	long random_type = 0;
	CHAR_DATA *ch;
	char buf[MSL];
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	long x = 0;

	for(obj = object_list; obj != 0; obj = obj_next)
	{
		CHAR_DATA *rch;
		char *message;

		obj_next = obj->next;



		if(obj->timer <= 0 || --obj->timer > 0)
		{
			if(!str_cmp(obj->name, "token winner"))
			{
				if(obj->timer == 5 || obj->timer == 2)
				{
					sprintf(buf, "{g%s {xhas {r%li{x ticks left til it's gone!{x",
						obj->short_descr, obj->timer);
					do_info(char_list, buf);
				}
			}
			continue;
		}

//random quest token stuff! -pip
		if(!str_cmp(obj->name, "token winner"))
		{
			if((ch = obj->carried_by) == 0 || IS_NPC(ch))
			{
				sprintf(buf, "{g%s{x has been eaten!  Better luck next time!", obj->short_descr);
				do_info(char_list, buf);
				if(obj != 0)
					extract_obj(obj);
				continue;
			}
			sprintf(buf, "{W%s{x claims {G%s{x prize for being the victor!{x", ch->name,obj->short_descr);
			do_info(char_list, buf);
// check type of win, give them approriate stuff
			if(obj->value[1] == T_QUEST)
			{
				sprintf(buf, "You gain %li quest points.\n\r", obj->value[0]);
				ch->pcdata->quest += obj->value[0];
			}
			else if(obj->value[1] == T_PRIMAL)
			{
				sprintf(buf, "You gain %li primal points.\n\r", obj->value[0]);
				ch->practice += obj->value[0];
			}
			else if(obj->value[1] == T_EXP)
			{
				sprintf(buf, "You gain %li exp points.\n\r", obj->value[0]);
				ch->exp += obj->value[0];
			}
			else if(obj->value[1] == T_WPN_SKILL)
			{
				sprintf(buf, "You gain %li (random weapon) level points.\n\r", obj->value[0]);
				x = number_range(0, 12);
				if(ch->wpn[x] >= 1000)
				{
					stc("No gain... you already have 1000 in that.\n\r", ch);
					 obj_from_char(obj);
		                        if(obj != 0)
                		                extract_obj(obj);   
					return;
				}
				else
				{
					ch->wpn[x] += obj->value[0];
					ch->wpn[x] = ch->wpn[x] > 1000 ? 1000 : ch->wpn[x];
				}
			}
			else if(obj->value[1] == T_SPELL)
			{
				sprintf(buf, "You gain %li (random spell) level points.\n\r", obj->value[0]);
				x = number_range(0, 4);
				if(ch->spl[x] >= 240)
				{
					stc("No gain... you already have 240 in that.\n\r", ch);
					 obj_from_char(obj);
			                        if(obj != 0)
                        	        extract_obj(obj);   
					return;
				}
				else
				{
					ch->spl[x] += obj->value[0];
					ch->spl[x] = ch->spl[x] > 240 ? 240 : ch->spl[x];
				}
			}
			else if(obj->value[1] == T_STANCE)
			{
				sprintf(buf, "You gain %li (random stance) level points.\n\r", obj->value[0]);
				x = number_range(1, 10);
				if(ch->stance[x] >= 200)
				{
					stc("No gain... you already have 200 in that.\n\r", ch);
					 obj_from_char(obj);
			                        if(obj != 0)
                        	        extract_obj(obj);   
					return;
				}
				else
				{
					ch->stance[x] += obj->value[0];
					ch->stance[x] = ch->stance[x] > 200 ? 200 : ch->stance[x];
				}
			}
			else if(obj->value[1] == T_HP)
			{
				sprintf(buf, "You gain %li hit points.\n\r", obj->value[0]);
				ch->max_hit += obj->value[0];
				ch->max_hit = ch->max_hit > 30000 ? 30000 : ch->max_hit;
			}
			else if(obj->value[1] == T_MANA)
			{
				sprintf(buf, "You gain %li mana points.\n\r", obj->value[0]);
				ch->max_mana += obj->value[0];
				ch->max_mana = ch->max_mana > 30000 ? 30000 : ch->max_mana;
			}
			else if(obj->value[1] == T_MOVE)
			{
				sprintf(buf, "You gain %li move points.\n\r", obj->value[0]);
				ch->max_move += obj->value[0];
				ch->max_move = ch->max_move > 30000 ? 30000 : ch->max_move;
			}
			else
			{
				random_type = number_range(1, 100);
				if(random_type > 70)
				{
					stc("All you got was one measly quest point!\n\r", ch);
					ch->pcdata->quest += 1;
				}
				else if(random_type > 60)
					do_restore(ch, "all tokenrestoreforall");
				else if(random_type > 40)
					paradox(ch);
				else if(random_type > 30)
				{
					godgift(ch);
					sprintf(buf, "{B%s has been blessed by %s!{x", ch->name, ch->name);
					do_info(ch, buf);
				}
				else
				{
					godcurse(ch);
					sprintf(buf, "{R%s has been cursed by %s!{x", ch->name, ch->name);
					do_info(ch, buf);
				}

//and random stuff later, tired of typing.
			}
			if(obj->value[1] == T_RANDOM)
				sprintf(buf, "You are randomly zapped!\n\r");
			send_to_char(buf, ch);
			obj_from_char(obj);
			if(obj != 0)
				extract_obj(obj);
			continue;
		}

		switch (obj->item_type)
		{
		default:
			message = "$p vanishes.";
			break;
		case ITEM_FOUNTAIN:
			message = "$p dries up.";
			break;
		case ITEM_CORPSE_NPC:
			message = "$p decays into dust.";
			break;
		case ITEM_CORPSE_PC:
			message = "$p decays into dust.";
			break;
		case ITEM_FOOD:
			message = "$p decomposes.";
			break;
		case ITEM_TRASH:
			message = "$p crumbles into dust.";
			break;
		case ITEM_EGG:
			message = "$p cracks open.";
			break;
		case ITEM_EXTRA_ARM:
			message = "$p rots away.";
			break;
		case ITEM_WEAPON:
			message = "The poison on $p melts through it.";
			break;
		case ITEM_WALL:
			message = "$p flows back into the ground.";
			break;
		}

		if(obj->carried_by != 0)
		{
			act(message, obj->carried_by, obj, 0, TO_CHAR);
		}
		else if(obj->in_room != 0 && (rch = obj->in_room->people) != 0)
		{
			act(message, rch, obj, 0, TO_ROOM);
			act(message, rch, obj, 0, TO_CHAR);
		}

// borlak
		while(obj->item_type == ITEM_EXTRA_ARM)
		{
			OBJ_DATA *weapon;
			CHAR_DATA *ch;

			if((ch = obj->carried_by) == 0)
				break;

			// v0.... 1 = third,  2 = fourth
			if(obj->value[0] == 1)
			{
				if((weapon = get_eq_char(ch, WEAR_THIRD_ARM)) != 0)
				{
					obj_from_char(weapon);
					obj_to_char(weapon, ch);
				}
			}
			else if(obj->value[0] == 2)
			{
				if((weapon = get_eq_char(ch, WEAR_FOURTH_ARM)) != 0)
				{
					obj_from_char(weapon);
					obj_to_char(weapon, ch);
				}
			}
			break;
		}

		/* If the item is an egg, we need to create a mob and shell!
		 * KaVir
		 */
		if(obj && obj->item_type == ITEM_EGG)
		{
			CHAR_DATA *creature;
			OBJ_DATA *egg;

			if(get_mob_index(obj->value[0]) != 0)
			{
				if(obj->carried_by != 0 && obj->carried_by->in_room != 0)
				{
					creature = create_mobile(get_mob_index(obj->value[0]));
					char_to_room(creature, obj->carried_by->in_room);
				}
				else if(obj->in_room != 0)
				{
					creature = create_mobile(get_mob_index(obj->value[0]));
					char_to_room(creature, obj->in_room);
				}
				else
				{
					creature = create_mobile(get_mob_index(obj->value[0]));
					char_to_room(creature, get_room_index(ROOM_VNUM_HELL));
/*
		    obj->timer = 1;
		    continue;
*/
				}
				egg = create_object(get_obj_index(OBJ_VNUM_EMPTY_EGG), 0);
				egg->timer = 2;
				obj_to_room(egg, creature->in_room);
				act("$n clambers out of $p.", creature, obj, 0, TO_ROOM);
			}
			else if(obj->in_room != 0)
			{
				egg = create_object(get_obj_index(OBJ_VNUM_EMPTY_EGG), 0);
				egg->timer = 2;
				obj_to_room(egg, obj->in_room);
			}
		}
		if(obj != 0)
			extract_obj(obj);
	}

	return;
}



/*
 * Aggress.
 *
 * for each mortal PC
 *     for each mob in room
 *         aggress on some random PC
 *
 * This function takes 25% to 35% of ALL Merc cpu time.
 * Unfortunately, checking on each PC move is too tricky,
 *   because we don't want the mob to just attack the first PC
 *   who leads the party into the room.
 *
 * -- Furey
 */
void aggr_update(void)
{
	CHAR_DATA *wch;
	CHAR_DATA *wch_next = 0;
	CHAR_DATA *ch;
	CHAR_DATA *ch_next;
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;
	CHAR_DATA *victim = 0;

	OBJ_DATA *obj = 0;
	OBJ_DATA *chobj = 0;
	ROOM_INDEX_DATA *objroom = 0;
	DESCRIPTOR_DATA *d;

	for(d = descriptor_list; d != 0; d = d->next)
	{
		if(d->connected == CON_PLAYING
		   && (ch = d->character) != 0 && !IS_NPC(ch) && ch->pcdata != 0 && (obj = ch->pcdata->chobj) != 0)
		{
			if(obj->in_room != 0)
				objroom = obj->in_room;
			else if(obj->in_obj != 0)
				objroom = get_room_index(ROOM_VNUM_IN_OBJECT);
			else if(obj->carried_by != 0)
			{
				if(obj->carried_by != ch && obj->carried_by->in_room != 0)
					objroom = obj->carried_by->in_room;
				else
					continue;
			}
			else
				continue;
			if(ch->in_room != objroom && objroom != 0)
			{
				char_from_room(ch);
				char_to_room(ch, objroom);
				do_look(ch, "auto");
			}
		}
		else if(d->connected == CON_PLAYING
			&& (ch = d->character) != 0
			&& !IS_NPC(ch)
			&& ch->pcdata != 0
			&& (IS_HEAD(ch, LOST_HEAD) || IS_EXTRA(ch, EXTRA_OSWITCH) || ch->pcdata->obj_vnum != 0))
		{
			if(ch->pcdata->obj_vnum != 0)
			{
				bind_char(ch);
				continue;
			}
			if(IS_HEAD(ch, LOST_HEAD))
			{
				REMOVE_BIT(ch->loc_hp[0], LOST_HEAD);
				send_to_char("You are able to regain a body.\n\r", ch);
				ch->position = POS_RESTING;
				ch->hit = 1;
			}
			else
			{
				send_to_char("You return to your body.\n\r", ch);
				REMOVE_BIT(ch->extra, EXTRA_OSWITCH);
			}
			REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
			free_string(ch->morph);
			ch->morph = str_dup("");
			char_from_room(ch);
			char_to_room(ch, get_room_index(ROOM_VNUM_ALTAR));
			if((chobj = ch->pcdata->chobj) != 0)
				chobj->chobj = 0;
			ch->pcdata->chobj = 0;
			do_look(ch, "auto");
		}
		continue;
	}

	for(wch = char_list; wch != 0; wch = wch_next)
	{
		if(!wch_next)
			break;

		wch_next = wch->next;


		if(IS_NPC(wch)
		   || (wch->desc != 0 && wch->desc->connected != CON_PLAYING)
		   || wch->position <= POS_STUNNED
		   || wch->level >= LEVEL_IMMORTAL
		   || wch->pcdata == 0 || ((chobj = wch->pcdata->chobj) != 0) || wch->in_room == 0)
			continue;

		for(ch = wch->in_room->people; ch != 0; ch = ch_next)
		{
			long count;

			ch_next = ch->next_in_room;

			if(!IS_NPC(ch)
			   || !IS_SET(ch->act, ACT_AGGRESSIVE)
			   || no_attack(ch, wch)
			   || ch->fighting != 0
			   || IS_AFFECTED(ch, AFF_CHARM)
			   || !IS_AWAKE(ch) || (IS_SET(ch->act, ACT_WIMPY) && IS_AWAKE(wch)) || !can_see(ch, wch))
				continue;

			/*
			 * Ok we have a 'wch' player character and a 'ch' npc aggressor.
			 * Now make the aggressor fight a RANDOM pc victim in the room,
			 *   giving each 'vch' an equal chance of selection.
			 */
			count = 0;
			victim = 0;
			for(vch = wch->in_room->people; vch != 0; vch = vch_next)
			{
				vch_next = vch->next_in_room;

				if(!IS_NPC(vch)
				   && !no_attack(ch, vch)
				   && vch->pcdata != 0
				   && ((chobj = vch->pcdata->chobj) == 0)
				   && vch->level < LEVEL_IMMORTAL
				   && vch->position > POS_STUNNED
				   && (!IS_SET(ch->act, ACT_WIMPY) || !IS_AWAKE(vch)) && can_see(ch, vch))
				{
					if(number_range(0, count) == 0)
						victim = vch;
					count++;
				}
			}

			if(victim == 0)
			{
/*
		bug( "Aggr_update: null victim attempt by mob %li.", ch->pIndexData->vnum );
*/
				continue;
			}
			if(IS_EXTRA(victim, EXTRA_SAVED))
				multi_hit(ch, victim, TYPE_UNDEFINED);
		}
	}

	return;
}


/*added by sage for embrace*/

void embrace_update(void)
{
	DESCRIPTOR_DATA *d;
	CHAR_DATA *ch;
	CHAR_DATA *victim;
	long blpr;		/* variable to check for amout of blood sucked. Shakti */
	char buf[MAX_STRING_LENGTH];

	for(d = descriptor_list; d != 0; d = d->next)
	{

		if((ch = d->character) == 0)
			continue;


		if(IS_NPC(ch) || ch->embracing == 0)
			continue;

		if(!IS_CLASS(ch, CLASS_VAMPIRE))
			stop_embrace(ch, 0);

		victim = ch->embracing;
		if(victim == 0)
			continue;
		if(victim->embraced == 0)
		{
			ch->embracing = 0;
			continue;
		}
		/* Fix for embracing mobs by Shakti.    */

		if(IS_NPC(victim))
		{
			if(victim->level > 75)
			{
				stop_embrace(ch, victim);
				send_to_char("You cannot embrace someone so powerful!\n\r", ch);
				return;
			}

			/* To keep how much blood was sucked, and how much gained the same. Shakti */

			(blpr = number_range(30, 40));
			victim->practice -= blpr;
			ch->pcdata->condition[COND_THIRST] += blpr;

			sprintf(buf, "%s shudders in ecstacy as he drinks blood from %s's neck.\n\r", ch->name,
				victim->short_descr);
			act(buf, ch, 0, 0, TO_ROOM);
			sprintf(buf, "You shudder in ecstacy as you drink blood from %s's neck.\n\r", victim->short_descr);
			send_to_char(buf, ch);

			if(ch->pcdata->condition[COND_THIRST] > (2000 / ch->pcdata->stats[UNI_GEN]))
			{
				stc("Your blood thirst is sated.\n\r", ch);
				ch->pcdata->condition[COND_THIRST] = (2000 / ch->pcdata->stats[UNI_GEN]);
			}

			if(victim->practice < 0)
			{
				victim->practice = 0;
			}

			if(victim->practice == 0)

			{
				raw_kill(victim);
				sprintf(buf, "%s's body falls lifless to the ground!.\n\r", victim->short_descr);
				act(buf, ch, 0, 0, TO_ROOM);
				send_to_char(buf, ch);
				stop_embrace(ch, victim);
				return;
			}

			continue;
		}



		if(get_char_world(ch, victim->name) == 0)
		{
			stop_embrace(ch, 0);
			continue;
		}
		if(ch->in_room != victim->in_room)
			stop_embrace(ch, victim);
		if(victim->pcdata->condition[COND_THIRST] < 0)
			victim->pcdata->condition[COND_THIRST] = 0;

		ch->pcdata->condition[COND_THIRST] += number_range(30, 40);
		sprintf(buf, "%s shudders in ecstacy as he drinks blood from %s's neck.\n\r", ch->name, victim->name);
		act(buf, ch, 0, 0, TO_ROOM);
		sprintf(buf, "You shudder in ecstacy as you drink blood from %s's neck.\n\r", victim->name);
		send_to_char(buf, ch);
		sprintf(buf, "You feel some of your life slip away as %s drinks from your neck.\n\r", ch->name);
		send_to_char(buf, victim);
		ch->pcdata->condition[COND_THIRST] += number_range(35, 40);
		victim->pcdata->condition[COND_THIRST] -= number_range(40, 42);
		if(ch->pcdata->condition[COND_THIRST] > 2000 / ch->pcdata->stats[UNI_GEN]);

		{
			ch->pcdata->condition[COND_THIRST] = 2000 / ch->pcdata->stats[UNI_GEN];
			send_to_char("Your bloodlust is sated.\n\r", ch);
		}
		if(victim->pcdata->condition[COND_THIRST] < 0)
			victim->pcdata->condition[COND_THIRST] = 0;
		if(victim->pcdata->condition[COND_THIRST] == 0)
		{
			sprintf(buf, "You have been diablerized!\n\r");
			send_to_char(buf, victim);
			raw_kill(victim);
			sprintf(buf, "%s's body falls lifeless to the ground.\n\r", victim->name);
			act(buf, ch, 0, 0, TO_ROOM);
			send_to_char(buf, ch);
			stop_embrace(ch, victim);
			if(victim->pcdata->stats[UNI_GEN] < ch->pcdata->stats[UNI_GEN] && ch->pcdata->stats[UNI_GEN] > 3)
			{
				sprintf(buf, "%s has been diablerized by %s.", victim->name, ch->name);
				victim->pcdata->condition[COND_THIRST] = 0;
				do_info(ch, buf);
			}
			else
			{
				sprintf(buf, "%s has been diablerized by %s for no generation.", victim->name, ch->name);
				victim->pcdata->condition[COND_THIRST] = 0;
				do_info(ch, buf);
			}
		}
		continue;
	}
	return;
}



void mud_update(void)
{
/*	CHAR_DATA *ch;
	CHAR_DATA *ch_next;
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;

	for( ch = char_list; ch; ch = ch_next )
	{
		ch_next = ch->next;

		if( !IS_NPC(ch) )
			continue;

		extract_char(ch,TRUE);
	}

	ch = ch_next = 0;

	for( ch = char_free; ch; ch = ch_next )
	{
		ch_next = ch->next;

		free_mem(ch,sizeof(CHAR_DATA));
	}

	char_free = 0;

	for( obj = object_list; obj != 0; obj = obj_next )
	{
		obj_next = obj->next;

		if( (obj->carried_by && !IS_NPC(obj->carried_by))
		||  (obj->questowner != 0 && obj->questowner[0] != '\0')
		||  obj->in_obj != 0 )
			continue;

		extract_obj(obj);
	}

	obj = obj_next = 0;

	for( obj = obj_free; obj != 0; obj = obj_next )
	{
		obj_next = obj->next;

		free_mem(obj,sizeof(OBJ_DATA));
	}

	obj_free = 0;
*/
}


void ww_update(void)
{
	DESCRIPTOR_DATA *d;
	CHAR_DATA *victim;
	long dam = 0;

	for(d = descriptor_list; d != 0; d = d->next)
	{
		if(!IS_PLAYING(d) || (victim = d->character) == 0
		   || IS_NPC(victim) || IS_IMMORTAL(victim)
		   || victim->in_room == 0 || victim->pcdata->chobj != 0 || IS_CLASS(victim, CLASS_WEREWOLF))
		{
			continue;
		}
		if(!IS_SET(d->character->in_room->room_flags, ROOM_BLADE_BARRIER))
			continue;

		act("The scattered blades on the ground fly up into the air ripping into you.", d->character, 0, 0,
		    TO_CHAR);
		act("The scattered blades on the ground fly up into the air ripping into $n.", d->character, 0, 0,
		    TO_ROOM);

		act("The blades drop to the ground inert.", d->character, 0, 0, TO_CHAR);
		act("The blades drop to the ground inert.", d->character, 0, 0, TO_ROOM);

		dam = number_range(7, 14);
		dam = dam / 100;
		dam = d->character->hit * dam;
		if(dam < 100)
			dam = 100;
		d->character->hit = d->character->hit - dam;
		if(d->character->hit < -10)
			d->character->hit = -10;
		update_pos(victim);
	}

	return;
}

void ping_update(void)
{
	static long i = 0;

	if(i++ % 2 == 0)
		get_player_pings();
	else
		ping_players();
}

void second_update(void)
{
	DESCRIPTOR_DATA *d;
	struct timeval now_time;
	long totsec;

	get_time(&now_time);
	totsec = (time_t) now_time.tv_sec;

	for(d = descriptor_list; d != 0; d = d->next)
	{
		CHAR_DATA *y2k;

		if(d->connected != CON_PLAYING)
			continue;

		y2k = (d->original != 0) ? d->original : d->character;

		if(IS_NPC(y2k))
			continue;

		if(IS_IMMORTAL(y2k) && totsec == 946601000)
		{
			send_to_char("{WTEST TEST TEST TEST TEST TEST TEST{x\n\r", y2k);
			send_to_char("lets just say it's almost time for y2k.\n\r", y2k);
			continue;
		}

		if(totsec < 946706099 || totsec > 946706411)
			return;


		if(totsec == 946706410)
		{
			send_to_char("You notice some improvements in your quest points as a y2k bug hits!\n\r", y2k);
			y2k->pcdata->quest += 2000;
			if(y2k->max_hit < 28000)
			{
				send_to_char("You notice some improvements in your hit points as another y2k bug hits!\n\r",
					     y2k);
				y2k->max_hit += 2000;
			}
			else
				y2k->pcdata->quest += 2000;
			if(y2k->max_mana < 28000)
			{
				send_to_char
					("You notice some improvements in your mana points as another y2k bug hits!\n\r",
					 y2k);
				y2k->max_mana += 2000;
			}
			else
				y2k->pcdata->quest += 2000;
			if(y2k->max_move < 28000)
			{
				send_to_char
					("You notice some improvements in your move points as another y2k bug hits!\n\r",
					 y2k);
				y2k->max_move += 2000;
			}
			else
				y2k->pcdata->quest += 2000;
			continue;
		}

		if(totsec == 946706400)
		{
			do_time(y2k, "");
			do_help(y2k, "flag");
			continue;
		}

		if(totsec == 946706399)
		{
			send_to_char("ONE second left!  AHHH!\n\r", y2k);
			continue;
		}

		if(totsec == 946706398)
		{
			send_to_char("TWO seconds left!  So damn close!\n\r", y2k);
			continue;
		}
		if(totsec == 946706397)
		{
			send_to_char("THREE seconds to go!  {WWoot!{x\n\r", y2k);
			continue;
		}
		if(totsec == 946706396)
		{
			send_to_char("FOUR more seconds!\n\r", y2k);
			continue;
		}
		if(totsec == 946706395)
		{
			send_to_char("FIVE TO GO!  Almost there!\n\r", y2k);
			continue;
		}
		if(totsec == 946706394)
		{
			send_to_char("SIX left!\n\r", y2k);
			continue;
		}
		if(totsec == 946706393)
		{
			send_to_char("SEVEN seconds til core meltdown!\n\r", y2k);
			continue;
		}
		if(totsec == 946706392)
		{
			send_to_char("EIGHT more til the millenium!\n\r", y2k);
			continue;
		}
		if(totsec == 946706391)
		{
			send_to_char("NINE seconds to go!\n\r", y2k);
			continue;
		}
		if(totsec == 946706390)
		{
			send_to_char("ONLY 10 MORE SECONDS TIL WE ALL BURN IN HELL!\n\r", y2k);
			continue;
		}
		if(totsec == 946706380)
		{
			send_to_char("20 SECOND WARNING for Y2K\n\r", y2k);
			continue;
		}
		if(totsec == 946706370)
		{
			send_to_char("30 SECOND WARNING for Y2K\n\r", y2k);
			continue;
		}
		if(totsec == 946706360)
		{
			send_to_char("40 SECOND WARNING for Y2K\n\r", y2k);
			continue;
		}
		if(totsec == 946706350)
		{
			send_to_char("50 SECOND WARNING for Y2K\n\r", y2k);
			continue;
		}
		if(totsec == 946706340)
		{
			send_to_char("ONLY 1 MINUTE left til we hit {WY2K{x!\n\r", y2k);
			continue;
		}
		if(totsec == 946706280)
		{
			send_to_char("2 Minutes left til Y2K hits.\n\r", y2k);
			continue;
		}
		if(totsec == 946706220)
		{
			send_to_char("3 mins left til y2K.  check your flashlight.\n\r", y2k);
			continue;
		}
		if(totsec == 946706160)
		{
			send_to_char("4 minutes left til y2k!  hide under your bed til it's over!\n\r", y2k);
			continue;
		}
		if(totsec == 946706110)
		{
			send_to_char
				("SYSTEM:  Diagnostics check complete.  We are ready.  Sit back and relax.  4 mins 50 seconds left.\n\r",
				 y2k);
			continue;
		}
		if(totsec == 946706100)
		{
			send_to_char
				("SYSTEM: 5 minutes left til the millenium.  Systems check will run a diagnostic now.\n\r",
				 y2k);
			continue;
		}
		continue;
	}
}



/*

 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */
void update_handler(void)
{
	static long pulse_code;
	static long pulse_insult;
	static long pulse_kingdom;
	static long pulse_update;
	static long pulse_area;
	static long pulse_mobile;
	static long pulse_violence;
	static long pulse_point;
	static long pulse_ww;
	static long pulse_vote;

        static  long     pulse_ping;
	static long pulse_tournament;
	static long pulse_second;
	static long pulse_token;

	if(--pulse_second <= 0)
	{
		pulse_second = PULSE_PER_SECOND;
		second_update();
	}

	if(--pulse_token <= 0)
	{
		pulse_token = (PULSE_PER_SECOND * 60) * number_range(15, 60);
		token_update("no");
	}

	if(--pulse_insult <= 0)
	{
		pulse_insult = number_range(PULSE_PER_SECOND * 60 * 5, PULSE_PER_SECOND * 60 * 30);
		insult_update("no");
	}

	if(--pulse_code <= 0)
	{
		pulse_code = number_range(PULSE_PER_SECOND * 60 * 15, PULSE_PER_SECOND * 60 * 60);
		code_update("");
	}

	if(--pulse_tournament <= 0)
	{
		pulse_tournament = PULSE_PER_SECOND * 60;
		tournament_update();
	}

	if(--pulse_kingdom <= 0)
	{
		pulse_kingdom = 125;
		update_kingdoms();
	}
// comment this out if problems arise - pip

	if ( --pulse_ping	  <= 0 )
	{
		pulse_ping	= PULSE_PER_SECOND * 2;
		ping_update	( );
	}

	if(--pulse_vote <= 0)
	{
		pulse_vote = PULSE_PER_SECOND * 5;
		vote_update();
		who_html_update();
		trivia_update();
	}

	if(--pulse_ww <= 0)
	{
		pulse_ww = PULSE_WW;
		ww_update();
	}

	if(--pulse_update <= 0)
	{
		pulse_update = (PULSE_PER_SECOND * 60) * 30;
		reset_mud = TRUE;
		mud_update();
		reset_mud = FALSE;
		pulse_area = 0;
	}

	if(--pulse_area <= 0)
	{
		pulse_area = number_range(PULSE_AREA / 2, 3 * PULSE_AREA / 2);
		area_update();
	}

	if(--pulse_mobile <= 0)
	{
		pulse_mobile = PULSE_MOBILE;
		mobile_update();
	}

	if(--pulse_violence <= 0)
	{
		pulse_violence = PULSE_VIOLENCE;
		violence_update();
		limbo_update();
//      bard_update     ( );
		embrace_update();
	}

	if(--pulse_point <= 0)
	{
		pulse_point = number_range(PULSE_TICK / 2, 3 * PULSE_TICK / 2);
		weather_update();
		char_update();
		obj_update();
	}

	aggr_update();
	tail_chain();
	return;
}

// THIS IS THE END OF THE FILE THANKS
