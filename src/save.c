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
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "merc.h"

#if !defined(macintosh)
extern int _filbuf args((FILE *));
#endif



/*
 * Array of containers read for proper re-nesting of objects.
 */
#define MAX_NEST	100
static OBJ_DATA *rgObjNest[MAX_NEST];



/*
 * Local functions.
 */
void fwrite_char args((CHAR_DATA * ch, FILE * fp));
void fwrite_obj args((CHAR_DATA * ch, OBJ_DATA * obj, FILE * fp, long iNest));
bool fread_char args((CHAR_DATA * ch, FILE * fp));
bool fread_obj args((CHAR_DATA * ch, FILE * fp));
void save_char_obj_finger args((CHAR_DATA * ch));

void convert_old_new args((CHAR_DATA * ch));

/*
 * Local variables.
 */
long save_in_binary = 0;

bool check_pfile(char *name)
{
	FILE *fp;
	char strsave[MAX_STRING_LENGTH];
	char k;
	char *word;
	char realname[200];
	long i;

	if(!name || name[0] == '\0')
		return FALSE;

	for(i = 0; i < (long) strlen(name); i++)
	{
		if(i == 0)
		{
			realname[0] = UPPER(name[0]);
		}
		else
		{
			realname[i] = LOWER(name[i]);
		}
	}

	realname[i] = '\0';

	fclose(fpReserve);
	sprintf(strsave, "%s%s", PLAYER_DIR, realname);

	if((fp = fopen(strsave, "r")) == 0)	// new player
		return TRUE;

	while((k = getc(fp)) != EOF)
	{
		if(k == '#')
		{
			word = fread_word(fp);

			if(!str_cmp(word, "end"))
			{
				fclose(fp);
				fpReserve = fopen(NULL_FILE, "r");
				return TRUE;
			}
		}
	}
	fclose(fp);
	fpReserve = fopen(NULL_FILE, "r");
	return FALSE;
}

bool bad_fword(char *word)
{
	if(word == 0)
		return TRUE;
	return FALSE;
}


/*
 * Save a character and inventory.
 * Would be cool to save NPC's too for quest purposes,
 *   some of the infrastructure is provided.
 */
void save_char_obj(CHAR_DATA * ch)
{
	char strsave[MAX_STRING_LENGTH];
	FILE *fp;

	if(IS_NPC(ch) || ch->level < 2)
		return;

	if(ch->desc != 0 && ch->desc->connected != CON_PLAYING)
		return;

	if(ch->desc != 0 && ch->desc->original != 0)
		ch = ch->desc->original;
/*
    save_char_obj_backup( ch );
*/
	ch->save_time = current_time;
	fclose(fpReserve);

	if(save_in_binary)
	{
		sprintf(strsave, "%sbinary/%s", PLAYER_DIR, capitalize(ch->name));
		if((fp = fopen(strsave, "wb")) == 0)
		{
			bug("Save_char_obj: fopen binary", 0);
			perror(strsave);
		}
		else
		{
			fwrite(ch, sizeof(*ch), 1, fp);
		}
	}
	else
	{
		sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(ch->name));

		if((fp = fopen(strsave, "w")) == 0)
		{
			bug("Save_char_obj: fopen", 0);
			perror(strsave);
		}
		else
		{
			fwrite_char(ch, fp);

			if(ch->carrying != 0)
				fwrite_obj(ch, ch->carrying, fp, 0);
			fprintf(fp, "#END\n");
		}
	}
	fflush(fp);
	fclose(fp);
	fpReserve = fopen(NULL_FILE, "r");
	save_char_obj_finger(ch);
	return;
}

void save_char_obj_backup(CHAR_DATA * ch)
{
	char chlevel[15];
	char buf[MAX_INPUT_LENGTH];
	char strsave[MAX_STRING_LENGTH];
	FILE *fp;

	if(IS_NPC(ch) || ch->level < 2)
		return;

	if(ch->desc != 0 && ch->desc->original != 0)
		ch = ch->desc->original;

	ch->save_time = current_time;
	fclose(fpReserve);
	sprintf(strsave, "%sstore/%s", PLAYER_DIR, capitalize(ch->name));
	if((fp = fopen(strsave, "w")) == 0)
	{
		bug("Save_char_obj: fopen", 0);
		perror(strsave);
	}
	else
	{
		fwrite_char(ch, fp);

		if(ch->carrying != 0)
			fwrite_obj(ch, ch->carrying, fp, 0);
		fprintf(fp, "#END\n");
		if(ch->level >= 11)
			sprintf(chlevel, "<Implementor>");
		else if(ch->level == 10)
			sprintf(chlevel, "<High Judge>");
		else if(ch->level == 9)
			sprintf(chlevel, "<Judge>");
		else if(ch->level == 8)
			sprintf(chlevel, "<Enforcer>");
		else if(ch->level == 7)
			sprintf(chlevel, "<Quest Maker>");
		else if(ch->level == 6)
			sprintf(chlevel, "<Builder>");
		else if(ch->level == 5)
			sprintf(chlevel, "<Avatar>");
		else if(ch->level == 4)
			sprintf(chlevel, "<Avatar>");
		else if(ch->level == 3)
			sprintf(chlevel, "<Avatar>");
		else
			sprintf(chlevel, "<Mortal>");
		if(strlen(ch->lasttime) > 1)
			sprintf(buf, "%s Last logged in on %s.\n\r", chlevel, ch->lasttime);
		else
			sprintf(buf, "%s New player logged in on %s.\n\r", chlevel, ch->createtime);
		fprintf(fp, buf);
	}
	fflush(fp);
	fclose(fp);
	fpReserve = fopen(NULL_FILE, "r");
	return;
}

void save_char_obj_finger(CHAR_DATA * ch)
{
	char strsave[MAX_STRING_LENGTH];
	FILE *fp;

	if(IS_NPC(ch) || ch->level < 2)
		return;

	if(ch->desc != 0 && ch->desc->original != 0)
		ch = ch->desc->original;

	ch->save_time = current_time;
	fclose(fpReserve);
	sprintf(strsave, "%sbackup/%s", PLAYER_DIR, capitalize(ch->name));
	if((fp = fopen(strsave, "w")) == 0)
	{
		bug("Save_char_obj: fopen", 0);
		perror(strsave);
	}
	else
	{
		fprintf(fp, "%s~\n", ch->name);
		fprintf(fp, "%s~\n", ch->pcdata->title);
		fprintf(fp, "%s~\n", ch->lasthost);
		fprintf(fp, "%s~\n", ch->lasttime);
		fprintf(fp, "%li\n", ch->extra);
		fprintf(fp, "%li\n", ch->sex);
		fprintf(fp, "%s~\n", ch->pcdata->conception);
		fprintf(fp, "%s~\n", ch->createtime);
		fprintf(fp, "%li\n", ch->level);
		fprintf(fp, "%li\n", ch->pcdata->status);
		fprintf(fp, "%li\n", ch->played + (long) (current_time - ch->logon));
		fprintf(fp, "%s~\n", ch->pcdata->marriage);
		fprintf(fp, "%li\n", ch->pkill);
		fprintf(fp, "%li\n", ch->pdeath);
		fprintf(fp, "%li\n", ch->mkill);
		fprintf(fp, "%li\n", ch->mdeath);
//      fprintf( fp, "%li\n",    ch->tlw );
//      fprintf( fp, "%li\n",    ch->tll );
		fprintf(fp, "%s~\n", ch->pcdata->email);
		fprintf(fp, "%s~\n", ch->pcdata->autostance);
	}
	fflush(fp);
	fclose(fp);
	fpReserve = fopen(NULL_FILE, "r");
	return;
}




/*
 * Write the char.
 */
void fwrite_char(CHAR_DATA * ch, FILE * fp)
{
	AFFECT_DATA *paf;
	long sn;

	fprintf(fp, "#%s\n", IS_NPC(ch) ? "MOB" : "PLAYERS");

	fprintf(fp, "Name         %s~\n", ch->name);
	fprintf(fp, "ShortDescr   %s~\n", ch->short_descr);
	fprintf(fp, "LongDescr    %s~\n", ch->long_descr);
	fprintf(fp, "ObjDesc      %s~\n", ch->objdesc);
	fprintf(fp, "Description  %s~\n", ch->description);
	fprintf(fp, "Lord         %s~\n", ch->lord);
	fprintf(fp, "Clan         %s~\n", ch->clan);
	fprintf(fp, "Class        %li\n", ch->pcdata->class);
	fprintf(fp, "Morph        %s~\n", ch->morph);
	fprintf(fp, "Createtime   %s~\n", ch->createtime);
	fprintf(fp, "Lasttime     %s~\n", ch->lasttime);
	fprintf(fp, "Lasthost     %s~\n", ch->lasthost);
	fprintf(fp, "Poweraction  %s~\n", ch->poweraction);
	fprintf(fp, "Powertype    %s~\n", ch->powertype);
	fprintf(fp, "Prompt       %s~\n", ch->prompt);
	fprintf(fp, "Cprompt      %s~\n", ch->cprompt);
	fprintf(fp, "Sex          %li\n", ch->sex);
	fprintf(fp, "Race         %li\n", ch->pcdata->status);
	fprintf(fp, "Immune       %li\n", ch->immune);
	fprintf(fp, "Polyaff      %li\n", ch->polyaff);
	fprintf(fp, "Souls	      %li\n", ch->pcdata->souls);
	fprintf(fp, "Itemaffect   %li\n", ch->itemaffect);
	fprintf(fp, "Form         %li\n", ch->form);
	fprintf(fp, "Beast        %li\n", ch->beast);
	fprintf(fp, "Spectype     %li\n", ch->spectype);
	fprintf(fp, "Specpower    %li\n", ch->specpower);
	fprintf(fp, "Home         %li\n", ch->home);
	//  fprintf( fp, "Tw/l               %li %li\n", ch->tlw,ch->tll                );
	fprintf(fp, "Level        %li\n", ch->level);
	fprintf(fp, "Trust        %li\n", ch->trust);
	fprintf(fp, "Paradox      %li %li %li\n", ch->paradox[0], ch->paradox[1], ch->paradox[2]);
	fprintf(fp, "Played       %li\n", ch->played + (long) (current_time - ch->logon));
	fprintf(fp, "Room         %li\n",
		(ch->in_room == get_room_index(ROOM_VNUM_LIMBO)
		 && ch->was_in_room != 0) ? ch->was_in_room->vnum : ch->in_room->vnum);

	fprintf(fp, "PkPdMkMd     %li %li %li %li\n", ch->pkill, ch->pdeath, ch->mkill, ch->mdeath);

	fprintf(fp, "Weapons      %li %li %li %li %li %li %li %li %li %li %li %li %li\n",
		ch->wpn[0], ch->wpn[1], ch->wpn[2], ch->wpn[3], ch->wpn[4],
		ch->wpn[5], ch->wpn[6], ch->wpn[7], ch->wpn[8], ch->wpn[9], ch->wpn[10], ch->wpn[11], ch->wpn[12]);
	fprintf(fp, "Spells       %li %li %li %li %li\n", ch->spl[0], ch->spl[1], ch->spl[2], ch->spl[3], ch->spl[4]);
	fprintf(fp, "Combat       %li %li %li %li %li %li %li %li\n",
		ch->cmbt[0], ch->cmbt[1], ch->cmbt[2], ch->cmbt[3], ch->cmbt[4], ch->cmbt[5], ch->cmbt[6], ch->cmbt[7]);
	fprintf(fp, "Stance       %li %li %li %li %li %li %li %li %li %li %li\n",
		ch->stance[0], ch->stance[1], ch->stance[2], ch->stance[3],
		ch->stance[4], ch->stance[5], ch->stance[6], ch->stance[7], ch->stance[8], ch->stance[9], ch->stance[10]);
	fprintf(fp, "Locationhp   %li %li %li %li %li %li %li\n",
		ch->loc_hp[0], ch->loc_hp[1], ch->loc_hp[2], ch->loc_hp[3], ch->loc_hp[4], ch->loc_hp[5], ch->loc_hp[6]);
	fprintf(fp, "HpManaMove   %li %li %li %li %li %li\n",
		ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move, ch->max_move);
	fprintf(fp, "Gold         %li\n", ch->gold);
	fprintf(fp, "Exp          %li\n", ch->exp);
	fprintf(fp, "Act          %li\n", ch->act);
	fprintf(fp, "Special      %li\n", ch->special);
	fprintf(fp, "Newbits      %li\n", ch->newbits);
	fprintf(fp, "Extra        %li\n", ch->extra);
	fprintf(fp, "AffectedBy   %li\n", ch->affected_by);
	fprintf(fp, "Swordtech    %li\n", ch->swordtech);
	fprintf(fp, "Cols         %li\n", ch->cols);
	fprintf(fp, "Rows         %li\n", ch->rows);

	fprintf(fp, "Aff2         %li\n", ch->flag2);
	fprintf(fp, "Spheres      %li %li %li %li %li %li %li %li %li\n",
		ch->spheres[0], ch->spheres[1], ch->spheres[2], ch->spheres[3],
		ch->spheres[4], ch->spheres[5], ch->spheres[6], ch->spheres[7], ch->spheres[8]);
	fprintf(fp, "Mflags	%li\n", ch->mflags);
	fprintf(fp, "Quint	     %li %li %li\n", ch->quint[0], ch->quint[1], ch->quint[2]);
	fprintf(fp, "Awins		%li\n", ch->awins);
	fprintf(fp, "Alosses	%li\n", ch->alosses);
	fprintf(fp, "Imms	     %li %li %li\n", ch->imms[0], ch->imms[1], ch->imms[2]);	/* Bug fix from Alander */
	fprintf(fp, "Position     %li\n", ch->position == POS_FIGHTING ? POS_STANDING : ch->position);

	fprintf(fp, "Practice     %li\n", ch->practice);
	fprintf(fp, "SavingThrow  %li\n", ch->saving_throw);
	fprintf(fp, "Alignment    %li\n", ch->alignment);
	fprintf(fp, "Hitroll      %li\n", ch->hitroll);
	fprintf(fp, "Damroll      %li\n", ch->damroll);
	fprintf(fp, "Armor        %li\n", ch->armor);
	fprintf(fp, "Wimpy        %li\n", ch->wimpy);
	fprintf(fp, "Deaf         %li\n", ch->deaf);
	fprintf(fp, "Mapbits      %li\n", ch->mapbits);
	if(IS_NPC(ch))
	{
		fprintf(fp, "Vnum         %li\n", ch->pIndexData->vnum);
	}
	else
	{
		fprintf(fp, "Surveying    %li\n", ch->pcdata->surveying);
		fprintf(fp, "Safetimer    %li\n", ch->pcdata->safe_timer);
		fprintf(fp, "Safewait     %li\n", ch->pcdata->safe_wait);
		fprintf(fp, "Password     %s~\n", ch->pcdata->pwd);
		fprintf(fp, "Bamfin       %s~\n", ch->pcdata->bamfin);
		fprintf(fp, "Bamfout      %s~\n", ch->pcdata->bamfout);
		fprintf(fp, "Bounty	   %li\n", ch->bounty);
		fprintf(fp, "Title        %s~\n", ch->pcdata->title);
		fprintf(fp, "Conception   %s~\n", ch->pcdata->conception);
		fprintf(fp, "Parents      %s~\n", ch->pcdata->parents);
		fprintf(fp, "Changesplace %li\n", ch->pcdata->changesplace);
		fprintf(fp, "Kingdom      %li\n", ch->pcdata->kingdom);
		fprintf(fp, "Krank        %s~\n", ch->pcdata->krank);
		fprintf(fp, "Login        %s~\n", ch->pcdata->login);
		fprintf(fp, "Logout       %s~\n", ch->pcdata->logout);
		fprintf(fp, "Enter        %s~\n", ch->pcdata->enter);
		fprintf(fp, "Exit         %s~\n", ch->pcdata->exit);
		fprintf(fp, "Cparents     %s~\n", ch->pcdata->cparents);
		fprintf(fp, "Marriage     %s~\n", ch->pcdata->marriage);
		fprintf(fp, "Email        %s~\n", ch->pcdata->email);
		fprintf(fp, "Autostance   %s~\n", ch->pcdata->autostance);
		fprintf(fp, "AttrPerm     %li %li %li %li %li\n",
			ch->pcdata->perm_str,
			ch->pcdata->perm_int, ch->pcdata->perm_wis, ch->pcdata->perm_dex, ch->pcdata->perm_con);

		fprintf(fp, "AttrMod      %li %li %li %li %li\n",
			ch->pcdata->mod_str,
			ch->pcdata->mod_int, ch->pcdata->mod_wis, ch->pcdata->mod_dex, ch->pcdata->mod_con);

		fprintf(fp, "Quest        %li\n", ch->pcdata->quest);
		fprintf(fp, "Wolf         %li\n", ch->pcdata->stats[UNI_RAGE]);
		fprintf(fp, "Rank         %li\n", ch->pcdata->rank);
		fprintf(fp, "Language     %li %li\n", ch->pcdata->language[0], ch->pcdata->language[1]);
		fprintf(fp, "Stage        %li %li %li\n", ch->pcdata->stage[0], ch->pcdata->stage[1], ch->pcdata->stage[2]);
		fprintf(fp, "Score        %li %li %li %li %li %li\n",
			ch->pcdata->score[0],
			ch->pcdata->score[1],
			ch->pcdata->score[2], ch->pcdata->score[3], ch->pcdata->score[4], ch->pcdata->score[5]);
		fprintf(fp, "Genes        %li %li %li %li %li %li %li %li %li %li\n",
			ch->pcdata->genes[0],
			ch->pcdata->genes[1],
			ch->pcdata->genes[2],
			ch->pcdata->genes[3],
			ch->pcdata->genes[4],
			ch->pcdata->genes[5],
			ch->pcdata->genes[6], ch->pcdata->genes[7], ch->pcdata->genes[8], ch->pcdata->genes[9]);
		fprintf(fp, "Power        ");
		for(sn = 0; sn < 20; sn++)
		{
			fprintf(fp, "%li ", ch->pcdata->powers[sn]);
		}
		fprintf(fp, "\n");

		fprintf(fp, "Stats        ");
		for(sn = 0; sn < 12; sn++)
		{
			fprintf(fp, "%li ", ch->pcdata->stats[sn]);
		}
		fprintf(fp, "\n");

		fprintf(fp, "FakeCon      %li %li %li %li %li %li %li %li\n",
			ch->pcdata->fake_skill,
			ch->pcdata->fake_stance,
			ch->pcdata->fake_hit,
			ch->pcdata->fake_dam,
			ch->pcdata->fake_ac, ch->pcdata->fake_hp, ch->pcdata->fake_mana, ch->pcdata->fake_move);

		if(ch->pcdata->obj_vnum != 0)
			fprintf(fp, "Objvnum      %li\n", ch->pcdata->obj_vnum);

		fprintf(fp, "Condition    %li %li %li\n",
			ch->pcdata->condition[0], ch->pcdata->condition[1], ch->pcdata->condition[2]);

		fprintf(fp, "StatAbility  %li %li %li %li\n",
			ch->pcdata->stat_ability[0],
			ch->pcdata->stat_ability[1], ch->pcdata->stat_ability[2], ch->pcdata->stat_ability[3]);

		fprintf(fp, "StatAmount   %li %li %li %li\n",
			ch->pcdata->stat_amount[0],
			ch->pcdata->stat_amount[1], ch->pcdata->stat_amount[2], ch->pcdata->stat_amount[3]);

		fprintf(fp, "StatDuration %li %li %li %li\n",
			ch->pcdata->stat_duration[0],
			ch->pcdata->stat_duration[1], ch->pcdata->stat_duration[2], ch->pcdata->stat_duration[3]);

		fprintf(fp, "Exhaustion   %li\n", ch->pcdata->exhaustion);

		for(sn = 0; sn < MAX_SKILL; sn++)
		{
			if(skill_table[sn].name != 0 && ch->pcdata->learned[sn] > 0)
			{
				fprintf(fp, "Skill        %li '%s'\n", ch->pcdata->learned[sn], skill_table[sn].name);
			}
		}
	}

	for(paf = ch->affected; paf != 0; paf = paf->next)
	{
		/* Thx Alander */
		if(paf->type < 0 || paf->type >= MAX_SKILL)
			continue;

		fprintf(fp, "AffectData   '%s' %3li %3li %3li %10li\n",
			skill_table[paf->type].name, paf->duration, paf->modifier, paf->location, paf->bitvector);
	}

	fprintf(fp, "End\n\n");
	return;
}



/*
 * Write an object and its contents.
 */
void fwrite_obj(CHAR_DATA * ch, OBJ_DATA * obj, FILE * fp, long iNest)
{
	EXTRA_DESCR_DATA *ed;
	AFFECT_DATA *paf;

/*
    long vnum = obj->pIndexData->vnum;

     *
     * Slick recursion to write lists backwards,
     *   so loading them will load in forwards order.
     */
	if(!fp || !obj || !ch)
		return;
	if(obj->next_content != 0)
		fwrite_obj(ch, obj->next_content, fp, iNest);
	/*
	 * Castrate storage characters.
	 */

	if((obj->chobj != 0 && (!IS_NPC(obj->chobj) && obj->chobj->pcdata->obj_vnum != 0)) || obj->item_type == ITEM_KEY)
		return;

	fprintf(fp, "#OBJECT\n");
	fprintf(fp, "Nest         %li\n", iNest);
	fprintf(fp, "Name         %s~\n", obj->name);
	fprintf(fp, "ShortDescr   %s~\n", obj->short_descr);
	fprintf(fp, "Description  %s~\n", obj->description);
	/* if (obj->chpoweron != 0 && strlen(obj->chpoweron) > 1 &&
	   str_cmp(obj->chpoweron,"(null)") )
	   fprintf( fp, "Poweronch    %s~\n",        obj->chpoweron       );
	   if (obj->chpoweroff != 0 && strlen(obj->chpoweroff) > 1 && str_cmp(obj->chpoweroff,"(null)"))
	   fprintf( fp, "Poweroffch   %s~\n",        obj->chpoweroff      );
	   if (obj->chpoweruse != 0 && strlen(obj->chpoweruse) > 1 && str_cmp(obj->chpoweruse,"(null)"))
	   fprintf( fp, "Powerusech   %s~\n",        obj->chpoweruse      );
	   if (obj->victpoweron != 0 && strlen(obj->victpoweron) > 1 && str_cmp(obj->victpoweron,"(null)"))
	   fprintf( fp, "Poweronvict  %s~\n",        obj->victpoweron     );
	   if (obj->victpoweroff != 0 && strlen(obj->victpoweroff) > 1 && str_cmp(obj->victpoweroff,"(null)"))
	   fprintf( fp, "Poweroffvict %s~\n",        obj->victpoweroff    );
	   if (obj->victpoweruse != 0 && strlen(obj->victpoweruse) > 1 && str_cmp(obj->victpoweruse,"(null)"))
	   fprintf( fp, "Powerusevict %s~\n",        obj->victpoweruse    );
	 */
	if(obj->questmaker != 0 && strlen(obj->questmaker) > 1)
		fprintf(fp, "Questmaker   %s~\n", obj->questmaker);
	if(obj->questowner != 0 && strlen(obj->questowner) > 1)
		fprintf(fp, "Questowner   %s~\n", obj->questowner);

	if(obj->pagetitle != 0 && strlen(obj->pagetitle) > 1)
		fprintf(fp, "Pagetitle    %s~\n", obj->pagetitle);
	if(obj->pagelines != 0 && strlen(obj->pagelines) > 1)
		fprintf(fp, "PageLines    %s~\n", obj->pagelines);
/*
    if ( vnum < 17 || ( vnum >= 30003 && vnum <= 30037 ) )
	fprintf( fp, "Vnum         %li\n",	obj->pIndexData->vnum     );
    else
	fprintf( fp, "Vnum         30041\n"			     );
*/
	fprintf(fp, "Vnum         %li\n", obj->pIndexData->vnum);

	fprintf(fp, "ExtraFlags   %li\n", obj->extra_flags);
	fprintf(fp, "WearFlags    %li\n", obj->wear_flags);
	fprintf(fp, "WearLoc      %li\n", obj->wear_loc);
	fprintf(fp, "ItemType     %li\n", obj->item_type);
	fprintf(fp, "Weight       %li\n", obj->weight);
	if(obj->spectype != 0)
		fprintf(fp, "Spectype     %li\n", obj->spectype);
	if(obj->specpower != 0)
		fprintf(fp, "Specpower    %li\n", obj->specpower);
	fprintf(fp, "Condition    %li\n", obj->condition);
	fprintf(fp, "Toughness    %li\n", obj->toughness);
	fprintf(fp, "Resistance   %li\n", obj->resistance);
	if(obj->quest != 0)
		fprintf(fp, "Quest        %li\n", obj->quest);
	if(obj->points != 0)
		fprintf(fp, "Points       %li\n", obj->points);
	fprintf(fp, "Level        %li\n", obj->level);
	fprintf(fp, "Timer        %li\n", obj->timer);
	fprintf(fp, "Cost         %li\n", obj->cost);
	fprintf(fp, "Values       %li %li %li %li\n", obj->value[0], obj->value[1], obj->value[2], obj->value[3]);

	switch (obj->item_type)
	{
	case ITEM_POTION:
		if(obj->value[1] > 0)
		{
			fprintf(fp, "Spell 1      '%s'\n", skill_table[obj->value[1]].name);
		}

		if(obj->value[2] > 0)
		{
			fprintf(fp, "Spell 2      '%s'\n", skill_table[obj->value[2]].name);
		}

		if(obj->value[3] > 0)
		{
			fprintf(fp, "Spell 3      '%s'\n", skill_table[obj->value[3]].name);
		}

		break;

	case ITEM_SCROLL:
		if(obj->value[1] > 0)
		{
			fprintf(fp, "Spell 1      '%s'\n", skill_table[obj->value[1]].name);
		}

		if(obj->value[2] > 0)
		{
			fprintf(fp, "Spell 2      '%s'\n", skill_table[obj->value[2]].name);
		}

		if(obj->value[3] > 0)
		{
			fprintf(fp, "Spell 3      '%s'\n", skill_table[obj->value[3]].name);
		}

		break;

	case ITEM_PILL:
	case ITEM_STAFF:
	case ITEM_WAND:
		if(obj->value[3] > 0)
		{
			fprintf(fp, "Spell 3      '%s'\n", skill_table[obj->value[3]].name);
		}

		break;
	}

	for(paf = obj->affected; paf; paf = paf->next)
	{
		if(!paf)
			break;

		if(paf->type < 0 || paf->type >= MAX_SKILL)
			continue;

		fprintf(fp, "AffectData   %li %li %li\n", paf->duration, paf->modifier, paf->location);
	}

	for(ed = obj->extra_descr; ed != 0; ed = ed->next)
	{
		fprintf(fp, "ExtraDescr   %s~ %s~\n", ed->keyword, ed->description);
	}
/*
    if ( vnum >= 17 && ( vnum < 30003 || vnum > 30037 ) )
    {
	for ( ed = obj->pIndexData->extra_descr; ed != 0; ed = ed->next )
	{
	    fprintf( fp, "ExtraDescr   %s~ %s~\n",
		ed->keyword, ed->description );
	}
    }
*/
	fprintf(fp, "End\n\n");

	if(obj->contains != 0)
		fwrite_obj(ch, obj->contains, fp, iNest + 1);

	return;
}



/*
 * Load a char and inventory into a new ch structure.
 */
long load_char_obj(DESCRIPTOR_DATA * d, char *name)
{
	static PC_DATA pcdata_zero;
	char strsave[MAX_STRING_LENGTH];
	char *strtime;
	CHAR_DATA *ch;
	FILE *fp;
	bool found;
	long sn;

	if(!save_in_binary && !check_pfile(name))
	{
		write_to_descriptor(d->descriptor,
				    "Your pfile is corrupted, attempting to restore from backup. RE-LOG.\n\r", 0);
		sprintf(strsave, "{RPfile [%s] is CORRUPTED!{x  Restoring.", name);
		log_string(strsave);
		sprintf(strsave, "cp %sstore/%s %s%s", PLAYER_DIR, capitalize(name), PLAYER_DIR, capitalize(name));
		system(strsave);
		return -1;
	}

	if(char_free == 0)
	{
		ch = alloc_perm(sizeof(*ch));
	}
	else
	{
		ch = char_free;
		char_free = char_free->next;
	}
	memset(ch, 0, sizeof(*ch));
	clear_char(ch);

	if(pcdata_free == 0)
	{
		ch->pcdata = alloc_perm(sizeof(*ch->pcdata));
	}
	else
	{
		ch->pcdata = pcdata_free;
		pcdata_free = pcdata_free->next;
	}
	*ch->pcdata = pcdata_zero;

	d->character = ch;
	ch->desc = d;
	ch->name = str_dup(name);
	ch->act = PLR_BLANK | PLR_COMBINE | PLR_PROMPT;
	ch->extra = 0;
	ch->special = 0;
	ch->newbits = 0;
	ch->pcdata->class = -1;
	ch->pcdata->familiar = 0;
	ch->pcdata->partner = 0;
	ch->pcdata->propose = 0;
	ch->pcdata->chobj = 0;
	ch->pcdata->memorised = 0;
	ch->pcdata->vision = 7;
	ch->pcdata->surveying = 0;
	ch->pcdata->pwd = str_dup("");
	ch->pcdata->bamfin = str_dup("");
	ch->pcdata->bamfout = str_dup("");
	ch->pcdata->title = str_dup("");
	ch->pcdata->conception = str_dup("");
	ch->pcdata->parents = str_dup("");
	ch->pcdata->cparents = str_dup("");
	ch->pcdata->marriage = str_dup("");
	ch->pcdata->email = str_dup("");
	ch->pcdata->autostance = str_dup("");
	ch->pcdata->krank = str_dup("");
	ch->pcdata->enter = str_dup("");
	ch->pcdata->exit = str_dup("");
	ch->pcdata->login = str_dup("");
	ch->pcdata->logout = str_dup("");
	ch->lord = str_dup("");
	ch->clan = str_dup("");
	ch->morph = str_dup("");
	ch->pload = str_dup("");
	ch->prompt = str_dup("");
	ch->cprompt = str_dup("");
	strtime = ctime(&current_time);
	strtime[strlen(strtime) - 1] = '\0';
	free_string(ch->lasttime);
	ch->createtime = str_dup(strtime);
	ch->lasttime = str_dup("");
	ch->lasthost = str_dup("");
	ch->lastchat = str_dup("");
	ch->poweraction = str_dup("");
	ch->powertype = str_dup("");
	ch->hunting = str_dup("");
	ch->pcdata->followers = 0;
	ch->spectype = 0;
	ch->specpower = 0;
	ch->mounted = 0;
	ch->home = 3001;
	ch->vampgen_a = 0;
	ch->paradox[0] = 0;	/* Total paradox */
	ch->paradox[1] = 0;	/* Current Paradox */
	ch->paradox[2] = 0;	/* Paradox Ticker */
	ch->damcap[0] = 1000;
	ch->damcap[1] = 0;
	ch->vampaff_a = 0;
	ch->itemaffect = 0;
	ch->swordtech = 0;
	ch->vamppass_a = -1;
	ch->polyaff = 0;
	ch->immune = 0;
	ch->form = 32767;
	ch->beast = 15;
	for(sn = 0; sn < 7; sn++)
		ch->loc_hp[sn] = 0;
	for(sn = 0; sn < 13; sn++)
		ch->wpn[sn] = 0;
	for(sn = 0; sn < 5; sn++)
		ch->spl[sn] = 0;
	for(sn = 0; sn < 8; sn++)
		ch->cmbt[sn] = 0;
	for(sn = 0; sn < 11; sn++)
		ch->cmbt[sn] = 0;

	ch->pkill = 0;
	ch->pdeath = 0;
	ch->mkill = 0;
	ch->mdeath = 0;
	ch->pcdata->class = -1;
	ch->pcdata->followers = 0;
	ch->pcdata->changesplace = 0;
	ch->pcdata->perm_str = 13;
	ch->pcdata->perm_int = 13;
	ch->pcdata->perm_wis = 13;
	ch->pcdata->perm_dex = 13;
	ch->pcdata->perm_con = 13;
	ch->pcdata->quest = 0;
	ch->pcdata->wolf = 0;
	ch->pcdata->rank = 0;
	ch->pcdata->language[0] = 0;
	ch->pcdata->language[1] = 0;
	ch->pcdata->stage[0] = 0;
	ch->pcdata->stage[1] = 0;
	ch->pcdata->stage[2] = 0;
	ch->pcdata->wolfform[0] = 1;
	ch->pcdata->wolfform[1] = 1;
	ch->pcdata->score[0] = 0;
	ch->pcdata->score[1] = 0;
	ch->pcdata->score[2] = 0;
	ch->pcdata->score[3] = 0;
	ch->pcdata->score[4] = 0;
	ch->pcdata->score[5] = 0;

	for(sn = 0; sn < 11; sn++)
		ch->pcdata->disc_a[sn] = 0;

	for(sn = 0; sn < 10; sn++)
		ch->pcdata->genes[sn] = 0;

	for(sn = 0; sn < 20; sn++)
		ch->pcdata->powers[sn] = 0;

	for(sn = 0; sn < 12; sn++)
		ch->pcdata->stats[sn] = 0;

	ch->pcdata->fake_skill = 0;
	ch->pcdata->fake_stance = 0;
	ch->pcdata->fake_hit = 0;
	ch->pcdata->fake_dam = 0;
	ch->pcdata->fake_ac = 0;
	ch->pcdata->fake_hp = 0;
	ch->pcdata->fake_mana = 0;
	ch->pcdata->fake_move = 0;
	ch->pcdata->obj_vnum = 0;
	ch->pcdata->condition[COND_THIRST] = 48;
	ch->pcdata->condition[COND_FULL] = 48;
	ch->pcdata->stat_ability[STAT_STR] = 0;
	ch->pcdata->stat_ability[STAT_END] = 0;
	ch->pcdata->stat_ability[STAT_REF] = 0;
	ch->pcdata->stat_ability[STAT_FLE] = 0;
	ch->pcdata->stat_amount[STAT_STR] = 0;
	ch->pcdata->stat_amount[STAT_END] = 0;
	ch->pcdata->stat_amount[STAT_REF] = 0;
	ch->pcdata->stat_amount[STAT_FLE] = 0;
	ch->pcdata->stat_duration[STAT_STR] = 0;
	ch->pcdata->stat_duration[STAT_END] = 0;
	ch->pcdata->stat_duration[STAT_REF] = 0;
	ch->pcdata->stat_duration[STAT_FLE] = 0;
	ch->pcdata->exhaustion = 0;

	found = FALSE;
	fclose(fpReserve);

	if(save_in_binary)
	{
		sprintf(strsave, "%sbinary/%s", PLAYER_DIR, capitalize(name));
		if((fp = fopen(strsave, "rb")) != 0)
		{
			fread(ch, sizeof(*ch), 1, fp);
			fclose(fp);
		}
	}
	else
	{
		sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(name));
		if((fp = fopen(strsave, "r")) != 0)
		{
			long iNest;

			for(iNest = 0; iNest < MAX_NEST; iNest++)
				rgObjNest[iNest] = 0;

			found = TRUE;
			for(;;)
			{
				char letter;
				char *word;

				letter = fread_letter(fp);
				if(letter == '*')
				{
					fread_to_eol(fp);
					continue;
				}

				if(letter != '#')
				{
					bug("Load_char_obj: # not found.", 0);
					break;
				}

				word = fread_word(fp);

				if(!str_cmp(word, "PLAYERS"))
				{
					if(fread_char(ch, fp) == FALSE)
					{
						write_to_descriptor(d->descriptor,
								    "{M\n\r\n\r\n\rYour pfile is corrupted!\n\r", 0);
						found = FALSE;
						break;
					}
				}
				else if(!str_cmp(word, "OBJECT"))
				{
					if(fread_obj(ch, fp) == FALSE)
					{
						write_to_descriptor(d->descriptor,
								    "{M\n\r\n\r\n\rYour pfile is corrupted!\n\r", 0);
						found = FALSE;
						break;
					}
				}
				else if(!str_cmp(word, "END"))
					break;
				else
				{
					bug("Load_char_obj: bad section.", 0);
					break;
				}
			}
			fclose(fp);
		}
	}
	fpReserve = fopen(NULL_FILE, "r");
	return found;
}



bool load_char_short(DESCRIPTOR_DATA * d, char *name)
{
	static PC_DATA pcdata_zero;
	char strsave[MAX_STRING_LENGTH];
	CHAR_DATA *ch;
	char *strtime;
	FILE *fp;
	bool found;

	if(char_free == 0)
	{
		ch = alloc_perm(sizeof(CHAR_DATA));
	}
	else
	{
		ch = char_free;
		char_free = char_free->next;
	}
	clear_char(ch);

	if(pcdata_free == 0)
	{
		ch->pcdata = alloc_perm(sizeof(PC_DATA));
	}
	else
	{
		ch->pcdata = pcdata_free;
		pcdata_free = pcdata_free->next;
	}
	*ch->pcdata = pcdata_zero;

	d->character = ch;
	ch->desc = d;
	ch->name = str_dup(name);
	ch->act = PLR_BLANK | PLR_COMBINE | PLR_PROMPT;
	ch->extra = 0;
	ch->pcdata->familiar = 0;
	ch->pcdata->partner = 0;
	ch->pcdata->propose = 0;
	ch->pcdata->chobj = 0;
	ch->pcdata->memorised = 0;
	ch->pcdata->pwd = str_dup("");
	ch->pcdata->bamfin = str_dup("");
	ch->pcdata->bamfout = str_dup("");
	ch->pcdata->title = str_dup("");
	ch->pcdata->conception = str_dup("");
	ch->pcdata->parents = str_dup("");
	ch->pcdata->cparents = str_dup("");
	ch->pcdata->marriage = str_dup("");
	ch->pcdata->email = str_dup("");
	ch->pcdata->krank = str_dup("");
	ch->pcdata->login = str_dup("");
	ch->pcdata->logout = str_dup("");
	ch->pcdata->enter = str_dup("");
	ch->pcdata->exit = str_dup("");
	ch->lord = str_dup("");
	ch->clan = str_dup("");
	ch->morph = str_dup("");
	ch->pload = str_dup("");
	ch->prompt = str_dup("");
	ch->cprompt = str_dup("");
	strtime = ctime(&current_time);
	strtime[strlen(strtime) - 1] = '\0';
	free_string(ch->lasttime);
	ch->createtime = str_dup(strtime);
	ch->lasttime = str_dup("");
	ch->lastchat = str_dup("");
	ch->lasthost = str_dup("");
	ch->poweraction = str_dup("");
	ch->powertype = str_dup("");
	ch->hunting = str_dup("");
	ch->spectype = 0;
	ch->specpower = 0;
	ch->mounted = 0;
	ch->home = 3001;
	ch->vampgen_a = 0;
	ch->paradox[0] = 0;	/* Total paradox */
	ch->paradox[1] = 0;	/* Current Paradox */
	ch->paradox[2] = 0;	/* Paradox Ticker */
	ch->damcap[0] = 1000;
	ch->damcap[1] = 0;
	ch->vampaff_a = 0;
	ch->itemaffect = 0;
	ch->vamppass_a = -1;
	ch->polyaff = 0;
	ch->immune = 0;
	ch->form = 32767;
	ch->beast = 15;
	ch->loc_hp[0] = 0;
	ch->loc_hp[1] = 0;
	ch->loc_hp[2] = 0;
	ch->loc_hp[3] = 0;
	ch->loc_hp[4] = 0;
	ch->loc_hp[5] = 0;
	ch->loc_hp[6] = 0;
	ch->wpn[0] = 0;
	ch->wpn[1] = 0;
	ch->wpn[2] = 0;
	ch->wpn[3] = 0;
	ch->wpn[4] = 0;
	ch->wpn[5] = 0;
	ch->wpn[6] = 0;
	ch->wpn[7] = 0;
	ch->wpn[8] = 0;
	ch->wpn[9] = 0;
	ch->wpn[10] = 0;
	ch->wpn[11] = 0;
	ch->wpn[12] = 0;
	ch->spl[0] = 4;
	ch->spl[1] = 4;
	ch->spl[2] = 4;
	ch->spl[3] = 4;
	ch->spl[4] = 4;
	ch->cmbt[0] = 0;
	ch->cmbt[1] = 0;
	ch->cmbt[2] = 0;
	ch->cmbt[3] = 0;
	ch->cmbt[4] = 0;
	ch->cmbt[5] = 0;
	ch->cmbt[6] = 0;
	ch->cmbt[7] = 0;
	ch->stance[0] = 0;
	ch->stance[1] = 0;
	ch->stance[2] = 0;
	ch->stance[3] = 0;
	ch->stance[4] = 0;
	ch->stance[5] = 0;
	ch->stance[6] = 0;
	ch->stance[7] = 0;
	ch->stance[8] = 0;
	ch->stance[9] = 0;
	ch->stance[10] = 0;
	ch->pkill = 0;
	ch->pdeath = 0;
	ch->mkill = 0;
	ch->mdeath = 0;
	ch->pcdata->class = -1;
	ch->pcdata->followers = 0;
	ch->pcdata->perm_str = 13;
	ch->pcdata->perm_int = 13;
	ch->pcdata->perm_wis = 13;
	ch->pcdata->perm_dex = 13;
	ch->pcdata->perm_con = 13;
	ch->pcdata->quest = 0;
	ch->pcdata->wolf = 0;
	ch->pcdata->rank = 0;
	ch->pcdata->language[0] = 0;
	ch->pcdata->language[1] = 0;
	ch->pcdata->stage[0] = 0;
	ch->pcdata->stage[1] = 0;
	ch->pcdata->stage[2] = 0;
	ch->pcdata->wolfform[0] = 1;
	ch->pcdata->wolfform[1] = 1;
	ch->pcdata->score[0] = 0;
	ch->pcdata->score[1] = 0;
	ch->pcdata->score[2] = 0;
	ch->pcdata->score[3] = 0;
	ch->pcdata->score[4] = 0;
	ch->pcdata->score[5] = 0;
	ch->pcdata->runes[0] = 0;
	ch->pcdata->runes[1] = 0;
	ch->pcdata->runes[2] = 0;
	ch->pcdata->runes[3] = 0;
	ch->pcdata->disc_a[0] = 0;
	ch->pcdata->disc_a[1] = 0;
	ch->pcdata->disc_a[2] = 0;
	ch->pcdata->disc_a[3] = 0;
	ch->pcdata->disc_a[4] = 0;
	ch->pcdata->disc_a[5] = 0;
	ch->pcdata->disc_a[6] = 0;
	ch->pcdata->disc_a[7] = 0;
	ch->pcdata->disc_a[8] = 0;
	ch->pcdata->disc_a[9] = 0;
	ch->pcdata->disc_a[10] = 0;
	ch->pcdata->genes[0] = 0;
	ch->pcdata->genes[1] = 0;
	ch->pcdata->genes[2] = 0;
	ch->pcdata->genes[3] = 0;
	ch->pcdata->genes[4] = 0;
	ch->pcdata->genes[5] = 0;
	ch->pcdata->genes[6] = 0;
	ch->pcdata->genes[7] = 0;
	ch->pcdata->genes[8] = 0;
	ch->pcdata->genes[9] = 0;
	ch->pcdata->powers[0] = 0;
	ch->pcdata->powers[1] = 0;
	ch->pcdata->fake_skill = 0;
	ch->pcdata->fake_stance = 0;
	ch->pcdata->fake_hit = 0;
	ch->pcdata->fake_dam = 0;
	ch->pcdata->fake_ac = 0;
	ch->pcdata->fake_hp = 0;
	ch->pcdata->fake_mana = 0;
	ch->pcdata->fake_move = 0;
	ch->pcdata->obj_vnum = 0;
	ch->pcdata->condition[COND_THIRST] = 48;
	ch->pcdata->condition[COND_FULL] = 48;
	ch->pcdata->stat_ability[STAT_STR] = 0;
	ch->pcdata->stat_ability[STAT_END] = 0;
	ch->pcdata->stat_ability[STAT_REF] = 0;
	ch->pcdata->stat_ability[STAT_FLE] = 0;
	ch->pcdata->stat_amount[STAT_STR] = 0;
	ch->pcdata->stat_amount[STAT_END] = 0;
	ch->pcdata->stat_amount[STAT_REF] = 0;
	ch->pcdata->stat_amount[STAT_FLE] = 0;
	ch->pcdata->stat_duration[STAT_STR] = 0;
	ch->pcdata->stat_duration[STAT_END] = 0;
	ch->pcdata->stat_duration[STAT_REF] = 0;
	ch->pcdata->stat_duration[STAT_FLE] = 0;
	ch->pcdata->exhaustion = 0;

	found = FALSE;
	fclose(fpReserve);
	sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(name));
	if((fp = fopen(strsave, "r")) != 0)
	{
		long iNest;

		for(iNest = 0; iNest < MAX_NEST; iNest++)
			rgObjNest[iNest] = 0;

		found = TRUE;
		for(;;)
		{
			char letter;
			char *word;

			letter = fread_letter(fp);
			if(letter == '*')
			{
				fread_to_eol(fp);
				continue;
			}

			if(letter != '#')
			{
				bug("Load_char_obj: # not found.", 0);
				break;
			}

			word = fread_word(fp);
			if(!str_cmp(word, "PLAYERS"))
				fread_char(ch, fp);
			else if(!str_cmp(word, "OBJECT"))
				break;
			else if(!str_cmp(word, "END"))
				break;
			else
			{
				bug("Load_char_obj: bad section.", 0);
				break;
			}
		}
		fclose(fp);
	}

	fpReserve = fopen(NULL_FILE, "r");
	return found;
}



/*
 * Read in a char.
 */

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}


bool fread_obj(CHAR_DATA * ch, FILE * fp)
{
	static OBJ_DATA obj_zero;
	OBJ_DATA *obj;
	char *word;
	long iNest;
	bool fMatch;
	bool fNest;
	bool fVnum;

	if(obj_free == 0)
	{
		obj = alloc_perm(sizeof(*obj));
	}
	else
	{
		obj = obj_free;
		obj_free = obj_free->next;
	}

	*obj = obj_zero;
	obj->name = str_dup("");
	obj->short_descr = str_dup("");
	obj->description = str_dup("");
	obj->chpoweron = str_dup("(null)");
	obj->chpoweroff = str_dup("(null)");
	obj->chpoweruse = str_dup("(null)");
	obj->victpoweron = str_dup("(null)");
	obj->victpoweroff = str_dup("(null)");
	obj->victpoweruse = str_dup("(null)");
	obj->questmaker = str_dup("");
	obj->questowner = str_dup("");
	obj->pagetitle = str_dup("");
	obj->pagelines = str_dup("");

	obj->spectype = 0;
	obj->specpower = 0;
	obj->condition = 100;
	obj->toughness = 0;
	obj->resistance = 100;
	obj->quest = 0;
	obj->points = 0;

	fNest = FALSE;
	fVnum = TRUE;
	iNest = 0;

	for(;;)
	{
		word = feof(fp) ? "End" : fread_word(fp);
		fMatch = FALSE;

		if(bad_fword(word))
			return FALSE;

		switch (UPPER(word[0]))
		{
		case '*':
			fMatch = TRUE;
			fread_to_eol(fp);
			break;

		case 'A':
			if(!str_cmp(word, "Affect") || !str_cmp(word, "AffectData"))
			{
				AFFECT_DATA *paf;

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
				paf->duration = fread_number(fp);
				paf->modifier = fread_number(fp);
				paf->location = fread_number(fp);
				paf->bitvector = 0;
				paf->next = obj->affected;
				obj->affected = paf;
				fMatch = TRUE;
				break;
			}
			break;

		case 'C':
			KEY("Condition", obj->condition, fread_number(fp));
			KEY("Cost", obj->cost, fread_number(fp));
			break;

		case 'D':
			KEY("Description", obj->description, fread_string(fp));
			break;

		case 'E':
			KEY("ExtraFlags", obj->extra_flags, fread_number(fp));

			if(!str_cmp(word, "ExtraDescr"))
			{
				EXTRA_DESCR_DATA *ed;

				if(extra_descr_free == 0)
				{
					ed = alloc_perm(sizeof(*ed));
				}
				else
				{
					ed = extra_descr_free;
					extra_descr_free = extra_descr_free->next;
				}

				ed->keyword = fread_string(fp);
				ed->description = fread_string(fp);
				ed->next = obj->extra_descr;
				obj->extra_descr = ed;
				fMatch = TRUE;
			}

			if(!str_cmp(word, "End"))
			{
				if(!fNest || !fVnum)
				{
					bug("Fread_obj: incomplete object.", 0);
					free_string(obj->name);
					free_string(obj->description);
					free_string(obj->short_descr);
					obj->next = obj_free;
					obj_free = obj;
					return TRUE;
				}
				else
				{
					obj->next = object_list;
					object_list = obj;
					obj->pIndexData->count++;
					if(iNest == 0 || rgObjNest[iNest] == 0)
						obj_to_char(obj, ch);
					else
						obj_to_obj(obj, rgObjNest[iNest - 1]);
					return TRUE;
				}
			}
			break;

		case 'I':
			KEY("ItemType", obj->item_type, fread_number(fp));
			break;

		case 'L':
			KEY("Level", obj->level, fread_number(fp));
			break;

		case 'N':
			KEY("Name", obj->name, fread_string(fp));

			if(!str_cmp(word, "Nest"))
			{
				iNest = fread_number(fp);
				if(iNest < 0 || iNest >= MAX_NEST)
				{
					bug("Fread_obj: bad nest %li.", iNest);
				}
				else
				{
					rgObjNest[iNest] = obj;
					fNest = TRUE;
				}
				fMatch = TRUE;
			}
			break;

		case 'P':
			KEY("Pagetitle", obj->pagetitle, fread_string(fp));
			KEY("PageLines", obj->pagelines, fread_string(fp));

			KEY("Points", obj->points, fread_number(fp));
			/* KEY( "Poweronch", obj->chpoweron,         fread_string( fp )
			   );
			   KEY( "Poweroffch",        obj->chpoweroff,        fread_string( fp ) );
			   KEY( "Powerusech",        obj->chpoweruse,        fread_string( fp ) );
			   KEY( "Poweronvict",       obj->victpoweron,       fread_string( fp ) );
			   KEY( "Poweroffvict",obj->victpoweroff,    fread_string( fp ) );
			   KEY( "Powerusevict",obj->victpoweruse,    fread_string( fp ) );
			 */
			break;

		case 'Q':
			KEY("Quest", obj->quest, fread_number(fp));
			KEY("Questmaker", obj->questmaker, fread_string(fp));
			KEY("Questowner", obj->questowner, fread_string(fp));
			break;

		case 'R':
			KEY("Resistance", obj->resistance, fread_number(fp));
			break;

		case 'S':
			KEY("ShortDescr", obj->short_descr, fread_string(fp));
			KEY("Spectype", obj->spectype, fread_number(fp));
			KEY("Specpower", obj->specpower, fread_number(fp));

			if(!str_cmp(word, "Spell"))
			{
				long iValue;
				long sn;

				iValue = fread_number(fp);

				word = fread_word(fp);

				if(bad_fword(word))
					return FALSE;

				sn = skill_lookup(word);
				if(iValue < 0 || iValue > 3)
				{
					bug("Fread_obj: bad iValue %li.", iValue);
				}
				else if(sn < 0)
				{
					bug("Fread_obj: unknown skill.", 0);
				}
				else
				{
					obj->value[iValue] = sn;
				}
				fMatch = TRUE;
				break;
			}
			break;

		case 'T':
			KEY("Timer", obj->timer, fread_number(fp));
			KEY("Toughness", obj->toughness, fread_number(fp));
			break;

		case 'V':
			if(!str_cmp(word, "Values"))
			{
				obj->value[0] = fread_number(fp);
				obj->value[1] = fread_number(fp);
				obj->value[2] = fread_number(fp);
				obj->value[3] = fread_number(fp);
				fMatch = TRUE;
				break;
			}

			if(!str_cmp(word, "Vnum"))
			{
				long vnum;

				vnum = fread_number(fp);
				if((obj->pIndexData = get_obj_index(vnum)) == 0)
				{
					vnum = 30041;
					if((obj->pIndexData = get_obj_index(vnum)) == 0)
						bug("Fread_obj: bad vnum %li.", vnum);
					else
						fVnum = TRUE;
				}
				else
					fVnum = TRUE;
				fMatch = TRUE;
				break;
			}
			break;

		case 'W':
			KEY("WearFlags", obj->wear_flags, fread_number(fp));
			KEY("WearLoc", obj->wear_loc, fread_number(fp));
			KEY("Weight", obj->weight, fread_number(fp));
			break;

		}

		if(!fMatch)
		{
			bug("Fread_obj: no match.", 0);
			fread_to_eol(fp);
		}
	}
}



bool fread_char(CHAR_DATA * ch, FILE * fp)
{
	char buf[MAX_STRING_LENGTH];
	char *word;
	bool fMatch;
	long dummy;
	long sn;
	long line = 0;

	for(;;)
	{
		word = feof(fp) ? "End" : fread_word(fp);
		fMatch = FALSE;

		if(bad_fword(word))
			return FALSE;

		switch (UPPER(word[0]))
		{
		case '*':
			fMatch = TRUE;
			fread_to_eol(fp);
			break;

		case 'A':
			KEY("Awins", ch->awins, fread_number(fp));
			KEY("Alosses", ch->alosses, fread_number(fp));
			KEY("Act", ch->act, fread_number(fp));
			KEY("AffectedBy", ch->affected_by, fread_number(fp));
			KEY("Alignment", ch->alignment, fread_number(fp));
			KEY("Armor", ch->armor, fread_number(fp));
			KEY("Aff2", ch->flag2, fread_number(fp));

			if(!str_cmp(word, "Affect") || !str_cmp(word, "AffectData"))
			{
				AFFECT_DATA *paf;

				if(affect_free == 0)
				{
					paf = alloc_perm(sizeof(*paf));
				}
				else
				{
					paf = affect_free;
					affect_free = affect_free->next;
				}

				if(!str_cmp(word, "Affect"))
				{
					/* Obsolete 2.0 form. */
					paf->type = fread_number(fp);
				}
				else
				{
					long sn;

					word = fread_word(fp);
					if(bad_fword(word))
						return FALSE;

					sn = skill_lookup(word);
					if(sn < 0)
						bug("Fread_char: unknown skill.", 0);
					else
						paf->type = sn;
				}

				paf->duration = fread_number(fp);
				paf->modifier = fread_number(fp);
				paf->location = fread_number(fp);
				paf->bitvector = fread_number(fp);
				paf->next = ch->affected;
				ch->affected = paf;
				fMatch = TRUE;
				break;
			}

			if(!str_cmp(word, "AttrMod"))
			{
				ch->pcdata->mod_str = fread_number(fp);
				ch->pcdata->mod_int = fread_number(fp);
				ch->pcdata->mod_wis = fread_number(fp);
				ch->pcdata->mod_dex = fread_number(fp);
				ch->pcdata->mod_con = fread_number(fp);
				fMatch = TRUE;
				break;
			}

			if(!str_cmp(word, "AttrPerm"))
			{
				ch->pcdata->perm_str = fread_number(fp);
				ch->pcdata->perm_int = fread_number(fp);
				ch->pcdata->perm_wis = fread_number(fp);
				ch->pcdata->perm_dex = fread_number(fp);
				ch->pcdata->perm_con = fread_number(fp);
				fMatch = TRUE;
				break;
			}
			KEY("Autostance", ch->pcdata->autostance, fread_string(fp));
			break;

		case 'B':
			KEY("Bamfin", ch->pcdata->bamfin, fread_string(fp));
			KEY("Bamfout", ch->pcdata->bamfout, fread_string(fp));
			KEY("Bounty", ch->bounty, fread_number(fp));
			KEY("Beast", ch->beast, fread_number(fp));
			break;

		case 'C':
			KEY("Cols", ch->cols, fread_number(fp));
			KEY("Changesplace", ch->pcdata->changesplace, fread_number(fp));
			KEY("Clan", ch->clan, fread_string(fp));
			KEY("Class", ch->pcdata->class, fread_number(fp));

			if(!str_cmp(word, "Combat"))
			{
				ch->cmbt[0] = fread_number(fp);
				ch->cmbt[1] = fread_number(fp);
				ch->cmbt[2] = fread_number(fp);
				ch->cmbt[3] = fread_number(fp);
				ch->cmbt[4] = fread_number(fp);
				ch->cmbt[5] = fread_number(fp);
				ch->cmbt[6] = fread_number(fp);
				ch->cmbt[7] = fread_number(fp);
				fMatch = TRUE;
				break;
			}

			KEY("Conception", ch->pcdata->conception, fread_string(fp));

			if(!str_cmp(word, "Condition"))
			{
				ch->pcdata->condition[0] = fread_number(fp);
				ch->pcdata->condition[1] = fread_number(fp);
				ch->pcdata->condition[2] = fread_number(fp);
				fMatch = TRUE;
				break;
			}
			KEY("Cparents", ch->pcdata->cparents, fread_string(fp));
			KEY("Cprompt", ch->cprompt, fread_string(fp));
			KEY("Createtime", ch->createtime, fread_string(fp));
			break;

		case 'D':
			KEY("Damroll", ch->damroll, fread_number(fp));
			KEY("Deaf", ch->deaf, fread_number(fp));
			KEY("Demonic", dummy, fread_number(fp));
			KEY("Description", ch->description, fread_string(fp));


			if(!str_cmp(word, "Disc"))
			{
				dummy = fread_number(fp);
				dummy = fread_number(fp);
				dummy = fread_number(fp);
				dummy = fread_number(fp);
				dummy = fread_number(fp);
				dummy = fread_number(fp);
				dummy = fread_number(fp);
				dummy = fread_number(fp);
				dummy = fread_number(fp);
				dummy = fread_number(fp);
				dummy = fread_number(fp);
				fMatch = TRUE;
				break;
			}
			KEY("Drowaff", dummy, fread_number(fp));	/* Drow */
			KEY("Drowpwr", dummy, fread_number(fp));	/* Drow */
			KEY("Drowmag", dummy, fread_number(fp));	/* Drow */
			break;

		case 'E':
			KEY("Email", ch->pcdata->email, fread_string(fp));

			if(!str_cmp(word, "Enter"))
			{
				ch->pcdata->enter = fread_string(fp);
				if(strlen(ch->pcdata->enter) > 2)
				{
					sprintf(buf, " %s", ch->pcdata->enter);
					free_string(ch->pcdata->enter);
					ch->pcdata->enter = str_dup(buf);
				}
				fMatch = TRUE;
				break;
			}
			KEY("Exit", ch->pcdata->exit, fread_string(fp));
			if(!str_cmp(word, "End"))
				return TRUE;
			KEY("Exhaustion", ch->pcdata->exhaustion, fread_number(fp));
			KEY("Exp", ch->exp, fread_number(fp));
			KEY("Extra", ch->extra, fread_number(fp));
			break;

		case 'F':
			if(!str_cmp(word, "FakeCon"))
			{
				ch->pcdata->fake_skill = fread_number(fp);
				ch->pcdata->fake_stance = fread_number(fp);
				ch->pcdata->fake_hit = fread_number(fp);
				ch->pcdata->fake_dam = fread_number(fp);
				ch->pcdata->fake_ac = fread_number(fp);
				ch->pcdata->fake_hp = fread_number(fp);
				ch->pcdata->fake_mana = fread_number(fp);
				ch->pcdata->fake_move = fread_number(fp);
				fMatch = TRUE;
				break;
			}
			KEY("Form", ch->form, fread_number(fp));
			break;

		case 'G':
			if(!str_cmp(word, "Genes"))
			{
				ch->pcdata->genes[0] = fread_number(fp);
				ch->pcdata->genes[1] = fread_number(fp);
				ch->pcdata->genes[2] = fread_number(fp);
				ch->pcdata->genes[3] = fread_number(fp);
				ch->pcdata->genes[4] = fread_number(fp);
				ch->pcdata->genes[5] = fread_number(fp);
				ch->pcdata->genes[6] = fread_number(fp);
				ch->pcdata->genes[7] = fread_number(fp);
				ch->pcdata->genes[8] = fread_number(fp);
				ch->pcdata->genes[9] = fread_number(fp);
				fMatch = TRUE;
				break;
			}
			KEY("Gold", ch->gold, fread_number(fp));
			break;

		case 'H':
			KEY("Hitroll", ch->hitroll, fread_number(fp));
			KEY("Home", ch->home, fread_number(fp));

			if(!str_cmp(word, "HpManaMove"))
			{
				ch->hit = fread_number(fp);
				ch->max_hit = fread_number(fp);
				ch->mana = fread_number(fp);
				ch->max_mana = fread_number(fp);
				ch->move = fread_number(fp);
				ch->max_move = fread_number(fp);
				fMatch = TRUE;
				break;
			}
			break;

		case 'I':
/* IMC */
			KEY("IMCdeny", ch->pcdata->imc_deny, fread_number(fp));
			KEY("IMCallow", ch->pcdata->imc_allow, fread_number(fp));
			KEY("IMCdeaf", ch->pcdata->imc_deaf, fread_number(fp));
			KEY("IMClisten", ch->pcdata->ice_listen, fread_string(fp));

			if(!str_cmp(word, "Imms"))
			{
				ch->imms[0] = fread_number(fp);
				ch->imms[1] = fread_number(fp);
				ch->imms[2] = fread_number(fp);
				fMatch = TRUE;
				break;
			}
			KEY("Immune", ch->immune, fread_number(fp));
			KEY("Itemaffect", ch->itemaffect, fread_number(fp));
			break;

		case 'K':
			KEY("Krank", ch->pcdata->krank, fread_string(fp));
			KEY("Kingdom", ch->pcdata->kingdom, fread_number(fp));
			break;

		case 'L':
			KEY("Login", ch->pcdata->login, fread_string(fp));
			KEY("Logout", ch->pcdata->logout, fread_string(fp));
			if(!str_cmp(word, "Language"))
			{
				ch->pcdata->language[0] = fread_number(fp);
				ch->pcdata->language[1] = fread_number(fp);
				fMatch = TRUE;
				break;
			}

			KEY("Lasthost", ch->lasthost, fread_string(fp));
			KEY("Lasttime", ch->lasttime, fread_string(fp));
			KEY("Level", ch->level, fread_number(fp));
			if(!str_cmp(word, "Locationhp"))
			{
				ch->loc_hp[0] = fread_number(fp);
				ch->loc_hp[1] = fread_number(fp);
				ch->loc_hp[2] = fread_number(fp);
				ch->loc_hp[3] = fread_number(fp);
				ch->loc_hp[4] = fread_number(fp);
				ch->loc_hp[5] = fread_number(fp);
				ch->loc_hp[6] = fread_number(fp);
				fMatch = TRUE;
				break;
			}
			KEY("LongDescr", ch->long_descr, fread_string(fp));
			KEY("Lord", ch->lord, fread_string(fp));
			break;

		case 'M':
			KEY("Mapbits", ch->mapbits, fread_number(fp));
			KEY("Marriage", ch->pcdata->marriage, fread_string(fp));
			KEY("Mflags", ch->mflags, fread_number(fp));
			KEY("Morph", ch->morph, fread_string(fp));
			break;

		case 'N':
			KEY("Newbits", ch->newbits, fread_number(fp));

			if(!str_cmp(word, "Name"))
			{
				/*
				 * Name already set externally.
				 */
				fread_to_eol(fp);
				fMatch = TRUE;
				break;
			}

			break;

		case 'O':
			KEY("Objvnum", ch->pcdata->obj_vnum, fread_number(fp));
			KEY("ObjDesc", ch->objdesc, fread_string(fp));
			break;

		case 'P':
			if(!str_cmp(word, "Paradox"))
			{
				ch->paradox[0] = fread_number(fp);
				ch->paradox[1] = fread_number(fp);
				ch->paradox[2] = fread_number(fp);
				fMatch = TRUE;
				break;
			}

			KEY("Parents", ch->pcdata->parents, fread_string(fp));
			KEY("Password", ch->pcdata->pwd, fread_string(fp));
			KEY("Played", ch->played, fread_number(fp));
			KEY("Polyaff", ch->polyaff, fread_number(fp));
			KEY("Power_Point", dummy, fread_number(fp));	/* drow */

			if(!str_cmp(word, "Power"))
			{
				for(sn = 0; sn < 20; sn++)
					ch->pcdata->powers[sn] = fread_number(fp);

				fMatch = TRUE;
				break;
			}

			KEY("Poweraction", ch->poweraction, fread_string(fp));
			KEY("Powertype", ch->powertype, fread_string(fp));
			KEY("Position", ch->position, fread_number(fp));
			KEY("Practice", ch->practice, fread_number(fp));
			if(!str_cmp(word, "PkPdMkMd"))
			{
				ch->pkill = fread_number(fp);
				ch->pdeath = fread_number(fp);
				ch->mkill = fread_number(fp);
				ch->mdeath = fread_number(fp);
				fMatch = TRUE;
				break;
			}
			KEY("Prompt", ch->prompt, fread_string(fp));
			break;

		case 'Q':
			if(!str_cmp(word, "Quint"))
			{
				ch->quint[0] = fread_number(fp);
				ch->quint[1] = fread_number(fp);
				ch->quint[2] = fread_number(fp);
				fMatch = TRUE;
				break;
			}
			KEY("Quest", ch->pcdata->quest, fread_number(fp));
			break;

		case 'R':
			KEY("Rows", ch->rows,		fread_number(fp));
			KEY("Race", ch->pcdata->status, fread_number(fp));
			KEY("Rank", ch->pcdata->rank, fread_number(fp));

			if(!str_cmp(word, "Room"))
			{
				ch->in_room = get_room_index(fread_number(fp));
				if(ch->in_room == 0)
					ch->in_room = get_room_index(ROOM_VNUM_LIMBO);
				fMatch = TRUE;
				break;
			}

			if(!str_cmp(word, "Runes"))
			{
				dummy = fread_number(fp);
				dummy = fread_number(fp);
				dummy = fread_number(fp);
				dummy = fread_number(fp);
				fMatch = TRUE;
				break;
			}

			break;

		case 'S':
			KEY("Surveying", ch->pcdata->surveying, fread_number(fp));
			KEY("Swordtech", ch->swordtech, fread_number(fp));
			KEY("Safetimer", ch->pcdata->safe_timer, fread_number(fp));
			KEY("Safewait", ch->pcdata->safe_wait, fread_number(fp));
			KEY("Safewat", ch->pcdata->safe_wait, fread_number(fp));
			KEY("SavingThrow", ch->saving_throw, fread_number(fp));
			KEY("Souls", ch->pcdata->souls, fread_number(fp));
			if(!str_cmp(word, "Score"))
			{
				ch->pcdata->score[0] = fread_number(fp);
				ch->pcdata->score[1] = fread_number(fp);
				ch->pcdata->score[2] = fread_number(fp);
				ch->pcdata->score[3] = fread_number(fp);
				ch->pcdata->score[4] = fread_number(fp);
				ch->pcdata->score[5] = fread_number(fp);
				fMatch = TRUE;
				break;
			}

			if(!str_cmp(word, "Spheres"))
			{
				ch->spheres[0] = fread_number(fp);
				ch->spheres[1] = fread_number(fp);
				ch->spheres[2] = fread_number(fp);
				ch->spheres[3] = fread_number(fp);
				ch->spheres[4] = fread_number(fp);
				ch->spheres[5] = fread_number(fp);
				ch->spheres[6] = fread_number(fp);
				ch->spheres[7] = fread_number(fp);
				ch->spheres[8] = fread_number(fp);
				fMatch = TRUE;
				break;
			}

			KEY("Sex", ch->sex, fread_number(fp));
			KEY("ShortDescr", ch->short_descr, fread_string(fp));

			if(!str_cmp(word, "Skill"))
			{
				long sn;
				long value;

				value = fread_number(fp);
				word = fread_word(fp);

				if(bad_fword(word))
					return FALSE;

				sn = skill_lookup(word);
/*
		if ( sn < 0 )
		    bug( "Fread_char: unknown skill.", 0 );
		else
		    ch->pcdata->learned[sn] = value;
*/
				if(sn >= 0)
					ch->pcdata->learned[sn] = value;
				fMatch = TRUE;
			}

			KEY("Specpower", ch->specpower, fread_number(fp));
			KEY("Spectype", ch->spectype, fread_number(fp));
			KEY("Special", ch->special, fread_number(fp));

			if(!str_cmp(word, "Spells"))
			{
				ch->spl[0] = fread_number(fp);
				ch->spl[1] = fread_number(fp);
				ch->spl[2] = fread_number(fp);
				ch->spl[3] = fread_number(fp);
				ch->spl[4] = fread_number(fp);
				fMatch = TRUE;
				break;
			}

			if(!str_cmp(word, "Stage"))
			{
				ch->pcdata->stage[0] = fread_number(fp);
				ch->pcdata->stage[1] = fread_number(fp);
				ch->pcdata->stage[2] = fread_number(fp);
				fMatch = TRUE;
				break;
			}

			if(!str_cmp(word, "Stance"))
			{
				ch->stance[0] = fread_number(fp);
				ch->stance[1] = fread_number(fp);
				ch->stance[2] = fread_number(fp);
				ch->stance[3] = fread_number(fp);
				ch->stance[4] = fread_number(fp);
				ch->stance[5] = fread_number(fp);
				ch->stance[6] = fread_number(fp);
				ch->stance[7] = fread_number(fp);
				ch->stance[8] = fread_number(fp);
				ch->stance[9] = fread_number(fp);
				ch->stance[10] = fread_number(fp);
				fMatch = TRUE;
				break;
			}

			if(!str_cmp(word, "StatAbility"))
			{
				ch->pcdata->stat_ability[0] = fread_number(fp);
				ch->pcdata->stat_ability[1] = fread_number(fp);
				ch->pcdata->stat_ability[2] = fread_number(fp);
				ch->pcdata->stat_ability[3] = fread_number(fp);
				fMatch = TRUE;
				break;
			}

			if(!str_cmp(word, "StatAmount"))
			{
				ch->pcdata->stat_amount[0] = fread_number(fp);
				ch->pcdata->stat_amount[1] = fread_number(fp);
				ch->pcdata->stat_amount[2] = fread_number(fp);
				ch->pcdata->stat_amount[3] = fread_number(fp);
				fMatch = TRUE;
				break;
			}

			if(!str_cmp(word, "StatDuration"))
			{
				ch->pcdata->stat_duration[0] = fread_number(fp);
				ch->pcdata->stat_duration[1] = fread_number(fp);
				ch->pcdata->stat_duration[2] = fread_number(fp);
				ch->pcdata->stat_duration[3] = fread_number(fp);
				fMatch = TRUE;
				break;
			}


			if(!str_cmp(word, "Stats"))
			{
				for(sn = 0; sn < 12; sn++)
					ch->pcdata->stats[sn] = fread_number(fp);
				fMatch = TRUE;
				break;
			}

			break;

		case 'T':
			KEY("Trust", ch->trust, fread_number(fp));

			if(!str_cmp(word, "Title"))
			{
				ch->pcdata->title = fread_string(fp);
				if(isalpha(ch->pcdata->title[0]) || isdigit(ch->pcdata->title[0]))
				{
					sprintf(buf, " %s", ch->pcdata->title);
					free_string(ch->pcdata->title);
					ch->pcdata->title = str_dup(buf);
				}
				fMatch = TRUE;
				break;
			}

			if(!str_cmp(word, "Tw/l"))
			{
				ch->tlw = fread_number(fp);
				ch->tll = fread_number(fp);
				fMatch = TRUE;
				break;
			}
			break;

		case 'V':
			KEY("Vampaff", dummy, fread_number(fp));
			KEY("Vampgen", dummy, fread_number(fp));
			KEY("Vamppass", dummy, fread_number(fp));
			if(!str_cmp(word, "Vnum"))
			{
				ch->pIndexData = get_mob_index(fread_number(fp));
				fMatch = TRUE;
				break;
			}
			break;

		case 'W':
			if(!str_cmp(word, "Weapons"))
			{
				ch->wpn[0] = fread_number(fp);
				ch->wpn[1] = fread_number(fp);
				ch->wpn[2] = fread_number(fp);
				ch->wpn[3] = fread_number(fp);
				ch->wpn[4] = fread_number(fp);
				ch->wpn[5] = fread_number(fp);
				ch->wpn[6] = fread_number(fp);
				ch->wpn[7] = fread_number(fp);
				ch->wpn[8] = fread_number(fp);
				ch->wpn[9] = fread_number(fp);
				ch->wpn[10] = fread_number(fp);
				ch->wpn[11] = fread_number(fp);
				ch->wpn[12] = fread_number(fp);
				fMatch = TRUE;
				break;
			}
			KEY("Wimpy", ch->wimpy, fread_number(fp));
			KEY("Wolf", dummy, fread_number(fp));
			if(!str_cmp(word, "Wolfform"))
			{
				dummy = fread_number(fp);
				dummy = fread_number(fp);
				fMatch = TRUE;
				break;
			}
			break;
		}

		line++;
		if(!fMatch)
		{
			sprintf(buf, "Fread_char: no match. WORD: %s, approximate line: %li", word, line + 1);
			bug(buf, 0);
			fread_to_eol(fp);
		}
	}
}

void convert_old_new(CHAR_DATA * ch)
{
	char buf[MAX_STRING_LENGTH];

	ch->pcdata->class = 0;
	sprintf(buf, "%s has been avatar upgraded!", ch->name);
	log_string(buf);
	return;

}


void do_convert_pfiles(CHAR_DATA * ch, char *argument)
{
	DESCRIPTOR_DATA *d = 0;
	CHAR_DATA *vch = 0;
	char buf[MSL];
	char *pname;
	FILE *fpnames;
	FILE *fp;
	char name[256];
	long pfiles = 0;

	if(!argument || argument[0] == '\0')
	{
		send_to_char("Convertpfiles <binary/text>\n\r", ch);
		return;
	}

	if(!str_cmp(argument, "text"))
	{
		sprintf(buf, "ls %sbinary > binarynames.txt", PLAYER_DIR);
		system(buf);
		sprintf(buf, "tar czhf backupbinary.tgz %sbinary/*", PLAYER_DIR);
		system(buf);

		if((fpnames = fopen("binarynames.txt", "r")) == 0)
		{
			bug("Can't read binarynames.txt!", 0);
			return;
		}

		save_in_binary = 0;

		while(1)
		{
			pname = fread_word(fpnames);
			strcpy(name, pname);

			log_string(name);

			if(name[0] == EOF || name[0] == '\0')
				break;

			sprintf(buf, "%sbinary/%s", PLAYER_DIR, name);
			if((fp = fopen(buf, "rb")) == 0)
			{
				sprintf(buf, "Can't open %s for binary reading for conversion!", name);
				bug(buf, 0);
				continue;
			}

			fread(vch, sizeof(*vch), 1, fp);	// BUG!! INITIALIZE VCH
			fclose(fp);
			save_char_obj(vch);
			sprintf(buf, "rm %sbinary/%s", PLAYER_DIR, name);
			system(buf);
			pfiles++;
		}

		fclose(fpnames);

		sprintf(buf, "%li binary files converted.\n\r", pfiles);
		send_to_char(buf, ch);
		return;
	}
	if(!str_cmp(argument, "binary"))
	{
		sprintf(buf, "ls %s > textnames.txt", PLAYER_DIR);
		system(buf);
		sprintf(buf, "tar czhf backuptext.tgz %s*", PLAYER_DIR);
		system(buf);

		if((fpnames = fopen("textnames.txt", "r")) == 0)
		{
			bug("Can't read textnames.txt!", 0);
			return;
		}

		vch = ch;
		d = ch->desc;

		while(1)
		{
			pname = fread_word(fpnames);
			strcpy(name, pname);

			if(name[0] == EOF || name[0] == '\0')
				break;

			if(!str_cmp(name, "binary") || !str_cmp(name, "store")
			   || !str_cmp(name, "store") || !str_cmp(name, "deleted"))
				continue;

			save_in_binary = 0;
			load_char_obj(d, name);
			save_in_binary = 1;
			save_char_obj(d->character);
			free_char(d->character);
			sprintf(buf, "rm %s%s", PLAYER_DIR, &name[0]);
			system(buf);
			pfiles++;
		}

		fclose(fpnames);

		sprintf(buf, "%li text files converted.\n\r", pfiles);
		send_to_char(buf, ch);
		d->character = vch;
		return;
	}

	do_convert_pfiles(ch, "");	// default error message
}



// THIS IS THE END OF THE FILE THANKS
