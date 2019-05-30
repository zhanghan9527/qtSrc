#ifndef PNTpackage_H
#define PNTpackage_H

#include <QDialog>

namespace Ui {
class PNTpackage;
}

class PNTpackage : public QDialog
{
    Q_OBJECT

public:
    explicit PNTpackage(QWidget *parent = 0);
    ~PNTpackage();

private:
    bool convert(const QString &fileSrc, const QString &fileDes);

private slots:
    void on_btnSrc_clicked();
    void on_btnSave_clicked();
    void on_btnConvert_clicked();



private:
    Ui::PNTpackage *ui;

    qint8 m_diffbuf[127];


};

#endif // PNTpackage_H
