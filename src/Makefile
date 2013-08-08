CC      = gcc
PROF    =
NOCRYPT = 
C_FLAGS = -O0 -Wextra -Wall -g3 -ggdb3 -Dlinux $(PROF) $(NOCRYPT)
L_FLAGS = -O0 $(PROF) 

O_FILES = \
          act_comm.o  \
          act_info.o  \
          act_move.o  \
          act_obj.o   \
          act_wiz.o   \
          bard.o      \
	  bounty.o    \
          clan.o      \
          comm.o      \
          config.o    \
          const.o     \
          db.o        \
          demon.o     \
          dempowers.o \
          drow.o      \
          fight.o     \
          gui.o       \
          handler.o   \
          hotreboot.o \
          interp.o    \
          kav_info.o  \
          kav_fight.o \
          kav_wiz.o   \
          kingdom.o   \
          magic.o     \
          map.o       \
          monk.o      \
          ninja.o     \
          os.o        \
          ping.o      \
          room.o      \
          rot_wiz.o   \
          sage.o      \
          save.o      \
          specials.o  \
          swordtech.o \
          tournament.o\
          trivia.o    \
          update.o    \
          vamp.o      \
          vote.o      \
          wizutil.o   


merc: $(O_FILES)
	rm -f merc
	$(CC) $(L_FLAGS) -o merc -lcrypt $(O_FILES)
	chmod g+w merc
	chmod g+w $(O_FILES)
        
.c.o: merc.h
	$(CC) -c $(C_FLAGS) $<

tar:
	tar czhf ../umgodwars.tgz *.[ch] Makefile godup

tarall:
	tar czhf ../umgodwarsfull.tgz ../*

rm:
	cp *.[ch] bak
	rm *.[cho] *.dsw *.dsp

clean:
	touch *.[ch]
	make


