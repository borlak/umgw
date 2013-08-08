
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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

#ifndef WIN32
#include <unistd.h>
#endif


/*
 * Local functions.
 */
bool is_note_to args((CHAR_DATA * ch, NOTE_DATA * pnote));
void note_attach args((CHAR_DATA * ch));
void note_remove args((NOTE_DATA * pnote));
void talk_channel args((CHAR_DATA * ch, char *argument, long channel, const char *verb));
bool is_in args((char *, char *));
bool all_in args((char *, char *));
char *oldelang args((CHAR_DATA * ch, char *argument));
char *badlang args((CHAR_DATA * ch, char *argument));
char *hacklang args((CHAR_DATA * ch, char *argument));
char *darktongue args((CHAR_DATA * ch, char *argument));
char *drunktalk args((CHAR_DATA * ch, char *argument));
char *socialc args((CHAR_DATA * ch, char *argument, char *you, char *them));
char *socialv args((CHAR_DATA * ch, char *argument, char *you, char *them));
char *socialn args((CHAR_DATA * ch, char *argument, char *you, char *them));

bool is_note_to(CHAR_DATA * ch, NOTE_DATA * pnote)
{
	if(!str_cmp(ch->name, pnote->sender))
		return TRUE;

	if(is_name("all", pnote->to_list))
		return TRUE;

	if(ch->level == MAX_LEVEL)
		return TRUE;

	if(IS_IMMORTAL(ch) && is_name("imm", pnote->to_list))
		return TRUE;

	if(IS_IMMORTAL(ch) && is_name("immortal", pnote->to_list))
		return TRUE;

	if(is_name(ch->name, pnote->to_list))
		return TRUE;

/*    if ( !IS_NPC(ch) && ch->pcdata->kingdom
    &&   get_kingdom(ch->pcdata->kingdom)->id != 0
    &&   !str_cmp(get_kingdom(ch->pcdata->kingdom)->noansiname, pnote->to_list) )
	return TRUE;
*/
	return FALSE;
}


const char *insults[] = {
	"I just had a vision, %s was killed by my hands...",
	"Time to die, Mr. %s.",
	"I don't like your attitude, %s, and come to think of it, I don't like YOU!",
	"I want a %s meyer weiner!",
	"Hey %s, do your panties glow in the dark?",
	"Do you always pee into the wind, %s?",
	"For 1000 qp I'll let you cap me, %s",
	"I don't need to wait for a tournament to PK you, %s!",
	"You think that's funny, %s?  Well you wont think this is funny...",
	"Come and get me, %s, if you dare.", /* 10 */
	"I'm gonna rip %s's heart out with a spooooon.",
	"There are millions of ways to die, and %s just had to choose me?  So be it.",
	"%s, mess with the best, die like the rest.",
	"Ash to ash, dust to dust, soon %s will be a True Beliver.",
	"%s$!*@#&$@# Shazbot!",
	"%s, when you get to hell, tell 'em I sent you...you'll get a group discount",
	"%s, this is gonna hurt you more than it hurts me...Seriously.",
	"%s, your life means nothing to me.",
	"%s, may a thousand camels step on your bad corns.",
	"%s, your admission to this mud was a terrible mistake, because you are about to FAIL!", /* 20 */
	"%s, your mother was a hamster, and father smelt of elderberries!",
	"I fart in %s's general direction.",
	"I see you're still alive, %s.  How did I overlook that?",
	"%s, I hope you were good at track, 'cuz you better start RUNNIN!",
	"www.ikilled%s.com",
	"Touch Me, Feel Me, Kiss Me, Kill %s!",
	"Don't logout yet, %s.  I'm just getting started with you!",
	"Yesterday I was scared of you, %s.   That was yesterday.",
	"I insult you, %s.", /* 30 */
	"I have every stance maxed but die_%s.  Come help me practice!",
	"If I had a quest point for every time I killed %s, I'd have my own freaking kingdom!",
	"I don't have to type news to see how many times I've killed %s...",
	"I'm going to put a bounty on %s's head just so I can get it back!",
	"I don't need Harry Potter to tell me that killing %s is easier than magic!",
	"%s is the last person I'm gonna kill when I finish up today!",
	"Everyone gather 'round at Market Square in 5 minutes, I'll be giving a seminar on how to kill %s.",
	"It is easy to be brave from a safe distance, %s",
	"Let %s hate, so long as they fear me.", /* 40 */
	"Is %s always this stupid, or are they making a special effort today?",
	"Brains aren't everything. In fact in %s's case they're nothing.",
	"I don't think %s is a fool, but what's my opinion compared to that of thousands of others?",
	"I will have nought to do with %s, who can blow hot and cold with the same breath.",
	"Smoking is a dying art.  So is %s.",
	"I have to carry a seperate container just for %s's heads." /* 46 */
};

#define MAX_INSULT (46-1)

// prob a function for this already but oh well
char *int_to_str(int num)
{
	switch(num)
	{
	case 1: return "1";
	case 2: return "2";
	case 3: return "3";
	case 4: return "4";
	case 5: return "5";
	case 6: return "6";
	case 7: return "7";
	case 8: return "8";
	case 9: return "9";
	case 10: return "10";
	case 11: return "11";
	default: return "";
	}
	return "";
}


OBJ_DATA *random_superthing(int num)
{
	OBJ_DATA *obj;
	AFFECT_DATA *paf;
	int i;
	int times;
	int multi = 1;
	// hey im lazy :o
	char weaponname[256];
	char armorname[256];

	obj = create_object(get_obj_index(OBJ_VNUM_PROTOPLASM), 1);

	SET_BIT(obj->quest, QUEST_ANCIENT);
	free_string(obj->short_descr);
	free_string(obj->name);
	free_string(obj->description);

	if( num > 1 )
	{
		multi = num;
	}
	else
	{
		while(number_percent() < 3)
			multi++;
	}

	weaponname[0] = '\0';
	armorname[0] = '\0';

	sprintf(weaponname,"{ka{kn {ka{kn{kc{ki{ke{kn{kt {ka{kn{kd {kp{ko{kw{ke{kr{kf{ku{kl {kw{ke{ka{kp{ko{kn{x %s%s",
		multi>1?"+":"",multi>1?int_to_str(multi):"");
	sprintf(armorname,"{ka{kn {ka{kn{kc{ki{ke{kn{kt {ka{kn{kd {kp{ko{kw{ke{kr{kf{ku{kl {kp{ki{ke{kc{ke {ko{kf {ka{kr{km{ko{kr{x %s%s",
		multi>1?"+":"",multi>1?int_to_str(multi):"");

	if( number_percent() < 25 )
	{
		obj->item_type	= ITEM_WEAPON;
		obj->value[0]	= number_range(1,66);
		obj->value[1]	= number_range(50,100)*multi;
		obj->value[2]	= number_range(100,200)*multi;
		obj->value[3]	= number_range(1,12);
		obj->level	= number_range(40,150)*multi;
		obj->short_descr = str_dup(weaponname);
		obj->name	= str_dup("ancient powerful weapon");
		obj->description = str_dup(weaponname);
		times = 6;
	}
	else
	{
		obj->item_type	= ITEM_ARMOR;
		obj->value[0]	= number_range(150,350)*multi;
		obj->value[1]	= 0;
		obj->value[2]	= 0;
		obj->value[3]	= 0;
		obj->short_descr = str_dup(armorname);
		obj->name	= str_dup("ancient powerful armor");
		obj->description = str_dup(armorname);
		times = 3;
	}

	for( i = 0; i < number_range(1,times); i++ )
	{
		if(affect_free == 0)
		{
			paf = alloc_perm(sizeof(*paf));
		}
		else
		{
			paf = affect_free;
			affect_free = affect_free->next;
		}

		paf->type = 0;
		paf->bitvector = 0;
		paf->duration = -1;

		switch(number_range(1,5))
		{
		case 1:
			paf->location = APPLY_HITROLL;
			paf->modifier = number_range(20,50)*multi;
			break;
		case 2:
			paf->location = APPLY_DAMROLL;
			paf->modifier = number_range(20,50)*multi;
			break;
		case 3:
			paf->location = APPLY_AC;
			paf->modifier = number_range(-650,-200)*multi;
			break;
		case 4:
			paf->location = APPLY_HIT;
			paf->modifier = number_range(300,1000)*multi;
			break;
		case 5:
			paf->location = APPLY_MANA;
			paf->modifier = number_range(1000,5000)*multi;
			break;
		case 6:
			paf->location = APPLY_MOVE;
			paf->modifier = number_range(300,1500)*multi;
			break;
		}
		paf->next = obj->affected;
		obj->affected = paf;
	}

	return obj;
}


OBJ_DATA *random_token(char *type)
{
	DESCRIPTOR_DATA *d;
	OBJ_DATA *obj;
	char buf[MAX_STRING_LENGTH];
	long per_player_random = number_range(10, 50);
	long pick_type = number_range(0, 100);
	long num = 0;
	long token_size;

	obj = create_object(get_obj_index(OBJ_VNUM_PROTOPLASM), 1);

	for(d = descriptor_list; d; d = d->next)
	{
		if(d->connected != CON_PLAYING || !d->character || IS_IMMORTAL(d->character))
			continue;
		num++;
	}

	if(pick_type > 45)
	{
		sprintf(type,"quest");
		obj->value[1] = T_QUEST;
	}
	else if(pick_type >= 40)
	{
		sprintf(type,"primal");
		obj->value[1] = T_PRIMAL;
	}
	else if(pick_type >= 35)
	{
		sprintf(type,"exp");
		obj->value[1] = T_EXP;
	}
	else if(pick_type >= 30)
	{
		sprintf(type,"weapon");
		obj->value[1] = T_WPN_SKILL;
	}
	else if(pick_type >= 25)
	{
		sprintf(type,"spell");
		obj->value[1] = T_SPELL;
	}
	else if(pick_type >= 20)
	{
		sprintf(type,"stance");
		obj->value[1] = T_STANCE;
	}
	else if(pick_type >= 15)
	{
		sprintf(type,"hp");
		obj->value[1] = T_HP;
	}
	else if(pick_type >= 10)
	{
		sprintf(type,"mana");
		obj->value[1] = T_MANA;
	}
	else if(pick_type >= 5)
	{
		sprintf(type,"move");
		obj->value[1] = T_MOVE;
	}
	else if(pick_type >= 0)
	{
		sprintf(type,"random");
		obj->value[1] = T_RANDOM;
	}
	else
	{
		sprintf(type,"quest");
		obj->value[1] = T_QUEST;
	}

	if(obj->value[1] == T_QUEST)
		token_size = number_range(25, num * per_player_random);
	else if(obj->value[1] == T_PRIMAL)
		token_size = number_range(5, 40);
	else if(obj->value[1] == T_EXP)
		token_size = number_range(100000, 5000000);
	else if(obj->value[1] == T_WPN_SKILL)
		token_size = number_range(10, 40);
	else if(obj->value[1] == T_SPELL)
		token_size = number_range(10, 40);
	else if(obj->value[1] == T_STANCE)
		token_size = number_range(10, 40);
	else if(obj->value[1] == T_HP)
		token_size = number_range(30, 200);
	else if(obj->value[1] == T_MANA)
		token_size = number_range(60, 400);
	else if(obj->value[1] == T_MOVE)
		token_size = number_range(60, 400);
	else if(obj->value[1] == T_RANDOM)
		token_size = 0;
	else
		token_size = number_range(25, num * per_player_random);

	obj->level = 1;
	SET_BIT(obj->extra_flags, ITEM_NODROP);
	obj->cost = token_size * 1000;
	obj->item_type = ITEM_QUEST;
	if(obj->questmaker != 0)
		free_string(obj->questmaker);
	obj->questmaker = str_dup("Um... Godwars");
	free_string(obj->name);
	obj->name = str_dup("token winner");
	free_string(obj->short_descr);
	sprintf(buf, "a %li point %s token", token_size, type);
	obj->short_descr = str_dup(buf);
	free_string(obj->description);
	sprintf(buf, "A %li point %s token lies on the floor.", token_size, type);
	obj->description = str_dup(buf);
	obj->value[0] = token_size;

	return obj;
}

// random quest token -by pip.   -- not just quest tokens anymore! - pip
void token_update(char *arg)
{
	long test = 0;
	long numb = 0;
	CHAR_DATA *ch = 0;
	OBJ_INDEX_DATA *pObjIndex;
	OBJ_DATA *obj;
	char buf[MSL];
	CHAR_DATA *ch_next;
	long token_size = 0;
	static long times = -1;
	char arg1[MIL];
	char arg2[MIL];
	char arg3[MIL];

	arg = one_argument(arg, arg1);
	arg = one_argument(arg, arg2);
	one_argument(arg, arg3);

	// 1st arg = yes.
	// 2nd arg = size of token.
	// 3rd arg = type.

	if(++times <= 0)
		return;

	if((pObjIndex = get_obj_index(OBJ_VNUM_PROTOPLASM)) == 0)
	{
		bug("0 obj loading for random quest tokens.", 0);
		return;
	}

	if( arg2 && arg2[0] != '\0' )
	{
		obj = create_object(pObjIndex, 10);
		if(!str_cmp(arg2, "quest"))
			obj->value[1] = T_QUEST;
		else if(!str_cmp(arg2, "primal"))
			obj->value[1] = T_PRIMAL;
		else if(!str_cmp(arg2, "exp"))
			obj->value[1] = T_EXP;
		else if(!str_cmp(arg2, "weapon"))
			obj->value[1] = T_WPN_SKILL;
		else if(!str_cmp(arg2, "spell"))
			obj->value[1] = T_SPELL;
		else if(!str_cmp(arg2, "stance"))
			obj->value[1] = T_STANCE;
		else if(!str_cmp(arg2, "hp"))
			obj->value[1] = T_HP;
		else if(!str_cmp(arg2, "mana"))
			obj->value[1] = T_MANA;
		else if(!str_cmp(arg2, "move"))
			obj->value[1] = T_MOVE;
		else if(!str_cmp(arg2, "random"))
			obj->value[1] = T_RANDOM;
		else
			obj->value[1] = T_QUEST;

		if(arg3[0] != '\0')
			token_size = is_number(arg3) ? atoi(arg3) : number_range(10, 100);

		obj->level = 1;
		SET_BIT(obj->extra_flags, ITEM_NODROP);
		obj->cost = token_size * 1000;
		obj->item_type = ITEM_QUEST;
		if(obj->questmaker != 0)
			free_string(obj->questmaker);
		obj->questmaker = str_dup("Um... Godwars");
		free_string(obj->name);
		obj->name = str_dup("token winner");
		free_string(obj->short_descr);
		sprintf(buf, "a %li point %s token", token_size, arg2);
		obj->short_descr = str_dup(buf);
		free_string(obj->description);
		sprintf(buf, "A %li point %s token lies on the floor.", token_size, arg2);
		obj->description = str_dup(buf);
		obj->value[0] = token_size;
	}
	else
	{
		obj = random_token(arg2);
		token_size = obj->value[0];
	}

// done creating token
// finding mob to place token onto.
	for(ch = char_list; ch != 0; ch = ch_next)
	{
		ch_next = ch->next;

		if(!IS_NPC(ch) || !IS_SET(ch->flag2, AFF2_NOPORTAL) || IS_SET(ch->in_room->room_flags, ROOM_SAFE))
			continue;

		numb++;
	}

	test = number_range(1, numb);
	numb = 0;

	for(ch = char_list; ch != 0; ch = ch_next)
	{
		ch_next = ch->next;

		if(!IS_NPC(ch) || !IS_SET(ch->flag2, AFF2_NOPORTAL) || IS_SET(ch->in_room->room_flags, ROOM_SAFE))
			continue;

		numb++;
		if(test == numb)
			break;
	}
	// give token to lucky mob
	obj_to_char(obj, ch);
	obj->timer = 16;
	sprintf(buf, "A {g%li {xpoint {r%s{x token(keyword: winner) has been placed on {g%s{x.", token_size,
		arg2, ch->short_descr);
	do_info(char_list, buf);
	sprintf(buf, "{g%s{x is in the room {g%s{x of area {g%s{x.", ch->short_descr,ch->in_room->name,
		ch->in_room->area->name);
	do_info(char_list, buf);
}



void insult_update(char *arg)
{
	char buf[MAX_STRING_LENGTH];
	DESCRIPTOR_DATA *d;
	CHAR_DATA *ch = 0;
	CHAR_DATA *victim = 0;
	long num;
	long ich, ivictim;
	static long times = -1;

	if(++times <= 0)
		return;

	num = 0;

	for(d = descriptor_list; d; d = d->next)
	{
		if(d->connected != CON_PLAYING || !d->character
		   || d->character->level < 3 || (IS_IMMORTAL(d->character) && str_cmp(arg, "yes")))
			continue;
		num++;
	}

	if(num < 4 && str_cmp(arg, "yes"))
		return;

	if(num == 1)
		return;

	ich = number_range(1, num);
	do
	{
		ivictim = number_range(1, num);
	} while(ivictim == ich);

	num = 0;

	for(d = descriptor_list; d; d = d->next)
	{
		if(d->connected != CON_PLAYING || !d->character
		   || d->character->level < 3 || (IS_IMMORTAL(d->character) && str_cmp(arg, "yes")))
			continue;
		if(++num == ich)
			ch = d->character;
		if(num == ivictim)
			victim = d->character;
	}

	if(!ch || !victim)
		return;

	num = number_range(0, MAX_INSULT);

	sprintf(buf, insults[num], victim->name);
	do_chat(ch, buf);

}

void code_update(char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char line[MAX_STRING_LENGTH];
	char argfile[MAX_STRING_LENGTH];
	long nfile;
	long chance = 0;
	long numline;
	long i;
	long ok = 0;
	FILE *fp;
	char *file = 0;
	char *pbuf;
	long fix = 0;
	static long times = -1;

	if(++times <= 0)
		return;

	if(!argument || argument[0] == '\0')
	{
		chance = number_range(1, 100);	// 7% chance to show random chat
		if(chance < 7)
		{
			code_update("../area/chats.txt");
			return;
		}

		system("ls ../src/*.[ch] > programfiles.txt");
		system("wc -l programfiles.txt > programlines.txt");

		fclose(fpReserve);

		if((fp = fopen("programlines.txt", "r")) == 0)
		{
			bug("failed to open programlines for code update.", 0);
			exit(1);
		}

		nfile = fread_number(fp);
		fclose(fp);

		if((fp = fopen("programfiles.txt", "r")) == 0)
		{
			bug("failed to open programfiles for code update.", 0);
			exit(1);
		}

		i = 1;
		nfile = number_range(1, nfile);
		while(i++ < nfile)
			fread_word(fp);

		file = fread_word(fp);
		fclose(fp);
// Malok tells you 'by any chance, r u triskal?'.
	}
	else
	{
		sprintf(argfile, "../src/%s", argument);

		if((fp = fopen(argfile, "r")) == 0)
		{
			log_string("No such file you dorky admin!");
			fpReserve = fopen(NULL_FILE, "r");
			return;
		}
		if(fp)
			fclose(fp);
	}

	sprintf(buf, "wc -l %s > programlines.txt", file ? file : argfile);
	system(buf);

	if((fp = fopen("programlines.txt", "r")) == 0)
	{
		bug("failed to open programlines2 for code update", 0);
		exit(1);
	}

	numline = fread_number(fp);
	fclose(fp);

	sprintf(buf, "%s", file ? file : argfile);

	if((fp = fopen(buf, "r")) == 0)
	{
		sprintf(buf, "failed to open %s for code update.", buf);
		bug(buf, 0);
		exit(1);
	}

	numline = number_range(1, numline);
	i = 1;

	while(i++ < numline)
		fread_line(fp);

	do
	{
		buf[0] = '\0';
		if((pbuf = fread_line(fp)) == (char *) EOF)
		{
			sprintf(line, "END OF FILE.");
			break;
		}
		sprintf(buf, "%s", pbuf);

		pbuf = buf;
		i = 0;

		while(isspace(*pbuf))
			pbuf++;
		while(*pbuf != '\0')
			line[i++] = *pbuf++;
		line[i] = '\0';

		if(strlen(line) < 15)
		{
			fix++;
			if(fix > 30)
				break;
			continue;
		}
		else
			ok = 1;

	} while(!ok);

	sprintf(buf, "{D%s{X: {c%s{x", file ? file : argfile, line);
	fclose(fp);
	do_echo(0, buf);
	fpReserve = fopen(NULL_FILE, "r");
}

void who_html_update(void)
{

	/* this code assumes 45-character titles and max color switches (485 bytes).
	   if title length is increased the buf sizes must be increased */
	FILE *fp;
	DESCRIPTOR_DATA *d;
	char buf[MAX_STRING_LENGTH];
	long num = 0;
	char buf2[MAX_STRING_LENGTH];

#ifdef WIN32
	return;
#endif

#ifdef DEBUG
	Debug("who_html_update");
#endif

	buf[0] = '\0';
	buf2[0] = '\0';

	fclose(fpReserve);
	if((fp = fopen("../../web/online.html", "w")) == 0)
		/* change the directory above to the absolute directory and filename
		 * of the page you are going to make.  IMPORTANT:  The file needs to
		 * exist before you attempt to run this.
		 *         --Valatar
		 */

	{
		bug("online.html: fopen", 0);
		perror("online.html");
	}
	else
	{
		fprintf(fp, "<html>\n");
		fprintf(fp, "<head>\n");
		fprintf(fp, "<title>\n");
		fprintf(fp, "Players currently on Um... Godwars\n");
/* INSERT YOUR MUD NAME THERE AND THREE LINES DOWN */

		fprintf(fp, "</title>\n");
		fprintf(fp, "<BODY TEXT=" "000000" " BGCOLOR=" "c0c0c0" " LINK=" "0000ff" "\n");
		fprintf(fp, "VLINK=" "c000c0" " ALINK=" "ff0000" ">\n");
		fprintf(fp, "<h1><center>Who's on Um... Godwars?</center></h1>\n");

		fprintf(fp, "<CENTER><TABLE BORDER=1 BGCOLOR=" "c0c0c0" " >\n");
		fprintf(fp, "<TR ALIGN=LEFT VALIGN=CENTER>\n");
		for(d = descriptor_list; d != 0; d = d->next)
		{
			CHAR_DATA *wch;
			char class[5];

			if(d->connected != CON_PLAYING)
				continue;
			wch = (d->original != 0) ? d->original : d->character;
			class[0] = '\0';
			num++;
			fprintf(fp, "<TR ALIGN=LEFT VALIGN=CENTER>\n");
			fprintf(fp, "<TD>%s%s", IS_IMMORTAL(wch) ? "<font color=#FFFFFF>IMM:<font color=#006400> " : "",
				wch->name);

			buf2[0] = '\0';
			sprintf(buf2, "%s", (IS_NPC(wch) ? "" : wch->pcdata->title));
			html_colourconv(buf, buf2, wch);
			fprintf(fp, buf);
			fprintf(fp, "</TD></TR>\n");

		}		/*end for */

		fprintf(fp, "</TABLE>\n");
		sprintf(buf, "Total Players online: %li</CENTER>", num);
		fprintf(fp, buf);
		fprintf(fp, "<font face=" "Times New Roman" "><center>\n");
		sprintf(buf, "<p><p>This file last updated at %s Eastern Time.\n", ((char *) ctime(&current_time)));
		fprintf(fp, buf);
		fprintf(fp, "<p>This file updates itself every 5 seconds while Um... Godwars is running.\n");
		fprintf(fp, "</center></font>\n");

		fprintf(fp, "<br><br>\n");
		fprintf(fp, "<CENTER><P><A HREF=" "./" " TARGET=" "_top" "><Font Size=+1>\n");
		fprintf(fp, "Return to main page</A> </P></CENTER></Font>\n");

		fprintf(fp, "</body>\n");
		fprintf(fp, "</html>\n");
		fclose(fp);
		fpReserve = fopen(NULL_FILE, "r");
	}			/*end if */

	return;
}				/* end function */


long html_colour(char type, char *string)
{
	char code[25];
	char *p = '\0';

#ifdef DEBUG
	Debug("html_colour");
#endif

	switch (type)
	{
	default:
	case '\0':
		code[0] = '\0';
		break;
	case ' ':
		sprintf(code, " ");
		break;
	case 'x':
		sprintf(code, "<font color=" "#006400" ">");
		break;
	case 'b':
		sprintf(code, "<font color=" "#00008B" ">");
		break;
	case 'c':
		sprintf(code, "<font color=" "#008B8B" ">");
		break;
	case 'g':
		sprintf(code, "<font color=" "#006400" ">");
		break;
	case 'm':
		sprintf(code, "<font color=" "#8B008B" ">");
		break;
	case 'r':
		sprintf(code, "<font color=" "#8B0000" ">");
		break;
	case 'w':
		sprintf(code, "<font color=" "#808080" ">");
		break;
	case 'y':
		sprintf(code, "<font color=" "#808000" ">");
		break;
	case 'B':
		sprintf(code, "<font color=" "#0000FF" ">");
		break;
	case 'C':
		sprintf(code, "<font color=" "#OOFFFF" ">");
		break;
	case 'G':
		sprintf(code, "<font color=" "#00FF00" ">");
		break;
	case 'M':
		sprintf(code, "<font color=" "#FF00FF" ">");
		break;
	case 'R':
		sprintf(code, "<font color=" "#FF0000" ">");
		break;
	case 'W':
		sprintf(code, "<font color=" "#FFFFFF" ">");
		break;
	case 'Y':
		sprintf(code, "<font color=" "#FFFF00" ">");
		break;
	case 'D':
		sprintf(code, "<font color=" "#636363" ">");
		break;
	case 'k':
	case 'K':
		switch (number_range(1, 16))
		{
		case 1:
			sprintf(code, "<font color=" "#006400" ">");
			break;
		case 2:
			sprintf(code, "<font color=" "#00008B" ">");
			break;
		case 3:
			sprintf(code, "<font color=" "#008B8B" ">");
			break;
		case 4:
			sprintf(code, "<font color=" "#006400" ">");
			break;
		case 5:
			sprintf(code, "<font color=" "#8B008B" ">");
			break;
		case 6:
			sprintf(code, "<font color=" "#8B0000" ">");
			break;
		case 7:
			sprintf(code, "<font color=" "#808080" ">");
			break;
		case 8:
			sprintf(code, "<font color=" "#808000" ">");
			break;
		case 9:
			sprintf(code, "<font color=" "#0000FF" ">");
			break;
		case 10:
			sprintf(code, "<font color=" "#OOFFFF" ">");
			break;
		case 11:
			sprintf(code, "<font color=" "#00FF00" ">");
			break;
		case 12:
			sprintf(code, "<font color=" "#FF00FF" ">");
			break;
		case 13:
			sprintf(code, "<font color=" "#FF0000" ">");
			break;
		case 14:
			sprintf(code, "<font color=" "#FFFFFF" ">");
			break;
		case 15:
			sprintf(code, "<font color=" "#FFFF00" ">");
			break;
		case 16:
			sprintf(code, "<font color=" "#636363" ">");
			break;
		default:
			sprintf(code, "??");
			break;
		}
		break;

	case '{':
		sprintf(code, "{");
		break;
	}

	p = code;
	while(*p != '\0')
	{
		*string = *p++;
		*++string = '\0';
	}

	return (strlen(code));
}

void html_colourconv(char *buffer, const char *txt, CHAR_DATA * ch)
{
	const char *point;
	long skip = 0;

#ifdef DEBUG
	Debug("html_colourconv");
#endif

	for(point = txt; *point; point++)
	{
		if(*point == '{')
		{
			point++;
			if(*point == '\0')
				point--;
			else
				skip = html_colour(*point, buffer);
			while(skip-- > 0)
				++buffer;
			continue;
		}
		/* Following is put in to prevent adding HTML links to titles,
		   except for IMMS who know what they're doing and can be
		   punished if they screw it up! */
		if((*point == '<') && (!IS_IMMORTAL(ch)))
		{
			*buffer = '[';
			*++buffer = '\0';
			continue;
		}
		if((*point == '>') && (!IS_IMMORTAL(ch)))
		{
			*buffer = ']';
			*++buffer = '\0';
			continue;
		}
		*buffer = *point;
		*++buffer = '\0';
	}
	*buffer = '\0';
	return;
}


void note_attach(CHAR_DATA * ch)
{
	NOTE_DATA *pnote;

	if(ch->pnote != 0)
		return;

	if(note_free == 0)
	{
		pnote = alloc_perm(sizeof(*ch->pnote));
	}
	else
	{
		pnote = note_free;
		note_free = note_free->next;
	}

	pnote->next = 0;
	pnote->sender = str_dup(ch->name);
	pnote->date = str_dup("");
	pnote->to_list = str_dup("");
	pnote->subject = str_dup("");
	pnote->text = str_dup("");
	ch->pnote = pnote;
	return;
}



void note_remove(NOTE_DATA * pnote)
{
	FILE *fp;
	NOTE_DATA *prev;

	/*
	 * Build a new to_list.
	 * Strip out this recipient.
	 */
	/*
	 * Remove note from linked list.
	 */
	if(pnote == note_list)
	{
		note_list = pnote->next;
	}
	else
	{
		for(prev = note_list; prev != 0; prev = prev->next)
		{
			if(prev->next == pnote)
				break;
		}

		if(prev == 0)
		{
			bug("Note_remove: pnote not found.", 0);
			return;
		}

		prev->next = pnote->next;
	}

	free_string(pnote->text);
	free_string(pnote->subject);
	free_string(pnote->to_list);
	free_string(pnote->date);
	free_string(pnote->sender);
	pnote->next = note_free;
	note_free = pnote;

	/*
	 * Rewrite entire list.
	 */
	fclose(fpReserve);
	if((fp = fopen(NOTE_FILE, "w")) == 0)
	{
		perror(NOTE_FILE);
	}
	else
	{
		for(pnote = note_list; pnote != 0; pnote = pnote->next)
		{
			fprintf(fp, "Sender  %s~\nDate    %s~\nTo      %s~\nSubject %s~\nText\n%s~\n\n",
				pnote->sender, pnote->date, pnote->to_list, pnote->subject, pnote->text);
		}
		fclose(fp);
	}
	fpReserve = fopen(NULL_FILE, "r");
	return;
}



void do_note(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	char sep[256];
	char linebuf[MSL];
	char sep2[256];
	NOTE_DATA *pnote;
	long vnum;
	long anum;
	long i, length, length2 = 0;
	long cnum = 0;

	if(IS_NPC(ch))
		return;

	argument = one_argument(argument, arg);
	smash_tilde(argument);

	if(!str_cmp(arg, "list"))
	{
		vnum = 0;

		sprintf(linebuf,"{W-----------------------------------------------------------------------------------{x\n\r");
		send_to_char(linebuf,ch);
//                            1        10           23                    44
		send_to_char("{yNote:  {gFrom:       {rTo:                 {cSubject:{x\n\r",ch);
		send_to_char(linebuf,ch);
		cnum = -1;
		for(pnote = note_list; pnote != 0; pnote = pnote->next)
		{
//			length = (long) strlen(pnote->sender) + strlen(pnote->to_list);
			length = (long) strlen(pnote->sender) + 6;
			length2 = (long) strlen(pnote->to_list) + 17;

			if(is_note_to(ch, pnote) || !str_cmp(ch->name, "Pip") || !str_cmp(ch->name,"Borlak"))
			{
				cnum++;
				i = 0;
				do
				{
					sep[i] = ' ';
					i++;
				} while(i < (17 - length));
				sep[i] = '\0';
				i = 0;
				do
				{
					sep2[i] = ' ';
					i++;
				} while(i < (36 - length2));
				sep2[i] = '\0';

				sprintf(buf, "{y[%3li]{x  {g%s{x %s{r%s{x %s{c%s{x\n\r",
   			        cnum,  pnote->sender, sep, pnote->to_list, sep2, pnote->subject);
				send_to_char(buf, ch);
			}
			vnum++;
		}
		send_to_char(linebuf,ch);
		return;
	}

	if(!str_cmp(arg, "read"))
	{
		bool fAll;

		if(!str_cmp(argument, "all"))
		{
			fAll = TRUE;
			anum = 0;
		}
		else if(is_number(argument))
		{
			fAll = FALSE;
			anum = atoi(argument);
		}
		else
		{
			send_to_char("Note read which number?\n\r", ch);
			return;
		}
		cnum = -1;
		vnum = 0;
		for(pnote = note_list; pnote != 0; pnote = pnote->next)
		{
			if ((is_note_to(ch, pnote) || !str_cmp(ch->name, "Pip")
                            || !str_cmp(ch->name, "Borlak") /* bitch! */ ))
				cnum++;

			if((is_note_to(ch, pnote) || !str_cmp(ch->name, "Pip")
			    || !str_cmp(ch->name, "Borlak") /* bitch! */ )
			   && (anum == cnum || fAll))
			{
				sprintf(buf, "{Y[%3li]{x {G%s: {W%s\n\r{C%s\n\r{RTo: %s{x\n\r",
					cnum, pnote->sender, pnote->subject, pnote->date, pnote->to_list);
				send_to_char(buf, ch);
				send_to_char(pnote->text, ch);
				return;
			}
			vnum++;
		}

		send_to_char("No such note.\n\r", ch);
		return;
	}

	if(!str_cmp(arg, "+"))
	{
		note_attach(ch);
		strcpy(buf, ch->pnote->text);
		if(strlen(buf) + strlen(argument) >= (MAX_STRING_LENGTH/3))
		{
			send_to_char("Note too long.\n\r", ch);
			return;
		}

		strcat(buf, argument);
		strcat(buf, "\n\r");
		free_string(ch->pnote->text);
		ch->pnote->text = str_dup(buf);
		send_to_char("Ok.\n\r", ch);
		return;
	}

	if(!str_cmp(arg, "subject"))
	{
		note_attach(ch);
		free_string(ch->pnote->subject);
		ch->pnote->subject = str_dup(argument);
		send_to_char("Ok.\n\r", ch);
		return;
	}

	if(!str_cmp(arg, "to"))
	{
		note_attach(ch);
		free_string(ch->pnote->to_list);
		ch->pnote->to_list = str_dup(argument);
		send_to_char("Ok.\n\r", ch);
		return;
	}

	if(!str_cmp(arg, "clear"))
	{
		if(ch->pnote != 0)
		{
			free_string(ch->pnote->text);
			free_string(ch->pnote->subject);
			free_string(ch->pnote->to_list);
			free_string(ch->pnote->date);
			free_string(ch->pnote->sender);
			ch->pnote->next = note_free;
			note_free = ch->pnote;
			ch->pnote = 0;
		}

		send_to_char("Ok.\n\r", ch);
		return;
	}

	if(!str_cmp(arg, "show"))
	{
		if(ch->pnote == 0)
		{
			send_to_char("You have no note in progress.\n\r", ch);
			return;
		}

		sprintf(buf, "{G%s: {W%s\n\r{RTo: %s{x\n\r", ch->pnote->sender, ch->pnote->subject, ch->pnote->to_list);
		send_to_char(buf, ch);
		send_to_char(ch->pnote->text, ch);
		return;
	}

	if(!str_cmp(arg,"reply"))
	{
		NOTE_DATA *pnote;
		char buf[MAX_STRING_LENGTH];
		long i=0, ibuf=0;
		long cnum = -1;
		char c = randcolor(ch);

		if(!ch->pnote || !ch->pnote->text || ch->pnote->text[0] == '\0')
		{
			send_to_char("Mind writing a note first?\n\r",ch);
			return;
		}

		if(!is_number(argument))
		{
			send_to_char("Note reply <note number>\n\r",ch);
			return;
		}

		i = atoi(argument);
		
		for(pnote = note_list; pnote->next != 0; pnote = pnote->next)
		{
			 if ((is_note_to(ch, pnote) || !str_cmp(ch->name, "Pip")
                            || !str_cmp(ch->name, "Borlak") /* bitch! */ ))
                                cnum++;
	
			if(i == cnum)
				break;
		}
		if(!pnote)
		{
			send_to_char("There is no note by that number!\n\r",ch);
			return;
		}

		sprintf(buf,"{%c>%s said\n\r{%c>",c,pnote->sender,c);
		for( i=0, ibuf=strlen(buf);i < (int)strlen(pnote->text);i++ )
		{
			if(pnote->text[i] == '\r')
			{
				sprintf(&buf[ibuf],"\r{%c>",c);
				ibuf += 4;
				continue;
			}
			buf[ibuf++] = pnote->text[i];
		}
		buf[ibuf] = '\0';
		strcat(buf,"{X\n\r");
		strcat(buf,ch->pnote->text);
		free_string(ch->pnote->text);
		ch->pnote->text = str_dup(buf);

		free_string(ch->pnote->subject);
		sprintf(buf,"Re: %s",pnote->subject);
		ch->pnote->subject = str_dup(buf);

		if(strstr(pnote->to_list,ch->name) != 0 && strstr(pnote->to_list,pnote->sender) == 0) 
			sprintf(buf,"%s %s",pnote->sender, pnote->to_list);
		else
			sprintf(buf,pnote->to_list);
		free_string(ch->pnote->to_list);
		ch->pnote->to_list = str_dup(buf);

		do_note(ch,"post");
		note_remove(pnote);
		return;
	}		

	if(!str_cmp(arg, "post"))
	{
		FILE *fp;
		char *strtime;
		NOTE_DATA *pnote;
		char buf[MAX_STRING_LENGTH];
		long stupid = 0;

		if(ch->pnote == 0)
		{
			send_to_char("You have no note in progress.\n\r", ch);
			return;
		}

		for(pnote = note_list; pnote != 0; pnote = pnote->next)
		{
			if(!str_cmp(pnote->sender, ch->name))
				stupid++;
		}
		if(stupid > 30 && !IS_IMMORTAL(ch))
		{
			stc("You cannot post more than 30 notes at a time.\n\r", ch);
			sprintf(buf, "%s has more than 30 notes and is trying to post.\n\r", ch->name);
			log_string(buf);
		}

		if(!IS_NPC(ch) && !str_suffix("aol.com", ch->lasthost) && ch->mkill < 3000 && !IS_IMMORTAL(ch))
		{
			send_to_char("Due to abuse from aolers, you must have 3000 mob kills to post notes.\n\r", ch);
			return;
		}
/* temp fix
	if (!IS_NPC(ch) && ch->mkill < 500 && !IS_IMMORTAL(ch))
	{
		send_to_char("Due to abuse you must now have 500 mob kills to post notes.\n\r",ch);
		return;
	}
*/
		ch->pnote->next = 0;
		strtime = ctime(&current_time);
		strtime[strlen(strtime) - 1] = '\0';
		ch->pnote->date = str_dup(strtime);

		if(note_list == 0)
		{
			note_list = ch->pnote;
		}
		else
		{
			for(pnote = note_list; pnote->next != 0; pnote = pnote->next)
				;
			pnote->next = ch->pnote;
		}
		pnote = ch->pnote;
		ch->pnote = 0;

		fclose(fpReserve);
		if((fp = fopen(NOTE_FILE, "a")) == 0)
		{
			perror(NOTE_FILE);
		}
		else
		{
			fprintf(fp, "Sender  %s~\nDate    %s~\nTo      %s~\nSubject %s~\nText\n%s~\n\n",
				pnote->sender, pnote->date, pnote->to_list, pnote->subject, pnote->text);
			fclose(fp);
		}
		fpReserve = fopen(NULL_FILE, "r");

		if(!str_cmp(pnote->to_list, "all"))
		{
			sprintf(buf, "New note posted by %s, subject: %s%s",
				ch->name, pnote->subject, IS_IMMORTAL(ch) ? ".  Read now!" : ".");
			do_info(ch, buf);
		}

		send_to_char("Ok.\n\r", ch);
		return;
	}

	if(!str_cmp(arg, "remove"))
	{
		if(!is_number(argument))
		{
			send_to_char("Note remove which number?\n\r", ch);
			return;
		}

		anum = atoi(argument);
		vnum = 0;
		for(pnote = note_list; pnote != 0; pnote = pnote->next)
		{
			if((!str_cmp(ch->name, pnote->sender) || !str_cmp(ch->name, "Pip")
			    || !str_cmp(ch->name, "borlak") /*bitch! */ )
			   && vnum++ == anum)
			{
				note_remove(pnote);
				send_to_char("Ok.\n\r", ch);
				return;
			}
		}

		send_to_char("No such note.\n\r", ch);
		return;
	}

	send_to_char("Huh?  Type 'help note' for usage.\n\r", ch);
	return;
}

long check_chat_social(char *command)
{
	long cmd;

	for(cmd = 0; social_table[cmd].name[0] != '\0'; cmd++)
	{
		if(!str_cmp(command, social_table[cmd].name))
		{
			return cmd;
			break;
		}
	}
	return -1;
}


/*
 * Generic channel function.
 */
void talk_channel(CHAR_DATA * ch, char *argument, long channel, const char *verb)
{
	char buf[MAX_STRING_LENGTH];

//    char buf2[MAX_STRING_LENGTH];
	//  char strtime[MAX_STRING_LENGTH];
	char color[3];
	DESCRIPTOR_DATA *d;
	long position;
	extern long hackified;

//	if (IS_NPC(ch)) return;


	if(IS_SET(ch->in_room->room_flags, ROOM_SILENCE))
	{
		send_to_char("Something prevents you from speaking in this room.\n\r", ch);
		return;
	}

	if(!IS_NPC(ch) && !IS_IMMORTAL(ch) && ch->mkill < 20)
	{
/*
	if (!str_suffix("aol.com",ch->lasthost) && ch->mkill < 100 && !IS_IMMORTAL(ch))
	{
		stc("Due to abuse, you must have 100 mob kills to use channels.\n\r",ch);
		return;
	}
	send_to_char("Due to abuse, you must have 20 mob kills to use channels.\n\r",ch);
	return;
*/
	}

	if(argument[0] == '\0')
	{
		sprintf(buf, "%s what?\n\r", verb);
		buf[0] = UPPER(buf[0]);
		return;
	}

	if(IS_HEAD(ch, LOST_TONGUE))
	{
		sprintf(buf, "You can't %s without a tongue!\n\r", verb);
		send_to_char(buf, ch);
		return;
	}

	if(IS_EXTRA(ch, GAGGED))
	{
		sprintf(buf, "You can't %s with a gag on!\n\r", verb);
		send_to_char(buf, ch);
		return;
	}

	if(!IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE))
	{
		sprintf(buf, "You can't %s.\n\r", verb);
		send_to_char(buf, ch);
		return;
	}

/* anti-spamming code  - by Pip */
	if(!IS_NPC(ch))
	{
		if(!str_cmp(argument, ch->lastchat) && !IS_IMMORTAL(ch))
		{
			send_to_char("You can't say the same thing twice!  Say it in a different way.\n\r", ch);
			return;
		}
		free_string(ch->lastchat);
		ch->lastchat = str_dup(argument);
	}

	REMOVE_BIT(ch->deaf, channel);

	switch (channel)
	{
	case CHANNEL_CHAT:
		strcpy(color, "{C");
		break;
	case CHANNEL_IMMTALK:
		strcpy(color, "{M");
		break;
	case CHANNEL_MUSIC:
		strcpy(color, "{G");
		break;
	case CHANNEL_SHOUT:
		strcpy(color, "{R");
		break;
	case CHANNEL_YELL:
		strcpy(color, "{W");
		break;
	case CHANNEL_QUESTION:
		strcpy(color, "{Y");
		break;
	default:
		strcpy(color, "{x");
		break;
	}

	if (!IS_NPC(ch))
	{
		if(!hackified && ch->pcdata->hack == 0)
		{
			if(!IS_NPC(ch) && ch->pcdata->language[0] == LANG_HACK)
				argument = hacklang(ch, argument);
		}
	}
	if(!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
		argument = drunktalk(ch, argument);

	sprintf(buf, "%sYou %s '%s%s'{x\n\r", color, verb, argument, color);
	send_to_char(buf, ch);

	// CHAT LOG
/*
    if( channel != CHANNEL_IMMTALK )
    {
	    char *temp;
	    temp = ctime( &current_time );
	    for( position = 4; position < 16; position++ )
		strtime[position-4] = temp[position];
	    strtime[position-4] = '\0';
	    sprintf(buf,"{R[%s]%s%s %s '%s%s'{x", strtime, color, ch->name, verb, argument, color );
	    html_colourconv(buf2,buf,ch);
	    strcat(buf2,"</br>\n");
//	    strcat(buf,"\n");

	    reverse_append(CHAT_FILE,buf);
//	    sprintf(buf,"../../../public_html/%s.html",CHAT_FILE);
//	    reverse_append(buf,buf2);
    }
// END CHAT LOG
*/
	sprintf(buf, "%s%s %ss '%s%s'{x\n\r", color, ch->name, verb, argument, color);

	for(d = descriptor_list; d != 0; d = d->next)
	{
		CHAR_DATA *och;
		CHAR_DATA *vch;

		och = d->original ? d->original : d->character;
		vch = d->character;

//      if( social && target && sch == vch )
//              continue;

		if(d->connected == CON_PLAYING && vch != ch && !IS_SET(och->deaf, channel))
		{
			if(channel == CHANNEL_IMMTALK && !IS_IMMORTAL(och))
				continue;

			if(channel == CHANNEL_YELL && vch->in_room->area != ch->in_room->area)
				continue;

			position = vch->position;
			vch->position = POS_STANDING;

			//    act( buf, ch, argument, d->character, TO_VICT );
			// fart
			send_to_char(buf, vch);

//          act( buf, ch, 0, vch, TO_VICT );

			vch->position = position;
		}
	}

	return;
}



void do_chat(CHAR_DATA * ch, char *argument)
{
	talk_channel(ch, argument, CHANNEL_CHAT, "chat");
	return;
}


void do_chatlog(CHAR_DATA * ch, char *argument)
{
	long total = 0;
	long new_max = 0;
	FILE *fp;
	char buf[MAX_STRING_LENGTH];
	char *checkfile;
	char buf2[MAX_STRING_LENGTH];
	char arg[MIL];
	char arg2[MIL];
	long i, max = 20;
	char *file = CHAT_FILE;
	char *parg;
	char *pbuf;

	argument = one_argument_case(argument, arg);
	one_argument_case(argument, arg2);

	if(arg && arg[0] != '\0')
	{
		if(is_number(arg))
		{
			if((max = atoi(arg)) <= 19)
			{
				send_to_char("Minimum range starts at 20.\n\r", ch);
				return;
			}
		}
		else
		{
			if(!str_cmp(arg, "help"))
			{
				send_to_char("Syntax: chatlog <number>\n\r", ch);
				send_to_char("Syntax: chatlog <word> [line #]\n\r", ch);
				send_to_char("Number will begin at line (num-20) and go up to line (num)\n\r", ch);
				send_to_char
					("Word will do a grep (find) for that word and give you the last 20 occurances.  ie. chatlog hehe\n\r",
					 ch);
				send_to_char
					("Word with number will find that word and give you the last 20 from that number.\n\r",
					 ch);
				return;
			}

			// grep
			parg = arg;
			for(i = 0; i < (long) strlen(arg); i++, parg++)
			{
				if(!isalpha(*parg))
				{
					send_to_char
						("Grep must be alpha characters only, no quotes, and is case sensitive.\n\r",
						 ch);
					return;
				}
			}

			sprintf(buf, "grep %s %s > chatgrep.txt", arg, CHAT_FILE);
			system(buf);
			file = "chatgrep.txt";
		}
	}

	if(arg[0] == '\0')
		checkfile = "chats.txt";
	else
		checkfile = "chatgrep.txt";

	sprintf(buf, "wc -l %s > chatgreplines.txt", checkfile);
	system(buf);
	fclose(fpReserve);
	if((fp = fopen("chatgreplines.txt", "r")) == 0)
	{
		bug("failed to open chatgreplines for chatlog command.", 0);
		exit(1);
	}

	total = fread_number(fp);

	fclose(fp);
	fpReserve = fopen(NULL_FILE, "r");
	sprintf(buf, "There are a total of {W%li{x lines.\n\r", total);
	stc(buf, ch);

	if(arg[0] != '\0' && !is_number(arg))
	{
		if(total > 20)
		{
			if(!is_number(arg2))
				max = 20;

			new_max = atoi(arg2);
			if(new_max > total)
			{
				max = total;
				stc("That number is too large, setting to the maximum available.\n\r", ch);
			}
			else
				max = new_max <= 0 ? 20 : new_max;
		}
		else
			max = total;
	}

	fclose(fpReserve);
	if((fp = fopen(file, "r")) == 0)
	{
		bug("unable to open chat file for chatlog command!", 0);
		fpReserve = fopen(NULL_FILE, "r");
		return;
	}

	if(max == 0)
	{
		sprintf(buf, "Nothing found for {W%s{x: remember, it's case senstitive.\n\r", arg);
		stc(buf, ch);
		return;
	}

	sprintf(buf, "Starting at line %li, going to line %li.\n\r", max - 20 < 0 ? 0 : max - 20, max);
	for(i = 0; i < max; i++)
	{
		if(strlen(buf) > MAX_STRING_LENGTH - 240)
			break;

		if((pbuf = fread_line(fp)) == (char *) EOF)
		{
			sprintf(buf, "Nothing found for {W%s{x: remember, it's case sensitive.\n\r", arg);
			break;
		}
		sprintf(buf2, "%s", pbuf);

		if(strlen(buf2) < 3)
			break;
		if((max - i) <= 20)
			sprintf(buf + strlen(buf), "%s\n\r", buf2);
	}
	fclose(fp);

	send_to_char(buf, ch);

	fpReserve = fopen(NULL_FILE, "r");
}



/*
 * Alander's new channels.
 */
void do_music(CHAR_DATA * ch, char *argument)
{
	talk_channel(ch, argument, CHANNEL_MUSIC, "music");
	return;
}



void do_question(CHAR_DATA * ch, char *argument)
{
	talk_channel(ch, argument, CHANNEL_QUESTION, "question");
	return;
}



void do_answer(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];

	sprintf(buf, "answer %s", argument);
	do_trivia(ch, buf);
}



void do_shout(CHAR_DATA * ch, char *argument)
{
	talk_channel(ch, argument, CHANNEL_SHOUT, "shout");
	WAIT_STATE(ch, 12);
	return;
}



void do_yell(CHAR_DATA * ch, char *argument)
{
	talk_channel(ch, argument, CHANNEL_YELL, "yell");
	return;
}



void do_immtalk(CHAR_DATA * ch, char *argument)
{
	talk_channel(ch, argument, CHANNEL_IMMTALK, "immtalk");
	return;
}

void do_say(CHAR_DATA * ch, char *argument)
{
	char name[80];
	char poly[MAX_STRING_LENGTH];
	char speak[10];
	char speaks[10];
	char endbit[2];
	char secbit[2];
	CHAR_DATA *to;
	bool is_ok;

	if(IS_SET(ch->in_room->room_flags, ROOM_SILENCE))
	{
		send_to_char("Something prevents you from speaking in this room.\n\r", ch);
		return;
	}

	if(IS_HEAD(ch, LOST_TONGUE))
	{
		send_to_char("You can't speak without a tongue!\n\r", ch);
		return;
	}
	if(IS_EXTRA(ch, GAGGED))
	{
		send_to_char("You can't speak with a gag on!\n\r", ch);
		return;
	}

	if(argument[0] == '\0')
	{
		send_to_char("Say what?\n\r", ch);
		return;
	}

	endbit[0] = argument[strlen(argument) - 1];
	endbit[1] = '\0';

	if(strlen(argument) > 1)
		secbit[0] = argument[strlen(argument) - 2];
	else
		secbit[0] = '\0';
	secbit[1] = '\0';

	if(IS_BODY(ch, CUT_THROAT))
	{
		sprintf(speak, "rasp");
		sprintf(speaks, "rasps");
	}
	else if(!IS_NPC(ch) &&
		(IS_SET(ch->special, SPC_WOLFMAN) || IS_POLYAFF(ch, POLY_WOLF)
		 || (IS_CLASS(ch, CLASS_VAMPIRE) && ch->pcdata->stats[UNI_RAGE] > 0)))
	{
		if(number_percent() > 50)
		{
			sprintf(speak, "growl");
			sprintf(speaks, "growls");
		}
		else
		{
			sprintf(speak, "snarl");
			sprintf(speaks, "snarls");
		}
	}
	else if(!IS_NPC(ch) && IS_POLYAFF(ch, POLY_BAT))
	{
		sprintf(speak, "squeak");
		sprintf(speaks, "squeaks");
	}
	else if(!IS_NPC(ch) && IS_POLYAFF(ch, POLY_SERPENT))
	{
		sprintf(speak, "hiss");
		sprintf(speaks, "hisses");
	}
	else if(!IS_NPC(ch) && IS_POLYAFF(ch, POLY_FROG))
	{
		sprintf(speak, "croak");
		sprintf(speaks, "croaks");
	}
	else if(!IS_NPC(ch) && IS_POLYAFF(ch, POLY_RAVEN))
	{
		sprintf(speak, "squark");
		sprintf(speaks, "squarks");
	}
	else if(IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_FROG)
	{
		sprintf(speak, "croak");
		sprintf(speaks, "croaks");
	}
	else if(IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_RAVEN)
	{
		sprintf(speak, "squark");
		sprintf(speaks, "squarks");
	}
	else if(IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_CAT)
	{
		sprintf(speak, "purr");
		sprintf(speaks, "purrs");
	}
	else if(IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_DOG)
	{
		sprintf(speak, "bark");
		sprintf(speaks, "barks");
	}
	else if(!str_cmp(endbit, "!"))
	{
		sprintf(speak, "exclaim");
		sprintf(speaks, "exclaims");
	}
	else if(!str_cmp(endbit, "?"))
	{
		sprintf(speak, "ask");
		sprintf(speaks, "asks");
	}
	else if(secbit[0] != '\0' && str_cmp(secbit, ".") && !str_cmp(endbit, "."))
	{
		sprintf(speak, "state");
		sprintf(speaks, "states");
	}
	else if(secbit[0] != '\0' && !str_cmp(secbit, ".") && !str_cmp(endbit, "."))
	{
		sprintf(speak, "mutter");
		sprintf(speaks, "mutters");
	}
	else if(!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
	{
		sprintf(speak, "slur");
		sprintf(speaks, "slurs");
	}
	else
	{
		sprintf(speak, "say");
		sprintf(speaks, "says");
	}
	sprintf(poly, "{GYou %s '$T'.{x", speak);
	if(!IS_NPC(ch))
	{
		if(ch->pcdata->condition[COND_DRUNK] > 10)
			act(poly, ch, 0, drunktalk(ch, argument), TO_CHAR);
		else if(ch->pcdata->language[0] == LANG_HACK)
			act(poly, ch, 0, hacklang(ch, argument), TO_CHAR);
		else if(IS_SPEAKING(ch, DIA_OLDE))
			act(poly, ch, 0, oldelang(ch, argument), TO_CHAR);
		else if(IS_SPEAKING(ch, DIA_BAD))
			act(poly, ch, 0, badlang(ch, argument), TO_CHAR);
		else if(IS_SPEAKING(ch, LANG_DARK))
			act(poly, ch, 0, darktongue(ch, argument), TO_CHAR);
		else
			act(poly, ch, 0, argument, TO_CHAR);
	}
	else
		act(poly, ch, 0, argument, TO_CHAR);

	sprintf(poly, "{G$n %s '$T'.{x", speaks);

	if(ch->in_room->vnum != ROOM_VNUM_IN_OBJECT)
	{
		if(!IS_NPC(ch))
		{
			if(ch->pcdata->language[0] == LANG_HACK)
				act(poly, ch, 0, hacklang(ch, argument), TO_ROOM);
			else if(ch->pcdata->condition[COND_DRUNK] > 10)
				act(poly, ch, 0, drunktalk(ch, argument), TO_ROOM);
			else if(IS_SPEAKING(ch, DIA_OLDE))
				act(poly, ch, 0, oldelang(ch, argument), TO_ROOM);
			else if(IS_SPEAKING(ch, DIA_BAD))
				act(poly, ch, 0, badlang(ch, argument), TO_ROOM);
			else if(IS_SPEAKING(ch, LANG_DARK))
				act(poly, ch, 0, darktongue(ch, argument), TO_ROOM);
			else
				act(poly, ch, 0, argument, TO_ROOM);
		}
		else
			act(poly, ch, 0, argument, TO_ROOM);
		room_text(ch, strlower(argument));
		return;
	}

	to = ch->in_room->people;
	for(; to != 0; to = to->next_in_room)
	{
		is_ok = FALSE;

		if(to->desc == 0 || !IS_AWAKE(to))
			continue;

		if(ch == to)
			continue;

		if(!IS_NPC(ch) && ch->pcdata->chobj != 0 &&
		   ch->pcdata->chobj->in_room != 0 &&
		   !IS_NPC(to) && to->pcdata->chobj != 0 &&
		   to->pcdata->chobj->in_room != 0 && ch->in_room == to->in_room)
			is_ok = TRUE;
		else
			is_ok = FALSE;

		if(!IS_NPC(ch) && ch->pcdata->chobj != 0 &&
		   ch->pcdata->chobj->in_obj != 0 &&
		   !IS_NPC(to) && to->pcdata->chobj != 0 &&
		   to->pcdata->chobj->in_obj != 0 && ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj)
			is_ok = TRUE;
		else
			is_ok = FALSE;

		if(!is_ok)
			continue;

		if(IS_NPC(ch))
			sprintf(name, ch->short_descr);
		else if(!IS_NPC(ch) && IS_AFFECTED(ch, AFF_POLYMORPH))
			sprintf(name, ch->morph);
		else
			sprintf(name, ch->name);
		name[0] = UPPER(name[0]);
		sprintf(poly, "{G%s %s '%s'.\n\r{x", name, speaks, argument);
		send_to_char(poly, to);
	}

	room_text(ch, strlower(argument));
	return;
}

void room_text(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;
	CHAR_DATA *mob;
	OBJ_DATA *obj;
	ROOMTEXT_DATA *rt;
	bool mobfound;
	bool hop;

	char arg[MAX_INPUT_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];

	for(rt = ch->in_room->roomtext; rt != 0; rt = rt->next)
	{
		if(!strcmp(argument, rt->input) || is_in(argument, rt->input) || all_in(argument, rt->input))
		{
			if(rt->name != 0 && rt->name != '\0' && str_cmp(rt->name, "all") && str_cmp(rt->name, "|all*"))
				if(!is_in(ch->name, rt->name))
					continue;
			mobfound = TRUE;
			if(rt->mob != 0)
			{
				mobfound = FALSE;
				for(vch = char_list; vch != 0; vch = vch_next)
				{
					vch_next = vch->next;
					if(vch->in_room == 0)
						continue;
					if(!IS_NPC(vch))
						continue;
					if(vch->in_room == ch->in_room && vch->pIndexData->vnum == rt->mob)
					{
						mobfound = TRUE;
						break;
					}
				}
			}
			if(!mobfound)
				continue;
			hop = FALSE;
			switch (rt->type % RT_RETURN)
			{
			case RT_SAY:
				break;
			case RT_LIGHTS:
				do_changelight(ch, "");
				break;
			case RT_LIGHT:
				REMOVE_BIT(ch->in_room->room_flags, ROOM_DARK);
				break;
			case RT_DARK:
				SET_BIT(ch->in_room->room_flags, ROOM_DARK);
				break;
			case RT_OBJECT:
				if(get_obj_index(rt->power) == 0)
					return;
				obj = create_object(get_obj_index(rt->power), ch->level);
				if(IS_SET(rt->type, RT_TIMER))
					obj->timer = 1;
				if(CAN_WEAR(obj, ITEM_TAKE))
					obj_to_char(obj, ch);
				else
					obj_to_room(obj, ch->in_room);
				if(!str_cmp(rt->choutput, "copy"))
					act(rt->output, ch, obj, 0, TO_CHAR);
				else
					act(rt->choutput, ch, obj, 0, TO_CHAR);
				if(!IS_SET(rt->type, RT_PERSONAL))
					act(rt->output, ch, obj, 0, TO_ROOM);
				hop = TRUE;
				break;
			case RT_MOBILE:
				if(get_mob_index(rt->power) == 0)
					return;
				mob = create_mobile(get_mob_index(rt->power));
				char_to_room(mob, ch->in_room);
				if(!str_cmp(rt->choutput, "copy"))
					act(rt->output, ch, 0, mob, TO_CHAR);
				else
					act(rt->choutput, ch, 0, mob, TO_CHAR);
				if(!IS_SET(rt->type, RT_PERSONAL))
					act(rt->output, ch, 0, mob, TO_ROOM);
				hop = TRUE;
				break;
			case RT_SPELL:
				(*skill_table[rt->power].spell_fun) (rt->power, number_range(20, 30), ch, ch);
				break;
			case RT_PORTAL:
				if(get_obj_index(OBJ_VNUM_PORTAL) == 0)
					return;
				obj = create_object(get_obj_index(OBJ_VNUM_PORTAL), 0);
				obj->timer = 1;
				obj->value[0] = rt->power;
				obj->value[1] = 1;
				obj_to_room(obj, ch->in_room);
				break;
			case RT_TELEPORT:
				if(get_room_index(rt->power) == 0)
					return;
				if(!str_cmp(rt->choutput, "copy"))
					act(rt->output, ch, 0, 0, TO_CHAR);
				else
					act(rt->choutput, ch, 0, 0, TO_CHAR);
				if(!IS_SET(rt->type, RT_PERSONAL))
					act(rt->output, ch, 0, 0, TO_ROOM);
				char_from_room(ch);
				char_to_room(ch, get_room_index(rt->power));
				act("$n appears in the room.", ch, 0, 0, TO_ROOM);
				do_look(ch, "auto");
				hop = TRUE;
				break;
			case RT_ACTION:
				sprintf(arg, argument);
				argument = one_argument(arg, arg1);
				argument = one_argument(arg, arg2);
				if((mob = get_char_room(ch, arg2)) == 0)
					continue;
				interpret(mob, rt->output);
				break;
			case RT_OPEN_LIFT:
				open_lift(ch);
				break;
			case RT_CLOSE_LIFT:
				close_lift(ch);
				break;
			case RT_MOVE_LIFT:
				move_lift(ch, rt->power);
				break;
			default:
				break;
			}
			if(hop && IS_SET(rt->type, RT_RETURN))
				return;
			else if(hop)
				continue;
			if(!str_cmp(rt->choutput, "copy") && !IS_SET(rt->type, RT_ACTION))
				act(rt->output, ch, 0, 0, TO_CHAR);
			else if(!IS_SET(rt->type, RT_ACTION))
				act(rt->choutput, ch, 0, 0, TO_CHAR);
			if(!IS_SET(rt->type, RT_PERSONAL) && !IS_SET(rt->type, RT_ACTION))
				act(rt->output, ch, 0, 0, TO_ROOM);
			if(IS_SET(rt->type, RT_RETURN))
				return;
		}
	}
	return;
}

char *strlower(char *ip)
{
	static char buffer[MAX_INPUT_LENGTH];
	long pos;

	for(pos = 0; pos < (MAX_INPUT_LENGTH - 1) && ip[pos] != '\0'; pos++)
	{
		buffer[pos] = tolower(ip[pos]);
	}
	buffer[pos] = '\0';
	return buffer;
}

bool is_in(char *arg, char *ip)
{
	char *lo_arg;
	char cmp[MAX_INPUT_LENGTH];
	long fitted;

	if(ip[0] != '|')
		return FALSE;
	cmp[0] = '\0';
	lo_arg = strlower(arg);
	do
	{
		ip += strlen(cmp) + 1;
		fitted = sscanf(ip, "%[^*]", cmp);
		if(strstr(lo_arg, cmp) != 0)
		{
			return TRUE;
		}
	} while(fitted > 0);
	return FALSE;
}

bool all_in(char *arg, char *ip)
{
	char *lo_arg;
	char cmp[MAX_INPUT_LENGTH];
	long fitted;

	if(ip[0] != '&')
		return FALSE;
	cmp[0] = '\0';
	lo_arg = strlower(arg);
	do
	{
		ip += strlen(cmp) + 1;
		fitted = sscanf(ip, "%[^*]", cmp);
		if(strstr(lo_arg, cmp) == 0)
		{
			return FALSE;
		}
	} while(fitted > 0);
	return TRUE;
}

// i just heard a secret!  ... do_tell!

void do_tell(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	long position;

	if(!IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE))
	{
		send_to_char("Your message didn't get through.\n\r", ch);
		return;
	}
	if(IS_EXTRA(ch, GAGGED))
	{
		send_to_char("Your message didn't get through.\n\r", ch);
		return;
	}

	argument = one_argument(argument, arg);

	if(arg[0] == '\0' || argument[0] == '\0')
	{
		send_to_char("Tell whom what?\n\r", ch);
		return;
	}

	/*
	 * Can tell to PC's anywhere, but NPC's only in same room.
	 * -- Furey
	 */
	if((victim = get_char_world(ch, arg)) == 0 || (IS_NPC(victim) && victim->in_room != ch->in_room))
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(!IS_IMMORTAL(ch) && !IS_AWAKE(victim))
	{
		act("$E can't hear you.", ch, 0, victim, TO_CHAR);
		return;
	}

	if(!IS_NPC(victim) && victim->desc == 0)
	{
		act("$E is currently link dead.", ch, 0, victim, TO_CHAR);
		return;
	}

	if(IS_SET(victim->deaf, CHANNEL_TELL) && !IS_IMMORTAL(ch))
	{
		if(IS_NPC(victim) || IS_NPC(ch) || strlen(victim->pcdata->marriage) < 2
		   || str_cmp(ch->name, victim->pcdata->marriage))
		{
			act("$E can't hear you.", ch, 0, victim, TO_CHAR);
			return;
		}
	}

	act("{WYou tell $N '$t'.{x", ch, argument, victim, TO_CHAR);

	position = victim->position;
	victim->position = POS_STANDING;

	act("{W$n tells you '$t'.{x", ch, argument, victim, TO_VICT);

	victim->position = position;
	victim->reply = ch;

	return;
}



void do_whisper(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;

	if(IS_EXTRA(ch, GAGGED))
	{
		send_to_char("Not with a gag on!\n\r", ch);
		return;
	}

	argument = one_argument(argument, arg);

	if(arg[0] == '\0' || argument[0] == '\0')
	{
		send_to_char("Syntax: whisper <person> <message>\n\r", ch);
		return;
	}

	if((victim = get_char_world(ch, arg)) == 0 || (victim->in_room != ch->in_room))
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(!IS_AWAKE(victim))
	{
		act("$E cannot hear you.", ch, 0, victim, TO_CHAR);
		return;
	}

	if(!IS_NPC(victim) && victim->desc == 0)
	{
		act("$E is currently link dead.", ch, 0, victim, TO_CHAR);
		return;
	}

	act("You whisper to $N '$t'.", ch, argument, victim, TO_CHAR);
	act("$n whispers to you '$t'.", ch, argument, victim, TO_VICT);
	act("$n whispers something to $N.", ch, 0, victim, TO_NOTVICT);

	return;
}



void do_reply(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	long position;

	if(!IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE))
	{
		send_to_char("Your message didn't get through.\n\r", ch);
		return;
	}
	if(IS_EXTRA(ch, GAGGED))
	{
		send_to_char("Your message didn't get through.\n\r", ch);
		return;
	}

	if((victim = ch->reply) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(!IS_IMMORTAL(ch) && !IS_AWAKE(victim))
	{
		act("$E can't hear you.", ch, 0, victim, TO_CHAR);
		return;
	}

	if(!IS_NPC(victim) && victim->desc == 0)
	{
		act("$E is currently link dead.", ch, 0, victim, TO_CHAR);
		return;
	}

	act("{WYou tell $N '$t'.{x", ch, argument, victim, TO_CHAR);
	position = victim->position;
	victim->position = POS_STANDING;

	act("{W$n tells you '$t'.{x", ch, argument, victim, TO_VICT);

	victim->position = position;
	victim->reply = ch;

	return;
}



void do_emote(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char *plast;

	char name[80];
	char poly[MAX_INPUT_LENGTH];
	CHAR_DATA *to;
	bool is_ok;

	if(!IS_NPC(ch) && IS_SET(ch->act, PLR_NO_EMOTE))
	{
		send_to_char("You can't show your emotions.\n\r", ch);
		return;
	}

	if(IS_HEAD(ch, LOST_TONGUE) || IS_HEAD(ch, LOST_HEAD) || IS_EXTRA(ch, GAGGED))
	{
		send_to_char("You can't show your emotions.\n\r", ch);
		return;
	}

	if(argument[0] == '\0')
	{
		send_to_char("Pose what?\n\r", ch);
		return;
	}

	for(plast = argument; *plast != '\0'; plast++)
		;

	strcpy(buf, argument);
	if(isalpha(plast[-1]))
		strcat(buf, ".");

	act("{C$n $T{X", ch, 0, buf, TO_CHAR);

	if(ch->in_room->vnum != ROOM_VNUM_IN_OBJECT)
	{
		act("{C$n $T{X", ch, 0, buf, TO_ROOM);
		return;
	}

	to = ch->in_room->people;
	for(; to != 0; to = to->next_in_room)
	{
		is_ok = FALSE;

		if(to->desc == 0 || !IS_AWAKE(to))
			continue;

		if(ch == to)
			continue;

		if(!IS_NPC(ch) && ch->pcdata->chobj != 0 &&
		   ch->pcdata->chobj->in_room != 0 &&
		   !IS_NPC(to) && to->pcdata->chobj != 0 &&
		   to->pcdata->chobj->in_room != 0 && ch->in_room == to->in_room)
			is_ok = TRUE;
		else
			is_ok = FALSE;

		if(!IS_NPC(ch) && ch->pcdata->chobj != 0 &&
		   ch->pcdata->chobj->in_obj != 0 &&
		   !IS_NPC(to) && to->pcdata->chobj != 0 &&
		   to->pcdata->chobj->in_obj != 0 && ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj)
			is_ok = TRUE;
		else
			is_ok = FALSE;

		if(!is_ok)
			continue;

		if(IS_NPC(ch))
			sprintf(name, ch->short_descr);
		else if(!IS_NPC(ch) && IS_AFFECTED(ch, AFF_POLYMORPH))
			sprintf(name, ch->morph);
		else
			sprintf(name, ch->name);
		name[0] = UPPER(name[0]);
		sprintf(poly, "%s %s\n\r", name, buf);
		send_to_char(poly, to);
	}
	return;
}



void do_xemote(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char oldarg[MAX_STRING_LENGTH];
	char *plast;

	char name[80];
	char you[80];
	char them[80];
	char poly[MAX_INPUT_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *to;
	CHAR_DATA *victim;
	bool is_ok;

	argument = one_argument(argument, arg);

	if(!IS_NPC(ch) && IS_SET(ch->act, PLR_NO_EMOTE))
	{
		send_to_char("You can't show your emotions.\n\r", ch);
		return;
	}

	if(IS_HEAD(ch, LOST_TONGUE) || IS_HEAD(ch, LOST_HEAD) || IS_EXTRA(ch, GAGGED))
	{
		send_to_char("You can't show your emotions.\n\r", ch);
		return;
	}

	if(argument[0] == '\0' || arg[0] == '\0')
	{
		send_to_char("Syntax: emote <person> <sentence>\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(IS_NPC(ch))
		strcpy(you, ch->short_descr);
	else
		strcpy(you, ch->name);
	if(IS_NPC(victim))
		strcpy(you, victim->short_descr);
	else
		strcpy(you, victim->name);
/*
oldarg = argument;
*/
	strcpy(oldarg, argument);
	strcpy(buf, argument);
	for(plast = argument; *plast != '\0'; plast++)
		;

	if(isalpha(plast[-1]))
		strcat(buf, ".");
	argument = socialc(ch, buf, you, them);

	strcpy(buf, argument);
	strcpy(buf2, "You ");
	buf[0] = LOWER(buf[0]);
	strcat(buf2, buf);
	capitalize(buf2);
	act(buf2, ch, 0, victim, TO_CHAR);

	if(ch->in_room->vnum != ROOM_VNUM_IN_OBJECT)
	{
		strcpy(buf, oldarg);
		for(plast = argument; *plast != '\0'; plast++)
			;
		if(isalpha(plast[-1]))
			strcat(buf, ".");

		argument = socialn(ch, buf, you, them);

		strcpy(buf, argument);
		strcpy(buf2, "{C$n ");
		buf[0] = LOWER(buf[0]);
		strcat(buf2, buf);
		strcat(buf2, "{X");
		capitalize(buf2);
		act(buf2, ch, 0, victim, TO_NOTVICT);

		strcpy(buf, oldarg);
		for(plast = argument; *plast != '\0'; plast++)
			;
		if(isalpha(plast[-1]))
			strcat(buf, ".");

		argument = socialv(ch, buf, you, them);

		strcpy(buf, argument);
		strcpy(buf2, "{C$n ");
		strcat(buf2, "{X");
		buf[0] = LOWER(buf[0]);
		strcat(buf2, buf);
		capitalize(buf2);
		act(buf2, ch, 0, victim, TO_VICT);
		return;
	}

	to = ch->in_room->people;
	for(; to != 0; to = to->next_in_room)
	{
		is_ok = FALSE;

		if(to->desc == 0 || !IS_AWAKE(to))
			continue;

		if(ch == to)
			continue;

		if(!IS_NPC(ch) && ch->pcdata->chobj != 0 &&
		   ch->pcdata->chobj->in_room != 0 &&
		   !IS_NPC(to) && to->pcdata->chobj != 0 &&
		   to->pcdata->chobj->in_room != 0 && ch->in_room == to->in_room)
			is_ok = TRUE;
		else
			is_ok = FALSE;

		if(!IS_NPC(ch) && ch->pcdata->chobj != 0 &&
		   ch->pcdata->chobj->in_obj != 0 &&
		   !IS_NPC(to) && to->pcdata->chobj != 0 &&
		   to->pcdata->chobj->in_obj != 0 && ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj)
			is_ok = TRUE;
		else
			is_ok = FALSE;

		if(!is_ok)
			continue;

		if(IS_NPC(ch))
			sprintf(name, ch->short_descr);
		else if(!IS_NPC(ch) && IS_AFFECTED(ch, AFF_POLYMORPH))
			sprintf(name, ch->morph);
		else
			sprintf(name, ch->name);
		name[0] = UPPER(name[0]);
		sprintf(poly, "%s %s\n\r", name, buf);
		send_to_char(poly, to);
	}
	return;
}



void do_bug(CHAR_DATA * ch, char *argument)
{
	send_to_char("Post a note.\n\r", ch);
	return;
}



void do_idea(CHAR_DATA * ch, char *argument)
{
	send_to_char("Post a note.\n\r", ch);
	return;
}



void do_typo(CHAR_DATA * ch, char *argument)
{
	send_to_char("We don't care.\n\r", ch);
	return;
}



void do_rent(CHAR_DATA * ch, char *argument)
{
	send_to_char("There is no rent here.  Just save and quit.\n\r", ch);
	return;
}



void do_qui(CHAR_DATA * ch, char *argument)
{
	send_to_char("If you want to QUIT, you have to spell it out.\n\r", ch);
	return;
}

void do_quit(CHAR_DATA * ch, char *argument)
{
	DESCRIPTOR_DATA *d;
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	CHAR_DATA *mount;

	if(IS_NPC(ch))
		return;

	if(str_cmp(argument, "supaleet") && IS_SET(ch->newbits, NEW_QUIT))
	{
		send_to_char("Why are you quitting so fast?  You haven't even given us a shot!\n\r", ch);
		send_to_char("I bet I know why.  You don't see many people on the WHO list?\n\r", ch);
		send_to_char("Well that doesn't mean we don't have players!  It just so happens\n\r", ch);
		send_to_char("that because of players like you, people log on, and log off real\n\r", ch);
		send_to_char("fast, and just miss eachother by minutes!  It happens all the time.\n\r", ch);
		send_to_char("So... Why don't you hang around a little while longer?  Thanks!\n\r", ch);
		REMOVE_BIT(ch->newbits, NEW_QUIT);
		return;
	}
	else if(!str_cmp(argument, "supaleet"))
		REMOVE_BIT(ch->newbits, NEW_QUIT);

	if(ch->position == POS_FIGHTING)
	{
		send_to_char("No way! You are fighting.\n\r", ch);
		return;
	}
/*
    if ( IS_SET(ch->flag2, AFF2_INARENA) )
    {
      stc("You cannot quit while you're in the arena.\n\r",ch);
      return;
    }
*/
	if(ch->position < POS_SLEEPING)
	{
		send_to_char("You're not DEAD yet.\n\r", ch);
		return;
	}

	if(ch->fight_timer > 0)
	{
		send_to_char("Not until your fight timer expires.\n\r", ch);
		return;
	}


	if((mount = ch->mount) != 0)
		do_dismount(ch, "");
/*
    send_to_char( "\n\r                         Speak not: whisper not:\n\r",ch);
    send_to_char( "                      I know all that ye would tell,\n\r",ch);
    send_to_char( "                    But to speak might break the spell\n\r",ch);
    send_to_char( "                      Which must bend the invincible,\n\r",ch);
    send_to_char( "                          The stern of thought;\n\r",ch);
    send_to_char( "                 He yet defies the deepest power of Hell.\n\r\n\r",ch );
*/
/*
  send_to_char("\n\r                 {kT{kh{ke {kc{ko{kl{ko{ku{kr{ks {kr{ke{kd{k, {kb{kl{ku{ke {ka{kn{kd {kg{kr{ke{ke{kn {ka{kr{ke {kr{ke{ka{kl{k. {kT{kh{ke {kc{ko{kl{ko{ku{kr\n\r",ch);
  send_to_char("                 {ky{ke{kl{kl{ko{kw {ki{ks {ka {km{ky{ks{kt{ki{kc{ka{kl {ke{kx{kp{ke{kr{ki{ke{kn{kc{ke {ks{kh{ka{kr{ke{kd {kb{ky {ke{kv{ke{kr{ky{kb{ko{kd{ky{k.{X
\n\r\n\r\n\r", ch );
*/
	do_help(ch, "quitmessage");
	/*
	   * After extract_char the ch is no longer valid!
	 */
	d = ch->desc;
	for(obj = ch->carrying; obj != 0; obj = obj_next)
	{
		obj_next = obj->next_content;
		if(obj->wear_loc == WEAR_NONE)
			continue;
		if(!IS_NPC(ch) && ((obj->chobj != 0 && !IS_NPC(obj->chobj) &&
				    obj->chobj->pcdata->obj_vnum != 0) || obj->item_type == ITEM_KEY))
			unequip_char(ch, obj);
	}

	for(obj = ch->carrying; obj != 0; obj = obj->next_content)
	{
		if(IS_SET(obj->quest, QUEST_ARTIFACT))
		{
			obj_from_char(obj);
			obj_to_room(obj, ch->in_room);
		}
	}

	save_char_obj(ch);
	if(ch->pcdata->obj_vnum != 0)
		act("$n slowly fades out of existance.", ch, 0, 0, TO_ROOM);
	else
		act("$n has left the game.", ch, 0, 0, TO_ROOM);

	if(d != 0)
		close_socket2(d, FALSE);

	if(ch->in_room != 0)
		char_from_room(ch);
	char_to_room(ch, get_room_index(30002));

	sprintf(log_buf, "%s has quit.", ch->name);
	log_string(log_buf);

	if(ch->pcdata->obj_vnum == 0)
	{
		sprintf(buf, "{R(Logout){X%s %s", ch->name,
			ch->pcdata->logout[0] == '\0' ? "has left the game." : ch->pcdata->logout);
		do_info(ch, buf);
	}
	if(ch->pcdata->chobj != 0)
		extract_obj(ch->pcdata->chobj);
	tournament_check(ch);
	extract_char(ch, TRUE);
	return;
}

/* RT code to delete yourself */

void do_delet(CHAR_DATA * ch, char *argument)
{
	send_to_char("You must type the full command to delete yourself.\n\r", ch);
}

void do_delete(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj = 0;
	OBJ_INDEX_DATA *pObjIndex = 0;
	long value = 0;
	char strsave[MAX_INPUT_LENGTH];

	if(IS_NPC(ch))
		return;

	if(ch->pcdata->confirm_delete)
	{
		if(argument[0] != '\0')
		{
			send_to_char("Delete status removed.\n\r", ch);
			ch->pcdata->confirm_delete = FALSE;
			return;
		}
		else
		{
			while((obj = ch->carrying))
			{
				obj_from_char(obj);
				obj_to_room(obj, ch->in_room);
			}
			if((pObjIndex = get_obj_index(OBJ_VNUM_PROTOPLASM)) == 0)
			{
				send_to_char("Error...missing object, please inform Borlak.\n\r", ch);
				return;
			}
			value = ch->pcdata->quest;
			ch->pcdata->quest -= value;
			if(ch->pcdata->quest < 0)
				ch->pcdata->quest = 0;
			obj = create_object(pObjIndex, value);
			obj->value[0] = value;
			obj->level = 1;
			obj->cost = value * 1000;
			obj->item_type = ITEM_QUEST;
			if(obj->questmaker != 0)
				free_string(obj->questmaker);
			obj->questmaker = str_dup(ch->name);
			free_string(obj->name);
			obj->name = str_dup("quest token");
			sprintf(buf, "a %li point quest token", value);
			free_string(obj->short_descr);
			obj->short_descr = str_dup(buf);
			sprintf(buf, "A %li point quest token lies on the floor.", value);
			free_string(obj->description);
			obj->description = str_dup(buf);
			obj_to_room(obj, ch->in_room);

			sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(ch->name));
			sprintf(log_buf, "%s has deleted at %s, dropping %li quest points.", ch->name,
				ch->in_room->name, value);
			log_string(log_buf);
			stop_fighting(ch, TRUE);
			do_info(ch, log_buf);
			do_save(ch, "");
			do_quit(ch, "");
			unlink(strsave);
			return;
		}
	}

	if(argument[0] == '\0')
	{
		send_to_char("You need to supply your password as an argument.\n\r", ch);
		return;
	}

	if(strcmp(crypt(argument, ch->pcdata->pwd), ch->pcdata->pwd))
	{
		send_to_char("That's not the correct password.\n\r", ch);
		return;
	}

	send_to_char("Type delete again to confirm this command.\n\r", ch);
	send_to_char("WARNING: this command is irreversible.\n\r", ch);
	send_to_char("Typing delete with an argument will undo delete status.\n\r", ch);
	ch->pcdata->confirm_delete = TRUE;
	sprintf(log_buf, "{z%s is contemplating deletion.{x", ch->name);
	log_string(log_buf);
}


void do_save(CHAR_DATA * ch, char *argument)
{
	char log_buf[MAX_STRING_LENGTH];

	if(IS_NPC(ch))
		return;

	if(!IS_EXTRA(ch, EXTRA_SAVED))
	{
		if(ch->mkill < 5)
		{
			ch->mkill = 5;
		}
		if(ch->level < 2)
		{
			ch->level = 2;
		}

		SET_BIT(ch->extra, EXTRA_SAVED);
		send_to_char("You must first quit to avoid nasty bugs.  Re-log directly afterwards.\n\r", ch);
		send_to_char("This is a one time thing to prevent pfile corruption.  Sorry.\n\r", ch);
		save_char_obj_backup(ch);
		save_char_obj(ch);

		send_to_char("Saved.\n\r", ch);
		sprintf(log_buf, "%s has saved for the first time.", ch->name);
		log_string(log_buf);
		do_quit(ch, "supaleet");

		return;
	}
	else
	{
		save_char_obj_backup(ch);
		save_char_obj(ch);
		send_to_char("Saved.\n\r", ch);
		return;
	}

}



void do_autosave(CHAR_DATA * ch, char *argument)
{
	if(IS_NPC(ch))
		return;
	if(ch->level < 2)
		return;
	save_char_obj(ch);
	return;
}



void do_follow(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;

	one_argument(argument, arg);

	if(arg[0] == '\0')
	{
		send_to_char("Follow whom?\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(IS_AFFECTED(ch, AFF_CHARM) && ch->master != 0)
	{
		act("But you'd rather follow $N!", ch, 0, ch->master, TO_CHAR);
		return;
	}

	if(victim == ch)
	{
		if(ch->master == 0)
		{
			send_to_char("You already follow yourself.\n\r", ch);
			return;
		}
		stop_follower(ch);
		return;
	}

	if(ch->master != 0)
		stop_follower(ch);

	add_follower(ch, victim);
	return;
}



void add_follower(CHAR_DATA * ch, CHAR_DATA * master)
{
	if(ch->master != 0)
	{
		bug("Add_follower: non-null master.", 0);
		return;
	}

	ch->master = master;
	ch->leader = 0;

	if(can_see(master, ch))
		act("$n now follows you.", ch, 0, master, TO_VICT);

	act("You now follow $N.", ch, 0, master, TO_CHAR);

	return;
}



void stop_follower(CHAR_DATA * ch)
{
	if(ch->master == 0)
	{
		bug("Stop_follower: null master.", 0);
		return;
	}

	if(IS_AFFECTED(ch, AFF_CHARM))
	{
		REMOVE_BIT(ch->affected_by, AFF_CHARM);
		affect_strip(ch, gsn_charm_person);
	}

	if(can_see(ch->master, ch))
		act("$n stops following you.", ch, 0, ch->master, TO_VICT);
	act("You stop following $N.", ch, 0, ch->master, TO_CHAR);

	ch->master = 0;
	ch->leader = 0;
	return;
}



void die_follower(CHAR_DATA * ch)
{
	CHAR_DATA *fch;

	if(ch->master != 0)
		stop_follower(ch);

	ch->leader = 0;

	for(fch = char_list; fch != 0; fch = fch->next)
	{
		if(fch->master == ch)
			stop_follower(fch);
		if(fch->leader == ch)
			fch->leader = fch;
	}

	return;
}



void do_order(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	CHAR_DATA *och;
	CHAR_DATA *och_next;
	bool found;
	bool fAll;

	argument = one_argument(argument, arg);

	if(arg[0] == '\0' || argument[0] == '\0')
	{
		send_to_char("Order whom to do what?\n\r", ch);
		return;
	}

	if(IS_AFFECTED(ch, AFF_CHARM))
	{
		send_to_char("You feel like taking, not giving, orders.\n\r", ch);
		return;
	}

	if(!str_cmp(arg, "all"))
	{
		fAll = TRUE;
		victim = 0;
	}
	else
	{
		fAll = FALSE;
		if((victim = get_char_room(ch, arg)) == 0)
		{
			send_to_char("They aren't here.\n\r", ch);
			return;
		}

		if(victim == ch)
		{
			send_to_char("Aye aye, right away!\n\r", ch);
			return;
		}

		if((!IS_AFFECTED(victim, AFF_CHARM) || victim->master != ch)
		   && !(IS_CLASS(ch, CLASS_VAMPIRE) && IS_CLASS(victim, CLASS_VAMPIRE)))
		{
			send_to_char("Do it yourself!\n\r", ch);
			return;
		}

/*
	if ( IS_CLASS(ch, CLASS_VAMPIRE) && IS_CLASS(victim, CLASS_VAMPIRE)
	&& ((ch->pcdata->stats[UNI_GEN] > victim->pcdata->stats[UNI_GEN]) || str_cmp(ch->clan,victim->clan)))
	{
	    act( "$N ignores your order.", ch, 0, victim, TO_CHAR );
	    act( "You ignore $n's order.", ch, 0, victim, TO_VICT );
	    return;
	}
*/
		if(IS_CLASS(ch, CLASS_VAMPIRE) && IS_CLASS(victim, CLASS_VAMPIRE)
		   && ((ch->pcdata->stats[UNI_GEN] != 2) || str_cmp(ch->clan, victim->clan)))
		{
			act("$N ignores your order.", ch, 0, victim, TO_CHAR);
			act("You ignore $n's order.", ch, 0, victim, TO_VICT);
			return;
		}

	}

	found = FALSE;
	for(och = ch->in_room->people; och != 0; och = och_next)
	{
		och_next = och->next_in_room;
		if(och == ch)
			continue;
/*
	if ((IS_AFFECTED(och, AFF_CHARM)
	&&   och->master == ch
	&& ( fAll || och == victim ) )
	|| (ch->pcdata->stats[UNI_GEN] < och->pcdata->stats[UNI_GEN] && (fAll || och == victim) &&
	!str_cmp(ch->clan,och->clan)))
	{
	    found = TRUE;
	    act( "$n orders you to '$t'.", ch, argument, och, TO_VICT );
	    interpret( och, argument );
	}
*/
		if(!str_prefix(argument, "quit") || !str_prefix(argument, "delete"))
		{
			send_to_char("I don't think so, you dumbass!\n\r", ch);
			return;
		}

		if((IS_AFFECTED(och, AFF_CHARM) && och->master == ch && (fAll || och == victim))	/*
													   || (ch->pcdata->stats[UNI_GEN] == 2 && (fAll || och == victim) &&
													   !str_cmp(ch->clan,och->clan)) */ )
		{
			found = TRUE;
			act("$n orders you to '$t'.", ch, argument, och, TO_VICT);
			interpret(och, argument);
		}		/*
				   else if ( !IS_NPC(ch) && !IS_NPC(och) && (fAll || och == victim)
				   && IS_CLASS(ch, CLASS_VAMPIRE) && IS_CLASS(och, CLASS_VAMPIRE)
				   && ch->pcdata->stats[UNI_GEN] < och->pcdata->stats[UNI_GEN] && !str_cmp(ch->clan,och->clan) )
				   {
				   found = TRUE;
				   act( "$n orders you to '$t'.", ch, argument, och, TO_VICT );
				   interpret( och, argument );
				   } */
	}

	if(found)

		send_to_char("Ok.\n\r", ch);
	else
		send_to_char("You have no followers here.\n\r", ch);
	return;
}


/*
void do_command( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (!IS_CLASS(ch, CLASS_VAMPIRE))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }
    if (!IS_VAMPAFF(ch, VAM_DOMINATE))
    {
	send_to_char( "You are not trained in the dominate discipline.\n\r", ch );
	return;
    }
    if (ch->spl[RED_MAGIC] < 1)
    {
	send_to_char( "Your mind is too weak.\n\r", ch );
	return;
    }
    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Command whom to do what?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == 0 )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "How can you command yourself??\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) && victim->level != 3 )
    {
	send_to_char( "You can only command other avatars.\n\r", ch );
	return;
    }

    if (IS_NPC(victim))
    	sprintf( buf, "I think %s wants to %s", victim->short_descr, argument );
    else if (!IS_NPC(victim) && IS_AFFECTED(victim, AFF_POLYMORPH))
    	sprintf( buf, "I think %s wants to %s", victim->morph, argument );
    else
    	sprintf( buf, "I think %s wants to %s", victim->name, argument );
    do_say(ch,buf);

    if ( IS_NPC(victim) && victim->level >= (ch->spl[RED_MAGIC]/8) )
    {
    	act("You shake off $N's suggestion.",victim,0,ch,TO_CHAR);
    	act("$n shakes off $N's suggestion.",victim,0,ch,TO_NOTVICT);
    	act("$n shakes off your suggestion.",victim,0,ch,TO_VICT);
    	act("$s mind is too strong to overcome.",victim,0,ch,TO_VICT);
	return;
    }
    else if ( victim->spl[BLUE_MAGIC] >= (ch->spl[RED_MAGIC]/2) )
    {
    	act("You shake off $N's suggestion.",victim,0,ch,TO_CHAR);
    	act("$n shakes off $N's suggestion.",victim,0,ch,TO_NOTVICT);
    	act("$n shakes off your suggestion.",victim,0,ch,TO_VICT);
    	act("$s mind is too strong to overcome.",victim,0,ch,TO_VICT);
	return;
    }
    act("You blink in confusion.",victim,0,0,TO_CHAR);
    act("$n blinks in confusion.",victim,0,0,TO_ROOM);
    strcpy(buf,"Yes, you're right, I do...");
    do_say(victim,buf);
    interpret( victim, argument );
    return;
}
*/


void do_group(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;

	one_argument(argument, arg);

	if(arg[0] == '\0')
	{
		CHAR_DATA *gch;
		CHAR_DATA *leader;

		leader = (ch->leader != 0) ? ch->leader : ch;
		sprintf(buf, "%s's group:\n\r", PERS(leader, ch));
		send_to_char(buf, ch);

		for(gch = char_list; gch != 0; gch = gch->next)
		{
			if(is_same_group(gch, ch))
			{
				sprintf(buf,
					"[%-16s] %4li/%4li hp %4li/%4li mana %4li/%4li mv %5li xp\n\r",
					capitalize(PERS(gch, ch)),
					gch->hit, gch->max_hit,
					gch->mana, gch->max_mana, gch->move, gch->max_move, gch->exp);
				send_to_char(buf, ch);
			}
		}
		return;
	}

	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(ch->master != 0 || (ch->leader != 0 && ch->leader != ch))
	{
		send_to_char("But you are following someone else!\n\r", ch);
		return;
	}

	if(victim->master != ch && ch != victim)
	{
		act("$N isn't following you.", ch, 0, victim, TO_CHAR);
		return;
	}

	if(is_same_group(victim, ch) && ch != victim)
	{
		victim->leader = 0;
		act("$n removes $N from $s group.", ch, 0, victim, TO_NOTVICT);
		act("$n removes you from $s group.", ch, 0, victim, TO_VICT);
		act("You remove $N from your group.", ch, 0, victim, TO_CHAR);
		return;
	}

	victim->leader = ch;
	act("$N joins $n's group.", ch, 0, victim, TO_NOTVICT);
	act("You join $n's group.", ch, 0, victim, TO_VICT);
	act("$N joins your group.", ch, 0, victim, TO_CHAR);
	return;
}



/*
 * 'Split' originally by Gnort, God of Chaos.
 */
void do_split(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *gch;
	long members;
	long amount;
	long share;
	long extra;

	one_argument(argument, arg);

	if(arg[0] == '\0')
	{
		send_to_char("Split how much?\n\r", ch);
		return;
	}

	amount = atoi(arg);

	if(amount < 0)
	{
		send_to_char("Your group wouldn't like that.\n\r", ch);
		return;
	}

	if(amount == 0)
	{
		send_to_char("You hand out zero coins, but no one notices.\n\r", ch);
		return;
	}

	if(ch->gold < amount)
	{
		send_to_char("You don't have that much gold.\n\r", ch);
		return;
	}

	members = 0;
	for(gch = ch->in_room->people; gch != 0; gch = gch->next_in_room)
	{
		if(is_same_group(gch, ch))
			members++;
	}

	if(members < 2)
	{
		send_to_char("Just keep it all.\n\r", ch);
		return;
	}

	share = amount / members;
	extra = amount % members;

	if(share == 0)
	{
		send_to_char("Don't even bother, cheapskate.\n\r", ch);
		return;
	}

	ch->gold -= amount;
	ch->gold += share + extra;

	sprintf(buf, "You split %li gold coins.  Your share is %li gold coins.\n\r", amount, share + extra);
	send_to_char(buf, ch);

	sprintf(buf, "$n splits %li gold coins.  Your share is %li gold coins.", amount, share);

	for(gch = ch->in_room->people; gch != 0; gch = gch->next_in_room)
	{
		if(gch != ch && is_same_group(gch, ch))
		{
			act(buf, ch, 0, gch, TO_VICT);
			gch->gold += share;
		}
	}

	return;
}



void do_gtell(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *gch;

	if(argument[0] == '\0')
	{
		send_to_char("Tell your group what?\n\r", ch);
		return;
	}

	if(IS_SET(ch->act, PLR_NO_TELL))
	{
		send_to_char("Your message didn't get through!\n\r", ch);
		return;
	}

	/*
	 * Note use of send_to_char, so gtell works on sleepers.
	 */
	sprintf(buf, "%s tells the group '%s'.\n\r", ch->name, argument);
	for(gch = char_list; gch != 0; gch = gch->next)
	{
		if(is_same_group(gch, ch))
			send_to_char(buf, gch);
	}

	return;
}



/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 */
bool is_same_group(CHAR_DATA * ach, CHAR_DATA * bch)
{
	if(ach->leader != 0)
		ach = ach->leader;
	if(bch->leader != 0)
		bch = bch->leader;
	return ach == bch;
}

void do_changelight(CHAR_DATA * ch, char *argument)
{
	if(IS_SET(ch->in_room->room_flags, ROOM_DARK))
	{
		REMOVE_BIT(ch->in_room->room_flags, ROOM_DARK);
		act("The room is suddenly filled with light!", ch, 0, 0, TO_CHAR);
		act("The room is suddenly filled with light!", ch, 0, 0, TO_ROOM);
		return;
	}
	SET_BIT(ch->in_room->room_flags, ROOM_DARK);
	act("The lights in the room suddenly go out!", ch, 0, 0, TO_CHAR);
	act("The lights in the room suddenly go out!", ch, 0, 0, TO_ROOM);
	return;
}

void open_lift(CHAR_DATA * ch)
{
	ROOM_INDEX_DATA *location;
	long in_room;

	in_room = ch->in_room->vnum;
	location = get_room_index(in_room);

	if(is_open(ch))
		return;

	act("The doors open.", ch, 0, 0, TO_CHAR);
	act("The doors open.", ch, 0, 0, TO_ROOM);
	move_door(ch);
	if(is_open(ch))
		act("The doors close.", ch, 0, 0, TO_ROOM);
	if(!same_floor(ch, in_room))
		act("The lift judders suddenly.", ch, 0, 0, TO_ROOM);
	if(is_open(ch))
		act("The doors open.", ch, 0, 0, TO_ROOM);
	move_door(ch);
	open_door(ch, FALSE);
	char_from_room(ch);
	char_to_room(ch, location);
	open_door(ch, TRUE);
	move_door(ch);
	open_door(ch, TRUE);
	thru_door(ch, in_room);
	char_from_room(ch);
	char_to_room(ch, location);
	return;
}

void close_lift(CHAR_DATA * ch)
{
	ROOM_INDEX_DATA *location;
	long in_room;

	in_room = ch->in_room->vnum;
	location = get_room_index(in_room);

	if(!is_open(ch))
		return;
	act("The doors close.", ch, 0, 0, TO_CHAR);
	act("The doors close.", ch, 0, 0, TO_ROOM);
	open_door(ch, FALSE);
	move_door(ch);
	open_door(ch, FALSE);
	char_from_room(ch);
	char_to_room(ch, location);
	return;
}

void move_lift(CHAR_DATA * ch, long to_room)
{
	ROOM_INDEX_DATA *location;
	long in_room;

	in_room = ch->in_room->vnum;
	location = get_room_index(in_room);

	if(is_open(ch))
		act("The doors close.", ch, 0, 0, TO_CHAR);
	if(is_open(ch))
		act("The doors close.", ch, 0, 0, TO_ROOM);
	if(!same_floor(ch, to_room))
		act("The lift judders suddenly.", ch, 0, 0, TO_CHAR);
	if(!same_floor(ch, to_room))
		act("The lift judders suddenly.", ch, 0, 0, TO_ROOM);
	move_door(ch);
	open_door(ch, FALSE);
	char_from_room(ch);
	char_to_room(ch, location);
	open_door(ch, FALSE);
	thru_door(ch, to_room);
	return;
}

bool same_floor(CHAR_DATA * ch, long cmp_room)
{
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;

	for(obj = ch->in_room->contents; obj != 0; obj = obj_next)
	{
		obj_next = obj->next_content;
		if(obj->item_type != ITEM_PORTAL)
			continue;
		if(obj->pIndexData->vnum == 30001)
			continue;
		if(obj->value[0] == cmp_room)
			return TRUE;
		else
			return FALSE;
	}
	return FALSE;
}

bool is_open(CHAR_DATA * ch)
{
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;

	for(obj = ch->in_room->contents; obj != 0; obj = obj_next)
	{
		obj_next = obj->next_content;
		if(obj->item_type != ITEM_PORTAL)
			continue;
		if(obj->pIndexData->vnum == 30001)
			continue;
		if(obj->value[2] == 0)
			return TRUE;
		else
			return FALSE;
	}
	return FALSE;
}

void move_door(CHAR_DATA * ch)
{
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	ROOM_INDEX_DATA *pRoomIndex;

	for(obj = ch->in_room->contents; obj != 0; obj = obj_next)
	{
		obj_next = obj->next_content;
		if(obj->item_type != ITEM_PORTAL)
			continue;
		if(obj->pIndexData->vnum == 30001)
			continue;
		pRoomIndex = get_room_index(obj->value[0]);
		char_from_room(ch);
		char_to_room(ch, pRoomIndex);
		return;
	}
	return;
}

void thru_door(CHAR_DATA * ch, long doorexit)
{
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;

	for(obj = ch->in_room->contents; obj != 0; obj = obj_next)
	{
		obj_next = obj->next_content;
		if(obj->item_type != ITEM_PORTAL)
			continue;
		if(obj->pIndexData->vnum == 30001)
			continue;
		obj->value[0] = doorexit;
		return;
	}
	return;
}

void open_door(CHAR_DATA * ch, bool be_open)
{
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;

	for(obj = ch->in_room->contents; obj != 0; obj = obj_next)
	{
		obj_next = obj->next_content;
		if(obj->item_type != ITEM_PORTAL)
			continue;
		if(obj->pIndexData->vnum == 30001)
			continue;
		if(obj->value[2] == 0 && !be_open)
			obj->value[2] = 3;
		else if(obj->value[2] == 3 && be_open)
			obj->value[2] = 0;
		return;
	}
	return;
}

void do_speak(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(arg[0] == '\0')
	{
		if(ch->pcdata->language[0] == LANG_HACK)
			send_to_char("You are speaking like an idiot.\n\r", ch);
		else if(IS_SPEAKING(ch, LANG_DARK))
			send_to_char("You are speaking the Dark tongue.\n\r", ch);
		else if(IS_SPEAKING(ch, DIA_OLDE))
			send_to_char("You are speaking Olde Worlde.\n\r", ch);
		else if(IS_SPEAKING(ch, DIA_BAD))
			send_to_char("You are speaking very badly.\n\r", ch);
		else
			send_to_char("You are speaking the common language.\n\r", ch);
		strcpy(buf, "You can speak the following languages:");
		strcat(buf, " Common Olde Bad Hack");
		if(CAN_SPEAK(ch, LANG_DARK))
			strcat(buf, " Dark");
		strcat(buf, ".\n\r");
		send_to_char(buf, ch);
		return;
	}

	if(!str_cmp(arg, "dark"))
	{
		if(!CAN_SPEAK(ch, LANG_DARK))
		{
			send_to_char("You cannot speak the Dark tongue.\n\r", ch);
			return;
		}
		if(IS_SPEAKING(ch, LANG_DARK))
		{
			send_to_char("But you are already speaking the Dark tongue!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = LANG_DARK;
		send_to_char("Ok.\n\r", ch);
		return;
	}
	else if(!str_cmp(arg, "hack"))
	{
		ch->pcdata->language[0] = LANG_HACK;
		send_to_char("Ok.\n\r", ch);
		return;
	}
	else if(!str_cmp(arg, "common"))
	{
		if(ch->pcdata->language[0] == LANG_COMMON)
		{
			send_to_char("But you are already speaking the common tongue!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = LANG_COMMON;
		send_to_char("Ok.\n\r", ch);
		return;
	}
	else if(!str_cmp(arg, "olde"))
	{
		if(ch->pcdata->language[0] == DIA_OLDE)
		{
			send_to_char("But you are already speaking Olde Worlde!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = DIA_OLDE;
		send_to_char("Ok.\n\r", ch);
		return;
	}
	else if(!str_cmp(arg, "bad"))
	{
		if(ch->pcdata->language[0] == DIA_BAD)
		{
			send_to_char("But you are already speaking badly!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = DIA_BAD;
		send_to_char("Ok.\n\r", ch);
		return;
	}
/*
    else if (!str_cmp(arg,"common"))
    {
	if (ch->pcdata->language[0] < LANG_DARK)
	{
	    send_to_char("But you are already speaking the common tongue!\n\r",ch);
	    return;
	}
	ch->pcdata->language[0] = LANG_COMMON;
	if (CAN_SPEAK(ch,DIA_OLDE)) ch->pcdata->language[0] = DIA_OLDE;
	else if (CAN_SPEAK(ch,DIA_BAD)) ch->pcdata->language[0] = DIA_BAD;
	send_to_char("Ok.\n\r",ch);
	return;
    }
*/
	else
	{
		strcpy(buf, "You can speak the following languages:");
		strcat(buf, " Common Olde Bad Hack");
		if(CAN_SPEAK(ch, LANG_DARK))
			strcat(buf, " Dark");
		strcat(buf, ".\n\r");
		send_to_char(buf, ch);
		return;
	}

	return;
}

char *badlang(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char *pName;
	long iSyl;
	long length;

	struct spk_type
	{
		char *old;
		char *new;
	};

	static const struct spk_type spk_table[] = {
		{" ", " "},
		{"my name is", "i calls meself"},
		{"are not", "aint"},
		{"have", "'av"},
		{"my", "me"},
		{"hello", "oy"},
		{"hi ", "oy "},
		{"i am", "im"},
		{"it is", "tis"},
		{"the ", "da "},
		{" the", " da"},
		{"thank", "fank"},
		{"that", "dat"},
		{"with", "wiv"},
		{"they", "day"},
		{"this", "dis"},
		{"then", "den"},
		{"there", "ver"},
		{"their", "ver"},
		{"thing", "fing"},
		{"think", "fink"},
		{"was", "woz"},
		{"would", "wud"},
		{"what", "wot"},
		{"where", "weer"},
		{"when", "wen"},
		{"are", "is"},
		{"you", "ya"},
		{"your", "yer"},
		{"dead", "ded"},
		{"kill", "stomp"},
		{"food", "nosh"},
		{"blood", "blud"},
		{"vampire", "sucker"},
		{"kindred", "suckers"},
		{"fire", "hot"},
		{"dwarf", "stunty"},
		{"dwarves", "stunties"},
		{"goblin", "gobbo"},
		{"death", "def"},
		{"immune", "mune"},
		{"immunit", "munit"},
		{"childer", "nippers"},
		{"childe", "nipper"},
		{"child", "nipper"},
		{"tradition", "wassname"},
		{"generation", "batch"},
		{"founded", "made"},
		{"sired", "nipped"},
		{"sire", "dad"},
		{"lineage", "istory"},
		{"recognize", "dats"},
		{"recognize", "dats"},
		{"decapitate", "headchop"},
		{"decap", "chop"},
		{"recites", "sez"},
		{"recite", "sez"},
		{"a", "a"}, {"b", "b"}, {"c", "c"}, {"d", "d"},
		{"e", "e"}, {"f", "f"}, {"g", "g"}, {"h", "h"},
		{"i", "i"}, {"j", "j"}, {"k", "k"}, {"l", "l"},
		{"m", "m"}, {"n", "n"}, {"o", "o"}, {"p", "p"},
		{"q", "q"}, {"r", "r"}, {"s", "s"}, {"t", "t"},
		{"u", "u"}, {"v", "v"}, {"w", "w"}, {"x", "x"},
		{"y", "y"}, {"z", "z"}, {",", ","}, {".", "."},
		{";", ";"}, {":", ":"}, {"(", "("}, {")", ")"},
		{")", ")"}, {"-", "-"}, {"!", "!"}, {"?", "?"},
		{"1", "1"}, {"2", "2"}, {"3", "3"}, {"4", "4"},
		{"5", "5"}, {"6", "6"}, {"7", "7"}, {"8", "8"},
		{"9", "9"}, {"0", "0"}, {"%", "%"}, {"", ""}
	};

	buf[0] = '\0';

	if(argument[0] == '\0')
		return argument;

	for(pName = str_dup(argument); *pName != '\0'; pName += length)
	{
		for(iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0; iSyl++)
		{
			if(!str_prefix(spk_table[iSyl].old, pName))
			{
				strcat(buf, spk_table[iSyl].new);
				break;
			}
		}

		if(length == 0)
			length = 1;
	}

	argument[0] = '\0';
	strcpy(argument, buf);
	argument[0] = UPPER(argument[0]);
	return argument;
}

char *hacklang(CHAR_DATA * ch, char *argument)
{
	static char buf[MAX_STRING_LENGTH * 2];
	char *pName;
	long iSyl;
	long wSyl;
	long nocap = 0;
	long length;

	struct spk_type
	{
		char *old;
		char *new;
	};

	static const struct spk_type spk_table[] = {
		{"{", "{"}, {"\x92", "\x92"}, {"\\", "\\"},	//THESE MUST STAY HERE!
		// DO NOT PUT ANYTHING ABOVE THIS LINE!
		{" ", " "},
		{"\n\r", "\n\r"},
		{"kill ", "UnF j00 up "},
		{"insult", ":~("},
		{"cool ", "kewl "},
		{"I ", "3y3 "},
		{"lol", "I laF s0 h4rD I p3e my p4n+$!"},
		{"imm ", "l33t d00d "},
		{"imp ", "c0w hUmp3r "},
		{"Avatars", "li++le ba8ie$"},
		{"mortal ", "<ry1n n00b"},
		{"newbie", "l33t m0f0"},
		{"wb ", "why j00 come ba<k f0? "},
		{"hehe", "s0m30nE $laP m3!"},
		{"you're", "j00 aRe st00p|d and"},
		{"your", "y0Ur"},
		{"laf", "laF! mY n|ppl3s ar3 7iCkl|sh!!"},
		{"lmao", "s+0p it!  Y0u make me 9iggl3 l|k3 a sk00l gUrl!"},
		{"haha", "0mG!! MiLk sh00ten 0u+ta my n05e tH4t wa5 5o fun33@!"},
		{"neat", "l33t"},
		{"kiss", "far+"},
		{"distance", "faR 0ff pL4<e"},
		{"stance", "l33t p0si+|on"},
		{"good", "l33t"},
		{"going", "9o|n9"},
		{"coder", "h4><0r w4rr10r"},
		{"hacker", "h4><0r w4rr10r"},
		{"what ", "wTF "},
		{"who ", "Wtf "},
		{"a", "4"}, {"b", "8"}, {"c", "c"}, {"d", "d"},
		{"e", "3"}, {"f", "f"}, {"g", "9"}, {"h", "h"},
		{"i", "|"}, {"j", "j"}, {"k", "k"}, {"l", "l"},
		{"m", "m"}, {"n", "n"}, {"o", "0"}, {"p", "p"},
		{"q", "q"}, {"r", "r"}, {"s", "5"}, {"t", "7"},
		{"W", "W"}, {"R", "R"}, {"G", "G"}, {"C", "C"},
		{"M", "M"}, {"B", "B"}, {"Y", "Y"}, {"$", "$"},
		{"u", "u"}, {"v", "v"}, {"w", "w"}, {"x", "x"},
		{"y", "y"}, {"z", "z"}, {",", ","}, {".", "."},
		{"[", "["}, {"]", "]"}, {"'", "'"}, {"+", "+"},
		{"=", "="}, {"/", "/"}, {">", ">"}, {"<", "<"},
		{";", ";"}, {":", ":"}, {"(", "("}, {")", ")"},
		{")", ")"}, {"-", "-"}, {"!", "!"}, {"?", "?!?"},
		{"1", "1"}, {"2", "2"}, {"3", "3"}, {"4", "4"},
		{"5", "5"}, {"6", "6"}, {"7", "7"}, {"8", "8"},
		{"9", "9"}, {"0", "0"}, {"%", "%"}, {"", ""}
	};

	buf[0] = '\0';

	if(argument[0] == '\0')
		return argument;

	for(pName = argument; *pName != '\0'; pName += length)
	{
		for(iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0; iSyl++)
		{
			if(!str_prefix(spk_table[iSyl].old, pName))
			{
				if(iSyl == 0 || iSyl == 1 || iSyl == 3)
				{
					strcat(buf, spk_table[iSyl].old);
					for(wSyl = 0;; wSyl++)
					{
						if(!str_prefix(spk_table[wSyl].old, pName + 1))
						{
							strcat(buf, spk_table[wSyl].old);
							length = 2;
							nocap = 1;
							break;
						}
						break;
					}
					break;
				}

				if(number_range(1, 100) < 50 && length < 2)
					strcat(buf, spk_table[iSyl].old);
				else
				{
					if(nocap)
						strcat(buf, spk_table[iSyl].old);
					else if(number_range(1, 100) < 30)
						strcat(buf, capitalize(spk_table[iSyl].new));
					else
						strcat(buf, spk_table[iSyl].new);
					nocap = 0;
				}
				break;
			}
		}

		if(length == 0)
			length = 1;
	}
	if(isalnum(buf[strlen(buf) - 1]))
	{
		if(number_percent() > 30)
			for(length = 0; length < number_range(1, 10); length++)
				strcat(buf, "!");
	}

	return buf;
}

char *oldelang(CHAR_DATA * ch, char *argument)
{
	static char buf[MAX_STRING_LENGTH * 2];
	char *pName;
	long iSyl;
	long length;

	struct spk_type
	{
		char *old;
		char *new;
	};

	static const struct spk_type spk_table[] = {
		{" ", " "},
		{"have", "hath"},
		{"hello", "hail"},
		{"hi ", "hail "},
		{" hi", " hail"},
		{"are", "art"},
		{"your", "thy"},
		{"you", "thou"},
		{"i think", "methinks"},
		{"do ", "doth "},
		{" do", " doth"},
		{"it was", "twas"},
		{"before", "ere"},
		{"his", "$s"},
		{"a", "a"}, {"b", "b"}, {"c", "c"}, {"d", "d"},
		{"e", "e"}, {"f", "f"}, {"g", "g"}, {"h", "h"},
		{"i", "i"}, {"j", "j"}, {"k", "k"}, {"l", "l"},
		{"m", "m"}, {"n", "n"}, {"o", "o"}, {"p", "p"},
		{"q", "q"}, {"r", "r"}, {"s", "s"}, {"t", "t"},
		{"u", "u"}, {"v", "v"}, {"w", "w"}, {"x", "x"},
		{"y", "y"}, {"z", "z"}, {",", ","}, {".", "."},
		{";", ";"}, {":", ":"}, {"(", "("}, {")", ")"},
		{")", ")"}, {"-", "-"}, {"!", "!"}, {"?", "?"},
		{"1", "1"}, {"2", "2"}, {"3", "3"}, {"4", "4"},
		{"5", "5"}, {"6", "6"}, {"7", "7"}, {"8", "8"},
		{"9", "9"}, {"0", "0"}, {"%", "%"}, {"", ""}
	};

	buf[0] = '\0';

	if(argument[0] == '\0')
		return argument;

	for(pName = argument; *pName != '\0'; pName += length)
	{
		for(iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0; iSyl++)
		{
			if(!str_prefix(spk_table[iSyl].old, pName))
			{
				strcat(buf, spk_table[iSyl].new);
				break;
			}
		}

		if(length == 0)
			length = 1;
	}

	return buf;
}

char *darktongue(CHAR_DATA * ch, char *argument)
{
	static char buf[MAX_STRING_LENGTH * 2];
	char *pName;
	long iSyl;
	long length;

	struct spk_type
	{
		char *old;
		char *new;
	};

	static const struct spk_type spk_table[] = {
		{" ", " "},
		{"a", "i"}, {"b", "t"}, {"c", "x"}, {"d", "j"},
		{"e", "u"}, {"f", "d"}, {"g", "k"}, {"h", "z"},
		{"i", "o"}, {"j", "s"}, {"k", "f"}, {"l", "h"},
		{"m", "b"}, {"n", "c"}, {"o", "e"}, {"p", "r"},
		{"q", "l"}, {"r", "v"}, {"s", "w"}, {"t", "q"},
		{"u", "a"}, {"v", "n"}, {"w", "y"}, {"x", "g"},
		{"y", "m"}, {"z", "p"}, {",", ","}, {".", "."},
		{";", ";"}, {":", ":"}, {"(", "("}, {")", ")"},
		{")", ")"}, {"-", "-"}, {"!", "!"}, {"?", "?"},
		{"", ""}
	};

	buf[0] = '\0';

	if(argument[0] == '\0')
		return argument;

	for(pName = str_dup(argument); *pName != '\0'; pName += length)
	{
		for(iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0; iSyl++)
		{
			if(!str_prefix(spk_table[iSyl].old, pName))
			{
				strcat(buf, spk_table[iSyl].new);
				break;
			}
		}

		if(length == 0)
			length = 1;
	}

	return buf;
}

char *drunktalk(CHAR_DATA * ch, char *argument)
{
	static char buf[MAX_STRING_LENGTH * 3];
	char *pName;
	long iSyl;
	long length;
	long loop;

	struct spk_type
	{
		char *old;
		char *new;
	};

	static const struct spk_type spk_table[] = {
		{" ", " "},
		{"is", "ish"},
		{"a", "a"}, {"b", "b"}, {"c", "c"}, {"d", "d"},
		{"e", "e"}, {"f", "f"}, {"g", "g"}, {"h", "h"},
		{"i", "i"}, {"j", "j"}, {"k", "k"}, {"l", "l"},
		{"m", "m"}, {"n", "n"}, {"o", "o"}, {"p", "p"},
		{"q", "q"}, {"r", "r"}, {"s", "s"}, {"t", "t"},
		{"u", "u"}, {"v", "v"}, {"w", "w"}, {"x", "x"},
		{"y", "y"}, {"z", "z"}, {",", ","}, {".", "."},
		{";", ";"}, {":", ":"}, {"(", "("}, {")", ")"},
		{")", ")"}, {"-", "-"}, {"!", "!"}, {"?", "?"},
		{"1", "1"}, {"2", "2"}, {"3", "3"}, {"4", "4"},
		{"5", "5"}, {"6", "6"}, {"7", "7"}, {"8", "8"},
		{"9", "9"}, {"0", "0"}, {"%", "%"}, {"", ""}
	};

	buf[0] = '\0';

	if(argument[0] == '\0')
		return argument;

	for(pName = str_dup(argument); *pName != '\0'; pName += length)
	{
		for(iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0; iSyl++)
		{
			if(!str_prefix(spk_table[iSyl].old, pName))
			{
				strcat(buf, spk_table[iSyl].new);
				if(number_range(1, 5) == 1 && str_cmp(spk_table[iSyl].new, " "))
					strcat(buf, spk_table[iSyl].new);
				else if(!str_cmp(spk_table[iSyl].new, " "))
				{
					if(number_range(1, 5) == 1 && strlen(buf) < MAX_INPUT_LENGTH)
						strcat(buf, "*hic* ");
				}
				break;
			}
		}

		if(length == 0)
			length = 1;
	}

	for(loop = 1; loop < (long) strlen(buf); loop++)
	{
		if(number_range(1, 2) == 1)
			buf[loop] = UPPER(buf[loop]);
	}

	return buf;
}

char *socialc(CHAR_DATA * ch, char *argument, char *you, char *them)
{
	static char buf[MAX_STRING_LENGTH * 2];
	char *pName;
	long iSyl;
	long length;

	struct spk_type
	{
		char *old;
		char *new;
	};

	static const struct spk_type spk_table[] = {
		{" ", " "},
		{"you are", "$E is"},
		{"you.", "$M."},
		{"you,", "$M,"},
		{"you ", "$M "},
		{" you", " $M"},
		{"your ", "$S "},
		{" your", " $S"},
		{"yours.", "theirs."},
		{"yours,", "theirs,"},
		{"yours ", "theirs "},
		{" yours", " theirs"},
		{"begins", "begin"},
		{"caresses", "caress"},
		{"gives", "give"},
		{"glares", "glare"},
		{"grins", "grin"},
		{"licks", "lick"},
		{"looks", "look"},
		{"loves", "love"},
		{"plunges", "plunge"},
		{"presses", "press"},
		{"pulls", "pull"},
		{"runs", "run"},
		{"slaps", "slap"},
		{"slides", "slide"},
		{"smashes", "smash"},
		{"squeezes", "squeeze"},
		{"stares", "stare"},
		{"sticks", "stick"},
		{"strokes", "stroke"},
		{"tugs", "tug"},
		{"thinks", "think"},
		{"thrusts", "thrust"},
		{"whistles", "whistle"},
		{"wraps", "wrap"},
		{"winks", "wink"},
		{"wishes", "wish"},
		{" winks", " wink"},
		{" his", " your"},
		{"his ", "your "},
		{" her", " your"},
		{"her ", "your "},
		{" him", " your"},
		{"him ", "your "},
		{"the", "the"},
		{" he", " you"},
		{"he ", "you "},
		{" she", " you"},
		{"she ", "you "},
		{"a", "a"}, {"b", "b"}, {"c", "c"}, {"d", "d"},
		{"e", "e"}, {"f", "f"}, {"g", "g"}, {"h", "h"},
		{"i", "i"}, {"j", "j"}, {"k", "k"}, {"l", "l"},
		{"m", "m"}, {"n", "n"}, {"o", "o"}, {"p", "p"},
		{"q", "q"}, {"r", "r"}, {"s", "s"}, {"t", "t"},
		{"u", "u"}, {"v", "v"}, {"w", "w"}, {"x", "x"},
		{"y", "y"}, {"z", "z"}, {",", ","}, {".", "."},
		{";", ";"}, {":", ":"}, {"(", "("}, {")", ")"},
		{")", ")"}, {"-", "-"}, {"!", "!"}, {"?", "?"},
		{"1", "1"}, {"2", "2"}, {"3", "3"}, {"4", "4"},
		{"5", "5"}, {"6", "6"}, {"7", "7"}, {"8", "8"},
		{"9", "9"}, {"0", "0"}, {"%", "%"}, {"", ""}
	};

	buf[0] = '\0';

	if(argument[0] == '\0')
		return argument;

	for(pName = str_dup(argument); *pName != '\0'; pName += length)
	{
		for(iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0; iSyl++)
		{
			if(!str_prefix(spk_table[iSyl].old, pName))
			{
				strcat(buf, spk_table[iSyl].new);
				break;
			}
		}

		if(length == 0)
			length = 1;
	}

	return buf;
}

char *socialv(CHAR_DATA * ch, char *argument, char *you, char *them)
{
	static char buf[MAX_STRING_LENGTH * 2];
	char *pName;
	long iSyl;
	long length;

	struct spk_type
	{
		char *old;
		char *new;
	};

	static const struct spk_type spk_table[] = {
		{" ", " "},
		{" his", " $s"},
		{"his ", "$s "},
		{" her", " $s"},
		{"her ", "$s "},
		{" him", " $m"},
		{"him ", "$m "},
		{" he", " $e"},
		{"he ", "$e "},
		{" she", " $e"},
		{"she ", "$e "},
		{"a", "a"}, {"b", "b"}, {"c", "c"}, {"d", "d"},
		{"e", "e"}, {"f", "f"}, {"g", "g"}, {"h", "h"},
		{"i", "i"}, {"j", "j"}, {"k", "k"}, {"l", "l"},
		{"m", "m"}, {"n", "n"}, {"o", "o"}, {"p", "p"},
		{"q", "q"}, {"r", "r"}, {"s", "s"}, {"t", "t"},
		{"u", "u"}, {"v", "v"}, {"w", "w"}, {"x", "x"},
		{"y", "y"}, {"z", "z"}, {",", ","}, {".", "."},
		{";", ";"}, {":", ":"}, {"(", "("}, {")", ")"},
		{")", ")"}, {"-", "-"}, {"!", "!"}, {"?", "?"},
		{"1", "1"}, {"2", "2"}, {"3", "3"}, {"4", "4"},
		{"5", "5"}, {"6", "6"}, {"7", "7"}, {"8", "8"},
		{"9", "9"}, {"0", "0"}, {"%", "%"}, {"", ""}
	};

	buf[0] = '\0';

	if(argument[0] == '\0')
		return argument;

	for(pName = str_dup(argument); *pName != '\0'; pName += length)
	{
		for(iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0; iSyl++)
		{
			if(!str_prefix(spk_table[iSyl].old, pName))
			{
				strcat(buf, spk_table[iSyl].new);
				break;
			}
		}

		if(length == 0)
			length = 1;
	}

	return buf;
}

char *socialn(CHAR_DATA * ch, char *argument, char *you, char *them)
{
	static char buf[MAX_STRING_LENGTH * 2];
	char *pName;
	long iSyl;
	long length;

	struct spk_type
	{
		char *old;
		char *new;
	};

	static const struct spk_type spk_table[] = {
		{" ", " "},
		{"you are", "$N is"},
		{"you.", "$N."},
		{"you,", "$N,"},
		{"you ", "$N "},
		{" you", " $N"},
		{"your.", "$N's."},
		{"your,", "$N's,"},
		{"your ", "$N's "},
		{" your", " $N's"},
		{"yourself", "$Mself"},
		{" his", " $s"},
		{"his ", "$s "},
		{" her", " $s"},
		{"her ", "$s "},
		{" him", " $m"},
		{"him ", "$m "},
		{" he", " $e"},
		{"he ", "$e "},
		{" she", " $e"},
		{"she ", "$e "},
		{"a", "a"}, {"b", "b"}, {"c", "c"}, {"d", "d"},
		{"e", "e"}, {"f", "f"}, {"g", "g"}, {"h", "h"},
		{"i", "i"}, {"j", "j"}, {"k", "k"}, {"l", "l"},
		{"m", "m"}, {"n", "n"}, {"o", "o"}, {"p", "p"},
		{"q", "q"}, {"r", "r"}, {"s", "s"}, {"t", "t"},
		{"u", "u"}, {"v", "v"}, {"w", "w"}, {"x", "x"},
		{"y", "y"}, {"z", "z"}, {",", ","}, {".", "."},
		{";", ";"}, {":", ":"}, {"(", "("}, {")", ")"},
		{")", ")"}, {"-", "-"}, {"!", "!"}, {"?", "?"},
		{"1", "1"}, {"2", "2"}, {"3", "3"}, {"4", "4"},
		{"5", "5"}, {"6", "6"}, {"7", "7"}, {"8", "8"},
		{"9", "9"}, {"0", "0"}, {"%", "%"}, {"", ""}
	};

	buf[0] = '\0';

	if(argument[0] == '\0')
		return argument;

	for(pName = str_dup(argument); *pName != '\0'; pName += length)
	{
		for(iSyl = 0; (length = strlen(spk_table[iSyl].old)) != 0; iSyl++)
		{
			if(!str_prefix(spk_table[iSyl].old, pName))
			{
				strcat(buf, spk_table[iSyl].new);
				break;
			}
		}

		if(length == 0)
			length = 1;
	}

	return buf;
}


// THIS IS THE END OF THE FILE THANKS
