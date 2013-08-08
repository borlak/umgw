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
#include <string.h>
#include "merc.h"

#define VOTE_FILE "../area/vote.txt"

/* globals */
char *vote_topic;
long vote[3];
long player_vote = 0;

void write_votes(char *topic, float yes, float no, long total);
void read_votes(void);

typedef struct vote_info VOTE;

struct vote_info
{
	VOTE *next;

	char *topic;
	float yes;
	float no;
	long total;
};

VOTE *vote_list;



void do_vote(CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	DESCRIPTOR_DATA *d;
	VOTE *s_vote;
	char buf[MAX_STRING_LENGTH];
	long num = 0;
	long i = 0;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if(arg1[0] == '\0')
	{
		if(vote[0])
		{
			sprintf(buf, "The topic of the vote is [%s]\n\r"
				"There are roughly %li seconds left to vote.\n\r", vote_topic, vote[0]);
			send_to_char(buf, ch);
			return;
		}
		send_to_char("There is no vote going on at this time.\n\r", ch);
		return;
	}

	if(!str_cmp(arg1, "show"))
	{
		VOTE *v_start = vote_list;

		buf[0] = '\0';

		if(arg2[0] == '\0')
		{
			num = 0;
		}
		else
		{
			num = atoi(arg2);
		}

		for(i = 0; i < num; i++)
		{
			buf[0] = '\0';
			for(s_vote = v_start; s_vote; s_vote = s_vote->next)
			{
				if(strlen(buf) > MAX_STRING_LENGTH - 250)
				{
					v_start = s_vote;
					break;
				}

				sprintf(buf + strlen(buf), "[{W%s{x] Yes[{Y%.2f%%{x] No[{R%.2f%%{x] Total Votes[%li]\n\r",
					s_vote->topic, s_vote->yes, s_vote->no, (long) s_vote->total);
			}
		}

		buf[0] = '\0';
		for(s_vote = vote_list; s_vote; s_vote = s_vote->next)
		{
			if(strlen(buf) > MAX_STRING_LENGTH - 250)
			{
				sprintf(buf + strlen(buf),
					"There is too much information. Type vote show %li to see more.\n\r", num + 1);
				break;
			}
			sprintf(buf + strlen(buf), "[{W%s{x] Yes[{Y%.2f%%{x] No[{R%.2f%%{x] Total Votes[%li]\n\r",
				s_vote->topic, s_vote->yes, s_vote->no, (long) s_vote->total);
		}
		send_to_char(buf, ch);
		return;
	}

	if(!str_cmp(arg1, "topic") && (IS_IMMORTAL(ch) || player_vote))
	{
		if(vote[0])
		{
			send_to_char("There is all ready a vote going on.\n\r", ch);
			return;
		}

		if(!is_number(arg2))
		{
			send_to_char("Second argument must be the amount of time in seconds, increments of 5.\n\r", ch);
			return;
		}

		if(argument[0] == '\0' || strlen(argument) < 5)
		{
			send_to_char("You must supply a valid topic.\n\r", ch);
			return;
		}

		if(atoi(arg2) % 5 != 0)
		{
			send_to_char("Timer must be in increments of 5.\n\r", ch);
			return;
		}

		if(atoi(arg2) > 300)
		{
			send_to_char("Dont you think 5 minutes is long enought to vote?\n\r", ch);
			return;
		}

		sprintf(buf, "Info-> %s has started a vote, topic is [%s]\n\r"
			"Info-> You have %d seconds to vote.", ch->name, argument, atoi(arg2));

		do_echo(ch, buf);

		vote[0] = atoi(arg2);
		vote[1] = 0;
		vote[2] = 0;
		vote[3] = 0;
		vote_topic = str_dup(argument);

		for(d = descriptor_list; d; d = d->next)
		{
			if(d->connected != CON_PLAYING || !d->character)
				continue;

			d->character->pcdata->voted = 0;
		}

		return;
	}

	if(IS_IMMORTAL(ch))
	{
		if(!str_cmp(arg1, "cancel"))
		{
			vote[0] = 0;
			vote[1] = 0;
			vote[2] = 0;
			free_string(vote_topic);

			sprintf(buf, "Info-> The vote has been canceled by %s.\n\r", ch->name);
			do_echo(ch, buf);
			return;
		}

		if(!str_cmp(arg1, "player"))
		{
			if(!str_cmp(arg2, "on"))
			{
				player_vote = 1;
				send_to_char("Players may start a vote topic now.\n\r", ch);
				return;
			}

			if(!str_cmp(arg2, "off"))
			{
				player_vote = 0;
				send_to_char("Players may not start a vote topic now.\n\r", ch);
				return;
			}

			send_to_char("Syntax: vote player <on/off>\n\r", ch);
			return;
		}

	}

	if(!vote[0])
	{
		send_to_char("There is no vote going on.\n\r", ch);
		return;
	}

	if(ch->pcdata->voted)
	{
		send_to_char("You can only vote once.\n\r", ch);
		return;
	}

	if(!str_cmp(arg1, "yes"))
	{
		vote[1]++;
		ch->pcdata->voted = 1;
		send_to_char("You vote yes.\n\r", ch);
		return;
	}

	if(!str_cmp(arg1, "no"))
	{
		vote[2]++;
		ch->pcdata->voted = 1;
		send_to_char("You vote no.\n\r", ch);
		return;
	}
	send_to_char("Vote yes or no dumbass.\n\r", ch);
	return;
}


void vote_update(void)
{
	char buf[MAX_STRING_LENGTH];
	float total, x1, x2;

	if(!vote[0])
		return;

	vote[0] -= 5;

	if(vote[0] <= 0)
	{
		total = (float) (vote[1] + vote[2]);
		x1 = vote[1] == 0 ? 0 : (float) vote[1] / total * 100;
		x2 = vote[2] == 0 ? 0 : (float) vote[2] / total * 100;

		sprintf(buf, "Info-> Vote has ended for topic [%s]\n\r"
			"Info-> Vote outcome is Yes[%.2f%%] No[%.2f%%] Total Votes[%li]\n\r",
			vote_topic, x1, x2, (long) total);

		if(char_list)
			do_echo(char_list, buf);

		write_votes(vote_topic, x1, x2, (long) total);
		vote[0] = 0;

		return;
	}
}


void write_votes(char *topic, float yes, float no, long total)
{
	FILE *fp;
	VOTE *svote;

	fclose(fpReserve);

	if((fp = fopen(VOTE_FILE, "w+")) == 0)
	{
		bug("write_vote: unable to open file for writing!", 0);
		return;
	}

	svote = alloc_perm(sizeof(*svote));
	svote->topic = topic;
	svote->yes = yes;
	svote->no = no;
	svote->total = total;
	svote->next = vote_list;
	vote_list = svote;

	for(svote = vote_list; svote; svote = svote->next)
		fprintf(fp, "vote\n%s~ %li %li %li\n", svote->topic, (long) svote->yes, (long) svote->no, (long) svote->total);

	fprintf(fp, "\nend\n\n");
	fclose(fp);
	fpReserve = fopen(NULL_FILE, "r");

	return;
}

void read_votes(void)
{
	FILE *fp;
	VOTE *svote;
	char *word;

	if((fp = fopen(VOTE_FILE, "r")) == 0)
	{
		bug("read_votes: unable to open file ro reading!", 0);
		return;
	}

	while(1)
	{
		word = fread_word(fp);

		if(!str_cmp(word, "end"))
			break;

		if(!str_cmp(word, "vote"))
		{
			svote = alloc_perm(sizeof(*svote));
			svote->topic = fread_string(fp);
			svote->yes = (float) fread_number(fp);
			svote->no = (float) fread_number(fp);
			svote->total = fread_number(fp);
			svote->next = vote_list;
			vote_list = svote;
			continue;
		}

		bug("read_votes: wrong word!", 0);
		break;
	}

	return;
}

// THIS IS THE END OF THE FILE THANKS
