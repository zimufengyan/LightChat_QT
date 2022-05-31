#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "epollworker.h"
#include <QMainWindow>
#include <QMessageBox>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QThread* thread;
    EpollWorker* worker;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void openThreadSignal();
    void sendSock(int &sock);
    void closeThreadSignal();

public slots:
    void StartClient();
    void CloseClient();
    void openThread();
    void closeThread();
    void showMsg(QString msg);
    void sendMsg();

private:
    Ui::MainWindow *ui;
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
    int server_sock;

};
#endif // MAINWINDOW_H
