#include "widget.h"
#include "rodentchangeform.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    //RodentChangeForm r;
    //r.show();
    return a.exec();
}


