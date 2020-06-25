#ifndef SERVERMSGTHREAD_H
#define SERVERMSGTHREAD_H

#include <QThread>
#include <QWidget>
#include <winsock2.h>
#include <QTime>

class ServerSendMsgThread : public QThread
{
    Q_OBJECT
private:
    QString msg;
    SOCKET clientSocket; // 套接字对象
    bool isSend;
protected:
    void run(); // 覆盖进程核心函数
public:
    ServerSendMsgThread(SOCKET socket, QWidget *parent = nullptr);
signals: // qt中信号函数
    void isMsg(QString msg); // 消息信号
    void isClose(); // 客户端关闭信号
public slots:
    void dealSend(QString msg);
};

class ServerReceiveMsgThread : public QThread
{
    Q_OBJECT
private:
    SOCKET clientSocket; // 客户端套接字对象
    sockaddr_in clientAddr; // 客户端地址
protected:
    void run();
public:
    ServerReceiveMsgThread(SOCKET sokcet, sockaddr_in addr, QWidget* parent = nullptr);
    ~ServerReceiveMsgThread();
signals:
    void isMsg(QString msg);
};

#endif // SERVERMSGTHREAD_H
