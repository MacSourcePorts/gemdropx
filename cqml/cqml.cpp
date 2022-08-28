#include "cqml.h"
#include "mainwin.h"


static bool initialized = 0;
static QPEApplication* qapp;
static MainWindow* mw;



/*****************************************************************************
* Accounting
*/

int cqmlInit(const char* name, int argc, char** argv)
{
	qapp = new QPEApplication(argc, argv);
	mw = new MainWindow(name);
	initialized = true;

	return 0;
}


int cqmlQuit()
{
	if(!initialized) return 0;
	else {
		initialized = 0;
		return mw->close();
	}
}


int cqmlRun()
{
	if(!initialized) return 0;

	qapp->showMainWidget(mw);
	return qapp->exec();
}


/*****************************************************************************
* Timer
*/

int cqmlAddTimerhandler(int (*timerhandler)(void), int ms)
{
	mw->setTimerhandler(timerhandler, ms);

	return 0;
}


/*****************************************************************************
* Keys
*/

int cqmlAddKeypresshandler(int (*keypresshandler)(int))
{
	mw->setKeypresshandler(keypresshandler);

	return 0;
}


int cqmlAddKeyreleasehandler(int (*keyreleasehandler)(int))
{
	mw->setKeyreleasehandler(keyreleasehandler);

	return 0;
}


/*****************************************************************************
* Video
*/

int cqmlPlot(int x, int y)
{
	mw->plot(x, y);

	return 0;
}


int cqmlFlush()
{
	mw->flush();

	return 0;
}


CqmlImage* cqmlImageLoad(const char* filename)
{
	QPixmap* image = new QPixmap();

	image->load(filename);

	return image;
}


int cqmlImageBlit(CqmlImage* image, int x, int y)
{
	mw->blit(image, x, y);

	return 0;
}


/*****************************************************************************
* Audio
*/

CqmlSample* cqmlSampleLoad(const char* filename)
{
	CqmlSample* sample = new CqmlSample(filename);

	return sample;
}


int cqmlSamplePlay(CqmlSample* sample)
{
	sample->play();

	return 0;
}

