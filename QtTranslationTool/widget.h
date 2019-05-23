#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QString>
#include <QFile>
#include <QList>
#include <QVariant>
#include <QStringList>
#include <QSettings>

#include "xmlhandle.h"
#include "excelhandle.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void getStringMapFromCsv(QString filePath);
    void initScreen();
    void setCheckBoxNum(int);//设置checkbox数量
    void setCheckBoxName(int num, QString str);//设置checkbox名字
    void setCheckBoxStatu();//设置checkbox状态
    int judgewhichlanguage();//判断有没有选中翻译的语言
    int getChoiceNum();//获取选中的翻译语言数目
    void clearChoiceArray();
private slots:
    void on_pushButton_clicked();

    void on_createBtAll_clicked();//生成

    void on_pushButton_Browse_clicked();

    void on_tsPushButton_clicked();

private:
    Ui::Widget *ui;
    int m_translatenumber;

public:
    QString m_FilePath;
    QString m_srcTsFilePath;
    QString m_destTsFilePath;
    QMap<QString,QMap<QString,QString>> m_LanguageMap;
    QFile m_ExcelFile;
    ExcelHandle *m_excelHandle;
    XmlHandle *m_xmlHandle;
    QList<QList<QVariant> >  m_dataVarList;
    QStringList m_readXmlTextList;
    QStringList m_LanguageList;
    QString  m_language[11];
};

#endif // WIDGET_H
