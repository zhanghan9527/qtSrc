#ifndef BITSWITCHTOOL_H
#define BITSWITCHTOOL_H

#include <QDialog>

namespace Ui {
class BitSwitchTool;
}

class BitSwitchTool : public QDialog
{
    Q_OBJECT

public:
    explicit BitSwitchTool(QWidget *parent = 0);
    ~BitSwitchTool();

private:
    bool convert(const QString &fileSrc, const QString &fileDes);
    int getChoiceNum();
    void setCheckBoxStatu();
    void clearChoiceArray();
    QString getcomboBox();
private slots:
    void on_btnSrc_clicked();
    void on_btnSave_clicked();
    void on_btnConvert_clicked();

    void on_BitSwitchTool_accepted();

    //void on_comboBox_currentTextChanged(const QString &arg1);

private:
    Ui::BitSwitchTool *ui;
    int  m_checkbox[20];
    bool  m_checkboxStatu[20];
    int m_bit1change;
    int m_bit2change;
};

#endif // BITSWITCHTOOL_H
