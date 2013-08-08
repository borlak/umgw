
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

/* Fille added by Borlak.
 * In this file, unlike the other special functions, the boolean DOES
 * matter, if someone is moving and it returns FALSE, it will not let
 * them move.  you have to return TRUE to let players move in rooms
 * these functinos are set.
 * Another addition. In the spec update in update.c if it returns false
 * on its final call (ROOM_END) then it will keep going for another tick.
 * so return TRUE to kill the function.
 *
 * these functions should never be called with a null to_room or from_room
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

/* Rooms */

/* Objects */

/* Mobs */
SPEC_FUN *spec_lookup(const char *name)
{

	/* Rooms */

	/* Objects */

	/* Mobiles */
	return 0;
}

char *spec_name_lookup(SPEC_FUN * spec)
{

	/* Rooms */

	/* Objects */

	/* Mobiles */
	return "none";
}

bool has_spec(const char *str, SPEC_OBJ_DATA * list)
{
	SPEC_OBJ_DATA *spec;

	for(spec = list; spec; spec = spec->next)

	{
		if(!str_cmp(spec_name_lookup(spec->spec_fun), str))
			return TRUE;
	}
	return FALSE;
}

SPEC_OBJ_DATA *new_spec_obj(void)
{
	SPEC_OBJ_DATA *spec_obj;

	if(spec_obj_free == 0)

	{
		spec_obj = alloc_perm(sizeof(*spec_obj));
	}

	else

	{
		spec_obj = spec_obj_free;
		spec_obj_free = spec_obj->next;
	}
	spec_obj->room = 0;
	spec_obj->ch = 0;
	spec_obj->vch = 0;
	spec_obj->obj = 0;
	spec_obj->next = 0;
	spec_obj->next_world = 0;
	spec_obj->timer = -1;
	spec_obj->value[0] = 0;
	spec_obj->value[1] = 0;
	spec_obj->value[2] = 0;
	spec_obj->value[3] = 0;
	return spec_obj;
}

SPEC_OBJ_DATA *add_spec(const char *spec_name, SPEC_OBJ_DATA * list)
{
	SPEC_OBJ_DATA *spec_obj;
	SPEC_FUN *spec_fun;

	if((spec_fun = spec_lookup(spec_name)) == 0)
		return 0;
	spec_obj = new_spec_obj();
	spec_obj->spec_fun = spec_fun;
	spec_obj->next_world = spec_obj_list;
	spec_obj_list = spec_obj;
	spec_obj->next = list;
	list = spec_obj;
	return spec_obj;
}


/* ok this is kind of tricky.  some updates will
 * return soon as they are FALSE, but keep going
 * on if they are true.. and some will return
 * right off as soon as they are activated, etc.
 */
/* what = what type of structure is calling this function, UPDATE_ROOM, etc.
 * type = the update.. UPDATE_ENTERED, etc.
 * order is: room updated first, then objects on the mob, then the mob(s), then objects in room
 */
/* definitions:
 * ROOMS: will return if exited/entered and FALSE from spec fun
 *
 */
long update_specs(ROOM_INDEX_DATA * room, long what, long type)
{
	SPEC_OBJ_DATA *spec;
	CHAR_DATA *ch = 0;
	CHAR_DATA *ch_next;
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	long flag = FALSE;

	if(!room)

	{
		bug("Update_specs: Called with null room!  Type = %li", type);
		return TRUE;
	}
	if(what == UPDATE_ALL || what == UPDATE_ROOM)

	{
		for(spec = room->spec_obj; spec; spec = spec->next)

		{
			flag = (*spec->spec_fun) (spec, type);
			if(what == UPDATE_ROOM || what == UPDATE_ALL)

			{
				switch (type)

				{
				case UPDATE_EXITED:	/* move_char calls these, if FALSE, dont move char */
				case UPDATE_ENTERED:
					if(flag == FALSE)
						return FALSE;
					break;
				default:
					break;
				}
			}
		}
	}
	if(what != UPDATE_ROOM)

	{
		for(ch = room->people; ch; ch = ch_next)

		{
			ch_next = ch->next_in_room;
			if(what == UPDATE_ALL || what == UPDATE_OBJ)

			{
				for(obj = ch->carrying; obj; obj = obj_next)

				{
					obj_next = obj->next_content;
					for(spec = obj->spec_obj; spec; spec = spec->next)
						(*spec->spec_fun) (spec, type);
				}
			}
			if(what == UPDATE_ALL || what == UPDATE_MOB)

			{
				for(spec = ch->spec_obj; spec; spec = spec->next)
					(*spec->spec_fun) (spec, type);
			}
		}
	}
	if(what == UPDATE_ALL || what == UPDATE_OBJ)

	{
		for(obj = room->contents; obj; obj = obj_next)

		{
			obj_next = obj->next_content;
			for(spec = obj->spec_obj; spec; spec = spec->next)
				(*spec->spec_fun) (spec, type);
		}
	}
	return TRUE;
}
extern const char *dir_name[];
extern const long rev_dir[];


/*************************************************************************/
/*                                ROOMS                                  */
/*************************************************************************/

/*************************************************************************/
/*                              OBJECTS                                  */
/*************************************************************************/

/*************************************************************************/
/*                              MOBILES                                  */
/*************************************************************************/

// THIS IS THE END OF THE FILE THANKS
