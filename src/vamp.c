#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

void set_fighting args((CHAR_DATA * ch, CHAR_DATA * victim));

void do_purification(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	long hps = ch->max_hit / 4;

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE) || ch->pcdata->powers[VAM_OBEA] < 4)
	{
		stc("Only the pure in heart can use purification!\n\r", ch);
		return;
	}
	if(ch->fight_timer > 0)
	{
		stc("You have to be totally calm to purify yourself!\n\r", ch);
		return;
	}
	if(IS_POLYAFF(ch, POLY_ZULO))
	{
		stc("Only the pure in heart can use purification!\n\r", ch);
		return;
	}
	if(IS_AFFECTED(ch, AFF_POLYMORPH))
	{
		stc("Only the pure in heart can use purification!\n\r", ch);
		return;
	}
	if(ch->move < 500)
	{
		stc("You are too exhausted to purify your mind\n\r", ch);
		return;
	}

	act("A bright halo glows above $n's head.", ch, 0, 0, TO_ROOM);
	send_to_char("You purify your mind.\n\r", ch);

	WAIT_STATE(ch, 30 - ch->pcdata->powers[VAM_OBEA]);
	ch->move -= 500;
	ch->hit += hps;

	if(ch->hit > ch->max_hit)
		ch->hit = ch->max_hit;
	return;
}

void animalism_disc(CHAR_DATA * ch)
{
	if(!ch)
		return;
	if(ch && IS_NPC(ch))
		return;

	if(ch->pcdata->powers[VAM_ANIM] == 0)
	{
		stc("      Animalism: None.\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_ANIM] >= 1)
		stc("      Animalism:  Beckon", ch);
	if(ch->pcdata->powers[VAM_ANIM] >= 2)
		stc(" Serenity", ch);
	if(ch->pcdata->powers[VAM_ANIM] >= 3)
		stc(" Pigeon", ch);
	if(ch->pcdata->powers[VAM_ANIM] >= 4)
		stc(" Share", ch);
	if(ch->pcdata->powers[VAM_ANIM] >= 5)
		stc(" Frenzy", ch);
	stc("\n\r", ch);
	return;
}

void obtenebration_disc(CHAR_DATA * ch)
{
	if(!ch)
		return;
	if(ch && IS_NPC(ch))
		return;

	if(ch->pcdata->powers[VAM_OBTE] == 0)
	{
		stc("  Obtenebration: None.\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_OBTE] >= 1)
		stc("  Obtenebration:  Shroud", ch);
	if(ch->pcdata->powers[VAM_OBTE] >= 2)
		stc(" ShadowSight", ch);
	if(ch->pcdata->powers[VAM_OBTE] >= 3)
		stc(" NightSight", ch);
	if(ch->pcdata->powers[VAM_OBTE] >= 4)
		stc(" Shadowstep", ch);
	if(ch->pcdata->powers[VAM_OBTE] >= 5)
		stc(" Lamprey", ch);
	stc("\n\r", ch);
	return;
}

void thaumaturgy_disc(CHAR_DATA * ch)
{
	if(!ch)
		return;
	if(ch && IS_NPC(ch))
		return;

	if(ch->pcdata->powers[VAM_THAU] == 0)
	{
		stc("    Thaumaturgy: None.\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_THAU] >= 1)
		stc("    Thaumaturgy:  Taste", ch);
	if(ch->pcdata->powers[VAM_THAU] >= 2)
		stc(" Cauldron", ch);
	if(ch->pcdata->powers[VAM_THAU] >= 3)
		stc(" Potency", ch);
	if(ch->pcdata->powers[VAM_THAU] >= 4)
		stc(" Theft", ch);
	if(ch->pcdata->powers[VAM_THAU] >= 5)
		stc(" Tide", ch);
	stc("\n\r", ch);
	return;
}

void obfuscate_disc(CHAR_DATA * ch)
{
	if(!ch)
		return;
	if(ch && IS_NPC(ch))
		return;

	if(ch->pcdata->powers[VAM_OBFU] == 0)
	{
		stc("      Obfuscate: None.\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_OBFU] >= 1)
		stc("      Obfuscate:  Vampvanish", ch);
	if(ch->pcdata->powers[VAM_OBFU] >= 2)
		stc(" Shield", ch);
	if(ch->pcdata->powers[VAM_OBFU] >= 3)
		stc(" Mask", ch);
	if(ch->pcdata->powers[VAM_OBFU] >= 4)
		stc(" Mortal", ch);
	if(ch->pcdata->powers[VAM_OBFU] >= 5)
		stc(" Conceal", ch);
	stc("\n\r", ch);
	return;
}


void serpentis_disc(CHAR_DATA * ch)
{
	if(!ch)
		return;
	if(IS_NPC(ch))
		return;

	if(ch->pcdata->powers[VAM_SERP] == 0)
	{
		stc("      Serpentis: None.\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_SERP] >= 1)
		stc("      Serpentis:  Darkheart", ch);
	if(ch->pcdata->powers[VAM_SERP] >= 2)
		stc(" SerpentForm", ch);
	if(ch->pcdata->powers[VAM_SERP] >= 3)
		stc(" Poison", ch);
	if(ch->pcdata->powers[VAM_SERP] >= 4)
		stc(" Tendrils", ch);
	if(ch->pcdata->powers[VAM_SERP] >= 5)
		stc(" Tongue", ch);
	stc("\n\r", ch);
	return;
}



void chimerstry_disc(CHAR_DATA * ch)
{
	if(!ch)
		return;
	if(IS_NPC(ch))
		return;

	if(ch->pcdata->powers[VAM_CHIM] == 0)
	{
		stc("     Chimerstry: None.\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_CHIM] >= 1)
		stc("     Chimerstry:  Mirror", ch);
	if(ch->pcdata->powers[VAM_CHIM] >= 2)
		stc(" Formillusion", ch);
	if(ch->pcdata->powers[VAM_CHIM] >= 3)
		stc(" Clone", ch);
	if(ch->pcdata->powers[VAM_CHIM] >= 4)
		stc(" Control", ch);
	if(ch->pcdata->powers[VAM_CHIM] >= 5)
		stc(" Objectmask", ch);
	stc("\n\r", ch);
	return;
}




void necromancy_disc(CHAR_DATA * ch)
{
	if(!ch)
		return;
	if(IS_NPC(ch))
		return;

	if(ch->pcdata->powers[VAM_NECR] == 0)
	{
		stc("     Necromancy: None.\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_NECR] >= 1)
		stc("     Necromancy:  BloodWater", ch);
	if(ch->pcdata->powers[VAM_NECR] >= 2)
		stc(" Preserve", ch);
	if(ch->pcdata->powers[VAM_NECR] >= 3)
		stc(" Spiritgate", ch);
	if(ch->pcdata->powers[VAM_NECR] >= 4)
		stc(" SpiritGuardian", ch);
	if(ch->pcdata->powers[VAM_NECR] >= 5)
		stc(" Zombie", ch);
	stc("\n\r", ch);
	return;
}



void celerity_disc(CHAR_DATA * ch)
{
	if(!ch)
		return;
	if(IS_NPC(ch))
		return;

	if(ch->pcdata->powers[VAM_CELE] == 0)
	{
		stc("       Celerity: None.\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_CELE] >= 1)
		stc("       Celerity:  1", ch);
	if(ch->pcdata->powers[VAM_CELE] >= 2)
		stc(" 2", ch);
	if(ch->pcdata->powers[VAM_CELE] >= 3)
		stc(" 3", ch);
	if(ch->pcdata->powers[VAM_CELE] >= 4)
		stc(" 4", ch);
	if(ch->pcdata->powers[VAM_CELE] >= 5)
		stc(" 5", ch);
	stc("\n\r", ch);
	return;
}


void presence_disc(CHAR_DATA * ch)
{
	if(!ch)
		return;
	if(IS_NPC(ch))
		return;

	if(ch->pcdata->powers[VAM_PRES] == 0)
	{
		stc("       Presence: None.\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_PRES] >= 1)
		stc("       Presence:  Awe", ch);
	if(ch->pcdata->powers[VAM_PRES] >= 2)
		stc(" Mindblast", ch);
	if(ch->pcdata->powers[VAM_PRES] >= 3)
		stc(" Entrance", ch);
	if(ch->pcdata->powers[VAM_PRES] >= 4)
		stc(" Summon", ch);
	if(ch->pcdata->powers[VAM_PRES] >= 5)
		stc(" Majesty", ch);
	stc("\n\r", ch);
	return;
}



void auspex_disc(CHAR_DATA * ch)
{
	if(!ch)
		return;
	if(IS_NPC(ch))
		return;

	if(ch->pcdata->powers[VAM_AUSP] == 0)
	{
		stc("         Auspex: None.\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_AUSP] >= 1)
		stc("         Auspex:  Truesight", ch);
	if(ch->pcdata->powers[VAM_AUSP] >= 2)
		stc(" Scry", ch);
	if(ch->pcdata->powers[VAM_AUSP] >= 3)
		stc(" Unveil", ch);
	if(ch->pcdata->powers[VAM_AUSP] >= 4)
		stc(" Astral Walk", ch);
	if(ch->pcdata->powers[VAM_AUSP] >= 5)
		stc(" Readaura", ch);
	stc("\n\r", ch);
	return;
}


void potence_disc(CHAR_DATA * ch)
{
	if(!ch)
		return;
	if(IS_NPC(ch))
		return;

	if(ch->pcdata->powers[VAM_POTE] == 0)
	{
		stc("        Potence: None.\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_POTE] >= 1)
		stc("        Potence:  1", ch);
	if(ch->pcdata->powers[VAM_POTE] >= 2)
		stc(" 2", ch);
	if(ch->pcdata->powers[VAM_POTE] >= 3)
		stc(" 3", ch);
	if(ch->pcdata->powers[VAM_POTE] >= 4)
		stc(" 4", ch);
	if(ch->pcdata->powers[VAM_POTE] >= 5)
		stc(" 5", ch);
	stc("\n\r", ch);
	return;
}



void vicissitude_disc(CHAR_DATA * ch)
{
	if(!ch)
		return;
	if(IS_NPC(ch))
		return;

	if(ch->pcdata->powers[VAM_VICI] == 0)
	{
		stc("    Vicissitude: None.\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_VICI] >= 1)
		stc("    Vicissitude:  Zuloform", ch);
	if(ch->pcdata->powers[VAM_VICI] >= 2)
		stc(" Fleshcraft", ch);
	if(ch->pcdata->powers[VAM_VICI] >= 3)
		stc(" Bonemod", ch);
	if(ch->pcdata->powers[VAM_VICI] >= 4)
		stc(" DragonForm", ch);
	if(ch->pcdata->powers[VAM_VICI] >= 5)
		stc(" Plasma", ch);
	stc("\n\r", ch);
	return;
}


void obeah_disc(CHAR_DATA * ch)
{
	if(!ch)
		return;
	if(IS_NPC(ch))
		return;

	if(ch->pcdata->powers[VAM_OBEA] == 0)
	{
		stc("          Obeah: None.\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_OBEA] >= 1)
		stc("          Obeah:  not in", ch);
	if(ch->pcdata->powers[VAM_OBEA] >= 2)
		stc(" not in", ch);
	if(ch->pcdata->powers[VAM_OBEA] >= 3)
		stc(" not in", ch);
	if(ch->pcdata->powers[VAM_OBEA] >= 4)
		stc(" not in", ch);
	if(ch->pcdata->powers[VAM_OBEA] >= 5)
		stc(" not in", ch);
	stc("\n\r", ch);
	return;
}



void fortitude_disc(CHAR_DATA * ch)
{
	if(!ch)
		return;
	if(IS_NPC(ch))
		return;

	if(ch->pcdata->powers[VAM_FORT] == 0)
	{
		stc("      Fortitude: None.\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_FORT] >= 1)
		stc("      Fortitude:  1", ch);
	if(ch->pcdata->powers[VAM_FORT] >= 2)
		stc(" 2", ch);
	if(ch->pcdata->powers[VAM_FORT] >= 3)
		stc(" 3", ch);
	if(ch->pcdata->powers[VAM_FORT] >= 4)
		stc(" 4", ch);
	if(ch->pcdata->powers[VAM_FORT] >= 5)
		stc(" 5", ch);
	stc("\n\r", ch);
	return;
}



void quietus_disc(CHAR_DATA * ch)
{
	if(!ch)
		return;
	if(IS_NPC(ch))
		return;

	if(ch->pcdata->powers[VAM_QUIE] == 0)
	{
		stc("        Quietus: None.\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_QUIE] >= 1)
		stc("        Quietus:  Spit", ch);
	if(ch->pcdata->powers[VAM_QUIE] >= 2)
		stc(" Infirmity", ch);
	if(ch->pcdata->powers[VAM_QUIE] >= 3)
		stc(" Bloodagony", ch);
	if(ch->pcdata->powers[VAM_QUIE] >= 4)
		stc(" Assassinate", ch);
	if(ch->pcdata->powers[VAM_QUIE] >= 5)
		stc(" Vsilence", ch);
	stc("\n\r", ch);
	return;
}


void dominate_disc(CHAR_DATA * ch)
{
	if(!ch)
		return;
	if(IS_NPC(ch))
		return;

	if(ch->pcdata->powers[VAM_DOMI] == 0)
	{
		stc("       Dominate: None.\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_DOMI] >= 1)
		stc("       Dominate:  Command", ch);
	if(ch->pcdata->powers[VAM_DOMI] >= 2)
		stc(" Mesmerise", ch);
	if(ch->pcdata->powers[VAM_DOMI] >= 3)
		stc(" Possession", ch);
	if(ch->pcdata->powers[VAM_DOMI] >= 4)
		stc(" Acid", ch);
	if(ch->pcdata->powers[VAM_DOMI] >= 5)
		stc(" Coil", ch);
	stc("\n\r", ch);
	return;
}


void protean_disc(CHAR_DATA * ch)
{
	if(!ch)
		return;
	if(IS_NPC(ch))
		return;

	if(ch->pcdata->powers[VAM_PROT] == 0)
	{
		stc("        Protean: None.\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_PROT] >= 1)
		stc("        Protean:  Umbravision", ch);
	if(ch->pcdata->powers[VAM_PROT] >= 2)
		stc(" Claws", ch);
	if(ch->pcdata->powers[VAM_PROT] >= 3)
		stc(" Change", ch);
	if(ch->pcdata->powers[VAM_PROT] >= 4)
		stc(" Earthmeld", ch);
	if(ch->pcdata->powers[VAM_PROT] >= 5)
		stc(" Flamehands", ch);
	stc("\n\r", ch);
	return;
}


void daimoinon_disc(CHAR_DATA * ch)
{
	if(!ch)
		return;
	if(IS_NPC(ch))
		return;

	if(ch->pcdata->powers[VAM_DAIM] == 0)
	{
		stc("      Daimoinon: None.\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_DAIM] >= 1)
		stc("      Daimoinon:  Guardian", ch);
	if(ch->pcdata->powers[VAM_DAIM] >= 2)
		stc(" Blood Wall", ch);
	if(ch->pcdata->powers[VAM_DAIM] >= 3)
		stc(" Gate", ch);
/*  if ( ch->pcdata->powers[VAM_DAIM] >= 4 )
    stc(" Vtwist",ch); */
	if(ch->pcdata->powers[VAM_DAIM] >= 4)
		stc(" Fear", ch);
	stc("\n\r", ch);
	return;
}


void melpominee_disc(CHAR_DATA * ch)
{
	if(!ch)
		return;
	if(IS_NPC(ch))
		return;

	if(ch->pcdata->powers[VAM_MELP] == 0)
	{
		stc("     Melpominee: None.\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_MELP] >= 1)
		stc("     Melpominee:  Scream", ch);
	if(ch->pcdata->powers[VAM_MELP] >= 2)
		stc(" Gourge", ch);
	if(ch->pcdata->powers[VAM_MELP] >= 3)
		stc(" Sharpen", ch);
	if(ch->pcdata->powers[VAM_MELP] >= 4)
		stc(" not in", ch);
	if(ch->pcdata->powers[VAM_MELP] >= 5)
		stc(" not in", ch);
	stc("\n\r", ch);
	return;
}


void thanatosis_disc(CHAR_DATA * ch)
{
	if(!ch)
		return;
	if(IS_NPC(ch))
		return;

	if(ch->pcdata->powers[VAM_THAN] == 0)
	{
		stc("     Thanatosis: None.\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_THAN] >= 1)
		stc("     Thanatosis:  Hagswrinkles", ch);
	if(ch->pcdata->powers[VAM_THAN] >= 2)
		stc(" Putrefaction", ch);
	if(ch->pcdata->powers[VAM_THAN] >= 3)
		stc(" Ashes", ch);
	if(ch->pcdata->powers[VAM_THAN] >= 4)
		stc(" Wither", ch);
	if(ch->pcdata->powers[VAM_THAN] >= 5)
		stc(" Drain", ch);
	stc("\n\r", ch);
	return;
}

void do_vampire(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);

	if(!ch)
		return;
	if(IS_NPC(ch))
		return;

	if(!IS_IMMUNE(ch, IMM_VAMPIRE))
	{
		send_to_char("You will now allow vampires to bite you.\n\r", ch);
		SET_BIT(ch->immune, IMM_VAMPIRE);
		return;
	}
	send_to_char("You will no longer allow vampires to bite you.\n\r", ch);
	REMOVE_BIT(ch->immune, IMM_VAMPIRE);
	return;
}


void do_racepower(CHAR_DATA * ch, char *argument)
{


	if(IS_NPC(ch))
		return;

	if(IS_CLASS(ch, CLASS_VAMPIRE))
	{
		stc("---------------------------==| Vampire Powers |==------------------------\n\r", ch);
		animalism_disc(ch);
		obtenebration_disc(ch);
		thaumaturgy_disc(ch);
		obfuscate_disc(ch);
		serpentis_disc(ch);
		chimerstry_disc(ch);
		necromancy_disc(ch);
		celerity_disc(ch);
		presence_disc(ch);
		auspex_disc(ch);
		potence_disc(ch);
		vicissitude_disc(ch);
		obeah_disc(ch);
		fortitude_disc(ch);
		quietus_disc(ch);
		dominate_disc(ch);
		protean_disc(ch);
		daimoinon_disc(ch);
		melpominee_disc(ch);
		thanatosis_disc(ch);
		divide2_to_char(ch);
		return;
	}
	else
		return;

	return;
}

/*embrace*/
void do_embrace(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	char buf[MAX_STRING_LENGTH];
	long bloodpool;

	one_argument(argument, arg);

	if(IS_SET(ch->newbits, NEW_TIDE))
		bloodpool = (3000 / ch->pcdata->stats[UNI_GEN]);
	else
		bloodpool = (2000 / ch->pcdata->stats[UNI_GEN]);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}


	if(arg[0] == '\0')
	{
		send_to_char("Who do you wish to embrace?\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They arent here.\n\r", ch);
		return;
	}

	if(IS_NPC(victim) && IS_SET(victim->act, ACT_NOAUTOKILL))
	{
		send_to_char("You can't do that to them.\n\r", ch);
		return;
	}

	/* Shaktis check for level */

	if(victim->level > 75)
	{
		send_to_char("They are too powerful to embrace!\n\r", ch);
		return;
	}


	if(!IS_NPC(victim))
	{
		send_to_char("You cannot embrace a person.\n\r", ch);
		return;
	}

	if(IS_SET(victim->in_room->room_flags, ROOM_SAFE))
	{
		send_to_char("You cannot embrace them here.\n\r", ch);
		return;
	}

	sprintf(buf, "%s leaps toward %s baring his fangs.\n\r", ch->name, victim->short_descr);
	act(buf, ch, 0, 0, TO_ROOM);
	sprintf(buf, "You leap toward %s baring your fangs.\n\r", victim->short_descr);
	send_to_char(buf, ch);
	WAIT_STATE(ch, 15);

	if(victim->position != POS_STUNNED && victim->position != POS_SLEEPING && victim->position != POS_DEAD)
	{
		send_to_char("They lunge away from you.\n\r", ch);
		sprintf(buf, "%s lunges away from $n.", victim->name);
		act(buf, ch, 0, 0, TO_ROOM);
		return;
	}

	send_to_char("You bury your fangs into their neck, and shiver ecstatically as you drain their lifeblood.!\n\r", ch);
	sprintf(buf, "%s buries his fangs into %s's neck and begins drinking $s lifeblood.\n\r", ch->name,
		victim->short_descr);
	act(buf, ch, 0, 0, TO_ROOM);

	/* New update send routine, allows for anything to get embraced. */
	/* and sets a mobs blood to its level  Shakti */

	victim->embraced = ch;
	ch->embracing = victim;
	if(IS_NPC(victim))
	{
		(victim->practice = victim->level);
		(victim->practice *= 8);
	}

	return;


}

void do_theft(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	long bloodpool;
	long blpr;		/* Blood sucked storage variable. Shakti */

	argument = one_argument(argument, arg);

	{
		if(ch->pcdata->stats[UNI_GEN] == 0)
			ch->pcdata->stats[UNI_GEN] = 3;

		if(IS_SET(ch->newbits, NEW_TIDE))
			bloodpool = (3000 / ch->pcdata->stats[UNI_GEN]);
		else
			bloodpool = (2000 / ch->pcdata->stats[UNI_GEN]);
	}

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_THAU] < 4)
	{
		send_to_char("You must obtain at least level 4 in Thaumaturgy to use Theft of Vitae.\n\r", ch);
		return;
	}


	if(arg[0] == '\0')
	{
		send_to_char("Steal blood from whom?\n\r", ch);
		return;
	}

	if(IS_AFFECTED(ch, AFF_POLYMORPH) && !IS_VAMPAFF(ch, VAM_DISGUISED))
	{
		send_to_char("Not while polymorphed.\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(IS_NPC(victim) && IS_SET(victim->act, ACT_NOAUTOKILL))
	{
		send_to_char("You can't do that to them.\n\r", ch);
		return;
	}

	/* Shaktis check for level */

	if(victim->level > 75)
	{
		send_to_char("They are too powerful to theft!\n\r", ch);
		return;
	}

/*
    if ( IS_NPC(victim) )
    {
        send_to_char( "Lower life forms are immune to Theft of Vitae.\n\r", ch);
        return;
    }
*/
	if(!IS_NPC(victim) && victim->pcdata->condition[COND_THIRST] <= 0)
	{
		send_to_char("There isn't enough blood to steal.\n\r", ch);
		return;
	}
	if(!IS_NPC(victim) && IS_IMMORTAL(victim) && victim != ch)
	{
		send_to_char("You can only steal blood from Avatar's or lower.\n\r", ch);
		return;
	}
	if(is_safe(ch, victim) == TRUE)
		return;

	if(!IS_NPC(victim))
	{
		sprintf(buf, "A stream of blood shoots from %s into your body.", victim->name);
		act(buf, ch, 0, victim, TO_CHAR);
		sprintf(buf, "A stream of blood shoots from %s into %s.", victim->name, ch->name);
		act(buf, ch, 0, victim, TO_ROOM);
		sprintf(buf, "A stream of blood shoots from your body into %s.", ch->name);
		act(buf, ch, 0, victim, TO_VICT);
	}
	else
	{
		sprintf(buf, "A stream of blood shoots from %s into your body.", victim->short_descr);
		act(buf, ch, 0, victim, TO_CHAR);
		sprintf(buf, "A stream of blood shoots from %s into %s.", victim->short_descr, ch->name);
		act(buf, ch, 0, victim, TO_ROOM);
		sprintf(buf, "A stream of blood shoots from your body into %s.", ch->name);
		act(buf, ch, 0, victim, TO_VICT);


	}
	ch->pcdata->condition[COND_THIRST] += number_range(10, 15);


	if(IS_NPC(victim))
	{
/* Raw-killing it from one theft is stupid. Im going to use the primal */
/* stat on the mobs for blood its quick, effective, and straightford, AND */
/* no new fields have to be added to the mob.Shakti 09/07/98 */
		(blpr = number_range(30, 40));
		(victim->practice -= (blpr / 2));
		(ch->pcdata->condition[COND_THIRST] += blpr);
		if(victim->practice < 0)
		{
			sprintf(buf, "%s falls over dead.", victim->short_descr);
			act(buf, ch, 0, victim, TO_ROOM);
			act(buf, ch, 0, victim, TO_CHAR);
			raw_kill(victim);
		}

		if(ch->pcdata->condition[COND_THIRST] >= bloodpool / ch->pcdata->stats[UNI_GEN])
		{
			ch->pcdata->condition[COND_THIRST] = bloodpool / ch->pcdata->stats[UNI_GEN];
		}
		if(ch->fighting == 0)
			set_fighting(ch, victim);

		return;
	}
	if(IS_SET(victim->act, PLR_ACID))
	{
		send_to_char("Their blood is a deadly acid!\n\r", ch);
		ch->hit -= 300;
		victim->pcdata->condition[COND_THIRST] -= 30;
		return;
	}

	if(!IS_NPC(victim))
	{
		victim->pcdata->condition[COND_THIRST] -= number_range(30, 40);
	}
	if(ch->pcdata->condition[COND_THIRST] >= bloodpool / ch->pcdata->stats[UNI_GEN])
	{
		ch->pcdata->condition[COND_THIRST] = bloodpool / ch->pcdata->stats[UNI_GEN];
	}

	if(victim->pcdata->condition[COND_THIRST] <= 0)
	{
		victim->pcdata->condition[COND_THIRST] = 0;
	}
	if(ch->fighting == 0)
		set_fighting(ch, victim);

	return;
}


void do_diablerise(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	char buf[MAX_STRING_LENGTH];

	one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(arg[0] == '\0' && ch->embracing == 0)
	{
		send_to_char("Who do you wish to embrace?\n\r", ch);
		return;
	}

	if(ch->embracing != 0)
	{
		if((victim = ch->embracing) != 0)
		{
			send_to_char("You retract your fangs.", ch);
			act("$N retracts his fangs.", ch, 0, 0, TO_ROOM);
			stop_embrace(ch, victim);
			return;
		}
	}

	if((victim = get_char_room(ch, arg)) == 0 && ch->embracing == 0)
	{
		send_to_char("They arent here.\n\r", ch);
		return;
	}


	if(IS_NPC(victim) || !IS_CLASS(victim, CLASS_VAMPIRE))
	{
		send_to_char("You can only embrace vampires.\n\r", ch);
		return;
	}

	if(victim->position != POS_MORTAL)
	{
		send_to_char("You can only embrace mortally wounded vampires.\n\r", ch);
		return;
	}

	sprintf(buf, "%s leaps toward %s baring his fangs.", ch->name, victim->name);
	act(buf, ch, 0, 0, TO_ROOM);
	sprintf(buf, "You leap toward %s baring your fangs.\n\r", victim->name);
	send_to_char(buf, ch);
	WAIT_STATE(ch, 15);

	sprintf(buf, "You sink your teeth into their throat.\n\r");
	send_to_char(buf, ch);
	sprintf(buf, "%s sinks their teeth into %s's throat.", ch->name, victim->name);
	act(buf, ch, 0, 0, TO_ROOM);
	sprintf(buf, "%s sinks their teeth into your throat.\n\r", ch->name);
	send_to_char(buf, victim);
	victim->embraced = ch;
	ch->embracing = victim;

	return;
}


void do_preserve(CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		stc("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_NECR] < 2)
	{
		stc("You must obtain level 2 Necromancy to use Preserve.\n\r", ch);
		return;
	}

	if((obj = get_obj_carry(ch, arg)) == 0)
	{
		stc("You do not have that item.\n\r", ch);
		return;
	}

	if(obj->item_type == OBJ_VNUM_QUIVERING_BRAIN)
	{
		stc("You cannot preserve that item.\n\r", ch);
		return;
	}

	if(obj->item_type == OBJ_VNUM_SEVERED_HEAD)
	{
		stc("You cannot preserve that item.\n\r", ch);
		return;
	}

	if(obj->timer <= 0)
	{
		stc("That object has no timer.\n\r", ch);
		return;
	}

	obj->timer = -1;
	act("You place your hands on $p and concentrate on it.", ch, obj, 0, TO_CHAR);
	act("$n places $s hands on $p and it glows brightly.", ch, obj, 0, TO_ROOM);
	return;
}

void do_spiritguard(CHAR_DATA * ch, char *argument)
{

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		stc("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_NECR] < 4)
	{
		stc("You must obtain level 4 Necromancy to use Spirit Guardian.\n\r", ch);
		return;
	}

	if(!IS_SET(ch->flag2, AFF2_SPIRITGUARD))
	{
		stc("You awaken your spirit guardian.\n\r", ch);
		SET_BIT(ch->flag2, AFF2_SPIRITGUARD);
		return;
	}

	else
	{
		stc("You dismiss your spirit guardian.\n\r", ch);
		REMOVE_BIT(ch->flag2, AFF2_SPIRITGUARD);
		return;
	}
	return;
}

void do_spiritgate(CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		stc("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_NECR] < 3)
	{
		stc("You must obtain level 3 Necromancy to use SpiritGate.\n\r", ch);
		return;
	}

	if((obj = get_obj_world(ch, arg)) == 0)
	{
		stc("You cannot find that object.\n\r", ch);
		return;
	}

	if(ch->pcdata->condition[COND_THIRST] < 66)
	{
		stc("You do not have enough blood.\n\r", ch);
		return;
	}

	if(obj->in_room == 0)
	{
		stc("You cannot locate that object.\n\r", ch);
		return;
	}

  if(IS_AFFECTED(ch, AFF_SAFE))
        {
                send_to_char("You can't move while in safe mode.\n\r", ch);
                return;
        }

	if(obj->carried_by != 0)
	{
		stc("That corpse is being carried by someone!\n\r", ch);
		return;
	}

	if(obj->item_type != ITEM_CORPSE_PC && obj->item_type != ITEM_CORPSE_NPC)
	{
		stc("That's not even a corpse!\n\r", ch);
		return;
	}

	if(obj->in_room->vnum == ch->in_room->vnum)
	{
		stc("You're already there!\n\r", ch);
		return;
	}

	act("$n steps into a spirit gate and vanishes.", ch, 0, 0, TO_ROOM);
	char_from_room(ch);
	char_to_room(ch, get_room_index(obj->in_room->vnum));
	act("You step through a spirit gate and appear before $p.", ch, obj, 0, TO_CHAR);
	act("$n steps out of a spirit gate in front of $p.", ch, obj, 0, TO_ROOM);
	ch->pcdata->condition[COND_THIRST] -= 65;
	return;
}

void do_scream(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;
	CHAR_DATA *mount;
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		stc("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_MELP] < 1)
	{
		stc("You must obtain level 1 Melpominee to use Scream.\n\r", ch);
		return;
	}

	if(IS_SET(ch->in_room->room_flags, ROOM_SAFE))
	{
		stc("Your screams will have no effect here.\n\r", ch);
		return;
	}

	if(ch->pcdata->condition[COND_THIRST] < 50)
	{
		stc("You have insufficient blood.\n\r", ch);
		return;
	}

	for(vch = char_list; vch != 0; vch = vch_next)
	{
		vch_next = vch->next;

		if(vch->in_room == 0)
			continue;
		if(!IS_NPC(vch) && vch->pcdata->chobj != 0)
			continue;
		if(ch == vch)
			continue;
		if(vch->in_room == ch->in_room)
		{
			if((mount = ch->mount) != 0)
			{
				if(mount == vch)
					continue;
			}
			if(can_see(ch, vch))
			{
				if(number_range(1, (7 - ch->pcdata->powers[VAM_MELP])) == 2)
				{
					act("$n lets out an ear-popping scream!", ch, 0, vch, TO_ROOM);
					stc("You fall to the ground, clutching your ears.\n\r", vch);
					vch->position = POS_STUNNED;
					act("$n falls to the ground, stunned.", vch, 0, vch, TO_NOTVICT);
					return;
				}
				else
				{
					act("$n lets out an ear-popping scream!", ch, 0, vch, TO_ROOM);
					return;
				}
			}
			else
				continue;

			return;
		}
	}

	return;
}


void do_zombie(CHAR_DATA * ch, char *argument)
{

	char buf2[MAX_STRING_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	char arg[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	AFFECT_DATA af;
	OBJ_DATA *obj;

	one_argument(argument, arg);
	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(arg[0] == '\0')
	{
		send_to_char("Zombie what corpse?\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_NECR] < 5)
	{
		send_to_char("You require level 5 Necromancy to create a zombie.\n\r", ch);
		return;
	}

	if(ch->pcdata->followers > 5)
	{
		send_to_char("Nothing happens.\n\r", ch);
		return;
	}
	if((obj = get_obj_carry(ch, arg)) == 0)
	{
		send_to_char("You dont have that corpse.", ch);
		return;
	}
	if(obj->item_type != ITEM_CORPSE_NPC || IS_SET(obj->quest, QUEST_ZOMBIE))
	{
		send_to_char("You can only Zombie original corpses.\n\r", ch);
		return;
	}


	ch->pcdata->followers++;


	victim = create_mobile(get_mob_index(obj->value[2]));
	sprintf(buf, "the zombie of %s", victim->short_descr);
	sprintf(buf2, "the zombie of %s is here.\n\r", victim->short_descr);
	free_string(victim->short_descr);
	victim->short_descr = str_dup(buf);
	free_string(victim->name);
	victim->name = str_dup(buf);
	free_string(victim->long_descr);
	victim->long_descr = str_dup(buf2);
	SET_BIT(victim->extra, EXTRA_ZOMBIE);
	strcpy(buf, "Rise corpse, and bow before me!");
	do_say(ch, buf);
	sprintf(buf, "%s clambers back up to its feet.\n\r", obj->short_descr);
	act(buf, ch, 0, 0, TO_ROOM);
	send_to_char(buf, ch);


	char_to_room(victim, ch->in_room);

	if(victim->level < 100)
	{
		add_follower(victim, ch);
		af.duration = 666;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = AFF_CHARM;
		affect_to_char(victim, &af);
	}
	WAIT_STATE(ch, 10);
	extract_obj(obj);
	return;
}

void do_fleshcraft(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_VICI] < 2)
	{
		send_to_char("You need Vicissitude 2 to fleshcraft.\n\r", ch);
		return;
	}

	if(arg[0] == '\0')
	{
		send_to_char("Change to look like whom?\n\r", ch);
		return;
	}

	if(IS_AFFECTED(ch, AFF_POLYMORPH) && !IS_VAMPAFF(ch, VAM_DISGUISED))
	{
		send_to_char("Not while polymorphed.\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(!IS_NPC(victim) && ch != victim)
	{
		send_to_char("Not on Players.\n\r", ch);
		return;
	}


	if(ch->pcdata->condition[COND_THIRST] < 40)
	{
		send_to_char("You have insufficient blood.\n\r", ch);
		return;
	}
	ch->pcdata->condition[COND_THIRST] -= number_range(30, 40);

	if(ch == victim)
	{
		if(!IS_AFFECTED(ch, AFF_POLYMORPH) && !IS_VAMPAFF(ch, VAM_DISGUISED))
		{
			send_to_char("You already look like yourself!\n\r", ch);
			return;
		}
		sprintf(buf, "Your flesh molds and transforms into %s.", ch->name);
		act(buf, ch, 0, victim, TO_CHAR);
		sprintf(buf, "%s's flesh molds and transforms into %s.", ch->morph, ch->name);
		act(buf, ch, 0, victim, TO_ROOM);
		REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
		REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_DISGUISED);
		free_string(ch->morph);
		ch->morph = str_dup("");
		return;
	}
	if(IS_VAMPAFF(ch, VAM_DISGUISED))
	{
		sprintf(buf, "Your flesh molds and transforms into a clone of %s.", victim->short_descr);
		act(buf, ch, 0, victim, TO_CHAR);
		sprintf(buf, "%s's flesh molds and transforms into a clone of %s.", ch->morph, victim->short_descr);
		act(buf, ch, 0, victim, TO_NOTVICT);
		sprintf(buf, "%s's flesh mols and transforms into a clone of you!", ch->morph);
		act(buf, ch, 0, victim, TO_VICT);
		free_string(ch->morph);
		ch->morph = str_dup(victim->short_descr);
		return;
	}
	sprintf(buf, "Your flesh molds and transforms into a clone of %s.", victim->short_descr);
	act(buf, ch, 0, victim, TO_CHAR);
	sprintf(buf, "%s's flesh molds and transforms into a clone of %s.", ch->name, victim->short_descr);
	act(buf, ch, 0, victim, TO_NOTVICT);
	sprintf(buf, "%s's flesh molds and transforms into a clone of you!", ch->name);
	act(buf, ch, 0, victim, TO_VICT);
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_DISGUISED);
	free_string(ch->morph);
	ch->morph = str_dup(victim->short_descr);
	return;
}

void do_bonemod(CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];

	smash_tilde(argument);
	argument = one_argument(argument, arg1);


	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_VICI] < 3)
	{
		send_to_char("You need level 3 Vicissitude to use this power.\n\r", ch);
		return;
	}

	if(arg1[0] == '\0')
	{
		send_to_char("Syntax: bonemod <modification>\n\r", ch);
		send_to_char("Horns, Wings, Head, Exoskeleton, Tail\n\r", ch);
		return;
	}


	/*
	 * Set something.
	 */
	if(!str_cmp(arg1, "horns"))
	{

		if(!IS_VAMPAFF(ch, VAM_HORNS))
		{
			SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_HORNS);
			send_to_char("You pull a set of horns out of your head.\n\r", ch);
			act("$n pulls a set of horns from his head!\n\r", ch, 0, 0, TO_ROOM);
			return;
		}

		if(IS_VAMPAFF(ch, VAM_HORNS))
		{
			REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_HORNS);
			send_to_char("You push your horns back into your head.\n\r", ch);
			act("$n pushes $n's horns back into $n head.\n\r", ch, 0, 0, TO_ROOM);
			return;

		}
	}

	if(!str_cmp(arg1, "wings"))
	{

		if(!IS_VAMPAFF(ch, VAM_WINGS))
		{
			SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_WINGS);
			send_to_char("You pull a pair of leathery wings from your back.\n\r", ch);
			act("$n pulls a pair of leathery wings from $s back!\n\r", ch, 0, 0, TO_ROOM);
			return;
		}

		if(IS_VAMPAFF(ch, VAM_WINGS))
		{
			REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_WINGS);
			send_to_char("You push your wings into your back.\n\r", ch);
			act("$n pushes $s wings into $s back.\n\r", ch, 0, 0, TO_ROOM);
			return;

		}
	}

	if(!str_cmp(arg1, "exoskeleton"))
	{

		if(!IS_VAMPAFF(ch, VAM_EXOSKELETON))
		{
			SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_EXOSKELETON);
			send_to_char("Your skin is covered by a hard exoskeleton.\n\r", ch);
			act("$n's skin is covered by a hard exoskeleton!\n\r", ch, 0, 0, TO_ROOM);
			return;
		}

		if(IS_VAMPAFF(ch, VAM_EXOSKELETON))
		{
			REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_EXOSKELETON);
			send_to_char("Your exoskeleton slowly disappears under your skin.\n\r", ch);
			act("$n's hard exoskeleton disappears under $s skin.\n\r", ch, 0, 0, TO_ROOM);
			return;

		}
	}

	if(!str_cmp(arg1, "tail"))
	{

		if(!IS_VAMPAFF(ch, VAM_TAIL))
		{
			SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_TAIL);
			send_to_char("Your spine extends out into a long, pointed tail.\n\r", ch);
			act("$n's spine extends to form a long pointed tail!\n\r", ch, 0, 0, TO_ROOM);
			return;
		}

		if(IS_VAMPAFF(ch, VAM_TAIL))
		{
			REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_TAIL);
			send_to_char("Your tail slowly retracts into your spine.\n\r", ch);
			act("$n's tail shrinks and vanishes into $s spine.\n\r", ch, 0, 0, TO_ROOM);
			return;

		}
	}

	if(!str_cmp(arg1, "head"))
	{

		if(!IS_VAMPAFF(ch, VAM_HEAD))
		{
			SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_HEAD);
			send_to_char("Your head transforms into that of a fierce lion.\n\r", ch);
			act("$n's head transforms into that of a fierce lion!\n\r", ch, 0, 0, TO_ROOM);
			return;
		}

		if(IS_VAMPAFF(ch, VAM_HEAD))
		{
			REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_HEAD);
			send_to_char("Your head slowly resumes it's normal form.\n\r", ch);
			act("$n's head resumes its normal form.\n\r", ch, 0, 0, TO_ROOM);
			return;

		}
	}




	/*
	 * Generate usage message.
	 */
	do_bonemod(ch, "");
	return;
}


void do_pigeon(CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	CHAR_DATA *victim;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_ANIM] < 3)
	{
		send_to_char("You require Aimalism level 3 to pigeon.\n\r", ch);
		return;
	}
	if(arg1[0] == '\0')
	{
		send_to_char("Pigeon which object?\n\r", ch);
		return;
	}

	if(arg2[0] == '\0')
	{
		send_to_char("Pigeon what to whom?\n\r", ch);
		return;
	}
	if((obj = get_obj_carry(ch, arg1)) == 0)
	{
		send_to_char("You are not carrying that item.\n\r", ch);
		return;
	}
	victim = get_char_world(ch, arg2);
	if((victim = get_char_world(ch, arg2)) == 0)
	{
		send_to_char("They arent here.\n\r", ch);
		return;
	}

	if(!IS_NPC(victim) && !IS_IMMUNE(victim, IMM_TRANSPORT))
	{
		send_to_char("They dont want anything transported to them.\n\r", ch);
		return;
	}

	act("You place $p in a pigeon's beak and it flies away.", ch, obj, 0, TO_CHAR);
	act("$n places $p in a pigeon's beak and it flies away.", ch, obj, 0, TO_ROOM);
	obj_from_char(obj);
	obj_to_char(obj, victim);
	act("A pigeon lands on your shoulders and flies away after handing you $p.", victim, obj, 0, TO_CHAR);
	act("A pigeon lands on $n's shoulders and flies away after handing him $p.", victim, obj, 0, TO_ROOM);
	do_autosave(ch, "");
	do_autosave(victim, "");
	return;
}


void do_beckon(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;

	one_argument(argument, arg);

	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_ANIM] < 1)
	{
		send_to_char("You require level 1 animalism to beckon the wild.\n\r", ch);
		return;
	}
	if(arg[0] == '\0')
	{
		send_to_char("You can beckon a frog, a raven, a cat or a dog.\n\r", ch);
		return;
	}

	if(ch->pcdata->followers > 5)
	{
		send_to_char("Nothing happened.\n\r", ch);
		return;
	}

	if(!str_cmp(argument, "frog"))
	{
		victim = create_mobile(get_mob_index(MOB_VNUM_FROG));
		if(victim == 0)
		{
			send_to_char("Error - please inform Infidel.\n\r", ch);
			return;
		}
	}
	else if(!str_cmp(argument, "raven"))
	{
		victim = create_mobile(get_mob_index(MOB_VNUM_RAVEN));
		if(victim == 0)
		{
			send_to_char("Error - please inform Infidel.\n\r", ch);
			return;
		}
	}
	else if(!str_cmp(argument, "cat"))
	{
		victim = create_mobile(get_mob_index(MOB_VNUM_CAT));
		if(victim == 0)
		{
			send_to_char("Error - please inform Infidel.\n\r", ch);
			return;
		}
	}
	else if(!str_cmp(argument, "dog"))
	{
		victim = create_mobile(get_mob_index(MOB_VNUM_DOG));
		if(victim == 0)
		{
			send_to_char("Error - please inform Infidel.\n\r", ch);
			return;
		}
	}
	else
	{
		send_to_char("Your can beckon a frog, a raven, a cat or a dog.\n\r", ch);
		return;
	}

	act("You whistle loudly and $N walks in.", ch, 0, victim, TO_CHAR);
	act("$n whistles loudly and $N walks in.", ch, 0, victim, TO_ROOM);
	char_to_room(victim, ch->in_room);
	ch->pcdata->followers += 1;
	SET_BIT(victim->act, ACT_NOEXP);
	return;
}

void do_share(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	CHAR_DATA *familiar;
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];

	one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_ANIM] < 4)
	{
		send_to_char("You must obtain at least level 4 in Animalism to use Share Spirits.\n\r", ch);
		return;
	}

	if((familiar = ch->pcdata->familiar) != 0)
	{
		sprintf(buf, "You release %s.\n\r", familiar->short_descr);
		send_to_char(buf, ch);
		familiar->wizard = 0;
		ch->pcdata->familiar = 0;
		return;
	}

	if(arg[0] == '\0')
	{
		send_to_char("What do you wish to Share spirits with?\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(ch == victim)
	{
		send_to_char("What an intelligent idea!\n\r", ch);
		return;
	}

	if(!IS_NPC(victim))
	{
		send_to_char("Not on players.\n\r", ch);
		return;
	}

	if(victim->wizard != 0)
	{
		send_to_char("You are unable to Share Spirits them.\n\r", ch);
		return;
	}

	if(victim->level > (ch->spl[RED_MAGIC] * 0.25))
	{
		send_to_char("They are too powerful.\n\r", ch);
		return;
	}

	if(ch->pcdata->condition[COND_THIRST] < 25)
	{
		send_to_char("You have insufficient blood.\n\r", ch);
		return;
	}

	ch->pcdata->condition[COND_THIRST] -= 25;
	ch->pcdata->familiar = victim;
	victim->wizard = ch;
	act("You share your being with $N.", ch, 0, victim, TO_CHAR);
	act("$n is staring at you!", ch, 0, victim, TO_VICT);
	act("$n starts staring at $N", ch, 0, victim, TO_NOTVICT);
	return;
}

void do_frenzy(CHAR_DATA * ch, char *argument)
{
/*
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument (argument, arg);
*/

	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_ANIM] < 5)
	{
		send_to_char("You need level 5 Animalism to use Frenzy.\n\r", ch);
		return;
	}
/*
    if ( ( victim = get_char_room( ch, arg ) ) == 0 )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC(victim) )
	{
	  send_to_char("Not on NPC's!\n\r",ch);
	  return;
	}



    if (IS_CLASS(victim, CLASS_VAMPIRE) && arg[0] != '\0')
    {
	if ( victim->rage >= 25 )
	{
	    send_to_char("Their beast is already controlling them!\n\r",ch);
	    return;
	}

      if (number_percent ( )  <= 50)
	{
	    send_to_char("They shake off your attempt.\n\r",ch);
	    act("You shake off $n's attempt to frenzy you.\n\r",ch,0,victim,TO_VICT);
	    act("$N shakes off $n's attempt to frenzy
$m.\n\r",ch,0,victim,TO_ROOM);
          return;
      }

      if (victim->beast == 0 )
	{
	  send_to_char("They are too calm to frenzy!\n\r",ch);
	  return;
	}

	if (!IS_VAMPAFF(victim, VAM_NIGHTSIGHT)) do_nightsight(victim,"");
	if (!IS_VAMPAFF(victim, VAM_FANGS)) do_fangs(victim,"");
	if (!IS_VAMPAFF(victim, VAM_CLAWS)) do_claws(victim,"");
      act("$n sends you into a frenzied rage!",ch,0,victim,TO_VICT);
	act("You excite the beast within $n!",victim,0,0,TO_CHAR);
	act("$n bares $s fangs and growls as $s inner beast consumes
$m.",ch,0,0,TO_ROOM);
	ch->pcdata->stats[UNI_RAGE] += number_range(10,20);
	if (victim->beast > 0) do_beastlike(victim,"");
	WAIT_STATE(ch,12);
	return;
    }

    send_to_char("But they are already in a rage!\n\r",ch);
    return;
*/
	if(IS_CLASS(ch, CLASS_VAMPIRE))
	{
		if(ch->beast >= 20)
		{
			if(ch->pcdata->stats[UNI_RAGE] >= ch->beast)
			{
				send_to_char("Your beast is already controlling you.\n\r", ch);
				return;
			}
		}
		if(ch->beast <= 19)
		{
			if(ch->pcdata->stats[UNI_RAGE] >= 20)
			{
				send_to_char("Your beast is already controlling you.\n\r", ch);
				return;
			}
		}

/*
        if (ch->beast == 0)
	{
	   send_to_char("You are too calm to use frenzy.\n\r",ch);
	   return;
	}

*/

		if(!IS_VAMPAFF(ch, VAM_NIGHTSIGHT))
			do_nightsight(ch, "");
		if(!IS_VAMPAFF(ch, VAM_FANGS))
			do_fangs(ch, "");
		if(!IS_VAMPAFF(ch, VAM_CLAWS))
			do_claws(ch, "");
		send_to_char("You bare your fangs and growl as your inner beast consumes you.\n\r", ch);
		act("$n bares $s fangs and growls as $s inner beast consumes $m.", ch, 0, 0, TO_ROOM);
		ch->pcdata->stats[UNI_RAGE] += number_range(20, 30);
		if(ch->beast > 0)
			do_beastlike(ch, "");
		WAIT_STATE(ch, 12);
		return;
	}

	send_to_char("But you are already in a rage!\n\r", ch);
	return;
}

void do_serenity(CHAR_DATA * ch, char *argument)
{
	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("huh?.\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_ANIM] < 2)
	{
		send_to_char("You must obtain at least level 2 in Animalism to use Song of Serenity.\n\r", ch);
		return;
	}



	if(IS_CLASS(ch, CLASS_VAMPIRE) && ch->beast < 1)
	{
		if(ch->pcdata->stats[UNI_RAGE] < 1)
		{
			send_to_char("Your beast doesn't control your actions.\n\r", ch);
			return;
		}
		send_to_char("You chant a soft tune and pacify your inner beast.\n\r", ch);
		act("$n chants a soft tune and pacifies their inner beast.", ch, 0, 0, TO_ROOM);
		ch->pcdata->stats[UNI_RAGE] = 0;
		if(IS_VAMPAFF(ch, VAM_NIGHTSIGHT))
			do_nightsight(ch, "");
		if(IS_VAMPAFF(ch, VAM_FANGS))
			do_fangs(ch, "");
		if(IS_VAMPAFF(ch, VAM_CLAWS))
			do_claws(ch, "");
		WAIT_STATE(ch, 12);
		return;
	}
	else
	{
		send_to_char("Only those who achieve Golconda can sing the Song of Serenity.\n\r", ch);
		return;
	}
}

void do_sharpen(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_STRING_LENGTH];
	OBJ_DATA *obj;

	argument = one_argument(argument, arg);
	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_MELP] < 3)
	{
		send_to_char("You need Melpominee 3 to sharpen.\n\r", ch);
		return;
	}

	if(arg[0] == '\0')
	{
		send_to_char("What do you wish to sharpen?\n\r", ch);
		return;
	}

	if((obj = get_obj_carry(ch, arg)) == 0)
	{
		send_to_char("You dont have that weapon.\n\r", ch);
		return;
	}

	if(IS_SET(obj->quest, QUEST_ARTIFACT) || obj->item_type != ITEM_WEAPON)
	{
		send_to_char("You cant sharpen that item.\n\r", ch);
		return;
	}

	if(obj->value[0] == 18000)
	{
		send_to_char("This item is already Sharp!\n\r", ch);
		return;
	}

	if(obj->value[0] != 0)
	{
		send_to_char(" This weapon already has a power.\n\r", ch);
		return;
	}

	obj->value[0] = 18000;
	obj->value[1] = 20;
	obj->value[2] = 30;

	/*mite as well recycle the arg string */
	sprintf(arg, "You grind away at %s until it is razor sharp!", obj->short_descr);
	send_to_char(arg, ch);
	sprintf(arg, "%s grinds away at %s until it is razor sharp!", ch->name, obj->short_descr);
	act(arg, ch, 0, 0, TO_ROOM);
	return;
}


void do_lamprey(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;

	char buf[MAX_STRING_LENGTH];

	long dam;
	long bloodpool;


	{
		if(IS_SET(ch->newbits, NEW_TIDE))
			bloodpool = (3000 / ch->pcdata->stats[UNI_GEN]);
		else
			bloodpool = (2000 / ch->pcdata->stats[UNI_GEN]);
	}
	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_OBTE] < 5)
	{
		send_to_char("You need level 5 Obtenebration to Lamprey.\n\r", ch);
		return;

		send_to_char("You need level 5 Obtenebration to Lamprey.\n\r", ch);
		return;
	}


	if((victim = ch->fighting) == 0)
	{
		send_to_char("You aren't fighting anyone.\n\r", ch);
		return;

	}
	WAIT_STATE(ch, 5);


	if(!IS_NPC(victim))
	{
		dam = ch->pcdata->powers[VAM_OBTE] * 20;
	}
	else if(IS_NPC(victim))
	{
		dam = ch->pcdata->powers[VAM_OBTE] * 100;
	}
/*
if ( !IS_NPC(victim) && IS_CLASS(victim, CLASS_WEREWOLF) )
{
    if (victim->power[DISC_WERE_BOAR] > 2 ) dam *= 0.5;
}
*/
	if(is_safe(ch, victim) == TRUE)
		return;
	dam += number_range(1, 30);
	if(dam <= 0)
		dam = 1;


	sprintf(buf, "Your tendrils of darkness hits $N incredibly hard! [%li]\n\r", dam);
	act(buf, ch, 0, victim, TO_CHAR);
	sprintf(buf, "$n's tendrils of darkness hits you incredibly hard! [%li]\n\r", dam);
	act(buf, ch, 0, victim, TO_VICT);
	sprintf(buf, "$n's tendrils of darkness hits $N incredibly hard! [%li]\n\r", dam);
	act(buf, ch, 0, victim, TO_NOTVICT);



	send_to_char("\n\r", ch);
	victim->hit -= dam;
	ch->pcdata->condition[COND_THIRST] += number_range(40, 50);
	if(ch->pcdata->condition[COND_THIRST] > bloodpool)
	{
		send_to_char("Your bloodlust is sated.\n\r", ch);
		ch->pcdata->condition[COND_THIRST] = bloodpool;
	}
	return;
}


void do_shroud(CHAR_DATA * ch, char *argument)
{
	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE) || (ch->pcdata->powers[VAM_OBTE] < 1))
	{
		send_to_char("You need level 1 Obtenebration to use this power.\n\r", ch);
		return;
	}

	if(IS_SET(ch->act, PLR_SHROUD))
	{
		REMOVE_BIT(ch->act, PLR_SHROUD);
		send_to_char("You step out from the shadows.\n\r", ch);
		act("$n emerges from the shadows.", ch, 0, 0, TO_ROOM);
	}
	else
	{
		send_to_char("You slowly blend with the shadows.\n\r", ch);
		act("$n blends in with the shadows and disappears.", ch, 0, 0, TO_ROOM);
		SET_BIT(ch->act, PLR_SHROUD);
	}
	return;
}

void do_shadowstep(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];

	if(IS_NPC(ch))
		return;

	argument = one_argument(argument, arg);

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_OBTE] < 4)
	{
		send_to_char("You must obtain at least level 4 in Obtenebration to use this power.\n\r", ch);
		return;
	}

	if(arg[0] == '\0')
	{
		send_to_char("Shadowstep to who?\n\r", ch);
		return;
	}

	if((victim = get_char_world(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(IS_IMMUNE(victim, IMM_SUMMON))
	{
		send_to_char("He doesnt want you near him!\n\r", ch);
		return;
	}

/*
    if (IS_NPC(victim))
    {
	send_to_char("Not to mobs!\n\r",ch);
	return;
    }
*/
	if(ch == victim)
	{
		send_to_char("But you're already at yourself!\n\r", ch);
		return;
	}

	if(IS_NPC(victim) && (!room_is_dark(victim->in_room)))
	{
		send_to_char("You cant find a shadow in his room.\n\r", ch);
		return;
	}

	if(victim->in_room == ch->in_room)
	{
		send_to_char("But you're already there!\n\r", ch);
		return;
	}

	char_from_room(ch);
	char_to_room(ch, victim->in_room);
	act("You step into the shadows and appear before $N.", ch, 0, victim, TO_CHAR);
	act("$n appears out of the shadows before $N.", ch, 0, victim, TO_NOTVICT);
	act("$n appears from the shadows in front of you.", ch, 0, victim, TO_VICT);
	do_look(ch, "auto");
	return;
}


void do_cauldron(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	long value;
	long dam;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	value = is_number(arg2) ? atoi(arg2) : -1;

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_THAU] < 2)
	{
		send_to_char("You need at least level 2 Thaumaturgy to use this power.\n\r", ch);
		return;
	}


	if(arg1[0] == '\0')
	{
		send_to_char("Whose blood do you wish to boil?\n\r", ch);
		return;
	}
	if((victim = get_char_room(ch, arg1)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(arg2[0] == '\0')
	{
		send_to_char("How much blood do you want to use?\n\r", ch);
		return;
	}

	if(value > 200)
	{
		stc("You can only use 200 blood on this power.\n\r", ch);
		return;
	}


	if(ch->pcdata->condition[COND_THIRST] < value)
	{
		send_to_char("You don't have that much blood!\n\r", ch);
		return;
	}

	if(is_safe(ch, victim) == TRUE)
		return;

	if(IS_NPC(victim))
	{
		dam = (value * 4);
		victim->hit = (victim->hit - dam);
		ch->pcdata->condition[COND_THIRST] = ch->pcdata->condition[COND_THIRST] - value;
		act("$N screams in agony as $m blood begins to boil.", ch, 0, victim, TO_CHAR);
		act("$N screams in agony as $n causes his blood to boil.", ch, 0, victim, TO_NOTVICT);
		act("You scream in agony as $n causes your blood to boil.", ch, 0, victim, TO_VICT);
		WAIT_STATE(ch, 12);
		return;
	}

	if(!IS_NPC(victim))
	{
		dam = (long) (value * 1.5);
		victim->hit = (victim->hit - dam);
		if(victim->pcdata->condition[COND_THIRST] < value)
			victim->pcdata->condition[COND_THIRST] = 1;
		victim->pcdata->condition[COND_THIRST] = victim->pcdata->condition[COND_THIRST] - value;
		ch->pcdata->condition[COND_THIRST] = ch->pcdata->condition[COND_THIRST] - value;
		act("$N screams in agony as $m blood begins to boil.", ch, 0, victim, TO_CHAR);
		act("$N screams in agony as $n causes his blood to boil.", ch, 0, victim, TO_NOTVICT);
		act("You scream in agony as $n causes your blood to boil.", ch, 0, victim, TO_VICT);
		WAIT_STATE(ch, 12);
		return;
	}


}


void do_taste(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char buf[MAX_STRING_LENGTH];
	char age[MAX_STRING_LENGTH];
	char lin[MAX_STRING_LENGTH];
	char lord[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_THAU] < 1)
	{
		send_to_char("You must obtain at least level 1 in Thaumaturgy to use Taste of Blood.\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(!IS_CLASS(victim, CLASS_VAMPIRE))
	{
		send_to_char("Only useful on vampire targets.\n\r", ch);
		return;
	}

	if(IS_NPC(victim))
	{
		send_to_char("Only useful on vampire targets.\n\r", ch);
		return;
	}

	sprintf(buf, "You examine $N's blood carefully.\n\r");
	act(buf, ch, 0, victim, TO_CHAR);
	sprintf(buf, "$n examines your blood carefully.\n\r");
	act(buf, ch, 0, victim, TO_VICT);
	sprintf(buf, "$n examines $N's blood carefully.\n\r");
	act(buf, ch, 0, victim, TO_NOTVICT);


	send_to_char("\n\r", ch);
	send_to_char("\n\r", victim);


	if(victim->pcdata->rank == AGE_ANCILLA)
		sprintf(age, "Ancilla");
	else if(victim->pcdata->rank == AGE_CHILDE)
		sprintf(age, "Childe");
	else if(victim->pcdata->rank == AGE_NEONATE)
		sprintf(age, "Neonate");
	else if(victim->pcdata->rank == AGE_ELDER)
		sprintf(age, "Elder");
	else if(victim->pcdata->rank == AGE_METHUSELAH)
		sprintf(age, "Methuselah");
	if(victim->lord == 0)
		sprintf(lord, "None");
	else
		sprintf(lord, "%s", victim->lord);
	sprintf(lin, "---------------------------------------------------------------------------\n\r");
	send_to_char(lin, ch);
	send_to_char("                              Vampire Status\n\r", ch);
	send_to_char(lin, ch);
	sprintf(buf,
		"Generation:%li  Bloodpool:%li  Age:%s  Lord:%s\n\r",
		victim->pcdata->stats[UNI_GEN], victim->pcdata->condition[COND_THIRST], age, lord);
	send_to_char(buf, ch);
	send_to_char(lin, ch);
	send_to_char("                              Disciplines\n\r", ch);
	send_to_char(lin, ch);
	sprintf(buf,
		"Animalism:    [%li]             Celerity:   [%li]             Fortitude: [%li]\n\r",
		victim->pcdata->powers[VAM_ANIM], victim->pcdata->powers[VAM_CELE], victim->pcdata->powers[VAM_FORT]);
	send_to_char(buf, ch);
	sprintf(buf,
		"Obtenebration:[%li]             Presence:   [%li]             Quietus: [%li]\n\r",
		victim->pcdata->powers[VAM_OBTE], victim->pcdata->powers[VAM_PRES], victim->pcdata->powers[VAM_QUIE]);
	send_to_char(buf, ch);
	sprintf(buf,
		"Thaumaturgy:  [%li]             Auspex:     [%li]             Dominate: [%li]\n\r",
		victim->pcdata->powers[VAM_THAU], victim->pcdata->powers[VAM_AUSP], victim->pcdata->powers[VAM_DOMI]);
	send_to_char(buf, ch);
	sprintf(buf,
		"Obfuscate:    [%li]             Potence:    [%li]             Protean: [%li]\n\r",
		victim->pcdata->powers[VAM_OBFU], victim->pcdata->powers[VAM_POTE], victim->pcdata->powers[VAM_PROT]);
	send_to_char(buf, ch);
	sprintf(buf,
		"Serpentis:    [%li]             Vicissitude:[%li]             Daimoinon: [%li]\n\r",
		victim->pcdata->powers[VAM_SERP], victim->pcdata->powers[VAM_VICI], victim->pcdata->powers[VAM_DAIM]);
	send_to_char(buf, ch);
	sprintf(buf,
		"Chimerstry:   [%li]             Obeah:      [%li]             Melpominee: [%li]\n\r",
		victim->pcdata->powers[VAM_CHIM], victim->pcdata->powers[VAM_OBEA], victim->pcdata->powers[VAM_MELP]);
	send_to_char(buf, ch);
	sprintf(buf,
		"Necromancy:   [%li]                                         Thanatosis: [%li]\n\r",
		victim->pcdata->powers[VAM_NECR], victim->pcdata->powers[VAM_THAN]);
	stc(buf, ch);
	send_to_char(lin, ch);

	return;
}

void do_tide(CHAR_DATA * ch, char *argument)
{
	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_THAU] < 5)
	{
		send_to_char("You require level 5 Thaumaturgy to use Tide of Vitae.\n\r", ch);
		return;
	}

	if(ch->practice < 10)
	{
		send_to_char("You require at least 10 primal to use Tide of Vitae.\n\r", ch);
		return;
	}

	if(IS_SET(ch->newbits, NEW_TIDE))
	{
		send_to_char("The tide is already with you.\n\r", ch);
		return;
	}
	SET_BIT(ch->newbits, NEW_TIDE);
	ch->practice -= 10;
	send_to_char("You feel a tide of vitae rush over you.\n\r", ch);


}

void do_potency(CHAR_DATA * ch, char *argument)
{

	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(ch->pcdata->stats[UNI_GEN] == 2)
	{
		send_to_char("you are at your max generation!\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_THAU] < 3)
	{
		send_to_char("You must obtain at least level 3 in Thaumaturgy to use Blood Potency.\n\r", ch);
		return;
	}
	if(IS_EXTRA(ch, EXTRA_POTENCY))
	{
		send_to_char("Your blood is already potent.\n\r", ch);
		return;
	}
	if(ch->pcdata->condition[COND_THIRST] < 300)
	{
		send_to_char("You have insufficient blood.\n\r", ch);
		return;
	}
	if(ch->pcdata->stats[UNI_GEN] == 3)
	{
		send_to_char("Your blood is as potent as it can get.\n\r", ch);
		return;
	}

	if(ch->pcdata->stats[UNI_GEN] <= 1)
	{
		send_to_char("You burst out in a massive flood of blood, covering the entire world.\n\r", ch);
		return;
	}

	ch->pcdata->condition[COND_THIRST] = ch->pcdata->condition[COND_THIRST] - 300;
	ch->pcdata->stats[UNI_GEN] = ch->pcdata->stats[UNI_GEN] - 1;
	SET_BIT(ch->extra, EXTRA_POTENCY);
	send_to_char("You feel your blood become more potent.\n\r", ch);
	return;

}

void do_conceal(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;

	one_argument(argument, arg);
	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_OBFU] < 5)
	{
		send_to_char("You need obfuscate 5 to conceal items.\n\r", ch);
		return;
	}
	if(arg[0] == '\0')
	{
		send_to_char("Syntax: Conceal (item).\n\r", ch);
		return;
	}
	if((obj = get_obj_carry(ch, arg)) == 0)
	{
		send_to_char("You dont have that item.\n\r", ch);
		return;
	}
	if(IS_SET(obj->extra_flags, ITEM_INVIS))
	{
		sprintf(buf, "%s fades into existance.", obj->short_descr);
		send_to_char(buf, ch);
		act(buf, ch, 0, 0, TO_ROOM);
		REMOVE_BIT(obj->extra_flags, ITEM_INVIS);
		return;
	}
	if(!IS_SET(obj->extra_flags, ITEM_INVIS))
	{
		sprintf(buf, "%s fades out of existance.", obj->short_descr);
		send_to_char(buf, ch);
		act(buf, ch, 0, 0, TO_ROOM);
		SET_BIT(obj->extra_flags, ITEM_INVIS);
		return;
	}

	return;
}

void do_tendrils(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	long sn;
	long level;
	long spelltype;

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_SERP] < 4)
	{
		send_to_char("you need level 4 serpentis to use Arms of the Abyss.\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(ch == victim)
	{
		send_to_char("You cannot use Arms of the Abyss on yourself.\n\r", ch);
		return;
	}

	if(IS_AFFECTED(victim, AFF_ETHEREAL))
	{
		send_to_char("You cannot Arms of Abyss an ethereal person.\n\r", ch);
		return;
	}
	if(is_safe(ch, victim) == TRUE)
		return;
	if((sn = skill_lookup("tendrils")) < 0)
		return;
	spelltype = skill_table[sn].target;
	level = (long) (ch->spl[spelltype] * 0.25);
	(*skill_table[sn].spell_fun) (sn, level, ch, victim);
	WAIT_STATE(ch, 12);
	return;
}


void do_tongue(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;

	char buf[MAX_STRING_LENGTH];

	long dam;

	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_SERP] < 5)
	{
		send_to_char("You need level 5 Serpentis to tongue.\n\r", ch);
		return;

		send_to_char("You need level 5 Serpentis to tongue.\n\r", ch);
		return;
	}


	if((victim = ch->fighting) == 0)
	{
		send_to_char("You aren't fighting anyone.\n\r", ch);
		return;

	}
	WAIT_STATE(ch, 5);

	if(!IS_NPC(victim))
	{
		dam = ch->pcdata->powers[VAM_SERP] * 25;
	}
	else if(IS_NPC(victim))
	{
		dam = ch->pcdata->powers[VAM_SERP] * 125;
	}
/*
if ( !IS_NPC(victim) && IS_CLASS(victim, CLASS_WEREWOLF) )
{
    if (victim->power[DISC_WERE_BOAR] > 2 ) dam *= 0.5;
}
*/
	if(is_safe(ch, victim) == TRUE)
		return;
	dam += number_range(1, 30);
	if(dam <= 0)
		dam = 1;
	sprintf(buf, "Your tongue of the serpent hits $N incredibly hard! [%li]\n\r", dam);
	act(buf, ch, 0, victim, TO_CHAR);
	sprintf(buf, "$n's tongue of the serpent hits you incredibly hard! [%li]\n\r", dam);
	act(buf, ch, 0, victim, TO_VICT);
	sprintf(buf, "$n's tongue of the serpent hits $N incredibly hard! [%li]\n\r", dam);
	act(buf, ch, 0, victim, TO_NOTVICT);


	send_to_char("\n\r", ch);
	victim->hit -= dam;
	return;
}


void do_poison(CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_SERP] < 3)
	{
		send_to_char("You must obtain at least level 3 in Serpentis to use Poison.\n\r", ch);
		return;
	}

	if(((obj = get_eq_char(ch, WEAR_WIELD)) == 0) && ((obj = get_eq_char(ch, WEAR_HOLD)) == 0))
	{
		send_to_char("You must wield the weapon you wish to poison.\n\r", ch);
		return;
	}

	if(obj->value[0] != 0)
	{
		send_to_char("This weapon cannot be poisoned.\n\r", ch);
		return;
	}

	if(ch->pcdata->condition[COND_THIRST] < 15)
	{
		send_to_char("You have insufficient blood.\n\r", ch);
		return;
	}
	ch->pcdata->condition[COND_THIRST] -= number_range(5, 15);
	act("You run your tongue along $p, poisoning it.", ch, obj, 0, TO_CHAR);
	act("$n runs $s tongue along $p, poisoning it.", ch, obj, 0, TO_ROOM);
	obj->value[0] = 53;
	obj->timer = number_range(10, 20);

	return;
}

void do_objmask(CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		stc("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_CHIM] < 5)
	{
		stc("You must obtain level 5 Chimerstry to Mask an Object.\n\r", ch);
		return;
	}

	if(IS_VAMPAFF(ch, VAM_CHANGED))
	{
		send_to_char("You can't objectmask while changed.\n\r", ch);
		return;
	}
	if(IS_POLYAFF(ch, POLY_SERPENT))
	{
		send_to_char("You cannot objectmask from this form.\n\r", ch);
		return;
	}

	if(IS_SET(ch->flag2, VAMP_OBJMASK))
	{
		stc("You return to your normal form.\n\r", ch);
		sprintf(buf, "%s transforms back into %s.\n\r", ch->morph, ch->name);
		act(buf, ch, 0, 0, TO_ROOM);
		ch->morph = str_dup("");
		ch->objdesc = str_dup("");
		ch->long_descr = str_dup("");
		ch->short_descr = str_dup(ch->name);
		REMOVE_BIT(ch->flag2, VAMP_OBJMASK);
		do_mortalvamp(ch, "");
		REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
		return;
	}

	if(has_timer(ch))
		return;

	if(ch->pcdata->condition[COND_THIRST] < 50)
	{
		send_to_char("You have insufficient blood.\n\r", ch);
		return;
	}

	ch->pcdata->condition[COND_THIRST] -= number_range(40, 50);

	if(arg[0] == '\0')
	{
		stc("What object do you wish to mask yourself as?\n\r", ch);
		return;
	}

	if((obj = get_obj_here(ch, arg)) == 0)
	{
		send_to_char("That object is not here.\n\r", ch);
		return;
	}

	SET_BIT(ch->flag2, VAMP_OBJMASK);
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	act("You mask yourself as $p.", ch, obj, 0, TO_CHAR);
	act("$n masks $mself as $p.", ch, obj, 0, TO_ROOM);
	free_string(ch->morph);
	ch->morph = str_dup(obj->short_descr);
	free_string(ch->objdesc);
	ch->objdesc = str_dup(obj->description);
	ch->long_descr = str_dup("");
	return;
}

void do_mirror(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char buf[MAX_INPUT_LENGTH];

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		stc("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_CHIM] < 1)
	{
		stc("You must obtain level 1 Chimersty to use Mirror Image.\n\r", ch);
		return;
	}

	if(ch->pcdata->condition[COND_THIRST] < 20)
	{
		stc("You do not have enough blood to create a Mirror Image of yourself.\n\r", ch);
		return;
	}

	if(ch->pcdata->followers > 4)
	{
		send_to_char("Nothing happens.\n\r", ch);
		return;
	}

	ch->pcdata->followers++;


	victim = create_mobile(get_mob_index(30000));
	victim->short_descr = str_dup(ch->name);
	sprintf(buf, "%s is hovering here.\n\r", ch->name);
	victim->long_descr = str_dup(buf);
	victim->name = str_dup(ch->name);
	victim->level = 20;
	victim->max_hit = 2000;
	victim->hit = 2000;
	victim->mana = 2000;
	victim->max_mana = 2000;
	victim->max_move = 2000;
	victim->move = 2000;
	SET_BIT(victim->flag2, VAMP_CLONE);
	SET_BIT(victim->act, ACT_NOEXP);
	ch->pcdata->condition[COND_THIRST] -= 20;
	char_to_room(victim, ch->in_room);
	act("You concentrate your powers and form a mirror image of yourself.", ch, 0, victim, TO_CHAR);
	act("$n waves $s hands and a mirror image of $mself appears.", ch, 0, victim, TO_ROOM);
	return;

}

void do_control(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	CHAR_DATA *familiar;
	char arg[MAX_INPUT_LENGTH];

	one_argument(argument, arg);

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		stc("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_CHIM] < 4)
	{
		stc("You must obtain level 4 Chimerstry to use Control the Clone.\n\r", ch);
		return;
	}

	if(arg[0] == '\0')
	{
		send_to_char("Which clone do you wish to control?\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(ch == victim)
	{
		send_to_char("Become your own familiar?\n\r", ch);
		return;
	}

	if(!IS_NPC(victim))
	{
		stc("Not on players.\n\r", ch);
		return;
	}

	if(!IS_SET(victim->flag2, VAMP_CLONE))
	{
		stc("That is not a clone.\n\r", ch);
		return;
	}

	if((familiar = ch->pcdata->familiar) != 0)
		familiar->wizard = 0;
	ch->pcdata->familiar = victim;
	victim->wizard = ch;
	act("You take control of $N's mind.", ch, 0, victim, TO_CHAR);
	act("$n takes control of $N's mind.", ch, 0, victim, TO_ROOM);

	return;
}


void do_formillusion(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char buf[MAX_INPUT_LENGTH];

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		stc("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_CHIM] < 2)
	{
		stc("You must obtain level 3 Chimersty to Form an Illusion of yourself.\n\r", ch);
		return;
	}

	if(ch->pcdata->condition[COND_THIRST] < 30)
	{
		stc("You do not have enough blood to Form an Illusion of yourself.\n\r", ch);
		return;
	}

	if(ch->pcdata->followers > 4)
	{
		send_to_char("Nothing happens.\n\r", ch);
		return;
	}

	ch->pcdata->followers++;


	victim = create_mobile(get_mob_index(30000));
	victim->short_descr = str_dup(ch->name);
	sprintf(buf, "%s is hovering here.\n\r", ch->name);
	victim->long_descr = str_dup(buf);
	victim->name = str_dup(ch->name);
	victim->level = 200;
	victim->max_hit = ch->max_hit;
	victim->hit = victim->max_hit;
	victim->max_mana = ch->max_mana;
	victim->mana = victim->max_mana;
	victim->max_move = ch->max_move;
	victim->move = victim->max_move;
	victim->hitroll = ch->hitroll;
	victim->damroll = ch->damroll;
	SET_BIT(victim->flag2, VAMP_CLONE);
	SET_BIT(victim->act, ACT_NOEXP);
	char_to_room(victim, ch->in_room);
	ch->pcdata->condition[COND_THIRST] -= 30;
	act("You concentrate your powers and form an illusion of yourself.", ch, 0, victim, TO_CHAR);
	act("$n waves $s hands and splits in two.", ch, 0, victim, TO_ROOM);
	return;

}

void do_clone(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	OBJ_INDEX_DATA *pObjIndex;
	OBJ_DATA *obj;
	OBJ_DATA *obj2;
	AFFECT_DATA *paf;
	AFFECT_DATA *paf2;

	argument = one_argument(argument, arg1);
	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_CHIM] < 3)
	{
		send_to_char("You need Chimeristry 3 to clone objects.\n\r", ch);
		return;
	}


	if(arg1[0] == '\0')
	{
		send_to_char("Make a clone of what object?\n\r", ch);
		return;
	}

	if((obj = get_obj_carry(ch, arg1)) == 0)
	{
		send_to_char("You dont have that item.\n\r", ch);
		return;
	}

	if(obj->points > 0
	   || IS_SET(obj->quest, QUEST_ARTIFACT)
	   || IS_SET(obj->quest, QUEST_RELIC)
	   || IS_SET(obj->quest, ITEM_EQUEST)
	   || IS_SET(obj->quest, QUEST_ANCIENT)
	   || (obj->chobj) != 0 || obj->item_type == ITEM_QUEST || obj->item_type == ITEM_QUESTCARD)
	{

		send_to_char("You cant clone that object.\n\r", ch);
		return;
	}
	if(obj->questowner != 0 && strlen(obj->questowner) > 1 && !str_cmp(ch->name, obj->questowner))
	{
		send_to_char("You can't clone that object.\n\r", ch);
		return;
	}

	if(obj->pIndexData->vnum >= 29662 && obj->pIndexData->vnum <= 29663)
	{
		send_to_char("Cheater.  We are docking you ALL of your experience.\n\r", ch);
		sprintf(buf, "%s is a big ass cloning longsword cheater.\n\r", ch->name);
		log_string(buf);
		ch->exp = 0;
		return;
	}
	pObjIndex = get_obj_index(obj->pIndexData->vnum);
	obj2 = create_object(pObjIndex, obj->level);
	/* Copy any changed parts of the object. */
	free_string(obj2->name);
	obj2->name = str_dup(obj->name);
	free_string(obj2->short_descr);
	obj2->short_descr = str_dup(obj->short_descr);
	free_string(obj2->description);
	obj2->description = str_dup(obj->description);

	if(obj->questmaker != 0 && strlen(obj->questmaker) > 1)
	{
		free_string(obj2->questmaker);
		obj2->questmaker = str_dup(obj->questmaker);
	}

	if(obj->chpoweron != 0)
	{
		free_string(obj2->chpoweron);
		obj2->chpoweron = str_dup(obj->chpoweron);
	}
	if(obj->chpoweroff != 0)
	{
		free_string(obj2->chpoweroff);
		obj2->chpoweroff = str_dup(obj->chpoweroff);
	}
	if(obj->chpoweruse != 0)
	{
		free_string(obj2->chpoweruse);
		obj2->chpoweruse = str_dup(obj->chpoweruse);
	}
	if(obj->victpoweron != 0)
	{
		free_string(obj2->victpoweron);
		obj2->victpoweron = str_dup(obj->victpoweron);
	}
	if(obj->victpoweroff != 0)
	{
		free_string(obj2->victpoweroff);
		obj2->victpoweroff = str_dup(obj->victpoweroff);
	}
	if(obj->victpoweruse != 0)
	{
		free_string(obj2->victpoweruse);
		obj2->victpoweruse = str_dup(obj->victpoweruse);
	}
	obj2->item_type = obj->item_type;
	obj2->extra_flags = obj->extra_flags;
	obj2->wear_flags = obj->wear_flags;
	obj2->weight = obj->weight;
	obj2->spectype = obj->spectype;
	obj2->specpower = obj->specpower;
	obj2->condition = obj->condition;
	obj2->toughness = obj->toughness;
	obj2->resistance = obj->resistance;
	obj2->quest = obj->quest;
	obj2->points = obj->points;
	obj2->cost = obj->cost;
	obj2->value[0] = obj->value[0];
	obj2->value[1] = obj->value[1];
	obj2->value[2] = obj->value[2];
	obj2->value[3] = obj->value[3];
    /*****************************************/
	obj_to_char(obj2, ch);

	if(obj->affected != 0)
	{
		for(paf = obj->affected; paf != 0; paf = paf->next)
		{
			if(affect_free == 0)
				paf2 = alloc_perm(sizeof(*paf));
			else
			{
				paf2 = affect_free;
				affect_free = affect_free->next;
			}
			paf2->type = 0;
			paf2->duration = paf->duration;
			paf2->location = paf->location;
			paf2->modifier = paf->modifier;
			paf2->bitvector = 0;
			paf2->next = obj2->affected;
			obj2->affected = paf2;
		}
	}
	obj2->timer = ch->pcdata->powers[VAM_CHIM] * 2;
	SET_BIT(obj2->quest, QUEST_CLONED);
	act("A clone of $p appears in your hands in a flash of light.", ch, obj, 0, TO_CHAR);
	act("A clone of $p appears in $n's hands in a flash of light.", ch, obj, 0, TO_ROOM);
	if(obj->questmaker != 0)
		free_string(obj->questmaker);
	obj->questmaker = str_dup(ch->name);
	WAIT_STATE(ch, 16);
	return;
}

void do_bloodwater(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg1[MAX_INPUT_LENGTH];
	long dam;

	argument = one_argument(argument, arg1);


	if(IS_NPC(ch))
		return;


	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_NECR] < 1)
	{
		send_to_char("You need at least level 1 Necromancy to use Blood Water.\n\r", ch);
		return;
	}


	if(arg1[0] == '\0')
	{
		send_to_char("Whose blood do you wish to turn to water?\n\r", ch);
		return;
	}
	if((victim = get_char_room(ch, arg1)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}


	if(is_safe(ch, victim) == TRUE)
		return;
	if(IS_NPC(victim))
		dam = 1000;
	if(!IS_NPC(victim))
		dam = 750;
	if(!IS_CLASS(victim, CLASS_VAMPIRE))
		victim->hit -= dam;

	if(!ch->fighting || ch->fighting != victim)
		set_fighting(ch, victim);

	act("$N screams in agony as you turn his blood to water.", ch, 0, victim, TO_CHAR);
	act("$N screams in agony as $n turns his blood to water.", ch, 0, victim, TO_NOTVICT);
	act("You scream in agony as $n turns your blood to water.", ch, 0, victim, TO_VICT);
	if(!IS_CLASS(victim, CLASS_VAMPIRE))
		WAIT_STATE(ch, 12);
	if(IS_CLASS(victim, CLASS_VAMPIRE))
	{
		WAIT_STATE(ch, 6);
		victim->pcdata->condition[COND_THIRST] -= 150;
	}
	return;
}

void do_mindblast(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	long dam;

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_PRES] < 2)
	{
		send_to_char("You require presence 2 to Mindblast.\n\r", ch);
		return;
	}

	if(arg[0] == '\0')
	{
		send_to_char("Mindblast Whom?\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They arent here.\n\r", ch);
		return;
	}

	if(ch == victim)
	{
		send_to_char("You cannot Mindblast yourself!\n\r", ch);
		return;
	}
	if(IS_SET(victim->in_room->room_flags, ROOM_SAFE))
	{
		send_to_char("You cannot attack them here.\n\r", ch);
		return;
	}
	if(is_safe(ch, victim) == TRUE)
		return;
	if(victim->hit < victim->max_hit)
	{
		send_to_char("They are hurt and alert.\n\r", ch);
		return;
	}


	act("You close your eyes and concentrate on $N.", ch, 0, victim, TO_CHAR);
	act("$n closes $s eyes and concentrates on you.", ch, 0, victim, TO_VICT);
	act("$n closes $s eyes and concentrates on $N.", ch, 0, victim, TO_NOTVICT);
	WAIT_STATE(ch, 12);
	if(number_percent() < 50)
		dam = number_range(1, 4);
	else
	{
		dam = 0;
		send_to_char("You failed.\n\r", ch);
		return;
	}

	dam += char_damroll(ch);
	if(dam == 0)
		dam = 1;
	if(!IS_AWAKE(victim))
		dam *= 2;

	if(dam <= 0)
		dam = 1;
	victim->hit -= dam;

	act("You clutch your head in agony!", victim, 0, 0, TO_CHAR);
	act("$n clutches his head in agony!", victim, 0, 0, TO_ROOM);
	victim->position = POS_STUNNED;
	if(IS_NPC(victim) && victim->hit < 0)
	{
		raw_kill(victim);
		return;
	}
	if(!IS_NPC(victim) && victim->hit < -2)
		update_pos(ch);
	return;
}


void do_entrance(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	long sn;
	long level;
	long spelltype;
	char buf[MAX_STRING_LENGTH];

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_PRES] < 3)
	{
		send_to_char("You need presence 3 to entrance.\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(ch == victim)
	{
		send_to_char("You cannot entrance yourself.\n\r", ch);
		return;
	}

	if(IS_AFFECTED(victim, AFF_ETHEREAL))
	{
		send_to_char("You cannot entrance an ethereal person.\n\r", ch);
		return;
	}
	if(is_safe(ch, victim) == TRUE)
		return;
	sprintf(buf, "A look of concentration crosses your face.");
	act(buf, ch, 0, 0, TO_CHAR);
	sprintf(buf, "A look of concentration crosses over $n's face.\n\r");
	act(buf, ch, 0, victim, TO_ROOM);

	if((sn = skill_lookup("charm")) < 0)
		return;
	spelltype = skill_table[sn].target;
	level = ch->pcdata->powers[VAM_PRES] * 40;
	(*skill_table[sn].spell_fun) (sn, level, ch, victim);
	WAIT_STATE(ch, 12);
	return;
}


void do_summon(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	CHAR_DATA *mount;
	char arg[MAX_INPUT_LENGTH];

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		do_nosummon(ch, argument);
		return;
	}

	argument = one_argument(argument, arg);

	if(arg[0] == '\0')
	{
		send_to_char("Summon whom?\n\r", ch);
		return;
	}

	if((victim = get_char_world(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}


	if(victim == ch
	   || victim->in_room == 0
	   || IS_SET(victim->in_room->room_flags, ROOM_SAFE)
	   || IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
	   || IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
	   || IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
	   || victim->level >= ch->level + 5
	   || victim->fighting != 0 || (!IS_NPC(victim) && !IS_IMMUNE(victim, IMM_SUMMON)))

	{
		send_to_char("You cannot locate them.\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_PRES] < 4)
	{
		send_to_char("You need level 4 in Presence to use this.\n\r", ch);
		return;
	}
	WAIT_STATE(ch, 10);
	act("$n disappears suddenly.", victim, 0, 0, TO_ROOM);
	char_from_room(victim);
	char_to_room(victim, ch->in_room);
	act("$n arrives suddenly.", victim, 0, 0, TO_ROOM);
	act("$N has summoned you!", victim, 0, ch, TO_CHAR);
	do_look(victim, "auto");
	if((mount = victim->mount) == 0)
		return;
	char_from_room(mount);
	char_to_room(mount, get_room_index(victim->in_room->vnum));
	do_look(mount, "auto");
	return;
}

void do_awe(CHAR_DATA * ch, char *argument)
{
	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_PRES] < 1)
	{
		send_to_char("You must obtain at least level 1 in Presence to you Awe.\n\r", ch);
		return;
	}
	if(IS_EXTRA(ch, EXTRA_AWE))
	{
		REMOVE_BIT(ch->extra, EXTRA_AWE);
		send_to_char("You are no longer Awe Inspiring.\n\r", ch);
		act("$n is no longer awe inspiring.", ch, 0, 0, TO_ROOM);
		return;
	}
	SET_BIT(ch->extra, EXTRA_AWE);
	send_to_char("You are now Awe Inspiring.\n\r", ch);
	act("$n is now awe inspiring", ch, 0, 0, TO_ROOM);
	return;


}

void do_unveil(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;

	one_argument(argument, arg);

	stc("Huh?\n\r", ch);
	return;

	if(ch->pcdata->powers[VAM_AUSP] < 3)
	{
		stc("You must obtain level 3 Auspex to use Unveil the Shrouded Mind.\n\r", ch);
		return;
	}

	if(arg[0] == '\0')
	{
		send_to_char("Whose mind do you wish to Unveil?\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(victim == ch)
	{
		stc("Why would you want to unveil your own mind?\n\r", ch);
		return;
	}

	if(victim->level > ch->level)
	{
		stc("Their mind is far too powerful for you to Unveil.\n\r", ch);
		return;
	}

	if(victim->spl[BLUE_MAGIC] > ch->spl[BLUE_MAGIC] / 2)
	{
		stc("Their mind is far too powerful for you to Unveil.\n\r", ch);
		return;
	}

	victim->unveil = ch;
	act("You gaze deeply into $N's eyes.\n\rYou have unveiled $S mind.\n\r", ch, 0, victim, TO_CHAR);
	return;
}

void do_astralwalk(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];

	if(IS_NPC(ch))
		return;

	argument = one_argument(argument, arg);

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_AUSP] < 4)
	{
		send_to_char("You must obtain level 4 Auspex to use Astral Walk.\n\r", ch);
		return;
	}

	if(arg[0] == '\0')
	{
		send_to_char("Astral Walk to who?\n\r", ch);
		return;
	}

	if((victim = get_char_world(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(!IS_NPC(victim) && !IS_IMMUNE(victim, IMM_SUMMON))
	{
		send_to_char("You are unable to locate them.\n\r", ch);
		return;
	}
  if(IS_AFFECTED(ch, AFF_SAFE))
        {
                send_to_char("You can't move while in safe mode.\n\r", ch);
                return;
        }

	if(ch == victim)
	{
		send_to_char("But you're already at yourself!\n\r", ch);
		return;
	}

	if(victim->in_room == ch->in_room)
	{
		send_to_char("But you're already there!\n\r", ch);
		return;
	}

	char_from_room(ch);
	char_to_room(ch, victim->in_room);
	act("You step through a rift in the Astral plane.\n\rYou leap out of the Astral Plane before $N.", ch, 0, victim,
	    TO_CHAR);
	act("$n leaps out of a rift in the Astral Plane.", ch, 0, victim, TO_NOTVICT);
	act("$n leaps out of a rift in the Astral Plane.", ch, 0, victim, TO_VICT);
	do_look(ch, "auto");
	return;
}

void do_dragonform(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_VICI] < 4)
	{
		send_to_char("You must obtain at least level 4 in Vicissitude to use Dragonform\n\r", ch);
		return;
	}


	if(!IS_POLYAFF(ch, POLY_ZULO))
	{
		send_to_char("You can only Dragonform while in Zuloform.\n\r", ch);
		return;
	}

	if(IS_EXTRA(ch, EXTRA_DRAGON))
	{
		REMOVE_BIT(ch->extra, EXTRA_DRAGON);
		act("You transform back into zuloform.", ch, 0, 0, TO_CHAR);
		act("$n shrinks back into a big black monster.", ch, 0, 0, TO_ROOM);

		if(ch->hit < 1)
			ch->hit = 1;
		ch->damroll = ch->damroll - 300;
		ch->hitroll = ch->hitroll - 300;
		free_string(ch->morph);
		ch->morph = str_dup("A big black monster");
		return;
	}


	if(ch->pcdata->condition[COND_THIRST] < 400)
	{
		send_to_char("You have insufficient blood.\n\r", ch);
		return;
	}

	if(ch->mounted == IS_RIDING)
		do_dismount(ch, "");
	ch->pcdata->condition[COND_THIRST] -= number_range(200, 400);
	act("You transform into a large dragon.", ch, 0, 0, TO_CHAR);
	act("$n's body grows and distorts into a large dragon.", ch, 0, 0, TO_ROOM);
	SET_BIT(ch->extra, EXTRA_DRAGON);
	sprintf(buf, "%s a huge rabid dragon", ch->name);
	free_string(ch->morph);
	ch->morph = str_dup(buf);
	ch->damroll = ch->damroll + 300;
	ch->hitroll = ch->hitroll + 300;
	return;
}


void do_plasma(CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_VICI] < 5)
	{
		send_to_char("You must obtain at least level 5 in Vicissitude to use Plasma Form.\n\r", ch);
		return;
	}

	save_char_obj(ch);

	if((obj = create_object(get_obj_index(30007), 60)) == 0)
	{
		send_to_char("Error - Please inform Maser.\n\r", ch);
		return;
	}

	if(IS_EXTRA(ch, EXTRA_PLASMA))
	{
		ch->pcdata->obj_vnum = 0;
		REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
		REMOVE_BIT(ch->extra, EXTRA_OSWITCH);
		REMOVE_BIT(ch->extra, EXTRA_PLASMA);
		ch->pcdata->chobj = 0;
		obj->chobj = 0;
		free_string(ch->morph);
		ch->morph = str_dup("");
		act("$p transforms into $n.", ch, obj, 0, TO_ROOM);
		act("Your reform your human body.", ch, obj, 0, TO_CHAR);
		extract_obj(obj);
		return;

	}

	if(IS_AFFECTED(ch, AFF_POLYMORPH))
	{
		send_to_char("Not while polymorphed.\n\r", ch);
		return;
	}
	act("$n transforms into $p and splashes to the ground.", ch, obj, 0, TO_ROOM);
	act("You transform into $p and splashes to the ground.", ch, obj, 0, TO_CHAR);
	ch->pcdata->obj_vnum = ch->pcdata->powers[VPOWER_OBJ_VNUM];
	obj->chobj = ch;
	ch->pcdata->chobj = obj;
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	SET_BIT(ch->extra, EXTRA_OSWITCH);
	SET_BIT(ch->extra, EXTRA_PLASMA);
	free_string(ch->morph);
	ch->morph = str_dup("a pool of blood");
	obj_to_room(obj, ch->in_room);
	return;
}

void do_bloodagony(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_STRING_LENGTH];
	long value;
	long pcost;
	OBJ_DATA *obj;

	one_argument(argument, arg);

	return;

	value = ch->pcdata->powers[VAM_QUIE];
	pcost = ch->pcdata->powers[VAM_QUIE] * 60;

	if(arg[0] == '\0')
	{
		send_to_char("Bloodagony what?\n\r", ch);
		return;
	}

	if(IS_NPC(ch))
		return;
	if((obj = get_obj_carry(ch, arg)) == 0)
	{
		send_to_char("You dont have that weapon.\n\r", ch);
		return;
	}

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_QUIE] < 3)
	{
		send_to_char("You need level 3 Quietus to use bloodagony.\n\r", ch);
		return;
	}

	if(obj->item_type != ITEM_WEAPON
	   || IS_SET(obj->quest, QUEST_ARTIFACT) || obj->chobj != 0 || IS_SET(obj->quest, QUEST_BLOODA))
	{
		send_to_char("You are unable to bloodagony this weapon.\n\r", ch);
		return;
	}
	ch->pcdata->quest += pcost;
	oset_affect(ch, obj, value, APPLY_DAMROLL, TRUE);
	oset_affect(ch, obj, value, APPLY_HITROLL, TRUE);
	obj->points -= pcost;
	SET_BIT(obj->quest, QUEST_BLOODA);

	act("You cut your wrist and smear your blood on $p.", ch, obj, 0, TO_CHAR);
	act("$n cuts his wrist and smears blood on $p.", ch, obj, 0, TO_ROOM);
	return;
}


void do_spit(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;

	char buf[MAX_STRING_LENGTH];

	long dam;

	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_QUIE] < 1)
	{
		send_to_char("You need level 1 Quietus to spit.\n\r", ch);
		return;
	}


	if((victim = ch->fighting) == 0)
	{
		send_to_char("You aren't fighting anyone.\n\r", ch);
		return;

	}
	if(ch->pcdata->condition[COND_THIRST] < 5)
	{
		send_to_char("You dont have enough blood.\n\r", ch);
		return;
	}
	WAIT_STATE(ch, 12);
	if(!IS_NPC(victim))
	{
		dam = ch->pcdata->powers[VAM_QUIE] * 50;
	}
	else if(IS_NPC(victim))
	{
		dam = ch->pcdata->powers[VAM_QUIE] * 250;
	}
	ch->pcdata->condition[COND_THIRST] -= 5;
	/*if ( !IS_NPC(victim) && IS_CLASS(victim, CLASS_WEREWOLF) )
	   {
	   if (victim->power[DISC_WERE_BOAR] > 2 ) dam *= 0.5;
	   }
	 */
	dam += number_range(1, 30);
	if(dam <= 0)
		dam = 1;
	sprintf(buf, "Your spit of acid hits $N incredibly hard! [%li]", dam);
	act(buf, ch, 0, victim, TO_CHAR);
	sprintf(buf, "$n's spit of acid hits you incredibly hard! [%li]", dam);
	act(buf, ch, 0, victim, TO_VICT);
	sprintf(buf, "$n's spit of acid hits $N incredibly hard! [%li]", dam);
	act(buf, ch, 0, victim, TO_NOTVICT);


	send_to_char("\n\r", ch);
	victim->hit -= dam;
	if(victim->hit <= 10)
		one_hit(ch, victim, 20, gsn_headbutt);
	return;
}

void do_assassinate(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	char buf[MAX_STRING_LENGTH];
	long dam;

	argument = one_argument(argument, arg);


	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_QUIE] < 4)
	{
		send_to_char("You need Quietus level 4 to Assassinate.\n\r", ch);
		return;
	}

	if(arg[0] == '\0')
	{
		send_to_char("Assassinate whom?\n\r", ch);
		return;
	}


	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(victim == ch)
	{
		send_to_char("How can you assassinate yourself?\n\r", ch);
		return;
	}

	if(in_tournament(ch, victim))
		return;

	if(is_safe(ch, victim) == TRUE)
		return;

	if(victim->fighting != 0)
	{
		send_to_char("You can't assassinate a fighting person.\n\r", ch);
		return;
	}

	if(victim->hit < victim->max_hit)
	{
		act("$N is hurt and suspicious ... you can't sneak up.", ch, 0, victim, TO_CHAR);
		return;
	}
	if(!IS_AFFECTED(ch, AFF_HIDE))
	{
		send_to_char("You must be hidden to assassinate!\n\r", ch);
		return;
	}
	WAIT_STATE(ch, 15);
	if(number_percent() < 5)
	{
		sprintf(buf, "%s has been assassinated by %s.", victim->name, ch->name);
		do_info(ch, buf);
		victim->hit = 1;
		ch->fighting = victim;
		send_to_char("You spin around and throw a headbutt to finish him.\n\r", ch);
		damage(ch, victim, 11, gsn_headbutt);
		return;
	}

	if(!IS_NPC(victim))
		dam = ch->pcdata->powers[VAM_QUIE] * 200;
	if(IS_NPC(victim))
		dam = ch->pcdata->powers[VAM_QUIE] * 400;
	dam += number_range(1, 20);
	sprintf(buf, "Your assassination hits $N incredibly hard! [%li]", dam);
	act(buf, ch, 0, victim, TO_CHAR);
	sprintf(buf, "$n's assassination hits you incredibly hard! [%li]\n\r", dam);
	act(buf, ch, 0, victim, TO_VICT);
	sprintf(buf, "$n's assassination hits $N incredibly hard! [%li]\n\r", dam);
	act(buf, ch, 0, victim, TO_NOTVICT);

	if(dam > victim->hit)
		dam = victim->hit - 1;
	victim->hit -= dam;
	ch->fighting = victim;
	victim->fighting = ch;
	if(victim->hit <= 1)
	{
		victim->hit = 1;
		send_to_char("You spin around and throw a headbutt to finish him.\n\r", ch);
		ch->fighting = victim;
		damage(ch, victim, 11, gsn_headbutt);

	}

	return;
}

void do_infirmity(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	long sn;
	long level;
	long spelltype;

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;


	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(IS_CLASS(ch, CLASS_VAMPIRE) && ch->pcdata->powers[VAM_QUIE] < 2)
	{
		send_to_char("You need level 2 Quietus to use this power.\n\r", ch);
		return;
	}
	if(arg[0] == '\0')
	{
		send_to_char("Who do you wish to use Infirmity on?\n\r", ch);
		return;
	}


	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}
	if(is_safe(ch, victim) == TRUE)
		return;
	if((sn = skill_lookup("infirmity")) < 0)
	{
		sprintf(buf, "Yep, sn is bieng set to %li.", sn);
		send_to_char(buf, ch);
		return;

	}

	spelltype = skill_table[sn].target;
	level = ch->pcdata->powers[VAM_QUIE];
	(*skill_table[sn].spell_fun) (sn, level, ch, victim);
	WAIT_STATE(ch, 12);
	return;
}


void do_death(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	ROOM_INDEX_DATA *inroom;

	inroom = ch->in_room;

	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_QUIE] < 5)
	{
		send_to_char("You require level 5 Quietus to use Silence of Death.\n\r", ch);
		return;
	}
	if(IS_SET(ch->in_room->room_flags, ROOM_SILENCE))
	{
		sprintf(buf, "The silence leaves the room.\n\r");
		act(buf, ch, 0, 0, TO_ROOM);
		send_to_char("The silence leaves the room.\n\r", ch);
		REMOVE_BIT(ch->in_room->room_flags, ROOM_SILENCE);
		return;
	}
	if(ch->pcdata->condition[COND_THIRST] < 300)
	{
		send_to_char("You have insufficient blood.\n\r", ch);
		return;
	}
	ch->pcdata->condition[COND_THIRST] -= 300;
	SET_BIT(ch->in_room->room_flags, ROOM_SILENCE);
	sprintf(buf, "A look of concentration passes over %s's face.\n\r", ch->name);
	act(buf, ch, 0, 0, TO_ROOM);
	send_to_char("A look of concentration passes over your face.\n\r", ch);
	sprintf(buf, "An eerie silence fills the room.\n\r");
	act(buf, ch, 0, 0, TO_ROOM);
	send_to_char(buf, ch);
	return;
}


void do_command(CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	char buffy[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	long awe;


	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_DOMI] < 1)
	{
		send_to_char("You must obtain at least level 1 in Dominate to use Command.\n\r", ch);
		return;
	}

	if(ch->spl[RED_MAGIC] < 1)
	{
		send_to_char("Your mind is too weak.\n\r", ch);
		return;
	}

	if(arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char("Command whom to do what?\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, arg1)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(victim == ch)
	{
		send_to_char("How can you command yourself??\n\r", ch);
		return;
	}

	{
		if(ch->pcdata->powers[VAM_DOMI] > 2)
			awe = 50;
		else if(ch->pcdata->powers[VAM_DOMI] > 3)
			awe = 75;
		else if(ch->pcdata->powers[VAM_DOMI] > 4)
			awe = 100;
		else
			awe = 25;
	}

	if(IS_EXTRA(ch, EXTRA_AWE))
	{
		awe += 75;
	}

	if(!IS_NPC(victim) && victim->level != 3)
	{
		send_to_char("You can only command other avatars.\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_DOMI] > 1)
	{
		sprintf(buffy, "%s %s", arg2, argument);
		if(IS_NPC(victim))
			sprintf(buf, "I think %s wants to %s", victim->short_descr, buffy);
		else if(!IS_NPC(victim) && IS_AFFECTED(victim, AFF_POLYMORPH))
			sprintf(buf, "I think %s wants to %s", victim->morph, buffy);
		else
			sprintf(buf, "I think %s wants to %s", victim->name, buffy);
		do_say(ch, buf);
	}
	else
	{
		if(IS_NPC(victim))
			sprintf(buf, "I think %s wants to %s", victim->short_descr, arg2);
		else if(!IS_NPC(victim) && IS_AFFECTED(victim, AFF_POLYMORPH))
			sprintf(buf, "I think %s wants to %s", victim->morph, arg2);
		else
			sprintf(buf, "I think %s wants to %s", victim->name, arg2);
		do_say(ch, buf);
	}

	if(IS_NPC(victim) && victim->level >= awe)
	{
		act("You shake off $N's suggestion.", victim, 0, ch, TO_CHAR);
		act("$n shakes off $N's suggestion.", victim, 0, ch, TO_NOTVICT);
		act("$n shakes off your suggestion.", victim, 0, ch, TO_VICT);
		act("$s mind is too strong to overcome.", victim, 0, ch, TO_VICT);
		return;
	}

	else if(victim->spl[BLUE_MAGIC] >= (ch->spl[RED_MAGIC] / 2))
	{
		act("You shake off $N's suggestion.", victim, 0, ch, TO_CHAR);
		act("$n shakes off $N's suggestion.", victim, 0, ch, TO_NOTVICT);
		act("$n shakes off your suggestion.", victim, 0, ch, TO_VICT);
		act("$s mind is too strong to overcome.", victim, 0, ch, TO_VICT);
		return;
	}

	act("You blink in confusion.", victim, 0, 0, TO_CHAR);
	act("$n blinks in confusion.", victim, 0, 0, TO_ROOM);
	strcpy(buf, "Yes, you're right, I do...");
	do_say(victim, buf);
	if(ch->pcdata->powers[VAM_DOMI] > 1)
		interpret(victim, buffy);
	else
		interpret(victim, arg2);
	return;

}


void do_mesmerise(CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	char buffy[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	long awe;


	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_DOMI] < 2)
	{
		send_to_char("You must obtain at least level 2 in Dominate to use mesmerise.\n\r", ch);
		return;
	}

	if(ch->spl[RED_MAGIC] < 1)
	{
		send_to_char("Your mind is too weak.\n\r", ch);
		return;
	}

	if(arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char("Command whom to do what?\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, arg1)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(victim == ch)
	{
		send_to_char("How can you command yourself??\n\r", ch);
		return;
	}

	{
		if(ch->pcdata->powers[VAM_DOMI] > 2)
			awe = 50;
		else if(ch->pcdata->powers[VAM_DOMI] > 3)
			awe = 75;
		else if(ch->pcdata->powers[VAM_DOMI] > 4)
			awe = 100;
		else
			awe = 25;
	}

	if(IS_EXTRA(ch, EXTRA_AWE))
	{
		awe += 75;
	}

	if(!IS_NPC(victim) && victim->level != 3)
	{
		send_to_char("You can only command other avatars.\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_DOMI] > 1)
	{
		sprintf(buffy, "%s %s", arg2, argument);
		if(IS_NPC(victim))
			sprintf(buf, "I think %s wants to %s", victim->short_descr, buffy);
		else if(!IS_NPC(victim) && IS_AFFECTED(victim, AFF_POLYMORPH))
			sprintf(buf, "I think %s wants to %s", victim->morph, buffy);
		else
			sprintf(buf, "I think %s wants to %s", victim->name, buffy);
		do_say(ch, buf);
	}
	else
	{
		if(IS_NPC(victim))
			sprintf(buf, "I think %s wants to %s", victim->short_descr, arg2);
		else if(!IS_NPC(victim) && IS_AFFECTED(victim, AFF_POLYMORPH))
			sprintf(buf, "I think %s wants to %s", victim->morph, arg2);
		else
			sprintf(buf, "I think %s wants to %s", victim->name, arg2);
		do_say(ch, buf);
	}

	if(IS_NPC(victim) && victim->level >= awe)
	{
		act("You shake off $N's suggestion.", victim, 0, ch, TO_CHAR);
		act("$n shakes off $N's suggestion.", victim, 0, ch, TO_NOTVICT);
		act("$n shakes off your suggestion.", victim, 0, ch, TO_VICT);
		act("$s mind is too strong to overcome.", victim, 0, ch, TO_VICT);
		return;
	}

	else if(victim->spl[BLUE_MAGIC] >= (ch->spl[RED_MAGIC] / 2))
	{
		act("You shake off $N's suggestion.", victim, 0, ch, TO_CHAR);
		act("$n shakes off $N's suggestion.", victim, 0, ch, TO_NOTVICT);
		act("$n shakes off your suggestion.", victim, 0, ch, TO_VICT);
		act("$s mind is too strong to overcome.", victim, 0, ch, TO_VICT);
		return;
	}

	act("You blink in confusion.", victim, 0, 0, TO_CHAR);
	act("$n blinks in confusion.", victim, 0, 0, TO_ROOM);
	strcpy(buf, "Yes, you're right, I do...");
	do_say(victim, buf);
	if(ch->pcdata->powers[VAM_DOMI] > 1)
		interpret(victim, buffy);
	else
		interpret(victim, arg2);
	return;

}


void do_acid(CHAR_DATA * ch, char *argument)
{
	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_DOMI] < 4)
	{
		send_to_char("You require level 4 dominate to use Acid Blood.\n\r", ch);
		return;
	}
	if(ch->pcdata->condition[COND_THIRST] < 500)
	{
		send_to_char("You have insufficient blood.\n\r", ch);
		return;
	}
	if(IS_SET(ch->act, PLR_ACID))
	{
		send_to_char("Your blood is already acid.\n\r", ch);
		return;
	}
	SET_BIT(ch->act, PLR_ACID);
	send_to_char("Your blood turns to a potent acid.\n\r", ch);
	return;
}

void do_possession(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	CHAR_DATA *familiar;
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];

	one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

/* kavirpoint
	send_to_char("This ability has been temporarily
disabled.\n\r",ch);
	return;
*/
	if(ch->pcdata->powers[VAM_DOMI] < 3)
	{
		send_to_char("You must obtain at least level 3 in Dominate to use Possession.\n\r", ch);
		return;
	}

	if((familiar = ch->pcdata->familiar) != 0)
	{
		sprintf(buf, "You break your hold over %s.\n\r", familiar->short_descr);
		send_to_char(buf, ch);
		familiar->wizard = 0;
		ch->pcdata->familiar = 0;
		return;
	}

	if(arg[0] == '\0')
	{
		send_to_char("What do you wish to Possess?\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(ch == victim)
	{
		send_to_char("What an intelligent idea!\n\r", ch);
		return;
	}

	if(!IS_NPC(victim))
	{
		send_to_char("Not on players.\n\r", ch);
		return;
	}

	if(victim->wizard != 0)
	{
		send_to_char("You are unable to possess them.\n\r", ch);
		return;
	}

	if(victim->level > (ch->spl[RED_MAGIC] * 0.25))
	{
		send_to_char("They are too powerful.\n\r", ch);
		return;
	}

	if(ch->pcdata->condition[COND_THIRST] < 50)
	{
		send_to_char("You have insufficient blood.\n\r", ch);
		return;
	}

	ch->pcdata->condition[COND_THIRST] -= 50;
	ch->pcdata->familiar = victim;
	victim->wizard = ch;
	act("You concentrate on $N.", ch, 0, victim, TO_CHAR);
	act("$n is staring at you!", ch, 0, victim, TO_VICT);
	act("$n starts staring at $N", ch, 0, victim, TO_NOTVICT);
	return;
}


void do_coil(CHAR_DATA * ch, char *argument)
{
	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_DOMI] < 5)
	{
		send_to_char("You require level 5 Dominate to use Body Coil.\n\r", ch);
		return;
	}
	if(!IS_SET(ch->newbits, NEW_COIL))
	{
		send_to_char("You prepare to coil your victims.\n\r", ch);
		SET_BIT(ch->newbits, NEW_COIL);
		return;
	}
	else if(IS_SET(ch->newbits, NEW_COIL))
	{
		send_to_char("You stand down from your coil posture.\n\r", ch);
		REMOVE_BIT(ch->newbits, NEW_COIL);
		return;
	}
	return;
}

void do_earthmeld(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *mount;

	one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_PROT] < 4)
	{
		send_to_char("You must obtain at least level 4 Protean to use Earthmeld.\n\r", ch);
		return;
	}


	if(IS_AFFECTED(ch, AFF_POLYMORPH))
	{
		send_to_char("Not while polymorphed.\n\r", ch);
		return;
	}

	if(has_timer(ch))
		return;

	if(IS_NPC(ch) || IS_EXTRA(ch, EXTRA_EARTHMELD))
	{
		REMOVE_BIT(ch->affected_by, AFF_SHIFT);
		REMOVE_BIT(ch->extra, EXTRA_EARTHMELD);
		REMOVE_BIT(ch->act, PLR_WIZINVIS);
		if(IS_HEAD(ch, LOST_HEAD))
			REMOVE_BIT(ch->loc_hp[0], LOST_HEAD);
		free_string(ch->morph);
		ch->morph = str_dup("");
		ch->level = 3;
		ch->trust = 3;
		send_to_char("You rise up from the ground.\n\r", ch);
		sprintf(buf, "%s rises up from the ground", ch->name);
		act(buf, ch, 0, 0, TO_ROOM);

		return;

	}

	if((mount = ch->mount) != 0)
		do_dismount(ch, "");
	SET_BIT(ch->affected_by, AFF_SHIFT);
	SET_BIT(ch->extra, EXTRA_EARTHMELD);
	SET_BIT(ch->act, PLR_WIZINVIS);
	free_string(ch->morph);
	ch->morph = str_dup("Someone");
	ch->level = 4;
	ch->trust = 4;
	send_to_char("You sink into the ground.\n\r", ch);
	sprintf(buf, "%s sinks into the ground.", ch->name);
	act(buf, ch, 0, 0, TO_ROOM);
	return;
}

void do_flamehands(CHAR_DATA * ch, char *argument)
{
	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_PROT] < 5)
	{
		send_to_char("You need level 5 Protean to use flaming hands.\n\r", ch);
		return;
	}

	if(IS_SET(ch->newbits, NEW_MONKFLAME))
	{
		REMOVE_BIT(ch->newbits, NEW_MONKFLAME);
		send_to_char("Your hands are no longer engulfed by flames.\n\r", ch);
		act("$n's hands are no longer engulfed by flames.\n\r", ch, 0, 0, TO_ROOM);
		return;
	}
	if(!IS_SET(ch->newbits, NEW_MONKFLAME))
	{
		SET_BIT(ch->newbits, NEW_MONKFLAME);
		send_to_char("Your hands are engulfed by flames!\n\r", ch);
		act("$n's hands are engulfed by flames!\n\r", ch, 0, 0, TO_ROOM);
		return;
	}
}


void do_drain(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	long dam;

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_THAN] < 5)
	{
		send_to_char("You must obtain level 5 Thanatosis to use Drain Life.\n\r", ch);
		return;
	}


	if((victim = get_char_room(ch, arg)) == 0 && (ch->fighting) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}
	if((ch->fighting) != 0)
	{
		victim = ch->fighting;
	}

	if(arg[0] == '\0' && (ch->fighting) == 0)
	{
		send_to_char("Whose life do you wish to drain?\n\r", ch);
		return;
	}

	if(victim->hit < 500)
	{
		send_to_char("They are far to hurt to do that!\n\r", ch);
		return;
	}

	if(is_safe(ch, victim))
	{
		send_to_char("Not while in a safe room!\n\r", ch);
		return;
	}

	if(IS_NPC(victim))
	{
		dam = ch->pcdata->powers[VAM_THAN] * 10;
		victim->hit = (victim->hit - dam);
		if(dam > 500)
			dam = number_range(450, 550);
		ch->hit = (ch->hit + dam);
		if(ch->hit > ch->max_hit + 1000)
			ch->hit = ch->max_hit + 1000;
		act("You drain the lifeforce out of $N.", ch, 0, victim, TO_CHAR);
		act("$n drains the lifeforce out of $N.", ch, 0, victim, TO_NOTVICT);
		act("You feel $n draining your lifeforce.", ch, 0, victim, TO_VICT);
		WAIT_STATE(ch, 12);
		return;
	}

	if(!IS_NPC(victim))
	{
		dam = ch->pcdata->powers[VAM_THAN] * 50;
		victim->hit = (victim->hit - dam);
		if(dam > 400)
			dam = number_range(350, 450);
		ch->hit = (ch->hit + dam);
		if(ch->hit > ch->max_hit + 1000)
			ch->hit = ch->max_hit + 1000;
		act("You drain the lifeforce out of $N.", ch, 0, victim, TO_CHAR);
		act("$n drains the lifeforce out of $N.", ch, 0, victim, TO_NOTVICT);
		act("You feel $n draining your lifeforce.", ch, 0, victim, TO_VICT);
		WAIT_STATE(ch, 12);
		return;
	}


}

void do_ashes(CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_THAN] < 3)
	{
		send_to_char("You must obtain at least level 5 in Vicissitude to use Plasma Form.\n\r", ch);
		return;
	}

	save_char_obj(ch);

	if((obj = create_object(get_obj_index(20008), 60)) == 0)
	{
		send_to_char("Error - Please inform Maser.\n\r", ch);
		return;
	}

	if(IS_EXTRA(ch, EXTRA_ASH))
	{
		ch->pcdata->obj_vnum = 0;
		REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
		REMOVE_BIT(ch->extra, EXTRA_OSWITCH);
		REMOVE_BIT(ch->extra, EXTRA_ASH);
		ch->pcdata->chobj = 0;
		obj->chobj = 0;
		free_string(ch->morph);
		ch->morph = str_dup("");
		act("$p transforms into $n.", ch, obj, 0, TO_ROOM);
		act("Your reform your human body.", ch, obj, 0, TO_CHAR);
		extract_obj(obj);
		return;

	}

	if(IS_AFFECTED(ch, AFF_POLYMORPH))
	{
		send_to_char("Not while polymorphed.\n\r", ch);
		return;
	}
	act("$n transforms into $p and falls to the ground.", ch, obj, 0, TO_ROOM);
	act("You transform into $p and falls to the ground.", ch, obj, 0, TO_CHAR);
	ch->pcdata->obj_vnum = ch->pcdata->powers[VPOWER_OBJ_VNUM];
	obj->chobj = ch;
	ch->pcdata->chobj = obj;
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	SET_BIT(ch->extra, EXTRA_OSWITCH);
	SET_BIT(ch->extra, EXTRA_ASH);
	free_string(ch->morph);
	ch->morph = str_dup("a pile of ashes");
	obj_to_room(obj, ch->in_room);
	return;
}

void do_withering(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];

	argument = one_argument(argument, arg);


	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_THAN] < 4)
	{
		send_to_char("You must obtain level 4 Thanatosis to use Withering.\n\r", ch);
		return;
	}
	if(arg[0] == '\0')
	{
		send_to_char("Who do you wish to wither?\n\r", ch);
		return;
	}
	victim = get_char_room(ch, arg);
	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They arent here.\n\r", ch);
		return;
	}

	if(IS_SET(victim->in_room->room_flags, ROOM_SAFE))
	{
		send_to_char("You cannot attack them here.\n\r", ch);
		return;
	}

	if(is_safe(ch, victim) == TRUE)
		return;

	WAIT_STATE(ch, 35);
	sprintf(buf, "A look of concentration crosses over %s's face.", ch->name);
	act(buf, ch, 0, 0, TO_ROOM);
	send_to_char("A look of concentration crosses over your face.\n\r", ch);
	if(IS_ARM_R(victim, LOST_ARM) && IS_ARM_L(victim, LOST_ARM) &&
	   IS_LEG_R(victim, LOST_LEG) && IS_LEG_L(victim, LOST_LEG))
	{
		send_to_char("All of their limbs are severed already!\n\r", ch);
		return;
	}
	if(number_percent() < 15)
	{
		if(!IS_ARM_R(victim, LOST_ARM))
		{
			SET_BIT(victim->loc_hp[3], LOST_ARM);
			if((obj = get_eq_char(victim, WEAR_HOLD)) != 0)
				take_item(victim, obj);
			if((obj = get_eq_char(victim, WEAR_HANDS)) != 0)
				take_item(victim, obj);
			if((obj = get_eq_char(victim, WEAR_WRIST_R)) != 0)
				take_item(victim, obj);
			if((obj = get_eq_char(victim, WEAR_FINGER_R)) != 0)
				take_item(victim, obj);
			make_part(victim, "arm");
			sprintf(buf, "A supernatural force rips %s's arm off!", victim->name);
			act(buf, ch, 0, 0, TO_ROOM);
			send_to_char(buf, ch);
			return;
		}
		if(!IS_ARM_L(victim, LOST_ARM))
		{
			SET_BIT(victim->loc_hp[2], LOST_ARM);
			if((obj = get_eq_char(victim, WEAR_HOLD)) != 0)
				take_item(victim, obj);
			if((obj = get_eq_char(victim, WEAR_HANDS)) != 0)
				take_item(victim, obj);
			if((obj = get_eq_char(victim, WEAR_WRIST_L)) != 0)
				take_item(victim, obj);
			if((obj = get_eq_char(victim, WEAR_FINGER_L)) != 0)
				take_item(victim, obj);
			make_part(victim, "arm");
			sprintf(buf, "A supernatural force rips %s's arm off!", victim->name);
			act(buf, ch, 0, 0, TO_ROOM);
			send_to_char(buf, ch);
			return;
		}
		if(!IS_LEG_R(victim, LOST_LEG))
		{
			SET_BIT(victim->loc_hp[5], LOST_LEG);
			if((obj = get_eq_char(victim, WEAR_FEET)) != 0)
				take_item(victim, obj);
			if((obj = get_eq_char(victim, WEAR_LEGS)) != 0)
				take_item(victim, obj);
			make_part(victim, "leg");
			sprintf(buf, "A supernatural force rips %s's leg off!", victim->name);
			act(buf, ch, 0, 0, TO_ROOM);
			send_to_char(buf, ch);
			return;
		}
		if(!IS_LEG_L(victim, LOST_LEG))
		{
			SET_BIT(victim->loc_hp[4], LOST_LEG);
			if((obj = get_eq_char(victim, WEAR_FEET)) != 0)
				take_item(victim, obj);
			if((obj = get_eq_char(victim, WEAR_LEGS)) != 0)
				take_item(victim, obj);
			make_part(victim, "leg");
			sprintf(buf, "A supernatural force rips %s's leg off!", victim->name);
			act(buf, ch, 0, 0, TO_ROOM);
			send_to_char(buf, ch);
			return;
		}
	}
	send_to_char("You failed.\n\r", ch);

	return;
}

void do_putrefaction(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_THAN] < 2)
	{
		send_to_char("You must obtain at level 2 in Thanatosis to use Putrefaction.\n\r", ch);
		return;
	}


	if(arg[0] == '\0')
	{
		send_to_char("Who do you wish to use Putrefaction on?\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(IS_NPC(victim))
	{
		send_to_char("Why use Putrefaction on a mob?\n\r", ch);
		return;
	}

	if(IS_IMMORTAL(victim) && victim != ch)
	{
		send_to_char("You can only use Putrefaction on Avatar's or lower.\n\r", ch);
		return;
	}

	if(is_safe(ch, victim) == TRUE)
		return;

	if(victim->hit < victim->max_hit)
	{
		send_to_char("They are hurt and suspicious.\n\r", ch);
		return;
	}
	act("You close your eyes and concentrate on $N.", ch, 0, victim, TO_CHAR);
	act("$n closes $s eyes and concentrates on you.", ch, 0, victim, TO_VICT);
	act("$n closes $s eyes and cencentrates on $N.", ch, 0, victim, TO_NOTVICT);
	WAIT_STATE(ch, 12);
	if(number_percent() < 50)
	{
		send_to_char("You failed.\n\r", ch);
		return;
	}

	act("Your flesh begins to rot!", victim, 0, 0, TO_CHAR);
	act("$n's flesh begins to rot!", victim, 0, 0, TO_ROOM);
	SET_BIT(victim->flag2, AFF2_ROT);
	return;
}

void do_hagswrinkles(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_THAN] < 1)
	{
		send_to_char("You must obtain at least level 1 in Thanatosis to use Hagswrinkles.\n\r", ch);
		return;
	}

	if(arg[0] == '\0')
	{
		send_to_char("Change to look like whom?\n\r", ch);
		return;
	}

	if(IS_AFFECTED(ch, AFF_POLYMORPH) && !IS_VAMPAFF(ch, VAM_DISGUISED))
	{
		send_to_char("Not while polymorphed.\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if(IS_NPC(victim))
	{
		send_to_char("Not on NPC's.\n\r", ch);
		return;
	}

	if(IS_IMMORTAL(victim) && victim != ch)
	{
		send_to_char("You can only use hagswrinkles on Avatars or lower.\n\r", ch);
		return;
	}

	if(ch->pcdata->condition[COND_THIRST] < 40)
	{
		send_to_char("You have insufficient blood.\n\r", ch);
		return;
	}
	ch->pcdata->condition[COND_THIRST] -= number_range(30, 40);

	if(ch == victim)
	{
		if(!IS_AFFECTED(ch, AFF_POLYMORPH) && !IS_VAMPAFF(ch, VAM_DISGUISED))
		{
			send_to_char("You already look like yourself!\n\r", ch);
			return;
		}
		sprintf(buf, "Your body wrinkles and reshapes as %s.", ch->name);
		act(buf, ch, 0, victim, TO_CHAR);
		sprintf(buf, "%s's body wrinkles and reshapes as %s.", ch->morph, ch->name);
		act(buf, ch, 0, victim, TO_ROOM);
		REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
		REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_DISGUISED);
		free_string(ch->morph);
		ch->morph = str_dup("");
		return;
	}
	if(IS_VAMPAFF(ch, VAM_DISGUISED))
	{
		sprintf(buf, "Your body wrinkles and reshapes as %s.", victim->name);
		act(buf, ch, 0, victim, TO_CHAR);
		sprintf(buf, "%s's body wrinkles and reshapes as %s.", ch->morph, victim->name);
		act(buf, ch, 0, victim, TO_NOTVICT);
		sprintf(buf, "%s's body wrinkles and reshapes as you!", ch->morph);
		act(buf, ch, 0, victim, TO_VICT);
		free_string(ch->morph);
		ch->morph = str_dup(victim->name);
		return;
	}
	sprintf(buf, "Your body wrinkles and reshapes as %s.", victim->name);
	act(buf, ch, 0, victim, TO_CHAR);
	sprintf(buf, "%s's body wrinkles and reforms as %s.", ch->name, victim->name);
	act(buf, ch, 0, victim, TO_NOTVICT);
	sprintf(buf, "%s's body wrinkles and reforms as you!", ch->name);
	act(buf, ch, 0, victim, TO_VICT);
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_DISGUISED);
	free_string(ch->morph);
	ch->morph = str_dup(victim->name);
	return;
}


void do_fear(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		stc("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_DAIM] < 5)
	{
		stc("You must obtain level 5 Daimoinon to use Fear.\n\r", ch);
		return;
	}

	if((victim = get_char_room(ch, arg)) == 0)
	{
		stc("They aren't here.\n\r", ch);
		return;
	}

	arg[0] = UPPER(arg[0]);

	if(ch->fighting == 0)
	{
		stc("You must be fighting to use Fear.\n\r", ch);
		return;
	}

	if(victim->fighting == 0)
	{
		sprintf(buf, "%s is not fighting anyone.", arg);
		stc(buf, ch);
		return;
	}

	WAIT_STATE(ch, 16);

	if(IS_NPC(victim))
	{
		act("You bare your fangs and growl at $N.", ch, 0, victim, TO_CHAR);
		act("$n bares $s fangs and growls at you.", ch, 0, victim, TO_VICT);
		act("$n bares $s fangs and growls at $N, and $N flees in terror.", ch, 0, victim, TO_NOTVICT);
		do_flee(victim, "");
		return;
	}

	if(!IS_NPC(victim))
	{
		if(ch->pcdata->powers[VAM_DAIM] < 5)
		{
			if(number_range(1, 4) != 2)
			{
				act("You bare your fangs and growl at $N, but nothing happens.", ch, 0, victim, TO_CHAR);
				act("$n bares $s fangs and growls at you.", ch, 0, victim, TO_VICT);
				act("$n bares $s fangs and growls at $N, but nothing happens.", ch, 0, victim,
				    TO_NOTVICT);
				return;
			}
		}

		act("You bare your fangs and growl at $N.", ch, 0, victim, TO_CHAR);
		act("$n bares $s fangs and growls at you.", ch, 0, victim, TO_VICT);
		act("$n bares $s fangs and growls at $N, and $N flees in terror.", ch, 0, victim, TO_NOTVICT);
		do_flee(victim, "");
		return;
	}

	return;

}

void do_vtwist(CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_STRING_LENGTH];
	char arg2[MAX_STRING_LENGTH];
	OBJ_DATA *obj;

	argument = one_argument(argument, arg1);
	strcpy(arg2, argument);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(ch->pcdata->powers[VAM_DAIM] < 4)
	{
		send_to_char("You must obtain level 5 Daimoinon to use Twist.\n\r", ch);
		return;
	}

	if(arg1 == 0 || arg2 == 0)
	{
		send_to_char("Syntax: Dub (item) (description)?\n\r", ch);
		return;
	}

	if((obj = get_obj_carry(ch, arg1)) == 0)
	{
		send_to_char("You dont have that item.\n\r", ch);
		return;
	}

	if(strlen(arg2) > 40 || strlen(arg2) < 3)
	{
		send_to_char("From 3 to 40 characters please.\n\r", ch);
		return;
	}

	free_string(obj->name);
	obj->name = str_dup(arg2);
	free_string(obj->short_descr);
	obj->short_descr = str_dup(arg2);
	send_to_char("Ok.\n\r", ch);
	return;

}

void do_gourge(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	char buf[MAX_STRING_LENGTH];

	one_argument(argument, arg);

	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_MELP] < 2)
	{
		send_to_char("You need Thaumaturgy 8 to Gourge.\n\r", ch);
		return;
	}
	if(arg[0] == '\0')
	{
		send_to_char("Who do you wish to gourge?\n\r", ch);
		return;
	}
	if((victim = get_char_room(ch, arg)) == 0)
	{
		send_to_char("They arent here.\n\r", ch);
		return;
	}
	if(IS_NPC(victim) && IS_SET(victim->act, ACT_NOAUTOKILL))
	{
		send_to_char("You can't do that to them.\n\r", ch);
		return;
	}

	if(!IS_NPC(victim))
	{
		send_to_char("You cannot gourge a person.\n\r", ch);
		return;
	}
	if(victim->level > 30)
	{
		send_to_char("Only small creatures are defenceless enough to be gourged on.\n\r", ch);
		return;
	}
	sprintf(buf, "%s leaps toward %s baring his fangs.", ch->name, victim->name);
	act(buf, ch, 0, 0, TO_ROOM);
	sprintf(buf, "You leap toward %s baring your fangs.\n\r", victim->name);
	send_to_char(buf, ch);
	WAIT_STATE(ch, 15);

	send_to_char("You rip their throat out and gourge on the blood.\n\r", ch);
	sprintf(buf, "%s rips %s's throat out, gourging on all of their blood.", ch->name, victim->name);
	act(buf, ch, 0, 0, TO_ROOM);
	ch->pcdata->condition[COND_THIRST] += number_range(100, 200);
	if(ch->pcdata->condition[COND_THIRST] >= 1000 / ch->pcdata->stats[UNI_GEN]);
	{
		send_to_char("Your bloodlust is sated.\n\r", ch);
		ch->pcdata->condition[COND_THIRST] = 1000 / ch->pcdata->stats[UNI_GEN];
	}
	sprintf(buf, "%s body falls to the ground lifeless.", victim->name);
	send_to_char(buf, ch);
	act(buf, ch, 0, 0, TO_ROOM);
	raw_kill(victim);
	return;
}


void do_bloodwall(CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj = 0;
	OBJ_DATA *objc;
	char arg[MAX_STRING_LENGTH];
	char arg2[MAX_STRING_LENGTH];
	char wall[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];
	long value;

	argument = one_argument(argument, arg);
	argument = one_argument(argument, arg2);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_DAIM] < 2)
	{
		send_to_char("You need Daiminion 2 to call Blood Walls.\n\r", ch);
		return;
	}

	if(arg[0] == '\0' || (str_cmp(arg, "n")
			      && str_cmp(arg, "s")
			      && str_cmp(arg, "e") && str_cmp(arg, "w") && str_cmp(arg, "u") && str_cmp(arg, "d")))
	{
		send_to_char("You may wall n, w, s, e, d or u.\n\r", ch);
		return;
	}

	if(arg2[0] == '\0')
	{
		send_to_char("Please supply the amount of blood to expend.\n\r", ch);
		return;
	}

	value = is_number(arg2) ? atoi(arg2) : -1;

	if(value > 200 || value < 1)
	{
		send_to_char("Please select a value between 1 and 200.\n\r", ch);
		return;
	}

	ch->pcdata->condition[COND_THIRST] -= value;
	sprintf(buf, "A look of concentration passes over %s's face.", ch->name);
	act(buf, ch, 0, 0, TO_ROOM);
	send_to_char("A look of concentration passes over your face.\n\r", ch);
	if(!str_cmp(arg, "n"))
		sprintf(wall, "walln");
	if(!str_cmp(arg, "w"))
		sprintf(wall, "wallw");
	if(!str_cmp(arg, "s"))
		sprintf(wall, "walls");
	if(!str_cmp(arg, "e"))
		sprintf(wall, "walle");
	if(!str_cmp(arg, "d"))
		sprintf(wall, "walld");
	if(!str_cmp(arg, "u"))
		sprintf(wall, "wallu");
	objc = get_obj_list(ch, wall, ch->in_room->contents);
	if(objc != 0)
	{
		send_to_char("There is already a wall blocking that direction.\n\r", ch);
		return;
	}
	WAIT_STATE(ch, 25);
	sprintf(buf, "A wall of blood pours out of the ground.");
	act(buf, ch, 0, 0, TO_ROOM);
	send_to_char(buf, ch);
	if(!str_cmp(arg, "n"))
	{
		obj = create_object(get_obj_index(30043), 0);
		sprintf(buf, "A wall of blood");
		obj->short_descr = str_dup(buf);
	}
	if(!str_cmp(arg, "s"))
	{
		obj = create_object(get_obj_index(30044), 0);
		sprintf(buf, "A wall of blood");
		obj->short_descr = str_dup(buf);
	}
	if(!str_cmp(arg, "e"))
	{
		obj = create_object(get_obj_index(30045), 0);
		sprintf(buf, "A wall of blood");
		obj->short_descr = str_dup(buf);
	}
	if(!str_cmp(arg, "w"))
	{
		obj = create_object(get_obj_index(30046), 0);
		sprintf(buf, "A wall of blood");
		obj->short_descr = str_dup(buf);
	}
	if(!str_cmp(arg, "d"))
	{
		obj = create_object(get_obj_index(30047), 0);
		sprintf(buf, "A wall of blood");
		obj->short_descr = str_dup(buf);
	}
	if(!str_cmp(arg, "u"))
	{
		obj = create_object(get_obj_index(30048), 0);
		sprintf(buf, "A wall of blood");
		obj->short_descr = str_dup(buf);
	}

	obj_to_room(obj, ch->in_room);
	obj->timer = 5;
	obj->item_type = ITEM_WALL;
	return;

}


void do_guardian(CHAR_DATA * ch, char *argument)
{

	char buf[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	AFFECT_DATA af;

	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_DAIM] < 1)
	{
		send_to_char("You require level 1 Daimoinon to create a guardian.\n\r", ch);
		return;
	}

	if(ch->pcdata->followers > 5)
	{
		send_to_char("Nothing happens.\n\r", ch);
		return;
	}
	ch->pcdata->followers++;

	victim = create_mobile(get_mob_index(MOB_VNUM_GUARDIAN));
	victim->level = 200;
	victim->hit = 5000;
	victim->max_hit = 5000;
	victim->hitroll = 50;
	victim->damroll = 50;
	victim->armor = 300;
	SET_BIT(victim->act, ACT_NOEXP);

	strcpy(buf, "Come forth, creature of darkness, and do my bidding!");
	do_say(ch, buf);

	send_to_char("A demon bursts from the ground and bows before you.\n\r", ch);
	act("$N bursts from the ground and bows before $n.", ch, 0, victim, TO_ROOM);

	char_to_room(victim, ch->in_room);


	add_follower(victim, ch);
	af.duration = 666;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = AFF_CHARM;
	affect_to_char(victim, &af);
	return;

}

void do_gate(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	char arg[MAX_INPUT_LENGTH];

	one_argument(argument, arg);

	if(IS_NPC(ch))
		return;
	if(!IS_CLASS(ch, CLASS_VAMPIRE))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	if(ch->pcdata->powers[VAM_DAIM] < 3)
	{
		send_to_char("You require Daimoinon level 3 to gate.\n\r", ch);
		return;
	}
	if(arg[0] == '\0')
	{
		send_to_char("Who do you wish to gate to?\n\r", ch);
		return;
	}

	if((victim = get_char_world(ch, arg)) == 0)
	{
		send_to_char("They arent here.\n\r", ch);
		return;
	}


	if((victim == ch)
	   || victim->in_room == 0
	   || IS_NPC(victim)
	   || (!IS_NPC(victim) && !IS_IMMUNE(victim, IMM_SUMMON))
	   || IS_SET(ch->in_room->room_flags, ROOM_PRIVATE)
	   || IS_SET(ch->in_room->room_flags, ROOM_SOLITARY)
	   || IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
	   || IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
	   || IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
	   || IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL) || victim->in_room->vnum == ch->in_room->vnum)
	{
		send_to_char("You failed.\n\r", ch);
		return;
	}


	obj = create_object(get_obj_index(OBJ_VNUM_GATE), 0);
	obj->value[0] = victim->in_room->vnum;
	obj->value[3] = ch->in_room->vnum;
	obj->timer = 5;
	if(IS_AFFECTED(ch, AFF_SHADOWPLANE))
		obj->extra_flags = ITEM_SHADOWPLANE;
	obj_to_room(obj, ch->in_room);

	obj = create_object(get_obj_index(OBJ_VNUM_GATE), 0);
	obj->value[0] = ch->in_room->vnum;
	obj->value[3] = victim->in_room->vnum;
	obj->timer = 5;
	if(IS_AFFECTED(victim, AFF_SHADOWPLANE))
		obj->extra_flags = ITEM_SHADOWPLANE;
	obj_to_room(obj, victim->in_room);
	act("A look of concentration passes over $n's face.", ch, 0, 0, TO_ROOM);
	send_to_char("A look of concentration passes over your face.\n\r", ch);
	act("$p appears in front of $n in a blast of flames.", ch, obj, 0, TO_ROOM);
	act("$p appears in front of you in a blast of flames.", ch, obj, 0, TO_CHAR);
	act("$p appears in front of $n in a blast of flames.", victim, obj, 0, TO_ROOM);
	act("$p appears in front of you in a blast of flames.", ch, obj, victim, TO_VICT);
	return;
}

// THIS IS THE END OF THE FILE THANKS
