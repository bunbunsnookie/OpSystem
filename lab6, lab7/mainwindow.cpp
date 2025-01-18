#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QLineSeries>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , chart(new QChart)
    , connection(new Connection)
{
    connect(connection, &Connection::RecievedString, this, &MainWindow::RecievedData);

    ui->setupUi(this);

    ui->graphicsView->setChart(chart);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::RecievedData(const QString data)
{
    chart->removeAllSeries();
    chart->legend()->hide();
    if (!axisX.isNull())
        chart->removeAxis(axisX);
    if (!axisY.isNull())
        chart->removeAxis(axisY);

    QPointer<QLineSeries> series = new QLineSeries;

    auto split = data.split("\n");
    for (int i = 0; i < split.size(); i++)
    {
        auto line = split[i].split(" ");

        if (line.size() > 1)
            series->append(line[0].toULongLong() * 1000, line[1].toDouble());
    }

    chart->addSeries(series);

    axisX = new QDateTimeAxis;
    axisX->setFormat("dd.MM.yy hh:mm:ss");
    axisX->setTitleText("Дата");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    axisY = new QValueAxis;
    axisY->setLabelFormat("%.2f");
    axisY->setTitleText("Температура");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
}

void MainWindow::on_secButton_clicked()
{
    connection->GetRequest(ui->ipEdit->text(), "/sec");
}


void MainWindow::on_hourButton_clicked()
{
    connection->GetRequest(ui->ipEdit->text(), "/hour");
}


void MainWindow::on_dayButton_clicked()
{
    connection->GetRequest(ui->ipEdit->text(), "/day");
}


void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    using Qt::KeyboardModifier;

    auto keys = ev->keyCombination();

    if (keys.keyboardModifiers() == QFlags(Qt::ControlModifier|Qt::AltModifier) && keys.key() == Qt::Key_Space)
    {
        can_close = true;
        qDebug() << "!!";
    }
}

void MainWindow::closeEvent(QCloseEvent *ev)
{
    if (!can_close)
    {
        ev->ignore();
    }
}
