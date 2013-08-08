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



char *const dir_name[] = {
	"{ynorth{x", "{geast{x", "{rsouth{x", "{mwest{x", "up", "{cdown{x"
};

const long rev_dir[] = {
	2, 3, 0, 1, 5, 4
};

const long movement_loss[SECT_MAX] = {
	1, 2, 2, 3, 4, 6, 4, 1, 6, 10, 6
};



/*
 * Local functions.
 */
long find_door args((CHAR_DATA * ch, char *arg));
bool has_key args((CHAR_DATA * ch, long key));
long count_imms args((CHAR_DATA * ch));
bool check_track args((CHAR_DATA * ch, long direction));
void add_tracks args((CHAR_DATA * ch, long direction));
void show_page args((CHAR_DATA * ch, OBJ_DATA * book, long pnum, bool pagefalse));
void show_runes args((CHAR_DATA * ch, OBJ_DATA * page, bool endline));
bool are_runes args((OBJ_DATA * page));



void move_char(CHAR_DATA * ch, long door)
{
	CHAR_DATA *fch;
	CHAR_DATA *fch_next;
	CHAR_DATA *mount;
	ROOM_INDEX_DATA *in_room;
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	OBJ_DATA *obj;
	DESCRIPTOR_DATA *d;
	char buf[MAX_STRING_LENGTH];
	char poly[MAX_STRING_LENGTH];
	char mount2[MAX_INPUT_LENGTH];
	char leaves[MAX_STRING_LENGTH];
	char enters[MAX_STRING_LENGTH];
	long revdoor;

	leaves[0] = '\0';
	enters[0] = '\0';

	if(door < 0 || door > 5)
	{
		bug("Do_move: bad door %li.", door);
		return;
	}

	if(IS_AFFECTED(ch, AFF_SAFE))
	{
		send_to_char("You can't move while in safe mode.\n\r", ch);
		return;
	}

	in_room = ch->in_room;
	if((pexit = in_room->exit[door]) == 0 || (to_room = pexit->to_room) == 0)
	{
		send_to_char("Alas, you cannot go that way.\n\r", ch);
		return;
	}

	if((door == DIR_NORTH
	    && (((obj = get_obj_list(ch, "walln", ch->in_room->contents)) != 0)
		|| (obj = get_obj_list(ch, "walls", to_room->contents)) != 0)) || (door == DIR_SOUTH
										      &&
										      (((obj =
											 get_obj_list(ch, "walls",
												      ch->in_room->
												      contents)) != 0)
										       || (obj =
											   get_obj_list(ch, "walln",
													to_room->
													contents)) != 0))
	   || (door == DIR_EAST
	       && (((obj = get_obj_list(ch, "walle", ch->in_room->contents)) != 0)
		   || (obj = get_obj_list(ch, "wallw", to_room->contents)) != 0)) || (door == DIR_WEST
											 &&
											 (((obj =
											    get_obj_list(ch, "wallw",
													 ch->in_room->
													 contents)) != 0)
											  || (obj =
											      get_obj_list(ch, "walle",
													   to_room->
													   contents)) !=
											  0)) || (door == DIR_UP
												     &&
												     (((obj =
													get_obj_list(ch,
														     "wallu",
														     ch->
														     in_room->
														     contents))
												       != 0)
												      || (obj =
													  get_obj_list(ch,
														       "walld",
														       to_room->
														       contents))
												      != 0))
	   || (door == DIR_DOWN
	       && (((obj = get_obj_list(ch, "walld", ch->in_room->contents)) != 0)
		   || (obj = get_obj_list(ch, "wallu", to_room->contents)) != 0)))
	{
		stc("You are unable to pass the wall of Blood.\n\r", ch);
		return;
	}

	if(IS_SET(pexit->exit_info, EX_CLOSED)
	   && !IS_AFFECTED(ch, AFF_PASS_DOOR)
	   && IS_NPC(ch) && !IS_AFFECTED(ch, AFF_ETHEREAL) && !IS_AFFECTED(ch, AFF_SHADOWPLANE))
	{
		if(!IS_NPC(ch) && IS_CLASS(ch, CLASS_WEREWOLF) && ch->pcdata->powers[WPOWER_BOAR] > 0)
		{
			act("You smash open the $d.", ch, 0, pexit->keyword, TO_CHAR);
			act("$n smashes open the $d.", ch, 0, pexit->keyword, TO_ROOM);
			REMOVE_BIT(pexit->exit_info, EX_CLOSED);
		}
		else
		{
			act("The $d is closed.", ch, 0, pexit->keyword, TO_CHAR);
			return;
		}
	}

	if(IS_AFFECTED(ch, AFF_CHARM) && ch->master != 0 && in_room == ch->master->in_room)
	{
		send_to_char("What?  And leave your beloved master?\n\r", ch);
		return;
	}

	if(IS_NPC(ch) && (mount = ch->mount) != 0 && IS_SET(ch->mounted, IS_MOUNT))
	{
		send_to_char("You better wait for instructions from your rider.\n\r", ch);
		return;
	}

	if(room_is_private(to_room))
	{

		if(IS_SET(pexit->exit_info, ITEM_WALL) && !IS_NPC(ch) && ch->pcdata->powers[VAM_THAN] < 2)
		{
			send_to_char("A wall of blood blocks your way.\n\r", ch);
			return;
		}

		if(IS_NPC(ch) || ch->trust < MAX_LEVEL)
		{
			send_to_char("That room is private right now.\n\r", ch);
			return;
		}
		else
			send_to_char("That room is private (Access granted).\n\r", ch);
	}

	if((IS_LEG_L(ch, BROKEN_LEG) || IS_LEG_L(ch, LOST_LEG)) &&
	   (IS_LEG_R(ch, BROKEN_LEG) || IS_LEG_R(ch, LOST_LEG)) &&
	   (IS_ARM_L(ch, BROKEN_ARM) || IS_ARM_L(ch, LOST_ARM) ||
	    get_eq_char(ch, WEAR_HOLD) != 0) &&
	   (IS_ARM_R(ch, BROKEN_ARM) || IS_ARM_R(ch, LOST_ARM) || get_eq_char(ch, WEAR_WIELD) != 0))
	{
		send_to_char("You need at least one free arm to drag yourself with.\n\r", ch);
		return;
	}
	else if(IS_BODY(ch, BROKEN_SPINE) &&
		(IS_ARM_L(ch, BROKEN_ARM) || IS_ARM_L(ch, LOST_ARM) ||
		 get_eq_char(ch, WEAR_HOLD) != 0) &&
		(IS_ARM_R(ch, BROKEN_ARM) || IS_ARM_R(ch, LOST_ARM) || get_eq_char(ch, WEAR_WIELD) != 0))
	{
		send_to_char("You cannot move with a broken spine.\n\r", ch);
		return;
	}

	if(!IS_NPC(ch))
	{
		long move;

		if(in_room->sector_type == SECT_AIR || to_room->sector_type == SECT_AIR)
		{
			if(!IS_AFFECTED(ch, AFF_FLYING) &&
			   (!IS_NPC(ch) && (!IS_CLASS(ch, CLASS_DROW) ||
					    !IS_SET(ch->pcdata->powers[1], DPOWER_LEVITATION))) &&
			   (!IS_NPC(ch) && !IS_VAMPAFF(ch, VAM_FLYING) &&
			    (!IS_NPC(ch) && !IS_DEMAFF(ch, DEM_UNFOLDED)))
			   && !((mount = ch->mount) != 0 && IS_SET(ch->mounted, IS_RIDING) &&
				IS_AFFECTED(mount, AFF_FLYING)))
			{
				send_to_char("You can't fly.\n\r", ch);
				return;
			}
		}

		if(in_room->sector_type == SECT_WATER_NOSWIM || to_room->sector_type == SECT_WATER_NOSWIM)
		{
			OBJ_DATA *obj;
			bool found;

			/*
			 * Look for a boat.
			 */
			found = FALSE;
			if(!IS_NPC(ch) && IS_CLASS(ch, CLASS_VAMPIRE))
			{
				if(IS_VAMPAFF(ch, VAM_FLYING))
					found = TRUE;
				else if(IS_POLYAFF(ch, POLY_SERPENT))
					found = TRUE;
				else if(IS_AFFECTED(ch, AFF_SHADOWPLANE))
					found = TRUE;
				else if((mount = ch->mount) != 0 && IS_SET(ch->mounted, IS_RIDING)
					&& IS_AFFECTED(mount, AFF_FLYING))
					found = TRUE;
				else
				{
					send_to_char("You are unable to cross running water.\n\r", ch);
					return;
				}
			}
			if(IS_AFFECTED(ch, AFF_FLYING))
				found = TRUE;
			else if(!IS_NPC(ch) && IS_DEMAFF(ch, DEM_UNFOLDED))
				found = TRUE;
			else if(!IS_NPC(ch) && (IS_CLASS(ch, CLASS_DROW) &&
						IS_SET(ch->pcdata->powers[1], DPOWER_LEVITATION)))
				found = TRUE;
			if(!found)
			{
				for(obj = ch->carrying; obj != 0; obj = obj->next_content)
				{
					if(obj->item_type == ITEM_BOAT)
					{
						found = TRUE;
						break;
					}
				}
				if(!found)
				{
					send_to_char("You need a boat to go there.\n\r", ch);
					return;
				}
			}
		}
		else if(!IS_AFFECTED(ch, AFF_FLYING) && IS_POLYAFF(ch, POLY_FISH))
		{
			bool from_ok = FALSE;
			bool to_ok = FALSE;

			if(in_room->sector_type == SECT_WATER_NOSWIM)
				from_ok = TRUE;
			if(in_room->sector_type == SECT_WATER_SWIM)
				from_ok = TRUE;
			if(to_room->sector_type == SECT_WATER_NOSWIM)
				to_ok = TRUE;
			if(to_room->sector_type == SECT_WATER_SWIM)
				to_ok = TRUE;
			if(!from_ok || !to_ok)
			{
				send_to_char("You cannot cross land.\n\r", ch);
				return;
			}
		}

		move = movement_loss[UMIN(SECT_MAX - 1, in_room->sector_type)]
			+ movement_loss[UMIN(SECT_MAX - 1, to_room->sector_type)];

		if(IS_HERO(ch))
			move = 0;

		if(ch->move <= 0)
		{
			send_to_char("You are too Exhausted.\n\r", ch);
			return;
		}

		if(IS_SET(ch->mounted, IS_RIDING) && (ch->move < move || ch->move < 1))
		{
			send_to_char("You are too exhausted.\n\r", ch);
			return;
		}

		WAIT_STATE(ch, 1);
		if(!IS_SET(ch->mounted, IS_RIDING))
			ch->move -= move;
	}

	/* Check for mount message - KaVir */
	if((mount = ch->mount) != 0 && ch->mounted == IS_RIDING)
	{
		if(IS_NPC(mount))
			sprintf(mount2, " on %s.", mount->short_descr);
		else
			sprintf(mount2, " on %s.", mount->name);
	}
	else
		sprintf(mount2, ".");

/*    if ( IS_HEAD(ch,LOST_HEAD) || IS_EXTRA(ch,EXTRA_OSWITCH) )
	sprintf(leave,"rolls");
    else if ( IS_AFFECTED(ch,AFF_ETHEREAL) )
	sprintf(leave,"floats");
    else if ( ch->in_room->sector_type == SECT_WATER_SWIM )
	sprintf(leave,"swims");
    else if ( IS_SET(ch->polyaff,POLY_SERPENT) )
	sprintf(leave,"slithers");
    else if ( IS_SET(ch->polyaff,POLY_WOLF) )
	sprintf(leave,"stalks");
    else if ( IS_SET(ch->polyaff,POLY_FROG) )
	sprintf(leave,"hops");
    else if ( IS_SET(ch->polyaff,POLY_FISH) )
	sprintf(leave,"swims");
    else if ( !IS_NPC(ch) && IS_DEMAFF(ch,DEM_UNFOLDED) )
	sprintf(leave,"flies");
    else if ( IS_BODY(ch,BROKEN_SPINE) )
	sprintf(leave,"drags $mself");
    else if ( IS_LEG_L(ch,LOST_LEG) && IS_LEG_R(ch,LOST_LEG) )
	sprintf(leave,"drags $mself");
    else if ( (IS_LEG_L(ch,BROKEN_LEG) || IS_LEG_L(ch,LOST_LEG) || IS_LEG_L(ch,LOST_FOOT)) &&
	 (IS_LEG_R(ch,BROKEN_LEG) || IS_LEG_R(ch,LOST_LEG) || IS_LEG_R(ch,LOST_FOOT)) )
	sprintf(leave,"crawls");
    else if ( ch->hit < (ch->max_hit/4) )
	sprintf(leave,"crawls");
    else if ( (IS_LEG_R(ch,LOST_LEG) || IS_LEG_R(ch,LOST_FOOT)) &&
	 (!IS_LEG_L(ch,BROKEN_LEG) && !IS_LEG_L(ch,LOST_LEG) &&
	  !IS_LEG_L(ch,LOST_FOOT)) )
	sprintf(leave,"hops");
    else if ( (IS_LEG_L(ch,LOST_LEG) || IS_LEG_L(ch,LOST_FOOT)) &&
	 (!IS_LEG_R(ch,BROKEN_LEG) && !IS_LEG_R(ch,LOST_LEG) &&
	  !IS_LEG_R(ch,LOST_FOOT)) )
	sprintf(leave,"hops");
    else if ( (IS_LEG_L(ch,BROKEN_LEG) || IS_LEG_L(ch,LOST_FOOT)) &&
	 (!IS_LEG_R(ch,BROKEN_LEG) && !IS_LEG_R(ch,LOST_LEG) &&
	  !IS_LEG_R(ch,LOST_FOOT)) )
	sprintf(leave,"limps");
    else if ( (IS_LEG_R(ch,BROKEN_LEG) || IS_LEG_R(ch,LOST_FOOT)) &&
	 (!IS_LEG_L(ch,BROKEN_LEG) && !IS_LEG_L(ch,LOST_LEG) &&
	  !IS_LEG_L(ch,LOST_FOOT)) )
	sprintf(leave,"limps");
    else if ( ch->hit < (ch->max_hit/3) )
	sprintf(leave,"limps");
    else if ( ch->hit < (ch->max_hit/2) )
	sprintf(leave,"staggers");
    else if ( !IS_NPC(ch) )
    {
	if (ch->pcdata->condition[COND_DRUNK] > 10)
	    sprintf(leave,"staggers");
	else
	    sprintf(leave,"walks");
    }
    else
	sprintf(leave,"walks");
*/

	if(!IS_NPC(ch) && strlen(ch->pcdata->exit) > 2)
		sprintf(leaves, ch->pcdata->exit);
	else
		sprintf(leaves, "leaves");

	if(!IS_NPC(ch) && strlen(ch->pcdata->enter) > 2)
		sprintf(enters, ch->pcdata->enter);
	else
		sprintf(enters, ".");

	if(!IS_NPC(ch) && ch->stance[0] != -1)
		do_stance(ch, "");

	for(d = descriptor_list; d != 0; d = d->next)
	{
		CHAR_DATA *victim;

		if((victim = d->character) == 0)
			continue;

		if(ch->in_room == 0 || victim->in_room == 0)
			continue;

		if(ch == victim || ch->in_room != victim->in_room)
			continue;

		if(d->connected != CON_PLAYING || !can_see(ch, victim))
			continue;

		sprintf(poly, "$n %s %s%s", leaves, dir_name[door], mount2);
		act(poly, ch, 0, victim, TO_VICT);

/*	if ( !IS_NPC(ch) && !IS_AFFECTED(ch, AFF_SNEAK) && IS_AFFECTED(ch,AFF_POLYMORPH)
	&& ( IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS) )
	&& can_see(victim,ch))
	{
	    if (((mount = ch->mount) != 0 && ch->mounted == IS_RIDING &&
	    IS_AFFECTED(mount,AFF_FLYING)) || IS_AFFECTED(ch,AFF_FLYING) ||
	    (!IS_NPC(ch) && IS_VAMPAFF(ch,VAM_FLYING)) )
		sprintf(poly,"%s flies $T%s",ch->morph,mount2);
	    else if ( (mount = ch->mount) != 0 && ch->mounted == IS_RIDING )
		sprintf(poly,"%s rides $T%s",ch->morph,mount2);
	    else
		sprintf(poly,"%s %s $T%s",ch->morph,leave,mount2);
	    act( poly, victim, 0, dir_name[door], TO_CHAR );
	}
	else if ( !IS_AFFECTED(ch, AFF_SNEAK)
	&& ( IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS) )
	&& can_see(victim,ch))
	{
	    if (((mount = ch->mount) != 0 && ch->mounted == IS_RIDING &&
	    IS_AFFECTED(mount,AFF_FLYING)) || IS_AFFECTED(ch,AFF_FLYING) ||
	    (!IS_NPC(ch) && IS_VAMPAFF(ch,VAM_FLYING)) )
		sprintf(poly,"$n flies %s%s",dir_name[door],mount2);
	    else if ( (mount = ch->mount) != 0 && ch->mounted == IS_RIDING )
		sprintf(poly,"$n rides %s%s",dir_name[door],mount2);
	    else
		sprintf(poly,"$n %s %s%s",leave,dir_name[door],mount2);
	    act( poly, ch, 0, victim, TO_VICT );
	}
*/
	}
	char_from_room(ch);
	char_to_room(ch, to_room);
	if(door == 0)
	{
		revdoor = 2;
		sprintf(buf, "the south");
	}
	else if(door == 1)
	{
		revdoor = 3;
		sprintf(buf, "the west");
	}
	else if(door == 2)
	{
		revdoor = 0;
		sprintf(buf, "the north");
	}
	else if(door == 3)
	{
		revdoor = 1;
		sprintf(buf, "the east");
	}
	else if(door == 4)
	{
		revdoor = 5;
		sprintf(buf, "below");
	}
	else
	{
		revdoor = 4;
		sprintf(buf, "above");
	}

	for(d = descriptor_list; d != 0; d = d->next)
	{
		CHAR_DATA *victim;

		if((victim = d->character) == 0)
			continue;

		if(ch->in_room == 0 || victim->in_room == 0)
			continue;

		if(ch == victim || ch->in_room != victim->in_room)
			continue;

		if(d->connected != CON_PLAYING || !can_see(ch, victim))
			continue;

		sprintf(poly, "$n arrives%s", enters);
		act(poly, ch, 0, victim, TO_VICT);

/*
	if ( !IS_NPC(ch) && !IS_AFFECTED(ch, AFF_SNEAK) && IS_AFFECTED(ch,AFF_POLYMORPH)
	&& ( IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS) ) && can_see(victim,ch))
	{
	    if (((mount = ch->mount) != 0 && ch->mounted == IS_RIDING &&
	    IS_AFFECTED(mount,AFF_FLYING)) || IS_AFFECTED(ch,AFF_FLYING) ||
	    (!IS_NPC(ch) && IS_VAMPAFF(ch,VAM_FLYING)) )
		sprintf(poly,"%s flies in from %s%s",ch->morph,buf,mount2);
	    else if ( (mount = ch->mount) != 0 && ch->mounted == IS_RIDING )
		sprintf(poly,"%s rides in from %s%s",ch->morph,buf,mount2);
	    else
		sprintf(poly,"%s %s in from %s%s",ch->morph,leave,buf,mount2);
	    act( poly, ch, 0, victim, TO_VICT );
	}
	else if ( !IS_AFFECTED(ch, AFF_SNEAK) && can_see(victim,ch)
	&& ( IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS) ) )
	{
	    if (((mount = ch->mount) != 0 && ch->mounted == IS_RIDING &&
	    IS_AFFECTED(mount,AFF_FLYING)) || IS_AFFECTED(ch,AFF_FLYING) ||
	    (!IS_NPC(ch) && IS_VAMPAFF(ch,VAM_FLYING)) )
		sprintf( poly, "$n flies in from %s%s",buf,mount2);
	    else if ( (mount = ch->mount) != 0 && ch->mounted == IS_RIDING )
		sprintf(poly,"$n rides in from %s%s",buf,mount2);
	    else
		sprintf( poly, "$n %s in from %s%s",leave,buf,mount2);
	    act( poly, ch, 0, victim, TO_VICT );
	}
*/
	}

	do_look(ch, "auto");

	if(!IS_NPC(ch))
	{
		for(fch = to_room->people; fch != 0; fch = fch_next)
		{
			fch_next = fch->next_in_room;

			if(IS_NPC(fch) || ch == fch)
				continue;

			if((!IS_NPC(ch) && ch->pcdata->class == fch->pcdata->class
			    && ch->pcdata->stats[UNI_GEN] < fch->pcdata->stats[UNI_GEN]) || IS_IMMORTAL(ch))
			{
				if(IS_IMMORTAL(ch))
				{
					act("$N kneels before the almighty $n.", ch, 0, fch, TO_NOTVICT);
					act("$N kneels before you.", ch, 0, fch, TO_CHAR);
					act("You kneel before the almighty $n.", ch, 0, fch, TO_VICT);
				}
				else if((fch->pcdata->stats[UNI_GEN] - ch->pcdata->stats[UNI_GEN]) > 1)
				{
					act("$N bows before $s master, $n.", ch, 0, fch, TO_NOTVICT);
					act("$N bows before you.", ch, 0, fch, TO_CHAR);
					act("You bow before your master, $n.", ch, 0, fch, TO_VICT);
				}
				else
				{
					act("$N says 'Hail $n!'", ch, 0, fch, TO_NOTVICT);
					act("$N says 'Hail $n!'", ch, 0, fch, TO_CHAR);
					act("You say 'Hail $n!'", ch, 0, fch, TO_VICT);
				}
			}
		}
	}

	for(fch = in_room->people; fch != 0; fch = fch_next)
	{
		fch_next = fch->next_in_room;
		if((mount = fch->mount) != 0 && mount == ch && IS_SET(fch->mounted, IS_MOUNT))
		{
			act("$N digs $S heels into you.", fch, 0, ch, TO_CHAR);
			char_from_room(fch);
			char_to_room(fch, ch->in_room);
		}

		if(fch->master == ch && fch->position == POS_STANDING && fch->in_room != ch->in_room)
		{
			act("You follow $N.", fch, 0, ch, TO_CHAR);
			move_char(fch, door);
		}
	}

	room_text(ch, ">ENTER<");
	return;
}



void do_north(CHAR_DATA * ch, char *argument)
{
	ROOM_INDEX_DATA *in_room;

	if(IS_AFFECTED(ch, AFF_WEBBED))
	{
		send_to_char("You are unable to move with all this sticky webbing on.\n\r", ch);
		return;
	}

	in_room = ch->in_room;
	move_char(ch, DIR_NORTH);
	if(!IS_NPC(ch) && ch->in_room != in_room)
	{
		ROOM_INDEX_DATA *old_room;

		old_room = ch->in_room;
		char_from_room(ch);
		char_to_room(ch, in_room);
		add_tracks(ch, DIR_NORTH);
		char_from_room(ch);
		char_to_room(ch, old_room);
	}
/*
    if (!IS_NPC(ch) && ch->in_room != in_room) add_tracks( ch, DIR_SOUTH );
*/
	return;
}



void do_east(CHAR_DATA * ch, char *argument)
{
	ROOM_INDEX_DATA *in_room;

	if(IS_AFFECTED(ch, AFF_WEBBED))
	{
		send_to_char("You are unable to move with all this sticky webbing on.\n\r", ch);
		return;
	}

	in_room = ch->in_room;
	move_char(ch, DIR_EAST);
	if(!IS_NPC(ch) && ch->in_room != in_room)
	{
		ROOM_INDEX_DATA *old_room;

		old_room = ch->in_room;
		char_from_room(ch);
		char_to_room(ch, in_room);
		add_tracks(ch, DIR_EAST);
		char_from_room(ch);
		char_to_room(ch, old_room);
	}
/*
    if (!IS_NPC(ch) && ch->in_room != in_room) add_tracks( ch, DIR_WEST );
*/
	return;
}



void do_south(CHAR_DATA * ch, char *argument)
{
	ROOM_INDEX_DATA *in_room;

	if(IS_AFFECTED(ch, AFF_WEBBED))
	{
		send_to_char("You are unable to move with all this sticky webbing on.\n\r", ch);
		return;
	}

	in_room = ch->in_room;
	move_char(ch, DIR_SOUTH);
	if(!IS_NPC(ch) && ch->in_room != in_room)
	{
		ROOM_INDEX_DATA *old_room;

		old_room = ch->in_room;
		char_from_room(ch);
		char_to_room(ch, in_room);
		add_tracks(ch, DIR_SOUTH);
		char_from_room(ch);
		char_to_room(ch, old_room);
	}
/*
    if (!IS_NPC(ch) && ch->in_room != in_room) add_tracks( ch, DIR_NORTH );
*/
	return;
}



void do_west(CHAR_DATA * ch, char *argument)
{
	ROOM_INDEX_DATA *in_room;

	if(IS_AFFECTED(ch, AFF_WEBBED))
	{
		send_to_char("You are unable to move with all this sticky webbing on.\n\r", ch);
		return;
	}

	in_room = ch->in_room;
	move_char(ch, DIR_WEST);
	if(!IS_NPC(ch) && ch->in_room != in_room)
	{
		ROOM_INDEX_DATA *old_room;

		old_room = ch->in_room;
		char_from_room(ch);
		char_to_room(ch, in_room);
		add_tracks(ch, DIR_WEST);
		char_from_room(ch);
		char_to_room(ch, old_room);
	}
/*
    if (!IS_NPC(ch) && ch->in_room != in_room) add_tracks( ch, DIR_EAST );
*/
	return;
}



void do_up(CHAR_DATA * ch, char *argument)
{
	ROOM_INDEX_DATA *in_room;

	if(IS_AFFECTED(ch, AFF_WEBBED))
	{
		send_to_char("You are unable to move with all this sticky webbing on.\n\r", ch);
		return;
	}

	in_room = ch->in_room;
	move_char(ch, DIR_UP);
	if(!IS_NPC(ch) && ch->in_room != in_room)
	{
		ROOM_INDEX_DATA *old_room;

		old_room = ch->in_room;
		char_from_room(ch);
		char_to_room(ch, in_room);
		add_tracks(ch, DIR_UP);
		char_from_room(ch);
		char_to_room(ch, old_room);
	}
/*
    if (!IS_NPC(ch) && ch->in_room != in_room) add_tracks( ch, DIR_DOWN );
*/
	return;
}



void do_down(CHAR_DATA * ch, char *argument)
{
	ROOM_INDEX_DATA *in_room;

	if(IS_AFFECTED(ch, AFF_WEBBED))
	{
		send_to_char("You are unable to move with all this sticky webbing on.\n\r", ch);
		return;
	}

	in_room = ch->in_room;
	move_char(ch, DIR_DOWN);
	if(!IS_NPC(ch) && ch->in_room != in_room)
	{
		ROOM_INDEX_DATA *old_room;

		old_room = ch->in_room;
		char_from_room(ch);
		char_to_room(ch, in_room);
		add_tracks(ch, DIR_DOWN);
		char_from_room(ch);
		char_to_room(ch, old_room);
	}
/*
    if (!IS_NPC(ch) && ch->in_room != in_room) add_tracks( ch, DIR_UP );
*/
	return;
}



long find_door(CHAR_DATA * ch, char *arg)
{
	EXIT_DATA *pexit;
	long door;

	if(!str_cmp(arg, "n") || !str_cmp(arg, "north"))
		door = 0;
	else if(!str_cmp(arg, "e") || !str_cmp(arg, "east"))
		door = 1;
	else if(!str_cmp(arg, "s") || !str_cmp(arg, "south"))
		door = 2;
	else if(!str_cmp(arg, "w") || !str_cmp(arg, "west"))
		door = 3;
	else if(!str_cmp(arg, "u") || !str_cmp(arg, "up"))
		door = 4;
	else if(!str_cmp(arg, "d") || !str_cmp(arg, "down"))
		door = 5;
	else
	{
		for(door = 0; door <= 5; door++)
		{
			if((pexit = ch->in_room->exit[door]) != 0
			   && IS_SET(pexit->exit_info, EX_ISDOOR) && pexit->keyword != 0 && is_name(arg, pexit->keyword))
				return door;
		}
		act("I see no $T here.", ch, 0, arg, TO_CHAR);
		return -1;
	}

	if((pexit = ch->in_room->exit[door]) == 0)
	{
		act("I see no door $T here.", ch, 0, arg, TO_CHAR);
		return -1;
	}

	if(!IS_SET(pexit->exit_info, EX_ISDOOR))
	{
		send_to_char("You can't do that.\n\r", ch);
		return -1;
	}

	return door;
}

/* Designed for the portal spell, but can also have other uses...KaVir
 * V0 = Where the portal will take you.
 * V1 = Number of uses (0 is infinate).
 * V2 = if 2, cannot be entered.
 * V3 = The room the portal is currently in.
 */
void do_enter(CHAR_DATA * ch, char *argument)
{
	ROOM_INDEX_DATA *pRoomIndex;
	ROOM_INDEX_DATA *location;
	char arg[MAX_INPUT_LENGTH];
	char poly[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	OBJ_DATA *portal;
	OBJ_DATA *portal_next;
	CHAR_DATA *mount;
	bool found;

	argument = one_argument(argument, arg);

	if(arg[0] == '\0')
	{
		send_to_char("Enter what?\n\r", ch);
		return;
	}
/*
    if ( is_inarena(ch) ) return;
*/
	obj = get_obj_list(ch, arg, ch->in_room->contents);
	if(obj == 0)
	{
		act("I see no $T here.", ch, 0, arg, TO_CHAR);
		return;
	}
	if(obj->item_type != ITEM_PORTAL && obj->item_type != ITEM_WGATE)
	{
		act("You cannot enter that.", ch, 0, arg, TO_CHAR);
		return;
	}

	if(IS_AFFECTED(ch, AFF_SHADOWPLANE) && !IS_SET(obj->extra_flags, ITEM_SHADOWPLANE))
	{
		send_to_char("You are too insubstantual.\n\r", ch);
		return;
	}
	else if(!IS_AFFECTED(ch, AFF_SHADOWPLANE) && IS_SET(obj->extra_flags, ITEM_SHADOWPLANE))
	{
		send_to_char("It is too insubstantual.\n\r", ch);
		return;
	}

	if(obj->item_type != ITEM_WGATE)
	{
		if(obj->value[2] == 2 || obj->value[2] == 3)
		{
			act("It seems to be closed.", ch, 0, arg, TO_CHAR);
			return;
		}
	}

	pRoomIndex = get_room_index(obj->value[0]);
	location = ch->in_room;

	if(pRoomIndex == 0)
	{
		act("You are unable to enter.", ch, 0, arg, TO_CHAR);
		return;
	}

	act("You step into $p.", ch, obj, 0, TO_CHAR);
	if(!IS_NPC(ch) && IS_AFFECTED(ch, AFF_POLYMORPH))
		sprintf(poly, "%s steps into $p.", ch->morph);
	else
		sprintf(poly, "$n steps into $p.");
	act(poly, ch, obj, 0, TO_ROOM);
	char_from_room(ch);
	char_to_room(ch, pRoomIndex);
	if(!IS_NPC(ch) && IS_AFFECTED(ch, AFF_POLYMORPH))
		sprintf(poly, "%s steps out of $p.", ch->morph);
	else
		sprintf(poly, "$n steps out of $p.");
	act(poly, ch, obj, 0, TO_ROOM);
	char_from_room(ch);
	char_to_room(ch, location);

	if(obj->item_type != ITEM_WGATE)
	{
		if(obj->value[1] != 0)
		{
			obj->value[1] = obj->value[1] - 1;
			if(obj->value[1] < 1)
			{
				act("$p vanishes.", ch, obj, 0, TO_CHAR);
				act("$p vanishes.", ch, obj, 0, TO_ROOM);
				extract_obj(obj);
			}
		}
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

			if(IS_AFFECTED(ch, AFF_SHADOWPLANE) && !IS_SET(portal->extra_flags, ITEM_SHADOWPLANE))
			{
				REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
				break;
			}
			else if(!IS_AFFECTED(ch, AFF_SHADOWPLANE) && IS_SET(portal->extra_flags, ITEM_SHADOWPLANE))
			{
				SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
				break;
			}
			if(portal->value[1] != 0 && portal->item_type != ITEM_WGATE)
			{
				portal->value[1] = portal->value[1] - 1;
				if(portal->value[1] < 1)
				{
					act("$p vanishes.", ch, portal, 0, TO_CHAR);
					act("$p vanishes.", ch, portal, 0, TO_ROOM);
					extract_obj(portal);
				}
			}
		}
	}
	do_look(ch, "auto");
	if((mount = ch->mount) == 0)
		return;
	char_from_room(mount);
	char_to_room(mount, ch->in_room);
	return;
}


void do_open(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	long door;

	one_argument(argument, arg);

	if(arg[0] == '\0')
	{
		send_to_char("Open what?\n\r", ch);
		return;
	}

	if((obj = get_obj_here(ch, arg)) != 0)
	{
		/* 'open object' */
		if(obj->item_type != ITEM_CONTAINER && obj->item_type != ITEM_BOOK)
		{
			send_to_char("That's not a container.\n\r", ch);
			return;
		}
		if(!IS_SET(obj->value[1], CONT_CLOSED))
		{
			send_to_char("It's already open.\n\r", ch);
			return;
		}
		if(!IS_SET(obj->value[1], CONT_CLOSEABLE) && obj->item_type != ITEM_BOOK)
		{
			send_to_char("You can't do that.\n\r", ch);
			return;
		}
		if(IS_SET(obj->value[1], CONT_LOCKED))
		{
			send_to_char("It's locked.\n\r", ch);
			return;
		}

		REMOVE_BIT(obj->value[1], CONT_CLOSED);
		send_to_char("Ok.\n\r", ch);
		act("$n opens $p.", ch, obj, 0, TO_ROOM);
		return;
	}

	if((door = find_door(ch, arg)) >= 0)
	{
		/* 'open door' */
		ROOM_INDEX_DATA *to_room;
		EXIT_DATA *pexit;
		EXIT_DATA *pexit_rev;

		pexit = ch->in_room->exit[door];
		if(!IS_SET(pexit->exit_info, EX_CLOSED))
		{
			send_to_char("It's already open.\n\r", ch);
			return;
		}
		if(IS_SET(pexit->exit_info, EX_LOCKED))
		{
			send_to_char("It's locked.\n\r", ch);
			return;
		}

		REMOVE_BIT(pexit->exit_info, EX_CLOSED);
		act("$n opens the $d.", ch, 0, pexit->keyword, TO_ROOM);
		send_to_char("Ok.\n\r", ch);

		/* open the other side */
		if((to_room = pexit->to_room) != 0
		   && (pexit_rev = to_room->exit[rev_dir[door]]) != 0 && pexit_rev->to_room == ch->in_room)
		{
			CHAR_DATA *rch;

			REMOVE_BIT(pexit_rev->exit_info, EX_CLOSED);
			for(rch = to_room->people; rch != 0; rch = rch->next_in_room)
				act("The $d opens.", rch, 0, pexit_rev->keyword, TO_CHAR);
		}
	}

	return;
}



void do_close(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	long door;

	one_argument(argument, arg);

	if(arg[0] == '\0')
	{
		send_to_char("Close what?\n\r", ch);
		return;
	}

	if((door = find_door(ch, arg)) >= 0)
	{
		/* 'close door' */
		ROOM_INDEX_DATA *to_room;
		EXIT_DATA *pexit;
		EXIT_DATA *pexit_rev;

		pexit = ch->in_room->exit[door];
		if(IS_SET(pexit->exit_info, EX_CLOSED))
		{
			send_to_char("It's already closed.\n\r", ch);
			return;
		}

		SET_BIT(pexit->exit_info, EX_CLOSED);
		act("$n closes the $d.", ch, 0, pexit->keyword, TO_ROOM);
		send_to_char("Ok.\n\r", ch);

		/* close the other side */
		if((to_room = pexit->to_room) != 0
		   && (pexit_rev = to_room->exit[rev_dir[door]]) != 0 && pexit_rev->to_room == ch->in_room)
		{
			CHAR_DATA *rch;

			SET_BIT(pexit_rev->exit_info, EX_CLOSED);
			for(rch = to_room->people; rch != 0; rch = rch->next_in_room)
				act("The $d closes.", rch, 0, pexit_rev->keyword, TO_CHAR);
		}
		if((obj = get_obj_here(ch, arg)) != 0)
		{
			/* 'close object' */
			if(obj->item_type != ITEM_CONTAINER && obj->item_type != ITEM_BOOK)
			{
				send_to_char("That's not a container.\n\r", ch);
				return;
			}
			if(IS_SET(obj->value[1], CONT_CLOSED))
			{
				send_to_char("It's already closed.\n\r", ch);
				return;
			}
			if(!IS_SET(obj->value[1], CONT_CLOSEABLE) && obj->item_type != ITEM_BOOK)
			{
				send_to_char("You can't do that.\n\r", ch);
				return;
			}

			SET_BIT(obj->value[1], CONT_CLOSED);
			if(obj->item_type == ITEM_BOOK)
				obj->value[2] = 0;
			send_to_char("Ok.\n\r", ch);
			act("$n closes $p.", ch, obj, 0, TO_ROOM);
			return;
		}
	}

	return;
}



void do_turn(CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	long value = 0;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if(arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char("Syntax: Turn <book> <forward/back>.\n\r", ch);
		return;
	}

	if(is_number(arg2))
		value = atoi(arg2);

	if((obj = get_obj_here(ch, arg1)) == 0)
	{
		send_to_char("You don't have that book.\n\r", ch);
		return;
	}

	if(obj->item_type != ITEM_BOOK)
	{
		send_to_char("That's not a book.\n\r", ch);
		return;
	}
	if(IS_SET(obj->value[1], CONT_CLOSED))
	{
		send_to_char("First you should open it.\n\r", ch);
		return;
	}

	if(!str_cmp(arg2, "f") || !str_cmp(arg2, "forward"))
	{
		if(obj->value[2] >= obj->value[3])
		{
			send_to_char("But you are already at the end of the book.\n\r", ch);
			return;
		}
		obj->value[2] += 1;
		act("You flip forward a page in $p.", ch, obj, 0, TO_CHAR);
		act("$n flips forward a page in $p.", ch, obj, 0, TO_ROOM);
	}
	else if(!str_cmp(arg2, "b") || !str_cmp(arg2, "backward"))
	{
		if(obj->value[2] <= 0)
		{
			send_to_char("But you are already at the beginning of the book.\n\r", ch);
			return;
		}
		obj->value[2] -= 1;
		act("You flip backward a page in $p.", ch, obj, 0, TO_CHAR);
		act("$n flips backward a page in $p.", ch, obj, 0, TO_ROOM);
	}
	else if(is_number(arg2) && value >= 0 && value <= obj->value[3])
	{
		if(value == obj->value[2])
		{
			act("$p is already open at that page.", ch, obj, 0, TO_CHAR);
			return;
		}
		else if(value < obj->value[2])
		{
			act("You flip backwards through $p.", ch, obj, 0, TO_CHAR);
			act("$n flips backwards through $p.", ch, obj, 0, TO_ROOM);
		}
		else
		{
			act("You flip forwards through $p.", ch, obj, 0, TO_CHAR);
			act("$n flips forwards through $p.", ch, obj, 0, TO_ROOM);
		}
		obj->value[2] = value;
	}
	else
		send_to_char("Do you wish to turn forward or backward a page?\n\r", ch);
	return;
}


bool has_key(CHAR_DATA * ch, long key)
{
	OBJ_DATA *obj;

	for(obj = ch->carrying; obj != 0; obj = obj->next_content)
	{
		if(obj->pIndexData->vnum == key)
			return TRUE;
	}

	return FALSE;
}



void do_lock(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	long door;

	one_argument(argument, arg);

	if(arg[0] == '\0')
	{
		send_to_char("Lock what?\n\r", ch);
		return;
	}

	if((obj = get_obj_here(ch, arg)) != 0)
	{
		/* 'lock object' */
		if(obj->item_type != ITEM_CONTAINER)
		{
			send_to_char("That's not a container.\n\r", ch);
			return;
		}
		if(!IS_SET(obj->value[1], CONT_CLOSED))
		{
			send_to_char("It's not closed.\n\r", ch);
			return;
		}
		if(obj->value[2] < 0)
		{
			send_to_char("It can't be locked.\n\r", ch);
			return;
		}
		if(!has_key(ch, obj->value[2]))
		{
			send_to_char("You lack the key.\n\r", ch);
			return;
		}
		if(IS_SET(obj->value[1], CONT_LOCKED))
		{
			send_to_char("It's already locked.\n\r", ch);
			return;
		}

		SET_BIT(obj->value[1], CONT_LOCKED);
		send_to_char("*Click*\n\r", ch);
		act("$n locks $p.", ch, obj, 0, TO_ROOM);
		return;
	}

	if((door = find_door(ch, arg)) >= 0)
	{
		/* 'lock door' */
		ROOM_INDEX_DATA *to_room;
		EXIT_DATA *pexit;
		EXIT_DATA *pexit_rev;

		pexit = ch->in_room->exit[door];
		if(!IS_SET(pexit->exit_info, EX_CLOSED))
		{
			send_to_char("It's not closed.\n\r", ch);
			return;
		}
		if(pexit->key < 0)
		{
			send_to_char("It can't be locked.\n\r", ch);
			return;
		}
		if(!has_key(ch, pexit->key))
		{
			send_to_char("You lack the key.\n\r", ch);
			return;
		}
		if(IS_SET(pexit->exit_info, EX_LOCKED))
		{
			send_to_char("It's already locked.\n\r", ch);
			return;
		}

		SET_BIT(pexit->exit_info, EX_LOCKED);
		send_to_char("*Click*\n\r", ch);
		act("$n locks the $d.", ch, 0, pexit->keyword, TO_ROOM);

		/* lock the other side */
		if((to_room = pexit->to_room) != 0
		   && (pexit_rev = to_room->exit[rev_dir[door]]) != 0 && pexit_rev->to_room == ch->in_room)
		{
			SET_BIT(pexit_rev->exit_info, EX_LOCKED);
		}
	}

	return;
}



void do_unlock(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	long door;

	one_argument(argument, arg);

	if(arg[0] == '\0')
	{
		send_to_char("Unlock what?\n\r", ch);
		return;
	}

	if((obj = get_obj_here(ch, arg)) != 0)
	{
		/* 'unlock object' */
		if(obj->item_type != ITEM_CONTAINER)
		{
			send_to_char("That's not a container.\n\r", ch);
			return;
		}
		if(!IS_SET(obj->value[1], CONT_CLOSED))
		{
			send_to_char("It's not closed.\n\r", ch);
			return;
		}
		if(obj->value[2] < 0)
		{
			send_to_char("It can't be unlocked.\n\r", ch);
			return;
		}
		if(!has_key(ch, obj->value[2]))
		{
			send_to_char("You lack the key.\n\r", ch);
			return;
		}
		if(!IS_SET(obj->value[1], CONT_LOCKED))
		{
			send_to_char("It's already unlocked.\n\r", ch);
			return;
		}

		REMOVE_BIT(obj->value[1], CONT_LOCKED);
		send_to_char("*Click*\n\r", ch);
		act("$n unlocks $p.", ch, obj, 0, TO_ROOM);
		return;
	}

	if((door = find_door(ch, arg)) >= 0)
	{
		/* 'unlock door' */
		ROOM_INDEX_DATA *to_room;
		EXIT_DATA *pexit;
		EXIT_DATA *pexit_rev;

		pexit = ch->in_room->exit[door];
		if(!IS_SET(pexit->exit_info, EX_CLOSED))
		{
			send_to_char("It's not closed.\n\r", ch);
			return;
		}
		if(pexit->key < 0)
		{
			send_to_char("It can't be unlocked.\n\r", ch);
			return;
		}
		if(!has_key(ch, pexit->key))
		{
			send_to_char("You lack the key.\n\r", ch);
			return;
		}
		if(!IS_SET(pexit->exit_info, EX_LOCKED))
		{
			send_to_char("It's already unlocked.\n\r", ch);
			return;
		}

		REMOVE_BIT(pexit->exit_info, EX_LOCKED);
		send_to_char("*Click*\n\r", ch);
		act("$n unlocks the $d.", ch, 0, pexit->keyword, TO_ROOM);

		/* unlock the other side */
		if((to_room = pexit->to_room) != 0
		   && (pexit_rev = to_room->exit[rev_dir[door]]) != 0 && pexit_rev->to_room == ch->in_room)
		{
			REMOVE_BIT(pexit_rev->exit_info, EX_LOCKED);
		}
	}

	return;
}



void do_pick(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *gch;
	OBJ_DATA *obj;
	long door;

	one_argument(argument, arg);

	if(arg[0] == '\0')
	{
		send_to_char("Pick what?\n\r", ch);
		return;
	}

	WAIT_STATE(ch, skill_table[gsn_pick_lock].beats);

	/* look for guards */
	for(gch = ch->in_room->people; gch; gch = gch->next_in_room)
	{
		if(IS_NPC(gch) && IS_AWAKE(gch) && ch->level + 5 < gch->level)
		{
			act("$N is standing too close to the lock.", ch, 0, gch, TO_CHAR);
			return;
		}
	}

	if(!IS_NPC(ch) && number_percent() > ch->pcdata->learned[gsn_pick_lock])
	{
		send_to_char("You failed.\n\r", ch);
		return;
	}

	if((obj = get_obj_here(ch, arg)) != 0)
	{
		/* 'pick object' */
		if(obj->item_type != ITEM_CONTAINER)
		{
			send_to_char("That's not a container.\n\r", ch);
			return;
		}
		if(!IS_SET(obj->value[1], CONT_CLOSED))
		{
			send_to_char("It's not closed.\n\r", ch);
			return;
		}
		if(obj->value[2] < 0)
		{
			send_to_char("It can't be unlocked.\n\r", ch);
			return;
		}
		if(!IS_SET(obj->value[1], CONT_LOCKED))
		{
			send_to_char("It's already unlocked.\n\r", ch);
			return;
		}
		if(IS_SET(obj->value[1], CONT_PICKPROOF))
		{
			send_to_char("You failed.\n\r", ch);
			return;
		}

		REMOVE_BIT(obj->value[1], CONT_LOCKED);
		send_to_char("*Click*\n\r", ch);
		act("$n picks $p.", ch, obj, 0, TO_ROOM);
		return;
	}

	if((door = find_door(ch, arg)) >= 0)
	{
		/* 'pick door' */
		ROOM_INDEX_DATA *to_room;
		EXIT_DATA *pexit;
		EXIT_DATA *pexit_rev;

		pexit = ch->in_room->exit[door];
		if(!IS_SET(pexit->exit_info, EX_CLOSED))
		{
			send_to_char("It's not closed.\n\r", ch);
			return;
		}
		if(pexit->key < 0)
		{
			send_to_char("It can't be picked.\n\r", ch);
			return;
		}
		if(!IS_SET(pexit->exit_info, EX_LOCKED))
		{
			send_to_char("It's already unlocked.\n\r", ch);
			return;
		}
		if(IS_SET(pexit->exit_info, EX_PICKPROOF))
		{
			send_to_char("You failed.\n\r", ch);
			return;
		}

		REMOVE_BIT(pexit->exit_info, EX_LOCKED);
		send_to_char("*Click*\n\r", ch);
		act("$n picks the $d.", ch, 0, pexit->keyword, TO_ROOM);

		/* pick the other side */
		if((to_room = pexit->to_room) != 0
		   && (pexit_rev = to_room->exit[rev_dir[door]]) != 0 && pexit_rev->to_room == ch->in_room)
		{
			REMOVE_BIT(pexit_rev->exit_info, EX_LOCKED);
		}
	}

	return;
}




void do_stand(CHAR_DATA * ch, char *argument)
{
	switch (ch->position)
	{
	case POS_SLEEPING:
		if(IS_AFFECTED(ch, AFF_SLEEP))
		{
			send_to_char("You can't wake up!\n\r", ch);
			return;
		}

		send_to_char("You wake and stand up.\n\r", ch);
		act("$n wakes and stands up.", ch, 0, 0, TO_ROOM);
		ch->position = POS_STANDING;
		break;

	case POS_RESTING:
	case POS_SITTING:
		send_to_char("You stand up.\n\r", ch);
		act("$n stands up.", ch, 0, 0, TO_ROOM);
		ch->position = POS_STANDING;
		break;

	case POS_MEDITATING:
		send_to_char("You uncross your legs and stand up.\n\r", ch);
		act("$n uncrosses $s legs and stands up.", ch, 0, 0, TO_ROOM);
		ch->position = POS_STANDING;
		break;

	case POS_STANDING:
		send_to_char("You are already standing.\n\r", ch);
		break;

	case POS_FIGHTING:
		send_to_char("You are already fighting!\n\r", ch);
		break;
	}

	return;
}



void do_rest(CHAR_DATA * ch, char *argument)
{
	switch (ch->position)
	{
	case POS_SLEEPING:
		send_to_char("You are already sleeping.\n\r", ch);
		break;

	case POS_RESTING:
		send_to_char("You are already resting.\n\r", ch);
		break;

	case POS_MEDITATING:
	case POS_SITTING:
	case POS_STANDING:
		send_to_char("You rest.\n\r", ch);
		act("$n rests.", ch, 0, 0, TO_ROOM);
		ch->position = POS_RESTING;
		break;

	case POS_FIGHTING:
		send_to_char("You are already fighting!\n\r", ch);
		break;
	}

	return;
}



void do_sit(CHAR_DATA * ch, char *argument)
{
	switch (ch->position)
	{
	case POS_SLEEPING:
		send_to_char("You are already sleeping.\n\r", ch);
		break;

	case POS_RESTING:
		send_to_char("You are already resting.\n\r", ch);
		break;

	case POS_MEDITATING:
		send_to_char("You are already meditating.\n\r", ch);
		break;

	case POS_SITTING:
		send_to_char("You are already sitting.\n\r", ch);
		break;

	case POS_STANDING:
		send_to_char("You sit down.\n\r", ch);
		act("$n sits down.", ch, 0, 0, TO_ROOM);
		ch->position = POS_SITTING;
		break;

	case POS_FIGHTING:
		send_to_char("You are already fighting!\n\r", ch);
		break;
	}

	return;
}



void do_meditate(CHAR_DATA * ch, char *argument)
{
	if(!IS_CLASS(ch, CLASS_NINJA)
	   && !IS_CLASS(ch, CLASS_MONK) && !IS_CLASS(ch, CLASS_DROW))
	{
		send_to_char("You are unable to attain the correct state of mind.\n\r", ch);
		return;
	}

	switch (ch->position)
	{
	case POS_SLEEPING:
		send_to_char("You are already sleeping.\n\r", ch);
		break;

	case POS_RESTING:
		send_to_char("You are already resting.\n\r", ch);
		break;

	case POS_MEDITATING:
		send_to_char("You are already meditating.\n\r", ch);
		break;

	case POS_SITTING:
		send_to_char("You cross your legs and start meditating.\n\r", ch);
		act("$n crosses $s legs and starts meditating.", ch, 0, 0, TO_ROOM);
		ch->position = POS_MEDITATING;
		break;

	case POS_STANDING:
		send_to_char("You sit down, cross your legs and start meditating.\n\r", ch);
		act("$n sits down, crosses $s legs and starts meditating.", ch, 0, 0, TO_ROOM);
		ch->position = POS_MEDITATING;
		break;

	case POS_FIGHTING:
		send_to_char("You are already fighting!\n\r", ch);
		break;
	}

	return;
}



void do_sleep(CHAR_DATA * ch, char *argument)
{
	switch (ch->position)
	{
	case POS_SLEEPING:
		send_to_char("You are already sleeping.\n\r", ch);
		break;

	case POS_SITTING:
	case POS_MEDITATING:
	case POS_RESTING:
	case POS_STANDING:
		send_to_char("You sleep.\n\r", ch);
		act("$n sleeps.", ch, 0, 0, TO_ROOM);
		ch->position = POS_SLEEPING;
		break;

	case POS_FIGHTING:
		send_to_char("You are already fighting!\n\r", ch);
		break;
	}

	return;
}



void do_wake(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;

	one_argument(argument, arg);
	if(arg[0] == '\0')
	{
		do_stand(ch, argument);
		return;
	}

	if(!IS_AWAKE(ch))
	{
		send_to_char("You are asleep yourself!\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(IS_AWAKE(victim))
	{
		act("$N is already awake.", ch, 0, victim, TO_CHAR);
		return;
	}

	if(IS_AFFECTED(victim, AFF_SLEEP))
	{
		act("You can't wake $M!", ch, 0, victim, TO_CHAR);
		return;
	}

	if(victim->position < POS_SLEEPING)
	{
		act("$E doesn't respond!", ch, 0, victim, TO_CHAR);
		return;
	}

	act("You wake $M.", ch, 0, victim, TO_CHAR);
	act("$n wakes you.", ch, 0, victim, TO_VICT);
	victim->position = POS_STANDING;
	return;
}



void do_sneak(CHAR_DATA * ch, char *argument)
{
	AFFECT_DATA af;

	send_to_char("You attempt to move silently.\n\r", ch);
	affect_strip(ch, gsn_sneak);

	if(IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_sneak])
	{
		af.type = gsn_sneak;
		af.duration = ch->level;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = AFF_SNEAK;
		affect_to_char(ch, &af);
	}

	return;
}



void do_hide(CHAR_DATA * ch, char *argument)
{
	send_to_char("You attempt to hide.\n\r", ch);

	if(IS_AFFECTED(ch, AFF_HIDE))
		REMOVE_BIT(ch->affected_by, AFF_HIDE);

	if(IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_hide])
		SET_BIT(ch->affected_by, AFF_HIDE);

	return;
}



/*
 * Contributed by Alander.
 */
void do_visible(CHAR_DATA * ch, char *argument)
{
	affect_strip(ch, gsn_invis);
	affect_strip(ch, gsn_mass_invis);
	affect_strip(ch, gsn_sneak);
	REMOVE_BIT(ch->affected_by, AFF_HIDE);
	REMOVE_BIT(ch->affected_by, AFF_INVISIBLE);
	REMOVE_BIT(ch->affected_by, AFF_SNEAK);
	REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
	send_to_char("Ok.\n\r", ch);
	return;
}



void do_unpolymorph(CHAR_DATA * ch, char *argument)
{
	if(!is_affected(ch, gsn_polymorph))
	{
		send_to_char("But you are not polymorphed!\n\r", ch);
		return;
	}
	act("$n's body begins to distort.", ch, 0, 0, TO_ROOM);
	affect_strip(ch, gsn_polymorph);
	act("$n resumes $s normal form.", ch, 0, 0, TO_ROOM);
	send_to_char("You resume your normal form.\n\r", ch);
	return;
}



void do_recall(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	CHAR_DATA *mount;
	ROOM_INDEX_DATA *location;

/*
    if ( IS_SET(ch->flag2, AFF2_INARENA) )
    {
      stc("You cannot recall while in the arena.\n\r",ch);
      return;
    }
  */
	act("$n's body flickers with green energy.", ch, 0, 0, TO_ROOM);
	act("Your body flickers with green energy.", ch, 0, 0, TO_CHAR);

	if((location = get_room_index(ch->home)) == 0)
	{
		ch->home = 3001;
		send_to_char("You are completely lost.\n\r", ch);
		send_to_char("Resetting to Temple of Midgaard.\n\r", ch);
		return;
	}

	if(ch->in_room == location)
		return;

	if(IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL) || IS_AFFECTED(ch, AFF_CURSE))
	{
		send_to_char("You are unable to recall.\n\r", ch);
		return;
	}

	if((victim = ch->fighting) != 0)
	{
		if(number_bits(1) == 0)
		{
			WAIT_STATE(ch, 4);
			sprintf(buf, "You failed!\n\r");
			send_to_char(buf, ch);
			return;
		}
		sprintf(buf, "You recall from combat!\n\r");
		send_to_char(buf, ch);
		stop_fighting(ch, TRUE);
	}

	act("$n disappears.", ch, 0, 0, TO_ROOM);
	char_from_room(ch);
	char_to_room(ch, location);
	act("$n appears in the room.", ch, 0, 0, TO_ROOM);
	do_look(ch, "auto");
	if((mount = ch->mount) == 0)
		return;
	char_from_room(mount);
	char_to_room(mount, ch->in_room);
	return;
}

void do_recall2(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	CHAR_DATA *mount;
	ROOM_INDEX_DATA *location;

	act("$n's body flickers with green energy.", ch, 0, 0, TO_ROOM);
	act("Your body flickers with green energy.", ch, 0, 0, TO_CHAR);

	if((location = get_room_index(3001)) == 0)
	{
		send_to_char("You are completely lost.\n\r", ch);
		return;
	}

	if(ch->in_room == location)
		return;

	if(IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL) || IS_AFFECTED(ch, AFF_CURSE))
	{
		send_to_char("You are unable to recall.\n\r", ch);
		return;
	}

	if((victim = ch->fighting) != 0)
	{
		if(number_bits(1) == 0)
		{
			WAIT_STATE(ch, 4);
			sprintf(buf, "You failed!\n\r");
			send_to_char(buf, ch);
			return;
		}
		sprintf(buf, "You recall from combat!\n\r");
		send_to_char(buf, ch);
		stop_fighting(ch, TRUE);
	}

	act("$n disappears.", ch, 0, 0, TO_ROOM);
	char_from_room(ch);
	char_to_room(ch, location);
	act("$n appears in the room.", ch, 0, 0, TO_ROOM);
	do_look(ch, "auto");
	if((mount = ch->mount) == 0)
		return;
	char_from_room(mount);
	char_to_room(mount, ch->in_room);
	return;
}

void do_home(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(arg[0] == '\0' || (str_cmp(arg, "here") && str_cmp(arg, "temple")))
	{
		send_to_char("If you wish this to be your room, you must type 'home here'.\n\r", ch);
		send_to_char("OR you can type 'home temple' to set temple in midgaard as recall.\n\r", ch);
		return;
	}
	if(!str_cmp(arg, "here"))
	{
		if(ch->in_room->vnum == ch->home)
		{
			send_to_char("But this is already your home!\n\r", ch);
			return;
		}

		if(IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL) || IS_SET(ch->in_room->room_flags, ROOM_SAFE))
		{
			send_to_char("You are unable to make this room your home.\n\r", ch);
			return;
		}

		ch->home = ch->in_room->vnum;
		send_to_char("This room is now your home.\n\r", ch);
	}
	else if(!str_cmp(arg, "temple"))
	{
		if(ch->in_room->vnum == ch->home)
		{
			send_to_char("But this already is your home.\n\r", ch);
			return;
		}

		ch->home = ROOM_VNUM_TEMPLE;
		send_to_char("The temple is now your home.\n\r", ch);
	}
	return;
}


void do_escape(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	ROOM_INDEX_DATA *location;

	if(IS_NPC(ch) || !IS_HERO(ch))
		return;

	if(ch->position >= POS_SLEEPING)
	{
		send_to_char("You can only do this if you are dying.\n\r", ch);
		return;
	}

	if((location = get_room_index(ROOM_VNUM_TEMPLE)) == 0)
	{
		send_to_char("You are completely lost.\n\r", ch);
		return;
	}

	if(in_tournament(ch, ch))
		return;

	if(ch->in_room == location)
		return;

	ch->move = 0;
	ch->mana = 0;
	act("$n fades out of existance.", ch, 0, 0, TO_ROOM);
	char_from_room(ch);
	char_to_room(ch, location);
	act("$n fades into existance.", ch, 0, 0, TO_ROOM);
	do_look(ch, "auto");
	sprintf(buf, "%s has escaped defenceless from a fight.", ch->name);
	do_info(ch, buf);

	return;
}

void do_train(CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_STRING_LENGTH];
	char arg2[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];
	long *pAbility;
	char *pOutput;
	long cost;
	long magic;
	long immcost;
	long amount = -1;
	long primal;
	long silver;
	long max_stat = 18;
	bool last = TRUE;
	bool is_ok = FALSE;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if(IS_NPC(ch))
		return;

	if(amount == 0)
		return;

	if(arg1[0] == '\0')
	{
		sprintf(buf, "You have %li experience points.\n\r", ch->exp);
		send_to_char(buf, ch);
		strcpy(arg1, "foo");
	}

	if(!str_cmp(arg1, "str"))
		is_ok = TRUE;
	else if(!str_cmp(arg1, "int"))
		is_ok = TRUE;
	else if(!str_cmp(arg1, "wis"))
		is_ok = TRUE;
	else if(!str_cmp(arg1, "dex"))
		is_ok = TRUE;
	else if(!str_cmp(arg1, "con"))
		is_ok = TRUE;
	else if(!str_cmp(arg1, "hp"))
		is_ok = TRUE;
	else if(!str_cmp(arg1, "mana"))
		is_ok = TRUE;
	else if(!str_cmp(arg1, "primal"))
		is_ok = TRUE;
	else if(!str_cmp(arg1, "move") && IS_CLASS(ch, CLASS_HIGHLANDER))
	{
		send_to_char("Highlanders must decapitate players to improve their movement.\n\r", ch);
		return;
	}
	else if(!str_cmp(arg1, "move"))
		is_ok = TRUE;
	else if(!str_cmp(arg1, "silver") && IS_CLASS(ch, CLASS_WEREWOLF))
		is_ok = TRUE;
	else if(!str_cmp(arg1, "ancilla") && IS_CLASS(ch, CLASS_VAMPIRE))
		is_ok = TRUE;
	else if(!str_cmp(arg1, "elder") && IS_CLASS(ch, CLASS_VAMPIRE))
		is_ok = TRUE;
	else if(!str_cmp(arg1, "methuselah") && IS_CLASS(ch, CLASS_VAMPIRE))
		is_ok = TRUE;
	else if(!str_cmp(arg1, "magic") && IS_CLASS(ch, CLASS_DROW))
		is_ok = TRUE;

	if(arg2[0] != '\0' && is_ok)
	{
		if (!str_cmp(arg2,"all"))
			amount = 50;
		if(!is_number(arg2) && str_cmp(arg2,"all"))
		{
			send_to_char("Please enter a numeric value.\n\r", ch);
			return;
		}
		if (str_cmp(arg2,"all"))
			amount = atoi(arg2);
		if(amount < 1 || amount > 50)
		{
		stc("Please enter a value greater than 1 and less than 50.\n\r",ch);
//			send_to_char("Please enter a value greater than 1 or type all to train all your exp in one area.\n\r",  ch);
			return;
		}

		if(amount > 1)
		{
			sprintf(buf, "%s %li", arg1, amount - 1);
			do_train(ch, buf);
			last = FALSE;
		}
	}
	cost = 200;
	immcost = count_imms(ch) * 10;
	primal = (1 + ch->practice) * 500;
	magic = (1 + ch->pcdata->stats[DROW_MAGIC]) * 1000;
	silver = (1 + ch->pcdata->powers[WPOWER_SILVER]) * 2500;
	if(IS_CLASS(ch, CLASS_HIGHLANDER))
		max_stat = 25;

	if(ch->max_hit < 1 || ch->max_mana < 1 || ch->max_move < 1)
	{
		send_to_char("You can't train with negative stats, remove the training equip.\n\r", ch);
		return;
	}

	if(!str_cmp(arg1, "str"))
	{
		pAbility = &ch->pcdata->perm_str;
		pOutput = "strength";
	}

	else if(!str_cmp(arg1, "int"))
	{
		pAbility = &ch->pcdata->perm_int;
		pOutput = "intelligence";
	}

	else if(!str_cmp(arg1, "wis"))
	{
		pAbility = &ch->pcdata->perm_wis;
		pOutput = "wisdom";
	}

	else if(!str_cmp(arg1, "dex"))
	{
		pAbility = &ch->pcdata->perm_dex;
		pOutput = "dexterity";
	}

	else if(!str_cmp(arg1, "con"))
	{
		pAbility = &ch->pcdata->perm_con;
		pOutput = "constitution";
	}

	else if(!str_cmp(arg1, "avatar") && (ch->level == 2))
	{
		cost = 1000;
		pAbility = &ch->level;
		pOutput = "level";
	}

	else if(!str_cmp(arg1, "hp") && ch->max_hit < 30000)
	{
		cost = (ch->max_hit - ch->pcdata->perm_con) * 2;
		pAbility = &ch->max_hit;
		pOutput = "hp";
	}

	else if(!str_cmp(arg1, "mana") && ch->max_mana < 30000)
	{
		cost = (ch->max_mana - ch->pcdata->perm_wis);
		pAbility = &ch->max_mana;
		pOutput = "mana";
	}

	else if(!str_cmp(arg1, "move") && ch->max_move < 30000)
	{
		cost = (ch->max_move - ch->pcdata->perm_con);
		pAbility = &ch->max_move;
		pOutput = "move";
	}

	else if(!str_cmp(arg1, "primal") && ch->practice < 1000)
	{
		cost = primal;
		pAbility = &ch->practice;
		pOutput = "primal";
	}
	else if(!str_cmp(arg1, "magic") && IS_CLASS(ch, CLASS_DROW) && ch->pcdata->stats[DROW_MAGIC] < 100)
	{
		long quiet_pointer = ch->pcdata->stats[DROW_MAGIC];

		cost = magic;
		pAbility = &quiet_pointer;
		pOutput = "drow magic resistance";
	}

	else if(!str_cmp(arg1, "silver") && IS_CLASS(ch, CLASS_WEREWOLF) && ch->pcdata->powers[WPOWER_SILVER] < 100)
	{
		long quiet_pointer = ch->pcdata->powers[WPOWER_SILVER];	/*
									   dirrent type of long */
		cost = silver;
		pAbility = &quiet_pointer;
		pOutput = "tolerance to silver";
	}

	else if(!str_cmp(arg1, "ancilla") && IS_CLASS(ch, CLASS_VAMPIRE)
		&& ch->pcdata->rank != AGE_ANCILLA && ch->pcdata->rank != AGE_ELDER && ch->pcdata->rank != AGE_METHUSELAH)
	{
		if(ch->exp < 2000000)
		{
			send_to_char("You don't have enough exp.\n\r", ch);
			return;
		}
		ch->exp = ch->exp - 2000000;
		ch->pcdata->rank = AGE_ANCILLA;
		send_to_char("You are now an Ancilla.\n\r", ch);
		return;
	}

	else if(!str_cmp(arg1, "elder") && IS_CLASS(ch, CLASS_VAMPIRE)
		&& ch->pcdata->rank != AGE_ELDER && ch->pcdata->rank == AGE_ANCILLA && ch->pcdata->rank != AGE_METHUSELAH)
	{
		if(ch->exp < 6000000)
		{
			send_to_char("You don't have enough exp.\n\r", ch);
			return;
		}
		ch->exp = ch->exp - 6000000;
		ch->pcdata->rank = AGE_ELDER;
		send_to_char("You are now an Elder.\n\r", ch);
		return;
	}

	else if(!str_cmp(arg1, "methuselah") && IS_CLASS(ch, CLASS_VAMPIRE)
		&& ch->pcdata->rank != AGE_METHUSELAH && ch->pcdata->rank != AGE_ANCILLA && ch->pcdata->rank == AGE_ELDER)
	{
		if(ch->exp < 10000000)
		{
			send_to_char("You don't have enough exp.\n\r", ch);
			return;
		}
		ch->exp = ch->exp - 10000000;
		ch->pcdata->rank = AGE_METHUSELAH;
		send_to_char("You are now a Methuselah.\n\r", ch);
		return;
	}
	else if(!str_cmp(arg1, "generation"))
	{
		if(ch->pcdata->stats[UNI_GEN] == 1)
		{
			send_to_char("You are already a master of your class!\n\r", ch);
			return;
		}

		if(ch->pcdata->stats[UNI_GEN] == 2)
		{
			if(ch->exp < 50000000)
			{
				send_to_char("It costs 50000000 experience points to become a master of your class.\n\r",
					     ch);
				return;
			}

			ch->exp -= 50000000;
			ch->pcdata->stats[UNI_GEN] = 1;
			sprintf(buf, "%s is now a Master %s!", ch->name, class_name(ch->pcdata->class));
			do_info(ch, buf);
			return;
		}

		if(ch->pcdata->stats[UNI_GEN] == 3)
		{
			if(ch->exp < 25000000)
			{
				send_to_char
					("It costs 25000000 experience points to become an apprentice of your class.\n\r",
					 ch);
				return;
			}

			ch->exp -= 25000000;
			ch->pcdata->stats[UNI_GEN] = 2;
			sprintf(buf, "%s is now an Apprentice %s!", ch->name, class_name(ch->pcdata->class));
			do_info(ch, buf);
			return;
		}

		ch->pcdata->stats[UNI_GEN] = 3;
		send_to_char("Your generation is reset to normal.\n\r", ch);
		return;
	}

	else if(!str_cmp(arg1, "kick") && !IS_IMMUNE(ch, IMM_KICK))
	{
		if(ch->exp < immcost)
		{
			send_to_char("You don't have enough exp.\n\r", ch);
			return;
		}
		ch->exp = ch->exp - immcost;
		SET_BIT(ch->immune, IMM_KICK);
		send_to_char("You are now more resistant to being kicked.\n\r", ch);
		return;
	}

	else if(!str_cmp(arg1, "disarm") && !IS_IMMUNE(ch, IMM_DISARM))
	{
		if(ch->exp < immcost)
		{
			send_to_char("You don't have enough exp.\n\r", ch);
			return;
		}
		ch->exp = ch->exp - immcost;
		SET_BIT(ch->immune, IMM_DISARM);
		send_to_char("You are now immune to being disarmed.\n\r", ch);
		return;
	}

	else if(!str_cmp(arg1, "steal") && !IS_IMMUNE(ch, IMM_STEAL))
	{
		if(ch->exp < immcost)
		{
			send_to_char("You don't have enough exp.\n\r", ch);
			return;
		}
		ch->exp = ch->exp - immcost;
		SET_BIT(ch->immune, IMM_STEAL);
		send_to_char("You are now immune to being stolen from.\n\r", ch);
		return;
	}
	else
	{
		sprintf(buf, "You can train the following:\n\r");
		send_to_char(buf, ch);

		send_to_char("Stats:", ch);
		if(ch->pcdata->perm_str < max_stat)
			send_to_char(" Str", ch);
		if(ch->pcdata->perm_int < max_stat)
			send_to_char(" Int", ch);
		if(ch->pcdata->perm_wis < max_stat)
			send_to_char(" Wis", ch);
		if(ch->pcdata->perm_dex < max_stat)
			send_to_char(" Dex", ch);
		if(ch->pcdata->perm_con < max_stat)
			send_to_char(" Con", ch);
		if((ch->pcdata->perm_str >= max_stat)
		   && (ch->pcdata->perm_wis >= max_stat)
		   && (ch->pcdata->perm_int >= max_stat)
		   && (ch->pcdata->perm_dex >= max_stat) && (ch->pcdata->perm_con >= max_stat))
			send_to_char(" None left to train.\n\r", ch);
		else
			send_to_char(".\n\r", ch);

		if(ch->level == 2)
		{
			sprintf(buf, "Become an avatar - 1000 exp.\n\r");
			send_to_char(buf, ch);
		}

		if(ch->pcdata->stats[UNI_GEN] > 1 || ch->pcdata->stats[UNI_GEN] < 1)
		{
			if(ch->pcdata->stats[UNI_GEN] > 3 || ch->pcdata->stats[UNI_GEN] < 1)
				sprintf(buf, "Generation       - Screwed up.  Type train generation.\n\r");
			else
				sprintf(buf, "Generation       - %d exp to be a %s %s.\n\r",
					ch->pcdata->stats[UNI_GEN] == 3 ? 25000000 : 50000000,
					ch->pcdata->stats[UNI_GEN] == 3 ? "Apprentice" : "Master",
					class_name(ch->pcdata->class));
			send_to_char(buf, ch);
		}

		if(ch->max_hit < 30000)
		{
			sprintf(buf, "Hp               - %li exp per point.\n\r", (ch->max_hit - ch->pcdata->perm_con) * 2);
			send_to_char(buf, ch);
		}
		if(ch->max_mana < 30000)
		{
			sprintf(buf, "Mana             - %li exp per point.\n\r", (ch->max_mana - ch->pcdata->perm_wis));
			send_to_char(buf, ch);
		}
		if(ch->max_move < 30000)
		{
			sprintf(buf, "Move             - %li exp per point.\n\r", (ch->max_move - ch->pcdata->perm_con));
			send_to_char(buf, ch);
		}
		if(ch->practice < 100)
		{
			sprintf(buf, "Primal           - %li exp per point of primal energy.\n\r", primal);
			send_to_char(buf, ch);
		}
		if(ch->pcdata->powers[WPOWER_SILVER] < 100 && IS_CLASS(ch, CLASS_WEREWOLF))
			{
			sprintf(buf, "Silver tolerance - %li exp per point of tolerance.\n\r", silver);
			send_to_char(buf, ch);
		}
		if(IS_CLASS(ch, CLASS_VAMPIRE) && ch->pcdata->rank != AGE_ANCILLA
		   && ch->pcdata->rank != AGE_ELDER && ch->pcdata->rank != AGE_METHUSELAH)
		{
			sprintf(buf, "Ancilla: You need 2 million exp to be an Ancilla.\n\r");
			send_to_char(buf, ch);
		}

		if(IS_CLASS(ch, CLASS_VAMPIRE) && ch->pcdata->rank != AGE_ELDER
		   && ch->pcdata->rank == AGE_ANCILLA && ch->pcdata->rank != AGE_METHUSELAH)
		{
			sprintf(buf, "Elder: You need 6 million exp to be an Elder.\n\r");
			send_to_char(buf, ch);
		}

		if(IS_CLASS(ch, CLASS_VAMPIRE) && ch->pcdata->rank != AGE_METHUSELAH
		   && ch->pcdata->rank == AGE_ELDER && ch->pcdata->rank != AGE_ANCILLA)
		{
			sprintf(buf, "Methuselah: You need 10 million exp to be an Methuselah.\n\r");
			send_to_char(buf, ch);
		}
		if(ch->pcdata->stats[DROW_MAGIC] < 100 && IS_CLASS(ch, CLASS_DROW))
		{
			sprintf(buf, "Magic resistance - %li drow points per point of magic.\n\r", magic);
			send_to_char(buf, ch);
		}

		sprintf(buf, "Natural resistances and immunities - %li exp each.\n\r", immcost);
		send_to_char(buf, ch);

		/* Weapon resistance affects
		   send_to_char( "Weapon resistances:", ch );
		   if ( !IS_IMMUNE(ch, IMM_SLASH)  ) send_to_char( " Slash", ch );
		   if ( !IS_IMMUNE(ch, IMM_STAB)   ) send_to_char( " Stab", ch );
		   if ( !IS_IMMUNE(ch, IMM_SMASH)  ) send_to_char( " Smash", ch );
		   if ( !IS_IMMUNE(ch, IMM_ANIMAL) ) send_to_char( " Beast", ch );
		   if ( !IS_IMMUNE(ch, IMM_MISC)   ) send_to_char( " Grab", ch );
		   if ( IS_IMMUNE(ch, IMM_SLASH) && IS_IMMUNE(ch, IMM_STAB) &&
		   IS_IMMUNE(ch, IMM_SMASH) && IS_IMMUNE(ch, IMM_ANIMAL) &&
		   IS_IMMUNE(ch, IMM_MISC) )
		   send_to_char( " None left to learn.\n\r", ch );
		   else
		   send_to_char( ".\n\r", ch ); */

		/* Spell immunity affects */
		send_to_char("Magical immunities:", ch);
/*	if ( !IS_IMMUNE(ch, IMM_HEAT)      ) send_to_char( " Heat", ch );
//	if ( !IS_IMMUNE(ch, IMM_COLD)      ) send_to_char( " Cold", ch );
	if ( !IS_IMMUNE(ch, IMM_LIGHTNING) ) send_to_char( " Lightning", ch );
	if ( !IS_IMMUNE(ch, IMM_ACID)      ) send_to_char( " Acid", ch ); */
//      if ( !IS_IMMUNE(ch, IMM_SLEEP)     ) send_to_char( " Sleep", ch );
//      if ( !IS_IMMUNE(ch, IMM_DRAIN)     ) send_to_char( " Drain", ch );
/*	if ( !IS_IMMUNE(ch, IMM_VOODOO)    ) send_to_char( " Voodoo", ch  ); */
		if(IS_IMMUNE(ch, IMM_LIGHTNING) && IS_IMMUNE(ch, IMM_ACID))
			send_to_char(" None left to learn.\n\r", ch);
		else
			send_to_char(".\n\r", ch);

		/* Skill immunity affects */
		send_to_char("Skill immunities:", ch);
		if(!IS_IMMUNE(ch, IMM_KICK))
			send_to_char(" Kick", ch);
		if(!IS_IMMUNE(ch, IMM_DISARM))
			send_to_char(" Disarm", ch);
		if(!IS_IMMUNE(ch, IMM_STEAL))
			send_to_char(" Steal", ch);
		if(IS_IMMUNE(ch, IMM_KICK) && IS_IMMUNE(ch, IMM_DISARM) && IS_IMMUNE(ch, IMM_STEAL))
			send_to_char(" None left to learn.\n\r", ch);
		else
			send_to_char(".\n\r", ch);
		return;
	}


	if((*pAbility >= max_stat) && (!str_cmp(arg1, "str")))
	{
		if(last)
			act("Your $T is already at maximum.", ch, 0, pOutput, TO_CHAR);
		return;
	}
	if((*pAbility >= max_stat) && (!str_cmp(arg1, "long")))
	{
		if(last)
			act("Your $T is already at maximum.", ch, 0, pOutput, TO_CHAR);
		return;
	}
	if((*pAbility >= max_stat) && (!str_cmp(arg1, "wis")))
	{
		if(last)
			act("Your $T is already at maximum.", ch, 0, pOutput, TO_CHAR);
		return;
	}
	if((*pAbility >= max_stat) && (!str_cmp(arg1, "dex")))
	{
		if(last)
			act("Your $T is already at maximum.", ch, 0, pOutput, TO_CHAR);
		return;
	}
	if((*pAbility >= max_stat) && (!str_cmp(arg1, "con")))
	{
		if(last)
			act("Your $T is already at maximum.", ch, 0, pOutput, TO_CHAR);
		return;
	}
	if((*pAbility >= 30000) && (!str_cmp(arg1, "hp")))
	{
		if(last)
			act("Your $T is already at maximum.", ch, 0, pOutput, TO_CHAR);
		return;
	}
	if((*pAbility >= 30000) && (!str_cmp(arg1, "mana")))
	{
		if(last)
			act("Your $T is already at maximum.", ch, 0, pOutput, TO_CHAR);
		return;
	}
	if((*pAbility >= 30000) && (!str_cmp(arg1, "move")))
	{
		if(last)
			act("Your $T is already at maximum.", ch, 0, pOutput, TO_CHAR);
		return;
	}
	if((*pAbility >= 1000) && (!str_cmp(arg1, "primal")))
	{
		if(last)
			act("Your $T is already at maximum.", ch, 0, pOutput, TO_CHAR);
		return;
	}
	if((*pAbility >= 100) && (!str_cmp(arg1, "magic")))
	{
		if(last)
			act("Your $T is already at a maximum.", ch, 0, pOutput, TO_CHAR);
		return;
	}

	if((*pAbility >= 100) && (!str_cmp(arg1, "silver")))
	{
		if(last)
			act("Your $T is already at maximum.", ch, 0, pOutput, TO_CHAR);
		return;
	}

	if(cost < 1)
		cost = 1;
	if(cost > ch->pcdata->stats[DROW_POWER] && !str_cmp(arg1, "magic"))
	{
		if(last)
			send_to_char("You don't have enough drow power points.\n\r", ch);
		return;
	}
	else if(cost > ch->exp)
	{
		if(last)
			send_to_char("You don't have enough exp.\n\r", ch);
		return;
	}
	if(!str_cmp(arg1, "magic"))
		ch->pcdata->stats[DROW_POWER] -= cost;
	else
		ch->exp -= cost;
	if(!str_cmp(arg1, "silver"))
		ch->pcdata->powers[WPOWER_SILVER] += 1;
	else if(!str_cmp(arg1, "magic"))
		ch->pcdata->stats[DROW_MAGIC] += 1;
	else
		*pAbility += 1;
	if(!str_cmp(arg1, "avatar"))
	{

		act("You become an avatar!", ch, 0, 0, TO_CHAR);
		sprintf(buf, "%s has become an avatar!", ch->name);
		do_info(ch, buf);
		ch->fight_timer = 0;
		if(IS_CLASS(ch, CLASS_DEMON))
			ch->alignment = -1000;
/*	if (ch->level < ch->trust) ch->level = ch->trust;
	if (!IS_NPC(ch) && IS_VAMPAFF(ch, VAM_MORTAL)) */
		if(IS_CLASS(ch, CLASS_VAMPIRE) && IS_VAMPAFF(ch, VAM_MORTAL))
		{
			do_mortalvamp(ch, "");
		}
		do_sset(ch, "self all 100");
		ch->level = 3;
		ch->trust = 3;
	}

	else if(last)
		act("Your $T increases!", ch, 0, pOutput, TO_CHAR);
	return;
}

void do_mount(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;

	argument = one_argument(argument, arg);

	if(arg[0] == '\0')
	{
		send_to_char("Mount what?\n\r", ch);
		return;
	}

	if(IS_AFFECTED(ch, AFF_POLYMORPH) && !IS_VAMPAFF(ch, VAM_DISGUISED))
	{
		send_to_char("You cannot ride in this form.\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(ch == victim)
	{
		send_to_char("You cannot ride on your own back!\n\r", ch);
		return;
	}

	if(ch->mounted > 0)
	{
		send_to_char("You are already riding.\n\r", ch);
		return;
	}

	if(!IS_NPC(victim) || victim->mounted > 0 || (IS_NPC(victim) && !IS_SET(victim->act, ACT_MOUNT)))
	{
		send_to_char("You cannot mount them.\n\r", ch);
		return;
	}

	if(victim->position < POS_STANDING)
	{
		if(victim->position < POS_SLEEPING)
			act("$N is too badly hurt for that.", ch, 0, victim, TO_CHAR);
		else if(victim->position == POS_SLEEPING)
			act("First you better wake $m up.", ch, 0, victim, TO_CHAR);
		else if(victim->position == POS_RESTING)
			act("First $e better stand up.", ch, 0, victim, TO_CHAR);
		else if(victim->position == POS_MEDITATING)
			act("First $e better stand up.", ch, 0, victim, TO_CHAR);
		else if(victim->position == POS_SITTING)
			act("First $e better stand up.", ch, 0, victim, TO_CHAR);
		else if(victim->position == POS_SLEEPING)
			act("First you better wake $m up.", ch, 0, victim, TO_CHAR);
		else if(victim->position == POS_FIGHTING)
			act("Not while $e's fighting.", ch, 0, victim, TO_CHAR);
		return;
	}

	if(!IS_NPC(ch) && ch->stance[0] != -1)
		do_stance(ch, "");

	ch->mounted = IS_RIDING;
	victim->mounted = IS_MOUNT;
	ch->mount = victim;
	victim->mount = ch;

	act("You clamber onto $N's back.", ch, 0, victim, TO_CHAR);
	act("$n clambers onto $N's back.", ch, 0, victim, TO_ROOM);
	return;
}

void do_dismount(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;

	if(ch->mounted == 0)
	{
		send_to_char("But you are not riding!\n\r", ch);
		return;
	}

	if((victim = ch->mount) == 0)
	{
		send_to_char("But you are not riding!\n\r", ch);
		return;
	}

	act("You clamber off $N's back.", ch, 0, victim, TO_CHAR);
	act("$n clambers off $N's back.", ch, 0, victim, TO_ROOM);

	ch->mounted = IS_ON_FOOT;
	victim->mounted = IS_ON_FOOT;

	ch->mount = 0;
	victim->mount = 0;

	return;
}

void do_tie(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);
	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}
	if(victim == ch)
	{
		send_to_char("You cannot tie yourself up!\n\r", ch);
		return;
	}
	if(IS_EXTRA(victim, TIED_UP))
	{
		send_to_char("But they are already tied up!\n\r", ch);
		return;
	}
	if(victim->position > POS_STUNNED || victim->hit > 0)
	{
		send_to_char("You can only tie up a defenceless person.\n\r", ch);
		return;
	}
	act("You quickly tie up $N.", ch, 0, victim, TO_CHAR);
	act("$n quickly ties up $N.", ch, 0, victim, TO_ROOM);
	send_to_char("You have been tied up!\n\r", victim);
	SET_BIT(victim->extra, TIED_UP);
	sprintf(buf, "%s has been tied up by %s.", victim->name, ch->name);
	do_info(ch, buf);
	return;
}

void do_untie(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);
	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}
	if(!IS_EXTRA(victim, TIED_UP))
	{
		send_to_char("But they are not tied up!\n\r", ch);
		return;
	}
	if(victim == ch)
	{
		send_to_char("You cannot untie yourself!\n\r", ch);
		return;
	}
	act("You quickly untie $N.", ch, 0, victim, TO_CHAR);
	act("$n quickly unties $N.", ch, 0, victim, TO_NOTVICT);
	act("$n quickly unties you.", ch, 0, victim, TO_VICT);
	REMOVE_BIT(victim->extra, TIED_UP);
	return;
}

void do_gag(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);
	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}
	if(victim == ch && !IS_EXTRA(victim, GAGGED) && IS_EXTRA(victim, TIED_UP))
	{
		send_to_char("You cannot gag yourself!\n\r", ch);
		return;
	}
	if(!IS_EXTRA(victim, TIED_UP) && !IS_EXTRA(victim, GAGGED))
	{
		send_to_char("You can only gag someone who is tied up!\n\r", ch);
		return;
	}
	if(!IS_EXTRA(victim, GAGGED))
	{
		act("You place a gag over $N's mouth.", ch, 0, victim, TO_CHAR);
		act("$n places a gag over $N's mouth.", ch, 0, victim, TO_NOTVICT);
		act("$n places a gag over your mouth.", ch, 0, victim, TO_VICT);
		SET_BIT(victim->extra, GAGGED);
		return;
	}
	if(ch == victim)
	{
		act("You remove the gag from your mouth.", ch, 0, victim, TO_CHAR);
		act("$n removes the gag from $s mouth.", ch, 0, victim, TO_ROOM);
		REMOVE_BIT(victim->extra, GAGGED);
		return;
	}
	act("You remove the gag from $N's mouth.", ch, 0, victim, TO_CHAR);
	act("$n removes the gag from $N's mouth.", ch, 0, victim, TO_NOTVICT);
	act("$n removes the gag from your mouth.", ch, 0, victim, TO_VICT);
	REMOVE_BIT(victim->extra, GAGGED);
	return;
}

void do_blindfold(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);
	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}
	if(victim == ch && !IS_EXTRA(victim, BLINDFOLDED) && IS_EXTRA(victim, TIED_UP))
	{
		send_to_char("You cannot blindfold yourself!\n\r", ch);
		return;
	}
	if(!IS_EXTRA(victim, TIED_UP) && !IS_EXTRA(victim, BLINDFOLDED))
	{
		send_to_char("You can only blindfold someone who is tied up!\n\r", ch);
		return;
	}
	if(!IS_EXTRA(victim, BLINDFOLDED))
	{
		act("You place a blindfold over $N's eyes.", ch, 0, victim, TO_CHAR);
		act("$n places a blindfold over $N's eyes.", ch, 0, victim, TO_NOTVICT);
		act("$n places a blindfold over your eyes.", ch, 0, victim, TO_VICT);
		SET_BIT(victim->extra, BLINDFOLDED);
		return;
	}
	if(ch == victim)
	{
		act("You remove the blindfold from your eyes.", ch, 0, victim, TO_CHAR);
		act("$n removes the blindfold from $s eyes.", ch, 0, victim, TO_ROOM);
		REMOVE_BIT(victim->extra, BLINDFOLDED);
		return;
	}
	act("You remove the blindfold from $N's eyes.", ch, 0, victim, TO_CHAR);
	act("$n removes the blindfold from $N's eyes.", ch, 0, victim, TO_NOTVICT);
	act("$n removes the blindfold from your eyes.", ch, 0, victim, TO_VICT);
	REMOVE_BIT(victim->extra, BLINDFOLDED);
	return;
}


long count_imms(CHAR_DATA * ch)
{
	long count = 0;

	if(IS_IMMUNE(ch, IMM_SLASH))
		count += 1;
	if(IS_IMMUNE(ch, IMM_STAB))
		count += 1;
	if(IS_IMMUNE(ch, IMM_SMASH))
		count += 1;
	if(IS_IMMUNE(ch, IMM_ANIMAL))
		count += 1;
	if(IS_IMMUNE(ch, IMM_MISC))
		count += 1;
	if(IS_IMMUNE(ch, IMM_CHARM))
		count += 1;
	if(IS_IMMUNE(ch, IMM_HEAT))
		count += 1;
	if(IS_IMMUNE(ch, IMM_COLD))
		count += 1;
	if(IS_IMMUNE(ch, IMM_LIGHTNING))
		count += 1;
	if(IS_IMMUNE(ch, IMM_ACID))
		count += 1;
	if(IS_IMMUNE(ch, IMM_SLEEP))
		count += 1;
	if(IS_IMMUNE(ch, IMM_DRAIN))
		count += 1;
	if(IS_IMMUNE(ch, IMM_VOODOO))
		count += 1;
	if(IS_IMMUNE(ch, IMM_HURL))
		count += 1;
	if(IS_IMMUNE(ch, IMM_BACKSTAB))
		count += 1;
	if(IS_IMMUNE(ch, IMM_KICK))
		count += 1;
	if(IS_IMMUNE(ch, IMM_DISARM))
		count += 1;
	if(IS_IMMUNE(ch, IMM_STEAL))
		count += 1;
	return ((count * 10000) + 10000);
}

/*
void do_track( CHAR_DATA *ch, char *argument )
{
    bool found = FALSE;

    if (!IS_NPC(ch) && number_percent() > ch->pcdata->learned[gsn_track])
    {
	send_to_char("You cannot sense any trails from this room.\n\r",ch);
	return;
    }



    if (check_track(ch,0)) found = TRUE;
    if (check_track(ch,1)) found = TRUE;
    if (check_track(ch,2)) found = TRUE;
    if (check_track(ch,3)) found = TRUE;
    if (check_track(ch,4)) found = TRUE;
    if (found == FALSE)
    {
	send_to_char("You cannot sense any trails from this room.\n\r",ch);
	return;
    }
    act("$n carefully examines the ground for tracks.",ch,0,0,TO_ROOM);
    return;
}

void do_hunt( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if ( !IS_CLASS(ch, CLASS_WEREWOLF) && !IS_CLASS(ch,CLASS_NINJA))
    {
      send_to_char("Huh?\n\r", ch);
     return;
    }

    if ( arg[0] == '\0' )
    {
	if (strlen(ch->hunting) > 1)
	{
	    free_string(ch->hunting);
	    ch->hunting = str_dup( "" );
	    send_to_char( "You stop hunting your prey.\n\r", ch );
	}
	else send_to_char( "Who do you wish to hunt?\n\r", ch );
	return;
    }
    if (!str_cmp(arg,ch->name))
    {
	send_to_char("How can you hunt yourself?\n\r",ch);
	return;
    }

    if ( IS_CLASS(ch, CLASS_WEREWOLF) && ch->pcdata->powers[WPOWER_LYNX] <
1 )
    {
        send_to_char("Huh?\n\r", ch);
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == 0 )
    {
        send_to_char( "They are not online.\n\r", ch );
        return;
    }

    ch->hunting = str_dup(arg);
    check_hunt(ch);
    send_to_char( "Ok.\n\r", ch );
    return;
}

void check_hunt( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    bool found = FALSE;
    long direction = 0;
    ROOM_INDEX_DATA *in_room;

    in_room = ch->in_room;
    if (!IS_NPC(ch) && number_percent() > ch->pcdata->learned[gsn_track])
    {
	send_to_char("You cannot sense any trails from this room.\n\r",ch);
	free_string(ch->hunting);
	ch->hunting = str_dup( "" );
	return;
    }

    if (check_track(ch,0)) {found = TRUE;direction =
ch->in_room->track_dir[0];}
    else if (check_track(ch,1)) {found = TRUE;direction =
ch->in_room->track_dir[1];}
    else if (check_track(ch,2)) {found = TRUE;direction =
ch->in_room->track_dir[2];}
    else if (check_track(ch,3)) {found = TRUE;direction =
ch->in_room->track_dir[3];}
    else if (check_track(ch,4)) {found = TRUE;direction =
ch->in_room->track_dir[4];}
    else if (check_track(ch,5)) {found = TRUE;direction =
ch->in_room->track_dir[5];}
    else if ( ( victim = get_char_room( ch, ch->hunting ) ) == 0 )
    {
	send_to_char("You cannot sense any trails from this room.\n\r",ch);
	free_string(ch->hunting);
	ch->hunting = str_dup( "" );
	return;
    }
    if (strlen(ch->hunting) < 2) return;
    if ( ( victim = get_char_room( ch, ch->hunting ) ) != 0 ) return;
    act("$n carefully examines the ground for tracks.",ch,0,0,TO_ROOM);
    move_char(ch,direction);

    if (in_room == ch->in_room || victim != 0)
    {
	free_string(ch->hunting);
	ch->hunting = str_dup( "" );
    }
    return;
}

void add_tracks( CHAR_DATA *ch, long direction )
{
    long loop;

    if (IS_NPC(ch)) return;
    if ( (IS_CLASS(ch, CLASS_WEREWOLF) && ch->pcdata->powers[WPOWER_LYNX]
> 0)
    || (IS_CLASS(ch, CLASS_NINJA) && ch->pcdata->powers[NPOWER_SORA] > 3)
)
	return;
    if (IS_ITEMAFF(ch, ITEMA_STALKER)) return;
    for ( loop = 0; loop <= 4; loop++ )
    {
    	if (ch->in_room->track[loop] != 0 &&
!str_cmp(ch->in_room->track[loop],ch->name))
    	{
	    free_string(ch->in_room->track[loop]);
	    ch->in_room->track[loop] = str_dup("");
    	}
    }
    if (strlen(ch->in_room->track[0]) < 2)
    {
	free_string(ch->in_room->track[0]);
	ch->in_room->track[0]     = str_dup(ch->name);
	ch->in_room->track_dir[0] = direction;
    }
    else if (strlen(ch->in_room->track[1]) < 2)
    {
	free_string(ch->in_room->track[1]);
	ch->in_room->track[1]     = str_dup(ch->name);
	ch->in_room->track_dir[1] = direction;
    }
    else if (strlen(ch->in_room->track[2]) < 2)
    {
	free_string(ch->in_room->track[2]);
	ch->in_room->track[2]     = str_dup(ch->name);
	ch->in_room->track_dir[2] = direction;
    }
    else if (strlen(ch->in_room->track[3]) < 2)
    {
	free_string(ch->in_room->track[3]);
	ch->in_room->track[3]     = str_dup(ch->name);
	ch->in_room->track_dir[3] = direction;
    }
    else if (strlen(ch->in_room->track[4]) < 2)
    {
	free_string(ch->in_room->track[4]);
	ch->in_room->track[4]     = str_dup(ch->name);
	ch->in_room->track_dir[4] = direction;
    }
    else
    {
	free_string(ch->in_room->track[0]);
	ch->in_room->track[0]     = str_dup(ch->in_room->track[1]);
	ch->in_room->track_dir[0] = ch->in_room->track_dir[1];
	free_string(ch->in_room->track[1]);
	ch->in_room->track[1]     = str_dup(ch->in_room->track[2]);
	ch->in_room->track_dir[1] = ch->in_room->track_dir[2];
	free_string(ch->in_room->track[2]);
	ch->in_room->track[2]     = str_dup(ch->in_room->track[3]);
	ch->in_room->track_dir[2] = ch->in_room->track_dir[3];
	free_string(ch->in_room->track[3]);
	ch->in_room->track[3]     = str_dup(ch->in_room->track[4]);
	ch->in_room->track_dir[3] = ch->in_room->track_dir[4];
	free_string(ch->in_room->track[4]);
	ch->in_room->track[4]     = str_dup(ch->name);
	ch->in_room->track_dir[4] = direction;
    }
    return;
}

bool check_track( CHAR_DATA *ch, long direction )
{
    CHAR_DATA *victim;
    char buf [MAX_INPUT_LENGTH];
    char vict [MAX_INPUT_LENGTH];
    long door;

    strcpy(vict,ch->hunting);
    if (!str_cmp(ch->hunting,vict))
    {
	if ( ( victim = get_char_room( ch, vict ) ) != 0 )
	{
	    act("You have found $N!",ch,0,victim,TO_CHAR);
	    free_string(ch->hunting);
	    ch->hunting = str_dup( "" );
	    return TRUE;
	}
    }
    if (strlen(ch->in_room->track[direction]) < 2) return FALSE;
    if (!str_cmp(ch->in_room->track[direction],ch->name)) return FALSE;
    if (strlen(ch->hunting) > 1 &&
str_cmp(ch->in_room->track[direction],ch->hunting)) return FALSE;
    door = ch->in_room->track_dir[direction];
    sprintf(buf,"You sense the trail of %s leading $T from here.",ch->in_room->track[direction]);
    act( buf, ch, 0, dir_name[door], TO_CHAR );
    return TRUE;
}
*/

void do_track(CHAR_DATA * ch, char *argument)
{
	bool found = FALSE;

	if(!IS_NPC(ch) && number_percent() > ch->pcdata->learned[gsn_track])
	{
		send_to_char("You cannot sense any trails from this room.\n\r", ch);
		return;
	}
	if(check_track(ch, 0))
		found = TRUE;
	if(check_track(ch, 1))
		found = TRUE;
	if(check_track(ch, 2))
		found = TRUE;
	if(check_track(ch, 3))
		found = TRUE;
	if(check_track(ch, 4))
		found = TRUE;
	if(found == FALSE)
	{
		send_to_char("You cannot sense any trails from this room.\n\r", ch);
		return;
	}
	act("$n carefully examines the ground for tracks.", ch, 0, 0, TO_ROOM);
	return;
}

void do_hunt(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);
/*
    send_to_char("This command is temporarily disabled.\n\r",ch);
    return;
*/
	if(arg[0] == '\0')
	{
		if(strlen(ch->hunting) > 1)
		{
			free_string(ch->hunting);
			ch->hunting = str_dup("");
			send_to_char("You stop hunting your prey.\n\r", ch);
		}
		else
			send_to_char("Who do you wish to hunt?\n\r", ch);
		return;
	}
	if(!str_cmp(arg, ch->name))
	{
		send_to_char("How can you hunt yourself?\n\r", ch);
		return;
	}
	ch->hunting = str_dup(arg);
	send_to_char("Ok.\n\r", ch);
	return;
}

void check_hunt(CHAR_DATA * ch)
{
	CHAR_DATA *victim;
	bool found = FALSE;
	long direction = 0;
	ROOM_INDEX_DATA *in_room;

	in_room = ch->in_room;
	if(!IS_NPC(ch) && number_percent() > ch->pcdata->learned[gsn_track])
	{
		send_to_char("You cannot sense any trails from this room.\n\r", ch);
		free_string(ch->hunting);
		ch->hunting = str_dup("");
		return;
	}
	if(check_track(ch, 0))
	{
		found = TRUE;
		direction = ch->in_room->track_dir[0];
	}
	else if(check_track(ch, 1))
	{
		found = TRUE;
		direction = ch->in_room->track_dir[1];
	}
	else if(check_track(ch, 2))
	{
		found = TRUE;
		direction = ch->in_room->track_dir[2];
	}
	else if(check_track(ch, 3))
	{
		found = TRUE;
		direction = ch->in_room->track_dir[3];
	}
	else if(check_track(ch, 4))
	{
		found = TRUE;
		direction = ch->in_room->track_dir[4];
	}
	else if((victim = get_char_room(ch, ch->hunting)) == 0)
	{
		send_to_char("You cannot sense any trails from this room.\n\r", ch);
		free_string(ch->hunting);
		ch->hunting = str_dup("");
		return;
	}
	if(strlen(ch->hunting) < 2)
		return;
	if((victim = get_char_room(ch, ch->hunting)) != 0)
		return;
	act("$n carefully examines the ground for tracks.", ch, 0, 0, TO_ROOM);
	move_char(ch, direction);
	if(in_room == ch->in_room || victim != 0)
	{
		free_string(ch->hunting);
		ch->hunting = str_dup("");
	}
	return;
}

void add_tracks(CHAR_DATA * ch, long direction)
{
	long loop;

	if(IS_NPC(ch))
		return;
	if(IS_CLASS(ch, CLASS_WEREWOLF) && ch->pcdata->powers[WPOWER_BOAR] > 0)
		return;
	if(IS_CLASS(ch, CLASS_NINJA) && ch->pcdata->powers[NPOWER_SORA] >= 4)
		return;
	if(IS_ITEMAFF(ch, ITEMA_STALKER))
		return;
	for(loop = 0; loop <= 4; loop++)
	{
		if(ch->in_room->track[loop] != 0 && !str_cmp(ch->in_room->track[loop], ch->name))
		{
			free_string(ch->in_room->track[loop]);
			ch->in_room->track[loop] = str_dup("");
		}
	}
	if(strlen(ch->in_room->track[0]) < 2)
	{
		free_string(ch->in_room->track[0]);
		ch->in_room->track[0] = str_dup(ch->name);
		ch->in_room->track_dir[0] = direction;
	}
	else if(strlen(ch->in_room->track[1]) < 2)
	{
		free_string(ch->in_room->track[1]);
		ch->in_room->track[1] = str_dup(ch->name);
		ch->in_room->track_dir[1] = direction;
	}
	else if(strlen(ch->in_room->track[2]) < 2)
	{
		free_string(ch->in_room->track[2]);
		ch->in_room->track[2] = str_dup(ch->name);
		ch->in_room->track_dir[2] = direction;
	}
	else if(strlen(ch->in_room->track[3]) < 2)
	{
		free_string(ch->in_room->track[3]);
		ch->in_room->track[3] = str_dup(ch->name);
		ch->in_room->track_dir[3] = direction;
	}
	else if(strlen(ch->in_room->track[4]) < 2)
	{
		free_string(ch->in_room->track[4]);
		ch->in_room->track[4] = str_dup(ch->name);
		ch->in_room->track_dir[4] = direction;
	}
	else
	{
		free_string(ch->in_room->track[0]);
		ch->in_room->track[0] = str_dup(ch->in_room->track[1]);
		ch->in_room->track_dir[0] = ch->in_room->track_dir[1];
		free_string(ch->in_room->track[1]);
		ch->in_room->track[1] = str_dup(ch->in_room->track[2]);
		ch->in_room->track_dir[1] = ch->in_room->track_dir[2];
		free_string(ch->in_room->track[2]);
		ch->in_room->track[2] = str_dup(ch->in_room->track[3]);
		ch->in_room->track_dir[2] = ch->in_room->track_dir[3];
		free_string(ch->in_room->track[3]);
		ch->in_room->track[3] = str_dup(ch->in_room->track[4]);
		ch->in_room->track_dir[3] = ch->in_room->track_dir[4];
		free_string(ch->in_room->track[4]);
		ch->in_room->track[4] = str_dup(ch->name);
		ch->in_room->track_dir[4] = direction;
	}
	return;
}

bool check_track(CHAR_DATA * ch, long direction)
{
	CHAR_DATA *victim;
	char buf[MAX_INPUT_LENGTH];
	char vict[MAX_INPUT_LENGTH];
	long door;

	strcpy(vict, ch->hunting);
	if(!str_cmp(ch->hunting, vict))
	{
		if((victim = get_char_room(ch, vict)) != 0)
		{
			act("You have found $N!", ch, 0, victim, TO_CHAR);
			free_string(ch->hunting);
			ch->hunting = str_dup("");
			return TRUE;
		}
	}
	if(strlen(ch->in_room->track[direction]) < 2)
		return FALSE;
	if(!str_cmp(ch->in_room->track[direction], ch->name))
		return FALSE;
	if(strlen(ch->hunting) > 1 && str_cmp(ch->in_room->track[direction], ch->hunting))
		return FALSE;
	door = ch->in_room->track_dir[direction];
	sprintf(buf, "You sense the trail of %s leading $T from here.", ch->in_room->track[direction]);
	act(buf, ch, 0, dir_name[door], TO_CHAR);
	return TRUE;
}


void do_roll(CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DEMON) && !IS_SET(ch->special, SPC_CHAMPION))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(!IS_DEMPOWER(ch, DEM_MOVE))
	{
		send_to_char("You haven't been granted the gift of movement.\n\r", ch);
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

	if(obj->in_room == 0)
	{
		send_to_char("You are unable to move.\n\r", ch);
		return;
	}

	if(!str_cmp(arg, "n") || !str_cmp(arg, "north"))
		do_north(ch, "");
	else if(!str_cmp(arg, "s") || !str_cmp(arg, "south"))
		do_south(ch, "");
	else if(!str_cmp(arg, "e") || !str_cmp(arg, "east"))
		do_east(ch, "");
	else if(!str_cmp(arg, "w") || !str_cmp(arg, "west"))
		do_west(ch, "");
	else if(!str_cmp(arg, "u") || !str_cmp(arg, "up"))
		do_up(ch, "");
	else if(!str_cmp(arg, "d") || !str_cmp(arg, "down"))
		do_down(ch, "");
	else
	{
		send_to_char("Do you wish to roll north, south, east, west, up or down?\n\r", ch);
		return;
	}
	obj_from_room(obj);
	obj_to_room(obj, ch->in_room);
	return;
}

void do_leap(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	OBJ_DATA *container;
	OBJ_DATA *obj;
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DEMON) && !IS_SET(ch->special, SPC_CHAMPION))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(!IS_DEMPOWER(ch, DEM_LEAP))
	{
		send_to_char("You haven't been granted the gift of leaping.\n\r", ch);
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

	if(obj->in_room != 0 && arg[0] == '\0')
	{
		send_to_char("Where do you want to leap?\n\r", ch);
		return;
	}

	if(obj->in_room != 0)
	{
		if((victim = get_char_room(ch, arg)) != 0)
		{
			act("$p leaps into your hands.", victim, obj, 0, TO_CHAR);
			act("$p leaps into $n's hands.", victim, obj, 0, TO_ROOM);
			obj_from_room(obj);
			obj_to_char(obj, victim);
			return;
		}
		else if((container = get_obj_room(ch, arg)) != 0)
		{
			if(container->item_type != ITEM_CONTAINER &&
			   container->item_type != ITEM_CORPSE_NPC && container->item_type != ITEM_CORPSE_PC)
			{
				send_to_char("You cannot leap into that sort of object.\n\r", ch);
				return;
			}
			act("$p leap into $P.", ch, obj, container, TO_CHAR);
			act("$p leaps into $P.", ch, obj, container, TO_ROOM);
			obj_from_room(obj);
			obj_to_obj(obj, container);
			return;
		}
		else
			send_to_char("Nothing here by that name.\n\r", ch);
		return;
	}
	if(obj->carried_by != 0)
	{
		act("$p leaps from your hands.", obj->carried_by, obj, 0, TO_CHAR);
		act("$p leaps from $n's hands.", obj->carried_by, obj, 0, TO_ROOM);
		obj_from_char(obj);
		obj_to_room(obj, ch->in_room);
		return;
	}
	else if((container = obj->in_obj) != 0 && container->in_room != 0)
	{
		obj_from_obj(obj);
		obj_to_room(obj, container->in_room);
		char_from_room(ch);
		char_to_room(ch, container->in_room);
		act("$p leap from $P.", ch, obj, container, TO_CHAR);
		act("$p leaps from $P.", ch, obj, container, TO_ROOM);
		return;
	}
	if(obj->in_room != 0)
		send_to_char("You seem unable to leap anywhere.\n\r", ch);
	else
		send_to_char("You seem to be stuck!\n\r", ch);
	return;
}


void drow_hate(CHAR_DATA * ch)
{
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_DROW) || !IS_SET(ch->newbits, NEW_DROWHATE))
		return;

	if(number_percent() < 25)
	{

		for(vch = char_list; vch != 0; vch = vch_next)
		{
			vch_next = vch->next;
			if(ch == vch)
				continue;
			if(vch->in_room == 0)
				continue;
			if(vch->in_room == ch->in_room)
			{
				send_to_char("You scream out in hatred and attack!\n\r", ch);
				act("$n screams out in hatred and attacks!", ch, 0, vch, TO_ROOM);
				do_kill(ch, vch->name);
				return;
			}
		}
	}

	return;
}

// THIS IS THE END OF THE FILE THANKS
