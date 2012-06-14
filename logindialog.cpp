#include <QString>
#include <QMessageBox>
#include <QLineEdit>
#include <QTcpSocket>
#include <QDebug>
#include "logindialog.h"
#include "ui_logindialog.h"


LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{

    ui->setupUi(this);
    ui->pwdEdit->setEchoMode(QLineEdit::Password);

    serverIp = QString("127.0.0.1");
    serverPort = QString("9998");
    connect(ui->loginButton,SIGNAL(clicked()),this,SLOT(login()));
    connect(this,SIGNAL(verified()),this,SLOT(processVerified()));
    connect(this,SIGNAL(unverified()),this,SLOT(processUnverified()));

}

QString LoginDialog::userName()
{
    return m_userName;
}

QString LoginDialog::ip()
{
    return serverIp;
}

QString LoginDialog::port()
{
    return serverPort;
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::login()
{

    m_userName = ui->userNameEdit->text();
    m_password = ui->pwdEdit->text();
    if(!ui->ipEdit->text().isEmpty())
        serverIp = ui->ipEdit->text();
    if(!ui->portEdit->text().isEmpty())
        serverPort = ui->portEdit->text();
    connectToServer();

}

//Connect to specified server ,wait for reply
void LoginDialog::connectToServer()
{
    client = new QTcpSocket(this);
    client->abort();
    client->connectToHost(serverIp,serverPort.toUInt());
    blockSize = 0;
    connect(client,SIGNAL(connected()),this,SLOT(verify()));
    connect(client,SIGNAL(readyRead()),this,SLOT(readMessage()));

}

//Analysis the info read from the server and make decision
void LoginDialog::readMessage()
{
    qDebug("message arrive...");
    QString result;
    QDataStream in(client);
    in.setVersion(QDataStream::Qt_4_7);
    if(blockSize == 0) {
        if(client->bytesAvailable() < (int)sizeof(quint16))
            return;
        in >> blockSize;

    }
    if(client->bytesAvailable() < blockSize)
        return;
    in >> result;
    if(result == "True")
        emit verified();
    else if(result == "False")
        emit unverified();

}

void LoginDialog::processVerified()
{
    accept();
}

//Tell the user to retry
void LoginDialog::processUnverified()
{
    QMessageBox::warning(this,"Warning","Wrong user name or password",QMessageBox::Yes);
    ui->userNameEdit->clear();
    ui->pwdEdit->clear();
    ui->userNameEdit->setFocus();
    ui->loginButton->setEnabled(true);
}

 //Write to the server to verify the user infomation
void LoginDialog::verify()
{
    qDebug("Connected to server...");
    ui->loginButton->setDisabled(true);

    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_7);
    out << quint16(0) << QString("Verify") << m_userName.trimmed() << m_password.trimmed();
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    qDebug() << block.size();
    client->write(block);

}

