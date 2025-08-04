#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "poopEntry.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ✅ 设置选中行的颜色（天蓝色背景 + 黑色文字）
    ui->tableWidget->setStyleSheet(
        "QTableWidget::item:selected { background-color: #B2FFB2; color: black; }"
        );
    loadPoopRecords();  // 启动时加载记录
    // 🟡 按钮点击后调用 recordPoop
    connect(ui->buttonDeletePoop, &QPushButton::clicked, this, &MainWindow::onDeletePoopClicked);
    connect(ui->buttonAddNowPoop, &QPushButton::clicked, this, &MainWindow::onAddNowPoopClicked);
    connect(ui->pushButtonAddPoop, &QPushButton::clicked, this, &MainWindow::onAddPoopClicked);
    ui->dateTimeEditPoop->setDateTime(QDateTime::currentDateTime()); // 🟢 设置当前时间
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "DateTime" << "Normal" << "Hard" << "Soft");
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateTableWithPoopEntry(const PoopEntry& entry)
{
    QString timeStr = entry.time.toString("yyyy-MM-dd hh:mm:ss");

    // 查找是否已有这一行
    int row = -1;
    for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        if (ui->tableWidget->item(i, 0)->text() == timeStr) {
            row = i;
            break;
        }
    }

    if (row == -1) {
        row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        QTableWidgetItem* timeItem = new QTableWidgetItem(timeStr);
        timeItem->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(row, 0, timeItem);

        for (int i = 1; i <= 3; ++i) {
            QTableWidgetItem* emptyItem = new QTableWidgetItem("");
            emptyItem->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(row, i, emptyItem);
        }
    }

    int col = -1;
    switch (entry.status)
    {
    case PoopStatus::Normal: col = 1; break;
    case PoopStatus::Hard: col = 2; break;
    case PoopStatus::Soft: col = 3; break;
    }
    if (col != -1) {
        QTableWidgetItem* item = new QTableWidgetItem("✔️");
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(row, col, item);
    }
}

void MainWindow::recordPoop(const QDateTime& time, PoopStatus status)
{
    PoopEntry entry{ time, status };
    poopRecords.append(entry);
    savePoopRecords();
    updateTableWithPoopEntry(entry);
}

void MainWindow::onAddNowPoopClicked()
{
    QDateTime currentTime = QDateTime::currentDateTime();

    QString statusStr = ui->comboBoxStatus->currentText(); // 用现有的状态
    PoopStatus status = poopStatusFromString(statusStr);

    // 追加记录
    recordPoop(currentTime, status);
}


void MainWindow::onAddPoopClicked()
{
    QDateTime time = ui->dateTimeEditPoop->dateTime();
    QString statusStr = ui->comboBoxStatus->currentText();
    PoopStatus status = poopStatusFromString(statusStr);

    if (!poopRecords.isEmpty()) {
        QDateTime lastTime = poopRecords.last().time;
        if (time <= lastTime) {
            QMessageBox::warning(this, "无效时间",
                                 "您选择的时间是：" + time.toString("yyyy-MM-dd hh:mm:ss") +
                                     "\n但最新记录是：" + lastTime.toString("yyyy-MM-dd hh:mm:ss") +
                                     "\n\n请设置一个更晚的时间。");
            return;
        }
    }

    recordPoop(time, status);
}


void MainWindow::savePoopRecords()
{
    QJsonArray jsonArray;
    for (int i = 0; i < poopRecords.size(); ++i)
    {
        jsonArray.append(poopRecords[i].toJson());
    }
    QJsonDocument doc(jsonArray);
    QFile file("poop_data.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

void MainWindow::loadPoopRecords()
{
    QFile file("poop_data.json");
    if (!file.open(QIODevice::ReadOnly))
        return;

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray())
        return;

    QJsonArray jsonArray = doc.array();
    for (int i = 0; i < jsonArray.size(); ++i)
    {
        QJsonObject obj = jsonArray[i].toObject();
        PoopEntry entry = PoopEntry::fromJson(obj);
        poopRecords.append(entry);
        updateTableWithPoopEntry(entry);
    }

}

void MainWindow::onDeletePoopClicked()
{
    QList<QTableWidgetSelectionRange> selectedRanges = ui->tableWidget->selectedRanges();
    if (selectedRanges.isEmpty()) {
        QMessageBox::information(this, "未选择", "请先选择一条或多条要删除的记录。");
        return;
    }

    int ret = QMessageBox::question(
        this, "确认删除",
        "你确定要删除所选记录吗？",
        QMessageBox::Yes | QMessageBox::No
        );

    if (ret != QMessageBox::Yes)
        return;

    // 获取选中行对应的时间字符串
    QSet<QString> selectedTimes;
    for (const QTableWidgetSelectionRange& range : selectedRanges) {
        for (int row = range.topRow(); row <= range.bottomRow(); ++row) {
            if (row >= 0 && row < ui->tableWidget->rowCount()) {
                QString timeStr = ui->tableWidget->item(row, 0)->text();
                selectedTimes.insert(timeStr);
            }
        }
    }

    // 删除 poopRecords 中匹配的
    for (int i = poopRecords.size() - 1; i >= 0; --i) {
        QString entryTime = poopRecords[i].time.toString("yyyy-MM-dd hh:mm:ss");
        if (selectedTimes.contains(entryTime)) {
            poopRecords.removeAt(i);
        }
    }

    // 删除 UI 中匹配的
    for (int row = ui->tableWidget->rowCount() - 1; row >= 0; --row) {
        QString timeStr = ui->tableWidget->item(row, 0)->text();
        if (selectedTimes.contains(timeStr)) {
            ui->tableWidget->removeRow(row);
        }
    }

    savePoopRecords();

    QMessageBox::information(this, "删除成功", "已删除所选记录。");
}




