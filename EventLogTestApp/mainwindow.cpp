#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    showMaximized();

    ui->eventLogWidget->initializeModel("D:\\EventLog\\eventlog.sqlite", "eventlog_connection");
    ui->eventLogWidget->setFlushIntervalMs(100);
    ui->eventLogWidget->setMaxBatchSize(300);

    // for testing manual add event
    QObject::connect(this, &MainWindow::addEvent, ui->eventLogWidget, &EventLogWidget::addEvent, Qt::QueuedConnection);

    // worker event timer object
    worker = new EventSimulator;    // wrong to use --> new EventTimer(this);
    connect(worker, &EventSimulator::newEvent, ui->eventLogWidget, &EventLogWidget::addEvent, Qt::QueuedConnection);
    worker->setInterval(500);

    // our thread
    thread = new QThread(this);
    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &EventSimulator::start);
    connect(thread, &QThread::finished, worker, &EventSimulator::stop);
}

MainWindow::~MainWindow()
{
    // on exit application stop thread
    if (thread->isRunning())
    {
        thread->quit();
        thread->wait();
    }

    worker->deleteLater();

    delete thread;
    thread = nullptr;
    worker = nullptr;

    delete ui;
}

void MainWindow::on_actionShow_EventLog_Widget_triggered()
{
    ui->eventLogDockWidget->show();
}

void MainWindow::on_addEventLogBtn_clicked()
{
    emit addEvent(QDateTime::currentDateTime(), EventLogWidget::EVENT_TYPE::Debug, "Custom Source", "Manual Event Added");
}

void MainWindow::on_startThreadBtn_clicked()
{
    if (thread->isRunning())
        return;

    thread->start();
}

void MainWindow::on_stopThreadBtn_clicked()
{
    if (!thread->isRunning())
        return;

    thread->quit();
    thread->wait();
}



