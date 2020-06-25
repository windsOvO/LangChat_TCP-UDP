#include "clientdialog.h"
#include "ui_clientdialog.h"


ClientDialog::ClientDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClientDialog)
{
    ui->setupUi(this);
    // winsock信息初始化
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    clientSocket = INVALID_SOCKET;
}

ClientDialog::~ClientDialog()
{
    closesocket(clientSocket);
    // 清空winsock信息
    WSACleanup();
    emit isClose();
    delete ui;
}

// 初始化网络
bool ClientDialog::initNetwork()
{
    // 创建套接字, 指明tcp协议
    clientSocket = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(clientSocket == INVALID_SOCKET)
        return false;
    // 获取ip&port
    addrStr = ui->lineEditAddr->text();
    char ip[1024] = {0};
    int port = 0;
    sscanf(addrStr.toUtf8().data(),"%[^:]:%d",ip,&port);
    // 地址结构体
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_port = htons((u_short)port);
    serverAddr.sin_addr.S_un.S_addr = inet_addr(ip); // 点分十进制转ip地址
    // 连接服务端
    int val = ::connect(clientSocket,(SOCKADDR*)&serverAddr,sizeof(serverAddr));
    if( val == SOCKET_ERROR)
        return false;
    return true;
}

// 连接服务器
void ClientDialog::on_pushButtonConnect_clicked()
{
    if (!initNetwork())
    {
        ui->labelHint->setText(QString("连接失败！"));
        ui->labelHint->setStyleSheet("color:red;");
        return;
    }
    ui->labelHint->setText(QString("连接成功"));
    ui->labelHint->setStyleSheet("color:green;");
    /* 创建发送通信线程*/
    sendMsgThread = new ClientSendMsgThread(clientSocket);
    sendMsgThread->start();
    // 信号槽连接
    // 点击发送消息按钮，消息进程处理发送
    connect(this, &ClientDialog::isSend, sendMsgThread, &ClientSendMsgThread::dealSend);
    // 消息线程发送了信息，ui界面处理信息（显示信息）
    connect(sendMsgThread, &ClientSendMsgThread::isMsg, this, &ClientDialog::dealMsg);
    // 消息线程断开，ui界面处理端口信息
    connect(sendMsgThread, &ClientSendMsgThread::isClose, this, &ClientDialog::dealClose);
    /* 创建接受通信线程*/
    receiveMsgThread = new ClientReceiveMsgThread(clientSocket, serverAddr);
    receiveMsgThread->start();
    // 通知UI界面更新
    // [](){} 为lambda表达式，[=]表示值传递捕捉所用作用域变量
    connect(receiveMsgThread, &ClientReceiveMsgThread::isMsg, this, [=](QString msg){
        ui->listWidgetMsg->addItem(msg);
    });

    connect(this, &ClientDialog::isClose, this, [=]{
        receiveMsgThread->requestInterruption();
        receiveMsgThread->terminate(); // 终止线程
        receiveMsgThread->quit(); // 退出线程的循环事件
        delete receiveMsgThread;
        sendMsgThread->requestInterruption();
        sendMsgThread->terminate();
        sendMsgThread->quit();
        delete sendMsgThread;
    });
}

void ClientDialog::on_pushButtonSend_clicked()
{
    if (clientSocket == INVALID_SOCKET)
    {
        ui->labelHint->setText(QString("发送失败！"));
        ui->labelHint->setStyleSheet("color:red;");
        return;
    }
    QString msg = ui->lineEditMsg->text();
    // 发射信号
    emit isSend(msg);
}

void ClientDialog::on_pushButtonLaunchClear_clicked()
{
    ui->listWidgetMsg->clear();
}

void ClientDialog::dealMsg(QString msg)
{
    ui->listWidgetMsg->addItem(msg);
}

void ClientDialog::dealClose()
{
    QTime time = QTime::currentTime();
    QString str = time.toString("hh:mm:ss");
    ui->listWidgetMsg->addItem(QString("%1 [%2]服务器连接断开").arg(str).arg(addrStr));
    ui->labelHint->setText(QString("连接断开！"));
    ui->labelHint->setStyleSheet("color:red;");
}
