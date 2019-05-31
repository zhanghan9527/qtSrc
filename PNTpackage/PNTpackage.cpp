#include "PNTpackage.h"
#include "ui_PNTpackage.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QDebug>
#include <qimage.h>

PNTpackage::PNTpackage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PNTpackage)
{
    ui->setupUi(this);
    ui->lineEditSrc->setText("");
    ui->lineEditDes->setText("");

    for(int i = 0; i< 127; i++) m_diffbuf[i] = -1;

    ui->btnSrc->setStyleSheet("QPushButton{background-color:rgba(255,178,0,100%);\
                                               color: white;   border-radius: 10px;  border: 2px groove gray; border-style: outset;}" // 按键本色
           "QPushButton:hover{background-color:white; color: black;}"  // 鼠标停放时的色彩
           "QPushButton:pressed{background-color:rgb(85, 170, 255); border-style: inset; }"   // 鼠标按下的色彩
            );
      ui->btnSave->setStyleSheet("QPushButton{background-color:rgba(255,178,0,100%);\
                                              color: white;   border-radius: 10px;  border: 2px groove gray; border-style: outset;}" // 按键本色
          "QPushButton:hover{background-color:white; color: black;}"  // 鼠标停放时的色彩
          "QPushButton:pressed{background-color:rgb(85, 170, 255); border-style: inset; }"   // 鼠标按下的色彩
           );

}

PNTpackage::~PNTpackage()
{
    delete ui;
}



void PNTpackage::on_btnSrc_clicked()
{
    QString fileSrc = QFileDialog::getOpenFileName(this,
                                            tr("打开文件"),
                                            QApplication::applicationDirPath(),
                                            tr("binary file (*.pnt)"));
    if(fileSrc.isEmpty())
        return;

    ui->lineEditSrc->setText(fileSrc);
}

void PNTpackage::on_btnSave_clicked()
{
    QString fileDes = QFileDialog::getSaveFileName(this,
                                                   tr("保存文件"),
                                                   QApplication::applicationDirPath(),
                                                   tr("binary file (*.pnt)"));
    if(fileDes.isEmpty())
        return;

    ui->lineEditDes->setText(fileDes);
}

void PNTpackage::on_btnConvert_clicked()
{
    if(ui->lineEditSrc->text().isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("未选择待转换的二进制文件！"));
        return;
    }

    if(ui->lineEditDes->text().isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("未选择转换后保存的二进制文件！"));
        return;
    }

    if(convert(ui->lineEditSrc->text(), ui->lineEditDes->text())) {
        QMessageBox::information(this, tr("提示"), tr("转换完成！"));

    }
}


bool PNTpackage::convert(const QString &fileSrc, const QString &fileDes)
{
    QFile _fileSrc(fileSrc);
    int fileSize = _fileSrc.size();//源文件的大小

    if(!_fileSrc.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("提示"), QString(tr("打开文件:%1 失败！").arg(fileSrc)));
        return false;
    }
    char *pSrcBuffer = new char[fileSize];
    char *pSrcBuffer_new = new char[fileSize];
    if(pSrcBuffer == nullptr) {
        QMessageBox::warning(this, tr("提示"), QString(tr("内存分配失败！")));
        return false;
    }

    for(int i=0; i<fileSize; i++)
        pSrcBuffer[i] = 0;

    for(int i=0; i<fileSize; i++)
        pSrcBuffer_new[i] = 0;

    _fileSrc.read(pSrcBuffer, _fileSrc.size());
    _fileSrc.close();

    QFile _fileDes(fileDes);
    if(!_fileDes.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("提示"), QString(tr("打开文件:%1 失败！").arg(fileSrc)));
        delete[] pSrcBuffer;
        delete[] pSrcBuffer_new;
        return false;
    }
    for(int i= 0; i<fileSize; i++)
    {
        char ver = *&pSrcBuffer[i];
        ver = ~ ver;
        *&pSrcBuffer[i] = ver;
    }

    char val = 0, val_2 = 0;
    int same_num = 0;
    int diff_num = 0;
    int i_diff = 0;
    int i_new = 0;
    char standardnum =  *&pSrcBuffer[0];


    for(int i= 1; i<fileSize; i++)
    {

        val = *&pSrcBuffer[i];

        if(i < fileSize - 1)
            val_2 =  *&pSrcBuffer[i + 1];
        else
            val_2 =  *&pSrcBuffer[i - 1];

        if(standardnum == val)
        {
            //写入不相同的字节
            if(diff_num)
            {
                *&pSrcBuffer_new[i_new] = (char)(diff_num + 0x80);
                i_new ++;

                for(int i= 0; i < diff_num; i++, i_new++)
                {
                    *&pSrcBuffer_new[i_new] = (char)m_diffbuf[i];
                }
                diff_num = 0;
                i_diff = 0;
            }
            else
            {
                //读取字节相同
                if(same_num < 127)
                {
                    same_num ++;
                    if(i == fileSize - 1)
                    {
                        //写入相同的字节
                        *&pSrcBuffer_new[i_new] = (char)same_num;
                        i_new ++;
                        *&pSrcBuffer_new[i_new] = (char)standardnum;
                        i_new ++;
                        same_num = 0;
                    }
                }
                else if(same_num == 127)
                {
                    //写入相同的字节
                    *&pSrcBuffer_new[i_new] = (char)same_num;
                    i_new ++;
                    *&pSrcBuffer_new[i_new] = (char)standardnum;
                    i_new ++;
                    same_num = 1;//超过127问题

                }
            }
        }
        else if(standardnum != val)
        {
            //相同的字节
            if(same_num)
            {
                same_num ++;
                *&pSrcBuffer_new[i_new] = (char)same_num;
                i_new ++;
                *&pSrcBuffer_new[i_new] = (char)standardnum;
                i_new ++;
                same_num = 0;
                standardnum = val;
            }
            else
            {
                if(val == val_2)
                {
                    diff_num ++;
                    m_diffbuf[i_diff] = standardnum;
                    i_diff++;
                    standardnum = val;

                    *&pSrcBuffer_new[i_new] = (char)(diff_num + 0x80);
                    i_new ++;

                    for(int i= 0; i < diff_num; i++, i_new++)
                    {
                        *&pSrcBuffer_new[i_new] = (char)m_diffbuf[i];
                    }
                    diff_num = 0;
                    i_diff = 0;
                    same_num = 0;//相同到相同的问题
                }
                else if(val != val_2)
                {
                    //qDebug() << "读取异同";
                    if(diff_num < 127)
                    {
                        diff_num ++;
                        m_diffbuf[i_diff] = standardnum;
                        i_diff++;
                        standardnum = val;

                        if(i == fileSize - 1)
                        {
                            diff_num ++;
                            m_diffbuf[i_diff] = standardnum;
                            i_diff++;
                            standardnum = val;

                            *&pSrcBuffer_new[i_new] = (char)(diff_num + 0x80);
                            i_new ++;

                            for(int i= 0; i < diff_num; i++, i_new++)
                            {
                                *&pSrcBuffer_new[i_new] = (char)m_diffbuf[i];
                            }
                            diff_num = 0;
                            i_diff = 0;
                        }
                    }
                    else
                    {
                        //写入bu同的字节
                        *&pSrcBuffer_new[i_new] = (char)(diff_num + 0x80);
                        i_new ++;
                        for(int i= 0; i < diff_num; i++, i_new++)
                        {
                            *&pSrcBuffer_new[i_new] = (char)m_diffbuf[i];
                        }
                        diff_num = 0;
                        i_diff = 0;
                    }
                }
            }
        }


    }


    _fileDes.write(pSrcBuffer_new, i_new);
    _fileDes.close();

    delete[] pSrcBuffer;
    delete[] pSrcBuffer_new;

    return true;
}


