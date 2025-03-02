#pragma once

#include <QtWidgets/QWidget>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include "ui_trafficadvisorysystem.h"
#include "graph.h"


QT_BEGIN_NAMESPACE
namespace Ui { class TrafficAdvisorySystemClass; };
QT_END_NAMESPACE

struct pathAll
{
    QStringList path;
    int length;
};
// 自定义比较函数，用于std::sort
static bool compareByLength(const pathAll& a, const pathAll& b) {
    return a.length < b.length;
}
class TrafficAdvisorySystem : public QWidget
{
    Q_OBJECT

public:
    TrafficAdvisorySystem(QWidget *parent = nullptr);
    ~TrafficAdvisorySystem();

public slots:
    void on_pushButtonVerify_clicked();
    void on_pushButtonSearch_clicked();
    void on_pushButtonSearch_2_clicked();
    void on_pushButtonSearch_3_clicked();
    void on_pushButtonSearch_4_clicked();
    void on_pushButtonSearch_5_clicked();
    void on_pushButtonQuickSort_clicked();
    void on_pushButtonClear_clicked();
    void on_pushButtonSaveRes_clicked();
    void on_pathAll(QStringList ll, int length);

private:
    Ui::TrafficAdvisorySystemClass *ui;
    QStringList cityName;
    Graph* myGraph = nullptr;
    std::vector<pathAll> pathA; // 保存所有路径，用于快速排序
    bool enabledPath = false;
};
