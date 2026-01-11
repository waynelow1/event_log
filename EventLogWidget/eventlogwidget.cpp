#include "eventlogwidget.h"
#include "ui_eventlogwidget.h"

EventLogWidget::EventLogWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EventLogWidget)
{
    ui->setupUi(this);

    // create database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("eventlog.db");
    if (!db.open())
    {
        qDebug() << "DB open failed:" << db.lastError();
    }

    // creat table database
    QSqlQuery q;
    q.exec("CREATE TABLE IF NOT EXISTS events ("
           "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
           "Timestamp TEXT,"
           "Severity TEXT,"
           "Source TEXT,"
           "Message TEXT"
           ")");

    // create sql model and connect it to db and table
    m_sqlTableModel = new QSqlTableModel(this, db);
    m_sqlTableModel->setTable("events");
    m_sqlTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_sqlTableModel->select();   // load existing rows if any

    // proxy filter
    m_filterProxy = new EventFilterProxy(this);
    m_filterProxy->setSourceModel(m_sqlTableModel);
    m_filterProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_filterProxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    ui->eventLogTableView->setModel(m_filterProxy);

    // UI filter controls
    ui->severityFilterCombo->addItems({"All", "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"});

    auto updateFilter = [this]() {
        auto f = static_cast<EventFilterProxy*>(m_filterProxy);
        f->sevFilter = ui->severityFilterCombo->currentText();
        f->srcFilter = ui->sourceFilterEdit->text();
        f->msgFilter = ui->messageFilterEdit->text();
        f->invalidate();   //re-evaluate which rows should be shown in the table
    };

    connect(ui->severityFilterCombo, &QComboBox::currentTextChanged, this, updateFilter);
    connect(ui->sourceFilterEdit, &QLineEdit::textChanged, this, updateFilter);
    connect(ui->messageFilterEdit, &QLineEdit::textChanged, this, updateFilter);


    // prevent items editable
    ui->eventLogTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // TableView column width
    ui->eventLogTableView->setColumnHidden(0, true);
    ui->eventLogTableView->setColumnWidth(1, 120);
    ui->eventLogTableView->setColumnWidth(2, 100);
    ui->eventLogTableView->setColumnWidth(3, 100);
    ui->eventLogTableView->setColumnWidth(4, 200);
    ui->eventLogTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    // TableView Style
    ui->eventLogTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->eventLogTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->eventLogTableView->verticalHeader()->hide();
    ui->eventLogTableView->setStyleSheet(R"(
    QTableView {
        gridline-color: #dddddd;
        selection-background-color: #2b6cb0;
        selection-color: white;
        alternate-background-color: #f9f9f9;
    }

    QHeaderView::section {
        background-color: antiquewhite;
        color: black;
        padding: 4px;
        border: 1px solid #bbbbbb;
        border-top: 1px solid #bbbbbb;
        border-bottom: 1px solid #bbbbbb;
        border-right: 1px solid #bbbbbb;
        font-weight: bold;
    }

    QTableCornerButton::section {
        background-color: #dddddd;
        border: none;
    }

    QScrollBar:vertical {
        width: 12px;
    }
)");

    // set item delegate to color rows with Error in Red
    //ui->eventLogTableView->setColumnHidden(0, true);

    delegate = new DisplayViewDelegate(ui->eventLogTableView);
    ui->eventLogTableView->setItemDelegate(delegate);

    // make table view scroll automatically to the last row when adding items
    connect(m_sqlTableModel, &QSqlTableModel::rowsInserted, this, [this]() {
        QTimer::singleShot(0, ui->eventLogTableView, &QTableView::scrollToBottom);
    });
}

EventLogWidget::~EventLogWidget()
{
    delete ui;
}

QString EventLogWidget::eventTypeToString(EVENT_TYPE eventType) const
{
    switch(eventType)
    {
    case EVENT_TYPE::Debug:
        return QStringLiteral("DEBUG");
    case EVENT_TYPE::Info:
        return QStringLiteral("INFO");
    case EVENT_TYPE::Warning:
        return QStringLiteral("WARNING");
    case EVENT_TYPE::Error:
        return QStringLiteral("ERROR");
    case EVENT_TYPE::Critital:
        return QStringLiteral("CRITICAL");
    }

    return QString();
}

void EventLogWidget::addEvent(const QDateTime &timestamp, const EVENT_TYPE &eventType, const QString &source, const QString &message)
{
    QSqlRecord record = m_sqlTableModel->record();
    record.setValue("Timestamp", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    record.setValue("Severity", eventTypeToString(eventType));
    record.setValue("Source", source);
    record.setValue("Message", message);

    if (!m_sqlTableModel->insertRecord(-1, record))
    {
        qDebug() << "Insert failed:" << m_sqlTableModel->lastError();
    }

    // Submit inserts
    m_sqlTableModel->submitAll();
}

void EventLogWidget::removeEventItem(int index)
{
    m_sqlTableModel->removeRow(index);
    m_sqlTableModel->submitAll();
}

void EventLogWidget::clearEventLog()
{
    QSqlQuery query("DELETE FROM events");
    m_sqlTableModel->select();
}

void EventLogWidget::on_clearEventLogBtn_clicked()
{

    auto reply = QMessageBox::question(this,
                                     tr("Clear Event Log"), tr("Are you sure you want to delete ALL event logs?\nThis operation cannot be undone."),
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        clearEventLog();
    }
}

bool EventLogWidget::exportCSV(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;


    QTextStream stream(&file);

    stream << "Timestamp,Severity,Source,Message\n";

    QStringList lines;
    lines.reserve(100000);

    QSqlQuery query("SELECT Timestamp, Severity, Source, Message FROM events ORDER BY ID DESC LIMIT 100000");
    if (!query.isActive())
    {
        qDebug() << "Query failed:" << query.lastError();
        return false;
    }

    while (query.next())
    {
        QString ts   = query.value(0).toString().replace(',', ' ');
        QString sev  = query.value(1).toString().replace(',', ' ');
        QString src  = query.value(2).toString().replace(',', ' ');
        QString msg  = query.value(3).toString().replace(',', ' ');

        lines << QString("%1,%2,%3,%4").arg(ts).arg(sev).arg(src).arg(msg);
    }

    // Write all collected lines once
    stream << lines.join("\n");
    stream << "\n"; // newline at end (optional)

    return true;
}

void EventLogWidget::on_removeLogItemBtn_clicked()
{
    QModelIndex index = ui->eventLogTableView->currentIndex();

    if (index.isValid())
    {
        removeEventItem(index.row());
    }
}

void EventLogWidget::on_exportCSVBtn_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save Event Log to CSV"), QDir::homePath() + "/event_log.csv", tr("CSV Files (*.csv);;All Files (*)"));
    if (filePath.isEmpty())
        return;  // User cancelled

    exportCSV(filePath);
}

