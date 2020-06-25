#include "serverdialog.h"
#include "ui_serverdialog.h"

#include "QDebug"

ServerDialog::ServerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerDialog)
{
    ui->setupUi(this);
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    serverSocket = INVALID_SOCKET;
    socketThread = nullptr;
}

ServerDialog::~ServerDialog()
{
    delete ui;
}

void ServerDialog::on_pushButton_clicked()
{
    if (serverSocket != INVALID_SOCKET)
        return;
    // 创建套接字, 指明udp协议
    this->serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket == INVALID_SOCKET)
        return;
    // 地址结构体
    int port = ui->lineEditPort->text().toInt();
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons((u_short)port);
    serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
    // 绑定套接字和地址信息
    int ret = bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (ret == SOCKET_ERROR)
        return;
    socketThread = new ServerSocketThread(serverSocket, ui);
    socketThread->start();
}

void ServerDialog::closeEvent(QCloseEvent *event)
{
    if (socketThread != nullptr)
    {
        socketThread->requestInterruption(); // 请求中止
        socketThread->terminate(); // 强制退出
        // 考虑到线程结束的慢，但是socket已经随着dialog关闭而清空造成卡死
        // 使用强制退出线程
        //socketThread->quit(); // 线程退出，结束循环事件
        //socketThread->wait(); // 等待线程的退出
        delete socketThread;
    }
    closesocket(serverSocket);
    // 清空winsock信息
    WSACleanup();
}
