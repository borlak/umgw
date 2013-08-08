// Draw a map for a linked-list type room system.
// Created by borlak

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

//#define MAX_VISION    13 // in merc.h
#define MAX_SEARCH(ch)	(((ch)->pcdata->vision-1)/2)
#define MIN_SEARCH(ch)	((((ch)->pcdata->vision-1)/2)*-1)
#define PLR_VISION(ch)  ((ch)->pcdata->vision)
#define DOOR_NORTH	(SECT_MAX+1)
#define DOOR_SOUTH	(SECT_MAX+2)
#define DOOR_EAST	(SECT_MAX+3)
#define DOOR_WEST	(SECT_MAX+4)

struct map
{
	char letter;
	long sector_type;
};

struct person
{
	CHAR_DATA *vch;
	char letter;
};

static struct map rooms[MAX_VISION][MAX_VISION];
static struct person pcs[MAX_VISION * 2];	// show two people per line
static struct person npcs[MAX_VISION * 2];

/*
make_map() is a recursive function that basically starts off by sending
off leads that branch off into sub-leads that search for links in every
direction.  I'll draw an example below:
o = point of origin
- = main lead
| = main lead
+ = sub-lead (branches off main lead)

++ ++
++ ++
--o--
++ ++
++ ++

++|++
++|++
  o
++|++
++|++

It attemps to fill up the array "rooms" in this manner.  This assumes
your linked list areas dont have conflicts, that is, rooms that overlap.
This wont work in mazes that use randomized exits obviously, but it may
help in causing the confusion mazes are supposed to have! :)

Although it may appear laggy from all the checks it does, you must realize
that most rooms dont have more than 2 exits, and not only that, this
isn't doing a "spider search" where it checks every possible direction
each room, it only does a lead, sub-lead search, which in effect also
gives a more realistic picture (doesn't let you see too much or around
branching corners.

This is my first attempt at a recursive function so there is probably
a better way to do this.
*/

void modify_rooms args((CHAR_DATA * ch, ROOM_INDEX_DATA * room, long x, long y, long which));
char *sector_grafx_single args((long x, long color));
char *sector_grafx_double args((long x, long color));
char *health_grafx args((CHAR_DATA * ch, long color));
char letter_grafx args((long x));

void make_map(CHAR_DATA * ch, long x, long y, bool leader, long which, ROOM_INDEX_DATA * toroom)
{
	ROOM_INDEX_DATA *room = 0;
	EXIT_DATA *exit;

	if(!ch || !ch->in_room)
		return;

	if(x == 0 && y == 0)
	{
		long i, j;

		room = ch->in_room;

		for(i = 0; i < MAX_VISION * 2; i++)
		{
			pcs[i].vch = 0;
			npcs[i].vch = 0;
		}

		for(i = 0; i < MAX_VISION; i++)
		{
			for(j = 0; j < MAX_VISION; j++)
			{
				rooms[i][j].sector_type = -1;
				rooms[i][j].letter = ' ';
			}
		}

		modify_rooms(ch, ch->in_room, 0, 0, -1);

		for(i = 0; i < 4; i++)
		{
			if((exit = room->exit[i]) != 0 && exit->to_room != 0)
			{
				if(IS_SET(exit->exit_info, EX_CLOSED))
				{
					switch (i)
					{
					case DIR_NORTH:
						modify_rooms(ch, exit->to_room, x, y + 1, i);
						break;
					case DIR_SOUTH:
						modify_rooms(ch, exit->to_room, x, y - 1, i);
						break;
					case DIR_WEST:
						modify_rooms(ch, exit->to_room, x - 1, y, i);
						break;
					case DIR_EAST:
						modify_rooms(ch, exit->to_room, x + 1, y, i);
						break;
					default:
						break;
					}
					continue;
				}

				switch (i)
				{
				case DIR_NORTH:
					make_map(ch, x, y + 1, TRUE, DIR_NORTH, exit->to_room);
					break;
				case DIR_SOUTH:
					make_map(ch, x, y - 1, TRUE, DIR_SOUTH, exit->to_room);
					break;
				case DIR_WEST:
					make_map(ch, x - 1, y, TRUE, DIR_WEST, exit->to_room);
					break;
				case DIR_EAST:
					make_map(ch, x + 1, y, TRUE, DIR_EAST, exit->to_room);
					break;
				default:
					break;
				}
			}
		}
		return;
	}

	if(leader == TRUE)
	{
		if((room = toroom) == 0)
			return;

		if(x > MAX_SEARCH(ch) || x < MIN_SEARCH(ch) || y > MAX_SEARCH(ch) || y < MIN_SEARCH(ch))
			return;

		modify_rooms(ch, room, x, y, -1);

		switch (which)
		{
		case DIR_NORTH:
			make_map(ch, x - 1, y, FALSE, DIR_WEST, room);
			make_map(ch, x + 1, y, FALSE, DIR_EAST, room);
			break;
		case DIR_SOUTH:
			make_map(ch, x - 1, y, FALSE, DIR_WEST, room);
			make_map(ch, x + 1, y, FALSE, DIR_EAST, room);
			break;
		case DIR_EAST:
			make_map(ch, x, y - 1, FALSE, DIR_SOUTH, room);
			make_map(ch, x, y + 1, FALSE, DIR_NORTH, room);
			break;
		case DIR_WEST:
			make_map(ch, x, y - 1, FALSE, DIR_SOUTH, room);
			make_map(ch, x, y + 1, FALSE, DIR_NORTH, room);
			break;
		default:
			return;
		}

		if((exit = room->exit[which]) != 0 && exit->to_room != 0)
		{
			room = exit->to_room;

			if(IS_SET(exit->exit_info, EX_CLOSED))
			{
				switch (which)
				{
				case DIR_NORTH:
					modify_rooms(ch, exit->to_room, x, y + 1, which);
					break;
				case DIR_SOUTH:
					modify_rooms(ch, exit->to_room, x, y - 1, which);
					break;
				case DIR_WEST:
					modify_rooms(ch, exit->to_room, x - 1, y, which);
					break;
				case DIR_EAST:
					modify_rooms(ch, exit->to_room, x + 1, y, which);
					break;
				default:
					break;
				}
				return;
			}

			switch (which)
			{
			case DIR_EAST:
				make_map(ch, x + 1, y, TRUE, which, room);
				break;
			case DIR_WEST:
				make_map(ch, x - 1, y, TRUE, which, room);
				break;
			case DIR_NORTH:
				make_map(ch, x, y + 1, TRUE, which, room);
				break;
			case DIR_SOUTH:
				make_map(ch, x, y - 1, TRUE, which, room);
				break;
			default:
				return;
			}
		}
		return;
	}

	if(x > MAX_SEARCH(ch) || x < MIN_SEARCH(ch) || y > MAX_SEARCH(ch) || y < MIN_SEARCH(ch))
		return;

	if((exit = toroom->exit[which]) == 0 || exit->to_room == 0)
		return;

	room = exit->to_room;

	if(IS_SET(exit->exit_info, EX_CLOSED))
	{
		modify_rooms(ch, room, x, y, which);
		return;
	}

	modify_rooms(ch, room, x, y, -1);

	switch (which)
	{
	case DIR_NORTH:
		make_map(ch, x, y + 1, FALSE, DIR_NORTH, room);
		break;
	case DIR_SOUTH:
		make_map(ch, x, y - 1, FALSE, DIR_SOUTH, room);
		break;
	case DIR_EAST:
		make_map(ch, x + 1, y, FALSE, DIR_EAST, room);
		break;
	case DIR_WEST:
		make_map(ch, x - 1, y, FALSE, DIR_WEST, room);
		break;
	default:
		return;
	}
	return;
}

void delete_nothings(long vision)
{
	long x, y;
	long ok;

	for(y = vision - 1; y >= 0; y--)
	{
		for(x = 0; x <= vision - 1; x++)
		{
			if(rooms[x][y].sector_type != -1)
				continue;

			ok = 1;

			if((x == 0 || y == 0 || rooms[x - 1][y - 1].sector_type < 0)
			   && (x == 0 || rooms[x - 1][y].sector_type < 0)
			   && (x == 0 || y >= vision - 1 || rooms[x - 1][y + 1].sector_type < 0)
			   && (y == 0 || rooms[x][y - 1].sector_type < 0)
			   && (y >= vision - 1 || rooms[x][y + 1].sector_type < 0)
			   && (x >= vision - 1 || y >= vision - 1 || rooms[x + 1][y + 1].sector_type < 0)
			   && (x >= vision - 1 || y == 0 || rooms[x + 1][y - 1].sector_type < 0)
			   && (x >= vision - 1 || rooms[x + 1][y].sector_type < 0))
				rooms[x][y].sector_type = -2;
		}
	}
}

void draw_map(CHAR_DATA * ch)
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char name[256];
	long y, x, i = 0, pc = 0, npc = 0;
	bool color = FALSE;

	if(!ch || IS_NPC(ch))
		return;

	if(!IS_SET(ch->mapbits, MAP_ON))
		return;

	modify_rooms(0, 0, 0, 0, -1);
	make_map(ch, 0, 0, 0, 0, 0);
	delete_nothings(ch->pcdata->vision);

	buf[0] = '\0';
	color = IS_SET(ch->mapbits, MAP_COLOR) ? TRUE : FALSE;

	for(i = 0; i < MAX_VISION * 2; i++)
	{
		if(!IS_SET(ch->mapbits,MAP_PLAYERS))
			pcs[i].vch = 0;
		else if(!IS_SET(ch->mapbits,MAP_NPCS))
			npcs[i].vch = 0;
	}

	for(y = MAX_SEARCH(ch) * 2; y >= 0; y--)
	{
		buf2[0] = '\0';

		for(x = 0; x <= MAX_SEARCH(ch) * 2; x++)
		{
			if(y == MAX_SEARCH(ch) && x == MAX_SEARCH(ch))
			{
				if(rooms[x][y].letter == ' ')
				{
					sprintf(buf + strlen(buf), "%s<>%s", color ? "{D" : "", color ? "{x" : "");
				}
				else
				{
					sprintf(buf + strlen(buf), "%s*%s%c%s",
						color ? "{D" : "",
						color ? "{R" : "", rooms[x][y].letter, color ? "{x" : "");
				}
			}
			else
			{
				if(rooms[x][y].letter == ' ')
				{
					sprintf(buf + strlen(buf), "%s",
						sector_grafx_double(rooms[x][y].sector_type, color));
				}
				else
				{
					sprintf(buf + strlen(buf), "%s%s%c",
						sector_grafx_single(rooms[x][y].sector_type, color),
						color ? "{R" : "", rooms[x][y].letter);
				}
			}
		}

		for(i = 0; i < 2; i++)
		{
			if(pcs[pc].vch != 0 || npcs[npc].vch != 0)
			{
				if(pcs[pc].vch == 0)
					strcpy(name, npcs[npc].vch->short_descr);
				else
					strcpy(name, pcs[pc].vch->name);

				name[20] = '\0';

				sprintf(buf2 + strlen(buf2), " %c %s %-20s",
					pcs[pc].vch != 0 ? pcs[pc].letter : npcs[npc].letter,
					pcs[pc].vch != 0 ? health_grafx(pcs[pc].vch, color) :
					health_grafx(npcs[npc].vch, color), name);
//                                      pcs[pc].vch != 0 ? pcs[pc].vch->name : npcs[npc].vch->short_descr);


				if(pcs[pc].vch != 0)
					pc++;
				else
					npc++;
			}
		}

		sprintf(buf + strlen(buf), "%s%s\n\r", color ? "{x" : "", buf2);
	}

	sprintf(buf + strlen(buf), "%s\n\r", color ? "{x" : "");
	send_to_char(buf, ch);
}

void modify_rooms(CHAR_DATA * ch, ROOM_INDEX_DATA * room, long x, long y, long which)
{
	CHAR_DATA *vch;
	bool found = FALSE;
	static long letter, pc, npc;

	if(!room)
	{
		letter = 0;
		pc = 0;
		npc = 0;
		return;
	}

	if(x < MIN_SEARCH(ch) || x > MAX_SEARCH(ch) || y < MIN_SEARCH(ch) || y > MAX_SEARCH(ch))
		return;


	if(which > -1)
	{
		switch (which)
		{
		case DIR_NORTH:
			rooms[x + MAX_SEARCH(ch)][y + MAX_SEARCH(ch)].sector_type = DOOR_NORTH;
			break;
		case DIR_SOUTH:
			rooms[x + MAX_SEARCH(ch)][y + MAX_SEARCH(ch)].sector_type = DOOR_SOUTH;
			break;
		case DIR_EAST:
			rooms[x + MAX_SEARCH(ch)][y + MAX_SEARCH(ch)].sector_type = DOOR_EAST;
			break;
		case DIR_WEST:
			rooms[x + MAX_SEARCH(ch)][y + MAX_SEARCH(ch)].sector_type = DOOR_WEST;
			break;
		default:
			return;
		}
		return;
	}

	rooms[x + MAX_SEARCH(ch)][y + MAX_SEARCH(ch)].sector_type = room->sector_type;

	if(letter <= ch->pcdata->vision * 2)
	{
		for(vch = room->people; vch; vch = vch->next_in_room)
		{
			if(vch == ch || !can_see(ch, vch) || pc > PLR_VISION(ch) * 2)
				continue;

			if(!IS_NPC(vch))
			{
				if(!IS_SET(ch->mapbits,MAP_PLAYERS) && !IS_SET(ch->mapbits,MAP_LETTERS))
					found = FALSE;
				else
					found = TRUE;
				pcs[pc].vch = vch;
				pcs[pc++].letter = letter_grafx(letter);
				continue;
			}

			if(npc < PLR_VISION(ch) * 2)
			{
				if(!IS_SET(ch->mapbits,MAP_NPCS) && !IS_SET(ch->mapbits,MAP_LETTERS))
					found = FALSE;
				else
					found = TRUE;
				npcs[npc].vch = vch;
				npcs[npc++].letter = letter_grafx(letter);
			}
		}
	}

	if(found)
	{
		rooms[x + MAX_SEARCH(ch)][y + MAX_SEARCH(ch)].letter = letter_grafx(letter);
		letter++;
	}
	else
	{
		rooms[x + MAX_SEARCH(ch)][y + MAX_SEARCH(ch)].letter = ' ';
	}
}

char *sector_grafx_double(long x, long color)
{
	if(color)
	{
		switch (x)
		{
		case -1:
			return "{x[]";
		case -2:
			return "{x  ";
		case SECT_INSIDE:
			return "{m%%";
		case SECT_CITY:
			return "{M::";
		case SECT_FIELD:
			return "{G..";
		case SECT_FOREST:
			return "{g&&";
		case SECT_HILLS:
			return "{c^^^^";
		case SECT_MOUNTAIN:
			return "{C/\\";
		case SECT_WATER_SWIM:
			return "{B,,";
		case SECT_WATER_NOSWIM:
			return "{b,,";
		case SECT_UNUSED:
			return "{rxx";
		case SECT_AIR:
			return "{W@@";
		case SECT_DESERT:
			return "{y$$";
		case DOOR_NORTH:
			return "{x__";
		case DOOR_SOUTH:
			return "{x--";
		case DOOR_WEST:
			return "{x |";
		case DOOR_EAST:
			return "{x| ";
		default:
			return "{x##";
		}
		return "{x[]";
	}
	else
	{
		switch (x)
		{
		case -1:
			return "[]";
		case -2:
			return "  ";
		case SECT_INSIDE:
			return "%%";
		case SECT_CITY:
			return "::";
		case SECT_FIELD:
			return "..";
		case SECT_FOREST:
			return "&&&&";
		case SECT_HILLS:
			return "^^^^";
		case SECT_MOUNTAIN:
			return "/\\";
		case SECT_WATER_SWIM:
			return ",,";
		case SECT_WATER_NOSWIM:
			return ",,";
		case SECT_UNUSED:
			return "xx";
		case SECT_AIR:
			return "@@";
		case SECT_DESERT:
			return "$$";
		case DOOR_NORTH:
			return "__";
		case DOOR_SOUTH:
			return "--";
		case DOOR_WEST:
			return " |";
		case DOOR_EAST:
			return "| ";
		default:
			return "##";
		}
		return "[]";
	}
	return "[]";
}

char *sector_grafx_single(long x, long color)
{
	if(color)
	{
		switch (x)
		{
		case -1:
			return "{xX";
		case -2:
			return "{x ";
		case SECT_INSIDE:
			return "{m%";
		case SECT_CITY:
			return "{M:";
		case SECT_FIELD:
			return "{G.";
		case SECT_FOREST:
			return "{g&";
		case SECT_HILLS:
			return "{c^";
		case SECT_MOUNTAIN:
			return "{C^";
		case SECT_WATER_SWIM:
			return "{B,";
		case SECT_WATER_NOSWIM:
			return "{b,";
		case SECT_UNUSED:
			return "{rx";
		case SECT_AIR:
			return "{W@";
		case SECT_DESERT:
			return "{y$";
		default:
			return "{x#";
		}
		return "{xX";
	}
	else
	{
		switch (x)
		{
		case -1:
			return "X";
		case -2:
			return " ";
		case SECT_INSIDE:
			return "%";
		case SECT_CITY:
			return ":";
		case SECT_FIELD:
			return ".";
		case SECT_FOREST:
			return "&&";
		case SECT_HILLS:
			return "^";
		case SECT_MOUNTAIN:
			return "^";
		case SECT_WATER_SWIM:
			return ",";
		case SECT_WATER_NOSWIM:
			return ",";
		case SECT_UNUSED:
			return "x";
		case SECT_AIR:
			return "@";
		case SECT_DESERT:
			return "$";
		default:
			return "#";
		}
		return "X";
	}
	return "X";
}

char *health_grafx(CHAR_DATA * ch, long color)
{
	static char buf[256];
	long percent = 0;

	if(!ch)
		return "!!!???";

	if(ch->hit == 0)
		percent = 0;
	else
		percent = (long) (((float) ch->hit / (float) ch->max_hit) * 100.0);

	sprintf(buf, "[%s%c%c%c%c%c{x]",
		color ? percent > 90 ? "{W" : percent > 60 ? "{C" : percent > 30 ? "{Y" : "{R" : "",
		percent > 15 ? '*' : '-',
		percent > 30 ? '*' : '-', percent > 45 ? '*' : '-', percent > 60 ? '*' : '-', percent > 75 ? '*' : '-');

	return buf;
}

char letter_grafx(long x)
{
	switch (x)
	{
	case 0:
		return 'A';
	case 1:
		return 'B';
	case 2:
		return 'C';
	case 3:
		return 'D';
	case 4:
		return 'E';
	case 5:
		return 'F';
	case 6:
		return 'G';
	case 7:
		return 'H';
	case 8:
		return 'I';
	case 9:
		return 'J';
	case 10:
		return 'K';
	case 11:
		return 'L';
	case 12:
		return 'M';
	case 13:
		return 'N';
	case 14:
		return 'O';
	case 15:
		return 'P';
	case 16:
		return 'Q';
	case 17:
		return 'R';
	case 18:
		return 'S';
	case 19:
		return 'T';
	case 20:
		return 'U';
	case 21:
		return 'W';
	case 22:
		return 'X';
	case 23:
		return 'Z';
	case 24:
		return 'V';
	case 25:
		return 'Y';
	default:
		return '?';
	}
	return '?';
}

void do_map(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	if(!ch || IS_NPC(ch))
		return;

	if( !argument || argument[0] == '\0' )
	{
		send_to_char("Sytax: map <option>\n\r",ch);
		send_to_char("       map pk\n\r",ch);
		send_to_char("       map default\n\r\n\r",ch);

		sprintf(buf,		"[Map On     : %-3s]\n\r",IS_SET(ch->mapbits,MAP_ON)?"Yes":"No");
		sprintf(buf+strlen(buf),"[Map Color  : %-3s]\n\r",IS_SET(ch->mapbits,MAP_COLOR)?"Yes":"No");
		sprintf(buf+strlen(buf),"[Map Players: %-3s]\n\r",IS_SET(ch->mapbits,MAP_PLAYERS)?"Yes":"No");
		sprintf(buf+strlen(buf),"[Map Mobs   : %-3s]\n\r",IS_SET(ch->mapbits,MAP_NPCS)?"Yes":"No");
		sprintf(buf+strlen(buf),"[Map Letters: %-3s]\n\r",IS_SET(ch->mapbits,MAP_LETTERS)?"Yes":"No");
		send_to_char(buf,ch);
		return;
	}

	if(!str_cmp(argument,"default"))
	{
		SET_BIT(ch->mapbits,MAP_ON);
		SET_BIT(ch->mapbits,MAP_COLOR);
		SET_BIT(ch->mapbits,MAP_PLAYERS);
		SET_BIT(ch->mapbits,MAP_NPCS);
		REMOVE_BIT(ch->mapbits,MAP_LETTERS);
	}
	else if(!str_cmp(argument,"pk"))
	{
		SET_BIT(ch->mapbits,MAP_ON);
		SET_BIT(ch->mapbits,MAP_COLOR);
		SET_BIT(ch->mapbits,MAP_PLAYERS);
		REMOVE_BIT(ch->mapbits,MAP_NPCS);
		REMOVE_BIT(ch->mapbits,MAP_LETTERS);
	}
	else if(!str_cmp(argument,"on"))
	{
		SWITCH_BIT(ch->mapbits,MAP_ON);
	}
	else if(!str_cmp(argument,"color"))
	{
		SWITCH_BIT(ch->mapbits,MAP_COLOR);
	}
	else if(!str_cmp(argument,"players"))
	{
		SWITCH_BIT(ch->mapbits,MAP_PLAYERS);
	}
	else if(!str_cmp(argument,"mobs"))
	{
		SWITCH_BIT(ch->mapbits,MAP_NPCS);
	}
	else if(!str_cmp(argument,"letters"))
	{
		SWITCH_BIT(ch->mapbits,MAP_LETTERS);
	}
	else
	{
		do_map(ch,"");
		return;
	}

	send_to_char("Done.\n\r", ch);
	return;
}

void do_vision(CHAR_DATA * ch, char *argument)
{
	long i;

	if(!ch || IS_NPC(ch))
		return;

	if(!is_number(argument))
	{
		send_to_char("This command requirse a number.\n\r", ch);
		return;
	}

	i = atoi(argument);

	if(i < 3 || i > 7)
	{
		send_to_char("Mortal's vision range is only settable from 3 to 7.\n\r", ch);
		return;
	}

	if(i % 2 == 0)
	{
		send_to_char("You must set vision in odd increments.\n\r", ch);
		return;
	}

	ch->pcdata->vision = i;
	do_look(ch, "auto");
}

// THIS IS THE END OF THE FILE THANKS
