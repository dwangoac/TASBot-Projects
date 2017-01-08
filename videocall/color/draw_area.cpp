#include "draw_area.h"
#include "settings.h"
#include <QDirIterator>
#include <QPainter>
#include <QDebug>

#include <IrcBuffer> 
#include <QThread>
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
	
	qputenv("IRC_DEBUG", "1");
	
	irc.setHost(IRC_HOST);
        irc.setUserName("tasbot");
	irc.setPassword(IRC_PASSWORD);
        irc.setNickName("tasbot");
        irc.setRealName("tasbot_666");
	irc.join(IRC_CHANNEL);
	irc.setPort(IRC_PORT);
        irc.setSecure(IRC_USE_SSL);
	irc.setReconnectDelay(1);
	
	connect(&irc, &bot::statusChanged, this, [](IrcConnection::Status status){ qDebug() << status; });
	
	irc.open();
}

void draw_area::paintEvent(QPaintEvent *event)
{
	Q_UNUSED (event);
	QPainter painter(this);
	
	painter.drawPixmap(0, 0, *image);
	
}
