#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->ipEdit->setText(DefaultIP);
    QString port = QString::number(DefaultPORT, 10);
    ui->portEdit->setText(port);
    this->worker = new EpollWorker();
    void(EpollWorker::*one)(QString msg) = &EpollWorker::sendString;
    void(MainWindow::*two)(int &sock) = &MainWindow::sendSock;
    connect(worker, one, this, &MainWindow::showMsg);
    connect(this, two, worker, &EpollWorker::recvSock);
    connect(this, &MainWindow::closeThreadSignal, worker, &EpollWorker::close);
    connect(ui->connBtn, &QPushButton::clicked, this, &MainWindow::StartClient);
    connect(ui->closeBtn, &QPushButton::clicked, this, &MainWindow::CloseClient);
    connect(this, &MainWindow::openThreadSignal, this, &MainWindow::openThread);
    connect(ui->sendBtn, &QPushButton::clicked, this, &MainWindow::sendMsg);
}

void MainWindow::StartClient()
{
    int port = ui->portEdit->text().toInt();
    QByteArray temp = ui->ipEdit->text().toLatin1();
    char* ip = temp.data();

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);
    bzero(&(server_addr.sin_zero), 8);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock < 0) {
        this->showMsg("[ERROR] Failed to create Client!");
        return;
    }
    if(::connect(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)  {
        this->showMsg("[ERROR] Failed to connect Server!");
        return;
    }
    this->showMsg("[INFO] Succeed to connect Server");
    emit sendSock(server_sock);
    emit openThreadSignal();
}

void MainWindow::sendMsg()
{
    QString text = ui->msgEdit->toPlainText();
    QByteArray temp = text.toLatin1();
    char* msg = temp.data();
    if (text.length() > 0){
        if (send(server_sock, msg, strlen(msg), 0) < 0){
            this->showMsg("[ERROR] Failed to send.");
        }
    }
}

void MainWindow::showMsg(QString msg)
{
    ui->msgBox->append(msg);
}

void MainWindow::openThread()
{
    thread = new QThread();
    this->worker->moveToThread(thread);
    connect(thread, &QThread::finished, this->worker, &QObject::deleteLater);
    connect(thread, SIGNAL(started()), this->worker, SLOT(EpollRun()));
    thread->start();
}
void MainWindow::closeThread()
{
    if (thread->isRunning())
    {
        emit this->closeThreadSignal();
        // thread->requestInterruption();
        // disconnect(this->worker, 0, this, 0);
        thread->quit();
        thread->wait();
        // delete this->worker;
        delete thread;
        thread = NULL;
    }

}

void MainWindow::CloseClient()
{
    this->closeThread();
    ::close(server_sock);
    this->showMsg("[INFO] Conn had disconnected.");
}
MainWindow::~MainWindow()
{
    delete ui;
}

