//  For.. Configuration.. To be... vague.

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "merc.h"
void do_configuration(CHAR_DATA * ch, char *argument)
{
	char arg[MIL];
	char buf[MSL];
	char buf2[MSL];
	char name[MSL];
	long mod;

	argument = one_argument(argument, arg);
	if(IS_NPC(ch))
		return;
	if(ch->in_room == 0)
		return;
	sprintf(name, "%s", ch->in_room->area->name);
	mod = ch->in_room->area->mod;
	if(arg[0] == '\0')

	{
		sprintf(buf2, "{r===|{b Configuration for %s {r|==={x\n\r", ch->in_room->area->name);
		centre_text(buf2, ch);
		if(mod > 0)
			sprintf(buf, "%s's mobiles are currently %liX their original strength.\n\r", name, mod);

		else if(mod < 0)
			sprintf(buf, "%s's mobiles are currently 1/%li their normal strength.\n\r", name, ((mod * -1) + 1));

		else
			sprintf(buf, "%s's mobiles are not modified.\n\r", name);
		centre_text(buf, ch);
		centre_text(buf2, ch);
		return;
	}
	if(!str_cmp(arg, "raise"))

	{
		sprintf(buf, "Raising mobiles for the area '%s'", name);
		stc(buf, ch);
		ch->in_room->area->mod++;
		return;
	}

	else if(!str_cmp(arg, "lower"))

	{
		sprintf(buf, "Lowering mobiles for the area '%s'", name);
		stc(buf, ch);
		ch->in_room->area->mod--;
		return;
	}

	else if(!str_cmp(arg, "normal"))

	{
		sprintf(buf, "Resetting mobiles for the area '%s'", name);
		stc(buf, ch);
		ch->in_room->area->mod = 0;
		return;
	}

	else

	{
		stc("Syntax: Configuration <lower|raise|normal>\n\r", ch);
		return;
	}
	return;
}


// THIS IS THE END OF THE FILE THANKS
