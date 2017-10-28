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

void bot::processColorCommand(QString color, const QStringRef& positions)
{
	QVector<QStringRef> locations = positions.split(',', QString::SkipEmptyParts);

	for(auto loc : locations){
		bool bvalid;
		unsigned int space = loc.toInt(&bvalid);
		if(bvalid)
			emit trigger_color(color, space);
	}
}

void bot::processMessage(IrcPrivateMessage* message)
{
	QString text = message->content();
	QVector<QStringRef> pieces = text.splitRef(' ', QString::SkipEmptyParts);

	if (pieces.count() == 2){
		// check for color code first, since it is less work
		if (pieces[0].at(0).unicode() == 35 && pieces[0].length() == 7) {   // 35 (U+23) = "#"
			bool rvalid, gvalid, bvalid;
			auto color = QColor(
				pieces[0].mid(1, 2).toInt(&rvalid, 16),
				pieces[0].mid(3, 2).toInt(&gvalid, 16),
				pieces[0].mid(5, 2).toInt(&bvalid, 16)
			);
			if (rvalid && gvalid && bvalid) {
				processColorCommand(color.name(), pieces[1]);
				return;
			}
		}
		for(auto color : colors){
			if(!pieces[0].compare(color, Qt::CaseInsensitive)){
				processColorCommand(color, pieces[1]);
				return;
			}
		}
	}
#if 0 // disable emotes
	for(const auto emote : emotes){
		if(!pieces[0].compare(emote)){
			unsigned int coords = hash(message->nick().toLatin1().data());
			// unsigned int x = (coords >> 2) % (256 + 64);
			unsigned int x = (coords >> 2) % (382);
			// unsigned int y = (coords >> 9) % (448 + 64);
			unsigned int y = (coords >> 9) % (207);
			emit trigger_emote(emote, x, y);
			return;
		}
	}
#endif
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
