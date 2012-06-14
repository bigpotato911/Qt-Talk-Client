#include <QUdpSocket>
#include <QHostAddress>
#include <QDataStream>
#include <QDebug>
#include <QStringList>

#include "chatdialog.h"
#include "ui_chatdialog.h"



ChatDialog::ChatDialog(const QString &userName,const QString &ip,const QString &port,QWidget *parent):
    QDialog(parent),
    ui(new Ui::ChatDialog)
{


    ui->setupUi(this);
    m_userName = userName;
    serverIp = ip;
    serverPort = port;
    udpSocket = new QUdpSocket(this);
    udpPort = 6666 + qrand()%10;

    while(!udpSocket->bind(QHostAddress(serverIp),udpPort)) {
        udpPort = 6666 + qrand()%10;

    }
    qDebug() <<"udp port:" << udpPort;
    connect(udpSocket,SIGNAL(readyRead()),SLOT(processReadyRead()));
    login();
}


void ChatDialog::setUserName(const QString &userName)
{
    m_userName = userName;
}


ChatDialog::~ChatDialog()
{
    delete ui;
}

void ChatDialog::processReadyRead()
{
    qDebug("ready read ...");
    while(udpSocket->hasPendingDatagrams()) {
        QByteArray block;
        block.resize(udpSocket->pendingDatagramSize());
        if(udpSocket->readDatagram(block.data(),block.size()) == -1)
            continue;
        processDatagram(block);

    }
}

//deliver the login info to the server
void ChatDialog::login()
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_7);

    out << QString("Login");
    udpSocket->writeDatagram(block,QHostAddress(serverIp),quint16(serverPort.toUInt())+1);

}

void ChatDialog::processShowOnlineUsers(QDataStream &in)
{
    qDebug("Process show online users");
    QList<QString> onlineUsers;
    in >> onlineUsers;
    QStringList items;
    for(int i = 0;i < onlineUsers.size();i++) {
        items << onlineUsers[i];
    }
    qDebug() <<"size of list:" << onlineUsers.size();
    ui->userList->addItems(items);

}

void ChatDialog::processDatagram(QByteArray block)
{
    QDataStream in(block);
    QString messageType;
    in.setVersion(QDataStream::Qt_4_7);
    in >> messageType;
    if(messageType == "ShowOnlineUsers") {
        processShowOnlineUsers(in);
    }

}
