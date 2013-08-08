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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"



void do_affects(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	AFFECT_DATA *paf;

	if(IS_NPC(ch))
		return;
	if(ch->affected == 0)
	{
		send_to_char("You have no spells affecting you at this time.\n\r", ch);
	}
	if(ch->affected != 0)
	{

		send_to_char("You are affected by:\n\r", ch);
		for(paf = ch->affected; paf != 0; paf = paf->next)
		{
			sprintf(buf, "Spell: '%s'", skill_table[paf->type].name);
			send_to_char(buf, ch);
			if(ch->level >= 0)
			{
				sprintf(buf, " modifies %s by %li for %li hours with bits %s.\n\r",
					affect_loc_name(paf->location),
					paf->modifier, paf->duration, affect_bit_name(paf->bitvector));
				send_to_char(buf, ch);
			}

		}
	}
	send_to_char("Combined affects of spells and equipment:\n\r", ch);
	sprintf(buf, "%s.\n\r", affect_bit_name(ch->affected_by));
	send_to_char(buf, ch);
	return;

}

void do_katana(CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_HIGHLANDER))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(60 > ch->practice)
	{
		send_to_char("It costs 60 points of primal to create a katana.\n\r", ch);
		return;
	}
	ch->practice -= 60;
	obj = create_object(get_obj_index(29695), 0);
	if(IS_SET(obj->quest, QUEST_ARTIFACT))
		REMOVE_BIT(obj->quest, QUEST_ARTIFACT);
	obj_to_char(obj, ch);
	act("A katana appears in your hands in a flash of light.", ch, 0, 0, TO_CHAR);
	act("A katana appears in $n's hands in a flash of light.", ch, 0, 0, TO_ROOM);
	return;
}

void do_level(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	char skill[MAX_STRING_LENGTH];
	ROOM_INDEX_DATA *home;

	one_argument(argument, arg);


	if(IS_NPC(ch))
		return;
	if(arg[0] == '\0')
	{
		send_to_char("{W--------------------------------------------------------------------------------{x\n\r",
			     ch);
		send_to_char("                               {W -={Y Experience {W=-{x\n\r", ch);
		send_to_char("{W--------------------------------------------------------------------------------{x\n\r",
			     ch);
		sprintf(buf, "Viper   : %3li.   Crane  : %3li.   Crab  : %3li.   Mongoose : %3li.   Bull    : %3li.\n\r",
			ch->stance[1], ch->stance[2], ch->stance[3], ch->stance[4], ch->stance[5]);
		send_to_char(buf, ch);
		sprintf(buf,
			"Mantis  : %3li.   Dragon : %3li.   Tiger : %3li.   Monkey   : %3li.   Swallow : %3li.\n\r",
			ch->stance[6], ch->stance[7], ch->stance[8], ch->stance[9], ch->stance[10]);
		send_to_char(buf, ch);
		send_to_char("{W--------------------------------------------------------------------------------{x\n\r",
			     ch);
		sprintf(buf, "Unarmed : %3li.   Slice  : %3li.   Stab  : %3li.   Slash    : %3li.   Whip    : %3li.\n\r",
			ch->wpn[0], ch->wpn[1], ch->wpn[2], ch->wpn[3], ch->wpn[4]);
		send_to_char(buf, ch);
		sprintf(buf,
			"Claw    : %3li.   Blast  : %3li.   Pound : %3li.   Crush    : %3li.   Grep    : %3li.\n\r",
			ch->wpn[5], ch->wpn[6], ch->wpn[7], ch->wpn[8], ch->wpn[9]);
		send_to_char(buf, ch);
		sprintf(buf, "Bite    : %3li.   Pierce : %3li.   Suck  : %3li.\n\r", ch->wpn[10], ch->wpn[11], ch->wpn[12]);
		send_to_char(buf, ch);
		send_to_char("{W--------------------------------------------------------------------------------{x\n\r",
			     ch);
		sprintf(buf,
			"{MPurple{x  : %3li.  {R Red {x   : %3li.  {B Blue {x : %3li.  {G Green {x   : %3li. {Y  Yellow{x  : %3li.\n\r",
			ch->spl[0], ch->spl[1], ch->spl[2], ch->spl[3], ch->spl[4]);
		send_to_char(buf, ch);
		send_to_char("{W--------------------------------------------------------------------------------{x\n\r",
			     ch);
		send_to_char("{CMiscellaneous things{x:\n\r", ch);
		if((home = get_room_index(ch->home)) != 0)
		{
			sprintf(buf, "{WRecall Room{x: %s.   {WVnum{x: %li                    \n\r", home->name, ch->home);
			send_to_char(buf, ch);
		}
		sprintf(buf, "{WAutostance{x: %s \n\r", ch->pcdata->autostance);
		send_to_char(buf, ch);
		sprintf(buf, "{WExit{x: %s %s {xeast.\n\r", ch->name, ch->pcdata->exit);
		send_to_char(buf, ch);
		sprintf(buf, "{WEnter{x:  %s arrives%s{x\n\r", ch->name, ch->pcdata->enter);
		send_to_char(buf, ch);
		sprintf(buf, "{WLogin{x: Info->{B(Login){X%s %s\n\r", ch->name, ch->pcdata->login);
		send_to_char(buf, ch);
		sprintf(buf, "{WLogout{x: Info->{R(Logout){X%s %s\n\r", ch->name, ch->pcdata->logout);
		send_to_char(buf, ch);

		send_to_char("{W--------------------------------------------------------------------------------{x\n\r",
			     ch);
		send_to_char("{YClass things{x:\n\r", ch);
		if(IS_CLASS(ch, CLASS_DEMON))
			send_to_char("You are a demon.                       \n\r", ch);
		if(IS_CLASS(ch, CLASS_WEREWOLF))
			send_to_char("You are a werewolf.                    \n\r", ch);
		if(IS_CLASS(ch, CLASS_NINJA))
			send_to_char("You are a ninja.                    \n\r", ch);
		if(IS_CLASS(ch, CLASS_VAMPIRE))
			send_to_char("You are a vampire.                     \n\r", ch);
		if(IS_CLASS(ch, CLASS_HIGHLANDER))
			send_to_char("You are a highlander.                  \n\r", ch);
		if(IS_CLASS(ch, CLASS_DROW))
			send_to_char("You are a drow.                     \n\r", ch);

		if(IS_CLASS(ch, CLASS_VAMPIRE))
		{

			sprintf(skill, "Generation : %li                       \n\r", ch->pcdata->stats[UNI_GEN]);
			send_to_char(skill, ch);
			if(ch->pcdata->rank == AGE_METHUSELAH)
				send_to_char("You are a Methuselah.                 \n\r", ch);
			else if(ch->pcdata->rank == AGE_ELDER)
				send_to_char("You are an Elder.                     \n\r", ch);
			else if(ch->pcdata->rank == AGE_ANCILLA)
				send_to_char("You are an Ancilla.                   \n\r", ch);
			else if(ch->pcdata->rank == AGE_NEONATE)
				send_to_char("You are a Neonate!                    \n\r", ch);
			else
				send_to_char(" You are a Childe.                     \n\r", ch);
			if(IS_SET(ch->special, SPC_PRINCE))
				send_to_char("You are a prince.                     \n\r", ch);
		}

		if(IS_CLASS(ch, CLASS_NINJA))
		{
			sprintf(skill, "Generation : %li\n\r", ch->pcdata->stats[UNI_GEN]);
			send_to_char(skill, ch);
			if(IS_SET(ch->special, SPC_PRINCE))
				send_to_char("You can discipline people!\n\r", ch);
		}

		if(IS_CLASS(ch, CLASS_WEREWOLF))
		{
			sprintf(skill, "Generation : %li                         \n\r", ch->pcdata->stats[UNI_GEN]);
			send_to_char(skill, ch);
			if(IS_SET(ch->special, SPC_SIRE) && ch->pcdata->stats[UNI_GEN] > 2)
				send_to_char("You are able to spread the gift!        \n\r", ch);
			if(IS_SET(ch->special, SPC_PRINCE) && ch->pcdata->stats[UNI_GEN] > 2)
				send_to_char("You are a shaman!                   \n\r", ch);
		}
		send_to_char("{W--------------------------------------------------------------------------------{x\n\r",
			     ch);
		send_to_char("See also 'EXP HELP' and 'EXP LEVELS'.\n\r", ch);
		send_to_char("{W--------------------------------------------------------------------------------{x\n\r",
			     ch);
		return;
	}
	if(!str_cmp(arg, "levels"))
	{
		send_to_char("--------------------------------------------------------------------------------\n\r", ch);
		send_to_char("                                -= Experience =-\n\r", ch);
		send_to_char("--------------------------------------------------------------------------------\n\r", ch);
		send_to_char("[EXP]        [STANCE LEVEL]         [WEAPON LEVEL]         [MAGIC LEVEL]\n\r", ch);
		send_to_char("  0          Completely unskilled.  Totally unskilled.     Untrained.\n\r", ch);
		send_to_char("  1 - 25     Apprentice.            Slightly skilled.      Apprentice.\n\r", ch);
		send_to_char(" 26 - 50     Trainee.               Reasonable.            Student.\n\r", ch);
		send_to_char(" 51 - 75     Student.               Fairly competent.      Scholar.\n\r", ch);
		send_to_char(" 76 - 100    Fairly experienced.    Highly skilled.        Magus.\n\r", ch);
		send_to_char("101 - 125    Well trained.          Very dangerous.        Adept.\n\r", ch);
		send_to_char("126 - 150    Highly experienced.    Extremely deadly.      Mage.\n\r", ch);
		send_to_char("151 - 175    Expert.                Expert.                Warlock.\n\r", ch);
		send_to_char("176 - 199    Master.                Master.                Master wizard.\n\r", ch);
		send_to_char("      200    Grand master.          Grand master.          Grand sorcerer.\n\r", ch);
		send_to_char("--------------------------------------------------------------------------------\n\r", ch);
		send_to_char("See also 'EXP HELP' and 'EXP LEVELS'.\n\r", ch);
		send_to_char("--------------------------------------------------------------------------------\n\r", ch);
		return;
	}
	if(!str_cmp(arg, "help"))
	{
		send_to_char("--------------------------------------------------------------------------------\n\r", ch);
		send_to_char("                                -= Experience =-\n\r", ch);
		send_to_char("--------------------------------------------------------------------------------\n\r", ch);
		send_to_char("[STANCES] The first 5 stances are basic stances, while the others are advanced\n\r", ch);
		send_to_char("stances.  In order to learn an advanced stance you must get 200 exp in two of\n\r", ch);
		send_to_char("the appropriate basic stances (see 'help advancedstance' for details).\n\r", ch);
		send_to_char("--------------------------------------------------------------------------------\n\r", ch);
		send_to_char("[WEAPON SKILLS] Each skill improves the chance of hitting, damage, number\n\r", ch);
		send_to_char("of attacks and parrying ability with that particular weapon.  Unarmed \n\r", ch);
		send_to_char("combat skill will also improve your ability to dodge incoming attacks.\n\r", ch);
		send_to_char("--------------------------------------------------------------------------------\n\r", ch);
		send_to_char("[MAGICAL ABILITY] The greater your ability in a field of magic, the \n\r", ch);
		send_to_char("greater the power of any spells you cast from that field.  Mages are able\n\r", ch);
		send_to_char("to get up to 240 exp in each type of magic.  Others are limited to 200 exp.\n\r", ch);
		send_to_char("--------------------------------------------------------------------------------\n\r", ch);
		send_to_char("See also 'EXP HELP' and 'EXP LEVELS'.\n\r", ch);
		send_to_char("--------------------------------------------------------------------------------\n\r", ch);
		return;
	}
	return;
}

void do_class(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if(IS_NPC(ch))
		return;

	if(arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char("Syntax: class <char> <class>.\n\r", ch);
		send_to_char("      Classes:\n\r", ch);
		send_to_char("None, ", ch);
		send_to_char("Demon, ", ch);
		send_to_char("Demon Lord , ", ch);
		send_to_char("Purple Mage, Blue Mage, Yellow Mage, Green Mage, Red Mage, ", ch);
		send_to_char("Werewolf, ", ch);
		send_to_char("Vampire, ", ch);
		send_to_char("Highlander, ", ch);
		return;
	}

	if((victim = get_char_world(ch, arg1)) == 0)
	{
		send_to_char("That player is not here.\n\r", ch);
		return;
	}

	if(IS_NPC(victim))
	{
		send_to_char("Not on NPC's.\n\r", ch);
		return;
	}

	if(!str_cmp(arg2, "none"))
	{
		/* Used to make sure the person has enough "stuff" to change */
		victim->pcdata->condition[COND_THIRST] = 10000;
		victim->move = 10000;
		victim->mana = 10000;

		victim->pcdata->powers[WPOWER_BEAR] = 4;
		victim->pcdata->powers[WPOWER_LYNX] = 4;
		victim->pcdata->powers[WPOWER_BOAR] = 4;
		victim->pcdata->powers[WPOWER_OWL] = 4;
		victim->pcdata->powers[WPOWER_SPIDER] = 4;
		victim->pcdata->powers[WPOWER_WOLF] = 4;
		victim->pcdata->powers[WPOWER_HAWK] = 4;
		victim->pcdata->powers[WPOWER_MANTIS] = 4;
		victim->pcdata->powers[WPOWER_TIGER] = 4;
		victim->pcdata->powers[WPOWER_GAIA] = 4;

		if(IS_CLASS(victim, CLASS_WEREWOLF) || IS_CLASS(victim, CLASS_VAMPIRE))
			do_unwerewolf(victim, "");
		if(IS_VAMPAFF(victim, VAM_DISGUISED))
			do_mask(victim, "self");
		victim->pcdata->stats[UNI_FORM0] = 0;

/* All classes in general */
		if(IS_VAMPAFF(victim, VAM_FANGS))
		{
			send_to_char("Your fangs slide back into your gums.\n\r", victim);
			act("$n's fangs slide back into $s gums.", ch, 0, victim, TO_ROOM);
			REMOVE_BIT(victim->pcdata->stats[UNI_AFF], VAM_FANGS);
		}
		if(IS_CLASS(victim, CLASS_VAMPIRE) && IS_VAMPAFF(victim, VAM_CLAWS))
		{
			send_to_char("Your claws slide back under your nails.\n\r", victim);
			act("$N's claws slide back under $s nails.", ch, 0, victim, TO_ROOM);
			REMOVE_BIT(victim->pcdata->stats[UNI_AFF], VAM_CLAWS);
		}
		else if(IS_CLASS(victim, CLASS_NINJA) && IS_VAMPAFF(victim, VAM_CLAWS))
		{
			send_to_char("You shove your iron claws up your sleeves\n\r", victim);
			act("$N shoves $S iron claws up $e sleeves.", ch, 0, victim, TO_ROOM);
			REMOVE_BIT(victim->pcdata->stats[UNI_AFF], VAM_CLAWS);
		}
		else if(IS_CLASS(victim, CLASS_WEREWOLF) && IS_VAMPAFF(victim, VAM_CLAWS))
		{
			send_to_char("Your talons slide back into your fingers.\n\r", victim);
			act("$N's talons slide back into $s fingers.", ch, 0, victim, TO_ROOM);
			REMOVE_BIT(victim->pcdata->stats[UNI_AFF], VAM_CLAWS);
		}

		if(IS_VAMPAFF(victim, VAM_NIGHTSIGHT))
		{
			send_to_char("The red glow in your eyes fades.\n\r", victim);
			act("The red glow in $N's eyes fades.", ch, 0, victim, TO_ROOM);
			REMOVE_BIT(victim->pcdata->stats[UNI_AFF], VAM_NIGHTSIGHT);
		}
		if(IS_AFFECTED(victim, AFF_SHADOWSIGHT))
		{
			send_to_char("You can no longer see between planes.\n\r", victim);
			REMOVE_BIT(victim->affected_by, AFF_SHADOWSIGHT);
		}

		if(IS_IMMUNE(victim, IMM_SHIELDED))
		{
			send_to_char("You stop shielding your aura.\n\r", victim);
			REMOVE_BIT(victim->immune, IMM_SHIELDED);
		}

		if(IS_VAMPAFF(victim, VAM_DISGUISED))
		{
			free_string(victim->morph);
			victim->morph = str_dup(victim->name);
			send_to_char("You transform into yourself.\n\r", victim);
			REMOVE_BIT(victim->pcdata->stats[UNI_AFF], VAM_DISGUISED);
		}

		if(IS_AFFECTED(victim, AFF_SHADOWPLANE))
		{
			send_to_char("You fade back into the real world.\n\r", victim);
			act("The shadows flicker and $N fades into existance.", ch, 0, victim, TO_ROOM);
			REMOVE_BIT(victim->affected_by, AFF_SHADOWPLANE);
			do_look(ch, "auto");
		}

		if(IS_SET(victim->act, PLR_WIZINVIS))
		{
			REMOVE_BIT(victim->act, PLR_WIZINVIS);
			send_to_char("You slowly fade into existence.\n\r", victim);
		}

		if(IS_SET(victim->act, PLR_HOLYLIGHT))
		{
			REMOVE_BIT(victim->act, PLR_HOLYLIGHT);
			REMOVE_BIT(victim->act, PLR_DROWSIGHT);
			send_to_char("Your senses return to normal.\n\r", victim);
		}

/* Demon Stuff */
		if(IS_DEMAFF(victim, DEM_HORNS) && IS_CLASS(victim, CLASS_DEMON))
		{
			send_to_char("Your horns slide back into your head.\n\r", victim);
			act("$N's horns slide back into $s head.", ch, 0, victim, TO_ROOM);
			REMOVE_BIT(victim->pcdata->powers[DPOWER_CURRENT], DEM_HORNS);
		}

		if(IS_DEMAFF(victim, DEM_HOOVES) && IS_CLASS(victim, CLASS_DEMON))
		{
			send_to_char("Your hooves transform into feet.\n\r", victim);
			act("$N's hooves transform back into $s feet.", ch, 0, victim, TO_ROOM);
			REMOVE_BIT(victim->pcdata->powers[DPOWER_CURRENT], DEM_HOOVES);
		}

		if(IS_DEMAFF(victim, DEM_WINGS) && IS_CLASS(victim, CLASS_DEMON))
		{
			if(IS_DEMAFF(victim, DEM_UNFOLDED) && IS_CLASS(victim, CLASS_DEMON))
			{
				send_to_char("Your wings fold up behind your back.\n\r", victim);
				act("$N's wings fold up behind $s back.", ch, 0, victim, TO_ROOM);
				REMOVE_BIT(victim->pcdata->powers[DPOWER_CURRENT], DEM_UNFOLDED);
			}
			send_to_char("Your wings slide into your back.\n\r", victim);
			act("$N's wings slide into $s back.", ch, 0, victim, TO_ROOM);
			REMOVE_BIT(victim->pcdata->powers[DPOWER_CURRENT], DEM_WINGS);
		}

		if(IS_EXTRA(victim, EXTRA_OSWITCH))
		{
			do_humanform(victim, "");
		}

		REMOVE_BIT(victim->special, SPC_CHAMPION);
		REMOVE_BIT(victim->special, SPC_INCONNU);
		REMOVE_BIT(victim->special, SPC_ANARCH);
		REMOVE_BIT(victim->special, SPC_SIRE);
		REMOVE_BIT(victim->special, SPC_PRINCE);
		REMOVE_BIT(victim->special, SPC_DEMON_LORD);
		REMOVE_BIT(victim->special, SPC_DROW_WAR);
		REMOVE_BIT(victim->special, SPC_DROW_MAG);
		REMOVE_BIT(victim->special, SPC_DROW_CLE);
		REMOVE_BIT(victim->affected_by, AFF_POLYMORPH);
		REMOVE_BIT(victim->pcdata->stats[UNI_AFF], VAM_CHANGED);
		REMOVE_BIT(victim->pcdata->powers[WOLF_POLYAFF], POLY_MIST);
		REMOVE_BIT(victim->affected_by, AFF_ETHEREAL);
		REMOVE_BIT(victim->pcdata->stats[UNI_AFF], VAM_FLYING);
		REMOVE_BIT(victim->pcdata->stats[UNI_AFF], VAM_SONIC);
		REMOVE_BIT(victim->pcdata->powers[WOLF_POLYAFF], POLY_BAT);
		REMOVE_BIT(victim->pcdata->powers[WOLF_POLYAFF], POLY_WOLF);

		victim->pcdata->powers[DPOWER_FLAGS] = 0;
		victim->pcdata->stats[DEMON_TOTAL] = 0;
		victim->pcdata->stats[DEMON_CURRENT] = 0;
		victim->pcdata->powers[WPOWER_BEAR] = 0;
		victim->pcdata->powers[WPOWER_LYNX] = 0;
		victim->pcdata->powers[WPOWER_BOAR] = 0;
		victim->pcdata->powers[WPOWER_OWL] = 0;
		victim->pcdata->powers[WPOWER_SPIDER] = 0;
		victim->pcdata->powers[WPOWER_WOLF] = 0;
		victim->pcdata->powers[WPOWER_HAWK] = 0;
		victim->pcdata->powers[WPOWER_MANTIS] = 0;
		victim->pcdata->powers[WPOWER_TIGER] = 0;
		victim->pcdata->powers[WPOWER_GAIA] = 0;
		victim->pcdata->powers[VAM_ANIM] = 0;
		victim->pcdata->powers[VAM_OBTE] = 0;
		victim->pcdata->powers[VAM_THAU] = 0;
		victim->pcdata->powers[VAM_OBFU] = 0;
		victim->pcdata->powers[VAM_SERP] = 0;
		victim->pcdata->powers[VAM_CHIM] = 0;
		victim->pcdata->powers[VAM_NECR] = 0;
		victim->pcdata->powers[VAM_CELE] = 0;
		victim->pcdata->powers[VAM_PRES] = 0;
		victim->pcdata->powers[VAM_AUSP] = 0;
		victim->pcdata->powers[VAM_POTE] = 0;
		victim->pcdata->powers[VAM_VICI] = 0;
		victim->pcdata->powers[VAM_OBEA] = 0;
		victim->pcdata->powers[VAM_FORT] = 0;
		victim->pcdata->powers[VAM_QUIE] = 0;
		victim->pcdata->powers[VAM_DOMI] = 0;
		victim->pcdata->powers[VAM_PROT] = 0;
		victim->pcdata->powers[VAM_DAIM] = 0;
		victim->pcdata->powers[VAM_MELP] = 0;
		victim->pcdata->powers[VAM_THAN] = 0;
		victim->pcdata->powers[NPOWER_SORA] = 0;
		victim->pcdata->powers[NPOWER_CHIKYU] = 0;
		victim->pcdata->powers[NPOWER_NINGENNO] = 0;
		victim->pcdata->powers[1] = 0;
		victim->pcdata->condition[COND_THIRST] = 0;
		victim->move = victim->max_move;
		victim->mana = victim->max_mana;
		victim->hit = victim->max_hit;
		victim->pcdata->rank = 0;

		victim->pcdata->stats[UNI_RAGE] = 0;
		free_string(victim->lord);
		victim->lord = str_dup("");
		free_string(victim->clan);
		victim->clan = str_dup("");
		victim->pcdata->stats[UNI_AFF] = 0;
		victim->pcdata->stats[UNI_CURRENT] = -1;
		victim->beast = 15;

		victim->pcdata->class = 0;
		victim->trust = 0;
		victim->level = 2;
		send_to_char("You are classless now!\n\r", victim);
	}
	else if(!str_cmp(arg2, "demon"))
	{
		victim->pcdata->class = 1;
		victim->level = 3;
		SET_BIT(victim->special, SPC_CHAMPION);
		victim->lord = str_dup(ch->name);
		send_to_char("You are now a demon!\n\r", victim);
	}
	else if(!str_cmp(arg2, "demon lord"))
	{
		victim->pcdata->class = 1;
		victim->level = 3;
		SET_BIT(victim->special, SPC_DEMON_LORD);
		victim->lord = str_dup(victim->name);
		send_to_char("You are now a Demon Lord!\n\r", victim);
	}
	else if(!str_cmp(arg2, "werewolf"))
	{
		victim->pcdata->class = 4;
		victim->level = 3;
		send_to_char("You are now a werewolf!\n\r", victim);
	}
	else if(!str_cmp(arg2, "vampire"))
	{
		victim->pcdata->class = 8;
		victim->level = 3;
		send_to_char("You are now a vampire!\n\r", victim);
	}
	else if(!str_cmp(arg2, "highlander"))
	{
		victim->pcdata->class = 16;
		victim->level = 3;
		send_to_char("You are now a highlander!\n\r", victim);
	}
	else if(!str_cmp(arg2, "ninja"))
	{
		victim->pcdata->class = 128;
		if(victim->trust > 6)
		{
			victim->trust = victim->trust;
		}
		else
		{
			victim->trust = 6;
		}
		victim->level = 6;
		send_to_char("You are now a ninja!\n\r", victim);
	}
	else if(!str_cmp(arg2, "monk"))
	{
		victim->pcdata->class = 512;
		if(victim->trust > 6)
		{
			victim->trust = victim->trust;
		}
		else
		{
			victim->trust = 6;
		}
		victim->level = 6;
		send_to_char("You are now a monk!\n\r", victim);
	}
	else if(!str_cmp(arg2, "drow"))
	{
		victim->pcdata->class = 32;
		send_to_char("You are now a drow!\n\r", victim);
	}
	else if(!str_cmp(arg2, "paladin"))
	{
		victim->pcdata->class = 256;
		send_to_char("You are now a paladin!\n\r", victim);
	}
	else
	{
		send_to_char("Syntax: class <char> <class>.\n\r", ch);
		send_to_char("      Classes:\n\r", ch);
		send_to_char("None, ", ch);
		send_to_char("Demon, ", ch);
		send_to_char("Demon Lord , ", ch);
		send_to_char("Purple Mage, Blue Mage, Yellow Mage, Green Mage, Red Mage, ", ch);
		send_to_char("Werewolf, ", ch);
		send_to_char("Vampire, ", ch);
		send_to_char("Ninja, ", ch);
		send_to_char("Monk, ", ch);
		send_to_char("Paladin, ", ch);
		send_to_char("Highlander, ", ch);
		send_to_char("Drow. ", ch);
		return;
	}
	send_to_char("Class Set.\n\r", ch);
	return;
}

void do_generation(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	long gen;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if(IS_NPC(ch))
		return;

	if(arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char("Syntax: generation <char> <generation>.\n\r", ch);
		send_to_char("Generation 1 is a Master <Class> and 2 is clan leader.\n\r", ch);
		return;
	}

	if((victim = get_char_world(ch, arg1)) == 0)
	{
		send_to_char("That player is not here.\n\r", ch);
		return;
	}

	gen = is_number(arg2) ? atoi(arg2) : -1;

	send_to_char("Generation Set.\n\r", ch);
	victim->pcdata->stats[UNI_GEN] = gen;
	return;
}
void reset_weapon(CHAR_DATA * ch, long dtype)
{
	if(ch->wpn[dtype] > 200)
		ch->wpn[dtype] = 200;
	return;
}

void reset_spell(CHAR_DATA * ch, long dtype)
{
	if(ch->spl[dtype] > 200)
		ch->spl[dtype] = 200;
	return;
}


void do_reimburse(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char strsave[MAX_STRING_LENGTH];
	char *word;
	FILE *fp;
	bool fMatch;

	if(IS_SET(ch->flag2, AFF2_REIMB))
	{
		stc("You already have a pfile.\n\r", ch);
		return;
	}

	sprintf(strsave, "%s%s", BACKUP_DIR, ch->name);
	if((fp = fopen(strsave, "r")) == 0)
	{
		send_to_char("You dont have a reimbursement file.\n\r", ch);
		return;
	}

	if((fp = fopen(strsave, "r")) != 0)
	{


		for(;;)
		{
			word = feof(fp) ? "End" : fread_word(fp);
			fMatch = FALSE;

			switch (UPPER(word[0]))
			{
			case '*':
				fMatch = TRUE;
				fread_to_eol(fp);
				break;

			case 'H':
				if(!str_cmp(word, "HpManaMove"))
				{
					ch->hit = fread_number(fp);
					ch->max_hit = fread_number(fp);
					ch->mana = fread_number(fp);
					ch->max_mana = fread_number(fp);
					ch->move = fread_number(fp);
					ch->max_move = fread_number(fp);
					send_to_char("***Hp/Mana/Move Reimbursed.\n\r", ch);
					fMatch = TRUE;
					break;
				}
				break;



			case 'P':
				if(!str_cmp(word, "Primal"))
				{
					ch->practice = fread_number(fp);
					fMatch = TRUE;
					send_to_char("***Primal reimbursed.\n\r", ch);
					break;
				}
				if(!str_cmp(word, "PkPdMkMd"))
				{
					ch->pkill = fread_number(fp);
					ch->pdeath = fread_number(fp);
					ch->mkill = fread_number(fp);
					ch->mdeath = fread_number(fp);
					send_to_char("***Player/Mob Deaths/Kills restored.\n\r", ch);
					fMatch = TRUE;
					break;
				}
				break;

			case 'S':



				if(!str_cmp(word, "Spells"))
				{
					ch->spl[0] = fread_number(fp);
					ch->spl[1] = fread_number(fp);
					ch->spl[2] = fread_number(fp);
					ch->spl[3] = fread_number(fp);
					ch->spl[4] = fread_number(fp);
					send_to_char("***Spell level reimbursed.\n\r", ch);
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
					send_to_char("***Stances reimbursed.\n\r", ch);
					break;
				}


				break;

			case 'E':

				if(!str_cmp(word, "End"))
				{
					send_to_char("***Reimbursed.. Player saved.\n\r", ch);
					send_to_char("***If you havnt noticed, There was a pwipe.\n\r", ch);
					send_to_char
						("***We have reimbursed what we could but you will have to earn the rest.\n\r",
						 ch);
					send_to_char("***Classes will be passed out again.-Osiris\n\r", ch);
					ch->level = 3;
					ch->trust = 3;
					ch->exp += 250000;
					do_autosave(ch, "");
					SET_BIT(ch->flag2, AFF2_REIMB);
					return;
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
					send_to_char("Weapons reimbursed.\n\r", ch);
					fMatch = TRUE;
					break;
				}
				break;
			}

			if(!fMatch)
			{
				sprintf(buf, "Fread_char: no match. WORD: %s", word);
				fread_to_eol(fp);
			}

		}
	}
	return;
}


void set_switchname(CHAR_DATA * ch, char *title)
{
	char buf[MAX_STRING_LENGTH];

	if(IS_NPC(ch))
	{
		bug("Set_switchname: NPC.", 0);
		return;
	}
	strcpy(buf, title);
	free_string(ch->pcdata->switchname);
	ch->pcdata->switchname = str_dup(buf);
	return;
}

void set_pc_name(CHAR_DATA * ch, char *title)
{
	char buf[MAX_STRING_LENGTH];

	if(IS_NPC(ch))
	{
		bug("Set_pc_name: NPC.", 0);
		return;
	}
	strcpy(buf, title);
	free_string(ch->name);
	ch->name = str_dup(buf);
	return;
}

void do_ninjaarmor(CHAR_DATA * ch, char *argument)
{
	OBJ_INDEX_DATA *pObjIndex;
	OBJ_DATA *obj;
	char arg[MAX_INPUT_LENGTH];
	long vnum = 0;

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(!IS_CLASS(ch, CLASS_NINJA))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if(arg[0] == '\0')
	{
		send_to_char("Please specify which piece of ninja armor you wish to make: Cloak Mask Dagger.\n\r", ch);
		return;
	}

	if(ch->practice < 60)
	{
		send_to_char("It costs 60 points of primal to create ninja equipment.\n\r", ch);
		return;
	}
	if(!str_cmp(arg, "cloak"))
		vnum = 29701;
	else if(!str_cmp(arg, "mask"))
		vnum = 29702;
	else if(!str_cmp(arg, "dagger"))
		vnum = 29700;
	else
	{
		send_to_char("Please specify which piece of ninja armor you wish to make: Cloak Mask Dagger.\n\r", ch);
		return;
	}

	if(vnum == 0 || (pObjIndex = get_obj_index(vnum)) == 0)
	{
		send_to_char("Missing object, please inform a God.\n\r", ch);
		return;
	}
	ch->practice -= 60;
	obj = create_object(pObjIndex, 50);
	obj_to_char(obj, ch);
	SET_BIT(obj->spectype, SITEM_NINJA);
	act("$p appears in your hands.", ch, obj, 0, TO_CHAR);
	act("$p appears in $n's hands.", ch, obj, 0, TO_ROOM);
	return;
}

void do_magearmor(CHAR_DATA * ch, char *argument)
{
	OBJ_INDEX_DATA *pObjIndex;
	OBJ_DATA *obj;
	char arg[MAX_INPUT_LENGTH];
	long vnum = 0;

	argument = one_argument(argument, arg);

	if(IS_NPC(ch))
		return;

	if(arg[0] == '\0')
	{
		send_to_char("Please specify which piece of mage armor you wish to make: Cape Mask Girth.\n\r", ch);
		return;
	}

	if(ch->practice < 60)
	{
		send_to_char("It costs 60 points of primal to create mage equipment.\n\r", ch);
		return;
	}

	if(!str_cmp(arg, "mask"))
		vnum = 2650;
	else if(!str_cmp(arg, "girth"))
		vnum = 2651;
	else if(!str_cmp(arg, "cape"))
		vnum = 2653;
	else
	{
		send_to_char("Please specify which piece of mage armor you wish to make: Cape Mask Girth.\n\r", ch);
		return;
	}

	if(vnum == 0 || (pObjIndex = get_obj_index(vnum)) == 0)
	{
		send_to_char("Missing object, please inform Noid.\n\r", ch);
		return;
	}
	ch->practice -= 60;
	obj = create_object(pObjIndex, 50);
	obj_to_char(obj, ch);
	act("$p appears in your hands.", ch, obj, 0, TO_CHAR);
	act("$p appears in $n's hands.", ch, obj, 0, TO_ROOM);
	return;
}


// THIS IS THE END OF THE FILE THANKS
