#ifndef CutPicture_H
#define CutPicture_H

#include <QDialog>
#include<QButtonGroup>
#include<QRadioButton>


namespace Ui {
class CutPicture;
}

class CutPicture : public QDialog
{
    Q_OBJECT

public:
    explicit CutPicture(QWidget *parent = 0);
    ~CutPicture();
private:
    bool convert(const QString &fileSrc, const QString &fileDes);
private slots:
    void on_btnSrc_clicked();
    void on_btnSave_clicked();
    void on_btnConvert_clicked();
private:
    Ui::CutPicture *ui;
    quint16 m_width;//src image width
    quint16 m_height;//src image height

    int m_xaxis;//x轴图片序号0~
    int m_yaxis;//y轴图片序号
    int m_xnum;//x轴图片个数
    int m_ynum;//y轴图片个数
};

#endif // CutPicture_H
