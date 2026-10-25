#include "registerdialog.h"

#include "databasemanager.h"

#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("注册新账号"));
    setModal(true);
    setFixedSize(380, 240);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(16);

    auto *title = new QLabel(QStringLiteral("创建你的音乐账号"), this);
    title->setStyleSheet(QStringLiteral(
        "color: #333333; font-size: 18px; font-weight: bold;"
    ));

    m_userEdit = new QLineEdit(this);
    m_userEdit->setPlaceholderText(QStringLiteral("用户名"));
    m_passEdit = new QLineEdit(this);
    m_passEdit->setPlaceholderText(QStringLiteral("密码"));
    m_passEdit->setEchoMode(QLineEdit::Password);
    m_passConfirmEdit = new QLineEdit(this);
    m_passConfirmEdit->setPlaceholderText(QStringLiteral("确认密码"));
    m_passConfirmEdit->setEchoMode(QLineEdit::Password);

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
    m_passConfirmEdit->setStyleSheet(editStyle);

    auto *btnLayout = new QHBoxLayout;
    btnLayout->setSpacing(12);

    auto *okButton = new QPushButton(QStringLiteral("注册"), this);
    auto *cancelButton = new QPushButton(QStringLiteral("取消"), this);
    const QString okStyle = QStringLiteral(
        "QPushButton {"
        "  min-width: 80px;"
        "  color: #ffffff;"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "      stop:0 #ff9a9e, stop:1 #fad0c4);"
        "  border-radius: 16px;"
        "  border: none;"
        "  padding: 6px 12px;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "      stop:0 #ff8a94, stop:1 #f8b8be);"
        "}"
    );
    okButton->setStyleSheet(okStyle);
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
    btnLayout->addWidget(okButton);

    mainLayout->addWidget(title);
    mainLayout->addWidget(m_userEdit);
    mainLayout->addWidget(m_passEdit);
    mainLayout->addWidget(m_passConfirmEdit);
    mainLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    connect(okButton, &QPushButton::clicked, this, [this] {
        const QString u = username();
        const QString p = password();
        const QString pc = passwordConfirm();
        if (u.isEmpty() || p.isEmpty() || pc.isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请输入用户名和两次密码"));
            return;
        }
        if (p != pc) {
            QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("两次输入的密码不一致"));
            return;
        }
        if (!DatabaseManager::instance().ensureUser(u, p)) {
            QMessageBox::warning(this, QStringLiteral("注册失败"), QStringLiteral("注册用户时出现错误"));
            return;
        }
        accept();
    });
    connect(cancelButton, &QPushButton::clicked, this, &RegisterDialog::reject);
}

QString RegisterDialog::username() const
{
    return m_userEdit->text().trimmed();
}

QString RegisterDialog::password() const
{
    return m_passEdit->text();
}

QString RegisterDialog::passwordConfirm() const
{
    return m_passConfirmEdit->text();
}


