#include "mainwindow.h"
#include "datamanager.h"
#include "ui_mainwindow.h"
#include "conversion.h"
#include <QtDataVisualization>
#include <QtWidgets>
#include <QtCore>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>

using namespace QtDataVisualization;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->comboBox->addItems(QStringList() << "z=" << "y=" << "x=");
    QWidget *container = QWidget::createWindowContainer(graph);
    ui->verticalLayout->addWidget(container);
	ui->textEdit_11->setText("-10");
    ui->textEdit_12->setText("10");
    ui->textEdit_13->setText("-10");
    ui->textEdit_14->setText("10");
    ui->textEdit_15->setText("1");
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Surface and text removal
void MainWindow::on_pushButton_2_clicked()
{
    ui->textEdit_2->setText("");
    graph->removeSeries(series1);
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->textEdit_3->setText("");
    graph->removeSeries(series2);
}

void MainWindow::on_pushButton_4_clicked()
{
    ui->textEdit_4->setText("");
    graph->removeSeries(series3);
}

void MainWindow::on_pushButton_5_clicked()
{
    ui->textEdit_5->setText("");
    graph->removeSeries(series4);
}

void MainWindow::on_pushButton_6_clicked()
{
    ui->textEdit_6->setText("");
    graph->removeSeries(series5);
}

void MainWindow::on_pushButton_7_clicked()
{
    ui->textEdit_7->setText("");
    graph->removeSeries(series6);
}

/**
 * @brief MainWindow::on_pushButton_clicked when clicked, first entered values are saved to variables
 * which are sent to background methods to calculate with them,
 * if some of the inputs are invalid, text in that field is colored red.
 */
void MainWindow::on_pushButton_clicked()
{
    bool inputOK = checkInputAndSetColor();
    if(inputOK){
        Conversion c;
        DataManager dm;
        QStringList values;
        calculateSpecificPoint();
        QSurfaceDataArray *array = new QSurfaceDataArray();
        QString letter = ui->comboBox->currentText();
        QString expression = ui->textEdit->toPlainText();
        QString rate = ui->textEdit_15->toPlainText();
        QString min1 = ui->textEdit_11->toPlainText();
        QString max1 = ui->textEdit_12->toPlainText();
        QString min2 = ui->textEdit_13->toPlainText();
        QString max2 = ui->textEdit_14->toPlainText();
        QString two_part_range = ui->textEdit_16->toPlainText();
        values << letter + expression << min1 << max1 << min2 << max2 << rate << two_part_range;
        bool existsInJson = dm.existsInJsonFile(values);
        if(existsInJson){
            *array = dm.representJsonDataOnGraph(graph);
        }
        else{
            *array = dm.representDataOnGraph(letter.toStdString(),values, graph, dm.createJsonFile(values));      
        }
        addEquationToField();
        addSeries(array);

    }
}

/**
 * @brief MainWindow::calculateSpecificPoint if other two coordinates are entered,
 * this will calculate third coordinate of that point and set the result to corresponding field.
 */
void MainWindow::calculateSpecificPoint(){
    Conversion c;
	string equation = ui->textEdit->toPlainText().toStdString();
	string letter = ui->comboBox->currentText().toStdString();
    std::string changed_equation;
	string replace1, replace2;
    double result;
    ui->textEdit_10->setText("");
    if(letter == "z="){
        replace1 = "x";
        replace2 = "y";
    } else if(letter == "y="){
        replace1 = "x";
        replace2 = "z";
    } else{
        replace1 = "y";
        replace2 = "z";
    }
    if(ui->textEdit_8->toPlainText() != "" && ui->textEdit_8->toPlainText() != ""){
            changed_equation = c.replaceSymbol(equation,replace1, ui->textEdit_8->toPlainText().toDouble());
            changed_equation = c.replaceSymbol(changed_equation, replace2, ui->textEdit_9->toPlainText().toDouble());
            result = c.SolveEquation(c.infixToPostfix(changed_equation));
            ui->textEdit_10->setText(QString::number(result));
        }
    }
	
void MainWindow::on_pushButton_8_clicked()
{
    QMessageBox::information(this, "Info", "For two-part equations write the range of other two coordinates here, "
                                           "delimited by ; (semicolon), and do the same in main input field. These"
                                           "two fields should contain one semicolon more than input field.\nFor example,"
                                           " for equation that has z value x+y in range x and y[0-4] and has value"
                                           "x-2 in range x and y[4-10] input filed looks like x+y;x-2\n"
                                           "Advanced field: 0;4;10\n"
                                           "Order of coordinates goes alphabetically.",QMessageBox::Ok);
}

/**
 * @brief MainWindow::semicolon_counter checks how many semicolons are in some input
 * called to check if two-parts range have 1 semicolon more than entered expression
 * @param str entered expression
 * @return number of semicolons
 */
int MainWindow::semicolon_counter(QString str)
{
    int count = 0;
    for(unsigned i = 0; i < str.size(); i++){
        if(str[i] == ';')
        {count++;}
    }
    return count;

}

/**
 * @brief MainWindow::addSeries adds surface data series depending on passed number
 * the number of series is same as number of text fields.
 * @param array data array to plot on graph
 */
void MainWindow::addSeries(QSurfaceDataArray *array){
    switch (surfaceCounter)
    {
        case 1:
        series1->dataProxy()->resetArray(array);
        graph->addSeries(series1);
        break;

        case 2:
        series2->dataProxy()->resetArray(array);
        graph->addSeries(series2);
        break;

        case 3:
        series3->dataProxy()->resetArray(array);
        graph->addSeries(series3);
        break;

        case 4:
        series4->dataProxy()->resetArray(array);
        graph->addSeries(series4);
        break;

        case 5:
        series5->dataProxy()->resetArray(array);
        graph->addSeries(series5);
        break;

        case 6:
        series6->dataProxy()->resetArray(array);
        graph->addSeries(series6);
        break;
    }
	graph->show();
}

/**
 * @brief MainWindow::addEquationToField puts entered text (takes value from combobox and joins it with
 * text entered in textEdit) to first empty text field and sets
 * the surfaceCounter variable, so when putting data series to
 * graph, the method will know which series to put.
 */
void MainWindow::addEquationToField() {
    QString input = ui->textEdit->toPlainText();
    QString combo_value = ui->comboBox->currentText();
	if(ui->textEdit_2->toPlainText() == ""){
        surfaceCounter = 1;
		ui->textEdit_2->setText(combo_value + input);
	}else if(ui->textEdit_3->toPlainText() == ""){
        surfaceCounter = 2;
		ui->textEdit_3->setText(combo_value + input);
	}else if(ui->textEdit_4->toPlainText() == ""){
        surfaceCounter = 3;
		ui->textEdit_4->setText(combo_value + input);
	}else if(ui->textEdit_5->toPlainText() == ""){
        surfaceCounter = 4;
		ui->textEdit_5->setText(combo_value + input);
	}else if(ui->textEdit_6->toPlainText() == ""){
        surfaceCounter = 5;
		ui->textEdit_6->setText(combo_value + input);
	}else if(ui->textEdit_7->toPlainText() == ""){
        surfaceCounter = 6;
		ui->textEdit_7->setText(ui->comboBox->currentText() + ui->textEdit->toPlainText());
    }
}

/**
 * @brief MainWindow::checkInputAndSetColor checks entered expressions and all numbers validity.
 * All cases covered if any input is wrong, color is changed to red, it also returns bool
 * to proceed with calculations only if inputs are valid.
 * @return true if input valid, false otherwise
 */
bool MainWindow::checkInputAndSetColor()
{
    DataManager dm;
    Conversion c;
    QString two_part_eq = ui->textEdit_16->toPlainText();
    bool allValid = true;
    if(two_part_eq != "" && semicolon_counter(two_part_eq) != semicolon_counter(ui->textEdit->toPlainText())+1){
        ui->textEdit->setStyleSheet("color:red");
        ui->textEdit_16->setStyleSheet("color:red");
        return false;
    }
    else{
        ui->textEdit->setStyleSheet("color:black");
        ui->textEdit_16->setStyleSheet("color:black");
    }
    if(!dm.isExpressionValid(ui->textEdit->toPlainText(), ui->comboBox->currentText())){
        ui->textEdit->setStyleSheet("color:red");
        allValid = false;
    }
    else{
        ui->textEdit->setStyleSheet("color:black");
    }
    if(!c.isWholeInputNumber(ui->textEdit_11->toPlainText().toStdString())){
        ui->textEdit_11->setStyleSheet("color:red");
        allValid = false;
    }
    else{
        ui->textEdit_11->setStyleSheet("color:black");
    }
    if(!c.isWholeInputNumber(ui->textEdit_12->toPlainText().toStdString())){
        ui->textEdit_12->setStyleSheet("color:red");
        allValid = false;
    }
    else{
        ui->textEdit_12->setStyleSheet("color:black");
    }
    if(!c.isWholeInputNumber(ui->textEdit_13->toPlainText().toStdString())){
        ui->textEdit_13->setStyleSheet("color:red");
        allValid = false;
    }
    else{
        ui->textEdit_13->setStyleSheet("color:black");
    }
    if(!c.isWholeInputNumber(ui->textEdit_14->toPlainText().toStdString())){
        ui->textEdit_14->setStyleSheet("color:red");
        allValid = false;
    }
    else{
        ui->textEdit_14->setStyleSheet("color:black");
    }
    if(!c.isWholeInputNumber(ui->textEdit_15->toPlainText().toStdString()) || ui->textEdit_15->toPlainText().toDouble() <= 0){
        ui->textEdit_15->setStyleSheet("color:red");
        allValid = false;
    }
    else{
        ui->textEdit_15->setStyleSheet("color:black");
    }
    return allValid;
}
