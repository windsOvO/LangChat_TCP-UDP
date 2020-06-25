#ifndef CLIENTDIALOG_H
#define CLIENTDIALOG_H

#include <QDialog>
#include <winsock2.h>
#include "clientmsgthread.h"

namespace Ui {
class ClientDialog;
}

class ClientDialog : public QDialog
{
    Q_OBJECT

private:
    Ui::ClientDialog *ui;
    WSADATA wsaData;
    SOCKET clientSocket; // 服务器套接字
    QString addrStr; // 服务器地址字符串
    SOCKADDR_IN serverAddr; // 服务器地址结构体
    ClientSendMsgThread* sendMsgThread;
    ClientReceiveMsgThread* receiveMsgThread;

private:
    bool initNetwork();

public:
    explicit ClientDialog(QWidget *parent = 0);
    ~ClientDialog();

signals:
    void isSend(QString msg);
    void isClose();

private slots:
    void on_pushButtonConnect_clicked();
    void on_pushButtonSend_clicked();
    void on_pushButtonLaunchClear_clicked();

public slots:
    void dealMsg(QString msg);
    void dealClose();
};

#endif // CLIENTDIALOG_H
