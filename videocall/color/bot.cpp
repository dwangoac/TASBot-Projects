#include "bot.h"
#include <IrcMessage>
#include <IrcCommand>
#include <QCoreApplication>
#include <QTimer>

bot::bot(QObject* parent) : IrcConnection(parent)
{
	connect(this, SIGNAL(privateMessageReceived(IrcPrivateMessage*)), this, SLOT(processMessage(IrcPrivateMessage*)));

	parser.addCommand(IrcCommand::CtcpAction, "ACT [target] <message...>");
	parser.addCommand(IrcCommand::Nick, "NICK <nick>");
	parser.addCommand(IrcCommand::Join, "JOIN <#channel> (<key>)");
	parser.addCommand(IrcCommand::Part, "PART (<#channel>) (<message...>)");
	parser.addCommand(IrcCommand::Quit, "QUIT (<message...>)");
	parser.addCommand(IrcCommand::Message, "SAY [target] <message...>");
	
	bufferModel.setConnection(this);
}

void bot::join(QString channel)
{
	sendCommand(IrcCommand::createJoin(channel));
}

void bot::processMessage(IrcPrivateMessage* message)
{
	parser.setTarget(message->isPrivate() ? message->nick() : message->target());
	parser.setTriggers(QStringList() << "!" << (message->isPrivate() ? "" : nickName().append(":")));
	
	IrcCommand* cmd = parser.parse(message->content());
	if(cmd){
		if(cmd->type() == IrcCommand::Custom && cmd->parameters().value(0) == "red"){ //todo
			sendCommand(IrcCommand::createMessage("#teamtasbot", "Red input color"));
		}else{
			sendCommand(cmd);
			
			if(cmd->type() == IrcCommand::Quit){
				connect(this, SIGNAL(disconnected()), qApp, SLOT(quit()));
				QTimer::singleShot(1000, qApp, SLOT(quit()));
			}
		}
	}
}
