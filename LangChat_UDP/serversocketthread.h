#ifndef SERVERSOCKETTHREAD_H
#define SERVERSOCKETTHREAD_H

#include <QThread>
#include <winsock2.h>
#include "ui_serverdialog.h"

class ServerSocketThread : public QThread
{
private:
    SOCKET serverSocket;
    sockaddr_in clientAddr;
    Ui::ServerDialog *dlg; // 更新窗口的指针
protected:
    void run(); // 覆盖进程核心函数
public:
    ServerSocketThread(SOCKET serverSocket, Ui::ServerDialog *dlg);
};

#endif // SERVERSOCKETTHREAD_H
