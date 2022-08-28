#ifndef MAINWIN_H_
#define MAINWIN_H_

#include <qpe/qpeapplication.h>
#include <qwidget.h>
#include <qobject.h>


class MainWindow: public QWidget
{
//	Q_OBJECT
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


#endif /* MAINWIN_H_ */

