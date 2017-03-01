#ifndef DRAW_AREA_H
#define DRAW_AREA_H

#include <QWidget>
#include <QImage>
#include <QMap>
#include <QKeyEvent>
#include <QStack>
#include "bot.h"

class draw_area : public QWidget
{
		Q_OBJECT
	public:
		explicit draw_area(QWidget *parent = 0);

	public slots:
		void register_color(QString color, unsigned int space);
		void register_emote(QString emote, int x, int y);
		
	protected:
		void paintEvent(QPaintEvent *event);
		void keyPressEvent(QKeyEvent *event);
		
	private:
		struct emote_location{
			QString name;
			int x;
			int y;
		};

		QImage *image;
		QMap<QString, QPixmap *> emotes;
		QVector<QImage *> pictures;
		QVector<QVector<QPair<int, int>>> picture_coords;
		int current_picture = 0;
		bot irc;
		QVector <emote_location> registered_emotes;
		QStack <QPair<int, int>> stack;

		void flood_fill(int x, int y, QColor target, QColor new_color);
		void update_picture();

};

#endif // DRAW_AREA_H
