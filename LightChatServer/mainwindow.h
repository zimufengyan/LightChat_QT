#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QString>
#include "epollworker.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QThread* thread;
    EpollWorker* worker;
signals:
    void openThreadSignal();
    void sendSock(int &sock);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void StartServer();
    void openThread();
    void closeServer();
    void closeThread();
    void showMsg(QString msg);

private:
    Ui::MainWindow *ui;
    void BindListen();
    int server;
    struct sockaddr_in sin;
};

#endif // MAINWINDOW_H
