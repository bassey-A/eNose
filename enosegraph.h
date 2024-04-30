#ifndef ENOSEGRAPH_H
#define ENOSEGRAPH_H

#include <QChartView>
#include <QMainWindow>

class eNoseGraph : public QMainWindow
{
    Q_OBJECT
public:
    explicit eNoseGraph(QChart *chart, QWidget *parent = nullptr);

signals:

private:

    QObject *QMainWindow;
};

#endif // ENOSEGRAPH_H
