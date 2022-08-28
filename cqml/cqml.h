#ifndef CQML_H_
#define CQML_H_

#include <qsound.h>


#ifdef __cplusplus
extern "C"
{
#endif


/*****************************************************************************
* Accounting
*/

int cqmlInit(const char* name, int argc, char** argv);
int cqmlQuit();
int cqmlRun();


/*****************************************************************************
* Timer
*/

int cqmlAddTimerhandler(int (*timerhandler)(void), int ms);


/*****************************************************************************
* Keys
*/

int cqmlAddKeypresshandler(int (*keypresshandler)(int));
int cqmlAddKeyreleasehandler(int (*keyreleasehandler)(int));


/*****************************************************************************
* Video
*/

typedef QPixmap CqmlImage;


int cqmlPlot(int x, int y);
int cqmlFlush();

CqmlImage* cqmlImageLoad(const char* filename);
int cqmlImageBlit(CqmlImage* image, int x, int y);


/*****************************************************************************
* Audio
*/

typedef QSound CqmlSample;

CqmlSample* cqmlSampleLoad(const char* filename);
int cqmlSamplePlay(CqmlSample* sample);



#ifdef __cplusplus
}
#endif

#endif /* CQML_H_ */

