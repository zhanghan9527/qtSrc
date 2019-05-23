#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

QVector<QString> tempstring;


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{   
    ui->setupUi(this);
    this->m_excelHandle =  new ExcelHandle(this);
    this->m_xmlHandle = new XmlHandle(this);

    m_readXmlTextList << "source" << "translation";
    m_LanguageList.clear();

    m_translatenumber = 0;



    initScreen();
    setCheckBoxNum(0);
}


void Widget::initScreen()
{
//    QSettings settings(QApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
//    QString lastPath = settings.value("LastFilePath").toString();
//    if(!lastPath.isEmpty())
//        ui->lineEdit->setText(lastPath);

    QSettings settings1(QApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    QString lastFilePath = settings1.value("LastTsFilePath").toString();
    if(!lastFilePath.isEmpty())
        ui->tsLineEdit->setText(lastFilePath);

    QSettings settings2(QApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    QString outputPath = settings2.value("OutputPath").toString();

    if(!outputPath.isEmpty())
        ui->lineEdit_output->setText(outputPath);

    ui->label_3->setText(tr("请导入excel或者txt文件：（最多支持10种多语言）"));
    ui->label_5->setText(tr("请导入需要翻译的ts文件："));
}


Widget::~Widget()
{
    delete ui;
}

//导入xecel或者txt
void Widget::on_pushButton_clicked()
{
    QString xlsFile;
    QVariant var;
    QString language;
    QMap<QString,QString> textMap;
    int lanuageCount,rowCount;

    clearChoiceArray();//清除已勾选的
    setCheckBoxNum(0);//清除上次的选项
    m_dataVarList.clear();//每次导入前先清除list

    QSettings settings(QApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    QString lastPath = settings.value("LastFilePath").toString();

    if(lastPath.isEmpty())
        xlsFile = QFileDialog::getOpenFileName(this,QString(),QString(),"(*.xls *.xlsx *.txt *.csv)");
    else
        xlsFile = QFileDialog::getOpenFileName(this,QString(),lastPath,"(*.xls *.xlsx *.txt *.csv)");

    if(!xlsFile.isEmpty())
        settings.setValue("LastFilePath", xlsFile);

    if(xlsFile.isEmpty())
        return;

    m_FilePath = xlsFile;
    ui->lineEdit->setText(xlsFile);
    int position = m_FilePath.lastIndexOf(".");
    QString xlsFileType =xlsFile.right(xlsFile.length() - position);

    if(xlsFileType == ".xls" || xlsFileType == ".xlsx")
    {
        var = m_excelHandle->excelLocalFileRead(xlsFile);
        m_excelHandle->qvariantToList(var, m_dataVarList);

        lanuageCount = m_dataVarList.at(0).count() - 1;
        if(lanuageCount > 10)
        {
            setCheckBoxNum(0);
            QMessageBox::warning(this, tr("warn"),tr("最多只能翻译10种语言！"));
            return;
        }
        setCheckBoxNum(lanuageCount);
        rowCount = m_dataVarList.count();

        if(lanuageCount)
        {
            m_LanguageMap.clear();
            m_LanguageList.clear();
            for(int i = 1; i <= lanuageCount; i++)
            {
                textMap.clear();
                language = QString(m_dataVarList.at(0).at(i).toByteArray());
                setCheckBoxName(i,language);
                if(i < 11)//最多支持10种多语言
                    m_language[i -1] = language;//保存勾选的复选框
                m_LanguageList<<language;

                for(int j = 1; j < rowCount; j++)
                {
                    QString key,value;
                    key = QString(m_dataVarList.at(j).at(0).toByteArray());
                    value = QString::fromUtf8(m_dataVarList.at(j).at(i).toByteArray());
                    textMap.insert(key,value);
                }
                m_LanguageMap.insert(language,textMap);
            }
        }
    }
    else if(xlsFileType == ".txt" || xlsFileType == ".csv")
    {
        getStringMapFromCsv(xlsFile);
    }
}

void Widget::getStringMapFromCsv(QString filePath)
{
    QStringList lanuageList;
    QList <QMap<QString,QString>> tmpLanguageMapList;
    int lanuageCount = 0;
    QStringList textList;
    bool firstLine = true;
    QFile *readFile = new QFile(filePath);
    if(readFile->open(QIODevice::ReadOnly))
    {
        QTextStream textStream(readFile);
        while (!textStream.atEnd())
        {
            QString tempText = textStream.readLine();
            tempText = tempText + "";
            textList = tempText.split("\t");

            if(firstLine)
            {
                m_LanguageList.clear();
                lanuageList = textList;
                lanuageList.removeFirst();
                m_LanguageList = lanuageList;
                lanuageCount = lanuageList.count();

                setCheckBoxNum(lanuageCount);
                for(int i= 0; i < lanuageCount; i++)
                {
                    setCheckBoxName(i + 1,m_LanguageList.at(i));
                }
                firstLine = false;            
            }
            else
            {
                //翻译的语言与内容（去除掉英文）保持一致
                while((textList.count() -1) != lanuageCount)
                {
                    textList.append("");
                }              
                for(int i = 1; i <= lanuageCount; i++)
                {
                    QString key,value;
                    key = textList.at(0);
                    value = textList.at(i);
                    if(tmpLanguageMapList.count() < lanuageCount)
                    {
                        QMap<QString, QString> tempMap;
                        tempMap.insert(key, value);
                        tmpLanguageMapList << tempMap;
                    }
                    else
                    {
                        QMap <QString, QString> tempMap;
                        tempMap = tmpLanguageMapList.at(i-1);
                        tempMap.insert(key, value);
                        tmpLanguageMapList.replace(i-1,tempMap);
                    }
                }
            }
        }
        for(int j = 0; j < lanuageCount; j++)
        {
            m_LanguageMap.insert(lanuageList.at(j),tmpLanguageMapList.at(j));
        }
    }
}

void Widget::on_createBtAll_clicked()
{
    QString srcFile1,srcFile2,tempsrcFile;

    QSettings settings(QApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    QString outputPath = settings.value("OutputPath").toString();

    ui->textBrowser->clear();

    int fileCount = 0;
    QString tmpFileName, srcFile;
    QMap<QString,QString> replaceMap;

    if(ui->tsLineEdit->text().isEmpty())
    {
        QMessageBox::warning(this, tr("warn"),tr("Create file failed,need to import ts file first"));
        return;
    }
    srcFile = ui->tsLineEdit->text();

    setCheckBoxStatu();

    if(judgewhichlanguage() == 0)//没有选中任何一种语言
    {
        return;
    }

    for(int i = 0; i < m_LanguageList.count(); i++)
    {
        if (m_language[i] == "")
        {
            continue;
        }
        int pos1 = srcFile.lastIndexOf("_");
        int pos2 = srcFile.lastIndexOf(".");
        int pos3 = srcFile.lastIndexOf("/");

        if(pos1 < pos3)//TS文件不带"_"的情况
        {
            srcFile1 = srcFile.left(pos2) + "_";
            srcFile2 = srcFile.right(srcFile.length() - pos2);

            tempsrcFile = srcFile1 + srcFile2;
            qDebug() << srcFile;
            pos1 = tempsrcFile.lastIndexOf("_");
            pos2 = tempsrcFile.lastIndexOf(".");
            pos3 = tempsrcFile.lastIndexOf("/");
            tmpFileName = tempsrcFile;
        }
        else
        {
           tmpFileName = srcFile;
        }

        tmpFileName.replace(pos1+1,pos2-pos1-1,m_LanguageList.at(i));
        tmpFileName = tmpFileName.right(tmpFileName.length() - pos3);
        tmpFileName = outputPath + tmpFileName;

        if(m_LanguageMap.contains(m_LanguageList.at(i)))
        {
            replaceMap = m_LanguageMap[m_LanguageList.at(i)];

            m_xmlHandle->changeXml(srcFile, tmpFileName, replaceMap);
            fileCount++;
            if(!tempstring.isEmpty())
            {
                QString tempstr = "***** Strings not translated into " +m_LanguageList.at(i);
                tempstr = tempstr + ": ";
                m_xmlHandle->printLog(tempstr);
                 ui->textBrowser->append(tempstr);
                //显示未翻译的文本
                QVector<QString>::iterator iter;
                for (iter=tempstring.begin();iter!=tempstring.end();iter++)
                {
                    m_xmlHandle->printLog(*iter);
                    ui->textBrowser->append(*iter);
                }
            }
            else
                ui->textBrowser->append("已经全部转换为"+ m_LanguageList.at(i));
        }
        tempstring.clear();
    }

    m_translatenumber = getChoiceNum();


    if(fileCount == m_translatenumber)
    {
        QMessageBox::information(this, tr("Info"),tr("Create file succeed"));
    }
    else
    {
        QMessageBox::warning(this, tr("warn"),tr("Create file failed,Have not found translation"));
        return;
    }
}

//输出目录
void Widget::on_pushButton_Browse_clicked()
{
    QString xmlFile;
    QSettings settings(QApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    QString outputPath = settings.value("OutputPath").toString();

    xmlFile = QFileDialog::getExistingDirectory(this,QString(),outputPath);

    if(!xmlFile.isEmpty())
        settings.setValue("OutputPath", xmlFile);

    ui->lineEdit_output->setText(xmlFile);
}

//导入ts文件
void Widget::on_tsPushButton_clicked()
{
    QString xmlFile;
    QSettings settings(QApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    QString lastFilePath = settings.value("LastTsFilePath").toString();
    if(lastFilePath.isEmpty())
        xmlFile = QFileDialog::getOpenFileName(this,QString(),QString(),"xml(*.ts *.xml)");
    else
        xmlFile = QFileDialog::getOpenFileName(this,QString(),lastFilePath,"xml(*.ts *.xml)");
    if(xmlFile.isEmpty())
        return;

    settings.setValue("LastTsFilePath", xmlFile);
    ui->tsLineEdit->setText(xmlFile);
    m_xmlHandle->readXml(xmlFile, m_readXmlTextList);
    m_srcTsFilePath = xmlFile;

}

void Widget::setCheckBoxNum(int num)
{
    ui->checkBox->setVisible(num > 0);
    ui->checkBox_2->setVisible(num > 1);
    ui->checkBox_3->setVisible(num > 2);
    ui->checkBox_4->setVisible(num > 3);
    ui->checkBox_5->setVisible(num > 4);
    ui->checkBox_6->setVisible(num > 5);
    ui->checkBox_7->setVisible(num > 6);
    ui->checkBox_8->setVisible(num > 7);
    ui->checkBox_9->setVisible(num > 8);
    ui->checkBox_10->setVisible(num > 9);

}

void Widget::setCheckBoxName(int num, QString str)
{
    if(num == 1)
        ui->checkBox->setText(str);
    else if(num == 2)
        ui->checkBox_2->setText(str);
    else if(num == 3)
        ui->checkBox_3->setText(str);
    else if(num == 4)
        ui->checkBox_4->setText(str);
    else if(num == 5)
        ui->checkBox_5->setText(str);
    else if(num == 6)
        ui->checkBox_6->setText(str);
    else if(num == 7)
        ui->checkBox_7->setText(str);
    else if(num == 8)
        ui->checkBox_8->setText(str);
    else if(num == 9)
        ui->checkBox_9->setText(str);
    else if(num == 10)
        ui->checkBox_10->setText(str);   
}


void Widget::setCheckBoxStatu()
{   
    if(ui->checkBox->isChecked() == false)
    {
        m_language[0] = "";
    }
    else
    {
        m_language[0] = m_LanguageList.at(0);
    }
    if(ui->checkBox_2->isChecked() == false)
    {
        m_language[1] = "";
    }
    else
    {
        m_language[1] = m_LanguageList.at(1);
    }
    if(ui->checkBox_3->isChecked() == false)
    {
        m_language[2] = "";
    }
    else
    {
        m_language[2] = m_LanguageList.at(2);
    }
    if(ui->checkBox_4->isChecked() == false)
    {
        m_language[3] = "";
    }
    else
    {
        m_language[3] = m_LanguageList.at(3);
    }
    if(ui->checkBox_5->isChecked() == false)
    {
        m_language[4] = "";
    }
    else
    {
        m_language[4] = m_LanguageList.at(4);
    }
    if(ui->checkBox_6->isChecked() == false)
    {
        m_language[5] = "";
    }
    else
    {
        m_language[5] = m_LanguageList.at(5);
    }
    if(ui->checkBox_7->isChecked() == false)
    {
        m_language[6] = "";
    }
    else
    {
        m_language[6] = m_LanguageList.at(6);
    }
    if(ui->checkBox_8->isChecked() == false)
    {
        m_language[7] = "";
    }
    else
    {
        m_language[7] = m_LanguageList.at(7);
    }
    if(ui->checkBox_9->isChecked() == false)
    {
        m_language[8] = "";
    }
    else
    {
        m_language[8] = m_LanguageList.at(8);
    }
    if(ui->checkBox_10->isChecked() == false)
    {
        m_language[9] = "";
    }
    else
    {
        m_language[9] = m_LanguageList.at(9);
    }
}


int Widget::judgewhichlanguage()
{
    int flag = 0;
    for(int i = 0; i < 10; i++)
    {
        if (m_language[i] == "")
        {
            continue;
        }
        return 1;
    }
    if(0 == flag)
    {
        QMessageBox::warning(this, tr("warn"),tr("没有选择翻译的语言！"));
        return 0;
    }
    return 1;
}

int Widget::getChoiceNum()
{
    int num = 0;
    for(int i = 0; i < 10; i++)
    {
        if(m_language[i] != "")
        {
            qDebug() << "选择的翻译语言编号" << i;
            num++;
        }
    }
    return num;
}

void Widget::clearChoiceArray()
{
//    for(int i = 0; i < 10; i++)
//    {
//        m_language[i] = "";
//    }
    //设置checkbox未选中
    ui->checkBox->setCheckState(Qt::Unchecked);
    ui->checkBox_2->setCheckState(Qt::Unchecked);
    ui->checkBox_3->setCheckState(Qt::Unchecked);
    ui->checkBox_4->setCheckState(Qt::Unchecked);
    ui->checkBox_5->setCheckState(Qt::Unchecked);
    ui->checkBox_6->setCheckState(Qt::Unchecked);
    ui->checkBox_7->setCheckState(Qt::Unchecked);
    ui->checkBox_8->setCheckState(Qt::Unchecked);
    ui->checkBox_9->setCheckState(Qt::Unchecked);
    ui->checkBox_10->setCheckState(Qt::Unchecked);
}


