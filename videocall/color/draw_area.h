#ifndef DRAW_AREA_H
#define DRAW_AREA_H

#include <QWidget>
#include <QImage>
#include <QMap>

class draw_area : public QWidget
{
		Q_OBJECT
	public:
		explicit draw_area(QWidget *parent = 0);
		
	private:
		QPixmap image;
		QMap<QString, QImage *> emotes;
};

#endif // DRAW_AREA_H
