#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>

namespace Ui {
class ChatDialog;
}

class QUdpSocket;

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(const QString &userName,const QString &ip,const QString &port,QWidget *parent = 0);
    void setUserName(const QString &userName);
    ~ChatDialog();
private slots:
    void processReadyRead();
private:
    void login();
    void processShowOnlineUsers(QDataStream &in);
    void processDatagram(QByteArray block);
    Ui::ChatDialog *ui;
    QString m_userName;
    QString serverIp;
    QString serverPort;
    QUdpSocket *udpSocket;
    quint16 udpPort;
};

#endif // CHATDIALOG_H
