
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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "bard.h"

/* instruments
 * v0 = type
 * v1 = tuning
 * v2 = damage
 */

#define MAX_INSTRUMENT	8
#define I_ANY		-1
#define I_LYRE		0
#define I_MANDOLIN	1
#define I_CHIMES	2
#define I_FLUTE		3
#define I_LUTE		4
#define I_FIDDLE	5
#define I_DRUMS		6
#define I_TAMBOURINE	7
struct instrument_type
{
	long speed;		/* how many rounds until you play */
	long tune;		/* how fast it goes out of tune, tuning/round */
	long affects;
	long type;
};
const struct instrument_type instrument_table[MAX_INSTRUMENT] = {
	/* SPEED     TUNE AFFECTS       TYPE */
	{3, 5, 0, INSTR_LYRE},
	{3, 7, 0, INSTR_MANDOLIN},
	{2, 4, 0, INSTR_CHIMES},
	{1, 1, 0, INSTR_FLUTE},
	{3, 3, 0, INSTR_LUTE},
    {1, 5, 0, INSTR_FIDDLE},
    {3, 2, 0, INSTR_DRUMS},
    {4, 6, 0, INSTR_TAMBOURINE}
};


/* functions */
OBJ_DATA *USING_INSTRUMENT(CHAR_DATA * ch, long type)
{
	OBJ_DATA *obj;

	if((obj = get_eq_char(ch, WEAR_HOLD)) == 0)
		return 0;
	if(obj->item_type != ITEM_INSTRUMENT)
		return 0;
	if(obj->value[0] != type && type != I_ANY)
		return 0;
	return obj;
}

void stop_playing(CHAR_DATA * ch)
{
	if(IS_NPC(ch) || (!IS_NPC(ch) && !IS_SET(ch->pcdata->class, CLASS_BARD)))
		return;
	ch->pcdata->stats[UNI_RAGE] = 0;
	ch->pcdata->stats[UNI_AFF] = 0;
	return;
}


/* the heart of bards, aww */
/* all checks should have been made thus far, he has an instrument, etc. */
void play_song(CHAR_DATA * ch, OBJ_DATA * obj)
{
	char buf[MAX_STRING_LENGTH];

	CHAR_DATA *vch;
	long song = ch->pcdata->stats[UNI_AFF];
	long duration = 0;
	long modifier = 0;
	long location = 0;
	long bitv = 0;
	long instrument = obj->value[0];
	long text = 0;

	ch->pcdata->stats[UNI_RAGE]++;
	switch (song)

	{
	case SONG_DEFENSE:
		text = TEXT_DEFENSE;
		duration = 1;
		modifier = -10;
		location = APPLY_AC;
		switch (instrument)

		{
		case I_DRUMS:
			duration += 1;
			modifier -= 10;
			break;
		case I_FIDDLE:
			duration += 2;
			break;
		case I_MANDOLIN:
		case I_TAMBOURINE:
			modifier -= 5;
			break;
		case I_CHIMES:
		case I_LYRE:
		case I_FLUTE:
			modifier += 10;
			break;
		case I_LUTE:
			duration += 2;
			break;
		default:
			break;
		}
		break;
	default:
		act("$n plays a everyday nice sounding tune.", ch, 0, 0, TO_ROOM);
		act("You play a regular ole nice sounding song.", ch, 0, 0, TO_CHAR);
		return;
	}
	if(!str_cmp(bard_songs[text][ch->pcdata->stats[UNI_RAGE]].lyric, "end"))
		ch->pcdata->stats[UNI_RAGE] = 1;
	sprintf(buf, "$n plays {y$p{x and {Csings{x '%s'", bard_songs[text][ch->pcdata->stats[UNI_RAGE]].lyric);
	act(buf, ch, obj, 0, TO_ROOM);
	sprintf(buf, "You play {y$p{x and {Csing{x '%s'", bard_songs[text][ch->pcdata->stats[UNI_RAGE]].lyric);
	act(buf, ch, obj, 0, TO_CHAR);
	for(vch = ch->in_room->people; vch; vch = vch->next_in_room)

	{
		switch (ch->pcdata->stats[UNI_AFF])

		{
		default:
			combat_affect(ch, gsn_bard, duration, modifier, location, bitv);
			break;
		}
	}
}


/** THE UPDATE **/
/* called right AFTER violence_update, remove and add affects */
void bard_update(void)
{
	AFFECT_DATA *aff;
	CHAR_DATA *ch;
	OBJ_DATA *obj = 0;
	long percent;


	/* step through entire list, eek :) */
	for(ch = char_list; ch; ch = ch->next)

	{
		if(!ch->combat_affects
		   && (!IS_NPC(ch) && IS_SET(ch->pcdata->class, CLASS_BARD) && ch->pcdata->stats[UNI_RAGE] <= 0))
			continue;
		if(ch->combat_affects)

		{

			/* decrment/remove affects */
			for(aff = ch->combat_affects; aff; aff = aff->next)

			{
				if(--aff->duration <= 0)
					combat_aff_remove(ch, ch->combat_affects);
			}
		}
		if(!IS_NPC(ch) && IS_SET(ch->pcdata->class, CLASS_BARD) && ch->pcdata->stats[UNI_RAGE] > 0)

		{
			percent = 55 + number_range(0, 40);
			if(ch->fighting && !USING_INSTRUMENT(ch, I_DRUMS))
				percent -= 15;
			if(number_percent() > percent || (obj = USING_INSTRUMENT(ch, I_ANY)) == 0)

			{
				send_to_char("You break down miserably in the middle of your song.\n\r", ch);
				act("$n hits a few bad notes and breaks up $s song, dissapointed.", ch, 0, 0,
				    TO_ROOM);
				stop_playing(ch);
				continue;
			}
			play_song(ch, obj);
		}
	}
}


/**** THE FUNCTIONS */
void do_play(CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;

	if(!IS_SET(ch->pcdata->class, CLASS_BARD))

	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if((obj = get_eq_char(ch, WEAR_HOLD)) == 0 || obj->item_type != ITEM_INSTRUMENT)

	{
		send_to_char("You need to be holding an instrument first.\n\r", ch);
		return;
	}

/* temporary to test singing */
	if(!str_cmp(argument, "defense"))

	{
		send_to_char("You begin playing the defense song.\n\r", ch);
		ch->pcdata->stats[UNI_AFF] = SONG_DEFENSE;
		ch->pcdata->stats[UNI_RAGE] = 1;
	}
}
void do_learn(CHAR_DATA * ch, char *argument)
{
}


// THIS IS THE END OF THE FILE THANKS
