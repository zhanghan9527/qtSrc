#include "CutPicture.h"
#include "ui_CutPicture.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QDebug>
#include <qimage.h>
#include <qsettings.h>

QButtonGroup  *BG;//按钮组

const quint8 colorhead[0x36] = {0x42,0x4d,0x38,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0x28,0x00,
                                0x00,0x00,0x80,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x01,0x00,0x18,0x00,0x00,0x00,
                                0x00,0x00,0x02,0xc0,0x00,0x00,0x20,0x2e,0x00,0x00,0x20,0x2e,0x00,0x00,0x00,0x00,
                                0x00,0x00,0x00,0x00,0x00,0x00};

CutPicture::CutPicture(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CutPicture)
{
    ui->setupUi(this);
     m_xaxis = 0;
     m_yaxis = 3;
     m_xnum = 1;
     m_ynum = 1;




    QSettings settings(QApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    QString lastFilePath = settings.value("LastTsFilePath").toString();
    if(!lastFilePath.isEmpty())
        ui->lineEditSrc->setText(lastFilePath);


    ui->label->setText("Tips:将图片裁剪成128*128格式");
}

CutPicture::~CutPicture()
{
    delete ui;
}

void CutPicture::on_btnSrc_clicked()
{
    QString xlsFile;
    QSettings settings(QApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    QString fileSrc = settings.value("LastFilePath").toString();

    if(fileSrc.isEmpty())
        xlsFile = QFileDialog::getOpenFileName(this,QString(),QString(),"(*.bmp)");
    else
        xlsFile = QFileDialog::getOpenFileName(this,QString(),fileSrc,"(*.bmp)");

    if(!xlsFile.isEmpty())
        settings.setValue("LastFilePath", xlsFile);

    if(xlsFile.isEmpty())
        return;

    ui->lineEditSrc->setText(xlsFile);


    QFile _fileSrc(xlsFile);
    int fileSize = _fileSrc.size();//源文件的大小

    if(!_fileSrc.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("提示"), QString(tr("读取文件:%1 失败！").arg(fileSrc)));
        return ;
    }
    char *pSrcBuffer = new char[fileSize];

    if(pSrcBuffer == nullptr) {
        QMessageBox::warning(this, tr("提示"), QString(tr("内存分配失败！")));
        return ;
    }
    for(int i=0; i<fileSize; i++)
        pSrcBuffer[i] = 0;

    _fileSrc.read(pSrcBuffer, _fileSrc.size());
    _fileSrc.close();
    //图像宽和高点像素
    for(int i = 0; i < 0x36; i = i +2)
    {
        if(i == 18)
        {
            m_width = *(quint16 *)& pSrcBuffer[i];
        }
        if(i == 22)
        {
            m_height = *(quint16 *)& pSrcBuffer[i];
        }
    }
    m_xnum = m_width / 130;   
    m_ynum = m_height / 130;

}

void CutPicture::on_btnSave_clicked()
{
}

void CutPicture::on_btnConvert_clicked()
{
    if(ui->lineEditSrc->text().isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("未选择待转换的bmp文件！"));
        return;
    }

    QString srcfile = ui->lineEditSrc->text();
    QString savedfile = srcfile;

    savedfile = savedfile.left(savedfile.lastIndexOf("."));

    int x_y =0;
    x_y=m_xnum * m_ynum;

    for(int i = 0; i < x_y; i++)
    {
        m_xaxis = i % m_xnum;
        m_yaxis = (m_ynum - 1) - (i / m_xnum);

        QString tempstr = QString("_%1").arg(QString::number(i));

        tempstr = tempstr + ".bmp";

        savedfile = savedfile + tempstr;
        if(convert(srcfile, savedfile) == false)
        {
            return;
        }

       savedfile = savedfile.left(savedfile.lastIndexOf("_"));
    }
     QMessageBox::information(this, tr("提示"), tr("转换完成！"));

}


bool CutPicture::convert(const QString &fileSrc, const QString &fileDes)
{
    QFile _fileSrc(fileSrc);
    int fileSize = _fileSrc.size();//源文件的大小
    int imageSize  = 49208; //0x36+128*128*3+2


    if(!_fileSrc.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("提示"), QString(tr("打开文件:%1 失败！").arg(fileSrc)));
        return false;
    }
    char *pSrcBuffer = new char[fileSize];
    char *pSrcBuffer_des = new char[imageSize];

    if(pSrcBuffer == nullptr) {
        QMessageBox::warning(this, tr("提示"), QString(tr("内存分配失败！")));
        return false;
    }

    for(int i=0; i<fileSize; i++)
        pSrcBuffer[i] = 0;
    for(int i=0; i<imageSize; i++)
        pSrcBuffer_des[i] = 0;

    _fileSrc.read(pSrcBuffer, _fileSrc.size());
    _fileSrc.close();

    QFile _fileDes(fileDes);
    if(!_fileDes.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("提示"), QString(tr("打开文件:%1 失败！").arg(fileSrc)));
        delete[] pSrcBuffer;
        delete[] pSrcBuffer_des;
        return false;
    }    
    //存储成为128*128图片，*3表示颜色深度为24
    for(int i= 0; i< (0x36+128*128*3); i++)
    {
        if(i < 0x36)//head部分
        {
            pSrcBuffer_des[i] = colorhead[i];            
        }
        else//data部分
        {
            int temp = m_xnum*130;

            pSrcBuffer_des[i] = pSrcBuffer[(1+130*m_xaxis)*3+ (1+130*m_yaxis)*temp*3 +  ((i-0x36)/(128*3))*(temp-128)* 3 + i];
        }
        // end 部分  0x00 0x00 为初始化值，写入时无需改值
    }

    _fileDes.write(pSrcBuffer_des,  imageSize);
    _fileDes.close();
    delete[] pSrcBuffer_des;
    delete[] pSrcBuffer;
    return true;
}





