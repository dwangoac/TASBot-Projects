#ifndef BOT_H
#define BOT_H

#include <IrcConnection>
#include <IrcBufferModel>
#include <IrcCommandParser>

class bot : public IrcConnection
{
		Q_OBJECT
		
	public:
		 bot(QObject* parent = 0);
		 void set_emotes(QList<QString> e){ emotes = e; }
		 void set_colors(QList<QString> c){ colors = c; }

	signals:
		 void trigger_emote(QString emote, int x, int y);
		 void trigger_color(QString color, int space);
		
	public slots:
		void join(QString channel);
		
	private slots:
		void processMessage(IrcPrivateMessage* message);
		
	private:
		IrcCommandParser parser;
		IrcBufferModel bufferModel;
		QList<QString> emotes;
		QList<QString> colors;

		unsigned int hash(const char *name);
		void processColorCommand(QString color, const QStringRef& position);
};

#endif // BOT_H
