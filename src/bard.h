/* bard header file, mostly song textx and definitions */

#define MAX_TEXT	10
#define TEXT_LOVE	0
#define TEXT_HATE	1
#define TEXT_BATTLE	2
#define TEXT_LAUGHTER	3
#define TEXT_DEFENSE	4
#define TEXT_CALM	5
#define TEXT_CHARMING	6
#define TEXT_FEAR	7
#define TEXT_ANIMAL	8
#define TEXT_SLEEP	9
struct bard_song_text
{
	char *lyric;
};
struct bard_song_text bard_songs[MAX_TEXT][37] = { {	/* love song, healing */
						    {"Fairies Love Song"},
						    {"Why should I sit and sigh"},
						    {"Broo and bracken, broo and bracken"},
						    {"Why should I sit and sigh"},
						    {"All alone and weary"},
						    {"When I see the plover rising "},
						    {"Or the curlew wheeling"},
						    {"It's then I'll court my mortal lover"},
						    {"Back to me is stealing"},
						    {"When the moon begins her waning"},
						    {"I sit by the water"},
						    {"Where a man born of the sunlight"},
						    {"Loved the Faerie's daughter"},
						    {"Oh, but there is something wanting"},
						    {"O but I am weary"},
						    {"Coming blithe, now bonny treads he"},
						    {"O'er the knolls to cheer me"},
						    {"end"}}, {	/* song of violence, hate/hurt song */
							       {"Sounds of Violence"},
							       {"Hello broadsword my old friend"},
							       {"I've come to fight with you again"},
							       {"Because the sounds of battle ringing"},
							       {"In my ears has me singing"},
							       {"And the rock that I have instead of a brain"},
							       {"Still remains"},
							       {"I love the sounds of violence"},
							       {"In tournaments I fight alone"},
							       {"I leave my melee gear at home"},
							       {"But when I go down to the Pennsic War"},
							       {"I often fight in groups of five or more"},
							       {"When my friend was stabbed by an Eastern spear in the head"},
							       {"He was dead"},
							       {"Touched by the sounds of violence"},
							       {"A thousand footmen waging war"},
							       {"A hundred archers maybe more"},
							       {"Polemen thrusting from the second row"},
							       {"Shieldmen dying, they're the first to go"},
							       {"Two-stick fighters can harry the enemy flank"},
							       {"They've got rank"},
							       {"And love the sounds of violence"},
							       {"Foolishly I pressed ahead"},
							       {"I'd be a hero or be dead"},
							       {"A belted fighter tried to teach me"},
							       {"With his polearm he might reach me"},
							       {"But my blows like violent hailstones fell"},
							       {"And struck well"},
							       {"Causing the sounds of violence"},
							       {"Eastrealm fighters fell and died"},
							       {"Before th'advancing Midrealm tide"},
							       {"And we shouted out our battle cry"},
							       {"We would conquer or we would die"},

							       {"And the bards sing the deeds of the fighters that bravely fall"},
							       {"And they all"},
							       {"Whisper the sounds of violence"}, {"end"}},
{				/* battle song */
 {"Bold Sir Robin"}, {"Bravely bold Sir Robin"}, {"Brought forth from Camelot"}, {"He was not afraid to die"},
 {"Bold, Brave Sir Robin"}, {"He was not at all afraid"}, {"To be killed in nasty ways"},
 {"Brave, brave, brave, brave Sir Robin"}, {"He was not in the least bit scared"},
 {"To be mashed into a pulp"}, {"Or to have his eyes gouged out"}, {"And his elbows broken"},
 {"To have his kneecaps split"}, {"And his body burned away"}, {"And his limbs all hacked and mangled"},
 {"Brave Sir Robin"}, {"His head smashed in and his heart cut out"},
 {"And his liver removed and his bowels unplugged"}, {"And his nostrils raped and his bottom burnt up"},
 {"And his penis.....  "}, {"end"}}, {	/* laughter song */
				      {"I Sing of Dead Bunnies"},
				      {"I sing of dead bunnies, and burnt baby chicks"},
				      {"Barbecued squirrels, and hamsters on sticks"},
				      {"Ducklings in blenders, and frogs off the road"},
				      {"Opossums on fenders and deep french-fried toad!"},
				      {"Sliced and diced sparrows, dead dogs on the lawn"},
				      {"Cats riddled with arrows, and disemboweled faun"},
				      {"Pickled canaries, and clubbed baby seals"},
				      {"Mice served in berries, and turtles 'neath wheels"},
				      {"Minced baby earwigs, koala fillet"},
				      {"Rat Pie with custard, and cockroach puree"},
				      {"Fred's little brother, and Mystery Beast:"},
				      {"These are the things that they served at the Feast!"}, {"end"}},
{				/* defense song */
 {"Ansteorran Fight Song"}, {"Hail, hail the Black and the Gold"}, {"We're gonna win because we're so bold"},
 {"Kick their faces in the mud"}, {"Stomp out their teeth and draw their blood"},
 {"We're gonna beat them, just wait and see"}, {"And we will win a great vic-to-ry"},
 {"And we'll teach them all to never"}, {"Take on the Black and Gold"}, {"end"}}, {	/* calm song */
										   {"The Bard's Lament"},

										   {"Long have I traveled"},

										   {"And long have I sang"},

										   {"My harp I have strummed"},

										   {"And my drum I have banged"},

										   {"I gather stories and songs to sing"},

										   {"Of knights and cowards, peasants and kings"},

										   {"So for some bread, meat and ale"},

										   {"I'll tell you songs that ne'er fail"},

										   {"And for some sweet smelling soup"},

										   {"I'll tell you tales of war and coups"},

										   {"And when my final songs is past"},

										   {"I'll leave your town or village fast"},
										   {"end"}},
{				/* charming song */
 {"The Unicorn"}, {"The unicorn is a wondrous beast"}, {"Bold, handsone brave and stron"},
 {"And upon its proud gentle head"}, {"It has a single spiral horn"}, {"Oh mighty beast, O unicorn"},
 {"Fill me with your light"}, {"And grant that I may hear your muse"}, {"And keep you in my isght"},
 {"Oh bold and lovely unicorn"}, {"Guide us with your horn"}, {"and teach us of the magick realms"},
 {"into which you have gone"}, {"Magick, magick, unicron"}, {"Please keep me in your grace"},
 {"and yould your like I ever see"}, {"I'll heartily embrace"}, {"end"}}, {	/* fear song */
									   {"Dragon Element Call"},

									   {"I call the Dragon from the Earth"},

									   {"To cast its strength around me"},

									   {"With its power of death and birth"},

									   {"I find the strength within me"},

									   {"I call the Dragon from the Air"},
									   {"To cast its love around me"},

									   {"With its beauty, both wan and fair"},
									   {"I find the love within me"},

									   {"I call the Dragon from the Fire"},

									   {"To cast its spark around me"},

									   {"With its warmth and with desire"},
									   {"I find the spark within me"},

									   {"I call the Dragon from the Sea"},

									   {"To cast its health around me"},

									   {"With its power and with the fey"},

									   {"I find the health within me"},
									   {"end"}},
{				/* animal song */
 {"Animal Elemental Call"}, {"Powers and magick of the East"}, {"Raven, flying on wings of Air"},
 {"My circle calls you to descend"}, {"Alone or in a pair"}, {"By the power of Air"}, {"So mote it be!"},
 {"Powers and magick of the South"}, {"Dragon, breathing streams of Fire"}, {"My circle calls you to descend"},
 {"On music of drum and lyre"}, {"By the power of Fire"}, {"So mote it be!"},
 {"Powers and magick of the West"}, {"Dolphins swimming in waves of Water"},
 {"My circle calls you to descend"}, {"With light, love and laughter"}, {"By the power of Water"},
 {"So mote it be!"}, {"Powers and magick of the North"}, {"Wolf, howling to mother Earth"},
 {"My circle calls you to descend"}, {"From your northern mountains of Earth"}, {"By the power of Earth"},
 {"So mote it be!"}, {"end"}}, {	/* sleep song */
				{"Sleep Prayer"}, {"Now I lay me down to rest"},
				{"I pray tomorrow I'll do my best"}, {"If I should err in any way"},
				{"Let me learn from it anyway"}, {"So mote it be."}, {"end"}}
};


// THIS IS THE END OF THE FILE THANKS
