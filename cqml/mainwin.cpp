#include <qtimer.h>
#include <qpainter.h>
#include "mainwin.h"


/*****************************************************************************
* Constructor
*/

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


/*****************************************************************************
* Timer controls
*/

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


/*****************************************************************************
* Event handlers
*/

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


/*****************************************************************************
* Video routines
*/

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

