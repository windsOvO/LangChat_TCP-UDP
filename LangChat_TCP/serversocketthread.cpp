#include "servermsgthread.h"
#include "serverdialog.h"
#include "serversocketthread.h"

#include "QDebug"

ServerSocketThread::ServerSocketThread(SOCKET socket, ServerDialog* dialog, QWidget *parent):
    QThread(parent)
{
    this->serverSocket = socket;
    this->dialog = dialog;
    this->parent = parent;
    this->isConnect = false;
}

ServerSocketThread::~ServerSocketThread()
{
    // 关闭socket
    closesocket(serverSocket);
    emit isClose();
}

void ServerSocketThread::run()
{
    sockaddr_in clientAddr;
    int size = sizeof(clientAddr);
    while (!isInterruptionRequested()) // 是否请求终止线程，没有则一直运行
    {
        // 每次接受客户端清空地址信息
        memset(&clientAddr, 0, size);
        // 等待新客户端连接——阻塞函数
        // ::func()说明调用全局函数
        SOCKET clientSocket = ::accept(serverSocket , (sockaddr*)&clientAddr, &size);
        char* clientIp = inet_ntoa(clientAddr.sin_addr);
        int clientPort = ntohs(clientAddr.sin_port);
        // 发送连接成功消息
        QTime time = QTime::currentTime();
        QString str = time.toString("hh:mm:ss");
        QString msg = QString("%1 [%2:%3] connect success").arg(str).arg(clientIp).arg(clientPort);
        // 发送消息
        emit isMsg(msg);
        isConnect = true;

        /* 创建发送通信线程*/
        sendMsgThread = new ServerSendMsgThread(clientSocket);
        sendMsgThread->start();
        // 信号槽连接
        // 点击发送消息按钮，消息进程处理发送
        // 多线程使用Qt信号槽，第五个参数使用DirectConnection代替默认的解决跨线程问题!!
        connect(dialog, &ServerDialog::isSend, sendMsgThread, &ServerSendMsgThread::dealSend, Qt::DirectConnection);
        // 消息线程发送了信息，ui界面处理信息（显示信息）
        connect(sendMsgThread, &ServerSendMsgThread::isMsg, dialog, &ServerDialog::dealMsg, Qt::DirectConnection);
        // 消息线程断开，ui界面处理端口信息
        connect(sendMsgThread, &ServerSendMsgThread::isClose, dialog, &ServerDialog::dealClose, Qt::DirectConnection);
        /* 创建接受通信线程*/
        receiveMsgThread = new ServerReceiveMsgThread(clientSocket, clientAddr, parent);
        receiveMsgThread->start();
        // 通知UI界面更新
        // [](){} 为lambda表达式，[=]表示值传递捕捉所用作用域变量
        connect(receiveMsgThread, &ServerReceiveMsgThread::isMsg, this, [=](QString msg){
            emit isMsg(msg);
        });
        connect(this, &ServerSocketThread::isClose, this, [=]{
            receiveMsgThread->requestInterruption();
            receiveMsgThread->terminate(); // 终止线程
            receiveMsgThread->quit(); // 退出线程的循环事件
            delete receiveMsgThread;
            sendMsgThread->requestInterruption();
            sendMsgThread->terminate();
            sendMsgThread->quit();
            delete sendMsgThread;
            isConnect = false;
        });
    }
}
