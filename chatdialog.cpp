#include <QUdpSocket>
#include <QHostAddress>
#include <QDataStream>
#include <QDebug>
#include <QStringList>
#include <QDateTime>
#include <QDate>
#include <QTime>
#include <QColor>

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
    connect(ui->sendButton,SIGNAL(clicked()),this,SLOT(broadcastMessage()));
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



//deliver the login info to the server
void ChatDialog::login()
{
    setWindowTitle(QString("Google Chatroom-%1").arg(m_userName));
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_7);

    out << QString("Login") << m_userName;
    udpSocket->writeDatagram(block,QHostAddress(serverIp),quint16(serverPort.toUInt())+1);

}

void ChatDialog::broadcastMessage()
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_7);

    out << QString("BroadcastMessage") << m_userName << getCurrentDate() << ui->inputEdit->toPlainText();
    udpSocket->writeDatagram(block,QHostAddress(serverIp),quint16(serverPort.toUInt())+1);

    ui->inputEdit->clear();


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

void ChatDialog::processDatagram(QByteArray block)
{
    QDataStream in(block);
    QString messageType;
    in.setVersion(QDataStream::Qt_4_7);
    in >> messageType;
    if(messageType == "ShowOnlineUsers") {
        processShowOnlineUsers(in);
    } else if(messageType == "NewUserLogin") {
        processNewUserLogin(in);
    } else if(messageType == "NewBroadcaseMessage") {
        processNewBroadcastMessage(in);
    } else if(messageType == "UserOffline") {
        processUserOffline(in);
    }

}

void ChatDialog::processShowOnlineUsers(QDataStream &in)
{
    qDebug("Process show online users");
    QStringList onlineUserNames;
    in >> onlineUserNames;

    qDebug() <<"size of list:" << onlineUserNames.size();
    onlineUserNames.sort();
    ui->userList->addItems(onlineUserNames);

}

void ChatDialog::processNewUserLogin(QDataStream &in)
{
    QString newUserLoginName;
    in >> newUserLoginName;
    int i;
    for(i = 0;i < ui->userList->count();i++) {
        if(newUserLoginName < ui->userList->item(i)->text())
            break;
    }
    ui->userList->insertItem(i,newUserLoginName);
    QColor color = ui->messageEdit->textColor();
    ui->messageEdit->setTextColor(QColor("Red"));

    ui->messageEdit->append(newUserLoginName + " has joined our chatroom ...\n");


    ui->messageEdit->setTextColor(color);
}

void ChatDialog::processNewBroadcastMessage(QDataStream &in)
{
    QString time;
    QString user;
    QString message;
    in >> user>> time >> message;
    QColor color = ui->messageEdit->textColor();
    if( user == m_userName)
        ui->messageEdit->setTextColor(QColor("Green"));
    else
        ui->messageEdit->setTextColor(QColor("Blue"));
    QStringList messages = message.split("\n");
    QString formatMessage = messages.join("\n  ");

    ui->messageEdit->append(user + " " + time + "\n  " + formatMessage );

    ui->messageEdit->setTextColor(color);

}

void ChatDialog::processUserOffline(QDataStream &in)
{
    QString user;
    in >> user;

    for(int i = 0;i < ui->userList->count();i++) {
        if(user == ui->userList->item(i)->text()){
            ui->userList->takeItem(i);
            break;
        }
    }
    QColor color = ui->messageEdit->textColor();
    ui->messageEdit->setTextColor(QColor("Red"));

    ui->messageEdit->append(user + " has left our chatroom ...\n");


    ui->messageEdit->setTextColor(color);
}

QString ChatDialog::getCurrentDate()
{
    QDateTime dateTime = QDateTime::currentDateTime();
    QDate date = dateTime.date();
    QTime time = dateTime.time();

    QString dateStr = QString("%1-%2-%3 %4:%5:%6").arg(date.year()).arg(date.month())
            .arg(date.day()).arg(time.hour()).arg(time.minute()).arg(time.second());

    return dateStr;
}


