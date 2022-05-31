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
    connect(ui->startBtn, &QPushButton::clicked, this, &MainWindow::StartServer);
    connect(ui->closeBtn, &QPushButton::clicked, this, &MainWindow::closeServer);
    connect(this, &MainWindow::openThreadSignal, this, &MainWindow::openThread);
}

void MainWindow::StartServer()
{
    int port = ui->portEdit->text().toInt();
    QByteArray temp = ui->ipEdit->text().toLatin1();
    char* ip = temp.data();
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = inet_addr(ip);

    // create socket
    if( (server = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        this->showMsg("[ERROR] Failed to create server!");
        return;
    }
    this->showMsg("[INFO] Succeed to create server!");
    this->BindListen();
}

void MainWindow::BindListen()
{
    //bind
    if(bind(server, (const struct sockaddr *)&sin,sizeof(sin)) < 0)
    {
        this->showMsg("[ERROR] Failed to bind!");
        ui->ipEdit->clear();
        ui->portEdit->clear();
        return;
    }
    //listen
    if(listen(server, backlog) < 0)
    {
        this->showMsg("[ERROR] Failed to listen!");
        return;
    }
    this->showMsg("[INFO] Start listenning");
    emit sendSock(server);
    emit openThreadSignal();
}


void MainWindow::showMsg(QString msg)
{
    ui->msgBox->append(msg);
}

void MainWindow::openThread()
{
    thread = new QThread();
    this->worker->moveToThread(thread);
    connect(thread, SIGNAL(finished()), this->worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(started()), this->worker, SLOT(EpollRun()));
    thread->start();
}

void MainWindow::closeServer()
{
    this->closeThread();
    ::close(server);
}

void MainWindow::closeThread()
{
    if (thread->isRunning())
    {
        thread->requestInterruption();
        thread->quit();
        delete this->worker;
        delete thread;
        // thread = NULL;
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}
