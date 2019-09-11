#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtDataVisualization>
#include "conversion.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>
#include <QComboBox>
#include <QTextEdit>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void calculateSpecificPoint();
    int surfaceCounter = 0;
    QtDataVisualization::Q3DSurface *graph = new QtDataVisualization::Q3DSurface();
    QtDataVisualization::QSurface3DSeries *series1 = new QtDataVisualization::QSurface3DSeries();
    QtDataVisualization::QSurface3DSeries *series2 = new QtDataVisualization::QSurface3DSeries();
    QtDataVisualization::QSurface3DSeries *series3 = new QtDataVisualization::QSurface3DSeries();
    QtDataVisualization::QSurface3DSeries *series4 = new QtDataVisualization::QSurface3DSeries();
    QtDataVisualization::QSurface3DSeries *series5 = new QtDataVisualization::QSurface3DSeries();
    QtDataVisualization::QSurface3DSeries *series6 = new QtDataVisualization::QSurface3DSeries();
    int semicolon_counter(QString str);
    void addSeries(QtDataVisualization::QSurfaceDataArray *array);
	void addEquationToField();
    bool checkInputAndSetColor();

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_clicked();
    void on_pushButton_8_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
