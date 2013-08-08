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
/* Released 9.12.2001 */
/* Notes:  You will need to call update_kingdoms() in some update function, and call write_kingdoms
 * in hotboot/copyover, if you have it.  This saves the kingdoms.  Also I call update_char_kingdom
 * everytime someone logs in.  This goes through the kingdoms and removes any players that might
 * have been outcasted while offline.
 * The do_commands are: klist, kingdom, ktalk, krecall
 * You will probably have to play with this file, one thing that comes to mind is award_points().
 *
 * You gather points from PK'ing, and once you get 1000 points, your kingdom becomes "active".  Once
 * it's active, it starts losing points (5 per hour or something similar).  If it gets to 0 or below,
 * it automatically destroys.  This is currently TURNED OFF.  If you don't have many players, this
 * does not work well, as they have to be on, and pkilling, quite often.
 *
 * There are wars and treaties, and making wars or treaties gives you bonuses/penalties.  For example,
 * if you are at war with X kingdom, and PK someone in that kingdom, you get 1.5* the normal point
 * award.  However, if they PK YOU, you LOSE points.  If you are treatied with a kingdom, you can
 * hear their ktalk (kingdom talk).  There are lots of little features, such as kingdom recall,
 * kingdom scrying, kingdom report, etc. etc.
 *
 * My mud uses { for ansi color codes ({R {r {X, etc), you may have to change this.
 */

/* kingdom.c and kingdom.h -- made by borlak */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#define KEY(literal,field,value) if( !str_cmp(word,literal) ) { field = value; break; }

KINGDOM_DATA *kingdom_list = 0;

KINGDOM_DATA *create_kingdom args((void));
char *str_sep args((char *str, char *sep));
bool is_leader args((CHAR_DATA * ch, long id));
bool is_trustee args((CHAR_DATA * ch, long id));
bool has_treaty args((long id1, long id2));
bool remove_trustee args((long id, char *name));
bool remove_member args((long id, char *name));
void remove_war args((long id1, long id2));
void remove_treaty args((long id1, long id2));
void delete_kingdom args((long id));
float calc_rating args((KINGDOM_DATA * kingdom));

extern AREA_DATA *area_first;

static KINGDOM_DATA kingdom_default = {
	0,
	"Ronin",
	"Ronin",
	"None",
	"None",
	"None",
	0,
	0,
	0, 0, 0, 0, 0, 0, 0, 0
};

#define KINGDOM_FILE "../area/kingdom.txt"


///////// I/O FUNCTIONS //////////////
void read_kingdoms(void)
{
	KINGDOM_DATA *kingdom = 0;
	FILE *fp;
	char buf[MAX_STRING_LENGTH];
	char *word;
	bool end = FALSE;
	long i = 0;

	fclose(fpReserve);

	if((fp = fopen(KINGDOM_FILE, "r")) == 0)
	{
		log_string("Error opening kingdom.txt for reading");
		fpReserve = fopen(NULL_FILE, "r");
		return;
	}

	while(!end)
	{
		word = fread_word(fp);

		switch (UPPER(word[0]))
		{
		case 'A':
			KEY("Assists", kingdom->assists, fread_number(fp));
			break;

		case 'E':
			if(!str_cmp(word, "End"))
			{
				end = TRUE;
				break;
			}
			break;

		case 'K':
			if(!str_cmp(word, "KingdomID"))
			{
				kingdom = create_kingdom();
				kingdom->id = fread_number(fp);
				continue;
			}
			break;

		case 'L':
			KEY("Leader", kingdom->leader, fread_string(fp));
			break;

		case 'M':
			KEY("Members", kingdom->members, fread_string(fp));
			break;

		case 'N':
			KEY("Name", kingdom->name, fread_string(fp));
			KEY("Noansiname", kingdom->noansiname, fread_string(fp));
			break;

		case 'P':
			KEY("Pks", kingdom->pks, fread_number(fp));
			KEY("Pds", kingdom->pds, fread_number(fp));
			KEY("Points", kingdom->points, fread_number(fp));
			break;

		case 'R':
			KEY("Realpoints", kingdom->realpoints, fread_number(fp));
			KEY("Recall", kingdom->recall, fread_number(fp));
			break;

		case 'T':
			KEY("Trustees", kingdom->trustees, fread_string(fp));

			if(!str_cmp(word, "Treaties"))
			{
				i = fread_number(fp);
				if(i > 0)
				{
					kingdom->treaties = malloc(sizeof(long) * (i+1));
					memset(kingdom->treaties,1,sizeof(long) * (i+1));
					kingdom->treaties[i] = 0;
					for(i = 0; kingdom->treaties[i]; i++)
						kingdom->treaties[i] = fread_number(fp);
				}
				else
				{
					kingdom->treaties = malloc(sizeof(long) * 1);
					kingdom->treaties[0] = 0;
				}
				break;
			}
			break;

		case 'U':
			KEY("Upkeep", kingdom->upkeep, fread_number(fp));
			break;

		case 'W':
			if(!str_cmp(word, "Wars"))
			{
				i = fread_number(fp);
				if(i > 0)
				{
					kingdom->wars = malloc(sizeof(long) * (i+1));
					memset(kingdom->wars,1,sizeof(long) * (i+1));
					kingdom->wars[i] = 0;
					for(i = 0; kingdom->wars[i]; i++)
						kingdom->wars[i] = fread_number(fp);
				}
				else
				{
					kingdom->wars = malloc(sizeof(long) * 1);
					kingdom->wars[0] = 0;
				}
				break;
			}
			break;

		default:
			sprintf(buf, "Bad word while reading kingdom.txt -> %s", word);
			bug(buf, 0);
			exit(1);
		}
	}

	fclose(fp);
	fpReserve = fopen(NULL_FILE, "r");
	update_kingdoms();
}

void write_kingdoms(void)
{
	KINGDOM_DATA *kingdom;
	FILE *fp;
	long i;

	fclose(fpReserve);

	if((fp = fopen(KINGDOM_FILE, "w")) == 0)
	{
		log_string("Error opening kingdom.txt for writing");
		fpReserve = fopen(NULL_FILE, "r");
		return;
	}

	for(kingdom = kingdom_list; kingdom; kingdom = kingdom->next)
	{
		fprintf(fp, "KingdomID      %li\n", kingdom->id);
		fprintf(fp, "Name           %s~\n", kingdom->name);
		fprintf(fp, "Noansiname     %s~\n", kingdom->noansiname);
		fprintf(fp, "Leader         %s~\n", kingdom->leader);
		fprintf(fp, "Members        %s~\n", kingdom->members);
		fprintf(fp, "Trustees       %s~\n", kingdom->trustees);
		fprintf(fp, "Points         %li\n", kingdom->points);
		fprintf(fp, "Realpoints     %li\n", kingdom->realpoints);
		fprintf(fp, "Pks            %li\n", kingdom->pks);
		fprintf(fp, "Assists        %li\n", kingdom->assists);
		fprintf(fp, "Pds            %li\n", kingdom->pds);
		fprintf(fp, "Upkeep         %li\n", kingdom->upkeep);
		fprintf(fp, "Recall         %li\n", kingdom->recall);
		for(i = 0; kingdom->treaties[i]; i++)
			;
		fprintf(fp, "Treaties       %li", i);
		for(i = 0; kingdom->treaties[i]; i++)
			fprintf(fp, " %li", kingdom->treaties[i]);
		fprintf(fp, "\n");
		for(i = 0; kingdom->wars[i]; i++)
			;
		fprintf(fp, "Wars           %li", i);
		for(i = 0; kingdom->wars[i]; i++)
			fprintf(fp, " %li", kingdom->wars[i]);
		fprintf(fp, "\n");
		fprintf(fp, "\n\n");
	}

	fprintf(fp, "\nEnd\n\n");
	fclose(fp);
	fpReserve = fopen(NULL_FILE, "r");
}


////////// MAIN FUNCTIONS ////////////////
KINGDOM_DATA *get_kingdom(long id)
{
	KINGDOM_DATA *kingdom;

	for(kingdom = kingdom_list; kingdom; kingdom = kingdom->next)
	{
		if(kingdom->id == id)
			return kingdom;
	}

	return &kingdom_default;
}

long num_kingdoms(void)
{
	KINGDOM_DATA *kingdom;
	long total = 0;

	for(kingdom = kingdom_list; kingdom; kingdom = kingdom->next)
		total++;
	return total;
}

void kingdom_update(void)
{
	KINGDOM_DATA *kingdom;
	KINGDOM_DATA *kingdom_next;

	for(kingdom = kingdom_list; kingdom; kingdom = kingdom_next)
	{
		kingdom_next = kingdom->next;

		if(kingdom->upkeep == 0)
		{
			if(kingdom->realpoints >= 1000)
			{
				kingdom_message(0, "%s is now an {BACTIVE{x Kingdom, kill to survive!", kingdom->name);
				kingdom->upkeep = 1;
			}
			continue;
		}

// update out for now!		kingdom->points -= 1;

		if(kingdom->points <= 0)
		{
			kingdom_message(0, "%s has {RFAILED{X to survive!", kingdom->name);
			delete_kingdom(kingdom->id);
		}
	}
	write_kingdoms();
}

void kingdom_message(long id, char *message, ...)
{
	DESCRIPTOR_DATA *d;
	char buf[MAX_STRING_LENGTH];
	char realbuf[MAX_STRING_LENGTH];
	va_list ap;

	if(!get_kingdom(id) && id != 0)
		return;

	va_start(ap, message);
	(void)vsnprintf(buf, MAX_STRING_LENGTH, message, ap);

	sprintf(realbuf, "{C[{B%s{N{C]{N %s\n\r", id == 0 ? "Kingdoms" : get_kingdom(id)->name, buf);

	for(d = descriptor_list; d; d = d->next)
	{
		if(d->connected != CON_PLAYING
		   || !d->character
		   || (d->character->pcdata->kingdom != id && id != 0 && !IS_IMMORTAL(d->character)
		       && !has_treaty(d->character->pcdata->kingdom, id))
		   || (id != 0 && IS_SET(d->character->deaf, CHANNEL_KTALK)))
			continue;

		send_to_char(realbuf, d->character);
	}
	va_end(ap);
}


long award_points(CHAR_DATA * ch, CHAR_DATA * victim)
{
	KINGDOM_DATA *kingdom;
	KINGDOM_DATA *vkingdom;
	AREA_DATA *area = 0;
	ASSIST_DATA *assist;
	long points = 0;
	long damage = 0;
	long totaldamage = 0;
	long totalpoints = 0;
	long mobdamage = 0;
	long percent = 0;

	if(!ch || !victim || IS_NPC(ch) || IS_NPC(victim))
		return FALSE;

	update_assist(victim, ch, 1);

	area = ch->in_room ? ch->in_room->area : 0;
	kingdom = get_kingdom(ch->pcdata->kingdom);
	vkingdom = get_kingdom(victim->pcdata->kingdom);

	points = victim->pcdata->status * number_range(5, 10);
	points += victim->max_hit / 100;
	points += is_leader(ch, kingdom->id) ? number_range(0, points) : 0;
	points /= vkingdom ? 1 : 2;

	if(kingdom)
	{
		kingdom->pks++;
		kingdom->points += is_at_war(kingdom->id, vkingdom->id) ? points * 2 : points;
		kingdom->realpoints += is_at_war(kingdom->id, vkingdom->id) ? points * 2 : points;
	}

	if(vkingdom)
	{
		vkingdom->pds++;

		if(is_at_war(vkingdom->id, kingdom->id))
		{
			points /= number_range(1, 3);
			kingdom_message(vkingdom->id, "You are at war with the %s Kingdom and lose %li points!", kingdom->name, points);
			vkingdom->points -= points;
		}

/*		if( calc_rating(vkingdom) < 200 && vkingdom != &kingdom_default
		&&  (vkingdom->pds + vkingdom->pks) > 50 )
		{
			kingdom_message(0,buf,"%s has {DDESTROYED{N the %s Kingdom!",
				ch->name,
				vkingdom->name );
			delete_kingdom(vkingdom->id);
		}
*/
	}

	totalpoints = UMAX(1, points);
	totaldamage = UMAX(1, find_total_assist_damage(victim));
	mobdamage = UMAX(1, find_mob_assist_damage(victim));
	damage = UMAX(1, find_assist_damage(victim, ch));
	percent = (long) UMAX(0, ((float) damage / (float) totaldamage) * 100);
	points = (long) UMAX(0, (float) totalpoints * ((float) damage / (float) (totaldamage - mobdamage)));

	kingdom_message(0, "[%s]%s has decapitated [%s]%s for %li(%li) points! (%li%% damage out of %li)",
			kingdom->name, ch->name, vkingdom->name, victim->name, points, totalpoints, percent, totaldamage);

	remove_assist(victim, ch);

	for(assist = victim->pcdata->assist; assist; assist = assist->next)
	{
		if(!assist->ch)
			continue;

		if(!IS_NPC(assist->ch))
		{
			damage = find_assist_damage(victim, assist->ch);
			percent = (long) (((float) damage / (float) totaldamage) * 100);
			points = (long) ((float) totalpoints * ((float) damage / (float) (totaldamage - mobdamage)));
			kingdom = get_kingdom(assist->ch->pcdata->kingdom);
			assist->ch->pcdata->assists++;

			if(kingdom != &kingdom_default)
			{
				kingdom->points += points;
				kingdom->assists++;
			}

			kingdom_message(0, "[%s]%s assists for %li points. (%li%% damage)",
					kingdom->name, assist->ch->name, points, percent);
		}
		else
		{
			damage = find_assist_damage(victim, assist->ch);
			percent = (long) (((float) damage / (float) totaldamage) * 100);

			kingdom_message(0, "[{GMobile{x]%s assists doing %li%% damage.", assist->ch->short_descr, percent);
		}
	}

	while((assist = victim->pcdata->assist))
		remove_assist(victim, assist->ch);

	write_kingdoms();
	return points;
}


///////////// HELPER FUNCTIONS //////////////////
KINGDOM_DATA *create_kingdom(void)
{
	KINGDOM_DATA *kingdom;
	long i=0;

	while(++i)
	{
		if(get_kingdom(i) == &kingdom_default)
			break;
	}

	kingdom = malloc(sizeof(KINGDOM_DATA));
	kingdom->name = str_dup("");
	kingdom->noansiname = str_dup("");
	kingdom->leader = str_dup("");
	kingdom->members = str_dup("");
	kingdom->trustees = str_dup("");
	kingdom->id = i;
	kingdom->treaty_pending = 0;
	kingdom->pds = 0;
	kingdom->pks = 0;
	kingdom->assists = 0;
	kingdom->points = 0;
	kingdom->realpoints = 0;
	kingdom->upkeep = 0;
	kingdom->recall = 0;
	kingdom->treaties = malloc(sizeof(long)*1);
	kingdom->treaties[0] = 0;
	kingdom->wars = malloc(sizeof(long)*1);
	kingdom->wars[0] = 0;
	kingdom->next = kingdom_list;
	kingdom_list = kingdom;
	return kingdom;
}

void update_char_kingdom(CHAR_DATA * ch)
{
	KINGDOM_DATA *kingdom;

	if(!ch || IS_NPC(ch))
		return;

	if(ch->pcdata->kingdom > 0)
	{
		kingdom = get_kingdom(ch->pcdata->kingdom);

		if(kingdom == &kingdom_default)
		{
			ch->pcdata->kingdom = 0;
			return;
		}

		if(strstr(kingdom->members, ch->name) == 0 && !is_leader(ch, kingdom->id))
		{
			remove_trustee(kingdom->id, ch->name);
			ch->pcdata->kingdom = 0;
			return;
		}
		return;
	}

	if(ch->pcdata->krank[0] != '\0')
	{
		free_string(ch->pcdata->krank);
		ch->pcdata->krank = str_dup("");
	}

	for(kingdom = kingdom_list; kingdom; kingdom = kingdom->next)
	{
		if(strstr(kingdom->members, ch->name) != 0)
			remove_member(kingdom->id, ch->name);
		if(strstr(kingdom->trustees, ch->name) != 0)
			remove_trustee(kingdom->id, ch->name);
	}
}

void update_kingdoms(void)
{
	KINGDOM_DATA *kingdom;
	long i;

	for(kingdom = kingdom_list; kingdom; kingdom = kingdom->next)
	{
		for(i = 0; kingdom->treaties[i]; i++)
		{
			if(get_kingdom(kingdom->treaties[i]) == &kingdom_default)
				remove_treaty(kingdom->id, kingdom->treaties[i]);
		}

		for(i = 0; kingdom->wars[i]; i++)
		{
			if(get_kingdom(kingdom->wars[i]) == &kingdom_default)
				remove_war(kingdom->id, kingdom->wars[i]);
		}
	}
}

float calc_kd(KINGDOM_DATA * kingdom)
{
	float pks;

	if(!kingdom)
		return 0;
	if(kingdom->id == 0)
		return -1;

	pks = (float) kingdom->pks + (kingdom->assists == 0 ? 0 : (float) (kingdom->assists / 2.0));

	if(kingdom->pds == 0)
		return (float) pks;
	if(kingdom->pks == 0)
		return 0.0;

	return (float) pks / (float) kingdom->pds;
}

float calc_avg(KINGDOM_DATA * kingdom)
{
	if(!kingdom)
		return 0;
	if(kingdom->id == 0)
		return -1;
	if(kingdom->pks == 0)
		return (float) kingdom->realpoints;
	if(kingdom->realpoints == 0)
		return 0;

	return (float) kingdom->realpoints / (float) kingdom->pks;
}

float calc_rating(KINGDOM_DATA * kingdom)
{
	if(!kingdom)
		return 0;
	if(kingdom->id == 0)
		return -1;
	return (calc_kd(kingdom) * calc_avg(kingdom));
}

bool is_leader(CHAR_DATA * ch, long id)
{
	if(!ch || get_kingdom(id) == &kingdom_default)
		return FALSE;

	if(IS_NPC(ch) || ch->pcdata->kingdom == 0 || str_cmp(ch->name, get_kingdom(ch->pcdata->kingdom)->leader))
		return FALSE;

	return TRUE;
}

bool is_trustee(CHAR_DATA * ch, long id)
{
	KINGDOM_DATA *kingdom = get_kingdom(id);

	if(!ch || kingdom == &kingdom_default)
		return FALSE;

	if(is_leader(ch, id) || strstr(kingdom->trustees, ch->name) != 0)
		return TRUE;

	return FALSE;
}

void delete_kingdom(long id)
{
	KINGDOM_DATA *kingdom = get_kingdom(id);
	KINGDOM_DATA *kingp;

	if(kingdom == &kingdom_default)
		return;

	if(kingdom == kingdom_list)
	{
		kingdom_list = kingdom->next;
		kingdom->next = 0;
	}
	else
	{
		for(kingp = kingdom_list; kingp; kingp = kingp->next)
		{
			if(kingp->next == kingdom)
			{
				kingp->next = kingdom->next;
				kingdom->next = 0;
				break;
			}
		}

		if(!kingp)
		{
			bug("delete_kingdom: kingp not found!", 0);
			return;
		}
	}

	kingdom_message(0, "The %s Kingdom has been {DDESTROYED{N.", kingdom->name);

	free(kingdom->treaties);
	free(kingdom->wars);
	free_string(kingdom->members);
	free_string(kingdom->trustees);
	free_string(kingdom->leader);
	free_string(kingdom->name);
	free_string(kingdom->noansiname);

	free(kingdom);
	update_kingdoms();
}

void add_member(long id, CHAR_DATA * ch)
{
	KINGDOM_DATA *kingdom;
	char buf[MAX_STRING_LENGTH];

	if(!ch || (kingdom = get_kingdom(id)) == &kingdom_default)
		return;

	if(kingdom->members[0] != '\0')
	{
		sprintf(buf, "%s %s", kingdom->members, ch->name);
	}
	else
	{
		sprintf(buf, "%s", ch->name);
	}

	free_string(kingdom->members);
	kingdom->members = str_dup(buf);
	ch->pcdata->kingdom = id;

	sprintf(buf, "You have joined the %s Kingdom.\n\r", kingdom->name);
	send_to_char(buf, ch);
	kingdom_message(0, "%s has joined the %s Kingdom!", ch->name, kingdom->name);
	write_kingdoms();
	return;
}

bool remove_member(long id, char *name)
{
	CHAR_DATA *ch = 0;
	KINGDOM_DATA *kingdom;
	char buf[MAX_STRING_LENGTH];
	char *str;

	if(!name || name[0] == '\0' || (kingdom = get_kingdom(id)) == &kingdom_default)
		return FALSE;

	if((ch = get_char_world(char_list, name)) != 0 && !IS_NPC(ch))
	{
		ch->pcdata->kingdom_invite = 0;
		ch->pcdata->kingdom = 0;
		sprintf(buf, "You have left the %s Kingdom.", kingdom->name);
		send_to_char(buf, ch);

		free_string(ch->pcdata->krank);
		ch->pcdata->krank = str_dup("");
	}

	if((str = str_sep(kingdom->members, name)) == 0)
		return FALSE;

	free_string(kingdom->members);
	kingdom->members = str;

	kingdom_message(0, "%s has left the %s Kingdom.", name, kingdom->name);
	write_kingdoms();
	return TRUE;
}

void add_trustee(long id, CHAR_DATA * ch)
{
	KINGDOM_DATA *kingdom;
	char buf[MAX_STRING_LENGTH];

	if(!ch || (kingdom = get_kingdom(id)) == &kingdom_default)
		return;

	if(kingdom->trustees && kingdom->trustees[0] != '\0')
	{
		sprintf(buf, "%s %s", kingdom->trustees, ch->name);
		free_string(kingdom->trustees);
	}
	else
	{
		sprintf(buf, "%s", ch->name);
	}

	kingdom->trustees = str_dup(buf);

	sprintf(buf, "You are now a trustee of the %s Kingdom!\n\r", kingdom->name);
	send_to_char(buf, ch);
	kingdom_message(0, "%s is now a trusted member of the %s Kingdom!", ch->name, kingdom->name);
	write_kingdoms();
	return;
}

bool remove_trustee(long id, char *name)
{
	KINGDOM_DATA *kingdom;
	char *str;

	if(!name || name[0] == '\0' || (kingdom = get_kingdom(id)) == &kingdom_default)
		return FALSE;

	if((str = str_sep(kingdom->trustees, name)) == 0)
		return FALSE;

	free_string(kingdom->trustees);
	kingdom->trustees = str;

	kingdom_message(0, "%s is no longer a trusted member of the %s Kingdom.", name, kingdom->name);
	write_kingdoms();
	return TRUE;
}

bool has_treaty(long id1, long id2)
{
	KINGDOM_DATA *kingdom = get_kingdom(id1);
	long i;

	if(kingdom == &kingdom_default || !kingdom->treaties)
		return FALSE;

	for(i = 0; kingdom->treaties[i]; i++)
	{
		if(kingdom->treaties[i] == id2)
			return TRUE;
	}

	return FALSE;
}


void add_treaty(long id1, long id2)
{
	KINGDOM_DATA *kingdom = get_kingdom(id1);
	KINGDOM_DATA *kingdom2 = get_kingdom(id2);
	long i;

	if(kingdom == &kingdom_default || kingdom2 == &kingdom_default)
		return;

	kingdom_message(0, "The %s Kingdom has formed a treaty with The %s Kingdom.", kingdom->name, kingdom2->name);

	for( i = 0; kingdom->treaties[i]; i++ )
		;
	kingdom->treaties	= realloc(kingdom->treaties, sizeof(long*) * (i==0?2:i + 2));
	kingdom->treaties[i]	= id2;
	kingdom->treaties[i+1]	= 0;

	for( i = 0; kingdom2->treaties[i]; i++ )
		;
	kingdom2->treaties	= realloc(kingdom2->treaties, sizeof(long*) * (i==0?2:i + 2));
	kingdom2->treaties[i]	= id1;
	kingdom2->treaties[i+1]	= 0;
}

void remove_treaty(long id1, long id2)
{
	KINGDOM_DATA *kingdom = get_kingdom(id1);
	KINGDOM_DATA *kingdom2 = get_kingdom(id2);
	long badkingdom=id2;

	if(!has_treaty(id1, id2))
		return;

	kingdom_message(0, "The %s Kingdom has broken their treaty with The %s Kingdom!", kingdom->name, kingdom2->name);

	kingdom->treaties[badkingdom]	= 0;
	kingdom->treaties		= realloc(kingdom->treaties,sizeof(long) * badkingdom);

	if (kingdom2 == &kingdom_default)
		return;
			badkingdom = kingdom->id;
	kingdom2->treaties[badkingdom] = kingdom2->treaties[badkingdom+1];
        kingdom2->treaties              = realloc(kingdom2->treaties,sizeof(long) * badkingdom);
	kingdom2->treaties[badkingdom+1]		= 0;
}

bool is_at_war(long id1, long id2)
{
	KINGDOM_DATA *kingdom = get_kingdom(id1);
	long i;

	if(kingdom == &kingdom_default || !kingdom->wars)
		return FALSE;

	if(id1 == id2)
		return FALSE;

	for(i = 0; kingdom->wars[i]; i++)
	{
		if(kingdom->wars[i] == id2)
			return TRUE;
	}

	return FALSE;
}

void add_war(long id1, long id2)
{
	KINGDOM_DATA *kingdom = get_kingdom(id1);
	KINGDOM_DATA *kingdom2 = get_kingdom(id2);
	long i;

	if(kingdom == &kingdom_default || kingdom2 == &kingdom_default)
		return;

	kingdom_message(0, "The %s Kingdom is now at {RWAR{N with The %s Kingdom.", kingdom->name, kingdom2->name);

	for( i = 0; kingdom->wars[i]; i++ )
		;
	kingdom->wars		= realloc(kingdom->wars, sizeof(long) * (i==0?2:i + 2));
	kingdom->wars[i]	= id2;
	kingdom->wars[i+1]	= 0;
}

void remove_war(long id1, long id2)
{
	KINGDOM_DATA *kingdom = get_kingdom(id1);
	KINGDOM_DATA *kingdom2 = get_kingdom(id2);
	long badkingdom=-1;
	long i;

	if(!is_at_war(id1, id2))
		return;

	kingdom_message(0, "The %s Kingdom is no longer at war with The %s Kingdom!", kingdom->name, kingdom2->name);

	for( i = 0; kingdom->wars[i]; i++ )
	{
		if( kingdom->wars[i] == id2 )
			badkingdom = i;
	}
	kingdom->wars[badkingdom]	= kingdom->wars[i-1];
	kingdom->wars			= realloc(kingdom->wars, sizeof(long) * i);
	kingdom->wars[i-1]		= 0;
}

long ansistrlen(char *buf)
{
	long length = 0;

	while(buf && *buf != '\0')
	{
		if(*buf == '{')
		{
			buf += 2;
			continue;
		}
		length++;
		buf++;
	}
	return length;
}

/////////////// DO FUNCTIONS ///////////////////
void do_kingdom(CHAR_DATA * ch, char *argument)
{
	KINGDOM_DATA *kingdom;
	CHAR_DATA *vch = 0;
	DESCRIPTOR_DATA *d = 0;
	char buf[MAX_STRING_LENGTH];
	char arg1[MAX_STRING_LENGTH];
	char arg2[MAX_STRING_LENGTH];
	char arg3[MAX_STRING_LENGTH];
	long id = 0;
	long i = 0;

	if(IS_NPC(ch))
		return;

	smash_tilde(argument);
	argument = one_argument_case(argument, arg1);
	argument = one_argument_case(argument, arg2);
	strcpy(arg3, argument);

	if(IS_IMMORTAL(ch) && ch->level == MAX_LEVEL)
	{
		if(!str_cmp(arg1, "save"))
		{
			write_kingdoms();
			sprintf(buf,"cp %s %s.bak",KINGDOM_FILE,KINGDOM_FILE);
			system(buf);
			send_to_char("Done, and made backup.\n\r", ch);
			return;
		}

		if(arg1[0] == '\0')
		{
			send_to_char("Syntax: kingdom create\n\r", ch);
			return;
		}

		if(!str_cmp(arg1, "create"))
		{
			create_kingdom();
			write_kingdoms();
			send_to_char("Done.\n\r", ch);
			return;
		}

		if((!is_number(arg1) || !str_cmp(arg1, "help")))
		{
			send_to_char("Syntax: kingdom <id #> <attribute> <value>\n\r", ch);
			send_to_char("        kingdom <id #> delete\n\r", ch);
			send_to_char("\n\r", ch);
			send_to_char("Attributes:\n\r", ch);
			send_to_char("  leader name points pks pds realpoints\n\r", ch);
			send_to_char("  noansiname\n\r", ch);
			send_to_char("\n\r", ch);
		}
		else
		{
			id = atoi(arg1);

			if((kingdom = get_kingdom(id)) == &kingdom_default)
			{
				send_to_char("There is no such kingdom with that ID.\n\r", ch);
				return;
			}

			if(!str_cmp(arg2, "leader"))
			{
				if((vch = get_char_world(ch, arg3)) == 0)
				{
					send_to_char("The leader must be logged on.\n\r", ch);
					return;
				}

				free_string(kingdom->leader);
				kingdom->leader = str_dup(arg3);
				vch->pcdata->kingdom = id;
				send_to_char("Done.\n\r", ch);
				return;
			}

			if(!str_cmp(arg2, "name"))
			{
				free_string(kingdom->name);
				kingdom->name = str_dup(arg3);
				send_to_char("Done.\n\r", ch);
				return;
			}

			if(!str_cmp(arg2, "noansiname"))
			{
				free_string(kingdom->noansiname);
				kingdom->noansiname = str_dup(arg3);
				send_to_char("Done.\n\r", ch);
				return;
			}

			if(!str_cmp(arg2, "delete") && !str_cmp(arg3, "yes"))
			{
				delete_kingdom(kingdom->id);
				return;
			}

			i = atoi(arg3);

			if(!str_cmp(arg2, "points"))
			{
				kingdom->points = i;
				send_to_char("Done.\n\r", ch);
				return;
			}
			if(!str_cmp(arg2, "realpoints"))
			{
				kingdom->realpoints = i;
				send_to_char("Done.\n\r", ch);
				return;
			}
			if(!str_cmp(arg2, "pds"))
			{
				kingdom->pds = i;
				send_to_char("Done.\n\r", ch);
				return;
			}
			if(!str_cmp(arg2, "pks"))
			{
				kingdom->pks = i;
				send_to_char("Done.\n\r", ch);
				return;
			}
			if(!str_cmp(arg2, "assists"))
			{
				kingdom->assists = i;
				send_to_char("Done.\n\r", ch);
				return;
			}
		}
	}

	if(ch->pcdata->kingdom == 0)	// player not in a kingdom
	{
		if(!str_cmp(arg1, "create"))
		{
			if(ch->pcdata->quest < 1000)
			{
				send_to_char("You need 1000 quest points to create a new kingdom.\n\r", ch);
				return;
			}

			if(!arg2 || arg2[0] == '\0' || !arg3 || arg3[0] == '\0')
			{
				send_to_char
					("Syntax (DO NOT MESS THIS UP!): kingdom create \"name WITH ansi\" \"name WITHOUT ansi\"\n\r",
					 ch);
				send_to_char("I suggest you SAY the name a few times to make SURE THE ANSI IS CORRECT.\n\r",
					     ch);
				send_to_char("And if you dont put a final {{x your kingdom will be DELETED.\n\r", ch);
				send_to_char("If your kingdom name has more than one word surround it in QUOTES!\n\r", ch);
				return;
			}

			if(ansistrlen(arg2) > 20 || strlen(arg3) > 20)
			{
				send_to_char("String length is limited to 20 characters!\n\r", ch);
				return;
			}

			kingdom = create_kingdom();
			ch->pcdata->kingdom = kingdom->id;
			ch->pcdata->quest -= 1000;
			free_string(kingdom->leader);
			kingdom->leader = str_dup(ch->name);
			free_string(kingdom->name);
			kingdom->name = str_dup(arg2);
			free_string(kingdom->noansiname);
			kingdom->noansiname = str_dup(arg3);

			kingdom_message(0, "%s has {WCREATED{X The %s Kingdom!", ch->name, kingdom->name);
			write_kingdoms();
			return;
		}

		if(ch->pcdata->kingdom_invite == 0)
		{
			send_to_char("You don't belong to a kingdom.\n\r", ch);
			return;
		}

		// make sure kingdom still exists
		if(get_kingdom(ch->pcdata->kingdom_invite) == &kingdom_default)
		{
			send_to_char("That kingdom was destroyed.\n\r", ch);
			return;
		}

		if(!str_cmp(arg1, "accept"))
		{
			add_member(ch->pcdata->kingdom_invite, ch);
			return;
		}

		if(!str_cmp(arg1, "decline"))
		{
			send_to_char("You decline the kingdom's invitation.\n\r", ch);

			if((vch = get_char_world(ch, get_kingdom(ch->pcdata->kingdom_invite)->leader)) != 0)
			{
				sprintf(buf, "%s has declined your invitation.\n\r", ch->name);
				send_to_char(buf, vch);
			}
			ch->pcdata->kingdom_invite = 0;
			return;
		}

		send_to_char("Type either kingdom accept or kingdom decline.\n\r", ch);
		return;
	}

	kingdom = get_kingdom(ch->pcdata->kingdom);

	if(!str_cmp(arg1, "leave") && !str_cmp(arg2, "for") && !str_cmp(arg3, "good"))
	{
		send_to_char("You leave the kingdom!\n\r", ch);
		remove_member(kingdom->id, ch->name);
		remove_trustee(kingdom->id, ch->name);
		return;
	}

	if(!str_prefix(arg1, "report"))
	{
		long percent;
		char *color;

		buf[0] = '\0';
		sprintf(buf, "%-4s %-12s{X %5s :: %s\n\r",
			"CMBT", "Name", "Hits", "Room" );

		for(d = descriptor_list; d; d = d->next)
		{
			if(d->connected != CON_PLAYING || !(vch = d->character) || vch->pcdata->kingdom != kingdom->id)
				continue;

			percent = (long) ((float) UMAX(1, vch->hit) / (float) UMAX(1, vch->max_hit) * 100);

			if(percent < 10)
				color = "{R";
			else if(percent < 40)
				color = "{B";
			else if(percent < 70)
				color = "{Y";
			else
				color = "{C";

			sprintf(buf + strlen(buf), "%-4s %s%-12s{X %5li :: %s\n\r",
				vch->fighting ? !IS_NPC(vch->fighting) ? "{R(PK){X" : "{r(F){X" : "",
				 color, vch->name, vch->hit, !vch->in_room ? "somewhere" : vch->in_room->name);
		}
		send_to_char(buf, ch);
		return;
	}

	if(!is_trustee(ch, kingdom->id))
	{
		send_to_char("Only leaders and trustees have power to modify their kingdom.\n\r", ch);
		return;
	}

	if(arg1[0] == '\0' || arg2[0] == '\0')
	{
		if(is_leader(ch, kingdom->id) || IS_IMMORTAL(ch))
		{
			send_to_char("Syntax:\n\r", ch);
			send_to_char("        kingdom report\n\r", ch);
			send_to_char("        kingdom invite <player>\n\r", ch);
			send_to_char("        kingdom outcast <member>\n\r", ch);
			send_to_char("        kingdom war <kingdom ID>\n\r", ch);
			send_to_char("        kingdom treaty <kingdom ID>\n\r", ch);
			send_to_char("        kingdom rank <member> <rank>\n\r", ch);
			send_to_char("        kingdom trust <member>\n\r", ch);
			send_to_char("        kingdom recall\n\r", ch);
			send_to_char("\n\r", ch);
		}
		send_to_char("Syntax: kingdom scry <person>\n\r", ch);
		return;
	}

	if(is_leader(ch, kingdom->id))
	{
		if(!str_cmp(arg1, "recall"))
		{
			if(kingdom->points < KINGDOM_RECALL_COST)
			{
				sprintf(buf, "It costs %d kingdom points to set a recall point.\n\r", KINGDOM_RECALL_COST);
				send_to_char(buf, ch);
				return;
			}

			if(ch->in_room == 0)
			{
				send_to_char("You are in a 0 room, eek.\n\r", ch);
				return;
			}

			kingdom->points -= KINGDOM_RECALL_COST;
			kingdom->recall = ch->in_room->vnum;

			kingdom_message(kingdom->id, "{MThe Kingdom Recall has been set to -> [%s]", ch->in_room->name);
			return;
		}

		if(!str_cmp(arg1, "trust"))
		{
			if((vch = get_char_world(ch, arg2)) == 0 || IS_NPC(vch))
			{
				send_to_char("They need to be logged on.\n\r", ch);
				return;
			}

			if(vch->pcdata->kingdom != ch->pcdata->kingdom)
			{
				send_to_char("They aren't in your kingdom!\n\r", ch);
				return;
			}

			arg2[0] = UPPER(arg2[0]);
			if( strstr(kingdom->trustees, arg2) == 0 )
				add_trustee(kingdom->id, vch);
			else
				remove_trustee(kingdom->id, arg2);
			return;
		}

		if(!str_cmp(arg1, "invite"))
		{
			if((vch = get_char_world(ch, arg2)) == 0)
			{
				send_to_char("They aren't logged on.\n\r", ch);
				return;
			}

			if(IS_NPC(vch))
			{
				send_to_char("That's an NPC.\n\r", ch);
				return;
			}

			if(vch->pcdata->kingdom)
			{
				send_to_char("They are already a part of another kingdom!\n\r", ch);
				return;
			}

			vch->pcdata->kingdom_invite = ch->pcdata->kingdom;
			sprintf(buf, "You are invited to join the %s Kingdom by %s.\n\r"
				"Type kingdom accept or kingdom decline.\n\r", kingdom->name, ch->name);
			send_to_char(buf, vch);
			sprintf(buf, "You invite %s to join your glorious kingdom.\n\r", vch->name);
			send_to_char(buf, ch);
			return;
		}

		if(!str_cmp(arg1, "outcast"))
		{
			arg2[0] = UPPER(arg2[0]);
			if( strstr(kingdom->members, arg2) == 0 )
			{
				send_to_char("That person isn't a member!\n\r",ch);
				return;
			}

			if(remove_member(ch->pcdata->kingdom, arg2))
				kingdom_message(0, "%s has been outcasted from the %s Kingdom!", capitalize(arg2), kingdom->name);
			remove_trustee(kingdom->id, arg2);
			return;
		}

		if(!str_cmp(arg1, "treaty"))
		{

			if(kingdom->treaty_pending)
			{
				if(!get_kingdom(kingdom->treaty_pending))
				{
					kingdom->treaty_pending = 0;
					return;
				}

				if(has_treaty(kingdom->id, kingdom->treaty_pending))
				{
					send_to_char("You already have a treaty with them.\n\r", ch);
					return;
				}

				if(!str_cmp(arg2, "accept"))
				{
					add_treaty(kingdom->id, kingdom->treaty_pending);
					kingdom->treaty_pending = 0;
					return;
				}

				if(!str_cmp(arg2, "decline"))
				{
					kingdom_message(kingdom->id, "Your kingdom declines a treaty with the %s Kingdom.",
						get_kingdom(kingdom->treaty_pending)->name);

					kingdom_message(kingdom->treaty_pending, "%s Kingdom has declined your treaty offer.", kingdom->name);
					kingdom->treaty_pending = 0;
					return;
				}

				sprintf(buf, "You have a treaty offer pending with the %s Kingdom.\n\r"
					"Please type kingdom treaty accept or kingdom treaty decline.\n\r",
					get_kingdom(kingdom->treaty_pending)->name);
				send_to_char(buf, ch);
				return;
			}

			if(!is_number(arg2) || get_kingdom(atoi(arg2)) == &kingdom_default)
			{
				send_to_char("There is no such kingdom to make a treaty with.\n\r", ch);
				return;
			}
			i = atoi(arg2);

			if(kingdom->id == i)
			{
				send_to_char("You already have a treaty with yourself, smart one.\n\r", ch);
				return;
			}

			if(is_at_war(kingdom->id, i))
			{
				send_to_char("You are at {RWAR{N with that kingdom!\n\r", ch);
				return;
			}

			if(has_treaty(kingdom->id, i))
			{
				remove_treaty(kingdom->id, i);
				return;
			}

			if(get_kingdom(i)->treaty_pending)
			{
				send_to_char("They are already pending a treaty confirmation.\n\r", ch);
				return;
			}

			get_kingdom(i)->treaty_pending = kingdom->id;
			kingdom_message(i, "The %s Kingdom is asking for a treaty with your kingdom.", kingdom->name);
			kingdom_message(kingdom->id, "You ask the %s Kingdom for a treaty.", get_kingdom(i)->name);
			return;
		}

		if(!str_cmp(arg1, "war"))
		{

			if(!is_number(arg2) || get_kingdom(atoi(arg2)) == &kingdom_default)
			{
				send_to_char("There is no such kingdom to make war with.\n\r", ch);
				return;
			}

			i = atoi(arg2);

			if(i == kingdom->id)
			{
				send_to_char("That wouldn't be a very smart political move.\n\r", ch);
				return;
			}

			if(get_kingdom(i) == &kingdom_default)
			{
				send_to_char("That kingdom doesn't exist.\n\r", ch);
				return;
			}

			if(has_treaty(kingdom->id, i))
			{
				send_to_char("But you have a treaty with them!\n\r", ch);
				return;
			}

			if(is_at_war(kingdom->id, i))
			{
				remove_war(kingdom->id, i);
				return;
			}

			add_war(kingdom->id, i);
			return;
		}

		if(!str_cmp(arg1, "rank"))
		{
			if(!arg3 || arg3[0] == '\0')
			{
				send_to_char("Rank who as what?\n\r", ch);
				return;
			}

			if((vch = get_char_world(ch, arg2)) == 0 || IS_NPC(vch))
			{
				send_to_char("They aren't on at the moment.\n\r", ch);
				return;
			}

			if(vch->pcdata->kingdom != ch->pcdata->kingdom)
			{
				send_to_char("They aren't apart of your kingdom!\n\r", ch);
				return;
			}

			if(ansistrlen(arg3) > 15)
			{
				send_to_char("String length is limited to 20 characters.\n\r", ch);
				return;
			}

			free_string(vch->pcdata->krank);

			if(!str_cmp(arg3, "clear"))
			{
				vch->pcdata->krank = str_dup("");
				return;
			}

			arg3[0] = UPPER(arg3[0]);
			vch->pcdata->krank = str_dup(arg3);
			send_to_char("Done.\n\r", ch);
			return;
		}
	}

	if(!str_cmp(arg1, "scry"))
	{
		if(arg2[0] == '\0')
		{
			send_to_char("You must supply the name of the person.\n\r", ch);
			return;
		}

		if((vch = get_char_world(ch, arg2)) == 0 || IS_NPC(vch))
		{
			send_to_char("They are not online.\n\r", ch);
			return;
		}

		if(kingdom->points < KINGDOM_SCRY_COST)
		{
			sprintf(buf, "It costs %d Kingdom Points to do this.\n\r", KINGDOM_SCRY_COST);
			send_to_char(buf, ch);
			return;
		}

		kingdom_message(kingdom->id, "%s has {cscryed{N %s {C. o O{N ({C%s{N)",
			ch->name, vch->name, vch->in_room == 0 ? "Nowhere" : vch->in_room->name);
		kingdom->points -= KINGDOM_SCRY_COST;
		return;
	}

	// default message
	do_kingdom(ch, "blah");
}


void do_klist(CHAR_DATA * ch, char *argument)
{
	KINGDOM_DATA *kingdom;
	char buf[MAX_STRING_LENGTH];
	long i = 0, iw = 0;
	long highest = 0;

	if(is_number(argument))
	{
		buf[0] = '\0';

		if((kingdom = get_kingdom(atoi(argument))) == &kingdom_default)
		{
			send_to_char("There is no kingdom with that ID.\n\r", ch);
			return;
		}
		kingdom = get_kingdom(atoi(argument));

		sprintf(buf + strlen(buf), " Leader: %-25s     Name: %s\n\r", kingdom->leader, kingdom->name);
		sprintf(buf + strlen(buf), "    PKS: %-25li      PDS: %li\n\r", kingdom->pks, kingdom->pds);
		sprintf(buf + strlen(buf), "    K/D: %-25.2f\n\r", calc_kd(kingdom));
		sprintf(buf + strlen(buf), " Points: %-25li    Total: %li\n\r", kingdom->points, kingdom->realpoints);
		sprintf(buf + strlen(buf), " Rating: %-25.2f      AVG: %.2f\n\r", calc_rating(kingdom), calc_avg(kingdom));
		sprintf(buf + strlen(buf), "Members:\n\r%s\n\r", kingdom->members);
		sprintf(buf + strlen(buf), "Trustees:\n\r%s\n\r", kingdom->trustees);
		sprintf(buf + strlen(buf), "Treaties:\n\r");
		for(i = 0; i < kingdom->treaties[i]; i++)
		{
			sprintf(buf + strlen(buf), "%-25s     %s",
				get_kingdom(kingdom->treaties[i]) !=
				0 ? get_kingdom(kingdom->treaties[i])->name : "Null", (i + 1) % 2 == 0 ? "\n\r" : "");
		}
		if((i + 1) % 2 == 0)
			sprintf(buf + strlen(buf), "\n\r");

		sprintf(buf + strlen(buf), "Wars:\n\r");
		for(i = 0; i < kingdom->wars[i]; i++)
		{
			sprintf(buf + strlen(buf), "%-25s     %s",
				get_kingdom(kingdom->wars[i]) != 0 ? get_kingdom(kingdom->wars[i])->name : "Null",
				(i + 1) % 2 == 0 ? "\n\r" : "");
		}
		if((i + 1) % 2 == 0)
			sprintf(buf + strlen(buf), "\n\r");

		sprintf(buf + strlen(buf), "\n\r\n\r");
		send_to_char(buf, ch);
		return;
	}

	buf[0] = '\0';

	if(argument[0] != '\0')
	{
		if(!str_prefix(argument, "wars"))
		{
			sprintf(buf, "{RWARS{N:\n\r");
			for(kingdom = kingdom_list; kingdom; kingdom = kingdom->next)
			{
				for(iw = 0; iw < kingdom->wars[iw]; iw++)
					sprintf(buf + strlen(buf), "%s is at {RWAR{N with %s.\n\r",
						kingdom->name, get_kingdom(kingdom->wars[iw])->name);
			}
			sprintf(buf + strlen(buf), "\n\r");
			send_to_char(buf, ch);
			return;
		}

		if(!str_prefix(argument, "treaties"))
		{
			long *kingdoms;
			long x = 0, curr = 0;
			bool already = FALSE;
			bool logged = FALSE;

			kingdoms = malloc(sizeof(long) * num_kingdoms());
			memset(kingdoms, 0, sizeof(long) * num_kingdoms());

			sprintf(buf, "{GTREATIES{N, in no particular order:\n\r");

			for(kingdom = kingdom_list; kingdom; kingdom = kingdom->next, logged = FALSE)
			{
				for(iw = 0; iw < kingdom->treaties[i]; iw++)
				{
					for(x = 0; x < num_kingdoms(); x++)
					{
						if(kingdoms[x] == kingdom->treaties[iw])
						{
							already = TRUE;
							break;
						}
					}
					if(already)
					{
						already = FALSE;
						continue;
					}
					if(!logged)
					{
						kingdoms[curr] = kingdom->id;
						curr++;
						logged = TRUE;
					}

					sprintf(buf + strlen(buf), "%s and %s have a treaty together.\n\r",
						kingdom->name, get_kingdom(kingdom->treaties[iw])->name);
				}
			}
			free(kingdoms);
			sprintf(buf + strlen(buf), "\n\r");
			send_to_char(buf, ch);
			return;
		}

	}

	sprintf(buf, "[ID] %-10s [Pks] [Pds] [Ast] [K/D] [Points] [AVG] [Rating] [Name]\n\r", "[Leader]");

	for(kingdom = kingdom_list; kingdom; kingdom = kingdom->next)
	{
		if( kingdom->id > highest )
			highest = kingdom->id;
	}
	for(i = 1, iw=0; i <= highest; i++)
	{
		if( (kingdom = get_kingdom(i)) == &kingdom_default )
			continue;
		iw++;

		sprintf(buf + strlen(buf), "%s%-4li %-10s %-5li %-5li %-5li %-5.2f %-8li %-5li %-8li %s%s\n\r",
			i % 2 == 0 ? "{Y" : "{R",
			kingdom->id,
			kingdom->leader,
			kingdom->pks,
			kingdom->pds,
			kingdom->assists,
			calc_kd(kingdom),
			kingdom->points,
			(long) calc_avg(kingdom), (long) calc_rating(kingdom), kingdom->name, iw % 2 == 0 ? "{Y" : "{R");

	}

	sprintf(buf + strlen(buf), "{N\n\r");
	send_to_char(buf, ch);
	send_to_char("To get more information about a kingdom, type: klist <id#>\n\r", ch);
}


void do_krecall(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	CHAR_DATA *mount;
	ROOM_INDEX_DATA *location;
	KINGDOM_DATA *kingdom;

	if(IS_NPC(ch) || (kingdom = get_kingdom(ch->pcdata->kingdom)) == &kingdom_default)
	{
		send_to_char("You have no kingdom!\n\r", ch);
		return;
	}

	act("$n's body flickers with green energy.", ch, 0, 0, TO_ROOM);
	act("Your body flickers with green energy.", ch, 0, 0, TO_CHAR);

	if((location = get_room_index(kingdom->recall)) == 0)
	{
		send_to_char("Your kingdom has no recall set.\n\r", ch);
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



void do_ktalk(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	long id = 0;

	if(IS_NPC(ch))
		return;

	if(IS_SET(ch->act, PLR_SILENCE) && !IS_IMMORTAL(ch))
	{
		send_to_char("Your mouth won't open!  You must be silenced.\n\r", ch);
		return;
	}


	if(!IS_IMMORTAL(ch) && ch->pcdata->kingdom == 0)
	{
		send_to_char("You are not part of a kingdom!", ch);
		return;
	}

	if(IS_IMMORTAL(ch))
	{
		argument = one_argument(argument, arg);

		if(!is_number(arg))
		{
			send_to_char("You are an immortal, you must supply the kingdom ID # before the message.\n\r", ch);
			return;
		}
		id = atoi(arg);
	}
	else
		id = ch->pcdata->kingdom;

	kingdom_message(id, "{C[{W%s{N{C]{N %s", ch->name, argument);
}

//////// NON-KINGDOM SPECIFIC CODE ////////
char *str_sep(char *str, char *sep)
{
	char buf[MAX_STRING_LENGTH];
	long addlen = 0;
	long lensep;
	long lenstr;
	long i;
	long k;
	long compare;
	long beginning;

	if(str == 0 || sep == 0)
		return 0;

	lensep = strlen(sep);
	lenstr = strlen(str);

	for(i = 0, compare = 0, beginning = 0; i < lenstr; i++)
	{
		if(LOWER(str[i]) == LOWER(sep[compare]))
		{
			if(compare == 0)
			{
				if( i == 0 )
				{
					beginning = i;
				}
				else
				{
					beginning = i-1; // get rid of space behind name
					addlen = 1;
				}
			}

			if(++compare == lensep)
				break;
		}
		else
		{
			compare = 0;
			beginning = 0;
		}
	}
	lensep += addlen;

	if(compare > 0 && compare == lensep)
	{
		for(i = 0, k = 0; i < lenstr; i++)
		{
			if(i < beginning || i > (beginning + lensep - 1))
				buf[k++] = str[i];
		}
		buf[k] = '\0';

		if(strlen(buf) <= 3)
		{
			log_string(buf);
			return str_dup("");
		}

		return str_dup(buf);
	}

	return 0;
}

