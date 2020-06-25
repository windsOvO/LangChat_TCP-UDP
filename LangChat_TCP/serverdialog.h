#ifndef SERVERDIALOG_H
#define SERVERDIALOG_H

class ServerDialog;
// 前置声明，解决头文件互相包含情况问题

#include <QDialog>
#include <winsock2.h>
#include "serversocketthread.h"
// #include "servermsgthread.h"


namespace Ui {
class ServerDialog;
}

class ServerDialog : public QDialog
{
    Q_OBJECT

private:
    Ui::ServerDialog *ui;
    // 一种数据结构，用来存储startup后，返回的windows socket数据
    WSADATA wsaData;
    SOCKET serverSocket;
    ServerSocketThread *socketThread;
private:
    bool checkPort(QString str);
    bool initNetwork();

public:
    explicit ServerDialog(QWidget *parent = 0);
    ~ServerDialog();
    void closeEvent(QCloseEvent *event); // 覆盖，关闭时触发

signals:
    void isSend(QString msg);

private slots:
    void on_pushButtonLaunch_clicked();
    void on_pushButtonSend_clicked();
    void on_pushButtonLaunchClear_clicked();

public slots:
    void dealMsg(QString msg);
    void dealClose();
};

#endif // SERVERDIALOG_H
