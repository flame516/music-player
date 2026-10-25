#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>

class QLineEdit;

class RegisterDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RegisterDialog(QWidget *parent = nullptr);

    QString username() const;
    QString password() const;
    QString passwordConfirm() const;

private:
    QLineEdit *m_userEdit = nullptr;
    QLineEdit *m_passEdit = nullptr;
    QLineEdit *m_passConfirmEdit = nullptr;
};

#endif // REGISTERDIALOG_H


