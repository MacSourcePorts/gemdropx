/*
  gemdropx.c
  
  Gem Drop for SDL (X-Window, MSWindows, MacOS, BeOS, etc.) and Qt/Embedded
  
  v.0.9
  
  based directly on GEMDROP.ACT, the original Atari 8-bit version of Gem Drop
  
  by Bill Kendrick
  gemdropx@newbreedsoftware.com
  http://www.newbreedsoftware.com/gemdropx/
  
  Atari version: August 17, 1997 - Sept. 24, 1997
  X Window version: November 5, 1997 - November 21, 1997
  X Window update: July 29, 1998 - April 4, 1999
  SDL mixer fixes: October 31, 1999
  100% SDL version: December 27, 1999
  SDL fixes: December 29, 1999 - January 26, 2000
  Sound tweaks: February 11, 2002
  Qt/Embedded Support: February 11, 2002 - February 12, 2002
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

#ifndef EMBEDDED
#include <SDL.h>
#else
#include <qtimer.h>
#include <qpainter.h>
#include <qpe/qpeapplication.h>
#include <qwidget.h>
#include <qobject.h>
#endif

#ifndef NOSOUND
#include <SDL_mixer.h>
#endif

#ifndef EMBEDDED
#include "data/images/nothing.xbm"
#include "data/images/nothing-mask.xbm"
#include "data/images/left.xbm"
#include "data/images/left-mask.xbm"
#include "data/images/right.xbm"
#include "data/images/right-mask.xbm"
#include "data/images/action.xbm"
#include "data/images/action-mask.xbm"
#endif


/* Misc. value defines: */

#define NO 0
#define YES 1


/* Game stuff: */

#define FPS 20
#define HEIGHT 11
#define WIDTH 10
#define NUM_EXPLOSIONS 100
#define STICKWAIT 2


/* Explosion type: */

typedef struct explosion_type {
  int exist;     /* do I exist? */
  int x, y;      /* position */
  int anim;      /* animation frame counter */
  int animrate;  /* pause for animation */
} explosion_type;


#ifdef EMBEDDED
typedef struct SDL_Rect { int x, y, w, h; } SDL_Rect;
typedef struct SDL_Surface { int w, h; QPixmap * image; } SDL_Surface;
#define Uint8 unsigned char
#define Uint32 unsigned long
#endif


/* Object specifications: */

#define NUM_NORMAL_BLOCKS 4
#define NUM_ALL_BLOCKS 8
#define BOMB NUM_ALL_BLOCKS + 1
#define CLOCK NUM_ALL_BLOCKS + 2
#define WILDCARD NUM_ALL_BLOCKS + 3
#define NUM_SPECIALS 3
#define NUM_EXPLOSION_GFX 4
#define EXPLOSION_SLOW_VAL 1
#define HAPPY_GFX NUM_ALL_BLOCKS + NUM_SPECIALS + NUM_EXPLOSION_GFX + 1
#define SAD_GFX HAPPY_GFX + 1
#define WIN1_GFX SAD_GFX + 1
#define WIN2_GFX WIN1_GFX + 1
#define PATIENCE_GFX WIN2_GFX + 1
#define WARNING_GFX PATIENCE_GFX + 1
#define BRICK_GFX WARNING_GFX + 4
#define TITLE_GFX BRICK_GFX + 1
#define CREDITS_GFX TITLE_GFX + 1
#define LEVEL_GFX CREDITS_GFX + 1
#define LINES_GFX LEVEL_GFX + 1
#define NEED_GFX LINES_GFX + 1
#define NUMBERS_GFX NEED_GFX + 1
#define PAUSED_GFX NUMBERS_GFX + 1
#define SCORE_GFX PAUSED_GFX + 1
#define AUTHOR_GFX SCORE_GFX + 1
#define UHOH_GFX AUTHOR_GFX + 1
#define CONGRATULATIONS_GFX UHOH_GFX + 1
#define NUM_OBJECTS CONGRATULATIONS_GFX


/* Object (graphics) filenames: */

char * object_filenames[NUM_OBJECTS] = {"block", "yellowgem",
					"diamond", "disc",
					"dot", "bluegem", "fuji", "triangle",
					"bomb", "stopwatch", "wildcard",
					"explode0", "explode1",
					"explode2", "explode3",
					"happy", "sad", "win1", "win2",
					"patience", "warning1", "warning2",
					"warning3", "warning4", "brick",
					"title", "credits", "level", "lines",
					"need", "numbers", "paused", "score",
					"author", "uhoh", "congratulations"};


/* Sounds: */

enum {
  SND_BEGIN,
  SND_BOMB,
  SND_CLOCK,
  SND_GAMEOVER,
  SND_GOT,
  SND_LEVELSELECT,
  SND_WILDCARD,
  SND_WIN,
  SND_GRAB,
  SND_THROW,
  SND_BUZZER,
  SND_APPLAUSE,
  NUM_SOUNDS
};

/* Sound filenames: */

char * sound_filenames[NUM_SOUNDS] = {
  "begin", "bomb", "clock", "gameover", "got", "levelselect",
  "wildcard", "win", "grab", "throw", "buzzer", "applause"
};

enum {
  MOD_TITLE,
  MOD_GAME1,
  MOD_GAME2,
  MOD_GAME3,
  NUM_MODS
};

#define NUM_GAME_MODS 3

char * music_filenames[NUM_MODS] = {
  "hr_suds.xm", "22drops.mod", "citron.mod", "2force.xm"
};

#ifndef NOSOUND
Mix_Chunk * samples[NUM_SOUNDS];
Mix_Music * songs[NUM_MODS];
#endif


#ifndef EMBEDDED

/* Cursors: */

enum {
  CURSOR_NOTHING,
  CURSOR_LEFT,
  CURSOR_RIGHT,
  CURSOR_ACTION,
  NUM_CURSORS
};

Uint8 * cursor_bitmap[NUM_CURSORS] = {
  nothing_bits, left_bits, right_bits, action_bits
};

Uint8 * cursor_bitmask[NUM_CURSORS] = {
  nothing_mask_bits, left_mask_bits, right_mask_bits, action_mask_bits
};

#endif


#ifdef EMBEDDED
#define SDLK_ESCAPE 27
#define SDLK_a 'a'
#define SDLK_k 'k'
#define SDLK_l 'l'
#define SDLK_p 'p'
#define SDLK_q 'q'
#define SDLK_z 'z'
#define SDLK_RETURN 10
#define SDLK_SPACE 32
#define SDLK_LAST 999
#define SDLK_RIGHT '<'
#define SDLK_LEFT '>'
#define SDLK_UP '^'
#define SDLK_DOWN 'v'
#endif


/* Globals: */

explosion_type explosions[NUM_EXPLOSIONS];
int blocks[HEIGHT][WIDTH], killsx[100], killsy[100];
int scorevals[15];
int level, gameover, frozen, carrying, howmany, leveldone, _warning,
  whichexplosion, lines, firstround, linesneeded, happy, score, playerx,
  use_sound, use_joystick, max_level, clicks, no_music, game_playing;


int windowwidth, windowheight;


SDL_Surface * window;
SDL_Surface * object_pixmaps[NUM_OBJECTS];

#ifndef EMBEDDED
SDL_Cursor * cursor[NUM_CURSORS];
SDL_Event event;
SDLKey key;
SDL_Joystick * js;
#else

class MainWindow: public QWidget
{
//      Q_OBJECT
        private:
                QPixmap* buffer;
                QPainter* bufpainter;
                QColor palette[2];

                int (*timerhandler)();
                int (*keypresshandler)(int);
                int (*keyreleasehandler)(int);

        protected:
                void timerEvent(QTimerEvent*);
                void keyPressEvent(QKeyEvent*);
                void keyReleaseEvent(QKeyEvent*);

        public:
                MainWindow(const char* name);
                void setTimerhandler(int (*timerhandler)(void), int ms = 15);
                void setKeypresshandler(int (*keypresshandler)(int));
                void setKeyreleasehandler(int (*keyreleasehandler)(int));

                void setBgColor(Q_UINT32 color);
                void setFgColor(Q_UINT32 color);

                void plot(int x, int y);
                void flush();

                void blit(QPixmap* image, int x, int y);
};

MainWindow::MainWindow(const char* name):
        QWidget(0, name, WType_TopLevel)
{
        // Setup the buffer
        buffer = new QPixmap(240, 320);
        bufpainter = new QPainter(buffer);

        // Initialize variables
        setBgColor(0x000000);
        setFgColor(0xffffff);

        // Initialize handlers
        timerhandler = NULL;
        keypresshandler = NULL;
        keyreleasehandler = NULL;

        // Setup the window
        resize(230, 280);
        setCaption(name);
        bufpainter->eraseRect(0, 0, 240, 320);
}

void MainWindow::setTimerhandler(int (*th)(void), int ms)
{
        timerhandler = th;                  // Install the timer handler
        startTimer(ms);
}

void MainWindow::setKeypresshandler(int (*kph)(int))
{
        keypresshandler = kph;              // Install the keypress handler
}

void MainWindow::setKeyreleasehandler(int (*krh)(int))
{
        keyreleasehandler = krh;            // Install the keypress handler
}

void MainWindow::timerEvent(QTimerEvent*)
{
        timerhandler();
}


void MainWindow::keyPressEvent(QKeyEvent* e)
{
        if(!e->isAutoRepeat() && keypresshandler)
        {
                keypresshandler(e->key());
        }
}


void MainWindow::keyReleaseEvent(QKeyEvent* e)
{
        if(!e->isAutoRepeat() && keyreleasehandler)
        {
                keyreleasehandler(e->key());
        }
}


void MainWindow::setBgColor(Q_UINT32 color)
{
        palette[0] = QColor((color >> 16 & 0xff), (color >> 8) & 0xff, color & 0xff);
        bufpainter->setBackgroundColor(palette[0]);
}


void MainWindow::setFgColor(Q_UINT32 color)
{
        palette[1] = QColor((color >> 16 & 0xff), (color >> 8) & 0xff, color & 0xff);
        bufpainter->setPen(palette[1]);
}


void MainWindow::plot(int x, int y)
{
        bufpainter->drawPoint(x, y);
}


void MainWindow::flush()
{
        QPainter p(this);
        p.drawPixmap(0, 0, *buffer);
}


void MainWindow::blit(QPixmap* image, int x, int y)
{
        bufpainter->drawPixmap(x, y, *image);
}



QPEApplication * qapp;
MainWindow * mw;
int key;
#endif


#ifdef EMBEDDED
int SDL_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect,
		    SDL_Surface *dst, SDL_Rect *dstrect)
{
  mw->blit(src->image, dstrect->x, dstrect->y);
  return(0);
}

int SDL_UpdateRect(SDL_Surface *screen, int x, int y, int w, int h)
{
  printf("$$ SDL_UpdateRect()\n");
  return(0);
}

Uint32 SDL_GetTicks(void)
{
  struct timeval tv;
  
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 100000 + tv.tv_usec) / 1000;
}

void SDL_Delay(Uint32 ms)
{
  usleep(ms * 1000);
}

SDL_Surface * SDL_LoadBMP(const char * file)
{
  SDL_Surface * surf;
  
  surf = (SDL_Surface *) malloc(sizeof(SDL_Surface));
  
  surf->image = new QPixmap();
  surf->image->load(file);
  surf->w = (surf->image)->width();
  surf->h = (surf->image)->height();
  return(surf);
}

char * SDL_GetError(void)
{
  printf("$$ SDL_GetError()\n");
  return ("???");
}

void SDL_Quit(void)
{
  exit(1);
}
#endif


int my_FillRect(SDL_Surface *dst, SDL_Rect *dstrect,
		Uint8 r, Uint8 g, Uint8 b)
{
#ifndef EMBEDDED
  return (SDL_FillRect(dst, dstrect,
		       SDL_MapRGB(dst->format, r, g, b)));
#else
  printf("$$ my_FillRect()\n");
  return(0);
#endif
}


void playsound(int which)
{
#ifndef NOSOUND
  int i, found;
 
  if (use_sound)
  {
    found = -1;
 
    for (i = 0; i < MIX_CHANNELS; i++)
      {
        if (!Mix_Playing(i))
	  found = i;
      }
  
    if (found == -1)
      found = rand() % MIX_CHANNELS;
  
    Mix_PlayChannel(found, samples[which], 0);
    /* Mix_Volume(found, 8); */
  }
#endif
}


/* Beep: (was flash and beep on the Atari) */

void complain(void)
{
  playsound(SND_BUZZER);
}


/* Clear the screen: */

void erasewindow(void)
{
  SDL_Rect dest;
  
  dest.x = 0;
  dest.y = 0;
  dest.w = windowwidth;
  dest.h = windowheight;
  
  my_FillRect(window, &dest, 0x00, 0x00, 0x00);
  SDL_UpdateRect(window, dest.x, dest.y, dest.w, dest.h);
}


/* Setup the application: */

void setup(int argc, char * argv[])
{
  int i;
  char file[128];
#ifndef EMBEDDED
  int j;
  Uint8 temp_bitmap[32], temp_bitmask[32];
  Uint8 b;
  SDL_Surface * image;
#endif
  SDL_Rect dest;

  
#ifndef EMBEDDED
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
      fprintf(stderr, "Couldn't initialize SDL video.\n%s\n",
              SDL_GetError());
      exit(1);
    }
  
  use_joystick = 1;
  if (SDL_Init(SDL_INIT_JOYSTICK) < 0)
    {
      fprintf(stderr, "Warning - Couldn't initialize SDL joystick.\n%s\n",
	      SDL_GetError());
      use_joystick = 0;
    }
  
  
  /* Set key repeat: */
  
  SDL_EnableKeyRepeat(100, 30);
  
  
  /* Open joystick: */
  
  if (use_joystick)
    {
      if (SDL_NumJoysticks() <= 0)
	{
	  fprintf(stderr, "Warning - No joysticks.\n");
	  use_joystick = 0;
	}
    }
  
  if (use_joystick)
    {
      js = SDL_JoystickOpen(0);
      if (js == NULL)
	{
	  fprintf(stderr, "Warning - Can't open joystick 1.\n%s\n",
		  SDL_GetError());
	  use_joystick = 0;
	}
    }
  
  
  /* Check for proper joystick config: */
  
  if (use_joystick)
    {
      if (SDL_JoystickNumAxes(js) < 1)
	{
	  fprintf(stderr, "Joystick doesn't have enough axes!\n");
	  use_joystick = 0;
	}
    }

  if (use_joystick)
    {
      if (SDL_JoystickNumButtons(js) < 2)
	{
	  fprintf(stderr, "Joystick doesn't have enough buttons!\n");
	  use_joystick = 0;
	}
    }
#else
  qapp = new QPEApplication(argc, argv);
  mw = new MainWindow("Gem Drop Z");
#endif
  
  
  /* Set the size of the window: */
 
#ifndef EMBEDDED
  windowwidth = WIDTH * 48;
  windowheight = (HEIGHT + 2) * 48;
#else
  windowwidth = 240;
  windowheight = 320;
#endif
  

  /* Open window: */

#ifndef EMBEDDED
  window = SDL_SetVideoMode(windowwidth, windowheight, 16, SDL_HWSURFACE);
  SDL_WM_SetCaption("Gem Drop X!", "Gem Drop X");
#else
  printf("$$ setup() - open window\n");
#endif
  
  
#ifndef EMBEDDED
  /* Make cursors: */
  
  for (i = 0; i < NUM_CURSORS; i++)
    {
      for (j = 0; j < 32; j++)
	{
	  b = cursor_bitmap[i][j];
	  
	  temp_bitmap[j] = (((b & 0x01) << 7) |
			    ((b & 0x02) << 5) |
			    ((b & 0x04) << 3) |
			    ((b & 0x08) << 1) |
			    ((b & 0x10) >> 1) |
			    ((b & 0x20) >> 3) |
			    ((b & 0x40) >> 5) |
			    ((b & 0x80) >> 7));
	  
	  b = cursor_bitmask[i][j];
	  
	  temp_bitmask[j] = (((b & 0x01) << 7) |
			     ((b & 0x02) << 5) |
			     ((b & 0x04) << 3) |
			     ((b & 0x08) << 1) |
			     ((b & 0x10) >> 1) |
			     ((b & 0x20) >> 3) |
			     ((b & 0x40) >> 5) |
			     ((b & 0x80) >> 7));
	}
      
      cursor[i] = SDL_CreateCursor(temp_bitmap, temp_bitmask,
				   16, 16, 8, 8);
    }
#endif
  
  
  /* Load the object graphics: */
  
  for (i = 0; i < NUM_OBJECTS; i++)
    {
      sprintf(file, "%s/images/%s.bmp", DATA_PREFIX, object_filenames[i]);
  
#ifndef EMBEDDED      
      image = SDL_LoadBMP(file);
      
      if (image == NULL)
	{
	  fprintf(stderr, "%s: can't load: %s\n", file, SDL_GetError());
	  SDL_Quit();
	  exit(1);
	}

      object_pixmaps[i] = SDL_DisplayFormat(image);      
      SDL_FreeSurface(image);
#else
      object_pixmaps[i] = SDL_LoadBMP(file);
      
      if (object_pixmaps[i] == NULL)
	{
	  fprintf(stderr, "%s: can't load: %s\n", file, SDL_GetError());
	  SDL_Quit();
	  exit(1);
	}
#endif
      
      
      
      if (i == PATIENCE_GFX)
	{
	  dest.x = (windowwidth - 96) / 2;
	  dest.y = (windowheight - 16) / 2;
	  dest.w = 96;
	  dest.h = 16;
	 
	  SDL_BlitSurface(object_pixmaps[PATIENCE_GFX - 1], NULL,
			  window, &dest);
	  SDL_UpdateRect(window, dest.x, dest.y, dest.w, dest.h);
	}	  
    }
  
  
  /* Initial level: */
  
  level = 1;
  
  
  /* Reset explosions: */
  
  for (i = 0; i < NUM_EXPLOSIONS; i++)
    {
      explosions[i].exist = NO;
    }
  
  
  /* Init. score value table: */
  
  scorevals[0] = 0;
  scorevals[1] = 0;
  scorevals[2] = 0;
  scorevals[3] = 10;
  scorevals[4] = 20;
  scorevals[5] = 50;
  scorevals[6] = 100;
  scorevals[7] = 150;
  scorevals[8] = 200;
  scorevals[9] = 250;
  scorevals[10] = 300;
  scorevals[11] = 500;
  scorevals[12] = 1000;
  scorevals[13] = 1000;
  scorevals[14] = 5000;
  
  
  /* Reset random number generator: */
  
  srand(SDL_GetTicks());
  
  
  /* Setup sound: */
 
  use_sound = 1;

#ifndef NOSOUND
  if (use_sound)
  {
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
      {
        fprintf(stderr, "Warning: Couldn't initialize SDL sound.\n%s\n",
                SDL_GetError());
        use_sound = 0;
      }
  }
 
  if (use_sound)
  {
    if (Mix_OpenAudio(44100, AUDIO_S8, 2, 512) < 0)
      {
        fprintf(stderr,
                "Warning: Couldn't set 11025 Hz 8-bit stereo audio.\n%s\n",
                SDL_GetError());
	use_sound = 0;
      }
  }


  if (use_sound)
  {
    /* Load sound files: */
  
    for (i = 0; i < NUM_SOUNDS; i++)
      {
        sprintf(file, "%s/sounds/%s.wav", DATA_PREFIX, sound_filenames[i]);
        samples[i] = Mix_LoadWAV(file);
        if (samples[i] == NULL)
	  {
	    fprintf(stderr, "Couldn't load %s: %s\n", file, SDL_GetError());
	    SDL_Quit();
	    exit(1);
	  }
      }


    /* Load music files: */
  
    if (no_music == 0)
      {
        for (i = 0; i < NUM_MODS; i++)
	  {
	    sprintf(file, "%s/sounds/%s", DATA_PREFIX, music_filenames[i]);
	    songs[i] = Mix_LoadMUS(file);
	    if (songs[i] == NULL)
	      {
	        fprintf(stderr, "Couldn't load %s: %s\n", file, SDL_GetError());
	        SDL_Quit();
	        exit(1);
	      }
	  }
      }
    }
#endif
}


/* Display a number using images: */

void shownumber(int num, int x, int y)
{
  int i;
  char temp[15];
  SDL_Rect src, dest;
  
  num = (num % 100000);
  
  sprintf(temp, "%d     ", num);
  
  for (i = 0; i < 5; i++)
    {
      dest.x = x + i * 16;
      dest.y = y;
      dest.w = 16;
      dest.h = 16;
      
      if (temp[i] >= '0' && temp[i] <= '9')
	{
	  src.x = (temp[i] - '0') * 16;
	  src.y = 0;
	  src.w = 16;
	  src.h = 16;
	  
	  SDL_BlitSurface(object_pixmaps[NUMBERS_GFX - 1], &src,
			  window, &dest);
	}
      else
	{
	  my_FillRect(window, &dest, 0x00, 0x00, 0x00);
	}
      
      SDL_UpdateRect(window, dest.x, dest.y, dest.w, dest.h);
    }
}


/* Update the score in the window: */

void drawscore()
{
  SDL_Rect dest;
  
  dest.x = 0;
  dest.y = (HEIGHT + 2) * 48 - 16;
  dest.w = 48;
  dest.h  =16;
  
  SDL_BlitSurface(object_pixmaps[SCORE_GFX - 1], NULL,
		  window, &dest);
  SDL_UpdateRect(window, dest.x, dest.y, dest.w, dest.h);
  
  shownumber(score, 48, (HEIGHT + 2) * 48 - 16);
}


/* Pick a random block: */

int randblock(void)
{
  int i;
  
  
  /* Pick one of the main four pieces in level 1-14: */
  
  i = (rand() % NUM_NORMAL_BLOCKS) + 1;
  
  
  /* Pick one of all eight pieces in levels 15-20: */
  
  if (level >= 15)
     i = rand() % (NUM_ALL_BLOCKS + 1);
  
  
  /* Maybe pick a non-gem object: */
  
  if ((rand() % 40) < 1)
     i = (rand() % NUM_SPECIALS) + NUM_ALL_BLOCKS + 1;
  
  return(i);
}


/* Draw an object graphic somewhere in the window: */

void drawblockgraphic(int x, int y, int c)
{
  SDL_Rect dest;
  
  dest.x = x;
  dest.y = y;
  dest.w = 48;
  dest.h = 48;
  
  if (c != 0)
    {
      SDL_BlitSurface(object_pixmaps[c - 1], NULL,
		      window, &dest);
    }
  else
    {
      my_FillRect(window, &dest, 0x00, 0x00, 0x00);
    }
  
  SDL_UpdateRect(window, dest.x, dest.y, dest.w, dest.h);
}


/* Update the status icons at the bottom: */

void drawstatus()
{
  if (frozen != 0)
    drawblockgraphic((WIDTH / 2) * 48, (HEIGHT + 1) * 48, CLOCK);
  else
    drawblockgraphic((WIDTH / 2) * 48, (HEIGHT + 1) * 48, 0);
  
  if (_warning != 0)
    drawblockgraphic((WIDTH / 2) * 48 + 48, (HEIGHT + 1) * 48, WARNING_GFX +
		     (clicks % 4));
  else
    drawblockgraphic((WIDTH / 2) * 48 + 48, (HEIGHT + 1) * 48, 0);    
}


/* Draw the player (it's two colors): */

void drawmangraphic(int x, int y, int c)
{
  SDL_Rect dest;
  
  if (c != 0)
    {
      dest.x = x;
      dest.y = y;
      dest.w = 48;
      dest.h = 48;
      
      SDL_BlitSurface(object_pixmaps[c - 1], NULL,
		      window, &dest);
      SDL_UpdateRect(window, dest.x, dest.y, dest.w, dest.h);
    }
}


/* Draw a block where it belongs: */

void drawblock(int x, int y)
{
  int c;
  
  
  /* Draw the block at (x, y): */
  
  c = blocks[y][x];
  
  drawblockgraphic(x * 48, y * 48, c);
}


/* Update the level number display: */

void updatelevel(void)
{
  SDL_Rect dest;
  
  dest.x = 0;
  dest.y = (HEIGHT + 2) * 48 - 16 - 16;
  dest.w = 48;
  dest.h = 16;
  
  SDL_BlitSurface(object_pixmaps[LEVEL_GFX - 1], NULL,
		  window, &dest);
  SDL_UpdateRect(window, dest.x, dest.y, dest.w, dest.h);
  
  shownumber(level, 48, (HEIGHT + 2) * 48 - 16 - 16);
}


/* Update the lines / needed lines displays: */

void updatelines(void)
{
  SDL_Rect dest;
  
  dest.x = WIDTH * 48 - 48 - (16 * 5);
  dest.y = (HEIGHT + 2) * 48 - 16 - 16;
  dest.w = 48;
  dest.h = 16;
  
  SDL_BlitSurface(object_pixmaps[LINES_GFX - 1], NULL,
		  window, &dest);

  SDL_UpdateRect(window, dest.x, dest.y, dest.w, dest.h);
  
  shownumber(lines, WIDTH * 48 - (16 * 5), (HEIGHT + 2) * 48 - 16 - 16);


  dest.x = WIDTH * 48 - 48 - (16 * 5);
  dest.y = (HEIGHT + 2) * 48 - 16;
  dest.w = 48;
  dest.h = 16;

  SDL_BlitSurface(object_pixmaps[NEED_GFX - 1], NULL,
		  window, &dest);

  SDL_UpdateRect(window, dest.x, dest.y, dest.w, dest.h);
  
  shownumber(linesneeded, WIDTH * 48 - (16 * 5), (HEIGHT + 2) * 48 - 16);
}


/* Get a key... mouse is seen as key, too!
   Maybe handle expose events
   (the original source that this was ported from was obviously
   not written for an event-based system): */

void getkey(int handle_expose)
{
  /* int x, y; */
  

#ifndef EMBEDDED  
  key = SDLK_LAST;
  
  while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_KEYDOWN)
	{
	  /* Get the key's name: */
	  
	  key = event.key.keysym.sym;
	}
      else if (event.type == SDL_MOUSEBUTTONDOWN)
	{
	  /* Mouse counts as keyboard: */
	  
	  if (event.button.y >= HEIGHT * 48)
	    {
	      /* Left or right of man at the bottom counts as left/right: */
	      
	      if (event.button.x < playerx * 48 + 16)
		key = SDLK_LEFT;
	      else if (event.button.x >= playerx * 48 + 31)
		key = SDLK_RIGHT;
	    }
	  else
	    {
	      /* Left or right click above man count as grab/throw: */
	      
	      if (event.button.button == 1)
		key = SDLK_DOWN;
	      else
		key = SDLK_UP;
	    }
	}
      else if (event.type == SDL_MOUSEMOTION)
	{
	  /* Handle cursor shape: */
	  
	  if (game_playing == 0)
	    SDL_SetCursor(cursor[CURSOR_NOTHING]);
	  else
	    {
	      if (event.motion.y >= HEIGHT * 48)
		{
		  /* Left/right arrows or nothing: */
		  
		  if (event.motion.x < playerx * 48 + 16)
		    SDL_SetCursor(cursor[CURSOR_LEFT]);
		  else if (event.motion.x >= playerx * 48 + 31)
		    SDL_SetCursor(cursor[CURSOR_RIGHT]);
		  else
		    SDL_SetCursor(cursor[CURSOR_NOTHING]);
		}
	      else
		SDL_SetCursor(cursor[CURSOR_ACTION]);
	    }
	}
    }
#else
  printf("$$ getkey()\n");
#endif
}


/* Wait until joystick buttons are released: */

#ifndef EMBEDDED

void eatjoybuttons(void)
{
  int i, any_down;
  SDL_Event event;
  
  
  if (use_joystick)
    {
      do
	{
	  any_down = 0;

	  SDL_PollEvent(&event);
	  
	  for (i = 0; i < SDL_JoystickNumButtons(js); i++)
	    {
	      if (SDL_JoystickGetButton(js, i))
		any_down = 1;
	    }
	  
	  SDL_Delay(10);
	}
      while (any_down);
    }
}

#endif


/* Show the title... let them change the level: */

int title(void)
{
#ifndef EMBEDDED
  int btn;
#endif
  int quit, ok, title_width, title_height, height,
    author_width, author_height, author_x, author_y, author,
    author_xm, author_ym, sine_timer, i, credits_width, credits_height;
  SDL_Rect src, dest;
  Uint32 last_time;
  
  
  /* Find out size of bitmaps we're using in the title screen: */

  title_width = object_pixmaps[TITLE_GFX - 1] -> w;
  title_height = object_pixmaps[TITLE_GFX - 1] -> h;

  author_width = object_pixmaps[AUTHOR_GFX - 1] -> w;
  author_height = object_pixmaps[AUTHOR_GFX - 1] -> h;

  credits_width = object_pixmaps[CREDITS_GFX - 1] -> w;
  credits_height = object_pixmaps[CREDITS_GFX - 1] -> h;
  
  author_x = WIDTH * 48 - author_width;
  author_y = HEIGHT * 48 - author_height - (rand() % 10) - 30;
  
  
  /* Clear the screen: */
  
  erasewindow();
  
  
  /* Draw credits / title / version: */
  
  dest.x = (WIDTH * 48 - credits_width) / 2;
  dest.y = (HEIGHT * 48 - credits_height) / 2;
  dest.w = credits_width;
  dest.h = credits_height;
  
  SDL_BlitSurface(object_pixmaps[CREDITS_GFX - 1], NULL,
		  window, &dest);
  
  SDL_UpdateRect(window, dest.x, dest.y, dest.w, dest.h);
  
  
  /* Draw the currently-selected level: */
  
  updatelevel();
  
  
  /* Init. animation variables: */
  
  author = 0;
  height = 0;
  do
    {
      author_xm = -(rand() % 3);
    }
  while (author_xm == 0);
  author_ym = (rand() % 6) - 3;
  
  
  /* We haven't quit and we haven't started a game: */
  
  quit = NO;
  ok = NO;
  
  
  /* Title animation loop: */
  
  sine_timer = 500 - title_height;
  
  do
    {
      last_time = SDL_GetTicks();
      
      
      /* Show more and more of the title: */
	  
      height = height + 2;
      if (height > title_height)
	height = title_height;
      
      
      /* Draw it: */
      
      sine_timer++;
      
      if (sine_timer < 500)
	{
	  dest.x = (WIDTH * 48 - title_width) / 2;
	  dest.y = -title_height + height;
	  dest.w = title_width;
	  dest.h = title_height;
	  
	  SDL_BlitSurface(object_pixmaps[TITLE_GFX - 1], NULL,
			  window, &dest);

	  SDL_UpdateRect(window, dest.x, dest.y, dest.w, dest.h);
	}
      else
	{
	  for (i = 0; i < title_height; i++)
	    {
	      src.x = 0;
	      src.y = i;
	      src.w = title_width;
	      src.h = 1;
	      
	      dest.x = (int) ((WIDTH * 48 - title_width) / 2 + 
			sin(M_PI * ((i + sine_timer) * 5) / 180.0) * 5.0);
	      dest.y = i;
	      dest.w = title_width;
	      dest.h = 1;
	      
	      SDL_BlitSurface(object_pixmaps[TITLE_GFX - 1], &src,
			      window, &dest);
	      
	      SDL_UpdateRect(window, dest.x, dest.y, dest.w, dest.h);
	    }
	}
      
      /* Hmm!? */
      
      author = author + 1;
      
      if (author > 1000)
	{
	  dest.x = author_x;
	  dest.y = author_y;
	  dest.w = author_width;
	  dest.h = author_height;
	  
	  my_FillRect(window, &dest, 0x00, 0x00, 0x00);
	  
	  SDL_UpdateRect(window, dest.x, dest.y, dest.w, dest.h);
	  
	  author_x = author_x + author_xm;
	  if (author_x < 0)
	    {
	      author_x = 0;
	      do
		{
		  author_xm = rand() % 3;
		}
	      while (author_xm == 0);
	    }
	  if (author_x > WIDTH * 48 - author_width)
	    {
	      author_x = WIDTH * 48 - author_width;
	      do
		{
		  author_xm = -rand() % 3;
		}
	      while (author_xm == 0);
	    }
	  
	  author_y = author_y + author_ym;
	  if (author_y < windowheight / 2)
	    {
	      author_y = windowheight / 2;
	    }
	  if (author_y > HEIGHT * 48 - author_height)
	    {
	      author_y = HEIGHT * 48 - author_height;
	      author_ym = -author_ym;
	    }
	  
	  author_ym = author_ym + 1;
	  if (author_ym > 10)
	    author_ym = 10;
	  
	  if (author < 1500)
	    {
	      dest.x = author_x;
	      dest.y = author_y;
	      dest.w = author_width;
	      dest.h = author_height;
	      
	      SDL_BlitSurface(object_pixmaps[AUTHOR_GFX - 1], NULL,
			      window, &dest);
	      
	      SDL_UpdateRect(window, dest.x, dest.y, dest.w, dest.h);
	    }
	  else
	    {
	      author = 0;
	    }
	}
      
      
      /* Check for keypresses: */
      
      getkey(0);
      
      if (key == SDLK_ESCAPE ||
	  key == SDLK_q)
	{
	  /* Escape to quit: */
	  
	  ok = YES;
	  quit = YES;
	}
      else if (key == SDLK_l)
	{
	  /* L for level select: */
	  
	  level = level + 1;
	  if (level > max_level)
	    level = 1;
	  
	  playsound(SND_LEVELSELECT);
	  
	  updatelevel();
	}
      else if (key == SDLK_RETURN ||
	       key == SDLK_SPACE)
	{
	  /* Return or Space to begin: */
	  
	  ok = YES;
	}
      else if (key == SDLK_a)
	{
	  /* Hmm!? */
	  
	  if (author < 1000)
	    author = 1000;
	  else
	    author = 1495;
	}
      
      
      /* Any joystick buttons? */

#ifndef EMBEDDED      
      if (use_joystick)
	{
	  for (btn = 0; btn < SDL_JoystickNumButtons(js); btn++)
	    {
	      if (SDL_JoystickGetButton(js, btn))
		ok = YES;
	    }
	}
#endif
      
      
      /* Keep framerate exact: */
      
      if (SDL_GetTicks() < last_time + (1000 / FPS))
	SDL_Delay(last_time + (1000 / FPS) - SDL_GetTicks());
      

      /* Play music: */
#ifndef NOSOUND
      if (use_sound)
      {
        if (no_music == 0)
	  {
	    if (!Mix_PlayingMusic())
	      Mix_PlayMusic(songs[MOD_TITLE], 0);
	    /* Mix_VolumeMusic(8); */
	  }
      }
#endif
    }
  while (ok != YES);
  
#ifndef EMBEDDED
  eatjoybuttons();
#endif
  
  return(quit);
}


/* See if you're happy or sad...  Depends on if there are any blocks
   close to the bottom: */

void checkhappy()
{
  int x, y;
  
  happy = YES;
  for (y = HEIGHT - (HEIGHT / 3); y < HEIGHT; y++)
    {
      for (x = 0; x < WIDTH; x++)
	{
	  if (blocks[y][x] != 0)
	    happy = NO;
	}
    }
}


/* Init this level (randomly place blocks, etc.) */

void initlevel(void)
{
  int x, y, yy;
  
  
  playsound(SND_BEGIN);
  
  
  /* Erase all blocks: */
  
  for (y = 0; y < HEIGHT; y++)
    for (x = 0; x < WIDTH; x++)
      blocks[y][x] = 0;
  
  
  /* How low should they go? */
  
  yy = level;
  if (level > 14)
    yy = yy - (HEIGHT - 2);
  
  if (yy > HEIGHT - 2)
    yy = HEIGHT - 2;
  
  
  /* Place random blocks: */
  
  for (y = 0; y <= yy; y++)
    for (x = 0; x < WIDTH; x++)
      blocks[y][x] = randblock();
  
  
  /* Draw all blocks: */
  
  for (y = 0; y < HEIGHT; y++)
    for (x = 0; x < WIDTH; x++)
      drawblock(x, y);
  
  
  /* Init level variables: */
  
  leveldone = NO;
  
  carrying = 0;
  howmany = 0;
  frozen = 0;
  _warning = NO;
  firstround = NO;
  
  
  /* Start out happy or not, depending on where the pieces are: */
  
  checkhappy();

  
  /* How many lines do you need / have? */
  
  lines = 0;
  linesneeded = (level * 3) + 2;
  
  
  /* Draw the rest of the game screen stuff: */
  
  drawscore();
  updatelines();
  drawstatus();
  updatelevel();
}


/* Erase player: */

void eraseyou(int x)
{
  SDL_Rect dest;
  
  dest.x = x * 48 - 48;
  dest.y = HEIGHT * 48;
  dest.w = 48 * 3;
  dest.h = 48;
  
  my_FillRect(window, &dest, 0x00, 0x00, 0x00);
  
  SDL_UpdateRect(window, dest.x, dest.y, dest.w, dest.h);
}


/* Draw player and objects he's carrying: */

void drawyou(int x)
{
  int y;
  
  
  y = HEIGHT * 48;
  
  if (howmany < 3)
    {
      /* Happy?  Or sad? */
      
      if (happy == YES)
	{
	  drawmangraphic(x * 48, y, HAPPY_GFX);
	}
      else
	{
	  drawmangraphic(x * 48, y, SAD_GFX);
	}
    }
  
  
  if (carrying != 0)
    {
      drawblockgraphic(x * 48 + 48, y, carrying);
      
      if (howmany >= 2)
	drawblockgraphic(x * 48 - 48, y, carrying);
      
      if (howmany >= 3)
	drawblockgraphic(x * 48, y, carrying);
    }
}


/* Assign an explosion: */

void explodeblock(int x, int y, int explosionwait)
{
  /* Pick an explosion entity to use: */
  
  whichexplosion = whichexplosion + 1;
  
  if (whichexplosion == NUM_EXPLOSIONS)
    whichexplosion = 0;
  
  
  /* Init. its values: */
  
  explosions[whichexplosion].exist = 1;
  explosions[whichexplosion].anim = -explosionwait;
  explosions[whichexplosion].animrate = 0;
  explosions[whichexplosion].x = x;
  explosions[whichexplosion].y = y;
}


/* See if two objects match: */

int same(int a, int b)
{
  int match;
  
  
  /* A match if it's the same, or the object (A) is a special object: */
  
  match = NO;
  
  if (a == b || a > NUM_ALL_BLOCKS || b > NUM_ALL_BLOCKS)
    match = YES;
  
  return(match);
}


/* Kill a block (may call itself recursively!) */

int killblock(int x, int y, int killed)
{
  int c, explosionwait;
  
  
  /* Values we use a lot later: */
  
  explosionwait = killed;
  c = blocks[y][x];
  
  
  if (c != 0)
    {
      /* Remove the block: */
      
      blocks[y][x] = 0;
      
      
      /* Put an explosion there: */
      
      explodeblock(x, y, explosionwait);
      
      
      if (c == BOMB)
	{
	  /* If it's a bomb, explode stuff next to it: */
	  
          playsound(SND_BOMB);
	  
	  if (y > 0)
	    {
	      blocks[y - 1][x] = 0;
	      explodeblock(x, y - 1, explosionwait);
	    }
	  
	  if (y < HEIGHT - 1)
	    {
	      blocks[y + 1][x] = 0;
	      explodeblock(x, y + 1, explosionwait);
	    }
	  
	  if (x > 0)
	    {
	      blocks[y][x - 1] = 0;
	      explodeblock(x - 1, y, explosionwait);
	    }
	  
	  if (x < WIDTH - 1)
	    {
	      blocks[y][x + 1] = 0;
	      explodeblock(x + 1, y, explosionwait);
	    }
	}
      else if (c == CLOCK)
	{
	  /* Hit a clock!  Freeze the game! */
	     
	  playsound(SND_CLOCK);
	  
	  frozen = 50;
	}
      else if (c == WILDCARD)
	{
	  /* Hit a wildcard - play a noise... the rest comes natural: */
	  
	  playsound(SND_WILDCARD);
	}
      
      
      /* Kill stuff next to you if they match! */
      
      if (y > 0)
	{
	  if (same(blocks[y - 1][x], c) == YES)
	    killed = killblock(x, y - 1, killed + 1);
	}
      
      if (y < HEIGHT - 1)
	{
	  if (same(blocks[y + 1][x], c) == YES)
	    killed = killblock(x, y + 1, killed + 1);
	}
      
      if (x > 0)
	{
	  if (same(blocks[y][x - 1], c) == YES)
	    killed = killblock(x - 1, y, killed + 1);
	}
      
      if (x < WIDTH - 1)
	{
	  if (same(blocks[y][x + 1], c) == YES)
	    killed = killblock(x + 1, y, killed + 1);
	}
    }
  
  if (killed > 14)
    killed = 14;
  
  return(killed);
}


/* Throw your gems: */

void _throw(int x)
{
  int lasty, killed;
  int y, c, last, nextlast, ytop, ybot, ok, doit;
  
  
  if (carrying != 0)
    {
      last = 0;
      nextlast = 0;
      lasty = -1;
      
      
      /* Find the lowest point that they'll attach to: */
      
      for (y = 0; y < HEIGHT; y++)
	{
	  c = blocks[y][x];
	  
	  if (c != 0)
	    {
	      nextlast = last;
	      last = c;
	      lasty = y;
	    }
	}
      
      
      /* See if there's a match: */
      
      ok = NO;
      
      if (same(last, carrying) == YES)
	{
	  if (same(nextlast, carrying) == YES || howmany > 1)
	    ok = YES;
	}
      
      doit = YES;
      
      ybot = lasty + 1 + howmany - 1;
      
      
      /* Don't go past the bottom: */
      
      if (ybot > 10)
	{
	  doit = NO;
	  ybot = 10;
	  if (ok == YES || howmany > 2)
	    doit = YES;
	}
      
      
      /* These'll fit?  Put them on the screen: */
      
      if (doit == YES)
	{
	  for (y = lasty + 1; y <= ybot; y++)
	    {
	      blocks[y][x] = carrying;
	      drawblock(x, y);
	    }
	}
      
      
      /* See if it's a match of 3 or more: */
      
      ok = NO;
      
      if (same(last, carrying) == YES)
	{
	  if (same(nextlast, carrying) == YES || howmany > 1)
	    ok = YES;
	}
      
      if (howmany > 2)
	ok = YES;
      
      
      /* It IS! */
      
      if (ok == YES)
	{
	  ytop = 0;
	  for (y = 0; y <= ybot; y++)
	    {
	      if (blocks[y][x] != carrying)
		ytop = y + 1;
	    }
	  
	  killed = killblock(x, ybot, 1);
	  
	  score = score + scorevals[killed];
	  
	  lines = lines + 1;
	  
	  drawscore();
	  updatelines();	
	  playsound(SND_GOT);
	}
      
      if (doit == YES)
	{
	  carrying = 0;
	  howmany = 0;
	  
	  playsound(SND_THROW);
	}
    }
  else
    {
      complain();
    }
  
  
  /* See if the level's done!? */
  
  if (lines >= linesneeded)
    leveldone = YES;
}


/* Grab some gems */

void grab(int x)
{
  int y, c, last, lasty, ok;
  
  
  /* Find the lowest gem on the column: */
  
  last = 0;
  lasty = 0;
  
  for (y = 0; y < HEIGHT; y++)
    {
      c = blocks[y][x];
      if (c != 0)
	{
	  last = c;
	  lasty = y;
	}
    }
  
  
  /* See if we got one: */
  
  if (last == 0 || last > NUM_ALL_BLOCKS)
    {
      /* Nope! */
      
      complain();
    }
  else
    {
      /* Yep!: */
      
      if (last != carrying && carrying != 0)
	{
	  /* Not the same as what we're carrying, though! */
	  
	  complain();
	}
      else
	{
	  /* Grab it: */
	  
	  playsound(SND_GRAB);
	  
	  carrying = last;
	  blocks[lasty][x] = 0;
	  drawblock(x, lasty);
	  howmany = howmany + 1;
	  
	  
	  /* Grab any of the same type that are above it: */
	  
	  do
	    {
	      ok=0;
	      lasty = lasty - 1;
	      if (lasty >= 0)
		{
		  if (blocks[lasty][x] == last)
		    {
		      blocks[lasty][x] = 0;
		      drawblock(x, lasty);
		      ok = 1;
		      howmany = howmany + 1;
		    }
		}
	    }
	  while (ok != 0);
	}
    }
}


/* Add more gems at the top of the screen: */

void addmore()
{
  int x, y;
  

  /* See if the game's over: */
  
  for (x = 0; x < WIDTH; x++)
    {
      if (blocks[HEIGHT - 1][x] != 0)
	gameover = YES;
    }
  
  
  if (gameover == NO)
    {
      /* Push the existing ones down: */
      
      for (y = HEIGHT - 1; y > 0; y--)
	{
	  for (x = 0; x < WIDTH; x++)
	    {
	      blocks[y][x] = blocks[y - 1][x];
	      drawblock(x, y);
	    }
	}
      
      
      /* Add random ones to the top: */
      
      for (x = 0; x < WIDTH; x++)
	{
	  blocks[0][x] = randblock();
	  drawblock(x, 0);
	}
    }
}


/* Update the explosion animations: */

int drawexplosions(void)
{
  int i, any;
  
  
  /* We'll say if there are any more or not: */
  
  any = 0;
  
  
  for (i = 0; i < NUM_EXPLOSIONS; i++)
    {
      if (explosions[i].exist == YES)
	{
	  /* (There are some!) */
	  
	  any = 1;
	  
	  
	  /* There's an animation slow-down... */
	  
	  explosions[i].animrate++;
	  
	  if (explosions[i].animrate >= EXPLOSION_SLOW_VAL)
	    {
	      explosions[i].animrate = 0;
	      explosions[i].anim++;
	      
	      if (explosions[i].anim >= NUM_EXPLOSION_GFX)
		{
		  /* It's gone!  Draw whatever was/is behind it: */
		  
		  explosions[i].exist = NO;
		  drawblock(explosions[i].x, explosions[i].y);
		}
	      else if (explosions[i].anim >= 0)
		{
		  /* Draw the animation frame: */
		  
		  drawblockgraphic(explosions[i].x * 48, explosions[i].y * 48,
				   NUM_ALL_BLOCKS + NUM_SPECIALS + 1 +
				   explosions[i].anim);
		}
	    }
	}
    }
  
  return(any);
}      


/* End-of-level effect (major destruction, dude!) */

void levelendfx(int yourx)
{
  int x, y, any, win;
  Uint32 last_time;
  
  
  /* Let all of old the explosions "fizzle out"... */
  
  do
    {
      last_time = SDL_GetTicks();
      
      
      /* Update explosions: */
      
      any = drawexplosions();
      
      
      /* Keep framerate exact: */
      
      if (SDL_GetTicks() < last_time + (1000 / FPS))
	SDL_Delay(last_time + (1000 / FPS) - SDL_GetTicks());
    }
  while(any == 1);
  
  
  /* Toggle value for winning animation: */
  
  win = 0;
  
  
  /* Explode from the top to the bottom: */
  
  playsound(SND_WIN);
  
  for (y = 0; y < HEIGHT; y++)
    {
      last_time = SDL_GetTicks();
      
      
      /* Erase this row: */
      
      for (x = 0; x < WIDTH; x++)
	{
	  blocks[y][x] = 0;
	  drawblock(x, y);
	}
      
      
      /* Add some explosions: */
      
      for (x = 0; x < WIDTH; x++)
	{
	  if ((rand() % 10) < 6)
	    explodeblock(x, y, -(rand() % 10));
	}
      
      
      /* Animate the dude: */
      
      win = 1 - win;
      drawmangraphic(yourx * 48, HEIGHT * 48, WIN1_GFX + win);
      
      
      /* Update the explosions: */
      
      drawexplosions();
      
      
      /* Keep framerate exact: */
      
      if (SDL_GetTicks() < last_time + (1000 / FPS))
	SDL_Delay(last_time + (1000 / FPS) - SDL_GetTicks());
    }
  
  
  /* Let all of the explosions "fizzle out"... */
  
  do
    {
      last_time = SDL_GetTicks();
      
      
      /* Update explosions: */
      
      any = drawexplosions();
      
      
      /* Draw man: */
      
      win = 1 - win;
      drawmangraphic(yourx * 48, HEIGHT * 48, WIN1_GFX + win);
      
      
      /* Keep framerate exact: */
      
      if (SDL_GetTicks() < last_time + (1000 / FPS))
	SDL_Delay(last_time + (1000 / FPS) - SDL_GetTicks());
    }
  while(any == 1);
}


/* Level 15 effect */

void level15fx()
{
  int x;
  Uint32 last_time;
  SDL_Rect dest;
  
  
  erasewindow();
  
  for (x = windowwidth; x >= -48; x = x - 8)
    {
      last_time = SDL_GetTicks();
      
      
      /* Draw player: */
      
      howmany = 2;
      
      dest.x = x;
      dest.y = windowheight / 2 - 16;
      dest.w = 56;
      dest.h = 64;
      
      my_FillRect(window, &dest, 0x00, 0x00, 0x00);
      SDL_UpdateRect(window, dest.x, dest.y, dest.w, dest.h);
      
      drawmangraphic(x, windowheight / 2 - 16, SAD_GFX);
      
      dest.x = x;
      dest.y = windowheight / 2 - 16 + 48;
      dest.w = 48;
      dest.h = 16;
      
      SDL_BlitSurface(object_pixmaps[UHOH_GFX - 1], NULL,
		      window, &dest);
      
      
      /* Keep framerate exact: */
      
      if (SDL_GetTicks() < last_time + (1000 / FPS))
	SDL_Delay(last_time + (1000 / FPS) - SDL_GetTicks());
    }
  
}


/* MAIN GAME FUNCTION!!!! */

void play()
{
  int oact, q, x, y, toggle, win;
#ifndef EMBEDDED
  int jsx, ojsx, jsbutton0, jsbutton1, jsbutton23,
    ojsbutton0, ojsbutton1, ojsbutton23, jstimer;
#endif
  Uint32 last_time;
  SDL_Rect dest;
#ifndef NOSOUND
  int i;
#endif
  
  
  /* Init the game variables: */
  
  initlevel();
  clicks = 0;
  playerx = 5;
  gameover = NO;
  win = NO;
  oact = 0 ;
  score = 0;
  
  
  /* Draw you! */
  
  drawyou(playerx);
  
  
#ifndef EMBEDDED
  ojsx = 0;
  ojsbutton0 = 0;
  ojsbutton1 = 0;
  ojsbutton23 = 0;
  jstimer = 0;
#endif
  
  
  /* MAIN GAME LOOP: */
  
  do
    {
      last_time = SDL_GetTicks();
      
      
      /* Get any keys: */
      
      getkey(1);

#ifndef EMBEDDED
      if (use_joystick)
	{
	  jsx = SDL_JoystickGetAxis(js, 0);
	  
	  if (jsx < -16384 && (ojsx >= -16384 || jstimer == STICKWAIT - 1))
	    key = SDLK_LEFT;
	  else if (jsx > 16384 && (ojsx <= 16384 || jstimer == STICKWAIT - 1))
	    key = SDLK_RIGHT;
	  
	  jsbutton0 = SDL_JoystickGetButton(js, 0);
	  jsbutton1 = SDL_JoystickGetButton(js, 1);
	  jsbutton23 = (SDL_JoystickGetButton(js, 2) +
			SDL_JoystickGetButton(js, 3));
	  
          if (jsbutton1 && ojsbutton1 == 0)
	    key = SDLK_DOWN;
          else if (jsbutton0 && ojsbutton0 == 0)
	    key = SDLK_UP;
          else if (jsbutton23 && ojsbutton23 == 0)
	    key = SDLK_RETURN;
          
          if ((jsx < -16384 && ojsx < -16384) ||
	      (jsx > 16384 && ojsx > 16384))
	    {
	      jstimer++;
	      if (jstimer >= STICKWAIT)
	        jstimer = 0;
	    }
          else
	    jstimer = 0;
	  
	  ojsx = jsx;
	  ojsbutton0 = jsbutton0;
	  ojsbutton1 = jsbutton1;
	  ojsbutton23 = jsbutton23;
	}
#endif
      
      if (key != SDLK_LAST)
	{
	  if (key == SDLK_ESCAPE ||
	      key == SDLK_q)
	    {
	      /* Escape or Q to quit: */
	      
	      gameover = YES;
	    }
	  else if (key == SDLK_RIGHT ||
		   key == SDLK_l)
	    {
	      /* Right to move you right */
	      
	      eraseyou(playerx);
	      playerx++;
	      
	      
	      /* Wrap around: */
	      
	      if (playerx > WIDTH - 1)
		playerx = 0;
	    }
	  else if (key == SDLK_LEFT ||
		   key == SDLK_k)
	    {
	      /* Left to move you left: */
	      
	      eraseyou(playerx);
	      playerx--;
	      
	      
	      /* Wrap around: */
	      
	      if (playerx < 0)
		playerx = WIDTH - 1;
	    }
	  else if (key == SDLK_UP ||
		   key == SDLK_a)
	    {
	      /* Up to throw: */
	      
	      eraseyou(playerx);
	      _throw(playerx);
	    }
	  else if (key == SDLK_DOWN ||
		   key == SDLK_z)
	    {
	      /* Down to grab: */
	      
	      eraseyou(playerx);
	      grab(playerx);
	    }
	  else if (key == SDLK_RETURN)
	    {
	      /* Return to get more gems NOW!: */
	      
	      firstround = YES;
	      frozen = NO;
	      clicks = 32767;
	    }
	  else if (key == SDLK_SPACE ||
		   key == SDLK_p)
	    {
	      /* Space or P to pause... wait for keypress to continue: */
	      
	      eraseyou(playerx);
	      
	      toggle = 0;

#ifndef NOSOUND
	      if (use_sound)
	      {
	        for (i = 0; i < MIX_CHANNELS; i++)
		  Mix_HaltChannel(i);
	      
	        if (no_music == 0)
		  Mix_HaltMusic();
	      }
#endif

	      do
		{
		  last_time = SDL_GetTicks();
		  
		  
		  /* Pause for a frame: */
		  
		  getkey(1);
		  
		  
		  /* Show the word "paused": */
		  
		  dest.x = (WIDTH * 48 - 48) / 2;
		  dest.y = (HEIGHT + 1) * 48;
		  dest.w = 48;
		  dest.h = 16;
		  
		  SDL_BlitSurface(object_pixmaps[PAUSED_GFX - 1], NULL,
				  window, &dest);
		  SDL_UpdateRect(window, dest.x, dest.y, dest.w, dest.h);
		  
		  
		  /* Keep framerate exact: */
		  
		  if (SDL_GetTicks() < last_time + (1000 / FPS))
		    SDL_Delay(last_time + (1000 / FPS) - SDL_GetTicks());
		}
	      while (key == SDLK_LAST);
	      
	      dest.x = (WIDTH * 48 - 48) / 2;
	      dest.y = (HEIGHT + 1) * 48;
	      dest.w = 48;
	      dest.h = 16;

	      my_FillRect(window, &dest, 0x00, 0x00, 0x00);
	      SDL_UpdateRect(window, dest.x, dest.y, dest.w, dest.h);
	    }
	  
	  
	  /* Draw you again, just in case: */
	  
	  drawyou(playerx);
	}
      
      
      /* Draw explosions: */
      
      drawexplosions();
      
      
      /* Move stuff if need be: */
      
      if (frozen == 0)
	{
	  /* No clock... so keep counting til we move stuff down: */
	  
	  checkhappy();
	  
	  clicks = clicks + 1;
	  
	  
	  /* The higher the level, the faster stuff comes: */
	  
	  if (level < 5)
	    q = level * 60;
	  else if (level < 14)
	    q = level * 25;
	  else if (level == 14)
	    q = level * 24;
	  else
	    q = (level - 2) * 20;
	  
	  
	  /* Warn them first... */
	  
	  if (clicks >= 370 - q && firstround == YES)
	    {
	      _warning = YES;
	    }
	  
	  
	  /* Drop stuff down: */
	  
	  if (clicks >= 400 - q)
	    {
	      if (firstround == YES)
		{
		  addmore();
		  playsound(SND_BEGIN);
		}
	      
	      clicks = 0;
	      firstround = YES;
	      
	      _warning = NO;
	    }
	}
      else
	{
	  /* Count down the clock... */
	  
	  happy = YES;
	  
	  frozen = frozen - 1;
	}
      
      
      drawstatus();

      
      /* See if the level's complete! */
      
      if (leveldone == YES)
	{
	  /* Do the end-of-level effect: */
	  
	  levelendfx(playerx);
	  
	  
	  /* Increment the level: */
	  
	  level = level + 1;
	  if (level > 20)
	    {
	      gameover = YES;
	      win = YES;
	    }
	  
	  if (level > max_level)
	    max_level = level;
	  
	  
	  /* Do the level 15 effect: */
	  
	  if (level == 15)
	    level15fx();
	  
	  
	  /* Reset the level! */
	  
	  initlevel();
	  clicks = 0;
	  updatelevel();
	}

      
      /* Keep framerate exact: */
      
      if (SDL_GetTicks() < last_time + (1000 / FPS))
	SDL_Delay(last_time + (1000 / FPS) - SDL_GetTicks());
      
      
      /* Play music: */
#ifndef NOSOUND
      if (use_sound)
      {
        if (no_music == 0)
	  {
	    if (!Mix_PlayingMusic())
	      Mix_PlayMusic(songs[MOD_GAME1 + (rand() % NUM_GAME_MODS)], 0);
	    /* Mix_VolumeMusic(4); */
	  }
      }
#endif
    }
  while (gameover == NO);
  
  
#ifndef NOSOUND
  if (use_sound)
  {
    if (no_music == 0)
      Mix_HaltMusic();
  }
#endif
  
  if (win == NO)
    {
      /* End of game effect: */
      
      playsound(SND_GAMEOVER);
      
      happy = NO;
      drawyou(playerx);
      
      
      /* Dump bricks down at you!: */
      
      for (y = 0; y < HEIGHT; y++)
	{
	  last_time = SDL_GetTicks();
	  
	  for (x = 0; x < WIDTH; x++)
	    {
	      drawblockgraphic(x * 48, y * 48, BRICK_GFX);
	    }
	  
	  
	  /* Keep framerate exact: */
	  
	  if (SDL_GetTicks() < last_time + (1000 / FPS))
	    SDL_Delay(last_time + (1000 / FPS) - SDL_GetTicks());
	}
    }
  else
    {
      /* Win game effect: */
      
      playsound(SND_APPLAUSE);
      
      dest.x = windowwidth / 2 - 96;
      dest.y = windowheight / 2 - 32;
      dest.w = 192;
      dest.h = 64;
      
      SDL_BlitSurface(object_pixmaps[CONGRATULATIONS_GFX - 1], NULL,
		      window, &dest);
      
      SDL_UpdateRect(window, dest.x, dest.y, dest.w, dest.h);
    }
  
  /* Wait for a keypress: */
  
  do
    {
      getkey(0);
    }
  while (key == SDLK_LAST);

#ifndef EMBEDDED
  eatjoybuttons();
#endif
}


/* Quit the application: */

void quitapp()
{
#ifndef EMBEDDED
  if (use_joystick)
    SDL_JoystickClose(js);
  
  SDL_Quit();
#endif
}


/* Usage error, and quit: */

void usage(int ret)
{
  fprintf(stderr, "Usage: gemdropx [--nomusic] | --version | --help | --usage\n\n");
  exit(ret);
}


/* MAIN FUNCTION */

int main(int argc, char * argv[])
{
  int quit;
  char datafile[1024];
  FILE * fi;
#ifndef EMBEDDED
  SDL_version sdlver;
#endif
  
  
  no_music = 0;
  
  if (argc == 2)
    {
      /* --version or --help?? */
      
      if (strcmp(argv[1], "--version") == 0)
	{
	  printf("\nGem Drop X 0.9\n\n");
	  printf("February 11, 2002\n");
	  printf("by Bill Kendrick\n");
	  printf("New Breed Software, (c) 1997-2002\n\n");
	  
	  printf("Most graphics by Bernhard Trummer\n");
	  printf("bernhard.trummer@gmx.net\n\n");
	  
	  printf("Visit our website: http://www.newbreedsoftware.com/\n\n");
	  
#ifndef EMBEDDED
	  SDL_VERSION(&sdlver);
	  printf("SDL version %d.%d.%d\n\n", sdlver.major,
		 sdlver.minor, sdlver.patch);
#else
	  printf("Built for Qt/Embedded\n\n");
#endif

	  exit(0);
	}
      else if (strcmp(argv[1], "--help") == 0)
	{
	  printf("GEM DROP X HELP\n");
	  printf("How To Play:\n");
	  printf("  Use the man to grab and throw gems.\n");
	  printf("  Match 3+ in a column when you throw to get a line.\n");
	  printf("  Get enough lines to beat the level.\n");
	  printf("  Gems come down at you from the top.\n");
	  printf("  If the screen fills, the game ends.\n\n");
	  
	  printf("Special Pieces:  (You can't grab them)\n");
	  printf("  Activate them by including them in a match.\n");
	  printf("  - Bombs explode.\n");
	  printf("  - Clocks stop the gems from coming for a while.\n");
	  printf("  - Wildcards (question-marks) make for more matches.\n\n");
	  
	  printf("Controls:\n");
	  printf("  Left/Right - Move left/right\n");
	  printf("  K/L        - Move left/right\n");
	  printf("  Up/Down    - Throw/Grab\n");
	  printf("  A/Z        - Throw/Grab\n");
	  printf("  Return     - Get more gems immediately\n");
	  printf("  Space / P  - Pause\n");
	  printf("  S          - Toggle sound\n");
	  printf("  Q/Escape   - Abort game\n\n");

	  printf("Mouse:\n");
	  printf("  Click Left/Right of Man - Move left/right\n");
	  printf("  Left-click Above Man    - Grab\n");
	  printf("  Right-click Above man   - Throw\n\n");

	  printf("Joystick:\n");
	  printf("  Left/Right - Move left/right\n");
	  printf("  Button 1/2 - Throw/Grab\n");
	  printf("  Button 3/4 - Get more gems immediately\n\n");
	  
	  exit(0);
	}
      else if (strcmp(argv[1], "--nomusic") == 0)
	{
	  no_music = 1;
	}
      else if (strcmp(argv[1], "--usage") == 0)
	{
	  usage(0);
	}
      else
	{
	  usage(1);
	}
    }
  else if (argc > 2)
    {
      /* Oops!  Usage! */
      
      usage(1);
    }
  
  
  /* Setup: */
  
  setup(argc, argv);
  
  
  /* Get max level: */
  
  sprintf(datafile, "%s/.gemdropx", getenv("HOME"));
  
  fi = fopen(datafile, "r");
  if (fi != NULL)
    {
      fscanf(fi, "%d", &max_level);
      
      if (max_level < 10 || max_level > 20)
	{
	  fprintf(stderr, "gemdropx: %s corrupt!\n", datafile);
	  max_level = 10;
	}
      
      fclose(fi);
    }
  else
    max_level = 10;
  

  /* Main loop: */
  
  do
    {
      game_playing = 0;
      quit = title();
      
#ifndef NOSOUND
      if (use_sound)
	{
	  if (no_music == 0)
	    Mix_HaltMusic();
	}
#endif

      if (quit == NO)
	{
	  game_playing = 1;
	  play();
	}
    }
  while (quit == NO);
  
  
  /* Save max. level: */
  
  fi = fopen(datafile, "w");
  
  if (fi != NULL)
    {
      fprintf(fi, "%d\n", max_level);
      fclose(fi);
    }
  else
    perror(datafile);
  
  quitapp();
  
  return(0);
}
