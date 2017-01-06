#include "draw_area.h"
#include "settings.h"
#include <QDirIterator>
#include <QPainter>
#include <QDebug>

draw_area::draw_area(QWidget *parent) : QWidget(parent)
{
	QDirIterator it("../twitchemotes", QStringList() << "*.png", QDir::Files, QDirIterator::Subdirectories);
	while (it.hasNext()){
	    QString trigger = it.fileName();
	    trigger.chop(4);
	    emotes.insert(trigger, new QImage(it.next()));
	}
	image = new QPixmap(SNES_WIDTH, SNES_HEIGHT);
	image->fill(Qt::blue);
}

void draw_area::paintEvent(QPaintEvent *event)
{
	Q_UNUSED (event);
	QPainter painter(this);
	
	painter.drawPixmap(0, 0, *image);
	
}
