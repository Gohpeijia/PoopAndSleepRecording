#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include "poopEntry.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddPoopClicked();
    void onAddNowPoopClicked();
    void onDeletePoopClicked();

private:
    Ui::MainWindow *ui;
    // 储存所有记录的列表
    QList<PoopEntry> poopRecords;
    void recordPoop(const QDateTime& time, PoopStatus status);
    void savePoopRecords();
    void loadPoopRecords();
    void updateTableWithPoopEntry(const PoopEntry& entry);
};

#endif // MAINWINDOW_H
