#include "main_window.h"
#include "draw_area.h"

main_window::main_window(QWidget *parent)
        : QMainWindow(parent)
{
	setCentralWidget(new draw_area(this));
}

main_window::~main_window()
{
	
}
