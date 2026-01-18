#include "eventlogwidget.h"
#include "ui_eventlogwidget.h"

EventLogWidget::EventLogWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EventLogWidget)
{
    ui->setupUi(this);

    m_controller = new EventLogController(this);

    // UI filter controls
    ui->severityFilterCombo->addItems({"All", "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"});

    connect(ui->severityFilterCombo, &QComboBox::currentTextChanged, m_controller, &EventLogController::setSeverityFilter);
    connect(ui->sourceFilterEdit, &QLineEdit::textChanged, m_controller, &EventLogController::setSourceFilter);
    connect(ui->messageFilterEdit, &QLineEdit::textChanged, m_controller, &EventLogController::setMessageFilter);
    connect(ui->fromDateTimeEdit, &QDateTimeEdit::dateTimeChanged, m_controller, &EventLogController::setFromTimestamp);
    connect(ui->toDateTimeEdit, &QDateTimeEdit::dateTimeChanged, m_controller, &EventLogController::setToTimestamp);

    ui->fromDateTimeEdit->setDateTime(QDateTime::currentDateTime().addDays(-1));
    ui->toDateTimeEdit->setDateTime(QDateTime::currentDateTime().addDays(1));

    ui->fromDateTimeEdit->setEnabled(false);
    ui->toDateTimeEdit->setEnabled(false);
    ui->severityFilterCombo->setEnabled(false);
    ui->sourceFilterEdit->setEnabled(false);
    ui->messageFilterEdit->setEnabled(false);
}

EventLogWidget::~EventLogWidget()
{
    delete ui;
}

void EventLogWidget::initializeModel(const QString &databasePath, const QString connectionName)
{
    m_controller->initializeDatabase(databasePath, connectionName);
    ui->eventLogTableView->setModel(m_controller->model());

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
    // delegate = new DisplayViewDelegate(ui->eventLogTableView);
    // ui->eventLogTableView->setItemDelegate(delegate);

    // // make table view scroll automatically to the last row when adding items
    // connect(m_controller, &EventLogController::rowsAdded,
    //         this, [this]() {
    //             auto* v = ui->eventLogTableView;
    //             bool atBottom = v->verticalScrollBar()->value() == v->verticalScrollBar()->maximum();
    //             if (atBottom)
    //                 v->scrollToBottom();
    //         });

}

void EventLogWidget::setFlushIntervalMs(int newFlushIntervalMs)
{
    m_controller->setFlushIntervalMs(newFlushIntervalMs);
}

void EventLogWidget::setMaxBatchSize(int newMaxBatchSize)
{
    m_controller->setMaxBatchSize(newMaxBatchSize);
}

void EventLogWidget::setMaxVisibleRows(int rows)
{
    m_controller->setMaxVisibleRows(rows);
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
    case EVENT_TYPE::Critical:
        return QStringLiteral("CRITICAL");
    }

    return QString();
}

void EventLogWidget::addEvent(const QDateTime &timestamp, const EVENT_TYPE &eventType, const QString &source, const QString &message)
{
    m_controller->addEvent(timestamp, eventTypeToString(eventType), source, message);
}

void EventLogWidget::removeEventItem(int index)
{
    m_controller->removeEvent(index);
}

void EventLogWidget::clearEventLog()
{
    m_controller->clearAll();
}

bool EventLogWidget::exportCSV(const QString &filePath)
{
    return m_controller->exportCSV(filePath);
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

void EventLogWidget::on_removeLogItemBtn_clicked()
{
    QModelIndex proxyIndex = ui->eventLogTableView->currentIndex();
    if (!proxyIndex.isValid())
        return;

    // Controller expects SOURCE row
    QModelIndex sourceIndex = qobject_cast<QSortFilterProxyModel*>(ui->eventLogTableView->model())->mapToSource(proxyIndex);

    m_controller->removeEvent(sourceIndex.row());
}

void EventLogWidget::on_exportCSVBtn_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save Event Log to CSV"), QDir::homePath() + "/event_log.csv", tr("CSV Files (*.csv);;All Files (*)"));
    if (filePath.isEmpty())
        return;  // User cancelled

    bool ok = exportCSV(filePath);

    if (!ok)
    {
        QMessageBox::warning(this, tr("Export Failed"), tr("Unable to write CSV file."));
    }
}

void EventLogWidget::on_enableFilterChkBox_toggled(bool checked)
{
    m_controller->setFilterEnabled(checked);
    ui->fromDateTimeEdit->setEnabled(checked);
    ui->toDateTimeEdit->setEnabled(checked);
    ui->severityFilterCombo->setEnabled(checked);
    ui->sourceFilterEdit->setEnabled(checked);
    ui->messageFilterEdit->setEnabled(checked);

}

