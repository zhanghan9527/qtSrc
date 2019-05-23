#include "CheckSum.h"
#include "ui_CheckSum.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QDebug>

CheckSum::CheckSum(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CheckSum)
{
    //setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint|Qt::Tool|Qt::X11BypassWindowManagerHint);




    ui->setupUi(this);
    ui->lineEditSrc->setText("");    
}

CheckSum::~CheckSum()
{
    delete ui;
}

void CheckSum::on_btnSrc_clicked()
{
    QString fileSrc = QFileDialog::getOpenFileName(this,
                                            tr("打开文件"),
                                            QApplication::applicationDirPath(),
                                            tr("binary file (*.bin)"));
    if(fileSrc.isEmpty())
        return;

    ui->lineEditSrc->setText(fileSrc);
}


void CheckSum::on_btnConvert_clicked()
{
    if(ui->lineEditSrc->text().isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("未选择待转换的二进制文件！"));
        return;
    }


    if(convert(ui->lineEditSrc->text()))
    {
        QMessageBox::information(this, tr("提示"), tr("转换完成！"));
    }

}


bool CheckSum::convert(const QString &fileSrc)
{
    QFile _fileSrc(fileSrc);
    int fileSize = _fileSrc.size();
    if(fileSize%2) fileSize++;

    if(!_fileSrc.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("提示"), QString(tr("打开文件:%1 失败！").arg(fileSrc)));
        return false;
    }

    char *pSrcBuffer = new char[fileSize];
    if(pSrcBuffer == nullptr) {
        QMessageBox::warning(this, tr("提示"), QString(tr("内存分配失败！")));
        return false;
    }

    for(int i=0; i<fileSize; i++)
        pSrcBuffer[i] = 0;

    _fileSrc.read(pSrcBuffer, _fileSrc.size());
    _fileSrc.close();

    quint16 val = 0;
    quint16 m_sum = 0;

    for(int i=0; i<fileSize; i+=2) {
        val = *(quint16 *)&pSrcBuffer[i];
         qDebug() << "val" << val;
        m_sum = m_sum + val;

    }
    qDebug() << "m_sum" << m_sum;

    ui->lineEditSrc_2->setText(QString::number(m_sum, 16));
    delete[] pSrcBuffer;
    return true;
}


