#include "CutPicture.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CutPicture w;
    w.setWindowTitle("图片裁剪工具v1.0");

    w.show();

    return a.exec();
}
