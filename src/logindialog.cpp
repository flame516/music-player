#include "logindialog.h"

#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("用户登录"));
    setModal(true);
    setFixedSize(360, 200);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(16);

    auto *title = new QLabel(QStringLiteral("登录音乐账号"), this);
    title->setStyleSheet(QStringLiteral(
        "color: #333333; font-size: 18px; font-weight: bold;"
    ));

    m_userEdit = new QLineEdit(this);
    m_userEdit->setPlaceholderText(QStringLiteral("用户名"));
    m_passEdit = new QLineEdit(this);
    m_passEdit->setPlaceholderText(QStringLiteral("密码"));
    m_passEdit->setEchoMode(QLineEdit::Password);

    const QString editStyle = QStringLiteral(
        "QLineEdit {"
        "  background: #ffffff;"
        "  border-radius: 16px;"
        "  padding-left: 12px;"
        "  color: #333333;"
        "  border: 1px solid #dde3f0;"
        "}"
        "QLineEdit:focus {"
        "  border: 1px solid #7c9cff;"
        "}"
    );
    m_userEdit->setStyleSheet(editStyle);
    m_passEdit->setStyleSheet(editStyle);

    auto *btnLayout = new QHBoxLayout;
    btnLayout->setSpacing(12);

    auto *loginButton = new QPushButton(QStringLiteral("登录"), this);
    auto *cancelButton = new QPushButton(QStringLiteral("取消"), this);
    const QString btnStyle = QStringLiteral(
        "QPushButton {"
        "  min-width: 80px;"
        "  color: #ffffff;"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "      stop:0 #7c9cff, stop:1 #9ad0ff);"
        "  border-radius: 16px;"
        "  border: none;"
        "  padding: 6px 12px;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "      stop:0 #6b8ff2, stop:1 #8bc4ff);"
        "}"
    );
    loginButton->setStyleSheet(btnStyle);
    cancelButton->setStyleSheet(QStringLiteral(
        "QPushButton {"
        "  min-width: 80px;"
        "  color: #607d8b;"
        "  background: #eef2f7;"
        "  border-radius: 16px;"
        "  border: none;"
        "  padding: 6px 12px;"
        "}"
        "QPushButton:hover {"
        "  background: #dde3f0;"
        "}"
    ));

    btnLayout->addStretch();
    btnLayout->addWidget(cancelButton);
    btnLayout->addWidget(loginButton);

    mainLayout->addWidget(title);
    mainLayout->addWidget(m_userEdit);
    mainLayout->addWidget(m_passEdit);
    mainLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    connect(loginButton, &QPushButton::clicked, this, [this] {
        if (m_userEdit->text().trimmed().isEmpty() || m_passEdit->text().isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请输入用户名和密码"));
            return;
        }
        accept();
    });
    connect(cancelButton, &QPushButton::clicked, this, &LoginDialog::reject);
}

QString LoginDialog::username() const
{
    return m_userEdit->text().trimmed();
}

QString LoginDialog::password() const
{
    return m_passEdit->text();
}
