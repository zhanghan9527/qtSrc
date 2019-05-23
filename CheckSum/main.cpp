#include "CheckSum.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CheckSum w;
    w.setWindowTitle("CheckSum");
    w.show();

    return a.exec();
}
