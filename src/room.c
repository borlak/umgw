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

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

/* //common room names//
*road path way street
patch
home room shed shop hut store kitchen bedroom barracks inn
tower cage
plains foothills field(s) mountain valley moutainside hill(s)
lair
citadel temple
stairwell stairs
courtyard
bridge
hall tunnel(s) trail corridor passage hallway alley
hurricane cloud air
EMPTY GRASSY BARN WILDERNESS SMALL SECRET BEND FOREST TURN SWAMP SWAMPY ROCKY INTERSECTION LARGE HUGE
catacombs cave dungeon
water sea

*/

struct room_type
{
	char	*keywords;
	long	minx;
	long	miny;
	long	maxx;
	long	maxy;
};


const struct room_type room_table[] =
{
	{"road path way street",
	3, 3, 4, 9},

	{"patch",
	3, 3, 3, 3},

	{"home room shed shop hut store kitchen bedroom barracks inn",
	5, 5, 9, 9},

	{"tower cage",
	5, 5, 5, 5},

	{"plains foothills field fields mountain valley moutainside hill hills",
	7, 7,13,13},

	{"lair",
	9, 9,15,15},

	{"citadel temple",
	6, 6,10,10},

	{"stairwell stairs",
	2, 2, 3, 3},

	{"courtyard",
	9, 9,12,12},

	{"bridge",
	3, 6, 5,15},

	{"hall tunnel tunnels trail corridor passage hallway alley",
	2, 5, 4,10},

	{"hurricane cloud air",
	2, 2,12,12},

	{"catacombs cave dungeon",
	5, 5, 7, 7},

	{"water sea",
	9, 9,17,17},

	{0,5,5,5,5}
};


void check_room_flags(ROOM_INDEX_DATA *room, char *word)
{
//EMPTY GRASSY BARN WILDERNESS SMALL SECRET BEND FOREST TURN SWAMP SWAMPY ROCKY INTERSECTION LARGE HUGE
	if( !str_cmp(word,	"small") ) { SET_BIT(room->room_flags, ROOM_SMALL);	return; }
	if( !str_cmp(word,     "secret") ) { SET_BIT(room->room_flags, ROOM_SMALL);	return; }
	if( !str_cmp(word,	"empty") ) { SET_BIT(room->room_flags, ROOM_EMPTY);	return; }
	if( !str_cmp(word,	 "bend") ) { SET_BIT(room->room_flags, ROOM_BEND);	return; }
	if( !str_cmp(word,	 "turn") ) { SET_BIT(room->room_flags, ROOM_BEND);	return; }
	if( !str_cmp(word,"intersection")) { SET_BIT(room->room_flags, ROOM_INTERSECTION); return; }
	if( !str_cmp(word,	"swamp") ) { SET_BIT(room->room_flags, ROOM_SWAMP);	return; }
	if( !str_cmp(word,	"rocky") ) { SET_BIT(room->room_flags, ROOM_ROCKY);	return; }
	if( !str_cmp(word,	"large") ) { SET_BIT(room->room_flags, ROOM_LARGE);	return; }
	if( !str_cmp(word,	 "huge") ) { SET_BIT(room->room_flags, ROOM_HUGE);	return; }
	if( !str_cmp(word,	 "dark") ) { SET_BIT(room->room_flags, ROOM_DARK);	return; }
	if( !str_cmp(word,     "inside") ) { SET_BIT(room->room_flags, ROOM_INDOORS);	return; }
}


// set up the rooms dimensions and objects
void setup_room(ROOM_INDEX_DATA *room)
{
	OBJ_DATA *obj;
	char realname[MAX_STRING_LENGTH];
	char *name;
	char argument[MAX_STRING_LENGTH];
	long i,x;
	long boxes;
	long numobjs;
	bool found = FALSE;

	for( i = 0; room_table[i].keywords != 0 && !found; i++ )
	{
		realname[0] = '\0';
		name = realname;
		strcpy(name,room->name);
		do
		{
			name = one_argument(name,argument);
			check_room_flags(room, argument);
			if( strstr(argument, room_table[i].keywords) != 0 )
				found = TRUE;
		} while(name[0] != '\0' && !found);
	}
	// we got a room, or they are using the last (default) in the room table
	room->coordx	= number_range(room_table[i].minx,room_table[i].maxx);
	room->coordy	= number_range(room_table[i].miny,room_table[i].maxy);

	if( IS_SET(room->room_flags, ROOM_SMALL) )
	{
		room->coordx /= 2;
		room->coordy /= 2;
	}
	if( IS_SET(room->room_flags, ROOM_LARGE) )
	{
		room->coordx *= 2;
		room->coordy *= 2;
	}
	if( IS_SET(room->room_flags, ROOM_HUGE) )
	{
		room->coordx *= 3;
		room->coordy *= 3;
	}

	boxes		= room->coordx*room->coordy;
	numobjs		= number_range(0, boxes/3);

	if( IS_SET(room->room_flags, ROOM_EMPTY) )
		numobjs = 0;
	if( IS_SET(room->room_flags, ROOM_BEND)
	||  IS_SET(room->room_flags, ROOM_INTERSECTION) )
		numobjs = numobjs ? 0 : numobjs/3;

	for( x = 0; x < numobjs; x++ )
	{
		obj = create_object(get_obj_index(30037),1);
		obj->extra_flags	= 0;
		obj->extra_flags2	= 0;
		obj->item_type		= 0;
		obj->weight		= 999999;
		obj->coordy		= number_range(0,room->coordy-1);
		obj->coordx		= number_range(0,room->coordx-1);
	}

}
