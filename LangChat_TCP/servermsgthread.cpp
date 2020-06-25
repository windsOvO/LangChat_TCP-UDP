#include "servermsgthread.h"

#include <QDebug>

/*
*   send
*/

//int ServerSendMsgThread::isSending = false;
//int ServerReceiveMsgThread::isReceiving = false;

ServerSendMsgThread::ServerSendMsgThread(SOCKET socket, QWidget *parent):
    QThread(parent)
{
    this->clientSocket = socket;
    this->isSend = false;
}

void ServerSendMsgThread::run()
{
    char buf[1024] = {0};
    while (!isInterruptionRequested())
    {
        if (isSend)
        {
            // 发送
            // serverMutex.lock();
            ::send(clientSocket, this->msg.toUtf8().data(), this->msg.length() + 1, 0);
            int errCode = WSAGetLastError();
            // 连接超时
            if (errCode == WSAECONNABORTED)
            {
                emit isClose();
                return;
            }
            // 阻塞函数，等待服务器回应
            // int ret = ::recv(clientSocket, buf, 1024, 0);
            // qDebug() << ret;
            // 客户端关闭
            // if (ret == -1) // 无法输出客户端断开连接信息bug
            // {
               // emit isClose();
                // qDebug() << "close";
              //  return;
            // }
            QTime time = QTime::currentTime();
            QString str = time.toString("hh:mm:ss");
            QString msg2 = QString("%1 [localhost]%2").arg(str).arg(msg);
            emit isMsg(msg2);
            isSend = false;
            // serverMutex.unlock();
        }
    }
}

void ServerSendMsgThread::dealSend(QString msg)
{
    this->isSend = true;
    this->msg = msg;
}

/*
*   receive
*/

ServerReceiveMsgThread::ServerReceiveMsgThread(SOCKET socket, sockaddr_in addr, QWidget* parent):
    QThread(parent)
{
    this->clientSocket = socket;
    this->clientAddr = addr;
}

ServerReceiveMsgThread::~ServerReceiveMsgThread()
{
    closesocket(clientSocket);
}

void ServerReceiveMsgThread::run()
{
    char* clientIp = inet_ntoa(clientAddr.sin_addr);
    int clientPort = ntohs(clientAddr.sin_port);
    char resp[1024] = {0}; // 返回消息response
    // 循环事件
    while(!isInterruptionRequested())
    {
        memset(resp, 0, 1024);
        char buf[1024] = {0}; // 缓存
        // 接受消息——阻塞函数
        int ret = recv(clientSocket, buf, 1024, 0);
        QTime time = QTime::currentTime();
        QString str = time.toString("hh:mm:ss");
        // 连接断开
        // qDebug() << ret;
        if (ret == -1)
        {
            emit isMsg(QString("%1 [%2:%3] is closed!").arg(str).arg(clientIp).arg(clientPort));
            break;
        }
        QString msg = QString("%1 [%2:%3]:%4").arg(str).arg(clientIp).arg(clientPort).arg(buf);
        // 通知GUI界面更新
        emit isMsg(msg);
        // 向用户端发送响应消息
        strcpy(resp, QString(buf).toUpper().toUtf8().data()); // 转码
        //serverMutex.lock();
        //send(clientSocket, resp, strlen(resp) + 1, 0);
        //serverMutex.unlock();
    }
}
