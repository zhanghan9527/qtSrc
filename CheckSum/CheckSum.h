#ifndef CHECKSUM_H
#define CHECKSUM_H

#include <QDialog>

namespace Ui {
class CheckSum;
}

class CheckSum : public QDialog
{
    Q_OBJECT

public:
    explicit CheckSum(QWidget *parent = 0);
    ~CheckSum();
    bool convert(const QString &fileSrc);

private slots:
    void on_btnSrc_clicked();   
    void on_btnConvert_clicked();

private:
    Ui::CheckSum *ui;

};

#endif // BITSWITCHTOOL_H

