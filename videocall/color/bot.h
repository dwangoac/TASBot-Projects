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
		
	public slots:
		void join(QString channel);
		
	private slots:
		void processMessage(IrcPrivateMessage* message);
		
	private:
		IrcCommandParser parser;
		IrcBufferModel bufferModel;
};

#endif // BOT_H
