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

#if !defined(macintosh)
extern int _filbuf args((FILE *));
#endif



/*
 * Globals.
 */
HELP_DATA *help_first;
HELP_DATA *help_last;

SHOP_DATA *shop_first;
SHOP_DATA *shop_last;

CHAR_DATA *char_free;
ASSIST_DATA *assist_free;
EXTRA_DESCR_DATA *extra_descr_free;
NOTE_DATA *note_free;
OBJ_DATA *obj_free;
PC_DATA *pcdata_free;
SPEC_OBJ_DATA *spec_obj_free;

char bug_buf[2 * MAX_INPUT_LENGTH];
CHAR_DATA *char_list;
char *help_greeting;
char *hack_greeting;
char **help_text_greeting;
char **help_ansi_greeting;
long greet_text_num = 0;
long greet_ansi_num = 0;
char log_buf[2 * MAX_INPUT_LENGTH];
KILL_DATA kill_table[MAX_LEVEL];
NOTE_DATA *note_list;
OBJ_DATA *object_list;
SPEC_OBJ_DATA *spec_obj_list;
TIME_INFO_DATA time_info;
WEATHER_DATA weather_info;

long gsn_swordtech;
long gsn_godgift;
long gsn_godcurse;
long gsn_bard;
long gsn_buffet;
long gsn_backstab;
long gsn_circle;
long gsn_tail;
long gsn_hide;
long gsn_peek;
long gsn_pick_lock;
long gsn_sneak;
long gsn_steal;
long gsn_inferno;
long gsn_blinky;
long gsn_spiderform;
long gsn_garotte;
long gsn_shred;

long gsn_disarm;
long gsn_fastdraw;
long gsn_berserk;
long gsn_punch;
long gsn_pummel;
long gsn_elbow;
long gsn_headbutt;
long gsn_sweep;
long gsn_knee;
long gsn_kick;
long gsn_hurl;
long gsn_rescue;
long gsn_track;
long gsn_polymorph;
long gsn_web;
long gsn_chill;
long gsn_tail;

long gsn_blindness;
long gsn_charm_person;
long gsn_curse;
long gsn_invis;
long gsn_mass_invis;
long gsn_poison;
long gsn_sleep;

long gsn_darkness;
long gsn_paradox;


long gsn_joust;
long gsn_shiroken;
long gsn_talon;
long gsn_drowfire;
long gsn_totalblind;		/* Monk Power - Loki */
long gsn_godbless;		/* Monk Power - Loki */
long gsn_llothbless;		/* drow bless -Sage */



extern void read_votes(void);
extern void read_trivia(void);


/*
 * Locals.
 */
MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];
OBJ_INDEX_DATA *obj_index_hash[MAX_KEY_HASH];
ROOM_INDEX_DATA *room_index_hash[MAX_KEY_HASH];
char *string_hash[MAX_KEY_HASH];

AREA_DATA *area_first;
AREA_DATA *area_last;
CLAN_DATA *clan_info;
LEADER_DATA *leader[6];
char *string_space;
char *top_string;
char str_empty[1];

long top_affect;
long top_area;
long top_rt;
long top_ed;
long top_exit;
long top_help;
long top_mob_index;
long top_obj_index;
long top_reset;
long top_room;
long top_shop;
long top_assist = 0;

// For Crashes
long debug_stage;
long debug_counter = 0;
long debug_last_room;
char *debug_last_character;
char *debug_last_command;
char *debug_procedure[20];
bool debug_dont_run;

// Crashes ^^

long top_affect;
long top_area;
long top_rt;
long top_ed;
long top_exit;
long top_help;
long top_mob_index;
long top_obj_index;
long top_reset;
long top_room;
long top_shop;


/*
 * Memory management.
 * Increase MAX_STRING if you have too.
 * Tune the others only if you understand what you're doing.
 */
//#define                       MAX_STRING      1048576
long MAX_STRING = 4194304;

#define			MAX_PERM_BLOCK	262144
#define			MAX_MEM_LIST	13

void *rgFreeList[MAX_MEM_LIST];
const long rgSizeList[MAX_MEM_LIST] = {
	16, 32, 64, 128, 256, 1024, 2048, 4096, 8192, 16384, 32768/* - 64*/, 65536, 131072
};

long nAllocString;
long sAllocString;
long nAllocPerm;
long sAllocPerm;



/*
 * Semi-locals.
 */
bool fBootDb;
FILE *fpArea;
char strArea[MAX_INPUT_LENGTH];


/*
 * Local booting procedures.
 */
void init_mm args((void));

void load_area args((FILE * fp));
void load_helps args((FILE * fp));
void load_mobiles args((FILE * fp));
void load_objects args((FILE * fp));
void load_resets args((FILE * fp));
void load_rooms args((FILE * fp));
void load_shops args((FILE * fp));
void load_specials args((FILE * fp));
void load_notes args((void));

void fix_exits args((void));

void reset_area args((AREA_DATA * pArea));
void give_supermob_artis args((void));
void throw_tokens args((void));

/*
 * Big mama top level function.
 */
void boot_db(bool fhotreboot)
{
	/*
	 * Init some data space stuff.
	 */
	{
		if((string_space = calloc(1, MAX_STRING)) == 0)
		{
			bug("Boot_db: can't alloc %li string space.", MAX_STRING);
			exit(1);
		}
		top_string = string_space;
		fBootDb = TRUE;
	}

	/*
	 * Init random number generator.
	 */
	{
		init_mm();
	}

	/*
	 * Set time and weather.
	 */
	{
		long lhour, lday, lmonth;

		lhour = (current_time - 650336715) / (PULSE_TICK / PULSE_PER_SECOND);
		time_info.hour = lhour % 24;
		lday = lhour / 24;
		time_info.day = lday % 35;
		lmonth = lday / 35;
		time_info.month = lmonth % 17;
		time_info.year = lmonth / 17;

		if(time_info.hour < 5)
			weather_info.sunlight = SUN_DARK;
		else if(time_info.hour < 6)
			weather_info.sunlight = SUN_RISE;
		else if(time_info.hour < 19)
			weather_info.sunlight = SUN_LIGHT;
		else if(time_info.hour < 20)
			weather_info.sunlight = SUN_SET;
		else
			weather_info.sunlight = SUN_DARK;

		weather_info.change = 0;
		weather_info.mmhg = 960;
		if(time_info.month >= 7 && time_info.month <= 12)
			weather_info.mmhg += number_range(1, 50);
		else
			weather_info.mmhg += number_range(1, 80);

		if(weather_info.mmhg <= 980)
			weather_info.sky = SKY_LIGHTNING;
		else if(weather_info.mmhg <= 1000)
			weather_info.sky = SKY_RAINING;
		else if(weather_info.mmhg <= 1020)
			weather_info.sky = SKY_CLOUDY;
		else
			weather_info.sky = SKY_CLOUDLESS;

	}

	/*
	 * Assign gsn's for skills which have them.
	 */
	{
		long sn;

		for(sn = 0; sn < MAX_SKILL; sn++)
		{
			if(skill_table[sn].pgsn != 0)
				*skill_table[sn].pgsn = sn;
		}
	}

	/*
	 * Read in all the area files.
	 */
	{
		FILE *fpList;

		if((fpList = fopen(AREA_LIST, "r")) == 0)
		{
			perror(AREA_LIST);
			exit(1);
		}

		for(;;)
		{
			strcpy(strArea, fread_word(fpList));
			if(strArea[0] == '$')
				break;

			if(strArea[0] == '-')
			{
				fpArea = stdin;
			}
			else
			{
				if((fpArea = fopen(strArea, "r")) == 0)
				{
					perror(strArea);
					exit(1);
				}
			}

			for(;;)
			{
				char *word;

				if(fread_letter(fpArea) != '#')
				{
					bug("Boot_db: # not found.", 0);
					exit(1);
				}

				word = fread_word(fpArea);

				if(word[0] == '$')
					break;
				else if(!str_cmp(word, "AREA"))
					load_area(fpArea);
				else if(!str_cmp(word, "HELPS"))
					load_helps(fpArea);
				else if(!str_cmp(word, "MOBILES"))
					load_mobiles(fpArea);
				else if(!str_cmp(word, "OBJECTS"))
					load_objects(fpArea);
				else if(!str_cmp(word, "RESETS"))
					load_resets(fpArea);
				else if(!str_cmp(word, "ROOMS"))
					load_rooms(fpArea);
				else if(!str_cmp(word, "SHOPS"))
					load_shops(fpArea);
				else if(!str_cmp(word, "SPECIALS"))
					load_specials(fpArea);
				else
				{
					bug("Boot_db: bad section name.", 0);
					exit(1);
				}
			}

			if(fpArea != stdin)
				fclose(fpArea);
			fpArea = 0;
		}
		fclose(fpList);
	}

	/* give king pin artifacts */

	/*
	 * Fix up exits.
	 * Declare db booting over.
	 * Reset all areas once.
	 * Load up the notes file.
	 */
	{
		fix_exits();
		fBootDb = FALSE;
		load_bans();
		load_notes();
		load_disabled();
	}

	read_trivia();
	read_votes();
	read_kingdoms();

	if(fhotreboot)
		hotreboot_recover();

// various changing of the database structure
	{
		AREA_DATA *pArea;
		AREA_DATA *high;
		MOB_INDEX_DATA *mob;
		OBJ_INDEX_DATA *obj;
		AFFECT_DATA *af;
		long random;
		long num, avg;
		long i;
		long areas = 0;

		for(pArea = area_first; pArea; pArea = pArea->next)
		{
			num = 0;
			avg = 0;

			if(pArea->name[0] == '*')
				random = 0;
			else if(pArea->name[0] == '&')
				random = number_range(1, 60);
			else
				random = number_range(60, 950);

			if(random > 0 && number_percent() < random / 10)
				random -= random / 2;

			for(i = 1; i < MAX_VNUM; i++)
			{
				if(random)
					break;

				if((mob = get_mob_index(i)) != 0
				   && mob->vnum >= pArea->lvnum && mob->vnum <= pArea->hvnum)
				{
					avg += mob->level;
					num++;
				}
			}

			if(random)
				pArea->level = random;
			else
				pArea->level = UMAX(1, avg) / UMAX(1, num);
			areas++;
		}

		do
		{
			random = number_range(1,areas);
			for( i = 1, pArea = area_first; i <= random; i++ )
				pArea = pArea->next;
		} while( pArea->name[0] == '*' || pArea->name[0] == '&' );

		// one super area each reboot
		pArea->level = number_range(2500,6000);

		for(i = 1; i <= areas; i++)
		{
			high = 0;

			for(pArea = area_first; pArea; pArea = pArea->next)
			{
				if(pArea->place > 0)
					continue;
				if(!high)
					high = pArea;
				if(high->level < pArea->level)
					high = pArea;
			}
			high->place = i;
		}

		{
			AREA_DATA *newareas[500];	// stupid windows...
			long x;

			i = 0;
			for(pArea = area_first; pArea; pArea = pArea->next)
				newareas[i++] = pArea;

			// bubble sort!
			for(i = 0; i < areas; i++)
			{
				for(x = 0; x < areas; x++)
				{
					if(newareas[i]->place > newareas[x]->place)
					{
						pArea = newareas[i];
						newareas[i] = newareas[x];
						newareas[x] = pArea;
					}
				}
			}

			area_first = newareas[0];

			// make the new list!
			for(i = 1, pArea = area_first; i < areas; i++)
			{
				pArea->next = newareas[i];
				pArea = newareas[i];
			}
			pArea->next = 0;
		}

		for(i = 0; i < MAX_VNUM; i++)
		{
			if((obj = get_obj_index(i)) != 0)
			{
				for(af = obj->affected; af; af = af->next)
				{
					if((af->location == APPLY_HIT
					    || af->location == APPLY_MANA
					    || af->location == APPLY_MOVE) && af->modifier < 0)
					{
						af->modifier = 0;
					}
				}
			}
		}
	}

	area_update();
	give_supermob_artis();
	throw_tokens();
	return;
}

/*
 * Snarf an 'area' header line.
 */
void load_area(FILE * fp)
{
	AREA_DATA *pArea;

	pArea = alloc_perm(sizeof(*pArea));
	pArea->reset_first = 0;
	pArea->reset_last = 0;
	pArea->name = fread_string(fp);
	pArea->age = 15;
	pArea->nplayer = 0;
	pArea->lvnum = MAX_VNUM + 1;
	pArea->hvnum = 0;
	pArea->level = 0;
	pArea->place = 0;

	if(area_first == 0)
		area_first = pArea;
	if(area_last != 0)
		area_last->next = pArea;
	area_last = pArea;
	pArea->next = 0;

	top_area++;
	return;
}



/*
 * Snarf a help section.
 */
void load_helps(FILE * fp)
{
	HELP_DATA *pHelp;
	long i = 0;

	for(;;)
	{
		pHelp = alloc_perm(sizeof(*pHelp));
		pHelp->level = fread_number(fp);
		pHelp->keyword = fread_string(fp);
		if(pHelp->keyword[0] == '$')
			break;
		pHelp->text = fread_string(fp);

		if(!str_cmp(pHelp->keyword, "greeting"))
			help_greeting = pHelp->text;
		else if(!str_cmp(pHelp->keyword, "hackgreet"))
			hack_greeting = pHelp->text;
		else if(!str_cmp(pHelp->keyword, "ansigreeting"))
			greet_ansi_num++;
		else if(!str_cmp(pHelp->keyword, "textgreeting"))
			greet_text_num++;


		if(help_first == 0)
			help_first = pHelp;
		if(help_last != 0)
			help_last->next = pHelp;

		help_last = pHelp;
		pHelp->next = 0;
		top_help++;
	}

	help_text_greeting = malloc(sizeof(char *) * greet_text_num);
	help_ansi_greeting = malloc(sizeof(char *) * greet_ansi_num);

	i = 0;
	for(pHelp = help_first; pHelp; pHelp = pHelp->next)
	{
		if(!str_cmp(pHelp->keyword, "ansigreeting"))
		{
			help_ansi_greeting[i++] = pHelp->text;
		}
	}
	i = 0;
	for(pHelp = help_first; pHelp; pHelp = pHelp->next)
	{
		if(!str_cmp(pHelp->keyword, "textgreeting"))
		{
			help_text_greeting[i++] = pHelp->text;
		}
	}


	return;
}



/*
 * Snarf a mob section.
 */
void load_mobiles(FILE * fp)
{
	MOB_INDEX_DATA *pMobIndex;

	for(;;)
	{
		long vnum;
		char letter;
		long iHash;

		letter = fread_letter(fp);
		if(letter != '#')
		{
			bug("Load_mobiles: # not found.", 0);
			exit(1);
		}

		vnum = fread_number(fp);
		if(vnum == 0)
			break;

		fBootDb = FALSE;
		if(get_mob_index(vnum) != 0)
		{
			bug("Load_mobiles: vnum %li duplicated.", vnum);
			exit(1);
		}
		fBootDb = TRUE;

		pMobIndex = alloc_perm(sizeof(*pMobIndex));
		pMobIndex->vnum = vnum;
		pMobIndex->player_name = fread_string(fp);
		pMobIndex->short_descr = fread_string(fp);
		pMobIndex->long_descr = fread_string(fp);
		pMobIndex->description = fread_string(fp);

		pMobIndex->long_descr[0] = UPPER(pMobIndex->long_descr[0]);
		pMobIndex->description[0] = UPPER(pMobIndex->description[0]);

		pMobIndex->act = fread_number(fp) | ACT_IS_NPC;
		pMobIndex->affected_by = fread_number(fp);
		pMobIndex->itemaffect = 0;
		pMobIndex->pShop = 0;
		pMobIndex->alignment = fread_number(fp);
		letter = fread_letter(fp);
		pMobIndex->level = fread_number(fp);

		/*
		 * The unused stuff is for imps who want to use the old-style
		 * stats-in-files method.
		 */
		pMobIndex->hitroll = fread_number(fp);	/* Unused */
		pMobIndex->ac = fread_number(fp);	/* Unused */
		pMobIndex->hitnodice = fread_number(fp);	/* Unused */
		/* 'd'          */ fread_letter(fp);
		/* Unused */
		pMobIndex->hitsizedice = fread_number(fp);	/* Unused */
		/* '+'          */ fread_letter(fp);
		/* Unused */
		pMobIndex->hitplus = fread_number(fp);	/* Unused */
		pMobIndex->damnodice = fread_number(fp);	/* Unused */
		/* 'd'          */ fread_letter(fp);
		/* Unused */
		pMobIndex->damsizedice = fread_number(fp);	/* Unused */
		/* '+'          */ fread_letter(fp);
		/* Unused */
		pMobIndex->damplus = fread_number(fp);	/* Unused */
		pMobIndex->gold = fread_number(fp);	/* Unused */
		/* xp can't be used! */ fread_number(fp);
		/* Unused */
		/* position     */ fread_number(fp);
		/* Unused */
		/* start pos    */ fread_number(fp);
		/* Unused */

		/*
		 * Back to meaningful values.
		 */
		pMobIndex->sex = fread_number(fp);
		pMobIndex->area = area_last;

		if(letter != 'S')
		{
			bug("Load_mobiles: vnum %li non-S.", vnum);
			exit(1);
		}

		iHash = vnum % MAX_KEY_HASH;
		pMobIndex->next = mob_index_hash[iHash];
		mob_index_hash[iHash] = pMobIndex;
		top_mob_index++;
		kill_table[URANGE(0, pMobIndex->level, MAX_LEVEL - 1)].number++;
	}

	return;
}



/*
 * Snarf an obj section.
 */
void load_objects(FILE * fp)
{
	OBJ_INDEX_DATA *pObjIndex;

	for(;;)
	{
		long vnum;
		char letter;
		long iHash;

		letter = fread_letter(fp);
		if(letter != '#')
		{
			bug("Load_objects: # not found.", 0);
			exit(1);
		}

		vnum = fread_number(fp);
		if(vnum == 0)
			break;

		fBootDb = FALSE;
		if(get_obj_index(vnum) != 0)
		{
			bug("Load_objects: vnum %li duplicated.", vnum);
			exit(1);
		}
		fBootDb = TRUE;

		pObjIndex = alloc_perm(sizeof(*pObjIndex));
		pObjIndex->vnum = vnum;
		pObjIndex->name = fread_string(fp);
		pObjIndex->short_descr = fread_string(fp);
		pObjIndex->description = fread_string(fp);
		/* Action description */ fread_string(fp);

		pObjIndex->short_descr[0] = LOWER(pObjIndex->short_descr[0]);
		pObjIndex->description[0] = UPPER(pObjIndex->description[0]);

		pObjIndex->item_type = fread_number(fp);
		pObjIndex->extra_flags = fread_number(fp);
		pObjIndex->wear_flags = fread_number(fp);
		pObjIndex->value[0] = fread_number(fp);
		pObjIndex->value[1] = fread_number(fp);
		pObjIndex->value[2] = fread_number(fp);
		pObjIndex->value[3] = fread_number(fp);
		pObjIndex->weight = fread_number(fp);
		pObjIndex->cost = fread_number(fp);	/* Unused */
		pObjIndex->affected = 0;
		pObjIndex->extra_descr = 0;
		pObjIndex->chpoweron = str_dup("");
		pObjIndex->chpoweroff = str_dup("");
		pObjIndex->chpoweruse = str_dup("");
		pObjIndex->victpoweron = str_dup("");
		pObjIndex->victpoweroff = str_dup("");
		pObjIndex->victpoweruse = str_dup("");
		pObjIndex->spectype = 0;
		pObjIndex->specpower = 0;
		/* Cost per day */ fread_number(fp);
/*
	if ( pObjIndex->item_type == ITEM_POTION )
	    SET_BIT(pObjIndex->extra_flags, ITEM_NODROP);
*/
		for(;;)
		{
			char letter;

			letter = fread_letter(fp);

			if(letter == 'A')
			{
				AFFECT_DATA *paf;

				paf = alloc_perm(sizeof(*paf));
				paf->type = -1;
				paf->duration = -1;
				paf->location = fread_number(fp);
				paf->modifier = fread_number(fp);
				paf->bitvector = 0;
				paf->next = pObjIndex->affected;
				pObjIndex->affected = paf;
				top_affect++;
			}

			else if(letter == 'E')
			{
				EXTRA_DESCR_DATA *ed;

				ed = alloc_perm(sizeof(*ed));
				ed->keyword = fread_string(fp);
				ed->description = fread_string(fp);
				ed->next = pObjIndex->extra_descr;
				pObjIndex->extra_descr = ed;
				top_ed++;
			}

			else if(letter == 'Q')
			{
				pObjIndex->chpoweron = fread_string(fp);
				pObjIndex->chpoweroff = fread_string(fp);
				pObjIndex->chpoweruse = fread_string(fp);
				pObjIndex->victpoweron = fread_string(fp);
				pObjIndex->victpoweroff = fread_string(fp);
				pObjIndex->victpoweruse = fread_string(fp);
				pObjIndex->spectype = fread_number(fp);
				pObjIndex->specpower = fread_number(fp);
			}

			else
			{
				ungetc(letter, fp);
				break;
			}
		}

		/*
		 * Translate spell "slot numbers" to internal "skill numbers."
		 */
		switch (pObjIndex->item_type)
		{
		case ITEM_PILL:
		case ITEM_POTION:
		case ITEM_SCROLL:
			pObjIndex->value[1] = slot_lookup(pObjIndex->value[1]);
			pObjIndex->value[2] = slot_lookup(pObjIndex->value[2]);
			pObjIndex->value[3] = slot_lookup(pObjIndex->value[3]);
			break;

		case ITEM_STAFF:
		case ITEM_WAND:
			pObjIndex->value[3] = slot_lookup(pObjIndex->value[3]);
			break;
		}

		iHash = vnum % MAX_KEY_HASH;
		pObjIndex->next = obj_index_hash[iHash];
		obj_index_hash[iHash] = pObjIndex;
		top_obj_index++;
	}

	return;
}



/*
 * Snarf a reset section.
 */
void load_resets(FILE * fp)
{
	RESET_DATA *pReset;

	if(area_last == 0)
	{
		bug("Load_resets: no #AREA seen yet.", 0);
		exit(1);
	}

	for(;;)
	{
		ROOM_INDEX_DATA *pRoomIndex;
		EXIT_DATA *pexit;
		char letter;

		if((letter = fread_letter(fp)) == 'S')
			break;

		if(letter == '*')
		{
			fread_to_eol(fp);
			continue;
		}

		pReset = alloc_perm(sizeof(*pReset));
		pReset->command = letter;
		/* if_flag */ fread_number(fp);
		pReset->arg1 = fread_number(fp);
		pReset->arg2 = fread_number(fp);
		pReset->arg3 = (letter == 'G' || letter == 'R') ? 0 : fread_number(fp);
		fread_to_eol(fp);

		/*
		 * Validate parameters.
		 * We're calling the index functions for the side effect.
		 */
		switch (letter)
		{
		default:
			bug("Load_resets: bad command '%c'.", letter);
			exit(1);
			break;

		case 'M':
			get_mob_index(pReset->arg1);
			get_room_index(pReset->arg3);
			break;

		case 'O':
			get_obj_index(pReset->arg1);
			get_room_index(pReset->arg3);
			break;

		case 'P':
			get_obj_index(pReset->arg1);
			get_obj_index(pReset->arg3);
			break;

		case 'G':
		case 'E':
			get_obj_index(pReset->arg1);
			break;

		case 'D':
			pRoomIndex = get_room_index(pReset->arg1);

			if(pReset->arg2 < 0
			   || pReset->arg2 > 5
			   || (pexit = pRoomIndex->exit[pReset->arg2]) == 0 || !IS_SET(pexit->exit_info, EX_ISDOOR))
			{
				bug("Load_resets: 'D': exit %li not door.", pReset->arg2);
				exit(1);
			}

			if(pReset->arg3 < 0 || pReset->arg3 > 2)
			{
				bug("Load_resets: 'D': bad 'locks': %li.", pReset->arg3);
				exit(1);
			}

			break;

		case 'R':
			pRoomIndex = get_room_index(pReset->arg1);

			if(pReset->arg2 < 0 || pReset->arg2 > 6)
			{
				bug("Load_resets: 'R': bad exit %li.", pReset->arg2);
				exit(1);
			}

			break;
		}

		if(area_last->reset_first == 0)
			area_last->reset_first = pReset;
		if(area_last->reset_last != 0)
			area_last->reset_last->next = pReset;

		area_last->reset_last = pReset;
		pReset->next = 0;
		top_reset++;
	}

	return;
}



/*
 * Snarf a room section.
 */
void load_rooms(FILE * fp)
{
	ROOM_INDEX_DATA *pRoomIndex;

	if(area_last == 0)
	{
		bug("Load_resets: no #AREA seen yet.", 0);
		exit(1);
	}

	for(;;)
	{
		long vnum;
		char letter;
		long door;
		long iHash;

		letter = fread_letter(fp);
		if(letter != '#')
		{
			bug("Load_rooms: # not found.", 0);
			exit(1);
		}

		vnum = fread_number(fp);
		if(vnum == 0)
			break;

		fBootDb = FALSE;
		if(get_room_index(vnum) != 0)
		{
			bug("Load_rooms: vnum %li duplicated.", vnum);
			exit(1);
		}
		fBootDb = TRUE;

		if(area_last->lvnum > vnum)
			area_last->lvnum = vnum;
		if(area_last->hvnum < vnum)
			area_last->hvnum = vnum;

		pRoomIndex = alloc_perm(sizeof(*pRoomIndex));
		pRoomIndex->people = 0;
		pRoomIndex->contents = 0;
		pRoomIndex->extra_descr = 0;
		pRoomIndex->area = area_last;
		pRoomIndex->vnum = vnum;
		pRoomIndex->name = fread_string(fp);
		pRoomIndex->description = fread_string(fp);
		/* Area number */ fread_number(fp);
		pRoomIndex->room_flags = fread_number(fp);
		pRoomIndex->sector_type = fread_number(fp);
		pRoomIndex->light = 0;
		pRoomIndex->blood = 0;
		pRoomIndex->roomtext = 0;
		for(door = 0; door <= 4; door++)
		{
			pRoomIndex->track[door] = str_dup("");
			pRoomIndex->track_dir[door] = 0;
		}
		for(door = 0; door <= 5; door++)
			pRoomIndex->exit[door] = 0;

		for(;;)
		{
			letter = fread_letter(fp);

			if(letter == 'S')
				break;

			if(letter == 'D')
			{
				EXIT_DATA *pexit;
				long locks;

				door = fread_number(fp);
				if(door < 0 || door > 5)
				{
					bug("Fread_rooms: vnum %li has bad door number.", vnum);
					exit(1);
				}

				pexit = alloc_perm(sizeof(*pexit));
				pexit->description = fread_string(fp);
				pexit->keyword = fread_string(fp);
				pexit->exit_info = 0;
				locks = fread_number(fp);
				pexit->key = fread_number(fp);
				pexit->vnum = fread_number(fp);

				switch (locks)
				{
				case 1:
					pexit->exit_info = EX_ISDOOR;
					break;
				case 2:
					pexit->exit_info = EX_ISDOOR | EX_PICKPROOF;
					break;
				}

				pRoomIndex->exit[door] = pexit;
				top_exit++;
			}
			else if(letter == 'E')
			{
				EXTRA_DESCR_DATA *ed;

				ed = alloc_perm(sizeof(*ed));
				ed->keyword = fread_string(fp);
				ed->description = fread_string(fp);
				ed->next = pRoomIndex->extra_descr;
				pRoomIndex->extra_descr = ed;
				top_ed++;
			}
			else if(letter == 'T')
			{
				ROOMTEXT_DATA *rt;
				long test;

				rt = alloc_perm(sizeof(*rt));
				rt->input = fread_string(fp);
				rt->output = fread_string(fp);
				rt->choutput = fread_string(fp);
				rt->name = fread_string(fp);
				rt->type = fread_number(fp);
				test = rt->power = fread_number(fp);
				rt->mob = fread_number(fp);
				rt->next = pRoomIndex->roomtext;
				pRoomIndex->roomtext = rt;
				top_rt++;

			}
			else
			{
				bug("Load_rooms: vnum %li has flag not 'DES'.", vnum);
				exit(1);
			}
		}

		iHash = vnum % MAX_KEY_HASH;
		pRoomIndex->next = room_index_hash[iHash];
		room_index_hash[iHash] = pRoomIndex;
		top_room++;
	}

	return;
}



/*
 * Snarf a shop section.
 */
void load_shops(FILE * fp)
{
	SHOP_DATA *pShop;

	for(;;)
	{
		MOB_INDEX_DATA *pMobIndex;
		long iTrade;

		pShop = alloc_perm(sizeof(*pShop));
		pShop->keeper = fread_number(fp);
		if(pShop->keeper == 0)
			break;
		for(iTrade = 0; iTrade < MAX_TRADE; iTrade++)
			pShop->buy_type[iTrade] = fread_number(fp);
		pShop->profit_buy = fread_number(fp);
		pShop->profit_sell = fread_number(fp);
		pShop->open_hour = fread_number(fp);
		pShop->close_hour = fread_number(fp);
		fread_to_eol(fp);
		pMobIndex = get_mob_index(pShop->keeper);
		pMobIndex->pShop = pShop;

		if(shop_first == 0)
			shop_first = pShop;
		if(shop_last != 0)
			shop_last->next = pShop;

		shop_last = pShop;
		pShop->next = 0;
		top_shop++;
	}

	return;
}



/*
 * Snarf spec proc declarations.
 */
void load_specials(FILE * fp)
{
	for(;;)
	{
//      MOB_INDEX_DATA *pMobIndex;
		char letter;

		switch (letter = fread_letter(fp))
		{
		case 'S':
			return;

		default:
			bug("Load_specials: letter '%c' not *MS.", letter);
			exit(1);
		}

		fread_to_eol(fp);
	}
}



/*
 * Snarf notes file.
 */
void load_notes(void)
{
	FILE *fp;
	NOTE_DATA *pnotelast;

	if((fp = fopen(NOTE_FILE, "r")) == 0)
		return;

	pnotelast = 0;
	for(;;)
	{
		NOTE_DATA *pnote;
		char letter;

		do
		{
			letter = getc(fp);
			if(feof(fp))
			{
				fclose(fp);
				return;
			}
		}
		while(isspace(letter));
		ungetc(letter, fp);

		pnote = alloc_perm(sizeof(*pnote));

		if(str_cmp(fread_word(fp), "sender"))
			break;
		pnote->sender = fread_string(fp);

		if(str_cmp(fread_word(fp), "date"))
			break;
		pnote->date = fread_string(fp);

		if(str_cmp(fread_word(fp), "to"))
			break;
		pnote->to_list = fread_string(fp);

		if(str_cmp(fread_word(fp), "subject"))
			break;
		pnote->subject = fread_string(fp);

		if(str_cmp(fread_word(fp), "text"))
			break;
		pnote->text = fread_string(fp);

		/* Fix from the Themoog from Xania */
		pnote->next = 0;

		if(note_list == 0)
			note_list = pnote;
		else
			pnotelast->next = pnote;

		pnotelast = pnote;
	}

	strcpy(strArea, NOTE_FILE);
	fpArea = fp;
	bug("Load_notes: bad key word.", 0);
	exit(1);
	return;
}



/*
 * Translate all room exits from virtual to real.
 * Has to be done after all rooms are read in.
 * Check for bad reverse exits.
 */
void fix_exits(void)
{
	extern const long rev_dir[];
	char buf[MAX_STRING_LENGTH];
	ROOM_INDEX_DATA *pRoomIndex;
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;
	long iHash;
	long door;

	for(iHash = 0; iHash < MAX_KEY_HASH; iHash++)
	{
		for(pRoomIndex = room_index_hash[iHash]; pRoomIndex != 0; pRoomIndex = pRoomIndex->next)
		{
			bool fexit;

			fexit = FALSE;
			for(door = 0; door <= 5; door++)
			{
				if((pexit = pRoomIndex->exit[door]) != 0)
				{
					fexit = TRUE;
					if(pexit->vnum <= 0)
						pexit->to_room = 0;
					else
						pexit->to_room = get_room_index(pexit->vnum);
				}
			}

			if(!fexit)
				SET_BIT(pRoomIndex->room_flags, ROOM_NO_MOB);
		}
	}

	for(iHash = 0; iHash < MAX_KEY_HASH; iHash++)
	{
		for(pRoomIndex = room_index_hash[iHash]; pRoomIndex != 0; pRoomIndex = pRoomIndex->next)
		{
			for(door = 0; door <= 5; door++)
			{
				if((pexit = pRoomIndex->exit[door]) != 0
				   && (to_room = pexit->to_room) != 0
				   && (pexit_rev = to_room->exit[rev_dir[door]]) != 0
				   && pexit_rev->to_room != pRoomIndex)
				{
					sprintf(buf, "Fix_exits: %li:%li -> %li:%li -> %li.",
						pRoomIndex->vnum, door,
						to_room->vnum, rev_dir[door],
						(pexit_rev->to_room == 0) ? 0 : pexit_rev->to_room->vnum);
/*		    bug( buf, 0 ); */
				}
			}
		}
	}

	return;
}



/*
 * Repopulate areas periodically.
 */
void area_update(void)
{
	AREA_DATA *pArea;

	for(pArea = area_first; pArea != 0; pArea = pArea->next)
	{
		CHAR_DATA *pch;

		if(++pArea->age < 3)
			continue;

		/*
		 * Check for PC's.
		 */
		if(pArea->nplayer > 0 && pArea->age == 15 - 1)
		{
			for(pch = char_list; pch != 0; pch = pch->next)
			{
				if(!IS_NPC(pch) && IS_AWAKE(pch) && pch->in_room != 0 && pch->in_room->area == pArea)
				{
// you hear a dead donkey utter the words 'spam'.
				}

			}
		}

		/*
		 * Check age and reset.
		 * Note: Mud School resets every 3 minutes (not 15).
		 */
		if(pArea->nplayer == 0 || pArea->age >= 15)
		{
			ROOM_INDEX_DATA *pRoomIndex;

			reset_area(pArea);
			pArea->age = number_range(0, 3);
			pRoomIndex = get_room_index(ROOM_VNUM_SCHOOL);
			if(pRoomIndex != 0 && pArea == pRoomIndex->area)
				pArea->age = 15 - 3;
		}
	}

	return;
}



/*
 * Reset one area.
 */
void reset_area(AREA_DATA * pArea)
{
	RESET_DATA *pReset;
	CHAR_DATA *mob;
	bool last;
	long level;

	mob = 0;
	last = TRUE;
	level = 0;
	for(pReset = pArea->reset_first; pReset != 0; pReset = pReset->next)
	{
		ROOM_INDEX_DATA *pRoomIndex;
		MOB_INDEX_DATA *pMobIndex;
		OBJ_INDEX_DATA *pObjIndex;
		OBJ_INDEX_DATA *pObjToIndex;
		EXIT_DATA *pexit;
		OBJ_DATA *obj;
		OBJ_DATA *obj_to;

		switch (pReset->command)
		{
		default:
			bug("Reset_area: bad command %c.", pReset->command);
			break;

		case 'M':
			if((pMobIndex = get_mob_index(pReset->arg1)) == 0)
			{
				bug("Reset_area: 'M': bad vnum %li.", pReset->arg1);
				continue;
			}

			if((pRoomIndex = get_room_index(pReset->arg3)) == 0)
			{
				bug("Reset_area: 'R': bad vnum %li.", pReset->arg3);
				continue;
			}

			level = URANGE(0, pMobIndex->level - 2, LEVEL_HERO);
			if(pMobIndex->count >= pReset->arg2)
			{
				last = FALSE;
				break;
			}

			mob = create_mobile(pMobIndex);

			/*
			 * Check for pet shop.
			 */
			{
				ROOM_INDEX_DATA *pRoomIndexPrev;

				pRoomIndexPrev = get_room_index(pRoomIndex->vnum - 1);
				if(pRoomIndexPrev != 0 && IS_SET(pRoomIndexPrev->room_flags, ROOM_PET_SHOP))
					SET_BIT(mob->act, ACT_PET);
			}

			if(room_is_dark(pRoomIndex))
				SET_BIT(mob->affected_by, AFF_INFRARED);


			char_to_room(mob, pRoomIndex);
			last = TRUE;
			break;

		case 'O':
			if((pObjIndex = get_obj_index(pReset->arg1)) == 0)
			{
				bug("Reset_area: 'O': bad vnum %li.", pReset->arg1);
				continue;
			}

			if((pRoomIndex = get_room_index(pReset->arg3)) == 0)
			{
				bug("Reset_area: 'R': bad vnum %li.", pReset->arg3);
				continue;
			}

			if(pArea->nplayer > 0 || count_obj_list(pObjIndex, pRoomIndex->contents) > 0)
			{
				last = FALSE;
				break;
			}

			obj = create_object(pObjIndex, number_range(1, 50));
			obj->cost = 0;
			obj_to_room(obj, pRoomIndex);
			last = TRUE;
			break;

		case 'P':
			if((pObjIndex = get_obj_index(pReset->arg1)) == 0)
			{
				bug("Reset_area: 'P': bad vnum %li.", pReset->arg1);
				continue;
			}

			if((pObjToIndex = get_obj_index(pReset->arg3)) == 0)
			{
				bug("Reset_area: 'P': bad vnum %li.", pReset->arg3);
				continue;
			}

			if(pArea->nplayer > 0
			   || (obj_to = get_obj_type(pObjToIndex)) == 0
			   || obj_to->in_room == 0 || count_obj_list(pObjIndex, obj_to->contains) > 0)
			{
				last = FALSE;
				break;
			}

			obj = create_object(pObjIndex, number_range(1, 50));
			obj_to_obj(obj, obj_to);
			last = TRUE;
			break;

		case 'G':
		case 'E':
			if((pObjIndex = get_obj_index(pReset->arg1)) == 0)
			{
				bug("Reset_area: 'E' or 'G': bad vnum %li.", pReset->arg1);
				continue;
			}

			if(!last)
				break;

			if(mob == 0)
			{
				bug("Reset_area: 'E' or 'G': null mob for vnum %li.", pReset->arg1);
				last = FALSE;
				break;
			}

			if(mob->pIndexData->pShop != 0)
			{
				long olevel;

				switch (pObjIndex->item_type)
				{
				default:
					olevel = 0;
					break;
				case ITEM_PILL:
					olevel = number_range(0, 10);
					break;
				case ITEM_POTION:
					olevel = number_range(0, 10);
					break;
				case ITEM_SCROLL:
					olevel = number_range(5, 15);
					break;
				case ITEM_WAND:
					olevel = number_range(10, 20);
					break;
				case ITEM_STAFF:
					olevel = number_range(15, 25);
					break;
				case ITEM_ARMOR:
					olevel = number_range(5, 15);
					break;
				case ITEM_WEAPON:
					olevel = number_range(5, 15);
					break;
				}

				obj = create_object(pObjIndex, olevel);
				SET_BIT(obj->extra_flags, ITEM_INVENTORY);
			}
			else
			{
				obj = create_object(pObjIndex, number_range(1, 50));
			}
			obj_to_char(obj, mob);
			if(pReset->command == 'E')
				equip_char(mob, obj, pReset->arg3);
			last = TRUE;
			break;

		case 'D':
			if((pRoomIndex = get_room_index(pReset->arg1)) == 0)
			{
				bug("Reset_area: 'D': bad vnum %li.", pReset->arg1);
				continue;
			}

			if((pexit = pRoomIndex->exit[pReset->arg2]) == 0)
				break;

			switch (pReset->arg3)
			{
			case 0:
				REMOVE_BIT(pexit->exit_info, EX_CLOSED);
				REMOVE_BIT(pexit->exit_info, EX_LOCKED);
				break;

			case 1:
				SET_BIT(pexit->exit_info, EX_CLOSED);
				REMOVE_BIT(pexit->exit_info, EX_LOCKED);
				break;

			case 2:
				SET_BIT(pexit->exit_info, EX_CLOSED);
				SET_BIT(pexit->exit_info, EX_LOCKED);
				break;
			}

			last = TRUE;
			break;

		case 'R':
			if((pRoomIndex = get_room_index(pReset->arg1)) == 0)
			{
				bug("Reset_area: 'R': bad vnum %li.", pReset->arg1);
				continue;
			}

			{
				long d0;
				long d1;

				for(d0 = 0; d0 < pReset->arg2 - 1; d0++)
				{
					d1 = number_range(d0, pReset->arg2 - 1);
					pexit = pRoomIndex->exit[d0];
					pRoomIndex->exit[d0] = pRoomIndex->exit[d1];
					pRoomIndex->exit[d1] = pexit;
				}
			}
			break;
		}
	}

	return;
}



/*
 * Create an instance of a mobile.
 */
CHAR_DATA *create_mobile(MOB_INDEX_DATA * pMobIndex)
{
	CHAR_DATA *mob;
	long level = 0;

	if(pMobIndex == 0)
	{
		bug("Create_mobile: 0 pMobIndex.", 0);
		exit(1);
	}

	if(char_free == 0)
	{
		mob = alloc_perm(sizeof(*mob));
	}
	else
	{
		mob = char_free;
		char_free = char_free->next;
	}

	clear_char(mob);
	mob->pIndexData = pMobIndex;

	mob->hunting = str_dup("");
	mob->lord = str_dup("");
	mob->clan = str_dup("");
	mob->morph = str_dup("");
	mob->createtime = str_dup("");
	mob->lasttime = str_dup("");
	mob->lasthost = str_dup("");
	mob->powertype = str_dup("");
	mob->poweraction = str_dup("");
	mob->pload = str_dup("");
	mob->prompt = str_dup("");
	mob->cprompt = str_dup("");

	mob->name = pMobIndex->player_name;
	mob->short_descr = pMobIndex->short_descr;
	mob->long_descr = pMobIndex->long_descr;
	mob->description = pMobIndex->description;

	mob->spec_obj = pMobIndex->spec_obj;

	if(pMobIndex->vnum == 29157)
		level = pMobIndex->level;
	else
		level = number_range(pMobIndex->area->level - 100, pMobIndex->area->level + 100);

	mob->level = UMAX(1,level);
	mob->max_hit = number_range(mob->level * 40, mob->level * 80);
	mob->hit = mob->max_hit;

	mob->home = 3001;
	mob->form = 32767;
	mob->act = pMobIndex->act;
	mob->affected_by = pMobIndex->affected_by;
	mob->alignment = pMobIndex->alignment;
	mob->sex = pMobIndex->sex;

	mob->armor = interpolate(mob->level, 100, -100);

	if(mob->level > 5000)
		mob->armor = -30000;


	mob->hitroll = mob->level;
	mob->damroll = mob->level;

	if(number_percent() < 50)
		SET_BIT(mob->flag2, AFF2_NOPORTAL);

	if(number_percent() < 25)
		SET_BIT(mob->flag2, AFF2_QUESTHOLDER);

	/*
	 * Insert in list.
	 */
	mob->next = char_list;
	char_list = mob;
	pMobIndex->count++;
	return mob;
}



/*
 * Create an instance of an object.
 */
OBJ_DATA *create_object(OBJ_INDEX_DATA * pObjIndex, long level)
{
	static OBJ_DATA obj_zero;
	OBJ_DATA *obj;

	if(pObjIndex == 0)
	{
		bug("Create_object: 0 pObjIndex.", 0);
		return 0;
	}

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
	obj->pIndexData = pObjIndex;
	obj->in_room = 0;
	obj->level = level;
	obj->wear_loc = -1;

	obj->name = pObjIndex->name;
	obj->short_descr = pObjIndex->short_descr;
	obj->description = pObjIndex->description;

	obj->chpoweron = pObjIndex->chpoweron;
	obj->chpoweroff = pObjIndex->chpoweroff;
	obj->chpoweruse = pObjIndex->chpoweruse;
	obj->victpoweron = pObjIndex->victpoweron;
	obj->victpoweroff = pObjIndex->victpoweroff;
	obj->victpoweruse = pObjIndex->victpoweruse;
//taken out for class eq	obj->spectype = pObjIndex->spectype;
	obj->specpower = pObjIndex->specpower;
	obj->questmaker = str_dup("");
	obj->questowner = str_dup("");

	obj->chobj = 0;

	obj->quest = 0;
	obj->points = 0;

	obj->item_type = pObjIndex->item_type;
	obj->extra_flags = pObjIndex->extra_flags;
	obj->wear_flags = pObjIndex->wear_flags;
	obj->value[0] = pObjIndex->value[0];
	obj->value[1] = pObjIndex->value[1];
	obj->value[2] = pObjIndex->value[2];
	obj->value[3] = pObjIndex->value[3];
	obj->weight = pObjIndex->weight;
	obj->cost = number_fuzzy(10) * number_fuzzy(level) * number_fuzzy(level);

	if(is_artifact(obj->pIndexData->vnum))
	{
		SET_BIT(obj->quest, QUEST_ARTIFACT);
		obj->condition = 100;
		obj->toughness = 100;
		obj->resistance = 1;
		obj->level = 59;
		obj->cost = 1000000;
	}
	else if(is_relic(obj->pIndexData->vnum))
	{
		SET_BIT(obj->quest, QUEST_RELIC);
		obj->condition = 100;
		obj->toughness = 100;
		obj->resistance = 1;
	}
	else
	{
		obj->condition = 100;
		obj->toughness = 5;
		obj->resistance = 25;
	}

	/*
	 * Mess with object properties.
	 */
	switch (obj->item_type)
	{
	default:
		bug("Read_object: vnum %li bad type.", pObjIndex->vnum);
		break;

	case ITEM_LIGHT:
	case ITEM_TREASURE:
	case ITEM_FURNITURE:
	case ITEM_TRASH:
	case ITEM_CONTAINER:
	case ITEM_DRINK_CON:
	case ITEM_KEY:
	case ITEM_FOOD:
	case ITEM_BOAT:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	case ITEM_FOUNTAIN:
	case ITEM_PORTAL:
	case ITEM_EGG:
	case ITEM_VOODOO:
	case ITEM_STAKE:
	case ITEM_MISSILE:
	case ITEM_AMMO:
	case ITEM_QUEST:
	case ITEM_QUESTCARD:
	case ITEM_QUESTMACHINE:
	case ITEM_SYMBOL:
	case ITEM_BOOK:
	case ITEM_PAGE:
	case ITEM_TOOL:
	case ITEM_INSTRUMENT:
		break;

	case ITEM_SCROLL:
		obj->value[0] = number_fuzzy(obj->value[0]);
		break;

	case ITEM_WAND:
	case ITEM_STAFF:
		obj->value[0] = number_fuzzy(obj->value[0]);
		obj->value[1] = number_fuzzy(obj->value[1]);
		obj->value[2] = obj->value[1];
		break;

	case ITEM_WEAPON:
		if(!IS_SET(obj->quest, QUEST_ARTIFACT) && !IS_SET(obj->quest, QUEST_RELIC))
		{
			obj->value[1] = number_range(1, 10);
			obj->value[2] = number_range((obj->value[1] + 1), (obj->value[1] * 2));
		}
		break;

	case ITEM_ARMOR:
		if(!IS_SET(obj->quest, QUEST_ARTIFACT) && !IS_SET(obj->quest, QUEST_RELIC))
			obj->value[0] = number_range(5, 15);
		break;

	case ITEM_POTION:
	case ITEM_PILL:
		obj->value[0] = number_fuzzy(number_fuzzy(obj->value[0]));
		break;

	case ITEM_MONEY:
		obj->value[0] = obj->cost;
		break;
	}

	obj->next = object_list;
	object_list = obj;
	pObjIndex->count++;

	return obj;
}


bool is_relic(int vnum)
{
	if((vnum == 29519 || vnum == 29520)
	|| (vnum >= 29600 && vnum <= 29729)
	|| (vnum >= 27650 && vnum <= 27661)
	|| (vnum >= 30333 && vnum <= 30343))
		return TRUE;
	return FALSE;
}

bool is_artifact(int vnum)
{
	if(vnum >= 29500 && vnum != 29519 && vnum <= 29599
&& vnum != 29520)
		return TRUE;
	return FALSE;
}

/*
 * Clear a new character.
 */
void clear_char(CHAR_DATA * ch)
{
	static CHAR_DATA ch_zero;

	*ch = ch_zero;
	ch->name = &str_empty[0];
	ch->short_descr = &str_empty[0];
	ch->long_descr = &str_empty[0];
	ch->description = &str_empty[0];
	ch->lord = &str_empty[0];
	ch->clan = &str_empty[0];
	ch->morph = &str_empty[0];
	ch->createtime = &str_empty[0];
	ch->lasthost = &str_empty[0];
	ch->lasttime = &str_empty[0];
	ch->powertype = &str_empty[0];
	ch->poweraction = &str_empty[0];
	ch->pload = &str_empty[0];
	ch->prompt = &str_empty[0];
	ch->cprompt = &str_empty[0];
	ch->hunting = &str_empty[0];

	ch->logon = current_time;
	ch->armor = 10;
	ch->position = POS_STANDING;
	ch->practice = 0;
	ch->hit = 500;
	ch->max_hit = 500;
	ch->mana = 500;
	ch->max_mana = 500;
	ch->move = 500;
	ch->max_move = 500;
	ch->master = 0;
	ch->leader = 0;
	ch->fighting = 0;
	ch->mount = 0;
	ch->wizard = 0;
	ch->paradox[0] = 0;
	ch->paradox[1] = 0;
	ch->paradox[2] = 0;
	ch->damcap[0] = 1000;
	ch->damcap[1] = 0;
	return;
}



/*
 * Free a character.
 */
void free_char(CHAR_DATA * ch)
{
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	AFFECT_DATA *paf;
	AFFECT_DATA *paf_next;

	for(obj = ch->carrying; obj != 0; obj = obj_next)
	{
		obj_next = obj->next_content;
		extract_obj(obj);
	}

	for(paf = ch->affected; paf != 0; paf = paf_next)
	{
		paf_next = paf->next;
		affect_remove(ch, paf);
	}

	free_string(ch->name);
	free_string(ch->short_descr);
	free_string(ch->long_descr);
	free_string(ch->description);
	free_string(ch->lord);
	free_string(ch->clan);
	free_string(ch->morph);
	free_string(ch->createtime);
	free_string(ch->lasttime);
	free_string(ch->lasthost);
	free_string(ch->powertype);
	free_string(ch->poweraction);
	free_string(ch->pload);
	free_string(ch->prompt);
	free_string(ch->cprompt);
	free_string(ch->hunting);

	if(ch->pcdata != 0)
	{
		free_string(ch->pcdata->pwd);
		free_string(ch->pcdata->bamfin);
		free_string(ch->pcdata->bamfout);
		free_string(ch->pcdata->title);
		free_string(ch->pcdata->conception);
		free_string(ch->pcdata->parents);
		free_string(ch->pcdata->cparents);
		free_string(ch->pcdata->marriage);
		free_string(ch->pcdata->email);
		ch->pcdata->next = pcdata_free;
		pcdata_free = ch->pcdata;
	}

	if(reset_mud)
	{
		free_mem(ch, sizeof(CHAR_DATA));
	}
	else
	{
		ch->next = char_free;
		char_free = ch;
	}
	return;
}



/*
 * Get an extra description from a list.
 */
char *get_extra_descr(const char *name, EXTRA_DESCR_DATA * ed)
{
	for(; ed != 0; ed = ed->next)
	{
		if(is_name(name, ed->keyword))
			return ed->description;
	}
	return 0;
}



/*
 * Translates mob virtual number to its mob index struct.
 * Hash table lookup.
 */
MOB_INDEX_DATA *get_mob_index(long vnum)
{
	MOB_INDEX_DATA *pMobIndex;

	for(pMobIndex = mob_index_hash[vnum % MAX_KEY_HASH]; pMobIndex != 0; pMobIndex = pMobIndex->next)
	{
		if(pMobIndex->vnum == vnum)
			return pMobIndex;
	}

	if(fBootDb)
	{
		bug("Get_mob_index: bad vnum %li.", vnum);
		exit(1);
	}

	return 0;
}



/*
 * Translates mob virtual number to its obj index struct.
 * Hash table lookup.
 */
OBJ_INDEX_DATA *get_obj_index(long vnum)
{
	OBJ_INDEX_DATA *pObjIndex;

	for(pObjIndex = obj_index_hash[vnum % MAX_KEY_HASH]; pObjIndex != 0; pObjIndex = pObjIndex->next)
	{
		if(pObjIndex->vnum == vnum)
			return pObjIndex;
	}

	if(fBootDb)
	{
		bug("Get_obj_index: bad vnum %li.", vnum);
		exit(1);
	}

	return 0;
}



/*
 * Translates mob virtual number to its room index struct.
 * Hash table lookup.
 */
ROOM_INDEX_DATA *get_room_index(long vnum)
{
	ROOM_INDEX_DATA *pRoomIndex;

	for(pRoomIndex = room_index_hash[vnum % MAX_KEY_HASH]; pRoomIndex != 0; pRoomIndex = pRoomIndex->next)
	{
		if(pRoomIndex->vnum == vnum)
			return pRoomIndex;
	}

	if(fBootDb)
	{
		bug("Get_room_index: bad vnum %li.", vnum);
		exit(1);
	}

	return 0;
}



/*
 * Read a letter from a file.
 */
char fread_letter(FILE * fp)
{
	char c;

	do
	{
		c = getc(fp);
	}
	while(isspace(c));

	return c;
}



/*
 * Read a number from a file.
 */
long fread_number(FILE * fp)
{
	long number;
	bool sign;
	char c;

	do
	{
		c = getc(fp);
	}
	while(isspace(c));

	number = 0;

	sign = FALSE;
	if(c == '+')
	{
		c = getc(fp);
	}
	else if(c == '-')
	{
		sign = TRUE;
		c = getc(fp);
	}

	if(!isdigit(c))
	{
		bug("Fread_number: bad format.", 0);
		exit(1);
	}

	while(isdigit(c))
	{
		number = number * 10 + c - '0';
		c = getc(fp);
	}

	if(sign)
		number = 0 - number;

	if(c == '|')
		number += fread_number(fp);
	else if(c != ' ')
		ungetc(c, fp);

	return number;
}


char *fread_line(FILE * fp)
{
	static char buf[MAX_STRING_LENGTH];
	char c;
	long i;

	buf[0] = '\0';

	for(i = 0; i < MAX_STRING_LENGTH; i++)
	{
		c = getc(fp);

		if(c == EOF)
			return (char *) EOF;

		if(c == '\n')
		{
			buf[i] = '\0';
			return buf;
		}
		buf[i] = c;
	}

	sprintf(buf, "fread_line line too long: %s", buf);
	log_string(buf);
	exit(1);
}

/*
 * Read and allocate space for a string from a file.
 * These strings are read-only and shared.
 * Strings are hashed:
 *   each string prepended with hash pointer to prev string,
 *   hash code is simply the string length.
 * This function takes 40% to 50% of boot-up time.
 */
char *fread_string(FILE * fp)
{
	char *plast;
	char c;

	plast = top_string + sizeof(char *);
	if(plast > &string_space[MAX_STRING - MAX_STRING_LENGTH])
	{
		bug("Fread_string: MAX_STRING %li exceeded, doubling.", MAX_STRING);
		MAX_STRING *= 2;
		if(plast > &string_space[MAX_STRING - MAX_STRING_LENGTH])
		{
			bug("Fread_string: doubling MAX_STRING not good enough, memory bug. aborting.", 0);
			exit(1);
		}
	}

	/*
	 * Skip blanks.
	 * Read first char.
	 */
	do
	{
		c = getc(fp);
	}
	while(isspace(c));

	if((*plast++ = c) == '~')
		return &str_empty[0];

	for(;;)
	{
		/*
		 * Back off the char type lookup,
		 *   it was too dirty for portability.
		 *   -- Furey
		 */
		switch (*plast = getc(fp))
		{
		default:
			plast++;
			break;

		case EOF:
			bug("Fread_string: EOF", 0);
			return &str_empty[0];
			break;

		case '\n':
			plast++;
			*plast++ = '\r';
			break;

		case '\r':
			break;

		case '~':
			plast++;
			{
				union
				{
					char *pc;
					char rgc[sizeof(char *)];
				}
				u1;
				long ic;
				long iHash;
				char *pHash;
				char *pHashPrev;
				char *pString;

				plast[-1] = '\0';
				iHash = UMIN(MAX_KEY_HASH - 1, plast - 1 - top_string);
				for(pHash = string_hash[iHash]; pHash; pHash = pHashPrev)
				{
					for(ic = 0; ic < sizeof(char *); ic++)
						u1.rgc[ic] = pHash[ic];
					pHashPrev = u1.pc;
					pHash += sizeof(char *);

					if(top_string[sizeof(char *)] == pHash[0]
					   && !strcmp(top_string + sizeof(char *) + 1, pHash + 1))
						return pHash;
				}

				if(fBootDb)
				{
					pString = top_string;
					top_string = plast;
					u1.pc = string_hash[iHash];
					for(ic = 0; ic < sizeof(char *); ic++)
						pString[ic] = u1.rgc[ic];
					string_hash[iHash] = pString;

					nAllocString += 1;
					sAllocString += top_string - pString;
					return pString + sizeof(char *);
				}
				else
				{
					return str_dup(top_string + sizeof(char *));
				}
			}
		}
	}
}



/*
 * Read to end of line (for comments).
 */
void fread_to_eol(FILE * fp)
{
	char c;

	do
	{
		c = getc(fp);
	}
	while(c != '\n' && c != '\r');

	do
	{
		c = getc(fp);
	}
	while(c == '\n' || c == '\r');

	ungetc(c, fp);
	return;
}



/*
 * Read one word (into static buffer).
 */
char *fread_word(FILE * fp)
{
	static char word[MAX_INPUT_LENGTH];
	char *pword;
	char cEnd;

	do
	{
		cEnd = getc(fp);
	}
	while(isspace(cEnd));

	if(cEnd == '\'' || cEnd == '"')
	{
		pword = word;
	}
	else
	{
		word[0] = cEnd;
		pword = word + 1;
		cEnd = ' ';
	}

	for(; pword < word + MAX_INPUT_LENGTH; pword++)
	{
		*pword = getc(fp);
		if(*pword == EOF)
		{
			*pword = '\0';
			return word;
		}
		if(cEnd == ' ' ? isspace(*pword) : *pword == cEnd)
		{
			if(cEnd == ' ')
				ungetc(*pword, fp);
			*pword = '\0';
			return word;
		}
	}

	bug("Fread_word: word too long.", 0);
	log_string(word);
//    exit( 1 );
	return 0;
}



/*
 * Allocate some ordinary memory,
 *   with the expectation of freeing it someday.
 */
void *alloc_mem(long sMem, char *reason)
{
	void *pMem;
	long iList;

	for(iList = 0; iList < MAX_MEM_LIST; iList++)
	{
		if(sMem <= rgSizeList[iList])
			break;
	}

	if(iList == MAX_MEM_LIST)
	{
		bug("Alloc_mem: size %li too large.", sMem);


		exit(1);
	}

	if(rgFreeList[iList] == 0)
	{
		pMem = alloc_perm(rgSizeList[iList]);
	}
	else
	{
		pMem = rgFreeList[iList];
		rgFreeList[iList] = *((void **) rgFreeList[iList]);
	}

	return pMem;
}



/*
 * Free some memory.
 * Recycle it back onto the free list for blocks of that size.
 */
void free_mem(void *pMem, long sMem)
{
	long iList;

	for(iList = 0; iList < MAX_MEM_LIST; iList++)
	{
		if(sMem <= rgSizeList[iList])
			break;
	}

	if(iList == MAX_MEM_LIST)
	{
		bug("Free_mem: size %li too large.", sMem);
		exit(1);
	}

	*((void **) pMem) = rgFreeList[iList];
	rgFreeList[iList] = pMem;

	return;
}



/*
 * Allocate some permanent memory.
 * Permanent memory is never freed,
 *   pointers into it may be copied safely.
 */
void *alloc_perm(long sMem)
{
	static char *pMemPerm;
	static long iMemPerm;
	void *pMem;

	while(sMem % sizeof(long) != 0)
		sMem++;
	if(sMem > MAX_PERM_BLOCK)
	{
		bug("Alloc_perm: %li too large.", sMem);
		exit(1);
	}

	if(pMemPerm == 0 || iMemPerm + sMem > MAX_PERM_BLOCK)
	{
		iMemPerm = 0;
		if((pMemPerm = calloc(1, MAX_PERM_BLOCK)) == 0)
		{
			perror("Alloc_perm");
			exit(1);
		}
	}

	pMem = pMemPerm + iMemPerm;
	iMemPerm += sMem;
	nAllocPerm += 1;
	sAllocPerm += sMem;
	return pMem;
}



/*
 * Duplicate a string into dynamic memory.
 * Fread_strings are read-only and shared.
 */
char *str_dup(const char *str)
{
	char *str_new;

	if(str[0] == '\0')
		return &str_empty[0];

	if(str >= string_space && str < top_string)
		return (char *) str;

	str_new = alloc_mem(strlen(str) + 1, "str_dup");
	strcpy(str_new, str);

	return str_new;
}



/*
 * Free a string.
 * Null is legal here to simplify callers.
 * Read-only shared strings are not touched.
 */
void free_string(char *pstr)
{
	if(pstr == 0 || pstr == &str_empty[0] || (pstr >= string_space && pstr < top_string))
		return;

	free_mem(pstr, strlen(pstr) + 1);
	return;
}

void do_findmobs(CHAR_DATA *ch, char *argument)
{
	char buf[MSL];
	AREA_DATA *area;
	char *area_buf;
	char area_buf2[MSL];
	char trash[MSL];	
	char arg[MIL];
	char sep[MSL];
	long num = 0, i = 0;
	CHAR_DATA *mch;
	CHAR_DATA *mch_next;

	if (argument[0] == '\0')
	{
		do_areas(ch,"");
		stc("Which area were you interested in?\n\r",ch);	
		stc("Syntax: findmobs <area name>\n\r",ch);
		return;
	}
	
	one_argument(argument,arg);

	for (area = area_first; area; area = area->next)
	{
		     // parse area name from area builder
                        area_buf = area_buf2;
                        sprintf(area_buf2, "%s", area->name);
                        area_buf = one_argument(area_buf, trash);
			
			if (!str_prefix(arg,area_buf))
				break;
	}
	
	if (!area)
	{
		do_areas(ch,"");
		stc("The area you tried to pick was not on the list.\n\r",ch);
		stc("Syntax: findmobs <area name>\n\r",ch);
		return;
	}

	sprintf(buf,"Mobs in area: %s\n\r", area_buf);
	stc(buf,ch);

	for(mch = char_list; mch != 0; mch = mch_next)
        {       
                mch_next = mch->next;
         
                if(!IS_NPC(mch) || area != mch->in_room->area)
                        continue;

		for(i = 0; i < (long) (33 - strlen(mch->name)); i++)
                        sep[i] = '.';
                sep[i] = '\0';

		sprintf(buf,"{GMob{x: {W%s{x.%s{YRoom{x: {W%s{x\n\r",mch->name,sep,mch->in_room->name);
		stc(buf,ch);
		num++;
        }
	
	sprintf(buf,"A total of %li mobs in this area.\n\r",num);
	stc(buf,ch);
	return;
}

void do_areas(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char sep[256];
	AREA_DATA *pArea;
	long i = 0;
	long min = 0;
	long num = 0;

	if(argument && argument[0] != '\0' && is_number(argument))
		min = atoi(argument);

	buf[0] = '\0';
	sprintf(buf, "{c%-31s  {R%-5s  {G%-5s  %-5s{X  %-5s\n\r\n\r", "Area Creator and Name", "Level", "Lvnum", 
		"Hvnum", "Rooms");
	send_to_char(buf, ch);

	for(pArea = area_first; pArea; pArea = pArea->next, num++)
	{
		if(((pArea->level <= 1 || pArea->lvnum < 100)
		    && (argument && str_cmp(argument, "all"))) || pArea->level < min)
			continue;

		for(i = 0; i < (long) (33 - strlen(pArea->name)); i++)
			sep[i] = '.';
		sep[i] = '\0';

		sprintf(buf, "{c%s%s{R%-5li  {G%-5li--%-5li{X  %-5li\n\r",
			pArea->name, sep, pArea->level, pArea->lvnum, pArea->hvnum, pArea->hvnum-pArea->lvnum);
		send_to_char(buf, ch);

	}

	sprintf(buf, "A total of %li areas.\n\r", num);
	send_to_char(buf, ch);

	return;
}



void do_memory(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];

	sprintf(buf, "Assists %5li\n\r", top_assist);
	send_to_char(buf, ch);
	sprintf(buf, "Affects %5li\n\r", top_affect);
	send_to_char(buf, ch);
	sprintf(buf, "Areas   %5li\n\r", top_area);
	send_to_char(buf, ch);
	sprintf(buf, "RmTxt   %5li\n\r", top_rt);
	send_to_char(buf, ch);
	sprintf(buf, "ExDes   %5li\n\r", top_ed);
	send_to_char(buf, ch);
	sprintf(buf, "Exits   %5li\n\r", top_exit);
	send_to_char(buf, ch);
	sprintf(buf, "Helps   %5li\n\r", top_help);
	send_to_char(buf, ch);
	sprintf(buf, "Mobs    %5li\n\r", top_mob_index);
	send_to_char(buf, ch);
	sprintf(buf, "Objs    %5li\n\r", top_obj_index);
	send_to_char(buf, ch);
	sprintf(buf, "Resets  %5li\n\r", top_reset);
	send_to_char(buf, ch);
	sprintf(buf, "Rooms   %5li\n\r", top_room);
	send_to_char(buf, ch);
	sprintf(buf, "Shops   %5li\n\r", top_shop);
	send_to_char(buf, ch);

	sprintf(buf, "Strings %5li strings of %7li bytes (max %li).\n\r", nAllocString, sAllocString, MAX_STRING);
	send_to_char(buf, ch);

	sprintf(buf, "Perms   %5li blocks  of %7li bytes.\n\r", nAllocPerm, sAllocPerm);
	send_to_char(buf, ch);

	return;
}



/*
 * Stick a little fuzz on a number.
 */
long number_fuzzy(long number)
{
	switch (number_bits(2))
	{
	case 0:
		number -= 1;
		break;
	case 3:
		number += 1;
		break;
	}

	return UMAX(1, number);
}



/*
 * Generate a random number.
 */
long number_range(long from, long to)
{
	long power;
	long number;

	if((to = to - from + 1) <= 1)
		return from;

	for(power = 2; power < to; power <<= 1)
		;

	while((number = number_mm() & (power - 1)) >= to)
		;

	return from + number;
}



/*
 * Generate a percentile roll.
 */
long number_percent(void)
{
	long percent;

	while((percent = number_mm() & (128 - 1)) > 99)
		;

	return 1 + percent;
}



/*
 * Generate a random door.
 */
long number_door(void)
{
	long door;

	while((door = number_mm() & (8 - 1)) > 5)
		;

	return door;
}



long number_bits(long width)
{
	return number_mm() & ((1 << width) - 1);
}



/*
 * I've gotten too many bad reports on OS-supplied random number generators.
 * This is the Mitchell-Moore algorithm from Knuth Volume II.
 * Best to leave the constants alone unless you've read Knuth.
 * -- Furey
 */
static long rgiState[2 + 55];

void init_mm()
{
	long *piState;
	long iState;

	piState = &rgiState[2];

	piState[-2] = 55 - 55;
	piState[-1] = 55 - 24;

	piState[0] = ((long) current_time) & ((1 << 30) - 1);
	piState[1] = 1;
	for(iState = 2; iState < 55; iState++)
	{
		piState[iState] = (piState[iState - 1] + piState[iState - 2]) & ((1 << 30) - 1);
	}
	return;
}



long number_mm(void)
{
	long *piState;
	long iState1;
	long iState2;
	long iRand;

	piState = &rgiState[2];
	iState1 = piState[-2];
	iState2 = piState[-1];
	iRand = (piState[iState1] + piState[iState2]) & ((1 << 30) - 1);
	piState[iState1] = iRand;
	if(++iState1 == 55)
		iState1 = 0;
	if(++iState2 == 55)
		iState2 = 0;
	piState[-2] = iState1;
	piState[-1] = iState2;
	return iRand >> 6;
}



/*
 * Roll some dice.
 */
long dice(long number, long size)
{
	long idice;
	long sum;

	switch (size)
	{
	case 0:
		return 0;
	case 1:
		return number;
	}

	for(idice = 0, sum = 0; idice < number; idice++)
		sum += number_range(1, size);

	return sum;
}



/*
 * Simple linear interpolation.
 */
long interpolate(long level, long value_00, long value_32)
{
	return value_00 + level * (value_32 - value_00) / 32;
}



/*
 * Removes the tildes from a string.
 * Used for player-entered strings that go into disk files.
 */
void smash_tilde(char *str)
{
	for(; *str != '\0'; str++)
	{
		if(*str == '~')
			*str = '-';
	}

	return;
}



/*
 * Compare strings, case insensitive.
 * Return TRUE if different
 *   (compatibility with historical functions).
 */

bool str_cmp(const char *astr, const char *bstr)
{
	char buf[MAX_STRING_LENGTH];

	if(astr == 0)
	{
		sprintf(buf, "Str_cmp: null astr with bstr: %s.", bstr);
		bug(buf, 0);
		return TRUE;
	}

	if(bstr == 0)
	{
		bug("Str_cmp: null bstr.", 0);
		return TRUE;
	}

	for(; *astr || *bstr; astr++, bstr++)
	{
		if(LOWER(*astr) != LOWER(*bstr))
			return TRUE;
	}

	return FALSE;
}



/*
 * Compare strings, case insensitive, for prefix matching.
 * Return TRUE if astr not a prefix of bstr
 *   (compatibility with historical functions).
 */
bool str_prefix(const char *astr, const char *bstr)
{
	if(astr == 0)
	{
		bug("Strn_cmp: null astr.", 0);
		return TRUE;
	}

	if(bstr == 0)
	{
		bug("Strn_cmp: null bstr.", 0);
		return TRUE;
	}

	for(; *astr; astr++, bstr++)
	{
		if(LOWER(*astr) != LOWER(*bstr))
			return TRUE;
	}

	return FALSE;
}



/*
 * Compare strings, case insensitive, for match anywhere.
 * Returns TRUE is astr not part of bstr.
 *   (compatibility with historical functions).
 */
bool str_infix(const char *astr, const char *bstr)
{
	long sstr1;
	long sstr2;
	long ichar;
	char c0;

	if((c0 = LOWER(astr[0])) == '\0')
		return FALSE;

	sstr1 = strlen(astr);
	sstr2 = strlen(bstr);

	for(ichar = 0; ichar <= sstr2 - sstr1; ichar++)
	{
		if(c0 == LOWER(bstr[ichar]) && !str_prefix(astr, bstr + ichar))
			return FALSE;
	}

	return TRUE;
}



/*
 * Compare strings, case insensitive, for suffix matching.
 * Return TRUE if astr not a suffix of bstr
 *   (compatibility with historical functions).
 */
bool str_suffix(const char *astr, const char *bstr)
{
	long sstr1;
	long sstr2;

	sstr1 = strlen(astr);
	sstr2 = strlen(bstr);
	if(sstr1 <= sstr2 && !str_cmp(astr, bstr + sstr2 - sstr1))
		return FALSE;
	else
		return TRUE;
}



/*
 * Returns an initial-capped string.
 */
char *capitalize(const char *str)
{
	static char strcap[MAX_STRING_LENGTH];
	long i;

	for(i = 0; str[i] != '\0'; i++)
		strcap[i] = LOWER(str[i]);
	strcap[i] = '\0';
	strcap[0] = UPPER(strcap[0]);
	return strcap;
}



/*
 * Append a string to a file.
 */
void append_file(CHAR_DATA * ch, char *file, char *str)
{
	FILE *fp;

	if(IS_NPC(ch) || str[0] == '\0')
		return;

	fclose(fpReserve);
	if((fp = fopen(file, "a")) == 0)
	{
		perror(file);
		send_to_char("Could not open the file!\n\r", ch);
	}
	else
	{
		fprintf(fp, "[%5li] %s: %s\n", ch->in_room ? ch->in_room->vnum : 0, ch->name, str);
		fclose(fp);
	}

	fpReserve = fopen(NULL_FILE, "r");
	return;
}

void reverse_append(const char *filename, const char *txt)
{
//      char buf[MAX_STRING_LENGTH];
	FILE *newfile;

	fclose(fpReserve);
	if((newfile = fopen("trashfile", "w+")) == 0)
	{
		bug("reverse_append farked up opening new file", 0);
		fpReserve = fopen(NULL_FILE, "r");
		return;
	}

	fprintf(newfile, "%s\n", txt);
//      sprintf(buf,"cat anewtrashfile %s > trashfile",filename);
//      system("cat anewtrashfile chats.txt > trashfile");
	system("cat chats.txt >> trashfile");
	fclose(newfile);
	fpReserve = fopen(NULL_FILE, "r");
//      sprintf(buf,"mv trashfile %s",filename);
//      system(buf);
}




/*
 * Reports a bug.
 */
void bug(const char *str, long param)
{
	char buf[MAX_STRING_LENGTH];
	FILE *fp;
	static long times = 0;

	if(times < 5000)
		times++;
	else
		return;

	if(fpArea != 0)
	{
		long iLine;
		long iChar;

		if(fpArea == stdin)
		{
			iLine = 0;
		}
		else
		{
			iChar = ftell(fpArea);
			fseek(fpArea, 0, 0);
			for(iLine = 0; ftell(fpArea) < iChar; iLine++)
			{
				while(getc(fpArea) != '\n')
					;
			}
			fseek(fpArea, iChar, 0);
		}

		sprintf(buf, "[*****] FILE: %s LINE: %li", strArea, iLine);
		log_string(buf);

		if((fp = fopen("shutdown.txt", "a")) != 0)
		{
			fprintf(fp, "[*****] %s\n", buf);
			fclose(fp);
		}
	}

	strcpy(buf, "[*****] BUG: ");
	sprintf(buf + strlen(buf), str, param);
	log_string(buf);

	fclose(fpReserve);
	if((fp = fopen(BUG_FILE, "a")) != 0)
	{
		fprintf(fp, "%s\n", buf);
		fclose(fp);
	}
	fpReserve = fopen(NULL_FILE, "r");

	return;
}



/*
 * Writes a string to the log.
 */
void log_string(const char *str)
{
	char *strtime;
	char logout[MAX_STRING_LENGTH];

	strtime = ctime(&current_time);
	strtime[strlen(strtime) - 1] = '\0';
	fprintf(stderr, "%s :: %s\n", strtime, str);
	strcpy(logout, str);
	logchan(logout);
	return;
}



/*
 * This function is here to aid in debugging.
 * If the last expression in a function is another function call,
 *   gcc likes to generate a JMP instead of a CALL.
 * This is called "tail chaining."
 * It hoses the debugger call stack for that call.
 * So I make this the last call in certain critical functions,
 *   where I really need the call stack to be right for debugging!
 *
 * If you don't understand this, then LEAVE IT ALONE.
 * Don't remove any calls to tail_chain anywhere.
 *
 * -- Furey
 */
void tail_chain(void)
{
	return;
}

void throw_tokens(void)
{
	CHAR_DATA *ch;

	for(ch = char_list;ch;ch = ch->next)
		if(!IS_NPC(ch))
			continue;

	do_repeat(char_list,"50 token 25");
	do_drop(char_list,"all");
	do_scatter(char_list,"");

	// now 1 point tokens to hide the 25's
	do_repeat(char_list,"100 token 1");
	do_drop(char_list,"all");
	do_scatter(char_list,"");
}

void give_supermob_artis(void)
{
	CHAR_DATA *ch;
	OBJ_DATA *obj;
	long num;
	long no_more_artis_geez_borlak = 0;

	for(ch = char_list; ch; ch = ch->next)
	{
		if(!IS_NPC(ch))
			continue;

		if(ch->pIndexData->vnum == 21597)
			break;
	}
	if(!ch)			// no king pin!
		return;

	for(num = 1; num < number_range(1, 7); num++)
	{
		if(no_more_artis_geez_borlak > 0)
			break;
		obj = create_object(get_obj_index(number_range(29500, 29519)), 0);
		if(!obj)	// infernal medallion
			obj = create_object(get_obj_index(29521), 0);
		obj_to_char(obj, ch);
		obj->questmaker = str_dup("Godwars");
		obj->questowner = str_dup("Godwars");
		no_more_artis_geez_borlak = 1;
	}

	ch->max_hit += 50000;
	ch->hit = ch->max_hit;
	ch->hitroll = 32000;
	ch->damroll = 32000;
	ch->max_move = 32000;
	ch->move = ch->max_move;
	do_wear(ch, "all");
	do_get(ch, "all");
	return;
}


// THIS IS THE END OF THE FILE THANKS
