#ifndef CLIENTDIALOG_H
#define CLIENTDIALOG_H

#include <QDialog>
#include <winsock2.h>

namespace Ui {
class ClientDialog;
}

class ClientDialog : public QDialog
{
    Q_OBJECT
private:
    Ui::ClientDialog *ui;
    WSADATA wsaData;
    SOCKET clientSocket;

public:
    explicit ClientDialog(QWidget *parent = 0);
    ~ClientDialog();
    void closeEvent(QCloseEvent *event); // 覆盖，关闭时触发

private slots:
    void on_pushButtonSend_clicked();


};

#endif // CLIENTDIALOG_H
