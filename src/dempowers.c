/*Demon's Sage/Dunkirk*/

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

void frost_breath args((CHAR_DATA * ch, CHAR_DATA * victim, bool all));
void fire_effect args((void *vo, long level, long dam, long target));

DECLARE_DO_FUN(do_stance);

void fire_effect(void *vo, long level, long dam, long target)
{
	if(target == TARGET_ROOM)	/* nail objects on the floor */
	{
		ROOM_INDEX_DATA *room = (ROOM_INDEX_DATA *) vo;
		OBJ_DATA *obj, *obj_next;

		for(obj = room->contents; obj != 0; obj = obj_next)
		{
			obj_next = obj->next_content;
			fire_effect(obj, level, dam, TARGET_OBJ);
		}
		return;
	}

	if(target == TARGET_CHAR)	/* do the effect on a victim */
	{
		CHAR_DATA *victim = (CHAR_DATA *) vo;
		OBJ_DATA *obj, *obj_next;

		/* chance of blindness */
		if(!IS_AFFECTED(victim, AFF_BLIND))
		{
			AFFECT_DATA af;

			act("$n is blinded by smoke!", victim, 0, 0, TO_ROOM);
			act("Your eyes tear up from smoke...you can't see a thing!", victim, 0, 0, TO_CHAR);

			af.type = skill_lookup("fire breath");
			af.duration = number_range(0, level / 10);
			af.location = APPLY_HITROLL;
			af.modifier = -20;
			af.bitvector = AFF_BLIND;

			affect_to_char(victim, &af);
		}

		/* let's toast some gear! */
		for(obj = victim->carrying; obj != 0; obj = obj_next)
		{
			obj_next = obj->next_content;
			fire_effect(obj, level, dam, TARGET_OBJ);
		}
		return;
	}

	if(target == TARGET_OBJ)	/* toast an object */
	{
		OBJ_DATA *obj = (OBJ_DATA *) vo;
		OBJ_DATA *t_obj, *n_obj;
		long chance;
		char *msg;

		chance = level / 4 + dam / 10;

		if(chance > 25)
			chance = (chance - 25) / 2 + 25;
		if(chance > 50)
			chance = (chance - 50) / 2 + 50;

		if(IS_OBJ_STAT(obj, ITEM_BLESS))
			chance -= 5;
		chance -= obj->level * 2;

		switch (obj->item_type)
		{
		default:
			return;
		case ITEM_CONTAINER:
			msg = "$p ignites and burns!";
			break;
		case ITEM_POTION:
			chance += 25;
			msg = "$p bubbles and boils!";
			break;
		case ITEM_SCROLL:
			chance += 50;
			msg = "$p crackles and burns!";
			break;
		case ITEM_STAFF:
			chance += 10;
			msg = "$p smokes and chars!";
			break;
		case ITEM_WAND:
			msg = "$p sparks and sputters!";
			break;
		case ITEM_FOOD:
			msg = "$p blackens and crisps!";
			break;
		case ITEM_PILL:
			msg = "$p melts and drips!";
			break;
		}

		chance = URANGE(5, chance, 95);

		if(number_percent() > chance)
			return;

		if(IS_SET(obj->quest, QUEST_SPELLPROOF))
			return;

		if(obj->carried_by != 0)
			act(msg, obj->carried_by, obj, 0, TO_ALL);
		else if(obj->in_room != 0 && obj->in_room->people != 0)
			act(msg, obj->in_room->people, obj, 0, TO_ALL);

		if(obj->contains)
		{
			/* dump the contents */

			for(t_obj = obj->contains; t_obj != 0; t_obj = n_obj)
			{
				n_obj = t_obj->next_content;
				obj_from_obj(t_obj);
				if(obj->in_room != 0)
					obj_to_room(t_obj, obj->in_room);
				else if(obj->carried_by != 0)
					obj_to_room(t_obj, obj->carried_by->in_room);
				else
				{
					extract_obj(t_obj);
					continue;
				}
				fire_effect(t_obj, level / 2, dam / 2, TARGET_OBJ);
			}
		}

		extract_obj(obj);
		return;
	}
}


void do_dinferno(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *vch, *vch_next;

/*    char buf[MAX_STRING_LENGTH];    */

	if(!IS_DEMPOWER(ch, DEM_INFERNO))
	{
		send_to_char("You have not been granted that power.\n\r", ch);
		return;
	}
/*
    if ( !TIME_UP(ch, TIMER_INFERNO) )
    {
	sprintf(buf,"You cannot use Inferno for another %li
hours.\n\r",ch->tick_timer[TIMER_INFERNO] );
	stc(buf,ch);
	return;
    }
*/
	if(ch->hit > 0)
	{
		send_to_char("This is only used as a last resort.\n\r", ch);
		return;
	}

	act("$n explodes, sending forth a massive fireballs in all directions.", ch, 0, 0, TO_ROOM);
	send_to_char("You explode in a huge blast of abyssal flame.\n\r", ch);
	for(vch = ch->in_room->people; vch != 0; vch = vch_next)
	{
		vch_next = vch->next_in_room;
		if(vch != ch)
		{
			damage(vch, ch, number_range(1500, 2000), gsn_inferno);
			damage(ch, ch, number_range(1500, 2000), gsn_inferno);
			fire_effect(vch, ch->level, 2000, TARGET_CHAR);
		}
	}
	WAIT_STATE(ch, 56);
	ch->hit = 10;
	update_pos(ch);
/*    SET_TIMER(ch, TIMER_INFERNO, 80);*/
}

void do_immolate(CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;

	if(!IS_DEMPOWER(ch, DEM_IMMOLATE))
	{
		stc("You have not been granted that power.\n\r", ch);
		return;
	}

	if(argument[0] == '\0')
	{
		send_to_char("Which item do you wish to immolate?\n\r", ch);
		return;
	}

	if((obj = get_obj_carry(ch, argument)) == 0)
	{
		send_to_char("You are not carrying that item.\n\r", ch);
		return;
	}

	if(obj->item_type != ITEM_WEAPON)
	{
		send_to_char("That is not a weapon!\n\r", ch);
		return;
	}

	if(dice(1, 100) == 1)
	{
		act("$p explodes in a burst of flames.", ch, obj, 0, TO_ROOM);
		act("$p explodes in a burst of flames.", ch, obj, 0, TO_CHAR);
		damage(ch, ch, 500, gsn_inferno);
		extract_obj(obj);
		return;
	}

	if(IS_WEAP(obj, WEAPON_FLAMING))
	{
		act("$p is already flaming.", ch, obj, 0, TO_CHAR);
		return;
	}

	act("$p bursts into flames.", ch, obj, 0, TO_CHAR);
	act("$p, carried by $n bursts into flames.", ch, obj, 0, TO_ROOM);

	WAIT_STATE(ch, 8);
	SET_BIT(obj->weapflags, WEAPON_FLAMING);
}


void do_leech(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim = 0;
	char buf[MAX_STRING_LENGTH];

	if(!IS_DEMPOWER(ch, DEM_LEECH))
	{

		stc("You do not have that power.\n\r", ch);
		return;
	}

	if(argument[0] == '\0')
	{
		stc("Who's life do you wish to leech off of?\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, argument)) == 0 && (victim = ch->fighting) == 0)
	{
		stc("They aren't here.\n\r", ch);
		return;
	}

	if(is_safe(ch, victim))
		return;

	WAIT_STATE(ch, 24);

	act("$n stares intently at $N.", ch, 0, victim, TO_NOTVICT);
	act("You stare intently at $N.", ch, 0, victim, TO_CHAR);
	act("$n stares intently at you.", ch, 0, victim, TO_VICT);

	if(victim->hit >= 1000)
	{
		long dam;

		dam = number_range(500, 1500);

		if(victim->hit - dam <= 0)
		{
			stc("Nothing seemed to happen.\n\r", ch);
			return;
		}

		victim->hit -= dam;
		ch->hit += dam;
		if(ch->hit > ch->max_hit)
			ch->hit = ch->max_hit;

		sprintf(buf, "You absorb %li hitpoints.\n\r", dam);
		stc(buf, ch);
		sprintf(buf, "%s absorbed %li of your hitpoints!\n\r", ch->name, dam);
		stc(buf, victim);
		if(!ch->fighting)
			set_fighting(ch, victim);
	}
	else
		stc("Nothing seemed to happen.\n\r", ch);
	return;

}

void do_unnerve(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;

	if(!IS_DEMPOWER(ch, DEM_UNNERVE))
	{
		stc("You have not been granted that power.\n\r", ch);
		return;
	}

	if(argument[0] == '\0')
	{
		stc("Who do you wish to unnerve?\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, argument)) == 0)
	{
		stc("They aren't here.\n\r", ch);
		return;
	}

	WAIT_STATE(ch, 7);
	do_say(ch, "#1Xeus Dominus Mortai!#n");
	do_stance(victim, "");
}

void do_wfreeze(CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;

	if(!IS_DEMPOWER(ch, DEM_FREEZEWEAPON))
	{
		stc("You have not been granted that power.\n\r", ch);
		return;
	}

	if(argument[0] == '\0')
	{
		send_to_char("Which item do you wish to freeze?\n\r", ch);
		return;
	}

	if((obj = get_obj_carry(ch, argument)) == 0)
	{
		send_to_char("You are not carrying that item.\n\r", ch);
		return;
	}

	if(obj->item_type != ITEM_WEAPON)
	{
		send_to_char("That is not a weapon!\n\r", ch);
		return;
	}

	if(dice(1, 100) == 1)
	{
		act("$p freezes and shatters.", ch, obj, 0, TO_ROOM);
		act("$p freezes and shatters.", ch, obj, 0, TO_CHAR);
		extract_obj(obj);
		return;
	}

	if(IS_WEAP(obj, WEAPON_FROST))
	{
		act("$p is already frozen.", ch, obj, 0, TO_CHAR);
		return;
	}

	act("$p is surrounded by ice crystals.", ch, obj, 0, TO_CHAR);
	act("$p, carried by $n is surrounded by ice crystals.", ch, obj, 0, TO_ROOM);

	WAIT_STATE(ch, 8);
	SET_BIT(obj->weapflags, WEAPON_FROST);
}


void do_graft(CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DEMON))
	{
		send_to_char("Demons only.\n\r", ch);
		return;
	}

	if(ch->pcdata->stats[UNI_GEN] > 2 || ch->pcdata->stats[UNI_GEN] < 1)
	{
		stc("You need to be an Apprentice demon before you can graft.\n\r", ch);
		return;
	}

	if(arg[0] == '\0')
	{
		stc("Which limb do you wish to graft on to yourself?\n\r", ch);
		return;
	}

	if((obj = get_obj_carry(ch, arg)) == 0)
	{
		stc("You do not have that limb.\n\r", ch);
		return;
	}

	if(obj->pIndexData->vnum != OBJ_VNUM_SLICED_ARM)
	{
		stc("That's not even an arm!\n\r", ch);
		return;
	}

	if(!has_arm(ch, 1))
	{
		act("You graft an arm onto your body.", ch, 0, obj, TO_CHAR);
		act("$n grafts an arm onto $m body.", ch, 0, obj, TO_ROOM);
		WAIT_STATE(ch, 18);
		obj->item_type = ITEM_EXTRA_ARM;
		obj->value[0] = 1;
		if(!obj->timer)
			obj->timer = 200;
		obj->wear_loc = -2;
		return;
	}

	if(!has_arm(ch, 2))
	{
		act("You graft an arm onto your body.", ch, 0, obj, TO_CHAR);
		act("$n grafts an arm onto $m body.", ch, 0, obj, TO_ROOM);
		WAIT_STATE(ch, 18);
		obj->item_type = ITEM_EXTRA_ARM;
		obj->value[0] = 2;
		if(!obj->timer)
			obj->timer = 200;
		obj->wear_loc = -2;
		return;
	}

	stc("You already have four arms!\n\r", ch);

	return;
}


void do_caust(CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;

	if(!IS_DEMPOWER(ch, DEM_CAUST))
	{
		stc("You have not been granted that power.\n\r", ch);
		return;
	}

	if(argument[0] == '\0')
	{
		send_to_char("Which item do you wish to caust?\n\r", ch);
		return;
	}

	if((obj = get_obj_carry(ch, argument)) == 0)
	{
		send_to_char("You are not carrying that item.\n\r", ch);
		return;
	}

	if(obj->item_type != ITEM_WEAPON)
	{
		send_to_char("That is not a weapon!\n\r", ch);
		return;
	}

	if(dice(1, 100) == 1)
	{
		act("$p is disintegrated by $n's poison.", ch, obj, 0, TO_ROOM);
		act("Your poison eats through $p.", ch, obj, 0, TO_CHAR);
		extract_obj(obj);
		return;
	}

	if(IS_SET(obj->extra_flags, ITEM_CAUST))
	{
		act("$p is already coated with deadly poison.", ch, obj, 0, TO_CHAR);
		return;
	}

	act("You run your tongue along $p, coating it with a sickly venom.", ch, obj, 0, TO_CHAR);
	act("$n runs $m tongue along $p, coating it with a sickly venom.", ch, obj, 0, TO_ROOM);

	WAIT_STATE(ch, 8);
	SET_BIT(obj->extra_flags, ITEM_CAUST);
	obj->value[1] += ch->pcdata->stats[UNI_GEN];
	obj->value[2] += ch->pcdata->stats[UNI_GEN];
}


void do_blink(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;

	if(IS_NPC(ch))
		return;

	if(!IS_DEMPOWER(ch, DEM_BLINK))
	{
		stc("You have not been granted the power of Blinking.\n\r", ch);
		return;
	}

	if(ch->fighting == 0 && argument[0] == '\0')
	{
		stc("Who do you wish to blink into combat with?\n\r", ch);
		return;
	}

	if(ch->fighting != 0 && ch->pcdata->stats[UNI_GEN] != 1)
	{
		stc("You blink your eyes and realize you're already fighting.\n\r", ch);
		return;
	}

	if(ch->fighting == 0)
	{
		if((victim = get_char_room(ch, argument)) == 0)
		{
			stc("They aren't here.\n\r", ch);
			return;
		}

		if(is_safe(ch, victim))
			return;

		WAIT_STATE(ch, 24);
		SET_BIT(ch->flag2, AFF2_BLINK_1ST_RD);
		stop_fighting(ch, TRUE);
		stc("You pop out of existance.\n\r", ch);
		act("$n pops out of existance.", ch, 0, 0, TO_ROOM);
		ch->blinkykill = victim;
		return;
	}
	WAIT_STATE(ch, 24);
	victim = ch->fighting;
	SET_BIT(ch->flag2, AFF2_BLINK_2ND_RD);
	stop_fighting(ch, TRUE);
	stc("You pop out of existance.\n\r", ch);
	act("$n pops out of existance.", ch, 0, 0, TO_ROOM);
	ch->blinkykill = victim;
}

// THIS IS THE END OF THE FILE THANKS
