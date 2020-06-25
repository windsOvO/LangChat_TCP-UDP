#include "serversocketthread.h"

#include <QDebug>

ServerSocketThread::ServerSocketThread(SOCKET serverSocket, Ui::ServerDialog *dlg):
    QThread(nullptr)
{
    this->serverSocket = serverSocket;
    this->dlg = dlg;
}

void ServerSocketThread::run()
{
    char buf[1024];
    // pos一定要严格按照大小
    int pos = sizeof(clientAddr);
    // isInterruptionRequested()代替true！
    while (!isInterruptionRequested())
    {
        memset(buf, 0 ,sizeof(buf));
        recvfrom(serverSocket, buf, 1024, 0, (sockaddr*)&clientAddr, &pos);
        dlg->listWidgetMsg->addItem(QString("[收到消息]%1").arg(buf));
    }
}
