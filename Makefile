# makefile for gemdropx

# Gem Drop X

# by Bill Kendrick
# bill@newbreedsoftware.com
# http://www.newbreedsoftware.com/

# Atari version: August 17, 1997 - September 24, 1997
# X Window version: November 5, 1997 - November 21, 1997
# X Window Updates: July 29, 1998- March 28, 1999
# 100% SDL Version: December 27, 1999 - January 26, 2000
# Qt/Embedded Port: February 11, 2002 - February 12, 2002
# Added 'install' target (finally!): May 18, 2002


DATA_PREFIX=
# SDL_CFLAGS := $(shell sdl-config --cflags)
# SDL_LDFLAGS := $(shell sdl-config --libs)
CC=gcc
CXX=gcc


MIXER=-lSDL_mixer
NOSOUNDFLAG=$(SOUND)SOUND
JOYFLAG=$(JOY)JOYSTICK
EMBEDDEDFLAG=NOT_EMBEDDED


CFLAGS=-arch $(ARCH) -Wall $(SDL_CFLAGS) \
	-DDATA_PREFIX=\"$(DATA_PREFIX)\" -D$(NOSOUNDFLAG) -D$(JOYFLAG) \
	-D$(EMBEDDEDFLAG) $(XTRA_FLAGS)
SDL_LIB=$(MIXER) $(SDL_LDFLAGS)
CQML_LIB=-L./cqml -lcqml -L/opt/Qtopia/lib -lqpe -lqte
#CQML_LIB=-L/opt/Qtopia/lib -lqpe -lqte
GFX_LIB=$(SDL_LIB)


all:	gemdropx

install:
	cp gemdropx /usr/local/bin/
	chmod 755 /usr/local/bin/gemdropx
	-mkdir -p $(DATA_PREFIX)
	chmod 755 $(DATA_PREFIX)
	cp -R data/* $(DATA_PREFIX)
	chmod -R a+rX $(DATA_PREFIX)/*

nosound:
	make MIXER= NOSOUNDFLAG=NOSOUND JOY=$(JOY)

#embedded:
#	cd cqml ; make
#	make MIXER= NOSOUNDFLAG=NOSOUND EMBEDDEDFLAG=EMBEDDED JOY=NO \
#		XTRA_FLAGS=-I/opt/Qtopia/include CC=g++ \
#		GFX_LIB="$(CQML_LIB)" SDL_CFLAGS=

clean:
	-rm gemdropx
	-rm *.o


gemdropx:	gemdropx.o
	$(CXX) $(CFLAGS) -o gemdropx gemdropx.o msputils.m $(GFX_LIB) -lm


gemdropx.o:	gemdropx.c
