#include "PNTpackage.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PNTpackage w;
    w.setWindowTitle("压缩工具 v1.2");
    w.show();

    return a.exec();
}
