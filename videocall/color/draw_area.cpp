#include "draw_area.h"
#include <QDirIterator>
#include <QDebug>

draw_area::draw_area(QWidget *parent) : QWidget(parent)
{
	QDirIterator it("../twitchemotes", QStringList() << "*.png", QDir::Files, QDirIterator::Subdirectories);
	while (it.hasNext()){
	    QString trigger = it.fileName();
	    trigger.chop(4);
	    emotes.insert(trigger, new QImage(it.next()));
	}
}
