#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class QTcpSocket;

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    QString userName();
    QString ip();
    QString port();
    ~LoginDialog();
signals:
    void verified();
    void unverified();
private slots:
    void login();
    void connectToServer();
    void readMessage();
    void processVerified();
    void processUnverified();
    void verify();
private:


    quint16 blockSize;
    QString m_userName;
    QString m_password;
    QString serverIp;
    QString serverPort;
    Ui::LoginDialog *ui;
    QTcpSocket *client;
};

#endif // LOGINDIALOG_H
