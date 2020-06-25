#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "serverdialog.h"
#include "clientdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_serverBtn_clicked();
    void on_clientBtn_clicked();

private:
    Ui::MainWindow *ui;
    // 客户端和服务端窗口
    ServerDialog *serverDialog;
    ClientDialog *clientDialog;
};
#endif // MAINWINDOW_H
