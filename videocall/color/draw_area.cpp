#include "draw_area.h"
#include "settings.h"
#include <QDirIterator>
#include <QPainter>
#include <QDebug>

#include <IrcBuffer> 
#include <QThread>

QMap<QString, QColor> colors = {
	{"dust", 0xb2996e},
	{"tea", 0x65ab7c},
	{"cement", 0xa5a391},
	{"spruce", 0x0a5f38},
	{"booger", 0x9bb53c},
	{"bland", 0xafa88b},
	{"desert", 0xccad60},
	{"purply", 0x983fb2},
	{"liliac", 0xc48efd},
	{"custard", 0xfffd78},
	{"manilla", 0xfffa86},
	{"bruise", 0x7e4071},
	{"azul", 0x1d5dec},
	{"darkgreen", 0x054907},
	{"lichen", 0x8fb67b},
	{"burple", 0x6832e3},
	{"butterscotch", 0xfdb147},
	{"toupe", 0xc7ac7d},
	{"squash", 0xf2ab15},
	{"cinnamon", 0xac4f06},
	{"cocoa", 0x875f42},
	{"orangeish", 0xfd8d49},
	{"swamp", 0x698339},
	{"camo", 0x7f8f4e},
	{"fern", 0x63a950},
	{"sapphire", 0x2138ab},
	{"parchment", 0xfefcaf},
	{"straw", 0xfcf679},
	{"terracota", 0xcb6843},
	{"creme", 0xffffb6},
	{"topaz", 0x13bbaf},
	{"wintergreen", 0x20f986},
	{"leather", 0xac7434},
	{"hazel", 0x8e7618},
	{"canary", 0xfdff63},
	{"mushroom", 0xba9e88},
	{"greenblue", 0x23c48b},
	{"carmine", 0x9d0216},
	{"grapefruit", 0xfd5956},
	{"ice", 0xd6fffa},
	{"algae", 0x54ac68},
	{"pinky", 0xfc86aa},
	{"darkblue", 0x030764},
	{"rosa", 0xfe86a4},
	{"lipstick", 0xd5174e},
	{"claret", 0x680018},
	{"dandelion", 0xfedf08},
	{"orangered", 0xfe420f},
	{"ruby", 0xca0147},
	{"dark", 0x1b2431},
	{"putty", 0xbeae8a},
	{"saffron", 0xfeb209},
	{"twilight", 0x4e518b},
	{"bluegrey", 0x85a3b2},
	{"petrol", 0x005f6a},
	{"royal", 0x0c1793},
	{"butter", 0xffff81},
	{"orangish", 0xfc824a},
	{"leaf", 0x71aa34},
	{"sunflower", 0xffc512},
	{"velvet", 0x750851},
	{"carnation", 0xfd798f},
	{"wisteria", 0xa87dc2},
	{"pale", 0xfff9d0},
	{"greyblue", 0x77a1b5},
	{"purpley", 0x8756e4},
	{"diarrhea", 0x9f8303},
	{"viridian", 0x1e9167},
	{"bile", 0xb5c306},
	{"spearmint", 0x1ef876},
	{"yellowgreen", 0xbbf90f},
	{"heather", 0xa484ac},
	{"mango", 0xffa62b},
	{"shamrock", 0x01b44c},
	{"bubblegum", 0xff6cb5},
	{"lightgreen", 0x76ff7b},
	{"merlot", 0x730039},
	{"apple", 0x6ecb3c},
	{"heliotrope", 0xd94ff5},
	{"dusk", 0x4e5481},
	{"kiwi", 0x9cef43},
	{"seaweed", 0x18d17b},
	{"iris", 0x6258c4},
	{"perrywinkle", 0x8f8ce7},
	{"tealish", 0x24bca8},
	{"pear", 0xcbf85f},
	{"sandy", 0xf1da7a},
	{"greyish", 0xa8a495},
	{"banana", 0xffff7e},
	{"tomato", 0xef4026},
	{"sea", 0x3c9992},
	{"buff", 0xfef69e},
	{"fawn", 0xcfaf7b},
	{"amethyst", 0x9b5fc0},
	{"chestnut", 0x742802},
	{"pea", 0xa4bf20},
	{"stone", 0xada587},
	{"earth", 0xa2653e},
	{"asparagus", 0x77ab56},
	{"blueberry", 0x464196},
	{"caramel", 0xaf6f09},
	{"ocher", 0xbf9b0c},
	{"lightblue", 0x7bc8f6},
	{"golden", 0xf5bf03},
	{"gunmetal", 0x536267},
	{"cherry", 0xcf0234},
	{"midnight", 0x03012d},
	{"blood", 0x770001},
	{"berry", 0x990f4b},
	{"poo", 0x8f7303},
	{"snot", 0xacbb0d},
	{"drab", 0x828344},
	{"rouge", 0xab1239},
	{"wheat", 0xfbdd7e},
	{"watermelon", 0xfd4659},
	{"mulberry", 0x920a4e},
	{"auburn", 0x9a3001},
	{"celadon", 0xbefdb7},
	{"celery", 0xc1fd95},
	{"strawberry", 0xfb2943},
	{"copper", 0xb66325},
	{"ivory", 0xffffcb},
	{"adobe", 0xbd6c48},
	{"barney", 0xac1db8},
	{"ocre", 0xc69c04},
	{"maize", 0xf4d054},
	{"sandstone", 0xc9ae74},
	{"camel", 0xc69f59},
	{"marine", 0x042e60},
	{"sepia", 0x985e2b},
	{"coffee", 0xa6814c},
	{"mocha", 0x9d7651},
	{"ecru", 0xfeffca},
	{"purpleish", 0x98568d},
	{"cranberry", 0x9e003a},
	{"melon", 0xff7855},
	{"silver", 0xc5c9c7},
	{"amber", 0xfeb308},
	{"vermillion", 0xf4320c},
	{"russet", 0xa13905},
	{"pine", 0x2b5d34},
	{"bluish", 0x2976bb},
	{"bronze", 0xa87900},
	{"shit", 0x7f5f00},
	{"dirt", 0x8a6e45},
	{"pistachio", 0xc0fa8b},
	{"yellowish", 0xfaee66},
	{"bordeaux", 0x7b002c},
	{"ocean", 0x017b92},
	{"marigold", 0xfcc006},
	{"steel", 0x738595},
	{"blush", 0xf29e8e},
	{"lemon", 0xfdff52},
	{"cerise", 0xde0c62},
	{"apricot", 0xffb16d},
	{"blurple", 0x5539cc},
	{"bluegreen", 0x017a79},
	{"forest", 0x0b5509},
	{"ultramarine", 0x2000b1},
	{"purplish", 0x94568c},
	{"reddish", 0xc44240},
	{"avocado", 0x90b134},
	{"umber", 0xb26400},
	{"poop", 0x7f5e00},
	{"eggshell", 0xffffd4},
	{"denim", 0x3b638c},
	{"evergreen", 0x05472a},
	{"aubergine", 0x3d0734},
	{"mahogany", 0x4a0100},
	{"mud", 0x735c12},
	{"brownish", 0x9c6d57},
	{"clay", 0xb66a50},
	{"jade", 0x1fa774},
	{"emerald", 0x01a049},
	{"sky", 0x82cafc},
	{"orchid", 0xc875c4},
	{"raspberry", 0xb00149},
	{"tangerine", 0xff9408},
	{"pumpkin", 0xe17701},
	{"charcoal", 0x343837},
	{"cornflower", 0x6a79f7},
	{"chocolate", 0x3d1c02},
	{"scarlet", 0xbe0119},
	{"sienna", 0xa9561e},
	{"terracotta", 0xca6641},
	{"grass", 0x5cac2d},
	{"moss", 0x769958},
	{"vomit", 0xa2a415},
	{"pinkish", 0xd46a7e},
	{"cobalt", 0x1e488f},
	{"wine", 0x80013f},
	{"azure", 0x069af3},
	{"grape", 0x6c3461},
	{"greenish", 0x40a368},
	{"coral", 0xfc5a50},
	{"cream", 0xffffc2},
	{"brick", 0xa03623},
	{"sage", 0x87ae73},
	{"white", 0xffffff},
	{"eggplant", 0x380835},
	{"puke", 0xa5a502},
	{"fuchsia", 0xed0dd9},
	{"crimson", 0x8c000f},
	{"ochre", 0xbf9005},
	{"cerulean", 0x0485d1},
	{"rust", 0xa83c09},
	{"slate", 0x516572},
	{"goldenrod", 0xfac205},
	{"seafoam", 0x80f9ad},
	{"puce", 0xa57e52},
	{"sand", 0xe2ca76},
	{"mint", 0x9ffeb0},
	{"chartreuse", 0xc1f80a},
	{"taupe", 0xb9a281},
	{"khaki", 0xaaa662},
	{"burgundy", 0x610023},
	{"plum", 0x580f41},
	{"gold", 0xdbb40c},
	{"glod", 0xffdf00},
	{"tasbot", 0x7a5b07},
	{"navy", 0x01153e},
	{"aquamarine", 0x04d8b2},
	{"rose", 0xcf6275},
	{"mustard", 0xceb301},
	{"indigo", 0x380282},
	{"lime", 0xaaff32},
	{"periwinkle", 0x8e82fe},
	{"peach", 0xffb07c},
	{"black", 0x000000},
	{"lilac", 0xcea2fd},
	{"beige", 0xe6daa6},
	{"salmon", 0xff796c},
	{"olive", 0x6e750e},
	{"maroon", 0x650021},
	{"mauve", 0xae7181},
	{"aqua", 0x13eac9},
	{"cyan", 0x00ffff},
	{"tan", 0xd1b26f},
	{"lavender", 0xc79fef},
	{"turquoise", 0x06c2ac},
	{"violet", 0x9a0eea},
	{"grey", 0x929591},
	{"yellow", 0xffff14},
	{"magenta", 0xc20078},
	{"orange", 0xf97306},
	{"teal", 0x029386},
	{"red", 0xe50000},
	{"brown", 0x653700},
	{"pink", 0xff81c0},
	{"blue", 0x0343df},
	{"green", 0x15b01a},
	{"purple", 0x7e1e9c}
};

draw_area::draw_area(QWidget *parent) : QWidget(parent)
{
	QDirIterator emo_it("../twitchemotes", QStringList() << "*.png", QDir::Files, QDirIterator::Subdirectories);
	while (emo_it.hasNext()){
		QString trigger = emo_it.fileName();
		trigger.chop(4);
		if(trigger.isEmpty()){
			emo_it.next();
			continue;
		}
		emotes.insert(trigger, new QPixmap(emo_it.filePath()));
		emo_it.next();
	}

	QDirIterator pic_it("../pictures", QStringList() << "*.png", QDir::Files, QDirIterator::Subdirectories);
	while (pic_it.hasNext()){
		pictures.push_back(new QImage(pic_it.next()));
	}

	image = new QImage(SNES_WIDTH, SNES_HEIGHT, QImage::Format_RGB32);

	for(int i = 0; i < pictures.count(); i++){
		current_picture = i;
		picture_coords.push_back({});
		update_picture();
		for(int x = 0; x < SNES_WIDTH; x++){
			for(int y = 0; y < SNES_HEIGHT; y++){
				if(image->pixel(x, y) == QColor(Qt::white).rgb()){
					picture_coords[i].push_back({x, y});
					flood_fill(x, y, Qt::white, Qt::black);
				}
			}
		}
		qDebug() << "parsed picture: " << current_picture << " points found: " << picture_coords[current_picture].count();
	}
	current_picture = 0;

	setMinimumSize(SNES_WIDTH, SNES_HEIGHT);

	qputenv("IRC_DEBUG", "0");
	
	irc.setHost(IRC_HOST);
	irc.setUserName("MrTASBot");
	irc.setPassword(IRC_PASSWORD);
	irc.setNickName("MrTASBot");
	irc.setRealName("MrTASBot");
	irc.join(IRC_CHANNEL);
	irc.setPort(IRC_PORT);
        irc.setSecure(IRC_USE_SSL);
	irc.setReconnectDelay(5);
	irc.set_emotes(emotes.keys());
	irc.set_colors(colors.keys());
	
	connect(&irc, &bot::statusChanged, this, [](IrcConnection::Status status){ qDebug() << status; });
	connect(&irc, &bot::trigger_color, this, &draw_area::register_color);
	connect(&irc, &bot::trigger_emote, this, &draw_area::register_emote);
	
	irc.open();
	update_picture();
	setFocus();
}

void draw_area::register_color(QString color, unsigned int space)
{
	QColor color_rgb;
	if (color.at(0).unicode() == 35) {   // "#"
		color_rgb = QColor(color);
	}
	else {
		color_rgb = colors[color.toLower()];
	}
	if(!color_rgb.isValid() || !(color_rgb.rgb() & 0xFFFFFF)){
		return;
	}
	auto coords = picture_coords[current_picture][space % picture_coords[current_picture].size()];
	flood_fill(coords.first, coords.second, QColor(image->pixel(coords.first, coords.second)), color_rgb);
	update();
	qDebug() << "registered color" << color << space << space % picture_coords[current_picture].size();
}

void draw_area::register_emote(QString emote, int x, int y)
{
	registered_emotes.append({emote, x, y});
	update();
	qDebug() << "registered emote" << emote << x << y;
}
void draw_area::paintEvent(QPaintEvent *event)
{
	Q_UNUSED (event);
	QPainter painter(this);
	
	painter.drawImage(0, 0, *image);

	for(const auto &emote : registered_emotes){
		painter.drawPixmap(emote.x, emote.y, *emotes[emote.name]);
	}
	
}

void draw_area::keyPressEvent(QKeyEvent *event)
{
	qDebug() << event;
	switch(event->key()){
		case Qt::Key_N:
			current_picture++;
			if(current_picture >= pictures.count()){
				current_picture = 0;
			}
			update_picture();
		break;
		case Qt::Key_P:
			current_picture--;
			if(current_picture < 0){
				current_picture = pictures.count() - 1;
			}
			update_picture();
		break;
		case Qt::Key_R:
			update_picture();
		break;
	}
	update();
}


void draw_area::flood_fill(int x, int y, QColor target_color, QColor new_color)
{
	QRgb target = target_color.rgb();
	if(target_color == new_color){
		return;
	}

	stack.clear();

	stack.push({x, y});

	while(stack.count()){
		auto coords = stack.pop();
		x = coords.first;
		y = coords.second;
		while(x >= 0 && image->pixel(x, y) == target) x--;
		x++;
		bool above = false;
		bool below = false;
		while(x < SNES_WIDTH && image->pixel(x, y) == target){
			image->setPixel(x, y, new_color.rgb());

			if(!above && y > 0 && image->pixel(x, y - 1) == target){
				stack.push({x, y - 1});
				above = true;
			}else if(above && y > 0 && image->pixel(x, y - 1) != target){
				above = false;
			}

			if(!below && y < SNES_HEIGHT - 1 && image->pixel(x, y + 1) == target){
				stack.push({x, y + 1});
				below = true;
			}else if(below && y < SNES_HEIGHT - 1 && image->pixel(x, y + 1) != target){
				below = false;
			}
			x++;
		}
	}
}

void draw_area::update_picture()
{
	registered_emotes.clear();
	image->fill(Qt::white);
	if(!pictures.count()){
		return;
	}
	QPainter painter(image);
	painter.drawImage(0, 0, *pictures[current_picture]);
}
