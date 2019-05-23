#ifndef XMLHANDLE_H
#define XMLHANDLE_H

#include <QObject>
#include <QDomDocument>
#include <QFile>
#include <QVector>

extern  QVector<QString> tempstring;

class XmlHandle : public QObject
{
    Q_OBJECT
public:
    explicit XmlHandle(QObject *parent = 0);
    void changeXml(QString srcPath, QMap<QString, QString> &textQmap);
    void changeXml(QString srcPath,QString destPath,QMap<QString,QString> &textQmap);
    void listDom(QDomElement& rootElem, QDomDocument &doc, QMap<QString,QString> &textQmap);
    void readXml(QString path, const QStringList &readTextList);
    void listDom(QDomElement &docElem, const QStringList &readTextList);
    void printLog(QString message);
signals:

   void signfailchange(QVector<QString> &tempstring);  
public slots:

};


#endif // XMLHANDLE_H
