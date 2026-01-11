#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(this, &MainWindow::addEvent, ui->eventLogWidget, &EventLogWidget::addEvent, Qt::QueuedConnection);

    ui->stopThreadBtn->setEnabled(false);
}

MainWindow::~MainWindow()
{
    stopAndDeleteThread();

    delete ui;
}

void MainWindow::createAndStartThread()
{
    // Ignore if already running
    if (thread && thread->isRunning())
        return;

    // create Event Simulator in another thread
    worker = new EventSimulator;
    connect(worker, &EventSimulator::newEvent, ui->eventLogWidget, &EventLogWidget::addEvent, Qt::QueuedConnection);

    // Create new thread
    thread = new QThread(this);

    // Move worker to new thread
    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, [this]() {
        worker->start(500);
    });

    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    thread->start();
}

void MainWindow::stopAndDeleteThread()
{
    if (thread->isRunning())
    {
        QMetaObject::invokeMethod(worker, "stop", Qt::QueuedConnection);
        thread->quit();
        thread->wait();
        // Thread will delete itself (deleteLater)
        worker->deleteLater();
        worker = nullptr;
        thread = nullptr;
    }
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
    createAndStartThread();

    ui->startThreadBtn->setEnabled(false);
    ui->stopThreadBtn->setEnabled(true);
}

void MainWindow::on_stopThreadBtn_clicked()
{
    stopAndDeleteThread();

    ui->startThreadBtn->setEnabled(true);
    ui->stopThreadBtn->setEnabled(false);
}



