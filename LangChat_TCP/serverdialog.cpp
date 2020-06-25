#include "serverdialog.h"
#include "ui_serverdialog.h"

#include <QDebug>

ServerDialog::ServerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerDialog)
{
    ui->setupUi(this);
    // winsock信息初始化
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    serverSocket = INVALID_SOCKET;
    socketThread = nullptr;
}

ServerDialog::~ServerDialog()
{
    ::closesocket(serverSocket);
    // 清空winsock信息
    WSACleanup();
    delete ui;
}

// 端口校验
bool ServerDialog::checkPort(QString str)
{
    QRegExp reg("[0-9]{1,5}"); // 5位数字
    QRegExpValidator validator(reg, this);
    int pos = 0; // 最后一个参数是int&
    if( QValidator::Acceptable != validator.validate(str, pos))
        return false;
    int port = str.toInt();
    if( port < 0 || port > 65535)
        return false;
    return true;
}


bool ServerDialog::initNetwork()
{
    // 创建套接字, 指明tcp协议
    this->serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
        return false;
    // 地址结构体
    int port = ui->lineEditPort->text().toInt();
    sockaddr_in addr;
    addr.sin_family = PF_INET;
    addr.sin_port = htons((u_short)port);
    addr.sin_addr.S_un.S_addr = INADDR_ANY;
    // 绑定套接字和地址信息
    int ret = bind(serverSocket, (sockaddr*)&addr, sizeof(addr));
    if (ret == SOCKET_ERROR)
        return false;
    // 端口监听
    ret = ::listen(serverSocket, SOMAXCONN);
    if (ret == SOCKET_ERROR)
        return false;
    return true;
}


// 启动服务器
void ServerDialog::on_pushButtonLaunch_clicked()
{
    if(serverSocket != INVALID_SOCKET) // 已经启动
            return;
    // 端口校验
    if (!checkPort(ui->lineEditPort->text()))
    {
        ui->labelHint->setText("端口错误！");
        ui->labelHint->setStyleSheet("color:red;");
        return;
    }
    // 初始化通信网络
    if (!initNetwork())
    {
        ui->labelHint->setText("启动失败！");
        ui->labelHint->setStyleSheet("color:red;");
        return;
    };
    // ui界面更新
    ui->labelHint->setText("启动成功");
    ui->labelHint->setStyleSheet("color:green;");
    // 开启后台通信进程。多线程防止ui线程被阻塞
    socketThread = new ServerSocketThread(serverSocket, this); // socket, qdialog, qwidget
    socketThread->start();
    // 信号槽连接，用于聊天记录更新
    connect(socketThread, &ServerSocketThread::isMsg, [=](QString msg){
       ui->listWidgetMsg->addItem(msg);
    });
}

void ServerDialog::on_pushButtonSend_clicked()
{
    if (serverSocket == INVALID_SOCKET || socketThread->isConnect == false)
    {
        ui->labelHint->setText(QString("无连接"));
        ui->labelHint->setStyleSheet("color:red;");
        return;
    }
    QString msg = ui->lineEditMsg->text();
    // 发射信号
    emit isSend(msg);
}

void ServerDialog::on_pushButtonLaunchClear_clicked()
{
    ui->listWidgetMsg->clear();
}

void ServerDialog::dealMsg(QString msg)
{
    ui->listWidgetMsg->addItem(msg);
}

void ServerDialog::dealClose()
{
    qDebug() << "close1";
    QTime time = QTime::currentTime();
    QString str = time.toString("hh:mm:ss");
    ui->listWidgetMsg->addItem(QString("%1 []客户端连接断开").arg(str));
}

void ServerDialog::closeEvent(QCloseEvent *event)
{
    // 释放socket线程
    if (socketThread != nullptr)
    {
        socketThread->requestInterruption();
        socketThread->terminate();
        socketThread->quit();
        socketThread->wait();
        delete socketThread;
    }
}

