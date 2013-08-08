/*
tournament starts
people join
tournament determines who fights first,
 by the two people with the lowest rating
tournament starts with first fight
* made by borlak
*/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


//// # DEFINES //////
// vnums of rooms
#define WAIT1	10
#define WAIT2	11
#define START1	12
#define START2	14


///////// GLOBAL SHIZ ////////////
typedef struct tournees TOURNEE;

struct tournees
{
	TOURNEE *next;
	CHAR_DATA *ch;
	char killed_by[25];
	long place;
	long dead;
	long damage;
	long kills;
};


TOURNEE *gTournees = 0;
CHAR_DATA *winner;
CHAR_DATA *fighter1;
CHAR_DATA *fighter2;
long tournament_on = 0;
long timer = 0;
long members = 0;
long cdown = 0;
long mobtourn = 0;

////////// GLOBAL FUNCS ////////////
void tournament_begin(void);
void tournament_end(void);
void get_next_fight(void);
void award_bonus(TOURNEE * t);

////////// MAIN FUNCTINS ///////////
TOURNEE *new_tournee(void)
{
	TOURNEE *t;

	t = alloc_mem(sizeof(TOURNEE), "tournee");
	t->ch = 0;
	t->killed_by[0] = '\0';
	t->place = 0;
	t->dead = 0;
	t->damage = 0;
	t->kills = 0;

	t->next = gTournees;
	gTournees = t;
	members++;
	return t;
}

bool tourn_on(void)
{
	if(!tournament_on)
		return FALSE;
	return TRUE;
}

bool tourn_started(void)
{
	if(!tournament_on)
		return FALSE;
	if(timer > 0)
		return TRUE;
	return FALSE;
}

void tournament_update(void)
{
	char buf[MAX_STRING_LENGTH];

	if(!tournament_on)
		return;

	if(cdown && --cdown == 0)
	{
		if(timer)
		{
			if(!fighter1 || !fighter2)
			{
				do_info(char_list, "One of the fighters has cowardly ran away! HAHAHA");
				get_next_fight();
				return;
			}

			sprintf(buf, "%s and %s are transported to the Tournament Arena!", fighter1->name, fighter2->name);
			do_info(char_list, buf);

			send_to_char("You are transported to the Tournament Arena, now die.\n\r", fighter1);
			char_from_room(fighter1);
			char_to_room(fighter1, get_room_index(START1));
			send_to_char("You are transported to the Tournament Arena, now die.\n\r", fighter2);
			char_from_room(fighter2);
			char_to_room(fighter2, get_room_index(START2));
			return;
		}
		else
		{
			timer = 15;
			tournament_begin();
		}
		return;
	}
	else if(cdown)
	{
		sprintf(buf, "There %s %li minute%s left until the Tournament begins.",
			cdown == 1 ? "is" : "are", cdown, cdown == 1 ? "" : "s");
		do_info(char_list, buf);
	}

	if(!cdown && --timer == 0)
	{
		do_info(char_list, "The tournament has gone on for too long, and has ended prematurely!");
		tournament_end();
	}
}

// updates global "members" long to see how many people really are in
// also gets rid of empty tournee things
void tournament_check(CHAR_DATA * ch)
{
	char buf[MAX_STRING_LENGTH];
	TOURNEE *t, *t_next, *t_prev;

	if(!gTournees)
		return;

	while(gTournees->ch == ch)
	{
		t = gTournees->next;
		free_mem(gTournees, sizeof(TOURNEE));
		gTournees = t;
		members--;
	}

	for(t = t_prev = gTournees; t; t = t_next)
	{
		t_next = t->next;

		if(t->ch == ch)
		{
			t_prev->next = t->next;
			free_mem(t, sizeof(TOURNEE));
			members--;
			continue;
		}
		t_prev = t;
	}

	if((ch && ch == fighter1) || (ch && ch == fighter2))
	{
		sprintf(buf, "%s wimped out and leaves the tournament...wuss!", IS_NPC(ch) ? ch->short_descr : ch->name);
		do_info(char_list, buf);
		char_from_room(ch);
		char_to_room(ch, get_room_index(ROOM_VNUM_TEMPLE));
		get_next_fight();
	}
}

// gets next fight and also determines winner
void get_next_fight(void)
{
	char buf[MAX_STRING_LENGTH];
	TOURNEE *low1, *low2;
	TOURNEE *t;
	long i;

	low1 = low2 = 0;

	for(t = gTournees; t; t = t->next)
	{
		if(!low1 && !t->dead && t->ch && (IS_NPC(t->ch) || t->ch->desc))
		{
			low1 = t;
			continue;
		}
		if(!low2 && !t->dead && t->ch && (IS_NPC(t->ch) || t->ch->desc))
		{
			low2 = t;
			continue;
		}
	}

	if(!low1 || !low2)
	{
		if(!winner)
		{
			do_info(char_list, "There is no tournament winner, snort.");
			tournament_end();
			return;
		}

		sprintf(buf, "{K%s is the winner of the tournament!{x", winner->name);
		do_info(char_list, buf);

		for(t = gTournees; t; t = t->next)
		{
			if(t->ch == winner)
			{
				t->place = 1;
				award_bonus(t);
				break;
			}
		}

		char_from_room(winner);
		char_to_room(winner, get_room_index(ROOM_VNUM_TEMPLE));

		tournament_end();
		return;
	}

	for(i = 0; i < 2; i++)
	{
		for(t = gTournees; t; t = t->next)
		{
			if(!t->ch || !t->ch->desc || t->dead)
				continue;

			// just based on HP instead of rating now
/*			if( low1 != t && t != low2 && low1->ch->max_hit > t->ch->max_hit )
				low1 = t;
			else if( low2 != t && t != low1 && low2->ch->max_hit > t->ch->max_hit )
				low2 = t;
*/
			if(!mobtourn)
			{
				if(low1 != t && t != low2 && player_rating(low1->ch) > player_rating(t->ch))
					low1 = t;
				else if(low2 != t && t != low1 && player_rating(low2->ch) > player_rating(t->ch))
					low2 = t;
			}
			else
			{
				if(low1 != t && t != low2 && low1->ch->level > t->ch->level)
					low1 = t;
				else if(low2 != t && t != low1 && low2->ch->level > t->ch->level)
					low2 = t;
			}
		}
	}

	sprintf(buf, "%s and %s are next in line to battle it out, you have %s to prepare.",
		mobtourn ? low1->ch->short_descr : low1->ch->name,
		mobtourn ? low2->ch->short_descr : low2->ch->name, mobtourn ? "no time" : "one minute");
	do_info(char_list, buf);

	if(mobtourn)
	{
		char_from_room(low1->ch);
		char_to_room(low1->ch, get_room_index(START1));
		do_visible(low1->ch, "");
		char_from_room(low2->ch);
		char_to_room(low2->ch, get_room_index(START1));
		do_visible(low2->ch, "");
		do_kill(low1->ch, low2->ch->name);
		return;
	}

	if(low1->ch->level != 3)
	{
		send_to_char("You are made Avatar for this match.\n\r", low1->ch);
		low1->ch->level = 3;
		low1->ch->trust = 3;
	}
	send_to_char("You are fully restored and transported to the waiting room, get out and DIE!\n\r", low1->ch);
	do_restore(low1->ch, low1->ch->name);
	char_from_room(low1->ch);
	char_to_room(low1->ch, get_room_index(WAIT1));
	fighter1 = low1->ch;

	if(low2->ch->level != 3)
	{
		send_to_char("You are made Avatar for this match.\n\r", low2->ch);
		low2->ch->level = 3;
		low2->ch->trust = 3;
	}
	send_to_char("You are fully restored and transported to the waiting room, get out and DIE!\n\r", low2->ch);
	do_restore(low2->ch, low2->ch->name);
	char_from_room(low2->ch);
	char_to_room(low2->ch, get_room_index(WAIT2));
	fighter2 = low2->ch;

	cdown = 1;
	timer++;
}

void tournament_begin(void)
{
	// make sure everyone is still online
	tournament_check(0);

	if(!gTournees || members == 2)
	{
		do_info(char_list, "Not enough people have joined the tournament...wimps.");
		tournament_end();
		return;
	}
	do_info(char_list, "The Tournament has BEGUN!  Use TCAP to decapitate your opponent.");

	get_next_fight();
}


void tournament_end(void)
{
	TOURNEE *t;
	char buf[MAX_STRING_LENGTH];
	long dudes = 0;

//      long lowest = 1;

	sprintf(buf, "The Tournament is OVER.\n\r\n\r");
	sprintf(buf + strlen(buf), "The Brave Ones\n\r");
	sprintf(buf + strlen(buf), "--------------\n\r");

	for(t = gTournees; t; t = t->next)
	{
		if(!t->ch || (!IS_NPC(t->ch) && !t->ch->desc))
			continue;

		dudes++;
		sprintf(buf + strlen(buf), "%-24s Placed %-2li  Damage: %-6li  Killed by: %s\n\r",
			t->ch->name, t->place, t->damage, t->killed_by[0] == '\0' ? "Nobody" : t->killed_by);
	}

	// just in case :P
	buf[MAX_STRING_LENGTH - 1] = '\0';
	do_info(char_list, buf);

	tournament_on = 0;
	timer = 0;
	members = 0;
	cdown = 0;

	// free the memory
	while(gTournees)
	{
		t = gTournees->next;
		free_mem(gTournees, sizeof(TOURNEE));
		gTournees = t;
	}
	return;
}


void award_bonus(TOURNEE * t)
{
	char buf[MAX_STRING_LENGTH];
	long standard_bonus = 294231;
	long bonus;
	OBJ_DATA *obj;

	if(!t || !t->ch || !t->ch->desc)
		return;

	// basically, ((damage*kills) + 500000/your_place) * kills...getting 2 kills is very good
	/*(members+1-t->place))) */

	bonus = ((t->damage * UMAX(t->kills, 1)) + (standard_bonus / t->place)) * UMAX(t->kills, 1);
	t->ch->exp += bonus;

	sprintf(buf, "{B%s is awarded %li experience for participating.{X", t->ch->name, bonus);
	do_info(char_list, buf);
	for( bonus = 1; bonus <= t->kills; bonus++ )
	{
		obj = random_token(buf);
		obj->timer = 1;
		obj_to_char(obj,t->ch);
		sprintf(buf,"{B%s gets {W%s{X{B for getting %li kill%s!{X", t->ch->name, obj->short_descr,
			bonus, bonus==1?"":"s");
		do_info(char_list, buf);
	}
}


// is dude in tournament? say you cant do that
bool in_tournament(CHAR_DATA * ch, CHAR_DATA * victim)
{
	if(!tournament_on)
		return FALSE;

	if(!ch || !victim || !ch->in_room || !victim->in_room)
		return TRUE;

	if(victim->in_room->vnum > MIN_TOURNAMENT_VNUM && victim->in_room->vnum < MAX_TOURNAMENT_VNUM)
	{
		send_to_char("You cant do that!  It's a TOURNAMENT!\n\r", ch);
		return TRUE;
	}
	return FALSE;
}


void tournament_kill(CHAR_DATA * ch, CHAR_DATA * victim)
{
	char buf[MAX_STRING_LENGTH];
	TOURNEE *t, *win = 0, *dead = 0;
	long place = 0;
	long cdamage, vdamage;

	cdamage = find_assist_damage(victim, ch);
	vdamage = find_assist_damage(ch, victim);

	tournament_check(0);
	winner = ch;
	fighter1 = 0;
	fighter2 = 0;
	place = members;

	for(t = gTournees; t; t = t->next)
	{
		if(t->ch == victim)
		{
			dead = t;
			t->dead = 1;
		}

		if(t->ch == ch)
			win = t;
		if(t->dead)
			place--;
	}
	if(dead)
	{
		dead->place = ++place;
		strcat(dead->killed_by, win->ch->name);
	}

	if(win && dead)
	{
		win->kills++;
		win->damage += cdamage;
		dead->damage += vdamage;
	}
	sprintf(buf, "{B%s[{b%li damage{B] defeats %s[{b%li damage{B] in the Tournament Arena!{x",
		IS_NPC(ch) ? ch->short_descr : ch->name, cdamage,
		IS_NPC(victim) ? victim->short_descr : victim->name, vdamage);
	do_info(ch, buf);

	if(!mobtourn)
		award_bonus(dead);

	if(win && dead)
		get_next_fight();
}


///////// DA DO_ FUNCTIONS AND PLAYER RELATED SHIZ ////////////

void do_tournament(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	TOURNEE *t;

	if(!argument || argument[0] == '\0')
	{
		if(!tournament_on)
		{
			send_to_char("There is no tournament going on!\n\r", ch);
			return;
		}

		sprintf(buf, "People signed up so far:\n\r");
		for(t = gTournees; t; t = t->next)
		{
			if(!t->ch || (!IS_NPC(t->ch) && !t->ch->desc))
				continue;

			sprintf(buf + strlen(buf), "%15s %-15s  Place %-2li  Damage: %li\n\r",
				fighter1 == t->ch ? "({RFIGHTING{X)" : fighter2 == t->ch ? "({RFIGHTING{X)"
				: t->dead ? "({DDEAD{X)" : "{x{x", t->ch->name, t->place, t->damage);
		}
		send_to_char(buf, ch);
		return;
	}

	if(!str_cmp(argument, "join"))
	{
		if(!tournament_on)
		{
			send_to_char("No tournament is going on at the moment.\n\r", ch);
			return;
		}

		if(timer > 0)
		{
			send_to_char("The tournament has already started, you are a little late.\n\r", ch);
			return;
		}

		for(t = gTournees; t; t = t->next)
		{
			if(t->ch == ch)
			{
				send_to_char("You are already in the tournament!\n\r", ch);
				return;
			}
		}
		if(ch->exp < 50000 && !mobtourn && !IS_NPC(ch))
		{
			send_to_char("You do not have the 50000 experience required to join.\n\r", ch);
			return;
		}

		if(mobtourn && !IS_NPC(ch))
		{
			send_to_char("This is for mobiles only!\n\r", ch);
			return;
		}

		t = new_tournee();
		t->ch = ch;
		send_to_char("It costs you 50,000 exp to join the tournament.\n\r", ch);
		ch->exp -= 50000;
		sprintf(buf, "{K%s has joined the tournament.{X", mobtourn ? ch->short_descr : ch->name);
		do_info(ch, buf);
		return;
	}

	if(!str_cmp(argument, "start"))
	{
		if(!IS_IMMORTAL(ch))
		{
			send_to_char("You aren't an immortal!\n\r", ch);
			return;
		}

		if(tournament_on)
		{
			send_to_char("There is one already going on.\n\r", ch);
			return;
		}

		tournament_on = 1;
		cdown = 2;
		timer = 0;
		sprintf(buf, "{k%s has started a TOURNAMENT, JOIN or DIE.{x  Tournament will begin in TWO minutes.",
			ch->name);
		do_info(ch, buf);
		return;
	}


	if(!str_cmp(argument, "startmob"))
	{
		CHAR_DATA *vch;
		long total;
		long i;

		if(!IS_IMMORTAL(ch))
		{
			send_to_char("You aren't an immortal!\n\r", ch);
			return;
		}

		if(tournament_on)
		{
			send_to_char("There is one already going on.\n\r", ch);
			return;
		}

		tournament_on = 1;
		mobtourn = 1;
		cdown = 1;
		timer = 0;
		i = 0;
		total = 0;

		sprintf(buf, "{k%s has started a MOB TOURNAMENT.  Tournament will begin in ONE minute.", ch->name);
		do_info(ch, buf);
		for(vch = char_list; vch; vch = vch->next)
		{
			if(!IS_NPC(vch))
				continue;
			total++;

		}
		for(vch = char_list; vch; vch = vch->next)
		{
			if(!IS_NPC(vch))
				continue;

			if(number_range(1, (total / 200)) == 2)
			{
				do_tournament(vch, "join");
				char_from_room(vch);
				char_to_room(vch, get_room_index(1));
				i++;
			}
		}
		sprintf(buf, "{kThere will be %li mobs participating!{x", i);
		do_info(ch, buf);
		return;
	}
	if(!str_cmp(argument, "end"))
	{
		if(!IS_IMMORTAL(ch))
		{
			send_to_char("You aren't an immortal!\n\r", ch);
			return;
		}

		if(!tournament_on)
		{
			send_to_char("End what?\n\r", ch);
			return;
		}
		tournament_end();
		return;
	}

	send_to_char("Syntax:   tournament\n\r", ch);
	send_to_char("          tournament join\n\r", ch);
	send_to_char("Imm only:\n\r", ch);
	send_to_char("		tournament start\n\r", ch);
	send_to_char("		tournament end\n\r", ch);
	return;
}


void do_tcap(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	long percent = 0;
	long total = 0;
	long damage = 0;

	one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(arg[0] == '\0')
	{
		send_to_char("Tournament cap whom?\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(ch == victim)
	{
		send_to_char("That might be a bit tricky...\n\r", ch);
		return;
	}

	if(IS_NPC(victim))
	{
		send_to_char("You can only decapitate other players.\n\r", ch);
		return;
	}

	if(victim->level != 3)
	{
		send_to_char("You can only decapitate other avatars.\n\r", ch);
		return;
	}

	if(ch->in_room->vnum > 15 || ch->in_room->vnum < 10)
	{
		send_to_char("You have to be in the tournament to use this dumbass.\n\r", ch);
		return;
	}

	if((victim->position > 1 || victim->hit > 0))
	{
		send_to_char("You can only do this to mortally wounded players.\n\r", ch);
		return;
	}

	if(fighter1 != ch && fighter2 != ch)
	{
		send_to_char("You are not a fighter in this tournament! GET OUT!\n\r", ch);
		return;
	}

	total = UMAX(1, find_total_assist_damage(victim));
//    mobdamage       = UMAX(1,find_mob_assist_damage( victim ));
	damage = UMAX(1, find_assist_damage(victim, ch));
	percent = (long) (((float) damage / (float) total) * 100);

	if(is_safe(ch, victim))
		return;

	act("You rip off $N's head!", ch, 0, victim, TO_CHAR);
	send_to_char("Your head is ripped off from its shoulders!\n\r", victim);
	act("$n rips off $N's head!", ch, 0, victim, TO_NOTVICT);
	act("$n's head is ripped from $s shoulders!", victim, 0, 0, TO_ROOM);

	ch->tlw += 1;
	victim->tll += 1;
	if(IS_SET(victim->special, SPC_WOLFMAN))
		do_unwerewolf(victim, "");

	// in case this was a cheapass cap, add each to eachothers assist group
	update_assist(ch, victim, 1);
	update_assist(victim, ch, 1);

// tournament stuff
	tournament_kill(ch, victim);

	if(IS_CLASS(victim, CLASS_VAMPIRE))
		do_mortalvamp(victim, "");

	char_from_room(victim);
	char_to_room(victim, get_room_index(ROOM_VNUM_TEMPLE));
	victim->fight_timer = 0;
	behead(victim);

}


void do_survey(CHAR_DATA * ch, char *argument)
{
	if(IS_NPC(ch))
		return;

	if(!argument || argument[0] == '\0')
	{
		send_to_char("Syntax: survey <on/off>\n\r", ch);
		return;
	}

	if(!str_cmp(argument, "on"))
	{
		if(ch->pcdata->surveying)
		{
			send_to_char("It's already on.\n\r", ch);
			return;
		}
		send_to_char("You are now surveying fights.\n\r", ch);
		ch->pcdata->surveying = TRUE;
		return;
	}

	if(!str_cmp(argument, "off"))
	{
		if(!ch->pcdata->surveying)
		{
			send_to_char("You aren't surveying!\n\r", ch);
			return;
		}
		ch->pcdata->surveying = FALSE;
		return;
	}

	do_survey(ch, "");
	return;
}

void battle_log(CHAR_DATA * ch, CHAR_DATA * vch, char *str)
{
	DESCRIPTOR_DATA *d = 0;
	char buf[MAX_STRING_LENGTH];

	if(!ch || !vch || !str || str[0] == '\0' || ch == vch
	   || !vch->in_room || vch->in_room->vnum < MIN_TOURNAMENT_VNUM || vch->in_room->vnum > MAX_TOURNAMENT_VNUM)
		return;

	sprintf(buf, "{R[%s:%-3lih]{Xvs{R[%s:%-3lih]{X: {y%s{X\n\r",
		IS_NPC(ch) ? ch->short_descr : ch->name, ch->hit,
		IS_NPC(vch) ? vch->short_descr : vch->name, vch->hit, str);

	for(d = descriptor_list; d; d = d->next)
	{
		if(d->connected != CON_PLAYING
		   || !d->character
		   || IS_NPC(d->character)
		   || d->character->pcdata->surveying == FALSE || fighter1 == d->character || fighter2 == d->character)
			continue;

		send_to_char(buf, d->character);
	}

	return;
}

// THIS IS THE END OF THE FILE THANKS
