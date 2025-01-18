#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "connection.h"

#include <QChart>
#include <QDateTimeAxis>
#include <QValueAxis>

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

    bool can_close = true;

private slots:
    void on_secButton_clicked();
    void on_hourButton_clicked();
    void on_dayButton_clicked();

private:
    Ui::MainWindow *ui;

    QPointer<Connection> connection;

    QPointer<QChart> chart;
    QPointer<QDateTimeAxis> axisX;
    QPointer<QValueAxis> axisY;

    void RecievedData(const QString data);

    void keyPressEvent(QKeyEvent *ev) override;
    void closeEvent(QCloseEvent *ev) override;
};
#endif 
