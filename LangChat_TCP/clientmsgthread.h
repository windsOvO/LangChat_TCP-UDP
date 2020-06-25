#ifndef CLIENTMSGTHREAD_H
#define CLIENTMSGTHREAD_H

#include <QWidget>
#include <QThread>
#include <winsock2.h>
#include <QTime>

class ClientSendMsgThread : public QThread
{
    Q_OBJECT
private:
    QString msg;
    SOCKET serverSocket; // 套接字对象
    bool isSend;
protected:
    void run(); // 覆盖进程核心函数
public:
    ClientSendMsgThread(SOCKET socket, QWidget *parent = nullptr);
signals: // qt中信号函数
    void isMsg(QString msg); // 消息信号
    void isClose(); // 服务端关闭信号
public slots:
    void dealSend(QString msg);
};

class ClientReceiveMsgThread : public QThread
{
    Q_OBJECT
private:
    SOCKET serverSocket; // 服务端套接字对象
    sockaddr_in serverAddr; // 服务端地址
protected:
    void run();
public:
    ClientReceiveMsgThread(SOCKET sokcet, sockaddr_in addr, QWidget* parent = nullptr);
    ~ClientReceiveMsgThread();
signals:
    void isMsg(QString msg);
};

#endif // CLIENTMSGTHREAD_H
