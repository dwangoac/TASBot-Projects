#include "bot.h"
#include <IrcMessage>
#include <IrcCommand>
#include <QCoreApplication>
#include <QTimer>
#include <QColor>

bot::bot(QObject* parent) : IrcConnection(parent)
{
	connect(this, &bot::privateMessageReceived, this, &bot::processMessage);

	bufferModel.setConnection(this);
}

void bot::join(QString channel)
{
	sendCommand(IrcCommand::createJoin(channel));
}

void bot::processMessage(IrcPrivateMessage* message)
{
	QString text = message->content();
	QVector<QStringRef> pieces = text.splitRef(' ', QString::SkipEmptyParts);

	for(auto color : colors){
		if(!pieces[0].compare(color, Qt::CaseInsensitive)){
			bool valid = false;
			unsigned int space = 0;
			if(pieces.count() > 1){
				space = pieces[1].toInt(&valid);
			}
			if(!valid){
				space = hash(message->nick().toLatin1().data());
			}
			emit trigger_color(color, space);
			return;
		}
	}
	if (pieces[0].at(0).unicode() == 35 && pieces[0].length() == 7) {   // 35 (U+23) = "#"
		bool rvalid, gvalid, bvalid;
		auto color = QColor(
			pieces[0].mid(1, 2).toInt(&rvalid, 16),
			pieces[0].mid(3, 2).toInt(&gvalid, 16),
			pieces[0].mid(5, 2).toInt(&bvalid, 16)
		);
		if (rvalid && gvalid && bvalid) {
			bool valid = false;
			unsigned int space = 0;
			if(pieces.count() > 1){
				space = pieces[1].toInt(&valid);
			}
			if(!valid){
				space = hash(message->nick().toLatin1().data());
			}
			emit trigger_color(color.name(), space);
			return;
		}
	}

	for(const auto emote : emotes){
		if(!pieces[0].compare(emote)){
			unsigned int coords = hash(message->nick().toLatin1().data());
			unsigned int x = (coords >> 2) % (128 + 64);
			unsigned int y = (coords >> 9) % (112 + 64);
			emit trigger_emote(emote, x - 64, y - 64);
			return;
		}
	}
}

unsigned int bot::hash(const char *name)
{
	unsigned int hash = 5381;
	int c;

	while((c = *name++)){
		hash = ((hash << 5) + hash) + c;
	}

	return hash;
}
