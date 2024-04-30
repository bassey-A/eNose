#include "enosegraph.h"

eNoseGraph::eNoseGraph(QChart *chart, QWidget *parent)
    : QMainWindow(parent)
{
    QChartView *chartView = new QChartView(chart);
    setCentralWidget(chartView);
    chartView->show();
}
