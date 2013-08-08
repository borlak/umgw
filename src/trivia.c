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
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "merc.h"

#define TRIVIA_FILE "../area/trivia.txt"
#define MAX_QUESTION_LENGTH 100
#define MAX_ANSWER_LENGTH 40

/* globals */
CHAR_DATA *winner;
char *trivia_char;
char *trivia_question;
char *trivia_answer;
char *trivia_winner;
long trivia[2] = { 2, 0 };
long player_trivia = 1;
long guesses;
long trivia_reward = 25;
long trivia_words;
long trivia_timeleft;

void write_trivia(void);
void read_trivia(void);

typedef struct trivia_info TRIVIA;
typedef struct hint_info HINT;

struct hint_info
{
	HINT *next;
	char *hint;
};

struct trivia_info
{
	TRIVIA *next;
	HINT *hints;
	char *question;
	char *answer;
	char *character;
	char *winner;
	long guesses;
	long time;
	long timeleft;
	long reward;
};

TRIVIA *trivia_list;
HINT *hint_list;

void do_trivia(CHAR_DATA * ch, char *argument)
{
	TRIVIA *s_trivia;
	HINT *hint, *hint_next;
	char buf[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char question[MAX_QUESTION_LENGTH];
	char answer[MAX_ANSWER_LENGTH];
	char reward[MAX_STRING_LENGTH];
	long i = 0;
	long len;
	long num = 0;
	long words = 1;

	argument = one_argument(argument, arg1);
	one_argument(argument, arg2);

	if(arg1[0] == '\0')
	{
		if(trivia[1])
		{
			sprintf(buf, "The question is [%s]\n\r"
				"The answer is %li words and the reward is %li points.\n\r"
				"There are roughly %li seconds left to guess.\n\r",
				trivia_question, trivia_words, trivia_reward, trivia[1]);

			for(hint = hint_list; hint; hint = hint->next)
				sprintf(buf + strlen(buf), "Hint: %s.\n\r", hint->hint);

			send_to_char(buf, ch);
			return;
		}
		send_to_char("There is no trivia going on at this time.\n\r", ch);
		return;
	}

	if(!str_cmp(arg1, "show"))
	{
		TRIVIA *t_start = trivia_list;

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
			for(s_trivia = t_start; s_trivia; s_trivia = s_trivia->next)
			{
				if(strlen(buf) > MAX_STRING_LENGTH - 250)
				{
					t_start = s_trivia;
					break;
				}

				sprintf(buf + strlen(buf), "[Q:%s] [A:%s]\n\r"
					"[Creator:%s] [Winner:%s] [Guesses:%li] [Time:%li] [Time left:%li] [Reward:%li]\n\r",
					s_trivia->question,
					s_trivia->answer,
					s_trivia->character,
					s_trivia->winner,
					s_trivia->guesses, s_trivia->time, s_trivia->timeleft, s_trivia->reward);

				for(hint = s_trivia->hints; hint; hint = hint->next)
					sprintf(buf + strlen(buf), "..Hint: %s.\n\r", hint->hint);

				sprintf(buf + strlen(buf), "\n\r");
			}
		}

		buf[0] = '\0';

		for(s_trivia = trivia_list; s_trivia; s_trivia = s_trivia->next)
		{
			if(strlen(buf) > MAX_STRING_LENGTH - 250)
			{
				sprintf(buf + strlen(buf), "File is too large. Type trivia show %li to view rest.\n\r",
					num + 1);
				break;
			}

			sprintf(buf + strlen(buf), "[Q:%s] [A:%s]\n\r"
				"[Creator:%s] [Winner:%s] [Guesses:%li] [Time:%li] [Time left:%li] [Reward:%li]\n\r",
				s_trivia->question,
				s_trivia->answer,
				s_trivia->character,
				s_trivia->winner, s_trivia->guesses, s_trivia->time, s_trivia->timeleft, s_trivia->reward);

			for(hint = s_trivia->hints; hint; hint = hint->next)
				sprintf(buf + strlen(buf), "..Hint: %s.\n\r", hint->hint);

			sprintf(buf + strlen(buf), "\n\r");
		}
		send_to_char(buf, ch);
		return;
	}

	if(!str_cmp(arg1, "answer") || !str_cmp(arg1, "guess"))
	{
		if(!trivia[1] || argument[0] == '\0')
		{
			send_to_char("42.\n\r", ch);
			return;
		}

		if(!str_cmp(trivia_char, ch->name))
		{
			send_to_char("Something smells fishy.\n\r", ch);
			return;
		}

		guesses++;

		if(str_cmp(argument, trivia_answer))
		{
			send_to_char("Sorry, that's wrong.\n\r", ch);
			return;
		}

		if(trivia_winner)
		{
			send_to_char("That's absolutely RIGHT!\n\r", ch);
			send_to_char("But someone beat you to it...\n\r", ch);
			return;
		}

		/* wow he got it */
		send_to_char("That's absolutely RIGHT!\n\r", ch);
		trivia_winner = str_dup(ch->name);
		trivia_timeleft = trivia[1];
		trivia[1] = 1;	/* set timer to 0 */
		winner = ch;
		return;
	}

	if(!str_cmp(arg1, "question") && (IS_IMMORTAL(ch) || player_trivia))
	{
		if(trivia[1])
		{
			send_to_char("There is already a trivia going on.\n\r", ch);
			return;
		}

		/* players must supply QP amount for rewared */
		if(!IS_IMMORTAL(ch))
		{
			argument = one_argument(argument, reward);

			if(!is_number(reward))
			{
				send_to_char("You must supply a reward amount to do trivia.\n\r", ch);
				return;
			}

			i = atoi(reward);

			if(i < 0 || i > 100)
			{
				send_to_char("Reward must be from 0 to 100.\n\r", ch);
				return;
			}
			if(ch->pcdata->quest < i)
			{
				send_to_char("You don't have that many Quest Points.\n\r", ch);
				return;
			}

			trivia_reward = i;
			ch->pcdata->quest -= i;
			i = 0;
		}

		if(argument[0] == '\0' || (len = strlen(argument)) < 5 || !strstr(argument, "?"))
		{
			send_to_char("You must supply a valid question and answer.\n\r", ch);
			return;
		}

		/* parse question */
		while(i < len && *argument != '?' && i < MAX_QUESTION_LENGTH)
			question[i++] = *argument++;

		if(i < MAX_QUESTION_LENGTH - 1)
			question[i++] = *argument++;	/* add the question mark */

		if(i == MAX_QUESTION_LENGTH)
		{
			sprintf(buf, "The question is limited to %d characters long.\n\r", MAX_QUESTION_LENGTH);
			send_to_char(buf, ch);
			return;
		}
		question[i] = '\0';

		if(i == len)
		{
			send_to_char("You must supply an answer!\n\r", ch);
			return;
		}

		i = 0;

		while(isspace(*argument))	/* get rid of spaces after question mark */
			argument++;

		while(i < len && i < MAX_ANSWER_LENGTH && *argument != '\0')
		{
			if(isspace(*argument))
				words++;
			answer[i++] = *argument++;
		}

		if(i == MAX_ANSWER_LENGTH)
		{
			sprintf(buf, "Answers are limited to %d characters long.\n\r", MAX_ANSWER_LENGTH);
			send_to_char(buf, ch);
			return;
		}

		answer[i] = '\0';

		trivia[1] = trivia[0] * 60;	/* minutes */
		trivia_char = str_dup(ch->name);
		trivia_question = str_dup(question);
		trivia_answer = str_dup(answer);
		trivia_words = words;
		guesses = 0;

		sprintf(buf, "Info-> %s has started a trivia! The question is [%s]\n\r"
			"Info-> The answer consists of %li words.\n\r"
			"Info-> You have %li minutes to guess the answer.\n\r", ch->name, question, trivia_words, trivia[0]);

		if(!IS_IMMORTAL(ch))
		{
			sprintf(buf + strlen(buf), "Info-> The reward for the trivia is %li Quest Points.\n\r",
				trivia_reward);
		}
		do_echo(ch, buf);
		return;
	}

	if(IS_IMMORTAL(ch) || (!str_cmp(trivia_char, ch->name)))
	{
		if(!str_cmp(arg1, "hint"))
		{
			if(trivia[1] <= 0)
			{
				send_to_char("There is no trivia going on.\n\r", ch);
				return;
			}

			if(argument[0] == '\0')
			{
				send_to_char("You must include the hint!\n\r", ch);
				return;
			}

			hint = alloc_mem(sizeof(*hint), "trivia hint");
			hint->hint = str_dup(argument);
			hint->next = hint_list;
			hint_list = hint;

			sprintf(buf, "Info-> %s adds a hint: %s.", ch->name, argument);
			do_echo(ch, buf);
			return;
		}

		if(!str_cmp(arg1, "cancel"))
		{
			trivia[1] = 0;
			free_string(trivia_question);
			free_string(trivia_char);
			free_string(trivia_answer);
			if(trivia_winner)
				free_string(trivia_winner);

			for(hint = hint_list; hint; hint = hint_next)
			{
				hint_next = hint->next;
				free_mem(hint, sizeof(HINT));
			}

			sprintf(buf, "Info-> The trivia has been canceled by %s.\n\r", ch->name);
			do_echo(ch, buf);
			return;
		}
	}

	if(IS_IMMORTAL(ch))
	{
		if(!str_cmp(arg1, "reward"))
		{
			long reward;

			if(argument[0] == '\0' || !is_number(argument))
			{
				send_to_char("Second argument must be a number.\n\r", ch);
				return;
			}
			reward = atoi(argument);

			if(reward < 0 || reward > 100)
			{
				send_to_char("Reward amount ranges from 0 to 100.\n\r", ch);
				return;
			}

			trivia_reward = reward;
			sprintf(buf, "Info-> %s changes the reward amount to %li points for trivia.\n\r",
				ch->name, trivia_reward);
			do_echo(ch, buf);
			return;
		}


		if(!str_cmp(arg1, "time"))
		{
			long time;

			if(argument[0] == '\0' || !is_number(argument))
			{
				send_to_char("You must supply an integer.\n\r", ch);
				return;
			}
			time = atoi(argument);

			if(time < 0 || time > 5)
			{
				send_to_char("I think 5 minutes is plenty of time.\n\r", ch);
				return;
			}

			trivia[0] = time;
			send_to_char("Ok.\n\r", ch);
			return;
		}

		if(!str_cmp(arg1, "player"))
		{
			if(argument[0] == '\0')
			{
				send_to_char("Must be on or off.\n\r", ch);
				return;
			}

			if(!str_cmp(argument, "on"))
			{
				player_trivia = 1;
				do_info(char_list, "Players may start trivia now!\n\r");
				send_to_char("Players may start trivia now.\n\r", ch);
				return;
			}

			if(!str_cmp(argument, "off"))
			{
				player_trivia = 0;
				do_info(char_list, "Players can no longer start trivia.\n\r");
				send_to_char("Players may not start trivia now.\n\r", ch);
				return;
			}

			send_to_char("Syntax: trivia player <on/off>\n\r", ch);
			return;
		}
	}

	send_to_char("Syntax: trivia\n\r", ch);
	send_to_char("or      trivia show\n\r", ch);
	send_to_char("or      trivia guess <answer>\n\r", ch);
	send_to_char("or      trivia question <question + ?> <answer>\n\r", ch);

	if(IS_IMMORTAL(ch))
	{
		send_to_char("or      trivia hint <hint>\n\r", ch);
		send_to_char("or      trivia reward <reward>\n\r", ch);
		send_to_char("or      trivia player <on/off>\n\r", ch);
		send_to_char("or      trivia cancel\n\r", ch);
	}

	send_to_char("You must supply the question mark for the question.\n\r\n\r", ch);
	send_to_char("Do not include any punctuation for the answer, it must be a direct match.\n\r", ch);
	return;
}


void trivia_update(void)
{
	CHAR_DATA *ch = 0;
	char buf[MAX_STRING_LENGTH];
	HINT *hint;

	if(!trivia[1])
		return;

	trivia[1] -= 5;

	if(trivia[1] <= 0)
	{
		if(!trivia_winner)
		{
			sprintf(buf, "Info-> The trivia has ended with no one guessing the correct answer.\n\r");
		}
		else
		{
			sprintf(buf, "Info-> %s has guessed the correct trivia answer and is awarded %li quest points!\n\r",
				trivia_winner, trivia_reward);

			if(winner && !IS_NPC(winner))
				winner->pcdata->quest += trivia_reward;
		}
		do_echo(char_list, buf);
		if(!trivia_winner || IS_IMMORTAL(winner))
		{
			if((ch = get_char_world(char_list, trivia_char)) != 0)
			{
				sprintf(buf, "You get your %li quest points back.\n\r", trivia_reward);
				send_to_char(buf, ch);
				ch->pcdata->quest += trivia_reward;
				buf[0] = '\0';
			}
		}

		sprintf(buf + strlen(buf), "Info-> The question was [%s]\n\r"
			"Info-> The answer was [%s]\n\r"
			"Info-> Creator of the trivia was [%s]\n\r"
			"Info-> You had [%li] minutes, there is %li seconds left, and there were [%li] guesses.\n\r",
			trivia_question, trivia_answer, trivia_char, trivia[0], trivia_timeleft, guesses);

		for(hint = hint_list; hint; hint = hint->next)
			sprintf(buf + strlen(buf), "Info-> Hint: %s.\n\r", hint->hint);

		if(char_list)
			do_echo(char_list, buf);

		write_trivia();
		trivia[1] = 0;
		winner = 0;

		return;
	}
}


void write_trivia(void)
{
	FILE *fp;
	HINT *hint;
	TRIVIA *triv;

	fclose(fpReserve);

	if((fp = fopen(TRIVIA_FILE, "w+")) == 0)
	{
		bug("write_trivia: unable to open file for writing!", 0);
		return;
	}

	triv = alloc_perm(sizeof(*triv));
	triv->question = trivia_question;
	triv->answer = trivia_answer;
	triv->character = trivia_char;
	triv->reward = trivia_reward;
	triv->winner = trivia_winner ? trivia_winner : str_dup("None");
	triv->guesses = guesses;
	triv->time = trivia[0];
	triv->timeleft = trivia_timeleft;
	trivia_winner = 0;
	triv->next = trivia_list;
	trivia_list = triv;
	triv->hints = hint_list;
	hint_list = 0;

	for(triv = trivia_list; triv; triv = triv->next)
	{
		fprintf(fp, "trivia\n%s~ %s~ %s~ %s~ %li %li %li %li\n",
			triv->question,
			triv->answer,
			triv->character, triv->winner, triv->guesses, triv->time, triv->timeleft, triv->reward);

		for(hint = hint_list; hint; hint = hint->next)
			fprintf(fp, "hint\n%s~\n", hint->hint);
	}

	fprintf(fp, "\nend\n\n");
	fclose(fp);
	fpReserve = fopen(NULL_FILE, "r");

	return;
}

void read_trivia(void)
{
	FILE *fp;
	TRIVIA *strivia = 0;
	HINT *hint;
	char *word;

	if((fp = fopen(TRIVIA_FILE, "r")) == 0)
	{
		bug("read_trivia: unable to open file ro reading!", 0);
		return;
	}

	while(1)
	{
		word = fread_word(fp);

		if(!str_cmp(word, "end"))
			break;

		if(!str_cmp(word, "trivia"))
		{
			strivia = alloc_perm(sizeof(*strivia));
			strivia->question = fread_string(fp);
			strivia->answer = fread_string(fp);
			strivia->character = fread_string(fp);
			strivia->winner = fread_string(fp);
			strivia->guesses = fread_number(fp);
			strivia->time = fread_number(fp);
			strivia->timeleft = fread_number(fp);
			strivia->reward = fread_number(fp);
			strivia->next = trivia_list;
			trivia_list = strivia;
			continue;
		}

		if(!str_cmp(word, "hint"))
		{
			if(!strivia)
			{
				bug("read_trivia: hint read before trivia laoded!", 0);
				exit(1);
			}

			hint = alloc_perm(sizeof(*hint));
			hint->hint = fread_string(fp);
			hint->next = strivia->hints;
			strivia->hints = hint;
			continue;
		}

		bug("read_trivia: wrong word!", 0);
		break;
	}

	return;
}

// THIS IS THE END OF THE FILE THANKS
