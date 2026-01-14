#include "eventlogwidget.h"
#include "ui_eventlogwidget.h"

EventLogWidget::EventLogWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EventLogWidget)
{
    ui->setupUi(this);

    m_eventModel = new EventLogModel(this);

    // proxy filter
    m_filterProxy = new EventFilterProxy(this);
    m_filterProxy->setSourceModel(m_eventModel->model());
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
    connect(m_eventModel->model(), &QSqlTableModel::rowsInserted, this, [this]() {
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
    m_eventModel->addEvent(timestamp, eventTypeToString(eventType), source, message);
}

void EventLogWidget::removeEventItem(int index)
{
    m_eventModel->removeEvent(index);
}

void EventLogWidget::clearEventLog()
{
    m_eventModel->clearAll();
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
    return m_eventModel->exportCSV(filePath);
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

