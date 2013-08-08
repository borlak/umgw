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

#include "os.h"
#include "clans.h"
#include "kingdom.h"
#include "specials.h"

/*
 * Accommodate old non-Ansi compilers.
 */
#if defined(TRADITIONAL)
#define const
#define args( list )			( )
#define DECLARE_DO_FUN( fun )		void fun( )
#define DECLARE_SPEC_FUN( fun )		bool fun( )
#define DECLARE_SPELL_FUN( fun )	void fun( )
#else
#define args( list )			list
#define DECLARE_DO_FUN( fun )		DO_FUN    fun
#define DECLARE_SPEC_FUN( fun )		SPEC_FUN  fun
#define DECLARE_SPELL_FUN( fun )	SPELL_FUN fun
#endif
#define stc(buf, ch)	send_to_char(buf,ch)


/*
 * Short scalar types.
 * Diavolo reports AIX compiler has bugs with short types.
 */
#if	!defined(FALSE)
#define FALSE	 0
#endif

#if	!defined(TRUE)
#define TRUE	 1
#endif

typedef long bool;

#include "player.h"
//#include "old.h"

/*
 * Structure types.
 */
typedef struct clan_data CLAN_DATA;
typedef struct leader_data LEADER_DATA;
typedef struct affect_data AFFECT_DATA;
typedef struct area_data AREA_DATA;
typedef struct ban_data BAN_DATA;
typedef struct char_data CHAR_DATA;
typedef struct descriptor_data DESCRIPTOR_DATA;
typedef struct exit_data EXIT_DATA;
typedef struct extra_descr_data EXTRA_DESCR_DATA;
typedef struct help_data HELP_DATA;
typedef struct kill_data KILL_DATA;
typedef struct mob_index_data MOB_INDEX_DATA;
typedef struct note_data NOTE_DATA;
typedef struct obj_data OBJ_DATA;
typedef struct obj_index_data OBJ_INDEX_DATA;
typedef struct pc_data PC_DATA;
typedef struct reset_data RESET_DATA;
typedef struct room_index_data ROOM_INDEX_DATA;
typedef struct shop_data SHOP_DATA;
typedef struct time_info_data TIME_INFO_DATA;
typedef struct weather_data WEATHER_DATA;

typedef struct disabled_data DISABLED_DATA;
typedef struct assist_data ASSIST_DATA;

/* one disabled command */
struct disabled_data
{
	DISABLED_DATA *next;	/* pointer to next node */
	struct cmd_type const *command;	/* pointer to the command struct */
	char *disabled_by;	/* name of disabler */
	long level;		/* level of disabler */
};

extern bool reset_mud;
extern long top_assist;
extern DISABLED_DATA *disabled_first;
extern BAN_DATA *ban_first;

/*
 * Function types.
 */
typedef void DO_FUN args((CHAR_DATA * ch, char *argument));
typedef long SPEC_FUN args((SPEC_OBJ_DATA * spec, long update));
typedef void SPELL_FUN args((long sn, long level, CHAR_DATA * ch, void *vo));

/*
 * String and memory management parameters.
 */
#define	MAX_KEY_HASH		 1024
#define MAX_STRING_LENGTH	 4096
#define MAX_INPUT_LENGTH	  320
#define MSL			(MAX_STRING_LENGTH)
#define MIL			(MAX_INPUT_LENGTH)


#define SCREEN_WIDTH		79



/*
 * Colour stuff by Lope of Loping Through The MUD
*/
#define CLEAR		"[0m"	// RESETS COLORS
#define C_RED		"[0;31m"	// NORMAL
#define C_GREEN		"[0;32m"
#define C_YELLOW	"[0;33m"
#define C_BLUE		"[0;34m"
#define C_MAGENTA	"[0;35m"
#define C_CYAN		"[0;36m"
#define C_WHITE		"[0;37m"
#define C_D_GREY	"[1;30m"	// light colors
#define C_B_RED		"[1;31m"
#define C_B_GREEN	"[1;32m"
#define C_B_YELLOW	"[1;33m"
#define C_B_BLUE	"[1;34m"
#define C_B_MAGENTA	"[1;35m"
#define C_B_CYAN	"[1;36m"
#define C_B_WHITE	"[1;37m"

/* token types winner - pip
*/
#define T_QUEST		0	// set by how many players on
#define T_PRIMAL	1	// make this 20 max
#define T_EXP		2	// make this 200k max
#define T_WPN_SKILL	3	// make this 5 max
#define T_SPELL		4	// make this 7 max
#define T_STANCE	5	// make this 5 max
#define T_HP		6	// make this 50 max
#define T_MANA		7	// make this 50 max
#define T_MOVE		8	// make this 25 max(cuz of highlanders)
#define T_RANDOM	9	// could be any of above, or could be wierd affect.


/*
 * Rotains Gobal Procedures
 */
void who_html_update args((void));
void clear_stats args((CHAR_DATA * ch));
void room_is_total_darkness args((ROOM_INDEX_DATA * pRoomIndex));
void improve_wpn args((CHAR_DATA * ch, long dtype, bool right_hand));
void improve_stance args((CHAR_DATA * ch));
void skillstance args((CHAR_DATA * ch, CHAR_DATA * victim));
void show_spell args((CHAR_DATA * ch, long dtype));
void fightaction args((CHAR_DATA * ch, CHAR_DATA * victim, long actype, long dtype, long wpntype));
void crack_head args((CHAR_DATA * ch, OBJ_DATA * obj, char *argument));
void critical_hit args((CHAR_DATA * ch, CHAR_DATA * victim, long dt, long dam));

void take_item args((CHAR_DATA * ch, OBJ_DATA * obj));
void raw_kill args((CHAR_DATA * victim));
void trip args((CHAR_DATA * ch, CHAR_DATA * victim));
void disarm args((CHAR_DATA * ch, CHAR_DATA * victim));
void make_corpse args((CHAR_DATA * ch));
void one_hit args((CHAR_DATA * ch, CHAR_DATA * victim, long dt, long handtype));
void make_part args((CHAR_DATA * ch, char *argument));

/*void clan_table_read        args( ( ) );
void artifact_table_read    args( ( ) );
void clan_table_write       args( ( ) );
void artifact_table_write   args( ( ) );
void home_write             args( ( ) );
void clan_table_powerselect args( (CHAR_DATA *ch, char *power_name) );
void clan_table_namefill    args( (CHAR_DATA *ch) );
void clan_table_bite        args( (CHAR_DATA *ch) );
void clan_table_dec         args( (CHAR_DATA *ch, CHAR_DATA *victim) );
*/
void behead args((CHAR_DATA * victim));
void paradox args((CHAR_DATA * ch));

void load_bans args((void));
void save_bans args((void));

/*
 * Godwars Game Parameters
 * By Rotain
 */

#define SKILL_ADEPT               100
#define SKILL_THAC0_32             18
#define SKILL_THAC0_00              6
#define VERSION_NUMBER              2
#define DONATION_ROOM_WEAPON     6641
#define DONATION_ROOM_ARMOR      6643
#define DONATION_ROOM_REST       6644
#define MAX_VAMPIRE_POWER           3
#define MAX_CLAN                   11
#define MAX_ART  		   12
/*
 * Game parameters.
 * Increase the max'es if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */

#define MAX_VNUM		30000
#define MAX_VISION		   14
#define PARADOX_TICK               10
#define MAX_SKILL	 	  152
#define MAX_SPELL		   60
#define MAX_LEVEL		   12
#define NO_WATCH		   10
#define NO_GODLESS		   (MAX_LEVEL - 2)
#define LEVEL_HERO		   (MAX_LEVEL - 9)
#define LEVEL_IMMORTAL		   (MAX_LEVEL - 5)

#define LEVEL_MORTAL		   (MAX_LEVEL - 10)
#define LEVEL_AVATAR		   (MAX_LEVEL - 9)
#define LEVEL_APPRENTICE	   (MAX_LEVEL - 8)
#define LEVEL_NINJA                (MAX_LEVEL - 6)
#define LEVEL_MONK                 (MAX_LEVEL - 6)
#define LEVEL_BUILDER		   (MAX_LEVEL - 5)
#define LEVEL_QUESTMAKER	   (MAX_LEVEL - 4)
#define LEVEL_ENFORCER		   (MAX_LEVEL - 3)
#define LEVEL_JUDGE		   (MAX_LEVEL - 2)
#define LEVEL_HIGHJUDGE		   (MAX_LEVEL - 1)
#define LEVEL_IMPLEMENTOR	   (MAX_LEVEL)

#define PULSE_PER_SECOND	    3
#define PULSE_VIOLENCE		  ( 3 * PULSE_PER_SECOND)
#define PULSE_MOBILE		  ( 4 * PULSE_PER_SECOND)
#define PULSE_TICK		  (10 * PULSE_PER_SECOND)
#define PULSE_AREA		  (60 * PULSE_PER_SECOND)
#define PULSE_WW                  ( 4 * PULSE_PER_SECOND)
#define PULSE_KINGDOM		  (350 * PULSE_PER_SECOND)

#define A			 1
#define B			 2
#define C 			 4
#define D	 		 8
#define E		        16
#define F		        32
#define G		        64
#define H		       128
#define I		       256
#define J		       512
#define K		      1024
#define L		      2048
#define M		      4096
#define N      		      8192
#define O      		     16384
#define P     	 	     32768
#define Q      		     65536
#define R      		    131072
#define S      		    262144
#define T      		    524288
#define U      		   1048576
#define V      		   2097152
#define W      		   4194304
#define X    	  	   8388608
#define Y    	  	  16777216
#define Z     	 	  33554432
#define aa     		  67108864
#define bb    	 	 134217728
#define cc     		 268435456
#define dd     		 536870912
#define ee		1073741824

/*
 * Rotains Clan Table Structure
 */

struct clan_table_type
{
	char *clan_name;
	char *clan_leader;
	char *clan_powers_1;
	char *clan_powers_2;
	char *clan_powers_3;
	long clan_kills[MAX_CLAN];
	long lvnum;		/* Lower vnum of clan area */
	long uvnum;		/* Upper vnum of clan area */
	long clan_recall;	/* Clan recall vnum */
};

struct artifact_type
{
	char *player_name;
	long object_vnum;
};


struct assist_data
{
	ASSIST_DATA *next;
	CHAR_DATA *ch;
	long damage;
};

/*
 * Site ban structure.
 */
struct ban_data
{
	BAN_DATA *next;
	char *name;
};





/*
 * Time and weather stuff.
 */
#define SUN_DARK		    0
#define SUN_RISE		    1
#define SUN_LIGHT		    2
#define SUN_SET			    3

#define SKY_CLOUDLESS		    0
#define SKY_CLOUDY		    1
#define SKY_RAINING		    2
#define SKY_LIGHTNING		    3

struct time_info_data
{
	long hour;
	long day;
	long month;
	long year;
};

struct weather_data
{
	long mmhg;
	long change;
	long sky;
	long sunlight;
};



/*
 * Connected state for a channel.
 */
#define CON_HOTREBOOT_RECOVER		-1
#define CON_PLAYING			0
#define CON_INTRO			1
#define CON_GET_NAME			2
#define CON_GET_OLD_PASSWORD		3
#define CON_CONFIRM_NEW_NAME		4
#define CON_GET_NEW_PASSWORD		5
#define CON_CONFIRM_NEW_PASSWORD	6
#define CON_GET_NEW_SEX			7
#define CON_GET_NEW_CLASS		8
#define CON_READ_MOTD			9
#define CON_NOT_PLAYING			10
#define CON_GET_DEITY			11

/*
 * Timers
 */

// add timers here.. #define TIMER_WHATEVER  0...

/*
 * Descriptor (channel) structure.
 */
struct descriptor_data
{
	DESCRIPTOR_DATA *next;
	DESCRIPTOR_DATA *snoop_by;
	CHAR_DATA *character;
	CHAR_DATA *original;
	char *host;
	long descriptor;
	long connected;
	bool fcommand;
	char inbuf[4 * MAX_INPUT_LENGTH];
	char incomm[MAX_INPUT_LENGTH];
	char inlast[MAX_INPUT_LENGTH];
	long repeat;
	char *outbuf;
	long outsize;
	long outtop;
	long wait;
	long login_time;
	long ping;
};



/*
 * Attribute bonus structures.
 */
struct str_app_type
{
	long tohit;
	long todam;
	long carry;
	long wield;
};

struct int_app_type
{
	long learn;
};

struct wis_app_type
{
	long practice;
};

struct dex_app_type
{
	long defensive;
};

struct con_app_type
{
	long hitp;
	long shock;
};



/*
 * TO types for act.
 */
#define TO_ROOM		    0
#define TO_NOTVICT	    1
#define TO_VICT		    2
#define TO_CHAR		    3
#define TO_ALL		    4


/*
 * Help table types.
 */
struct help_data
{
	HELP_DATA *next;
	long level;
	char *keyword;
	char *text;
};



/*
 * Shop types.
 */
#define MAX_TRADE	 5

struct shop_data
{
	SHOP_DATA *next;	/* Next shop in list            */
	long keeper;		/* Vnum of shop keeper mob      */
	long buy_type[MAX_TRADE];	/* Item types shop will buy     */
	long profit_buy;		/* Cost multiplier for buying   */
	long profit_sell;	/* Cost multiplier for selling  */
	long open_hour;		/* First opening hour           */
	long close_hour;		/* First closing hour           */
};



/*
 * Data structure for notes.
 */
struct note_data
{
	NOTE_DATA *next;
	char *sender;
	char *date;
	char *to_list;
	char *subject;
	char *text;
};



/*
 * An affect.
 */
struct affect_data
{
	AFFECT_DATA *next;
	long type;
	long duration;
	long location;
	long modifier;
	long bitvector;
};



/*
 * A kill structure (indexed by level).
 */
struct kill_data
{
	long number;
	long killed;
};



/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (Start of section ... start here)                     *
 *                                                                         *
 ***************************************************************************/

/*
 * Well known mob virtual numbers.
 * Defined in #MOBILES.
 */
#define MOB_VNUM_CITYGUARD	   3060
#define MOB_VNUM_VAMPIRE	   26002


/*
 * Immunities, for players.  KaVir.
 */
#define IMM_SLASH	      1	/* Resistance to slash, slice.          */
#define IMM_STAB	      2	/* Resistance to stab, pierce.          */
#define IMM_SMASH	      4	/* Resistance to blast, pound, crush.   */
#define IMM_ANIMAL	      8	/* Resistance to bite, claw.            */
#define IMM_MISC	     16	/* Resistance to grep, suck, whip.      */
#define IMM_CHARM	     32	/* Immune to charm spell.               */
#define IMM_HEAT	     64	/* Immune to fire/heat spells.          */
#define IMM_COLD	    128	/* Immune to frost/cold spells.         */
#define IMM_LIGHTNING	    256	/* Immune to lightning spells.          */
#define IMM_ACID	    512	/* Immune to acid spells.               */
#define IMM_SUMMON	   1024	/* Immune to being summoned.            */
#define IMM_VOODOO	   2048	/* Immune to voodoo magic.              */
#define IMM_VAMPIRE	   4096	/* Allow yourself to become a vampire.  */
#define IMM_STAKE	   8192	/* Immune to being staked (vamps only). */
#define IMM_SUNLIGHT	  16384	/* Immune to sunlight (vamps only).     */
#define IMM_SHIELDED	  32768	/* For Obfuscate. Block scry, etc.      */
#define IMM_HURL	  65536	/* Cannot be hurled.                    */
#define IMM_BACKSTAB	 131072	/* Cannot be backstabbed.               */
#define IMM_KICK	 262144	/* Cannot be kicked.                    */
#define IMM_DISARM	 524288	/* Cannot be disarmed.                  */
#define IMM_STEAL	1048576	/* Cannot have stuff stolen.            */
#define IMM_SLEEP	2097152	/* Immune to sleep spell.               */
#define IMM_DRAIN	4194304	/* Immune to energy drain.              */
#define IMM_DEMON	8388608	/* Allow yourself to become a demon.    */
#define IMM_TRANSPORT  16777216	/* Objects can't be transported to you. */
#define IMM_TRAVEL     33554432	/*so demons can turn travel off */

/*
 * ACT bits for mobs.
 * Used in #MOBILES.
 */
#define ACT_IS_NPC		      1	/* Auto set for mobs    */
#define ACT_SENTINEL		      2	/* Stays in one room    */
#define ACT_SCAVENGER		      4	/* Picks up objects     */
#define ACT_AGGRESSIVE		     32	/* Attacks PC's         */
#define ACT_STAY_AREA		     64	/* Won't leave area     */
#define ACT_WIMPY		    128	/* Flees when hurt      */
#define ACT_PET			    256	/* Auto set for pets    */
#define ACT_TRAIN		    512	/* Can train PC's       */
#define ACT_PRACTICE		   1024	/* Can practice PC's    */
#define ACT_MOUNT		   2048	/* Can be mounted       */
#define ACT_NOPARTS		   4096	/* Dead = no body parts */
#define ACT_NOEXP		   8192	/* No exp for killing   */
#define ACT_NOAUTOKILL		  16384


#define STECH_BERSERK		0x00000001
#define STECH_DISARM		0x00000002
#define STECH_FURY		0x00000004
#define STECH_TRUESIGHT 	0x00000008
#define STECH_CONCENTRATE	0x00000010


/*
 * Bits for 'affected_by'.
 * Used in #MOBILES.
 */
#define AFF_BLIND		      1
#define AFF_INVISIBLE		      2
#define AFF_DETECT_EVIL		      4
#define AFF_DETECT_INVIS	      8
#define AFF_DETECT_MAGIC	     16
#define AFF_DETECT_HIDDEN	     32
#define AFF_SHADOWPLANE		     64	/* Creatures in shadow plane - KaVir */
#define AFF_SANCTUARY		    128
#define AFF_FAERIE_FIRE		    256
#define AFF_INFRARED		    512
#define AFF_CURSE		   1024
#define AFF_FLAMING		   2048	/* For burning creatures - KaVir */
#define AFF_POISON		   4096
#define AFF_PROTECT		   8192
#define AFF_ETHEREAL		  16384	/* For ethereal creatures - KaVir */
#define AFF_SNEAK		  32768
#define AFF_HIDE		  65536
#define AFF_SLEEP		 131072
#define AFF_CHARM		 262144
#define AFF_FLYING		 524288
#define AFF_PASS_DOOR		1048576
#define AFF_POLYMORPH		2097152	/* For polymorphed creatures - KaVir */
#define AFF_SHADOWSIGHT		4194304	/* Can see between planes - KaVir */
#define AFF_WEBBED		8388608	/* Cannot move - KaVir */
#define AFF_TOTALBLIND      16777216	/* Cannot get pregnant - KaVir */
#define AFF_DARKNESS           33554432	/* Drow Darkness - Rotain */
#define AFF_DROWFIRE	       67108864
#define AFF_SAFE              134217728
#define AFF_SHIFT	      268435456
#define AFF_GODBLESS          536870912	/* Monk and Paladin Bless */
#define AFF_UNUSED       1073741824	/* Total Blind, nothing can penerate */

/*
 * The Affs Strike Back
 * 'flag2'
 */
#define AFF2_BLINK_1ST_RD		1
#define AFF2_BLINK_2ND_RD		2
#define AFF2_SPIRITGUARD                4
#define VAMP_ASHES			8
#define VAMP_CLONE			16
#define VAMP_OBJMASK			32
#define AFF2_ROT			64
#define AFF2_CONTRACEPTION	       128
#define AFF2_TENDRILS		       256
#define EXTRA_AFK		       512
#define AFF2_AFK		      1024
#define AFF2_REIMB		      2048
#define AFF2_CHALLENGED		      4096
#define AFF2_CHALLENGER		      8192
/*#define AFF2_INARENA	 	     16384
*/
#define AFF2_LLOTHBLESS		     32768
#define AFF2_NOPORTAL		     65536
#define AFF2_QUESTHOLDER	    131072

/*
 * Bits for 'itemaffect'.
 * Used in #MOBILES.
 */
#define ITEMA_SHOCKSHIELD	      1
#define ITEMA_FIRESHIELD	      2
#define ITEMA_ICESHIELD		      4
#define ITEMA_ACIDSHIELD	      8
#define ITEMA_DBPASS                 16
#define ITEMA_CHAOSSHIELD            32
#define ITEMA_ARTIFACT               64
#define ITEMA_REGENERATE            128
#define ITEMA_SPEED                 256
#define ITEMA_VORPAL                512
#define ITEMA_PEACE                1024
#define ITEMA_RIGHT_SILVER         2048
#define ITEMA_LEFT_SILVER          4096
#define ITEMA_REFLECT              8192
#define ITEMA_RESISTANCE          16384
#define ITEMA_VISION              32768
#define ITEMA_STALKER             65536
#define ITEMA_VANISH             131072
#define ITEMA_RAGER              262144
#define ITEMA_HIGHLANDER         524288
#define ITEMA_PALADIN           1048576


/*
 * Rune, Glyph and Sigil bits.
 */
#define RUNE_FIRE	      1
#define RUNE_AIR	      2
#define RUNE_EARTH	      4
#define RUNE_WATER	      8
#define RUNE_DARK	      16
#define RUNE_LIGHT	      32
#define RUNE_LIFE	      64
#define RUNE_DEATH	      128
#define RUNE_MIND	      256
#define RUNE_SPIRIT	      512
#define RUNE_MASTER	      1024
#define GLYPH_CREATION	      1
#define GLYPH_DESTRUCTION     2
#define GLYPH_SUMMONING	      4
#define GLYPH_TRANSFORMATION  8
#define GLYPH_TRANSPORTATION  16
#define GLYPH_ENHANCEMENT     32
#define GLYPH_REDUCTION	      64
#define GLYPH_CONTROL	      128
#define GLYPH_PROTECTION      256
#define GLYPH_INFORMATION     512
#define SIGIL_SELF	      1
#define SIGIL_TARGETING       2
#define SIGIL_AREA	      4
#define SIGIL_OBJECT	      8


/*
 * Advanced spells.
 */
#define ADV_DAMAGE	      1
#define ADV_AFFECT	      2
#define ADV_ACTION	      4
#define ADV_AREA_AFFECT       8
#define ADV_VICTIM_TARGET    16
#define ADV_OBJECT_TARGET    32
#define ADV_GLOBAL_TARGET    64
#define ADV_NEXT_PAGE       128
#define ADV_PARAMETER       256
#define ADV_SPELL_FIRST     512
#define ADV_NOT_CASTER     1024
#define ADV_NO_PLAYERS     2048
#define ADV_SECOND_VICTIM  4096
#define ADV_SECOND_OBJECT  8192
#define ADV_REVERSED      16384
#define ADV_STARTED	  32768
#define ADV_FINISHED	  65536
#define ADV_FAILED	 131072
#define ADV_MESSAGE_1	 262144
#define ADV_MESSAGE_2	 524288
#define ADV_MESSAGE_3	1048576

/*
 * Advanced spell actions.
 */
#define ACTION_NONE	      0
#define ACTION_MOVE	      1
#define ACTION_MOB	      2
#define ACTION_OBJECT	      3

/*
 * Advanced spell affects.
 */
#define ADV_STR			      1
#define ADV_DEX			      2
#define ADV_INT			      4
#define ADV_WIS			      8
#define ADV_CON			     16
#define ADV_SEX			     32
#define ADV_MANA		     64
#define ADV_HIT			    128
#define ADV_MOVE		    256
#define ADV_AC			    512
#define ADV_HITROLL		   1024
#define ADV_DAMROLL		   2048
#define ADV_SAVING_SPELL	   4096



/* Colour scale macros - added 12th Aug 1995 by Calamar */

#define NO_COLOUR	"{x"	/* Blank */
#define GREY		"{x"	/* Dark Grey */
#define D_RED		"{r"	/* Dark Red */
#define L_RED		"{R"	/* Light Red */
#define D_GREEN		"{g"	/* Dark Green */
#define L_GREEN		"{G"	/* Light Green */
#define BROWN		"{y"	/* Brown */
#define YELLOW		"{Y"	/* Yellow */
#define D_BLUE		"{b"	/* Dark Blue */
#define L_BLUE		"{B"	/* Light Blue */
#define MAGENTA		"{m"	/* Magenta */
#define PINK		"{M"	/* Pink */
#define D_CYAN		"{c"	/* Dark Cyan */
#define L_CYAN		"{C"	/* Light Cyan */
#define NORMAL		"{x"	/* Light Grey */
#define WHITE		"{W"	/* White */

#define ADD_COLOUR(_player,_str,_col) {char swh_temp[255]; \
if (!IS_NPC(_player) && IS_SET(_player->act, PLR_ANSI)) {swh_temp[0] = '\0'; \
strcpy(swh_temp, _col); strcat(swh_temp, _str); strcat(swh_temp, NORMAL); \
strcpy(_str, swh_temp);}}

#define SCALE_COLS 4

#define COL_SCALE(_swh_str,_swh_ch,_swh_curr,_swh_max) \
	ADD_COLOUR(_swh_ch, _swh_str, \
	(_swh_curr < 1) ? D_CYAN : \
	(_swh_curr < _swh_max) ? \
	scale[(SCALE_COLS * _swh_curr) / ((_swh_max > 0) \
	? _swh_max : 1)] : D_CYAN)

extern char *scale[SCALE_COLS];

/*
 * Bits for 'vampire'.
 * Used for player vampires.
 */
#define VAM_FANGS		      1
#define VAM_CLAWS		      2
#define VAM_NIGHTSIGHT		      4
#define VAM_FLYING		      8	/* For flying creatures */
#define VAM_SONIC		     16	/* For creatures with full detect */
#define VAM_CHANGED		     32	/* Changed using a vampire power */

#define VAM_PROTEAN		     64	/* Claws, nightsight, and change */
#define VAM_CELERITY		    128	/* 66%/33% chance 1/2 extra attacks */
#define VAM_FORTITUDE		    256	/* 5 hp less per hit taken */
#define VAM_POTENCE		    512	/* Deal out 1.5 times normal damage */
#define VAM_OBFUSCATE		   1024	/* Disguise and invis */
#define VAM_AUSPEX		   2048	/* Truesight, etc */
#define VAM_OBTENEBRATION	   4096	/* Shadowplane/sight and shadowbody */
#define VAM_SERPENTIS		   8192	/* Eyes/serpent, heart/darkness, etc */

#define VAM_DISGUISED		  16384	/* For the Obfuscate disguise ability */
#define VAM_MORTAL		  32768	/* For Obfuscate mortal ability. */

#define VAM_DOMINATE		  65536	/* Evileye, command */

#define VAM_EVILEYE		 131072	/* Evileye, command */

#define VAM_PRESENCE		 262144	/* Presence discipline */

#define VAM_VICISSITUDE          524288

/* New vamp defines*/
#define VAM_ANIM	0
#define VAM_AUSP	1
#define VAM_DAIM	2
#define VAM_DOMI	3
#define VAM_FORT	4
#define VAM_OBFU	5
#define VAM_OBTE	6
#define VAM_PRES	7
#define VAM_PROT	8
#define VAM_QUIE	9
#define VAM_SERP	10
#define VAM_THAU	11
#define VAM_VICI	12
#define VAM_CELE	13
#define VAM_CHIM	14
#define VAM_POTE	15
#define VAM_OBEA	16
#define VAM_MELP	17
#define VAM_THAN 	18
#define VAM_NECR	19

/*end new vamp section*/

/*Define new vamp powers*/

/*
 * Bits for 'polymorph'.
 * Used for players.
 */
#define POLY_BAT		      1
#define POLY_WOLF		      2
#define POLY_MIST		      4
#define POLY_SERPENT		      8
#define POLY_RAVEN		     16
#define POLY_FISH		     32
#define POLY_FROG		     64
#define POLY_ZULO                   128
#define POLY_SPIDERFORM		    256

/*
 * Languages.
 */
#define LANG_COMMON		      0
#define DIA_OLDE		      1
#define DIA_BAD			      2
#define LANG_HACK		      3
#define LANG_DARK		      4


/*
 * Score.
 */
#define SCORE_TOTAL_XP		      0
#define SCORE_HIGH_XP		      1
#define SCORE_TOTAL_LEVEL	      2
#define SCORE_HIGH_LEVEL	      3
#define SCORE_QUEST		      4
#define SCORE_NUM_QUEST		      5



/*
 * Zombie Lord.
 */
#define ZOMBIE_NOTHING		      0
#define ZOMBIE_TRACKING		      1
#define ZOMBIE_ANIMATE		      2
#define ZOMBIE_CAST		      3
#define ZOMBIE_REST		      4



/*
 * Damcap values.
 */
#define DAM_CAP		      0
#define DAM_CHANGE	      1

/*
 * Bits for Demonic Champions.
 *
 * new demon fields in player.h and old ones in old.h
 */


/*
 * Mounts
 */
#define IS_ON_FOOT		      0
#define IS_MOUNT		      1
#define IS_RIDING		      2
#define IS_CARRIED		      4
#define IS_CARRYING		      8




/*
 * Sex.
 * Used in #MOBILES.
 */
#define SEX_NEUTRAL		      0
#define SEX_MALE		      1
#define SEX_FEMALE		      2



/*
 * Well known object virtual numbers.
 * Defined in #OBJECTS.
 */
#define OBJ_VNUM_MONEY_ONE	      2
#define OBJ_VNUM_MONEY_SOME	      3

#define OBJ_VNUM_CORPSE_NPC	     10
#define OBJ_VNUM_CORPSE_PC	     11
#define OBJ_VNUM_SEVERED_HEAD	     12
#define OBJ_VNUM_TORN_HEART	     13
#define OBJ_VNUM_SLICED_ARM	     14
#define OBJ_VNUM_SLICED_LEG	     15
#define OBJ_VNUM_FINAL_TURD	     16
#define OBJ_VNUM_STAKE       18

#define OBJ_VNUM_MUSHROOM	     20
#define OBJ_VNUM_LIGHT_BALL	     21
#define OBJ_VNUM_SPRING		     22
#define OBJ_VNUM_BLOOD_SPRING	     23

#define OBJ_VNUM_GRAFTED_ARM	     27

#define OBJ_VNUM_SCHOOL_MACE	   3700
#define OBJ_VNUM_SCHOOL_DAGGER	   3701
#define OBJ_VNUM_SCHOOL_SWORD	   3702
#define OBJ_VNUM_SCHOOL_VEST	   3703
#define OBJ_VNUM_SCHOOL_SHIELD	   3704
#define OBJ_VNUM_SCHOOL_BANNER     3716

/* For KaVir's stuff */
#define OBJ_VNUM_SOULBLADE	  30000
#define OBJ_VNUM_PORTAL		  30001
#define OBJ_VNUM_EGG		  30002
#define OBJ_VNUM_EMPTY_EGG	  30003
#define OBJ_VNUM_SPILLED_ENTRAILS 30004
#define OBJ_VNUM_QUIVERING_BRAIN  30005
#define OBJ_VNUM_SQUIDGY_EYEBALL  30006
#define OBJ_VNUM_SPILT_BLOOD      30007
#define OBJ_VNUM_VOODOO_DOLL      30010
#define OBJ_VNUM_RIPPED_FACE      30012
#define OBJ_VNUM_TORN_WINDPIPE    30013
#define OBJ_VNUM_CRACKED_HEAD     30014
#define OBJ_VNUM_SLICED_EAR	  30025
#define OBJ_VNUM_SLICED_NOSE	  30026
#define OBJ_VNUM_KNOCKED_TOOTH	  30027
#define OBJ_VNUM_TORN_TONGUE	  30028
#define OBJ_VNUM_SEVERED_HAND	  30029
#define OBJ_VNUM_SEVERED_FOOT	  30030
#define OBJ_VNUM_SEVERED_THUMB	  30031
#define OBJ_VNUM_SEVERED_INDEX	  30032
#define OBJ_VNUM_SEVERED_MIDDLE	  30033
#define OBJ_VNUM_SEVERED_RING	  30034
#define OBJ_VNUM_SEVERED_LITTLE	  30035
#define OBJ_VNUM_SEVERED_TOE	  30036
#define OBJ_VNUM_PROTOPLASM	  30037
#define OBJ_VNUM_QUESTCARD	  30039
#define OBJ_VNUM_QUESTMACHINE	  30040
#define OBJ_VNUM_GATE		  30042
#define MOB_VNUM_GUARDIAN	  30001
#define MOB_VNUM_MOUNT		  30006
#define MOB_VNUM_FROG		  30007
#define MOB_VNUM_RAVEN		  30008
#define MOB_VNUM_CAT		  30009
#define MOB_VNUM_DOG		  30010
#define MOB_VNUM_EYE		  30012
#define OBJ_VNUM_COPPER		  30043
#define OBJ_VNUM_IRON		  30044
#define OBJ_VNUM_STEEL		  30045
#define OBJ_VNUM_ADAMANTITE	  30046

/*
 * Item types.
 * Used in #OBJECTS.
 */
#define ITEM_LIGHT		      1
#define ITEM_SCROLL		      2
#define ITEM_WAND		      3
#define ITEM_STAFF		      4
#define ITEM_WEAPON		      5
#define ITEM_TREASURE		      8
#define ITEM_ARMOR		      9
#define ITEM_POTION		     10
#define ITEM_FURNITURE		     12
#define ITEM_TRASH		     13
#define ITEM_CONTAINER		     15
#define ITEM_DRINK_CON		     17
#define ITEM_KEY		     18
#define ITEM_FOOD		     19
#define ITEM_MONEY		     20
#define ITEM_BOAT		     22
#define ITEM_CORPSE_NPC		     23
#define ITEM_CORPSE_PC		     24
#define ITEM_FOUNTAIN		     25
#define ITEM_PILL		     26
#define ITEM_PORTAL		     27
#define ITEM_EGG		     28
#define ITEM_VOODOO		     29
#define ITEM_STAKE		     30
#define ITEM_MISSILE		     31	/* Ammo vnum, cur, max, type */
#define ITEM_AMMO		     32	/* ???, dam min, dam max, type */
#define ITEM_QUEST		     33
#define ITEM_QUESTCARD		     34
#define ITEM_QUESTMACHINE	     35
#define ITEM_SYMBOL		     36
#define ITEM_BOOK		     37
#define ITEM_PAGE		     38
#define ITEM_TOOL		     39
#define ITEM_DTOKEN		     40
#define ITEM_WALL		     41
#define ITEM_WGATE		     42
#define ITEM_COPPER		     43
#define ITEM_IRON		     44
#define ITEM_STEEL		     45
#define ITEM_ADAMANTITE		     46
#define ITEM_INSTRUMENT		     47
#define ITEM_EXTRA_ARM		     48

/*
 * Weapon flags
 * field = 'weapflags'
 * For Flaming\Icing weapons
 */
#define WEAPON_FLAMING		      1
#define WEAPON_POISON		      2
#define WEAPON_FROST		      4


/*
 * Extra flags.
 * Used in #OBJECTS.
 */
#define ITEM_GLOW		      1
#define ITEM_HUM		      2
#define ITEM_THROWN		      4
#define ITEM_KEEP		      8
#define ITEM_VANISH		     16
#define ITEM_INVIS		     32
#define ITEM_MAGIC		     64
#define ITEM_NODROP		    128
#define ITEM_BLESS		    256
#define ITEM_ANTI_GOOD		    512
#define ITEM_ANTI_EVIL		   1024
#define ITEM_ANTI_NEUTRAL	   2048
#define ITEM_NOREMOVE		   4096
#define ITEM_INVENTORY		   8192
#define ITEM_LOYAL		  16384
#define ITEM_SHADOWPLANE	  32768
#define ITEM_ARM		  65536
#define ITEM_MENCHANT		 131072
#define ITEM_CAUST		 262144

/*
 * Wear flags.
 * Used in #OBJECTS.
 */
#define ITEM_TAKE		      1
#define ITEM_WEAR_FINGER	      2
#define ITEM_WEAR_NECK		      4
#define ITEM_WEAR_BODY		      8
#define ITEM_WEAR_HEAD		     16
#define ITEM_WEAR_LEGS		     32
#define ITEM_WEAR_FEET		     64
#define ITEM_WEAR_HANDS		    128
#define ITEM_WEAR_ARMS		    256
#define ITEM_WEAR_SHIELD	    512
#define ITEM_WEAR_ABOUT		   1024
#define ITEM_WEAR_WAIST		   2048
#define ITEM_WEAR_WRIST		   4096
#define ITEM_WIELD		   8192
#define ITEM_HOLD		  16384
#define ITEM_WEAR_FACE		  32768



/*
 * Special types.
 * Used in #OBJECTS for special items - KaVir.
 */
#define SITEM_ACTIVATE		      1
#define SITEM_TWIST		      2
#define SITEM_PRESS		      4
#define SITEM_PULL		      8
#define SITEM_TARGET		      16
#define SITEM_SPELL		      32
#define SITEM_TRANSPORTER	      64
#define SITEM_TELEPORTER	      128
#define SITEM_DELAY1		      256
#define SITEM_DELAY2		      512
#define SITEM_OBJECT		     1024
#define SITEM_MOBILE		     2048
#define SITEM_ACTION		     4096
#define SITEM_MORPH		     8192
#define SITEM_SILVER		    16384
#define SITEM_WOLFWEAPON	    32768
#define SITEM_DROWWEAPON	    65536
#define SITEM_CHAMPWEAPON	   131072
#define SITEM_DEMONIC		   262144
#define SITEM_HIGHLANDER	   524288
#define SITEM_NINJA               1048576
#define SITEM_MAGE		  2097152
#define SITEM_DROW		  4194304
#define SITEM_GEMSTONE		  8388608
#define SITEM_HILT		 16777216
#define SITEM_MONK		 33554432
#define SITEM_WW		 67108864

/*
 * Apply types (for quest affects).
 * Used in #OBJECTS.
 */
#define QUEST_STR		      1
#define QUEST_DEX		      2
#define QUEST_INT		      4
#define QUEST_WIS		      8
#define QUEST_CON		     16
#define QUEST_HITROLL		     32
#define QUEST_DAMROLL		     64
#define QUEST_HIT		    128
#define QUEST_MANA		    256
#define QUEST_MOVE		    512
#define QUEST_AC		   1024

#define QUEST_NAME		   2048
#define QUEST_SHORT		   4096
#define QUEST_LONG		   8192
#define QUEST_FREENAME		  16384

#define QUEST_ENCHANTED		  32768
#define QUEST_SPELLPROOF	  65536
#define QUEST_ARTIFACT		 131072
#define QUEST_IMPROVED		 262144
#define QUEST_MASTER_RUNE	 524288
#define QUEST_RELIC             1048576
#define QUEST_ZOMBIE		2097152
#define QUEST_CLONED     	4194304
#define QUEST_BLOODA		8388608
#define ITEM_EQUEST	       16777216
#define QUEST_ANCIENT	       33554432

/*
 * Tool types.
 */
#define TOOL_PEN		      1
#define TOOL_PLIERS		      2
#define TOOL_SCALPEL		      4



/*
 * Apply types (for affects).
 * Used in #OBJECTS.
 */
#define APPLY_NONE		      0
#define APPLY_STR		      1
#define APPLY_DEX		      2
#define APPLY_INT		      3
#define APPLY_WIS		      4
#define APPLY_CON		      5
#define APPLY_SEX		      6
#define APPLY_CLASS		      7
#define APPLY_LEVEL		      8
#define APPLY_AGE		      9
#define APPLY_HEIGHT		     10
#define APPLY_WEIGHT		     11
#define APPLY_MANA		     12
#define APPLY_HIT		     13
#define APPLY_MOVE		     14
#define APPLY_GOLD		     15
#define APPLY_EXP		     16
#define APPLY_AC		     17
#define APPLY_HITROLL		     18
#define APPLY_DAMROLL		     19
#define APPLY_SAVING_PARA	     20
#define APPLY_SAVING_ROD	     21
#define APPLY_SAVING_PETRI	     22
#define APPLY_SAVING_BREATH	     23
#define APPLY_SAVING_SPELL	     24
#define APPLY_POLY		     25



/*
 * Values for containers (value[1]).
 * Used in #OBJECTS.
 */
#define CONT_CLOSEABLE		      1
#define CONT_PICKPROOF		      2
#define CONT_CLOSED		      4
#define CONT_LOCKED		      8



/*
 * Well known room virtual numbers.
 * Defined in #ROOMS.
 */
#define ROOM_VNUM_LIMBO		      2
#define ROOM_VNUM_CHAT		   1200
#define ROOM_VNUM_TEMPLE	   3001
#define ROOM_VNUM_ALTAR		   3054
#define ROOM_VNUM_SCHOOL	   3700
#define ROOM_VNUM_HELL		   8151
#define ROOM_VNUM_CRYPT		  30001
#define ROOM_VNUM_DISCONNECTION	  30002
#define ROOM_VNUM_IN_OBJECT	  30008



/*
 * Room flags.
 * Used in #ROOMS.
 */
#define ROOM_DARK		0x00000001
#define ROOM_SMALL		0x00000002
#define ROOM_NO_MOB		0x00000004
#define ROOM_INDOORS		0x00000008
#define ROOM_EMPTY		0x00000010


#define ROOM_BEND		0x00000080
#define ROOM_INTERSECTION	0x00000100
#define ROOM_PRIVATE		0x00000200
#define ROOM_SAFE		0x00000400
#define ROOM_SOLITARY		0x00000800
#define ROOM_PET_SHOP		0x00001000
#define ROOM_NO_RECALL		0x00002000
#define ROOM_NO_TELEPORT	0x00004000
#define ROOM_TOTAL_DARKNESS     0x00008000
#define ROOM_BLADE_BARRIER      0x00010000
#define ROOM_SILENCE		0x00020000
#define ROOM_FLAMING		0x00040000
#define ROOM_SWAMP		0x00080000
#define ROOM_ROCKY		0x00100000
#define ROOM_LARGE		0x00200000
#define ROOM_HUGE		0x00400000

/*
 * Room text flags (KaVir).
 * Used in #ROOMS.
 */
#define RT_LIGHTS		      1	/* Toggles lights on/off */
#define RT_SAY			      2	/* Use this if no others powers */
#define RT_ENTER		      4
#define RT_CAST			      8
#define RT_THROWOUT		     16	/* Erm can't remember ;) */
#define RT_OBJECT		     32	/* Creates an object */
#define RT_MOBILE		     64	/* Creates a mobile */
#define RT_LIGHT		    128	/* Lights on ONLY */
#define RT_DARK			    256	/* Lights off ONLY */
#define RT_OPEN_LIFT		    512	/* Open lift */
#define RT_CLOSE_LIFT		   1024	/* Close lift */
#define RT_MOVE_LIFT		   2048	/* Move lift */
#define RT_SPELL		   4096	/* Cast a spell */
#define RT_PORTAL		   8192	/* Creates a one-way portal */
#define RT_TELEPORT		  16384	/* Teleport player to room */

#define RT_ACTION		  32768
#define RT_BLANK_1		  65536
#define RT_BLANK_2		 131072

#define RT_RETURN		1048576	/* Perform once */
#define RT_PERSONAL		2097152	/* Only shows message to char */
#define RT_TIMER		4194304	/* Sets object timer to 1 tick */



/*
 * Directions.
 * Used in #ROOMS.
 */
#define DIR_NORTH		      0
#define DIR_EAST		      1
#define DIR_SOUTH		      2
#define DIR_WEST		      3
#define DIR_UP			      4
#define DIR_DOWN		      5



/*
 * Exit flags.
 * Used in #ROOMS.
 */
#define EX_ISDOOR		      1
#define EX_CLOSED		      2
#define EX_LOCKED		      4
#define EX_PICKPROOF		     32



/*
 * Sector types.
 * Used in #ROOMS.
 */
#define SECT_INSIDE		      0
#define SECT_CITY		      1
#define SECT_FIELD		      2
#define SECT_FOREST		      3
#define SECT_HILLS		      4
#define SECT_MOUNTAIN		      5
#define SECT_WATER_SWIM		      6
#define SECT_WATER_NOSWIM	      7
#define SECT_UNUSED		      8
#define SECT_AIR		      9
#define SECT_DESERT		     10
#define SECT_MAX		     11



/*
 * Equipment wear locations.
 * Used in #RESETS.
 */
#define WEAR_NONE		     -1
#define WEAR_LIGHT		      0
#define WEAR_FINGER_L		      1
#define WEAR_FINGER_R		      2
#define WEAR_NECK_1		      3
#define WEAR_NECK_2		      4
#define WEAR_BODY		      5
#define WEAR_HEAD		      6
#define WEAR_LEGS		      7
#define WEAR_FEET		      8
#define WEAR_HANDS		      9
#define WEAR_ARMS		     10
#define WEAR_SHIELD		     11
#define WEAR_ABOUT		     12
#define WEAR_WAIST		     13
#define WEAR_WRIST_L		     14
#define WEAR_WRIST_R		     15
#define WEAR_WIELD		     16
#define WEAR_HOLD		     17
#define WEAR_FACE		     18
#define WEAR_SCABBARD_L		     19
#define WEAR_SCABBARD_R		     20
#define WEAR_THIRD_ARM		     21
#define WEAR_FOURTH_ARM		     22
#define MAX_WEAR		     23



/*
 * Locations for damage.
 */
#define LOC_HEAD		      0
#define LOC_BODY		      1
#define LOC_ARM_L		      2
#define LOC_ARM_R		      3
#define LOC_LEG_L		      4
#define LOC_LEG_R		      5

/*
 * For Head
 */
#define LOST_EYE_L		       1
#define LOST_EYE_R		       2
#define LOST_EAR_L		       4
#define LOST_EAR_R		       8
#define LOST_NOSE		      16
#define BROKEN_NOSE		      32
#define BROKEN_JAW		      64
#define BROKEN_SKULL		     128
#define LOST_HEAD		     256
#define LOST_TOOTH_1		     512	/* These should be added..... */
#define LOST_TOOTH_2		    1024	/* ...together to caculate... */
#define LOST_TOOTH_4		    2048	/* ...the total number of.... */
#define LOST_TOOTH_8		    4096	/* ...teeth lost.  Total..... */
#define LOST_TOOTH_16		    8192	/* ...possible is 31 teeth.   */
#define LOST_TONGUE		   16384

/*
 * For Body
 */
#define BROKEN_RIBS_1		       1	/* Remember there are a total */
#define BROKEN_RIBS_2		       2	/* of 12 pairs of ribs in the */
#define BROKEN_RIBS_4		       4	/* human body, so not all of  */
#define BROKEN_RIBS_8		       8	/* these bits should be set   */
#define BROKEN_RIBS_16		      16	/* at the same time.          */
#define BROKEN_SPINE		      32
#define BROKEN_NECK		      64
#define CUT_THROAT		     128
#define CUT_STOMACH		     256
#define CUT_CHEST		     512

/*
 * For Arms
 */
#define BROKEN_ARM		       1
#define LOST_ARM		       2
#define LOST_HAND		       4
#define LOST_FINGER_I		       8	/* Index finger */
#define LOST_FINGER_M		      16	/* Middle finger */
#define LOST_FINGER_R		      32	/* Ring finger */
#define LOST_FINGER_L		      64	/* Little finger */
#define LOST_THUMB		     128
#define BROKEN_FINGER_I		     256	/* Index finger */
#define BROKEN_FINGER_M		     512	/* Middle finger */
#define BROKEN_FINGER_R		    1024	/* Ring finger */
#define BROKEN_FINGER_L		    2048	/* Little finger */
#define BROKEN_THUMB		    4096

/*
 * For Legs
 */
#define BROKEN_LEG		       1
#define LOST_LEG		       2
#define LOST_FOOT		       4
#define LOST_TOE_A		       8
#define LOST_TOE_B		      16
#define LOST_TOE_C		      32
#define LOST_TOE_D		      64	/* Smallest toe */
#define LOST_TOE_BIG		     128
#define BROKEN_TOE_A		     256
#define BROKEN_TOE_B		     512
#define BROKEN_TOE_C		    1024
#define BROKEN_TOE_D		    2048	/* Smallest toe */
#define BROKEN_TOE_BIG		    4096

/*
 * For Bleeding
 */
#define BLEEDING_HEAD		       1
#define BLEEDING_THROAT		       2
#define BLEEDING_ARM_L		       4
#define BLEEDING_ARM_R		       8
#define BLEEDING_HAND_L		      16
#define BLEEDING_HAND_R		      32
#define BLEEDING_LEG_L		      64
#define BLEEDING_LEG_R		     128
#define BLEEDING_FOOT_L		     256
#define BLEEDING_FOOT_R		     512


/*
 * For Spec powers on players
 */
#define EYE_SPELL		       1	/* Spell when they look at you */
#define EYE_SELFACTION		       2	/* You do action when they look */
#define EYE_ACTION		       4	/* Others do action when they look */


// special objects
struct spec_obj_data
{
	SPEC_OBJ_DATA *next_world;
	SPEC_OBJ_DATA *next;
	ROOM_INDEX_DATA *room;
	CHAR_DATA *ch;
	CHAR_DATA *vch;
	OBJ_DATA *obj;
	SPEC_FUN *spec_fun;
	long value[4];
	long timer;
};




/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (End of this section ... stop here)                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Conditions.
 */
#define COND_DRUNK		      0
#define COND_FULL		      1
#define COND_THIRST		      2



/*
 * Stats - KaVir.
 */
#define STAT_STR		      0
#define STAT_END		      1
#define STAT_REF		      2
#define STAT_FLE		      2



/*
 * Positions.
 */
#define POS_DEAD		      0
#define POS_MORTAL		      1
#define POS_INCAP		      2
#define POS_STUNNED		      3
#define POS_SLEEPING		      4
#define POS_MEDITATING		      5
#define POS_SITTING		      6
#define POS_RESTING		      7
#define POS_FIGHTING		      8
#define POS_STANDING		      9



/*
 * ACT bits for players.
 */
#define PLR_IS_NPC		      1	/* Don't EVER set.      */
#define PLR_AUTOEXIT		      8
#define PLR_AUTOLOOT		     16
#define PLR_AUTOSAC                  32
#define PLR_BLANK		     64
#define PLR_BRIEF		    128
#define PLR_COMBINE		    512
#define PLR_PROMPT		   1024
#define PLR_TELNET_GA		   2048
#define PLR_HOLYLIGHT		   4096
#define PLR_WIZINVIS		   8192
#define PLR_ANSI		  16384
#define PLR_COLOUR		  16384
#define	PLR_SILENCE		  32768
#define PLR_NO_EMOTE		  65536
#define PLR_INCOG                131072
#define PLR_NO_TELL		 262144
#define PLR_LOG			 524288
#define PLR_DENY		1048576
#define PLR_FREEZE		2097152
#define PLR_GODLESS	       16777216
#define PLR_WATCHER	       33554432
#define PLR_DROWSIGHT		67108864
#define PLR_EMBRACING          134217728
#define PLR_EMBRACED           268435456
#define PLR_ACID               536870912
#define PLR_SHROUD		1073741824

/*NEWBITS FOR PLAYERS..(added by sage)10/17/98*/
#define NEWBIE_PACK	1
#define THIRD_HAND	2
#define FOURTH_HAND	4
#define NEW_DROWHATE    8
#define NEW_DARKNESS    16
#define NEW_TIDE	32
#define NEW_COIL	64
#define NEW_MONKFLAME	128
#define NEW_CLANNABLE   256
#define NEW_CLOAK	512
#define NEW_STATUS      1024


#define NEW_QUIT	8192

// MAP BITS!
#define MAP_ON		1
#define MAP_COLOR	2
#define MAP_PLAYERS	4
#define MAP_NPCS	8
#define MAP_LETTERS	16


/*
 * EXTRA bits for players. (KaVir)
 */
				/*    1 */
				/*    2 */
#define EXTRA_TRUSTED		      4
#define EXTRA_NEWPASS		      8
#define EXTRA_OSWITCH		     16
#define EXTRA_SWITCH		     32
#define EXTRA_FAKE_CON		     64
#define TIED_UP			    128
#define GAGGED			    256
#define BLINDFOLDED		    512
				/* 1024 */
#define EXTRA_DONE		   2048
#define EXTRA_EXP		   4096
#define EXTRA_PREGNANT		   8192
#define EXTRA_LABOUR		  16384
#define EXTRA_BORN		  32768
#define EXTRA_PROMPT		  65536
#define EXTRA_MARRIED		 131072
			/*       262144 */
#define EXTRA_SAVED			 524288
#define EXTRA_CALL_ALL		1048576
#define EXTRA_ASH	        2097152
#define EXTRA_NOTE_TRUST       4194304
#define EXTRA_STANCE           8388608
#define EXTRA_ANTI_GODLESS    16777216
#define EXTRA_ZOMBIE	      33554432
#define EXTRA_POTENCY	      67108864
#define EXTRA_AWE	     134217728
#define EXTRA_DRAGON	     268435456
#define EXTRA_PLASMA	     536870912
#define EXTRA_EARTHMELD     1073741824
#define EXTRA_ROT	    2147483648

/*
 * AGE Bits.
 */
#define AGE_CHILDE		      0
#define AGE_NEONATE		      1
#define AGE_ANCILLA		      2
#define AGE_ELDER		      3
#define AGE_METHUSELAH		      4


/*
 * Stances for combat
 */
#define STANCE_NONE		     -1
#define STANCE_NORMAL		      0
#define STANCE_VIPER		      1
#define STANCE_CRANE		      2
#define STANCE_CRAB		      3
#define STANCE_MONGOOSE		      4
#define STANCE_BULL		      5
#define STANCE_MANTIS		      6
#define STANCE_DRAGON		      7
#define STANCE_TIGER		      8
#define STANCE_MONKEY		      9
#define STANCE_SWALLOW		     10


/*
 * Obsolete bits.
 */
#if 0
#define PLR_AUCTION		      4	/* Obsolete     */
#define PLR_CHAT		    256	/* Obsolete     */
#define PLR_NO_SHOUT		 131072	/* Obsolete     */
#endif



/*
 * Channel bits.
 */
#define	CHANNEL_AUCTION		      1
#define	CHANNEL_CHAT		      2
#define	CHANNEL_HACKER		      4
#define	CHANNEL_IMMTALK		      8
#define	CHANNEL_MUSIC		     16
#define	CHANNEL_QUESTION	     32
#define	CHANNEL_SHOUT		     64
#define	CHANNEL_YELL		    128
#define	CHANNEL_LOG		   1024
#define	CHANNEL_PRAY		   2048
#define	CHANNEL_INFO		   4096
#define	CHANNEL_SPARE		   8192
#define	CHANNEL_TELL		  16384
#define	CHANNEL_KTALK		  32768







#define CHANNEL_RASHA		4194303
#define CHANNEL_TYR		8388608
#define CHANNEL_MODI	       16799216

/*
 * Prototype for a mob.
 * This is the in-memory version of #MOBILES.
 */
struct mob_index_data
{
	MOB_INDEX_DATA *next;
	SPEC_OBJ_DATA *spec_obj;
	SHOP_DATA *pShop;
	CHAR_DATA *mount;
	CHAR_DATA *wizard;
	AREA_DATA *area;
	char *hunting;
	char *player_name;
	char *short_descr;
	char *long_descr;
	char *description;
	char *lord;
	char *clan;
	char *morph;
	char *createtime;
	char *pload;
	char *lasttime;
	char *lasthost;
	char *powertype;
	char *poweraction;
	char *prompt;
	char *cprompt;
	long spectype;
	long specpower;
	long loc_hp[7];
	long vnum;
	long count;
	long killed;
	long sex;
	long mounted;
	long home;
	long level;
	long immune;
	long polyaff;
	long vampaff;
	long itemaffect;
	long vamppass;
	long form;
	long act;
	long extra;
	long affected_by;
	long alignment;
	long hitroll;		/* Unused */
	long ac;			/* Unused */
	long hitnodice;		/* Unused */
	long hitsizedice;	/* Unused */
	long hitplus;		/* Unused */
	long damnodice;		/* Unused */
	long damsizedice;	/* Unused */
	long damplus;		/* Unused */
	long gold;		/* Unused */
};

struct clan_data
{
	long kills[6];
	long deaths[6];
};

struct leader_data
{
	char name[40];
	long kills;
	long deaths;
};

/*
 * One character (PC or NPC).
 */
struct char_data
{
	CHAR_DATA *next;
	CHAR_DATA *next_in_room;
	CHAR_DATA *master;
	CHAR_DATA *leader;
	CHAR_DATA *fighting;
	CHAR_DATA *reply;
	CHAR_DATA *mount;
	CHAR_DATA *wizard;
	SPEC_OBJ_DATA *spec_obj;
	MOB_INDEX_DATA *pIndexData;
	DESCRIPTOR_DATA *desc;
	AFFECT_DATA *affected;
	AFFECT_DATA *combat_affects;
	NOTE_DATA *pnote;
	OBJ_DATA *carrying;
	ROOM_INDEX_DATA *in_room;
	ROOM_INDEX_DATA *was_in_room;
	PC_DATA *pcdata;
	long mode;
	long bounty;
	long tll;
	long tlw;
	char *hunting;
	char *name;
	char *pload;
	char *short_descr;
	char *long_descr;
	char *description;
	char *lord;
	char *clan;
	CHAR_DATA *unveil;
	char *objdesc;
	char *morph;
	char *createtime;
	char *lasttime;
	char *lasthost;
	char *poweraction;
	char *powertype;
	char *prompt;
	char *cprompt;
	char *lastchat;
	long sex;
	long immune;
	long polyaff;
	long vampaff_a;
	long fight_timer;
	CHAR_DATA *embraced;
	CHAR_DATA *embracing;
	long itemaffect;
	long vamppass_a;
	long form;
	long generation;
	long vampgen_a;
	long spectype;
	long specpower;
	long loc_hp[7];
	long wpn[13];
	long spl[5];
	long cmbt[8];
	long stance[12];
	long beast;
	long mounted;
	long home;
	long level;
	long trust;
	long played;
	time_t logon;
	time_t save_time;
	long timer;
	long wait;
	long pkill;
	long pdeath;
	long mkill;
	long mdeath;
	long hit;
	long max_hit;
	long mana;
	long max_mana;
	long move;
	long max_move;
	long gold;
	long exp;
	long act;
	long extra;
	long newbits;
	long mapbits;
	long special;
	long affected_by;
	long swordtech;
	long position;
	long practice;
	long carry_weight;
	long carry_number;
	long saving_throw;
	long alignment;
	long hitroll;
	long damroll;
	long armor;
	long wimpy;
	long deaf;
	long paradox[3];
	long damcap[2];
	CHAR_DATA *blinkykill;
	long flag2;
	long spheres[9];
	long mflags;
	long quint[3];
	long imms[3];
	CHAR_DATA *challenged;
	long awins;
	long alosses;
	long rows;
	long cols;
};



/*
 * Data which only PC's have.
 */
struct pc_data
{
	PC_DATA *next;
	ASSIST_DATA *assist;
	CHAR_DATA *familiar;
	CHAR_DATA *partner;
	CHAR_DATA *propose;
	OBJ_DATA *chobj;
	OBJ_DATA *memorised;
	char *pwd;
	char *bamfin;
	char *bamfout;
	char *title;
	char *conception;
	char *parents;
	char *cparents;
	char *marriage;
	char *email;
	char *autostance;
	char *switchname;
	char *krank;
	char *enter;
	char *exit;
	char *login;
	char *logout;
	long status;
	long voted;
	long souls;
	long perm_str;
	long perm_int;
	long perm_wis;
	long perm_dex;
	long perm_con;
	long mod_str;
	long mod_int;
	long mod_wis;
	long mod_dex;
	long mod_con;
	long hack;
	long colr;
	long class;
	long changesplace;
	long quest;
	long powers[20];
	long stats[12];
	long wolf;
	long rank;
	long deity;
	long deitypoints;
	long deityrank;
	long demonic_a;
	long language[2];
	long stage[3];
	long wolfform[2];
	long score[6];
	long runes[4];
	long disc_a[11];
	long genes[10];
	long fake_skill;
	long fake_stance;
	long fake_hit;
	long fake_dam;
	long fake_hp;
	long fake_mana;
	long fake_move;
	long fake_ac;
	long obj_vnum;
	long condition[3];
	long learned[MAX_SKILL];
	long stat_ability[4];
	long stat_amount[4];
	long stat_duration[4];
	long exhaustion;
	long followers;
	long generation;
	long kingdom;
	long kingdom_invite;
	bool confirm_delete;
	long safe_wait;
	long vision;
	long safe_timer;
	long assists;
	long surveying;
	/* IMC */
	long imc_deny;
	long imc_allow;
	long imc_deaf;
	char *ice_listen;
	char *rreply;
	char *rreply_name;
};





/*
 * Liquids.
 */
#define LIQ_WATER        0
#define LIQ_MAX		16

struct liq_type
{
	char *liq_name;
	char *liq_color;
	long liq_affect[3];
};



/*
 * Extra description data for a room or object.
 */
struct extra_descr_data
{
	EXTRA_DESCR_DATA *next;	/* Next in list                     */
	char *keyword;		/* Keyword in look/examine          */
	char *description;	/* What to see                      */
};



/*
 * Prototype for an object.
 */
struct obj_index_data
{
	OBJ_INDEX_DATA *next;
	EXTRA_DESCR_DATA *extra_descr;
	AFFECT_DATA *affected;
	SPEC_OBJ_DATA *spec_obj;
	char *name;
	char *short_descr;
	char *description;
	char *chpoweron;
	char *chpoweroff;
	char *chpoweruse;
	char *victpoweron;
	char *victpoweroff;
	char *victpoweruse;
	char *questmaker;
	char *questowner;
	char *pagetitle;
	char *pagelines;
	long vnum;
	long item_type;
	long extra_flags;
	long extra_flags2;
	long wear_flags;
	long count;
	long weight;
	long spectype;
	long specpower;
	long condition;
	long toughness;
	long resistance;
	long quest;
	long points;
	long cost;		/* Unused */
	long value[4];
};



/*
 * One object.
 */
struct obj_data
{
	OBJ_DATA *next;
	OBJ_DATA *next_content;
	OBJ_DATA *contains;
	OBJ_DATA *in_obj;
	SPEC_OBJ_DATA *spec_obj;
	CHAR_DATA *carried_by;
	CHAR_DATA *chobj;
	EXTRA_DESCR_DATA *extra_descr;
	AFFECT_DATA *affected;
	OBJ_INDEX_DATA *pIndexData;
	ROOM_INDEX_DATA *in_room;
	char *name;
	char *short_descr;
	char *description;
	char *chpoweron;
	char *chpoweroff;
	char *chpoweruse;
	char *victpoweron;
	char *victpoweroff;
	char *victpoweruse;
	char *questmaker;
	char *questowner;
	char *pagetitle;
	char *pagelines;

	long item_type;
	long extra_flags;
	long extra_flags2;
	long wear_flags;
	long wear_loc;
	long weight;
	long spectype;
	long specpower;
	long condition;
	long toughness;
	long resistance;
	long quest;
	long points;
	long cost;
	long level;
	long timer;
	long value[4];
	long weapflags;
	long coordy;
	long coordx;
};



/*
 * Exit data.
 */
struct exit_data
{
	ROOM_INDEX_DATA *to_room;
	long vnum;
	long exit_info;
	long key;
	char *keyword;
	char *description;
};



/*
 * Room text checking data.
 */
typedef struct roomtext_data
{
	long type;
	long power;
	long mob;
	char *input;
	char *output;
	char *choutput;
	char *name;
	struct roomtext_data *next;
}
ROOMTEXT_DATA;



/*
 * Reset commands:
 *   '*': comment
 *   'M': read a mobile
 *   'O': read an object
 *   'P': put object in object
 *   'G': give object to mobile
 *   'E': equip object to mobile
 *   'D': set state of door
 *   'R': randomize room exits
 *   'S': stop (end of list)
 */

/*
 * Area-reset definition.
 */
struct reset_data
{
	RESET_DATA *next;
	char command;
	long arg1;
	long arg2;
	long arg3;
};



/*
 * Area definition.
 */
struct area_data
{
	AREA_DATA *next;
	RESET_DATA *reset_first;
	RESET_DATA *reset_last;
	char *name;
	long area;
	long age;
	long nplayer;
	char *filename;
	long mod;
	long place;
	long level;
	long hvnum;
	long lvnum;
};



/*
 * Room type.
 */
struct room_index_data
{
	ROOM_INDEX_DATA *next;
	CHAR_DATA *people;
	OBJ_DATA *contents;
	EXTRA_DESCR_DATA *extra_descr;
	AREA_DATA *area;
	SPEC_OBJ_DATA *spec_obj;
	EXIT_DATA *exit[6];
	ROOMTEXT_DATA *roomtext;
	char *track[5];
	char *name;
	char *description;
	long vnum;
	long room_flags;
	long light;
	long blood;
	long track_dir[5];
	long sector_type;
	long coordx;
	long coordy;
};



/*
 * Types of attacks.
 * Must be non-overlapping with spell/skill types,
 * but may be arbitrary beyond that.
 */
#define TYPE_UNDEFINED               -1
#define TYPE_HIT                     1000



/*
 *  Target types.
 */
#define TAR_IGNORE		    0
#define TAR_CHAR_OFFENSIVE	    1
#define TAR_CHAR_DEFENSIVE	    2
#define TAR_CHAR_SELF		    3
#define TAR_OBJ_INV		    4



#define TARGET_CHAR                 0
#define TARGET_OBJ                  1
#define TARGET_ROOM                 2
#define TARGET_NONE                 3

#define PURPLE_MAGIC		    0
#define RED_MAGIC		    1
#define BLUE_MAGIC		    2
#define GREEN_MAGIC		    3
#define YELLOW_MAGIC		    4



/*
 * Skills include spells as a particular case.
 */
struct skill_type
{
	char *name;		/* Name of skill                */
	long skill_level;	/* Level needed by class        */
	SPELL_FUN *spell_fun;	/* Spell pointer (for spells)   */
	long target;		/* Legal targets                */
	long minimum_position;	/* Position for caster / user   */
	long *pgsn;		/* Pointer to associated gsn    */
	long slot;		/* Slot for #OBJECT loading     */
	long min_mana;		/* Minimum mana used            */
	long beats;		/* Waiting time after use       */
	char *noun_damage;	/* Damage message               */
	char *msg_off;		/* Wear off message             */
};



/*
 * These are skill_lookup return values for common skills and spells.
 */
extern long gsn_swordtech;
extern long gsn_godgift;
extern long gsn_godcurse;
extern long gsn_bard;
extern long gsn_backstab;
extern long gsn_hide;
extern long gsn_peek;
extern long gsn_pick_lock;
extern long gsn_sneak;
extern long gsn_steal;
extern long gsn_inferno;
extern long gsn_blinky;
extern long gsn_fastdraw;
extern long gsn_berserk;
extern long gsn_punch;
extern long gsn_elbow;
extern long gsn_headbutt;
extern long gsn_buffet;
extern long gsn_tail;
extern long gsn_shiroken;
extern long gsn_joust;
extern long gsn_sweep;
extern long gsn_knee;
extern long gsn_spiderform;
extern long gsn_garotte;
extern long gsn_disarm;
extern long gsn_hurl;
extern long gsn_kick;
extern long gsn_circle;
extern long gsn_talon;
extern long gsn_rescue;
extern long gsn_track;
extern long gsn_polymorph;
extern long gsn_web;
extern long gsn_drowfire;
extern long gsn_godbless;	/* Vic - Monks */
extern long gsn_llothbless;
extern long gsn_totalblind;	/* Vic - Monks */
extern long gsn_blindness;
extern long gsn_charm_person;
extern long gsn_curse;
extern long gsn_invis;
extern long gsn_mass_invis;
extern long gsn_poison;
extern long gsn_sleep;

extern long gsn_paradox;
extern long gsn_darkness;
extern long gsn_chill;
extern long gsn_pummel;
extern long gsn_shred;

/*
 * Utility macros.
 */
#define UMIN(a, b)		((a) < (b) ? (a) : (b))
#define UMAX(a, b)		((a) > (b) ? (a) : (b))
#define URANGE(a, b, c)		((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define LOWER(c)		((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)		((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))
#define IS_SET(flag, bit)	((flag) & (bit))
#define SET_BIT(var, bit)	((var) |= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))
#define SWITCH_BIT(var, bit)	if(IS_SET(var,bit)) {REMOVE_BIT(var,bit);} else {SET_BIT(var,bit);}

#define IS_LEADER(ch)		(!str_cmp((ch)->name, leader[0]->name) || !str_cmp((ch)->name, leader[1]->name) || !str_cmp((ch)->name, leader[2]->name) )
#define IS_FODDER(ch)		(!str_cmp((ch)->name, leader[3]->name) || !str_cmp((ch)->name, leader[4]->name) || !str_cmp((ch)->name, leader[5]->name) )
#define IS_WEAP(obj,stat)	(IS_SET((obj)->weapflags, (stat)))
#define IS_WEAPON(obj)		((obj)->item_type == ITEM_WEAPON ? TRUE : FALSE)
/*
 * Character macros.
 */
#define IS_APPRENTICE(ch)	((ch)->pcdata->stats[UNI_GEN] == 2)
#define IS_MASTER(ch)		((ch)->pcdata->stats[UNI_GEN] == 1)
#define IS_NEWFLAG(ch, sn)      (IS_SET((ch)->flag2, (sn)))
#define IS_NPC(ch)		(IS_SET((ch)->act, ACT_IS_NPC))
#define IS_JUDGE(ch)		(get_trust(ch) >= LEVEL_JUDGE)
#define IS_HIGHJUDGE(ch)	(get_trust(ch) >= LEVEL_HIGHJUDGE)
#define IS_IMMORTAL(ch)		(get_trust(ch) >= LEVEL_IMMORTAL)
#define IS_HERO(ch)		(get_trust(ch) >= LEVEL_HERO)
#define CAN_PK(ch)		((get_trust(ch) >= 3 && get_trust(ch) <= 6) || IS_IMMORTAL(ch))
#define IS_AFFECTED(ch, sn)	(IS_SET((ch)->affected_by, (sn)))
#define IS_SWORDTECH(ch, sn)	(IS_SET((ch)->swordtech, (sn)))
#define IS_SPEAKING(ch, sn)	(IS_SET((ch)->pcdata->language[0], (sn)))
#define CAN_SPEAK(ch, sn)	(IS_SET((ch)->pcdata->language[1], (sn)))
#define CAN_DANCE(ch, sn)	(IS_SET((ch)->pcdata->powers[1], (sn)))
#define IS_ITEMAFF(ch, sn)	(IS_SET((ch)->itemaffect, (sn)))
#define IS_IMMUNE(ch, sn)	(IS_SET((ch)->immune, (sn)))
#define IS_VAMPAFF(ch, sn)	(IS_SET((ch)->pcdata->stats[UNI_AFF], (sn)))
#define IS_VAMPPASS(ch, sn)	(IS_SET((ch)->pcdata->stats[UNI_CURRENT], (sn)))
#define IS_FORM(ch, sn)		(IS_SET((ch)->form, (sn)))
#define IS_POLYAFF(ch, sn)	(IS_SET((ch)->polyaff, (sn)))
#define IS_EXTRA(ch, sn)	(IS_SET((ch)->extra, (sn)))
#define IS_STANCE(ch, sn)	(ch->stance[0] == sn)
#define IS_DEMPOWER(ch, sn)	(IS_SET((ch)->pcdata->powers[DPOWER_FLAGS], (sn)))
#define IS_DEMAFF(ch, sn)	(IS_SET((ch)->pcdata->powers[DPOWER_CURRENT], (sn)))
#define IS_MPOWER(ch, sn)	(IS_SET((ch)->pcdata->powers[MPOWER_POWER], (sn)))
#define IS_PPOWER(ch, sn)	(IS_SET((ch)->pcdata->powers[PPOWER_FLAGS], (sn)))	/* Paladin - Loki */
#define IS_PAFF(ch, sn)	(IS_SET((ch)->pcdata->powers[PPOWER_CURRENT], (sn)))	/* Paladin - Loki */
#define IS_CLASS(ch, CLASS)	(IS_NPC((ch)) ? 0 : IS_SET((ch)->pcdata->class, CLASS))

#define IS_HEAD(ch, sn)		(IS_SET((ch)->loc_hp[0], (sn)))
#define IS_BODY(ch, sn)		(IS_SET((ch)->loc_hp[1], (sn)))
#define IS_ARM_L(ch, sn)	(IS_SET((ch)->loc_hp[2], (sn)))
#define IS_ARM_R(ch, sn)	(IS_SET((ch)->loc_hp[3], (sn)))
#define IS_LEG_L(ch, sn)	(IS_SET((ch)->loc_hp[4], (sn)))
#define IS_LEG_R(ch, sn)	(IS_SET((ch)->loc_hp[5], (sn)))
#define IS_BLEEDING(ch, sn)	(IS_SET((ch)->loc_hp[6], (sn)))

#define IN_CLAN( ch )           (IS_NPC(ch)?0:ch->clan)
#define IS_PLAYING( d )         (d->connected==CON_PLAYING)

#define IS_GOOD(ch)		(ch->alignment >= 350)
#define IS_EVIL(ch)		(ch->alignment <= -350)
#define IS_NEUTRAL(ch)		(!IS_GOOD(ch) && !IS_EVIL(ch))

#define IS_AWAKE(ch)		(ch->position > POS_SLEEPING)
#define GET_AC(ch)		((ch)->armor				    \
				    + ( IS_AWAKE(ch)			    \
				    ? dex_app[get_curr_dex(ch)].defensive   \
				    : 0 ))
#define GET_HITROLL(ch)		((ch)->hitroll+str_app[get_curr_str(ch)].tohit)
#define GET_DAMROLL(ch)		((ch)->damroll+str_app[get_curr_str(ch)].todam)

#define IS_OUTSIDE(ch)		(!IS_SET(				    \
				    (ch)->in_room->room_flags,		    \
				    ROOM_INDOORS))

#define WAIT_STATE(ch, npulse)	((ch)->wait = UMAX((ch)->wait, (npulse)))

#define GET_PROPER_NAME(ch)     (IS_NPC((ch)) ? (ch)->short_descr : (ch)->pcdata->switchname)
#define GET_PC_NAME(ch)     	(IS_NPC((ch)) ? "<npc>" : (ch)->pcdata->switchname)

/*
 * Object Macros.
 */
#define CAN_WEAR(obj, part)	(IS_SET((obj)->wear_flags,  (part)))
#define IS_OBJ_STAT(obj, stat)	(IS_SET((obj)->extra_flags, (stat)))
#define IS_OBJ_STAT2(obj, stat) (IS_SET((obj)->extra_flags2, (stat)))


/*
 * Description macros.
 */
#define PERS(ch, looker)	( can_see( looker, (ch) ) ?		\
				( IS_NPC(ch) ? (ch)->short_descr	\
				: ( IS_AFFECTED( (ch), AFF_POLYMORPH) ?   \
				(ch)->morph : (ch)->name ) )		\
				: "someone" )

// Room Macros
#define IN_HELL(ch)		( !IS_NPC((ch)) && (ch)->in_room && \
				(ch)->in_room->vnum >= 8151 && \
				(ch)->in_room->vnum <= 8231 )

/*
 * Structure for a command in the command lookup table.
 */
struct cmd_type
{
	char *const name;
	DO_FUN *do_fun;
	long position;
	long level;
	long log;
};



/*
 * Structure for a social in the socials table.
 */
struct social_type
{
	char *const name;
	char *const char_no_arg;
	char *const others_no_arg;
	char *const char_found;
	char *const others_found;
	char *const vict_found;
	char *const char_auto;
	char *const others_auto;
};


/*
 * Structure for an X-social in the socials table.
 */
struct xsocial_type
{
	char *const name;
	char *const char_no_arg;
	char *const others_no_arg;
	char *const char_found;
	char *const others_found;
	char *const vict_found;
	char *const char_auto;
	char *const others_auto;
	long gender;
	long stage;
	long position;
	long self;
	long other;
	long extra;
	bool chance;
};



/*
 * Global constants.
 */
extern const struct str_app_type str_app[26];
extern const struct int_app_type int_app[26];
extern const struct wis_app_type wis_app[26];
extern const struct dex_app_type dex_app[26];
extern const struct con_app_type con_app[26];

extern const struct cmd_type cmd_table[];
extern const struct liq_type liq_table[LIQ_MAX];
extern const struct skill_type skill_table[MAX_SKILL];
extern struct clan_table_type clan_table[MAX_CLAN];
extern struct artifact_type artifact_table[MAX_ART];
extern const struct social_type social_table[];
extern const struct xsocial_type xsocial_table[];
extern const char *sphere_name[9];


/*
 * Global variables.
 */
extern HELP_DATA *help_first;
extern SHOP_DATA *shop_first;

extern BAN_DATA *ban_list;
extern CHAR_DATA *char_list;
extern SPEC_OBJ_DATA *spec_obj_list;
extern DESCRIPTOR_DATA *descriptor_list;
extern NOTE_DATA *note_list;
extern OBJ_DATA *object_list;
extern LEADER_DATA *leader[6];
extern CLAN_DATA *clan_info;
extern AFFECT_DATA *affect_free;
extern BAN_DATA *ban_free;
extern CHAR_DATA *char_free;
extern DESCRIPTOR_DATA *descriptor_free;
extern EXTRA_DESCR_DATA *extra_descr_free;
extern ROOMTEXT_DATA *roomtext_free;
extern NOTE_DATA *note_free;
extern OBJ_DATA *obj_free;
extern PC_DATA *pcdata_free;
extern SPEC_OBJ_DATA *spec_obj_free;
extern ASSIST_DATA *assist_free;

extern char bug_buf[];
extern time_t current_time;
extern bool fLogAll;
extern FILE *fpReserve;
extern KILL_DATA kill_table[];
extern char log_buf[];
extern TIME_INFO_DATA time_info;
extern WEATHER_DATA weather_info;

/*
extern          long			arena;
*/

/* For Crashes */
extern long debug_counter;
extern char *debug_procedure[20];
extern char *debug_last_character;
extern long debug_last_room;
extern long debug_stage;
extern char *debug_last_command;
extern bool debug_dont_run;


DECLARE_DO_FUN(do_accept);
DECLARE_DO_FUN(do_acid);
DECLARE_DO_FUN(do_activate);
DECLARE_DO_FUN(do_affects);
DECLARE_DO_FUN(do_afk);
DECLARE_DO_FUN(do_alignment);
DECLARE_DO_FUN(do_allow);
DECLARE_DO_FUN(do_almightyfavor);
DECLARE_DO_FUN(do_anarch);
DECLARE_DO_FUN(do_ansi);
DECLARE_DO_FUN(do_answer);
DECLARE_DO_FUN(do_apply);
DECLARE_DO_FUN(do_areas);
DECLARE_DO_FUN(do_artifact);
DECLARE_DO_FUN(do_ashes);
DECLARE_DO_FUN(do_assassinate);
DECLARE_DO_FUN(do_astralwalk);
DECLARE_DO_FUN(do_at);
DECLARE_DO_FUN(do_autoexit);
DECLARE_DO_FUN(do_autoloot);
DECLARE_DO_FUN(do_autosac);
DECLARE_DO_FUN(do_autostance);
DECLARE_DO_FUN(do_awe);
DECLARE_DO_FUN(do_backstab);
DECLARE_DO_FUN(do_bamfin);
DECLARE_DO_FUN(do_bamfout);
DECLARE_DO_FUN(do_ban);
DECLARE_DO_FUN(do_bash);
DECLARE_DO_FUN(do_beckon);
DECLARE_DO_FUN(do_berserk);
DECLARE_DO_FUN(do_bind);
DECLARE_DO_FUN(do_birth);
DECLARE_DO_FUN(do_blank);
DECLARE_DO_FUN(do_blink);
DECLARE_DO_FUN(do_blindfold);
DECLARE_DO_FUN(do_bloodagony);
DECLARE_DO_FUN(do_bloodwall);
DECLARE_DO_FUN(do_bloodwater);
DECLARE_DO_FUN(do_bomuzite);
DECLARE_DO_FUN(do_bonemod);
DECLARE_DO_FUN(do_bounty);
DECLARE_DO_FUN(do_bragcap);
DECLARE_DO_FUN(do_brandish);
DECLARE_DO_FUN(do_breakup);
DECLARE_DO_FUN(do_brief);
DECLARE_DO_FUN(do_bug);
DECLARE_DO_FUN(do_burrow);
DECLARE_DO_FUN(do_buy);
DECLARE_DO_FUN(do_call);
DECLARE_DO_FUN(do_calm);
DECLARE_DO_FUN(do_cauldron);
DECLARE_DO_FUN(do_caust);
DECLARE_DO_FUN(do_cast);
DECLARE_DO_FUN(do_celestialpath);
DECLARE_DO_FUN(do_champions);
DECLARE_DO_FUN(do_change);
DECLARE_DO_FUN(do_channels);
DECLARE_DO_FUN(do_chaosblast);
DECLARE_DO_FUN(do_chat);
DECLARE_DO_FUN(do_chatlog);
DECLARE_DO_FUN(do_circle);
DECLARE_DO_FUN(do_claim);
DECLARE_DO_FUN(do_clandisc);
DECLARE_DO_FUN(do_clanname);
DECLARE_DO_FUN(do_class);
DECLARE_DO_FUN(do_classset);
DECLARE_DO_FUN(do_claws);
DECLARE_DO_FUN(do_clearstats);
DECLARE_DO_FUN(do_clearvam);
DECLARE_DO_FUN(do_clearvamp);
DECLARE_DO_FUN(do_clone);
DECLARE_DO_FUN(do_close);
DECLARE_DO_FUN(do_code);
DECLARE_DO_FUN(do_coil);
DECLARE_DO_FUN(do_colorize);
DECLARE_DO_FUN(do_cols);
DECLARE_DO_FUN(do_command);
DECLARE_DO_FUN(do_commands);
DECLARE_DO_FUN(do_compare);
DECLARE_DO_FUN(do_complete);
DECLARE_DO_FUN(do_conceal);
DECLARE_DO_FUN(do_config);
DECLARE_DO_FUN(do_configuration);
DECLARE_DO_FUN(do_confuse);
DECLARE_DO_FUN(do_consent);
DECLARE_DO_FUN(do_consider);
DECLARE_DO_FUN(do_control);
DECLARE_DO_FUN(do_convert_pfiles);
DECLARE_DO_FUN(do_countdown);
DECLARE_DO_FUN(do_cprompt);
DECLARE_DO_FUN(do_crack);
DECLARE_DO_FUN(do_create);
DECLARE_DO_FUN(do_credits);
DECLARE_DO_FUN(do_dark_garotte);
DECLARE_DO_FUN(do_darkblaze);
DECLARE_DO_FUN(do_darkheart);
DECLARE_DO_FUN(do_darkness);
DECLARE_DO_FUN(do_death);
DECLARE_DO_FUN(do_decapitate);
DECLARE_DO_FUN(do_defences);
DECLARE_DO_FUN(do_deimm);
DECLARE_DO_FUN(do_delet);
DECLARE_DO_FUN(do_delete);
DECLARE_DO_FUN(do_deletechars);
DECLARE_DO_FUN(do_demonarmour);
DECLARE_DO_FUN(do_demonform);
DECLARE_DO_FUN(do_deny);
DECLARE_DO_FUN(do_description);
DECLARE_DO_FUN(do_diablerise);
DECLARE_DO_FUN(do_diagnose);
DECLARE_DO_FUN(do_dice);
DECLARE_DO_FUN(do_dinferno);
DECLARE_DO_FUN(do_disable);
DECLARE_DO_FUN(do_disarm);
DECLARE_DO_FUN(do_disconnect);
DECLARE_DO_FUN(do_dismount);
DECLARE_DO_FUN(do_divorce);
DECLARE_DO_FUN(do_down);
DECLARE_DO_FUN(do_dragonform);
DECLARE_DO_FUN(do_drain);
DECLARE_DO_FUN(do_draw);
DECLARE_DO_FUN(do_drink);
DECLARE_DO_FUN(do_drop);
DECLARE_DO_FUN(do_drowcreate);
DECLARE_DO_FUN(do_drowfire);
DECLARE_DO_FUN(do_drowhate);
DECLARE_DO_FUN(do_drowpowers);
DECLARE_DO_FUN(do_drows);
DECLARE_DO_FUN(do_drowshield);
DECLARE_DO_FUN(do_drowsight);
DECLARE_DO_FUN(do_earthmeld);
DECLARE_DO_FUN(do_earthshatter);
DECLARE_DO_FUN(do_east);
DECLARE_DO_FUN(do_eat);
DECLARE_DO_FUN(do_echo);
DECLARE_DO_FUN(do_email);
DECLARE_DO_FUN(do_embrace);
DECLARE_DO_FUN(do_empty);
DECLARE_DO_FUN(do_emote);
DECLARE_DO_FUN(do_enter);
DECLARE_DO_FUN(do_entrance);
DECLARE_DO_FUN(do_equipment);
DECLARE_DO_FUN(do_escape);
DECLARE_DO_FUN(do_evileye);
DECLARE_DO_FUN(do_examine);
DECLARE_DO_FUN(do_exits);
DECLARE_DO_FUN(do_exlist);
DECLARE_DO_FUN(do_eyesofgod);
DECLARE_DO_FUN(do_familiar);
DECLARE_DO_FUN(do_fangs);
DECLARE_DO_FUN(do_favour);
DECLARE_DO_FUN(do_fcommand);
DECLARE_DO_FUN(do_fear);
DECLARE_DO_FUN(do_feed);
DECLARE_DO_FUN(do_fightstyle);
DECLARE_DO_FUN(do_fill);
DECLARE_DO_FUN(do_findbest);
DECLARE_DO_FUN(do_findmobs);
DECLARE_DO_FUN(do_findvnums);
DECLARE_DO_FUN(do_finger);
DECLARE_DO_FUN(do_fix);
DECLARE_DO_FUN(do_flamehands);
DECLARE_DO_FUN(do_flee);
DECLARE_DO_FUN(do_fleshcraft);
DECLARE_DO_FUN(do_flex);
DECLARE_DO_FUN(do_follow);
DECLARE_DO_FUN(do_forcesilent);
DECLARE_DO_FUN(do_for);
DECLARE_DO_FUN(do_force);
DECLARE_DO_FUN(do_forceauto);
DECLARE_DO_FUN(do_formillusion);
DECLARE_DO_FUN(do_freevnums);
DECLARE_DO_FUN(do_freeze);
DECLARE_DO_FUN(do_frenzy);
DECLARE_DO_FUN(do_gag);
DECLARE_DO_FUN(do_garotte);
DECLARE_DO_FUN(do_gate);
DECLARE_DO_FUN(do_generation);
DECLARE_DO_FUN(do_get);
DECLARE_DO_FUN(do_ghoul);
DECLARE_DO_FUN(do_gift);
DECLARE_DO_FUN(do_give);
DECLARE_DO_FUN(do_godcurse);
DECLARE_DO_FUN(do_godgift);
DECLARE_DO_FUN(do_goto);
DECLARE_DO_FUN(do_gourge);
DECLARE_DO_FUN(do_guardian);
DECLARE_DO_FUN(do_graft);
DECLARE_DO_FUN(do_grant);
DECLARE_DO_FUN(do_group);
DECLARE_DO_FUN(do_gtell);
DECLARE_DO_FUN(do_hackify);
DECLARE_DO_FUN(do_hagswrinkles);
DECLARE_DO_FUN(do_hara_kiri);
DECLARE_DO_FUN(do_heal);
DECLARE_DO_FUN(do_help);
DECLARE_DO_FUN(do_hide);
DECLARE_DO_FUN(do_holylight);
DECLARE_DO_FUN(do_home);
DECLARE_DO_FUN(do_hooves);
DECLARE_DO_FUN(do_horns);
DECLARE_DO_FUN(do_hotreboot);
DECLARE_DO_FUN(do_humanform);
DECLARE_DO_FUN(do_huh);
DECLARE_DO_FUN(do_hunt);
DECLARE_DO_FUN(do_hurl);
DECLARE_DO_FUN(do_idea);
DECLARE_DO_FUN(do_immolate);
DECLARE_DO_FUN(do_immune);
DECLARE_DO_FUN(do_immtalk);
DECLARE_DO_FUN(do_incog);
DECLARE_DO_FUN(do_infirmity);
DECLARE_DO_FUN(do_inpart);
DECLARE_DO_FUN(do_insult);
DECLARE_DO_FUN(do_introduce);
DECLARE_DO_FUN(do_inventory);
DECLARE_DO_FUN(do_invis);
DECLARE_DO_FUN(do_kakusu);
DECLARE_DO_FUN(do_kanzuite);
DECLARE_DO_FUN(do_katana);
DECLARE_DO_FUN(do_kick);
DECLARE_DO_FUN(do_kill);
DECLARE_DO_FUN(do_kingdom);
DECLARE_DO_FUN(do_klaive);
DECLARE_DO_FUN(do_klist);
DECLARE_DO_FUN(do_koryou);
DECLARE_DO_FUN(do_krecall);
DECLARE_DO_FUN(do_ktalk);
DECLARE_DO_FUN(do_lamprey);
DECLARE_DO_FUN(do_leap);
DECLARE_DO_FUN(do_leech);
DECLARE_DO_FUN(do_level);
DECLARE_DO_FUN(do_lifespan);
DECLARE_DO_FUN(do_list);
DECLARE_DO_FUN(do_locate);
DECLARE_DO_FUN(do_lock);
DECLARE_DO_FUN(do_log);
DECLARE_DO_FUN(do_look);
DECLARE_DO_FUN(do_maketoken);
DECLARE_DO_FUN(do_mantra);
DECLARE_DO_FUN(do_map);
DECLARE_DO_FUN(do_marry);
DECLARE_DO_FUN(do_mask);
DECLARE_DO_FUN(do_mclan);
DECLARE_DO_FUN(do_mclear);
DECLARE_DO_FUN(do_meditate);
DECLARE_DO_FUN(do_memory);
DECLARE_DO_FUN(do_mesmerise);
DECLARE_DO_FUN(do_michi);
DECLARE_DO_FUN(do_mienaku);
DECLARE_DO_FUN(do_mindblast);
DECLARE_DO_FUN(do_mirror);
DECLARE_DO_FUN(do_mitsukeru);
DECLARE_DO_FUN(do_mfind);
DECLARE_DO_FUN(do_mload);
DECLARE_DO_FUN(do_monkarmor);
DECLARE_DO_FUN(do_moonarmour);
DECLARE_DO_FUN(do_morph);
DECLARE_DO_FUN(do_mortal);
DECLARE_DO_FUN(do_mount);
DECLARE_DO_FUN(do_mset);
DECLARE_DO_FUN(do_mstat);
DECLARE_DO_FUN(do_murder);
DECLARE_DO_FUN(do_music);
DECLARE_DO_FUN(do_mwhere);
DECLARE_DO_FUN(do_newbiepack);
DECLARE_DO_FUN(do_news);
DECLARE_DO_FUN(do_nightsight);
DECLARE_DO_FUN(do_ninjaarmor);
DECLARE_DO_FUN(do_noemote);
DECLARE_DO_FUN(do_north);
DECLARE_DO_FUN(do_nosummon);
DECLARE_DO_FUN(do_note);
DECLARE_DO_FUN(do_notell);
DECLARE_DO_FUN(do_ntrust);
DECLARE_DO_FUN(do_objmask);
DECLARE_DO_FUN(do_oclone);
DECLARE_DO_FUN(do_offer);
DECLARE_DO_FUN(do_offersoul);
DECLARE_DO_FUN(do_ofind);
DECLARE_DO_FUN(do_oload);
DECLARE_DO_FUN(do_omni);
DECLARE_DO_FUN(do_open);
DECLARE_DO_FUN(do_order);
DECLARE_DO_FUN(do_oreturn);
DECLARE_DO_FUN(do_oset);
DECLARE_DO_FUN(do_ostat);
DECLARE_DO_FUN(do_oswitch);
DECLARE_DO_FUN(do_otransfer);
DECLARE_DO_FUN(do_palmthrust);
DECLARE_DO_FUN(do_paradox);
DECLARE_DO_FUN(do_password);
DECLARE_DO_FUN(do_peace);
DECLARE_DO_FUN(do_pick);
DECLARE_DO_FUN(do_pigeon);
DECLARE_DO_FUN(do_ping);
DECLARE_DO_FUN(do_play);
DECLARE_DO_FUN(do_plasma);
DECLARE_DO_FUN(do_pload);
DECLARE_DO_FUN(do_poison);
DECLARE_DO_FUN(do_possession);
DECLARE_DO_FUN(do_potency);
DECLARE_DO_FUN(do_prayer);
DECLARE_DO_FUN(do_preserve);
DECLARE_DO_FUN(do_press);
DECLARE_DO_FUN(do_priceless);
DECLARE_DO_FUN(do_prompt);
DECLARE_DO_FUN(do_pset);
DECLARE_DO_FUN(do_pull);
DECLARE_DO_FUN(do_punch);
DECLARE_DO_FUN(do_purification);
DECLARE_DO_FUN(do_purge);
DECLARE_DO_FUN(do_put);
DECLARE_DO_FUN(do_putrefaction);
DECLARE_DO_FUN(do_practice);
DECLARE_DO_FUN(do_principles);
DECLARE_DO_FUN(do_propose);
DECLARE_DO_FUN(do_qmake);
DECLARE_DO_FUN(do_qset);
DECLARE_DO_FUN(do_qstat);
DECLARE_DO_FUN(do_qtrust);
DECLARE_DO_FUN(do_quaff);
DECLARE_DO_FUN(do_quest);
DECLARE_DO_FUN(do_question);
DECLARE_DO_FUN(do_qui);
DECLARE_DO_FUN(do_quit);
DECLARE_DO_FUN(do_racepower);
DECLARE_DO_FUN(do_rage);
DECLARE_DO_FUN(do_rating);
DECLARE_DO_FUN(do_readaura);
DECLARE_DO_FUN(do_reboo);
DECLARE_DO_FUN(do_recall);
DECLARE_DO_FUN(do_recall2);
DECLARE_DO_FUN(do_recharge);
DECLARE_DO_FUN(do_recho);
DECLARE_DO_FUN(do_recite);
DECLARE_DO_FUN(do_regenerate);
DECLARE_DO_FUN(do_reimburse);
DECLARE_DO_FUN(do_relevel);
DECLARE_DO_FUN(do_release);
DECLARE_DO_FUN(do_reload);
DECLARE_DO_FUN(do_remove);
DECLARE_DO_FUN(do_rent);
DECLARE_DO_FUN(do_repeat);
DECLARE_DO_FUN(do_reply);
DECLARE_DO_FUN(do_report);
DECLARE_DO_FUN(do_rescue);
DECLARE_DO_FUN(do_rest);
DECLARE_DO_FUN(do_restore);
DECLARE_DO_FUN(do_return);
DECLARE_DO_FUN(do_roll);
DECLARE_DO_FUN(do_roomnames);
DECLARE_DO_FUN(do_rows);
DECLARE_DO_FUN(do_rset);
DECLARE_DO_FUN(do_rstat);
DECLARE_DO_FUN(do_sacredinvisibility);
DECLARE_DO_FUN(do_sacrifice);
DECLARE_DO_FUN(do_safe);
DECLARE_DO_FUN(do_save);
DECLARE_DO_FUN(do_say);
DECLARE_DO_FUN(do_scan);
DECLARE_DO_FUN(do_scatter);
DECLARE_DO_FUN(do_score);
DECLARE_DO_FUN(do_scream);
DECLARE_DO_FUN(do_scry);
DECLARE_DO_FUN(do_sell);
DECLARE_DO_FUN(do_serenity);
DECLARE_DO_FUN(do_serpent);
DECLARE_DO_FUN(do_shadowsight);
DECLARE_DO_FUN(do_shadowplane);
DECLARE_DO_FUN(do_shadowstep);
DECLARE_DO_FUN(do_shadowwalk);
DECLARE_DO_FUN(do_share);
DECLARE_DO_FUN(do_sharpen);
DECLARE_DO_FUN(do_sheath);
DECLARE_DO_FUN(do_shield);
DECLARE_DO_FUN(do_shoot);
DECLARE_DO_FUN(do_shout);
DECLARE_DO_FUN(do_shutdow);
DECLARE_DO_FUN(do_shutdown);
DECLARE_DO_FUN(do_shroud);
DECLARE_DO_FUN(do_silence);
DECLARE_DO_FUN(do_sit);
DECLARE_DO_FUN(do_skill);
DECLARE_DO_FUN(do_sla);
DECLARE_DO_FUN(do_slay);
DECLARE_DO_FUN(do_sleep);
DECLARE_DO_FUN(do_slookup);
DECLARE_DO_FUN(do_smother);
DECLARE_DO_FUN(do_sneak);
DECLARE_DO_FUN(do_snoop);
DECLARE_DO_FUN(do_socials);
DECLARE_DO_FUN(do_south);
DECLARE_DO_FUN(do_speak);
DECLARE_DO_FUN(do_special);
DECLARE_DO_FUN(do_spell);
DECLARE_DO_FUN(do_spiderform);
DECLARE_DO_FUN(do_spiritgate);
DECLARE_DO_FUN(do_spiritguard);
DECLARE_DO_FUN(do_spit);
DECLARE_DO_FUN(do_split);
DECLARE_DO_FUN(do_spy);
DECLARE_DO_FUN(do_sset);
DECLARE_DO_FUN(do_stake);
DECLARE_DO_FUN(do_stance);
DECLARE_DO_FUN(do_stand);
DECLARE_DO_FUN(do_stat);
DECLARE_DO_FUN(do_steal);
DECLARE_DO_FUN(do_summon);
DECLARE_DO_FUN(do_superthing);
DECLARE_DO_FUN(do_survey);
DECLARE_DO_FUN(do_switch);
DECLARE_DO_FUN(do_swordtech);
DECLARE_DO_FUN(do_tail);
DECLARE_DO_FUN(do_talon);
DECLARE_DO_FUN(do_taste);
DECLARE_DO_FUN(do_tell);
DECLARE_DO_FUN(do_tcap);
DECLARE_DO_FUN(do_tear);
DECLARE_DO_FUN(do_tendrils);
DECLARE_DO_FUN(do_test);
DECLARE_DO_FUN(do_testarm);
DECLARE_DO_FUN(do_theft);
DECLARE_DO_FUN(do_throw);
DECLARE_DO_FUN(do_tide);
DECLARE_DO_FUN(do_tie);
DECLARE_DO_FUN(do_time);
DECLARE_DO_FUN(do_title);
DECLARE_DO_FUN(do_token);
DECLARE_DO_FUN(do_tongue);
DECLARE_DO_FUN(do_tournament);
DECLARE_DO_FUN(do_totems);
DECLARE_DO_FUN(do_touch);
DECLARE_DO_FUN(do_track);
DECLARE_DO_FUN(do_train);
DECLARE_DO_FUN(do_transfer);
DECLARE_DO_FUN(do_transport);
DECLARE_DO_FUN(do_travel);
DECLARE_DO_FUN(do_tribe);
DECLARE_DO_FUN(do_trivia);
DECLARE_DO_FUN(do_truesight);
DECLARE_DO_FUN(do_trust);
DECLARE_DO_FUN(do_tsume);
DECLARE_DO_FUN(do_turn);
DECLARE_DO_FUN(do_typo);
DECLARE_DO_FUN(do_twist);
DECLARE_DO_FUN(do_unclass);
DECLARE_DO_FUN(do_undeny);
DECLARE_DO_FUN(do_unload);
DECLARE_DO_FUN(do_unlock);
DECLARE_DO_FUN(do_unnerve);
DECLARE_DO_FUN(do_unpolymorph);
DECLARE_DO_FUN(do_untie);
DECLARE_DO_FUN(do_unveil);
DECLARE_DO_FUN(do_up);
DECLARE_DO_FUN(do_upkeep);
DECLARE_DO_FUN(do_uptime);
DECLARE_DO_FUN(do_users);
DECLARE_DO_FUN(do_value);
DECLARE_DO_FUN(do_vampire);
DECLARE_DO_FUN(do_vampvanish);
DECLARE_DO_FUN(do_vanish);
DECLARE_DO_FUN(do_vclan);
DECLARE_DO_FUN(do_visible);
DECLARE_DO_FUN(do_vision);
DECLARE_DO_FUN(do_vlist);
DECLARE_DO_FUN(do_voodoo);
DECLARE_DO_FUN(do_vote);
DECLARE_DO_FUN(do_wake);
DECLARE_DO_FUN(do_watcher);
DECLARE_DO_FUN(do_weaponform);
DECLARE_DO_FUN(do_weather);
DECLARE_DO_FUN(do_wear);
DECLARE_DO_FUN(do_web);
DECLARE_DO_FUN(do_west);
DECLARE_DO_FUN(do_wfreeze);
DECLARE_DO_FUN(do_where);
DECLARE_DO_FUN(do_whisper);
DECLARE_DO_FUN(do_who);
DECLARE_DO_FUN(do_wimpy);
DECLARE_DO_FUN(do_wings);
DECLARE_DO_FUN(do_wizhelp);
DECLARE_DO_FUN(do_wizlist);
DECLARE_DO_FUN(do_wizlock);
DECLARE_DO_FUN(do_xemote);
DECLARE_DO_FUN(do_xsocials);
DECLARE_DO_FUN(do_yell);
DECLARE_DO_FUN(do_zap);
DECLARE_DO_FUN(do_zombie);
DECLARE_DO_FUN(do_zuloform);

// NON PC COMMANDS
DECLARE_DO_FUN(do_autosave);
DECLARE_DO_FUN(do_beastlike);
DECLARE_DO_FUN(do_bloodline);
DECLARE_DO_FUN(do_changelight);
DECLARE_DO_FUN(do_humanity);
DECLARE_DO_FUN(do_info);
DECLARE_DO_FUN(do_killperson);
DECLARE_DO_FUN(do_mortalvamp);
DECLARE_DO_FUN(do_spydirection);
DECLARE_DO_FUN(do_tradition);
DECLARE_DO_FUN(do_unwerewolf);
DECLARE_DO_FUN(do_watching);
DECLARE_DO_FUN(do_werewolf);
DECLARE_DO_FUN(do_withering);

/*
 * Spell functions.
 */
DECLARE_SPELL_FUN(spell_null);
DECLARE_SPELL_FUN(spell_acid_blast);
DECLARE_SPELL_FUN(spell_armor);
DECLARE_SPELL_FUN(spell_bless);
DECLARE_SPELL_FUN(spell_blindness);
DECLARE_SPELL_FUN(spell_burning_hands);
DECLARE_SPELL_FUN(spell_call_lightning);
DECLARE_SPELL_FUN(spell_cause_critical);
DECLARE_SPELL_FUN(spell_cause_light);
DECLARE_SPELL_FUN(spell_cause_serious);
DECLARE_SPELL_FUN(spell_change_sex);
DECLARE_SPELL_FUN(spell_charm_person);
DECLARE_SPELL_FUN(spell_chill_touch);
DECLARE_SPELL_FUN(spell_colour_spray);
DECLARE_SPELL_FUN(spell_continual_light);
DECLARE_SPELL_FUN(spell_control_weather);
DECLARE_SPELL_FUN(spell_create_food);
DECLARE_SPELL_FUN(spell_create_spring);
DECLARE_SPELL_FUN(spell_create_water);
DECLARE_SPELL_FUN(spell_cure_blindness);
DECLARE_SPELL_FUN(spell_cure_critical);
DECLARE_SPELL_FUN(spell_cure_light);
DECLARE_SPELL_FUN(spell_cure_poison);
DECLARE_SPELL_FUN(spell_cure_serious);
DECLARE_SPELL_FUN(spell_curse);
DECLARE_SPELL_FUN(spell_darkness);
DECLARE_SPELL_FUN(spell_detect_evil);
DECLARE_SPELL_FUN(spell_detect_hidden);
DECLARE_SPELL_FUN(spell_detect_invis);
DECLARE_SPELL_FUN(spell_detect_magic);
DECLARE_SPELL_FUN(spell_detect_poison);
DECLARE_SPELL_FUN(spell_dispel_evil);
DECLARE_SPELL_FUN(spell_dispel_magic);
DECLARE_SPELL_FUN(spell_drowfire);
DECLARE_SPELL_FUN(spell_earthquake);
DECLARE_SPELL_FUN(spell_enchant_weapon);
DECLARE_SPELL_FUN(spell_energy_drain);
DECLARE_SPELL_FUN(spell_faerie_fire);
DECLARE_SPELL_FUN(spell_faerie_fog);
DECLARE_SPELL_FUN(spell_fireball);
DECLARE_SPELL_FUN(spell_flamestrike);
DECLARE_SPELL_FUN(spell_fly);
DECLARE_SPELL_FUN(spell_gate);
DECLARE_SPELL_FUN(spell_general_purpose);
DECLARE_SPELL_FUN(spell_giant_strength);
DECLARE_SPELL_FUN(spell_godbless);
DECLARE_SPELL_FUN(spell_harm);
DECLARE_SPELL_FUN(spell_heal);
DECLARE_SPELL_FUN(spell_high_explosive);
DECLARE_SPELL_FUN(spell_identify);
DECLARE_SPELL_FUN(spell_infravision);
DECLARE_SPELL_FUN(spell_invis);
DECLARE_SPELL_FUN(spell_know_alignment);
DECLARE_SPELL_FUN(spell_llothbless);
DECLARE_SPELL_FUN(spell_lightning_bolt);
DECLARE_SPELL_FUN(spell_locate_object);
DECLARE_SPELL_FUN(spell_magic_missile);
DECLARE_SPELL_FUN(spell_mass_invis);
DECLARE_SPELL_FUN(spell_pass_door);
DECLARE_SPELL_FUN(spell_poison);
DECLARE_SPELL_FUN(spell_protection);
DECLARE_SPELL_FUN(spell_refresh);
DECLARE_SPELL_FUN(spell_remove_curse);
DECLARE_SPELL_FUN(spell_sanctuary);
DECLARE_SPELL_FUN(spell_shocking_grasp);
DECLARE_SPELL_FUN(spell_shield);
DECLARE_SPELL_FUN(spell_sleep);
DECLARE_SPELL_FUN(spell_stone_skin);
DECLARE_SPELL_FUN(spell_summon);
DECLARE_SPELL_FUN(spell_teleport);
DECLARE_SPELL_FUN(spell_ventriloquate);
DECLARE_SPELL_FUN(spell_weaken);
DECLARE_SPELL_FUN(spell_word_of_recall);
DECLARE_SPELL_FUN(spell_acid_breath);
DECLARE_SPELL_FUN(spell_fire_breath);
DECLARE_SPELL_FUN(spell_frost_breath);
DECLARE_SPELL_FUN(spell_gas_breath);
DECLARE_SPELL_FUN(spell_lightning_breath);

DECLARE_SPELL_FUN(spell_guardian);
DECLARE_SPELL_FUN(spell_soulblade);
DECLARE_SPELL_FUN(spell_mana);
DECLARE_SPELL_FUN(spell_frenzy);
DECLARE_SPELL_FUN(spell_darkblessing);
DECLARE_SPELL_FUN(spell_portal);
DECLARE_SPELL_FUN(spell_energyflux);
DECLARE_SPELL_FUN(spell_voodoo);
DECLARE_SPELL_FUN(spell_transport);
DECLARE_SPELL_FUN(spell_regenerate);
DECLARE_SPELL_FUN(spell_clot);
DECLARE_SPELL_FUN(spell_mend);
DECLARE_SPELL_FUN(spell_quest);
DECLARE_SPELL_FUN(spell_minor_creation);
DECLARE_SPELL_FUN(spell_brew);
DECLARE_SPELL_FUN(spell_scribe);
DECLARE_SPELL_FUN(spell_carve);
DECLARE_SPELL_FUN(spell_engrave);
DECLARE_SPELL_FUN(spell_bake);
DECLARE_SPELL_FUN(spell_mount);
DECLARE_SPELL_FUN(spell_scan);
DECLARE_SPELL_FUN(spell_repair);
DECLARE_SPELL_FUN(spell_spellproof);
DECLARE_SPELL_FUN(spell_preserve);
DECLARE_SPELL_FUN(spell_major_creation);
DECLARE_SPELL_FUN(spell_copy);
DECLARE_SPELL_FUN(spell_insert_page);
DECLARE_SPELL_FUN(spell_chaos_blast);
DECLARE_SPELL_FUN(spell_resistance);
DECLARE_SPELL_FUN(spell_web);
DECLARE_SPELL_FUN(spell_polymorph);
DECLARE_SPELL_FUN(spell_contraception);
DECLARE_SPELL_FUN(spell_remove_page);
DECLARE_SPELL_FUN(spell_find_familiar);
DECLARE_SPELL_FUN(spell_improve);
DECLARE_SPELL_FUN(spell_enchant_armor);

/*
 * OS-dependent declarations.
 * These are all very standard library functions,
 *   but some systems have incomplete or non-ansi header files.
 */


/*
 * Data files used by the server.
 *
 * AREA_LIST contains a list of areas to boot.
 * All files are read in completely at bootup.
 * Most output files (bug, idea, typo, shutdown) are append-only.
 *
 * The NULL_FILE is held open so that we have a stream handle in reserve,
 *   so players can go ahead and telnet to all the other descriptors.
 * Then we close it whenever we need to open a file (e.g. a save file).
 */
#define PLAYER_DIR	"../player/"	/* Player files                 */
#ifndef WIN32
#define NULL_FILE	"/dev/null"	/* To reserve one stream        */
#else
#define NULL_FILE	"../null_file_do_not_touch"
#endif
#define AREA_LIST	"area.lst"	/* List of areas                */
#define CLAN_LIST	"clan1.txt"	/* List of clans                */
#define ART_LIST	"art1.txt"	/* List of artifacts            */
#define CHAT_FILE	"chats.txt"	// CHATS!@#*&
#define HOME_AREA	"../area/homes.are"	/* Mages towers, etc        */

#define BACKUP_DIR	"../player/backupinfo/"

#define BUG_FILE	"bugs.txt"	/* For 'bug' and bug( )         */
#define IDEA_FILE	"ideas.txt"	/* For 'idea'                   */
#define TYPO_FILE	"typos.txt"	/* For 'typo'                   */
#define NOTE_FILE	"notes.txt"	/* For 'notes'                  */
#define SHUTDOWN_FILE	"shutdown.txt"	/* For 'shutdown'               */



/*
 * Our function prototypes.
 * One big lump ... this is every function in Merc.
 */
#define CD	CHAR_DATA
#define MID	MOB_INDEX_DATA
#define OD	OBJ_DATA
#define OID	OBJ_INDEX_DATA
#define RID	ROOM_INDEX_DATA
#define SF	SPEC_FUN

/* act_comm.c */
void code_update args((char *arg));
void token_update args((char *arg));
void insult_update args((char *arg));
void add_follower args((CHAR_DATA * ch, CHAR_DATA * master));
void stop_follower args((CHAR_DATA * ch));
void die_follower args((CHAR_DATA * ch));
bool is_same_group args((CHAR_DATA * ach, CHAR_DATA * bch));
void room_text args((CHAR_DATA * ch, char *argument));
char *strlower args((char *ip));
void excessive_cpu args((long blx));
bool check_parse_name args((char *name));
long html_colour args((char type, char *string));
void html_colourconv args((char *buffer, const char *txt, CHAR_DATA * ch));
char *hacklang args((CHAR_DATA * ch, char *argument));
OD* random_token args((char *type));
OD* random_superthing args((int num));

/* arena.c

void    clean_arena	args( ( CHAR_DATA *loser, CHAR_DATA *winner ) );
bool    is_inarena	args( ( CHAR_DATA *ch ) );
*/

/* act_info.c */
bool check_empty_file args((FILE * fp));
char *uptime args((void));
double get_uptime args((void));
void set_title args((CHAR_DATA * ch, char *title));
void stc args((const char *txt, CHAR_DATA * ch));
void cent_to_char args((char *txt, CHAR_DATA * ch));
void banner_to_char args((char *txt, CHAR_DATA * ch));
void banner2_to_char args((char *txt, CHAR_DATA * ch));
void divide_to_char args((CHAR_DATA * ch));
void divide2_to_char args((CHAR_DATA * ch));
void divide3_to_char args((CHAR_DATA * ch));
void divide4_to_char args((CHAR_DATA * ch));
void divide5_to_char args((CHAR_DATA * ch));
void divide6_to_char args((CHAR_DATA * ch));
void show_list_to_char args((OBJ_DATA * list, CHAR_DATA * ch, bool fShort, bool fShowNothing));
long char_hitroll args((CHAR_DATA * ch));
long char_damroll args((CHAR_DATA * ch));
long char_ac args((CHAR_DATA * ch));
long player_rating args((CHAR_DATA * ch));
char *class_name args((long class));

/* act_move.c */
void move_char args((CHAR_DATA * ch, long door));
void open_lift args((CHAR_DATA * ch));
void close_lift args((CHAR_DATA * ch));
void move_lift args((CHAR_DATA * ch, long to_room));
void move_door args((CHAR_DATA * ch));
void thru_door args((CHAR_DATA * ch, long doorexit));
void open_door args((CHAR_DATA * ch, bool be_open));
bool is_open args((CHAR_DATA * ch));
bool same_floor args((CHAR_DATA * ch, long cmp_room));
void check_hunt args((CHAR_DATA * ch));

/* act_obj.c */
bool has_arm args((CHAR_DATA * ch, long arm));
bool is_ok_to_wear args((CHAR_DATA * ch, bool wolf_ok, char *argument));
void quest_object args((CHAR_DATA * ch, OBJ_DATA * obj));
bool remove_obj args((CHAR_DATA * ch, long iWear, bool fReplace));
void wear_obj args((CHAR_DATA * ch, OBJ_DATA * obj, bool fReplace));

/* act_wiz.c */
void bind_char args((CHAR_DATA * ch));
void logchan args((char *argument));
void godgift args((CHAR_DATA * victim));
void godcurse args((CHAR_DATA * victim));
char *colorize args((char *str));
char *hackify args((CHAR_DATA * ch, char *str));
char *fixify args((CHAR_DATA * ch, char *str));

/* comm.c */
void close_socket args((DESCRIPTOR_DATA * dclose));
void close_socket2 args((DESCRIPTOR_DATA * dclose, bool kickoff));
void write_to_buffer args((DESCRIPTOR_DATA * d, const char *txt, long length));
void send_to_char args((const char *txt, CHAR_DATA * ch));
void act args((const char *format, CHAR_DATA * ch, const void *arg1, const void *arg2, long type));
void act2 args((const char *format, CHAR_DATA * ch, const void *arg1, const void *arg2, long type));
void kavitem args((const char *format, CHAR_DATA * ch, const void *arg1, const void *arg2, long type));
void init_descriptor args((DESCRIPTOR_DATA * dnew, long desc));
bool write_to_descriptor args((long desc, char *txt, long length));
char randcolor args((CHAR_DATA *ch));

// Color stuff
long colour args((char type, CHAR_DATA * ch, char *string));
void colourconv args((char *buffer, const char *txt, CHAR_DATA * ch));

#define DISABLED_FILE "disabled.txt"	/* disabled commands */

/* prototypes from db.c */
void load_disabled args((void));
void save_disabled args((void));


/* db.c */
bool is_relic args((int vnum));
bool is_artifact args((int vnum));
void reverse_append args((const char *filename, const char *txt));
void boot_db args((bool fhotreboot));
void area_update args((void));
CD *create_mobile args((MOB_INDEX_DATA * pMobIndex));
OD *create_object args((OBJ_INDEX_DATA * pObjIndex, long level));
void clear_char args((CHAR_DATA * ch));
void free_char args((CHAR_DATA * ch));
char *get_extra_descr args((const char *name, EXTRA_DESCR_DATA * ed));
char *get_roomtext args((const char *name, ROOMTEXT_DATA * rt));
MID *get_mob_index args((long vnum));
OID *get_obj_index args((long vnum));
RID *get_room_index args((long vnum));
char fread_letter args((FILE * fp));
long fread_number args((FILE * fp));
char *fread_line args((FILE * fp));
char *fread_string args((FILE * fp));
void fread_to_eol args((FILE * fp));
char *fread_word args((FILE * fp));
void *alloc_mem args((long sMem, char *reason));
void *alloc_perm args((long sMem));
void free_mem args((void *pMem, long sMem));
char *str_dup args((const char *str));
void free_string args((char *pstr));
long number_fuzzy args((long number));
long number_range args((long from, long to));
long number_percent args((void));
long number_door args((void));
long number_bits args((long width));
long number_mm args((void));
long dice args((long number, long size));
long interpolate args((long level, long value_00, long value_32));
void smash_tilde args((char *str));
bool str_cmp args((const char *astr, const char *bstr));
bool str_prefix args((const char *astr, const char *bstr));
bool str_infix args((const char *astr, const char *bstr));
bool str_suffix args((const char *astr, const char *bstr));
char *capitalize args((const char *str));
void append_file args((CHAR_DATA * ch, char *file, char *str));
void bug args((const char *str, long param));
void log_string args((const char *str));
void tail_chain args((void));

/* fight.c */
void set_fighting args((CHAR_DATA * ch, CHAR_DATA * victim));
void update_damcap args((CHAR_DATA * ch, CHAR_DATA * victim));
void combat_affect args((CHAR_DATA * ch, long type, long duration, long modifier, long location, long bitvector));
void combat_aff_remove args((CHAR_DATA * ch, AFFECT_DATA * af));
void violence_update args((void));
void multi_hit args((CHAR_DATA * ch, CHAR_DATA * victim, long dt));
void damage args((CHAR_DATA * ch, CHAR_DATA * victim, long dam, long dt));
void adv_damage args((CHAR_DATA * ch, CHAR_DATA * victim, long dam));
void update_pos args((CHAR_DATA * victim));
void stop_fighting args((CHAR_DATA * ch, bool fBoth));
void stop_embrace args((CHAR_DATA * ch, CHAR_DATA * victim));
bool no_attack args((CHAR_DATA * ch, CHAR_DATA * victim));
bool is_safe args((CHAR_DATA * ch, CHAR_DATA * victim));
void hurt_person args((CHAR_DATA * ch, CHAR_DATA * victim, long dam));
bool has_timer args((CHAR_DATA * ch));
ASSIST_DATA *new_assist args((void));
ASSIST_DATA *add_assist args((CHAR_DATA * ch, CHAR_DATA * attacker));
void update_assist args((CHAR_DATA * ch, CHAR_DATA * attacker, long damage));
long find_assist_damage args((CHAR_DATA * ch, CHAR_DATA * attacker));
long find_total_assist_damage args((CHAR_DATA * ch));
long find_mob_assist_damage args((CHAR_DATA * ch));
void remove_assist args((CHAR_DATA * ch, CHAR_DATA * attacker));

/* gui.c */
void init_vt args((CHAR_DATA *ch));

/* handler.c */
long get_duration args((CHAR_DATA * ch, long gsn));
void make_duration args((CHAR_DATA * ch, long gsn, long time));
long get_trust args((CHAR_DATA * ch));
long get_age args((CHAR_DATA * ch));
long get_curr_str args((CHAR_DATA * ch));
long get_curr_int args((CHAR_DATA * ch));
long get_curr_wis args((CHAR_DATA * ch));
long get_curr_dex args((CHAR_DATA * ch));
long get_curr_con args((CHAR_DATA * ch));
long can_carry_n args((CHAR_DATA * ch));
long can_carry_w args((CHAR_DATA * ch));
bool is_name args((const char *str, char *namelist));
void affect_to_char args((CHAR_DATA * ch, AFFECT_DATA * paf));
void affect_remove args((CHAR_DATA * ch, AFFECT_DATA * paf));
void affect_strip args((CHAR_DATA * ch, long sn));
bool is_affected args((CHAR_DATA * ch, long sn));
void affect_join args((CHAR_DATA * ch, AFFECT_DATA * paf));
void char_from_room args((CHAR_DATA * ch));
void char_to_room args((CHAR_DATA * ch, ROOM_INDEX_DATA * pRoomIndex));
void obj_to_char args((OBJ_DATA * obj, CHAR_DATA * ch));
void obj_from_char args((OBJ_DATA * obj));
long apply_ac args((OBJ_DATA * obj, long iWear));
OD *get_eq_char args((CHAR_DATA * ch, long iWear));
void equip_char args((CHAR_DATA * ch, OBJ_DATA * obj, long iWear));
void unequip_char args((CHAR_DATA * ch, OBJ_DATA * obj));
long count_obj_list args((OBJ_INDEX_DATA * obj, OBJ_DATA * list));
void obj_from_room args((OBJ_DATA * obj));
void obj_to_room args((OBJ_DATA * obj, ROOM_INDEX_DATA * pRoomIndex));
void obj_to_obj args((OBJ_DATA * obj, OBJ_DATA * obj_to));
void obj_from_obj args((OBJ_DATA * obj));
void extract_obj args((OBJ_DATA * obj));
void extract_char args((CHAR_DATA * ch, bool fPull));
CD *get_char_room args((CHAR_DATA * ch, char *argument));
CD *get_char_world args((CHAR_DATA * ch, char *argument));
CD *get_char_world2 args((CHAR_DATA * ch, char *argument));
OD *get_obj_type args((OBJ_INDEX_DATA * pObjIndexData));
OD *get_obj_list args((CHAR_DATA * ch, char *argument, OBJ_DATA * list));
OD *get_obj_in_obj args((CHAR_DATA * ch, char *argument));
OD *get_obj_carry args((CHAR_DATA * ch, char *argument));
OD *get_obj_wear args((CHAR_DATA * ch, char *argument));
OD *get_obj_here args((CHAR_DATA * ch, char *argument));
OD *get_obj_room args((CHAR_DATA * ch, char *argument));
OD *get_obj_world args((CHAR_DATA * ch, char *argument));
OD *get_obj_world2 args((CHAR_DATA * ch, char *argument));
OD *create_money args((long amount));
long get_obj_number args((OBJ_DATA * obj));
long get_obj_weight args((OBJ_DATA * obj));
bool room_is_dark args((ROOM_INDEX_DATA * pRoomIndex));
bool room_is_private args((ROOM_INDEX_DATA * pRoomIndex));
bool can_see args((CHAR_DATA * ch, CHAR_DATA * victim));
bool can_see_obj args((CHAR_DATA * ch, OBJ_DATA * obj));
bool can_drop_obj args((CHAR_DATA * ch, OBJ_DATA * obj));
char *item_type_name args((OBJ_DATA * obj));
char *affect_loc_name args((long location));
char *affect_bit_name args((long vector));
char *extra_bit_name args((long extra_flags));
void affect_modify args((CHAR_DATA * ch, AFFECT_DATA * paf, bool fAdd));

/* hotreboot.c */
void hotreboot_recover args((void));
void prepare_players args((void));

/* interp.c */
void enable args((char *command));
void disable args((char *command));
void interpret args((CHAR_DATA * ch, char *argument));
bool is_number args((char *arg));
long number_argument args((char *argument, char *arg));
char *one_argument args((char *argument, char *arg_first));
char *one_argument_case args((char *argument, char *arg_first));
void stage_update args((CHAR_DATA * ch, CHAR_DATA * victim, long stage));

/* kingdom.c */
void kingdom_update args((void));
void kingdom_message args((long id, char *message, ...));
void read_kingdoms args((void));
void write_kingdoms args((void));
void update_char_kingdom args((CHAR_DATA * ch));
void update_kingdoms args((void));
long award_points args((CHAR_DATA * ch, CHAR_DATA * victim));
long ansistrlen args((char *arg));
bool is_at_war args((long id1, long id2));
KINGDOM_DATA *get_kingdom args((long id));


/* magic.c */
long skill_lookup args((const char *name));
long slot_lookup args((long slot));
bool saves_spell args((long level, CHAR_DATA * victim));
void obj_cast_spell args((long sn, long level, CHAR_DATA * ch, CHAR_DATA * victim, OBJ_DATA * obj));
void enhance_stat args((long sn, long level, CHAR_DATA * ch, CHAR_DATA * victim, long apply_bit, long bonuses, long affect_bit));

/* map.c */
void draw_map args((CHAR_DATA * ch));

/* os.c */
char *crypt args((const char *pw, const char *salt));
void get_time args((struct timeval * time));

/* ping.c */
void ping_players args((void));
void get_player_pings args((void));

/* save.c */
void save_char_obj args((CHAR_DATA * ch));
void save_char_obj_backup args((CHAR_DATA * ch));
bool check_pfile args((char *name));
long load_char_obj args((DESCRIPTOR_DATA * d, char *name));
bool load_char_short args((DESCRIPTOR_DATA * d, char *name));

/* specials.c */
long update_specs args((ROOM_INDEX_DATA * room, long what, long type));

/* swordtech.c */
void swordtech args((CHAR_DATA * ch, long bit));

/* tournament.c */
#define MAX_TOURNAMENT_VNUM 60
#define MIN_TOURNAMENT_VNUM 9
void tournament_update args((void));
void tournament_kill args((CHAR_DATA * ch, CHAR_DATA * victim));
void battle_log args((CHAR_DATA * ch, CHAR_DATA * victim, char *buf));
void tournament_check args((CHAR_DATA * ch));
bool in_tournament args((CHAR_DATA * ch, CHAR_DATA * victim));
bool tourn_on args((void));
bool tourn_started args((void));

/* update.c */
void gain_exp args((CHAR_DATA * ch, long gain));
void gain_condition args((CHAR_DATA * ch, long iCond, long value));
void update_handler args((void));
void limbo_update args((void));

/* kav_fight.c */
void special_move args((CHAR_DATA * ch, CHAR_DATA * victim));

/* kav_info.c */
void birth_date args((CHAR_DATA * ch, bool is_self));
void other_age args((CHAR_DATA * ch, long extra, bool is_preg, char *argument));
long years_old args((CHAR_DATA * ch));
void centre_text args((char *text, CHAR_DATA * ch));
void line2 args((CHAR_DATA * ch));
void line args((CHAR_DATA * ch));

/* kav_wiz.c */
void oset_affect args((CHAR_DATA * ch, OBJ_DATA * obj, long value, long affect, bool is_quest));

/* clan.c */
void werewolf_regen args((CHAR_DATA * ch));
void reg_mend args((CHAR_DATA * ch));
void vamp_rage args((CHAR_DATA * ch));
bool char_exists args((bool backup, char *argument));
OD *get_page args((OBJ_DATA * book, long page_num));

/* crash.c */
#define WRITE_STR(field, value); free_string(field); field = str_dup( value );
#define SS(stage)               debug_stage = (stage)
void crash args((char *arg));
void init_signals args((void));

/* clan_sav.c */
void save_clans args((void));
void load_clans args((void));
void init_clans args((void));
void update_clans args((CHAR_DATA * ch, CHAR_DATA * victim));
void update_leaders args((CHAR_DATA * ch));
void init_leaders args((void));

/* vic.c */
void reset_weapon args((CHAR_DATA * ch, long dtype));
void reset_spell args((CHAR_DATA * ch, long dtype));

#undef	CD
#undef	MID
#undef	OD
#undef	OID
#undef	RID
#undef	SF

#define BOLD "#7"
#define NO_COLOR "#n"
#define GREEN "#2"
#define RED "#1"
#define BLUE "#4"

// THIS IS THE END OF THE FILE THANKS
