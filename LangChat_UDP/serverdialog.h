#ifndef SERVERDIALOG_H
#define SERVERDIALOG_H

#include <QDialog>
#include <winsock2.h>
#include "serversocketthread.h"

namespace Ui {
class ServerDialog;
}

class ServerDialog : public QDialog
{
    Q_OBJECT

private:
    Ui::ServerDialog *ui;
    WSADATA wsaData;
    SOCKET serverSocket;
    // 接受消息的Socket线程
    ServerSocketThread *socketThread;

public:
    explicit ServerDialog(QWidget *parent = 0);
    ~ServerDialog();
    void closeEvent(QCloseEvent *event);

private slots:

    void on_pushButton_clicked();
};

#endif // SERVERDIALOG_H
