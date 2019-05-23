#include "BitSwitchTool.h"
#include "ui_BitSwitchTool.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QDebug>
#include <qimage.h>

BitSwitchTool::BitSwitchTool(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BitSwitchTool)
{
    ui->setupUi(this);
    ui->lineEditSrc->setText("");
    ui->lineEditDes->setText("");

//   ui->btnSrc->setStyleSheet("QPushButton{border:1px solid lightgray;background:rgb(230,230,230);}"
//                             "QPushButton:hover{border-color:green;background:transparent;}");

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
//    ui->btnConvert->setStyleSheet("border:2px groove gray;border-radius:10px;padding:2px 4px;");

    clearChoiceArray();
    //QFont font ( 0, 0, 50);
    //ui->checkBox_0->setFont(font);




    //默认第一选择为位交换
    ui->comboBox->addItem(tr("word位交换"));
    ui->comboBox->addItem(tr("(8位)地址交换"));
    ui->comboBox->addItem(tr("(16位)地址交换"));
}

BitSwitchTool::~BitSwitchTool()
{
    delete ui;
}

QString BitSwitchTool::getcomboBox()
{
    return  ui->comboBox->currentText();
}

void BitSwitchTool::on_btnSrc_clicked()
{
    QString fileSrc = QFileDialog::getOpenFileName(this,
                                            tr("打开文件"),
                                            QApplication::applicationDirPath(),
                                            tr("binary file (*.bin)"));
    if(fileSrc.isEmpty())
        return;

    ui->lineEditSrc->setText(fileSrc);
}

void BitSwitchTool::on_btnSave_clicked()
{
    QString fileDes = QFileDialog::getSaveFileName(this,
                                                   tr("保存文件"),
                                                   QApplication::applicationDirPath(),
                                                   tr("binary file (*.bin)"));
    if(fileDes.isEmpty())
        return;

    ui->lineEditDes->setText(fileDes);
}

void BitSwitchTool::on_btnConvert_clicked()
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
        clearChoiceArray();
    }
}


bool BitSwitchTool::convert(const QString &fileSrc, const QString &fileDes)
{
    //处理选择的互换bit
    setCheckBoxStatu();
    int num = getChoiceNum();
    if(num != 2)
    {
        QMessageBox::information(this, tr("提示"), tr("请选择两个互换的bit"));
        return false;
    }

    QFile _fileSrc(fileSrc);
    int fileSize = _fileSrc.size();//源文件的大小
    qDebug() << "fileSize" << _fileSrc.size();

    if(fileSize%2) fileSize++;//数据异常处理

    if(!_fileSrc.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("提示"), QString(tr("打开文件:%1 失败！").arg(fileSrc)));
        return false;
    }
    char *pSrcBuffer = new char[fileSize];
    char *pSrcBuffer_temp = new char[fileSize];
    char *pSrcBuffer_new = new char[fileSize*2];
    if(pSrcBuffer == nullptr) {
        QMessageBox::warning(this, tr("提示"), QString(tr("内存分配失败！")));
        return false;
    }

    for(int i=0; i<fileSize; i++)
        pSrcBuffer[i] = 0;
    for(int i=0; i<fileSize; i++)
        pSrcBuffer_temp[i] = 0;
    for(int i=0; i<fileSize*2; i++)
        pSrcBuffer_new[i] = 0;

    _fileSrc.read(pSrcBuffer, _fileSrc.size());
    _fileSrc.close();

    QFile _fileDes(fileDes);
    if(!_fileDes.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("提示"), QString(tr("打开文件:%1 失败！").arg(fileSrc)));
        delete[] pSrcBuffer;
        delete[] pSrcBuffer_temp;
        delete[] pSrcBuffer_new;
        return false;
    }
    if(getcomboBox() == "(8位)地址交换")
    {
        //地址操作
       // quint8 val = 0;
        unsigned char bit1 = 0;
        unsigned char bit2 = 0;

        for(int i=0; i<fileSize; i++)
        {
            int temp = i;

            bit1 = (temp >> m_bit1change)&0x01;
            bit2 = (temp >> m_bit2change)&0x01;
            // 置零操作
            temp &= (~(1ul << m_bit1change));
            temp &= (~(1ul << m_bit2change));
            // 互换bit
            temp |= (bit1 << m_bit2change);
            temp |= (bit2 << m_bit1change);

             *&pSrcBuffer_new[temp] = *&pSrcBuffer[i];
        }
        _fileDes.write(pSrcBuffer_new, _fileSrc.size());
    }
    else if(getcomboBox() == "(16位)地址交换")
    {
        //地址操作
        quint16 val = 0;
        unsigned char bit1 = 0;
        unsigned char bit2 = 0;

        for(int i=0, j = 0; i<fileSize; i=i+2, j++)
        {
            val = *(quint16 *)&pSrcBuffer[i];
            *&pSrcBuffer_temp[j] = val;
         }
        for(int j = 0; j < fileSize; j ++)
        {
            int temp = j;

            bit1 = (temp >> m_bit1change)&0x01;
            bit2 = (temp >> m_bit2change)&0x01;
            // 置零操作
            temp &= (~(1ul << m_bit1change));
            temp &= (~(1ul << m_bit2change));
            // 互换bit
            temp |= (bit1 << m_bit2change);
            temp |= (bit2 << m_bit1change);

             *&pSrcBuffer[temp] = *(quint8 *)&pSrcBuffer_temp[j];
        }
        _fileDes.write(pSrcBuffer, _fileSrc.size());
    }
    else if(getcomboBox() == "word位交换")
    {
        //位操作
        quint16 val = 0;
        unsigned char bit1 = 0;
        unsigned char bit2 = 0;

        for(int i=0; i<fileSize; i+=2) {
            val = *(quint16 *)&pSrcBuffer[i];

            bit1 = (val >> m_bit1change)&0x01;
            bit2 = (val >> m_bit2change)&0x01;
            // 置零操作
            val &= (~(1ul << m_bit1change));
            val &= (~(1ul << m_bit2change));
            // 互换bit
            val |= (bit1 << m_bit2change);
            val |= (bit2 << m_bit1change);
            *(quint16 *)&pSrcBuffer[i] = val;
        }

        _fileDes.write(pSrcBuffer, _fileSrc.size());
    }
//    qDebug()  << "m_bit1change = " << m_bit1change;
//    qDebug()  << "m_bit2change = " << m_bit2change;

    _fileDes.close();

    delete[] pSrcBuffer;
    delete[] pSrcBuffer_temp;
    delete[] pSrcBuffer_new;
    return true;
}


//获取选择的checkbox数量
int BitSwitchTool::getChoiceNum()
{
    int num = 0;
    for(int i = 0; i < 20; i++)
    {
        if(m_checkbox[i] != 255)
        {
            if(m_bit1change == 0)
                m_bit1change = m_checkbox[i];
            else
                m_bit2change = m_checkbox[i];

            num++;
        }
    }   
    return num;
}

//获取checkbox状态
void BitSwitchTool::setCheckBoxStatu()
{
    if(ui->checkBox_0->isChecked() == false)
    {
        m_checkbox[0] = 255;
    }
    else
    {
        m_checkbox[0] = 0;
    }
    if(ui->checkBox_1->isChecked() == false)
    {
        m_checkbox[1] = 255;
    }
    else
    {
        m_checkbox[1] = 1;
    }
    if(ui->checkBox_2->isChecked() == false)
    {
        m_checkbox[2] = 255;
    }
    else
    {
        m_checkbox[2] = 2;
    }
    if(ui->checkBox_3->isChecked() == false)
    {
        m_checkbox[3] = 255;
    }
    else
    {
        m_checkbox[3] = 3;
    }
    if(ui->checkBox_4->isChecked() == false)
    {
        m_checkbox[4] = 255;
    }
    else
    {
        m_checkbox[4] = 4;
    }
    if(ui->checkBox_5->isChecked() == false)
    {
        m_checkbox[5] = 255;
    }
    else
    {
        m_checkbox[5] = 5;
    }
    if(ui->checkBox_6->isChecked() == false)
    {
        m_checkbox[6] = 255;
    }
    else
    {
        m_checkbox[6] = 6;
    }
    if(ui->checkBox_7->isChecked() == false)
    {
        m_checkbox[7] = 255;
    }
    else
    {
        m_checkbox[7] = 7;
    }
    if(ui->checkBox_8->isChecked() == false)
    {
        m_checkbox[8] = 255;
    }
    else
    {
        m_checkbox[8] = 8;
    }
    if(ui->checkBox_9->isChecked() == false)
    {
        m_checkbox[9] = 255;
    }
    else
    {
        m_checkbox[9] = 9;
    }
    if(ui->checkBox_10->isChecked() == false)
    {
        m_checkbox[10] = 255;
    }
    else
    {
        m_checkbox[10] = 10;
    }
    if(ui->checkBox_11->isChecked() == false)
    {
        m_checkbox[11] = 255;
    }
    else
    {
        m_checkbox[11] = 11;
    }
    if(ui->checkBox_12->isChecked() == false)
    {
        m_checkbox[12] = 255;
    }
    else
    {
        m_checkbox[12] = 12;
    }
    if(ui->checkBox_13->isChecked() == false)
    {
        m_checkbox[13] = 255;
    }
    else
    {
        m_checkbox[13] = 13;
    }
    if(ui->checkBox_14->isChecked() == false)
    {
        m_checkbox[14] = 255;
    }
    else
    {
        m_checkbox[14] = 14;
    }
    if(ui->checkBox_15->isChecked() == false)
    {
        m_checkbox[15] = 255;
    }
    else
    {
        m_checkbox[15] = 15;
    }

}

void BitSwitchTool::clearChoiceArray()
{
    //设置checkbox未选中
    ui->checkBox_0->setCheckState(Qt::Unchecked);
    ui->checkBox_1->setCheckState(Qt::Unchecked);
    ui->checkBox_2->setCheckState(Qt::Unchecked);
    ui->checkBox_3->setCheckState(Qt::Unchecked);
    ui->checkBox_4->setCheckState(Qt::Unchecked);
    ui->checkBox_5->setCheckState(Qt::Unchecked);
    ui->checkBox_6->setCheckState(Qt::Unchecked);
    ui->checkBox_7->setCheckState(Qt::Unchecked);
    ui->checkBox_8->setCheckState(Qt::Unchecked);
    ui->checkBox_9->setCheckState(Qt::Unchecked);
    ui->checkBox_10->setCheckState(Qt::Unchecked);
    ui->checkBox_11->setCheckState(Qt::Unchecked);
    ui->checkBox_12->setCheckState(Qt::Unchecked);
    ui->checkBox_13->setCheckState(Qt::Unchecked);
    ui->checkBox_14->setCheckState(Qt::Unchecked);
    ui->checkBox_15->setCheckState(Qt::Unchecked);

    for(int i = 0; i < 20; i++)
    {
        m_checkbox[i] = 255;
    }

    m_bit1change = 0;
    m_bit2change = 0;
}

void BitSwitchTool::on_BitSwitchTool_accepted()
{


}

//void BitSwitchTool::on_comboBox_currentTextChanged(const QString &arg1)
//{
//    clearChoiceArray();

//}
