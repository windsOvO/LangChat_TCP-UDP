#ifndef SERVERSOCKETTHREAD_H
#define SERVERSOCKETTHREAD_H

/*
    这个线程类用于服务端始终保持接受状态，等待任何客户端的连接。
    并保证主ui线程不被阻塞
*/
class ServerSocketThread;
#include <QThread>
#include <QWidget>
#include <winsock2.h>
#include <QTime>
#include "servermsgthread.h"
#include "serverdialog.h"
// #include "ui_serverdialog.h"
// ui_serverdialog.h与serverdialog.h不同

class ServerSocketThread : public QThread
{
    Q_OBJECT
private:
    QWidget *parent; // 该线程所在的组件对象
    ServerDialog *dialog; // 记录属于的对话框指针
    SOCKET serverSocket; // 套接字对象
public:
    ServerReceiveMsgThread* receiveMsgThread;
    ServerSendMsgThread* sendMsgThread;
protected:
    void run(); // 覆盖进程核心函数
public:
    ServerSocketThread(SOCKET serverSocket, ServerDialog* dialog, QWidget *parent = nullptr);
    ~ServerSocketThread();
    bool isConnect; // 存在客户端连接
signals: // qt中信号函数
    void isMsg(QString msg); // 消息信号
    void isClose(); // 服务端关闭信号
};

#endif // SERVERSOCKETTHREAD_H
