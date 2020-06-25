#include "clientdialog.h"
#include "ui_clientdialog.h"

#include <QDebug>
#include <string>

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
    delete ui;
}

void ClientDialog::on_pushButtonSend_clicked()
{
    // 创建套接字, 指明udp协议
    if(clientSocket == INVALID_SOCKET)
        clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    // 处理消息
    QString msg = ui->lineEditMsg->text();
    std::string buf0 = msg.toStdString();
    char buf[1024] = {0};
    int i;
    for (i = 0; i < buf0.length(); i++)
    {
        buf[i] = buf0[i];
    }
    buf[i] = 0;
    // 获取ip&port
    QString addrStr = ui->lineEditIP->text();
    char ip[1024] = {0};
    int port = 0;
    sscanf(addrStr.toUtf8().data(),"%[^:]:%d",ip,&port);
    // 地址结构体
    SOCKADDR_IN serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons((u_short)port);
    serverAddr.sin_addr.S_un.S_addr = inet_addr(ip); // 点分十进制转ip地址
    // 发送
    qDebug() << buf << "位数" << sizeof(buf);
    sendto(clientSocket, buf, sizeof(buf), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
    // 同理发送到第二个服务器
    addrStr = ui->lineEditIP_2->text();
    sscanf(addrStr.toUtf8().data(),"%[^:]:%d",ip,&port);
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_port = htons((u_short)port);
    serverAddr.sin_addr.S_un.S_addr = inet_addr(ip); // 点分十进制转ip地址
    // 发送
    sendto(clientSocket, buf, sizeof(buf), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
    return;
}

void ClientDialog::closeEvent(QCloseEvent *event)
{
    closesocket(clientSocket);
    // 清空winsock信息
    WSACleanup();
}
