#include "clientmsgthread.h"

#include <QDebug>

/*
*   send
*/

ClientSendMsgThread::ClientSendMsgThread(SOCKET socket, QWidget *parent):
    QThread(parent)
{
    this->serverSocket = socket;
    this->isSend = false;
}

void ClientSendMsgThread::run()
{
    char buf[1024] = {0};
    while (!isInterruptionRequested())
    {
        if (isSend)
        {
            // 发送
            ::send(serverSocket, msg.toUtf8().data(), msg.length() + 1, 0);
            int errCode = WSAGetLastError();
            // 连接超时
            if (errCode == WSAECONNABORTED)
            {
                emit isClose();
                return;
            }
            // 阻塞函数，等待服务器回应
            //int ret = ::recv(serverSocket, buf, 1024, 0);
            // 服务器关闭
            //if (ret == 0)
            //{
            //    emit isClose();
             //   return;
            //}
            QTime time = QTime::currentTime();
            QString str = time.toString("hh:mm:ss");
            QString msg2 = QString("%1 [localhost]%2").arg(str).arg(msg);
            emit isMsg(msg2);
            isSend = false;
            //clientMutex.unlock();
        }
    }
}

void ClientSendMsgThread::dealSend(QString msg)
{
    isSend = true;
    this->msg = msg;
}

/*
*   receive
*/

ClientReceiveMsgThread::ClientReceiveMsgThread(SOCKET socket, sockaddr_in addr, QWidget* parent):
    QThread(parent)
{
    this->serverSocket = socket;
    this->serverAddr = addr;
}

ClientReceiveMsgThread::~ClientReceiveMsgThread()
{
    closesocket(serverSocket);
}

void ClientReceiveMsgThread::run()
{
    char* clientIp = inet_ntoa(serverAddr.sin_addr);
    int clientPort = ntohs(serverAddr.sin_port);
    char resp[1024] = {0}; // 返回消息response
    // 循环事件
    while(!isInterruptionRequested())
    {
        memset(resp, 0, 1024);
        char buf[1024] = {0}; // 缓存
        // 接受消息——阻塞函数
        int ret = recv(serverSocket, buf, 1024, 0);
        QTime time = QTime::currentTime();
        QString str = time.toString("hh:mm:ss");
        // 连接断开
        if (ret == -1)
        {
            emit isMsg(QString("%1 [%2:%3] is closed!").arg(str).arg(clientIp).arg(clientPort));
            break;
        }
        QString msg = QString("%1 [%2:%3]:%4").arg(str).arg(clientIp).arg(clientPort).arg(buf);
        // qDebug() << "客户端收到消息：" << buf;
        // 通知GUI界面更新
        emit isMsg(msg);
        // 向用户端发送响应消息
        strcpy(resp, QString(buf).toUpper().toUtf8().data()); // 转码
        //clientMutex.lock();
        // send(serverSocket, resp, strlen(resp) + 1, 0);
        //clientMutex.unlock();
    }
}
