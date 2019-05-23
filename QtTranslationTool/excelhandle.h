#ifndef EXCELHANDLE_H
#define EXCELHANDLE_H

#include <QObject>
#include <QVariant>
#include <QVariantList>
#include <QAxObject>
#include <QByteArray>

class ExcelHandle : public QObject
{
    Q_OBJECT
public:
    explicit ExcelHandle(QObject *parent = 0);
    QVariant excelLocalFileRead(QString filepath);
    void qvariantToList(QVariant &var, QList<QList<QVariant> > &res);
private:
    QAxObject *m_excel;
signals:

public slots:
};

#endif // EXCELHANDLE_H
