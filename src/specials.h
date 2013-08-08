
/********This is all the special function defines and structures*********
 * Some defines are for rooms, some for objs, mobs, etc. but i dont want*
 * To keep defining new ones for each kind of structure, so in the spec *
 * code use a simple switch() to do what you want with the updates      *
 ************************************************************************/

#define UPDATE_ALL		0
#define UPDATE_ROOM		1
#define UPDATE_OBJ		2
#define UPDATE_MOB		3
#define UPDATE_END		4
#define UPDATE_TIMER		5
#define UPDATE_FIGHTING		6
#define UPDATE_DAMAGE		7
#define UPDATE_ENTERED		8
#define UPDATE_EXITED		9
#define UPDATE_GIVE		10
#define UPDATE_SAY		11
#define UPDATE_LOOK		12
typedef struct spec_obj_data SPEC_OBJ_DATA;


// THIS IS THE END OF THE FILE THANKS
