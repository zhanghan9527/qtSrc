#include "xmlhandle.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

XmlHandle::XmlHandle(QObject *parent) : QObject(parent)
{

}

void XmlHandle::changeXml(QString srcPath,QString destPath,QMap<QString,QString> &textQmap)
{
    QString name;
    QString replaceString;
    QDomDocument doc;
    QFile srcFile(srcPath);

    if (!srcFile.open(QIODevice::ReadOnly))
    {
        return;
    }

    if (!doc.setContent(&srcFile))
    {
        srcFile.close();
        return;
    }

    srcFile.close();
    replaceString.clear();
    QDomElement rootElem = doc.documentElement();
    listDom(rootElem, doc, textQmap);
    QString xml = doc.toString();
    QFile destFile(destPath);

    if(!destFile.exists())
    {
        qDebug()<<"destFile file not exist:";
    }
    if(!destFile.open(QFile::WriteOnly|QFile::Truncate))
    {
        qDebug()<<"open new file failed:";
        return;
    }
    QTextStream out(&destFile);
    out.setCodec("UTF-8");  //保存文件为UTF-8格式（很重要）
    out<<xml;
    destFile.close();
}
//读取xml文件
void XmlHandle::readXml(QString path,const QStringList &readTextList)
{    
    QDomDocument doc;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"open xml file failed";
        return;
    }
    if (!doc.setContent(&file))
    {
        qDebug()<<"doc.setContent failed";
        file.close();
        return;
    }
    QDomElement rootElem = doc.documentElement();//读取根节点   
    listDom(rootElem, readTextList);
    file.close();
}

void XmlHandle::listDom(QDomElement& docElem,const QStringList &readTextList)
{
    QDomNode node = docElem.firstChild();//读取第一个子节点
    while(!node.isNull())
    {
        QDomElement element = node.toElement(); // try to convert the node to an element.
        QString name = element.tagName();//从节点获取标签名
        if( !element.isNull() )
        {
            for(int i = 0; i < readTextList.count(); i++)
            {
                if(name == readTextList[i])
                {
                    for(QDomNode childNode = element.firstChild(); !childNode.isNull();childNode = childNode.nextSibling())
                    {
                        QDomText childText = childNode.toText(); //.读取节点文本
                        //qDebug()<<"name:"<<name<<" "<<childText.data();
                    }
                    break;
                }
            }
            listDom(element,readTextList);
        }
        node = node.nextSibling();//读取下一个子节点
    }
    return;
}

void XmlHandle::listDom(QDomElement& rootElem,QDomDocument& doc,QMap<QString,QString> &textQmap)
{
    QString name;
    QString replaceString;
    QDomNode rootNode = rootElem.firstChild();
    while(!rootNode.isNull())
    {        
        QDomElement fileElem = rootNode.toElement();
        if(!fileElem.isNull())
        {
            name = fileElem.tagName();
            if(name == "source")//
            {
                for(QDomNode childNode = fileElem.firstChild(); !childNode.isNull();
                    childNode = childNode.nextSibling())
                {
                    QString key = childNode.toText().data();
                    //转义字符
                    if(key.contains("&apos;"))
                    {
                       key.replace("&apos;", "'");
                    }
                    if(textQmap.contains(key))
                    {
                        replaceString.clear();
                        replaceString = textQmap[key];
                        if(replaceString.isEmpty())//翻译表格为空
                         {
                            tempstring.append(key);
                        }
                    }
                    else//没有对应的英文
                    {                       
                       // qDebug()<<"没有翻译的： "<<key;
                        tempstring.append(key);
                    }
                }
            }           
            if(name == "translation" && !replaceString.isEmpty())
            {
                QDomElement newnode = doc.createElement("translation");
                QDomText text = doc.createTextNode(replaceString);
                newnode.appendChild(text);

                rootElem.replaceChild(newnode,rootNode);
                replaceString.clear();
            }            
            listDom(fileElem, doc, textQmap);
        }
        rootNode = rootNode.nextSibling();
    }
    return;
}

void XmlHandle::printLog(QString message)
{
    QFile file;
    QString str;
    QDateTime dateTime;

    QString datestring = dateTime.currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString txtname = dateTime.currentDateTime().toString("yyyyMMdd");

   // QString filepath = QString("%1Log").arg(txtname);
    QString filepath = QString("Log");

    file.setFileName(filepath);
    file.open(QIODevice::WriteOnly | QIODevice::Append);//没有就新创建
//    if(!file.isOpen())
//    {
//        qDebug() << "打开文件日志失败";
//        return;
//    }
    QTextStream stream(&file);

    str += datestring;
    str += ": ";
    str += message;
    str += "\r\n";
    stream << str ;
    file.close();
}

