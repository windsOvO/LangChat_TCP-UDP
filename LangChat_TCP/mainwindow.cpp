#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_serverBtn_clicked()
{
    serverDialog = new ServerDialog;
    this->hide();
    serverDialog->show();
    serverDialog->exec();
    this->show();
    delete serverDialog;
}

void MainWindow::on_clientBtn_clicked()
{
    clientDialog = new ClientDialog;
    this->hide();
    clientDialog->show();
    clientDialog->exec();
    this->show();
    delete clientDialog;
}
