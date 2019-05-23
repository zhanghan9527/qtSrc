#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setWindowTitle("QtTranslationTool v1.5.0215.1");
    w.show();
    return a.exec();
}
