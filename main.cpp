#include <QtGui/QApplication>
#include <QDialog>
#include "logindialog.h"
#include "chatdialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoginDialog login;


    if(login.exec()==QDialog::Accepted){

        ChatDialog chatDialog(login.userName(),login.ip(),login.port());

        chatDialog.show();
        return a.exec();
    }

}
