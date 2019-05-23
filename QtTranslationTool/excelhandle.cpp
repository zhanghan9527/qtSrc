#include "excelhandle.h"
#include <QDebug>

ExcelHandle::ExcelHandle(QObject *parent) : QObject(parent)
{
    m_excel = new QAxObject("Excel.Application");
}

QVariant ExcelHandle::excelLocalFileRead(QString filepath)
{
     QAxObject *workbook = NULL;
     QAxObject *workbooks = NULL;
     if(m_excel == NULL)
     {
        return NULL;
     }     
     /*调用方法用dynamicCall，获取属性或获取返回值用querySubObject()*/
     m_excel->dynamicCall("SetVisible(bool)", false); //调用函数
     workbooks = m_excel->querySubObject("WorkBooks");//获取操作对象，用于执行打开文件操作
     workbook = workbooks->querySubObject("Open(QString, QVariant)", filepath);//获得文件对象
     QAxObject * worksheet = workbook->querySubObject("WorkSheets(int)", 1);//打开第一个sheet

     QVariant var;
     if (worksheet != NULL && ! worksheet->isNull())
     {
         QAxObject *usedRange = worksheet->querySubObject("UsedRange");
         if(NULL == usedRange || usedRange->isNull())
         {
             return var;
         }
         var = usedRange->dynamicCall("Value");
         delete usedRange;
     }

     workbook->dynamicCall("Close (Boolean)", false);
     return var;
}

void ExcelHandle::qvariantToList(QVariant &var, QList<QList<QVariant> > &res)
{
    QVariantList varRows = var.toList();
    if(varRows.isEmpty())
    {
        return;
    }
    const int rowCount = varRows.size();
    QVariantList rowData;
    QByteArray byte;
    QString date;

    for(int i=0;i<rowCount;++i)
    {
        rowData = varRows[i].toList();
        res.push_back(rowData);
        for(int j = 0; j < rowData.count(); j++)
        {
            byte = res.at(i).at(j).toByteArray();

            date = QString::fromUtf8(byte);
        }
    }
}
