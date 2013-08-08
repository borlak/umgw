typedef struct kingdom_data KINGDOM_DATA;

#define KINGDOM_SCRY_COST	75
#define KINGDOM_RECALL_COST	100

struct kingdom_data
{
	KINGDOM_DATA *next;

	char *name;
	char *noansiname;
	char *leader;
	char *members;
	char *trustees;

	long *treaties;
	long *wars;
	long treaty_pending;
	long realpoints;
	long points;
	long pks;
	long pds;
	long assists;
	long id;
	long upkeep;
	long recall;
};

extern KINGDOM_DATA *kingdom_list;

// THIS IS THE END OF THE FILE THANKS
