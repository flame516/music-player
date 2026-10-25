#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

class QLineEdit;

class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr);

    QString username() const;
    QString password() const;

private:
    QLineEdit *m_userEdit = nullptr;
    QLineEdit *m_passEdit = nullptr;
};

#endif // LOGINDIALOG_H


