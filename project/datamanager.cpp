#include "datamanager.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "conversion.h"

using namespace QtDataVisualization;

DataManager::DataManager(QObject *parent) : QObject(parent) {

}

/**
 * @brief DataManager::representDataOnGraph checks entered expression and
 * values and calculates points to form data array
 * @param letter combobox chosen item
 * @param values values to read, equation, range, rate
 * @param graph surface graph to fill with data
 * @param jsonobject object to update json file with data
 * @return array of data
 */
QSurfaceDataArray DataManager::representDataOnGraph(std::string letter, QList<QString> values, Q3DSurface *graph, QJsonObject jsonobject) {
    QStringList two_part_range = values[6].split(";");
    QStringList two_part_eq = values[0].remove(0,2).split(";");
    QStringList pointsData;
    graph->setFlags(graph->flags() ^ Qt::FramelessWindowHint);
    double var1 = (two_part_eq.size() == 2) ?two_part_range[0].toDouble() :NULL;
    double var2 = (two_part_eq.size() == 2) ?two_part_range[1].toDouble() :NULL;
    double var3 = (two_part_eq.size() == 2) ?two_part_range[2].toDouble() :NULL;
    double min1 = values[1].toDouble(), max1 = values[2].toDouble();
    double min2 = values[3].toDouble(), max2 = values[4].toDouble();
    double rate = values[5].toDouble();
    string expression;
    string changedExpression;
    Conversion c;
    QSurfaceDataArray *array = new QSurfaceDataArray();
    try{
        for(double i= min1 ; i < max1; i+=rate){
            QSurfaceDataRow *row = new QSurfaceDataRow(ceil((abs(max2) + abs(min2))/rate));
            int position=0;
            for(double j = min2; j < max2; j+=rate){
                if(var1 != NULL && var2 != NULL && var3 != NULL && i >= var1 && i < var2 && j>= var1 && j<var2){
                    expression = two_part_eq[0].toStdString();
                }else if(var1 != NULL && var2 != NULL && var3 != NULL && i >= var2 && i < var3 && j>= var2 && j<var3){
                    expression = two_part_eq[1].toStdString();
                }
                else{
                    expression = two_part_eq[0].toStdString();
                }
                if(letter == "z=") {
                    changedExpression = c.replaceSymbol(expression, "x", i);
                    changedExpression = c.replaceSymbol(changedExpression, "y", j);
                    double result = c.SolveEquation(c.infixToPostfix(changedExpression));
                    pointsData << QString::number(i) + ";" + QString::number(j) +  ";" + QString::number(result) + ";" + QString::number(position);
                    (*row)[position].setPosition(QVector3D(i, j, result));

                }
                else if(letter == "y=") {
                    changedExpression = c.replaceSymbol(expression, "x", i);
                    changedExpression = c.replaceSymbol(changedExpression, "z", j);
                    double result = c.SolveEquation(c.infixToPostfix(changedExpression));
                    pointsData << QString::number(i) + ";" + QString::number(result) +  ";" + QString::number(j) + ";" + QString::number(position);
                    (*row)[position].setPosition(QVector3D(i, result, j));
                }
                else if(letter == "x=") {
                    changedExpression = c.replaceSymbol(expression, "y", i);
                    changedExpression = c.replaceSymbol(changedExpression, "z", j);
                    double result = c.SolveEquation(c.infixToPostfix(changedExpression));
                    pointsData << QString::number(result) + ";" + QString::number(i) +  ";" + QString::number(j) + ";" + QString::number(position);
                    (*row)[position].setPosition(QVector3D(result, i, j));
                }
                position++;
            }
            *array << row;
        }
    }
    catch(...)
    {}
    updateJsonFile(jsonobject, pointsData);
    return *array;
}

/**
 * @brief DataManager::representJsonDataOnGraph reads JSON file and puts all the
 * points to data array which will be sent to a method to show it on a graph
 * @param graph surface graph to fill with data
 * @return array of data
 */
QSurfaceDataArray DataManager::representJsonDataOnGraph(Q3DSurface *graph){
    graph->setFlags(graph->flags() ^ Qt::FramelessWindowHint);
    QSurfaceDataArray *array = new QSurfaceDataArray();
    QSurfaceDataRow *row;
    int position;

    QFile jsonfile("parseddata.json");
    jsonfile.open(QIODevice::ReadOnly);
    QByteArray bytearray = jsonfile.readAll();
    jsonfile.close();
    QJsonDocument jsondocument = QJsonDocument::fromJson(bytearray);
    QJsonObject jsonobject = jsondocument.object();
    QJsonValue value = jsonobject.value("points_on_graph");
    QJsonArray pointsArray = value.toArray();

    double min2 = jsonobject["coord2_min"].toDouble();
    double max2 = jsonobject["coord2_max"].toDouble();
    double rate = jsonobject["rate"].toDouble();

    Q_FOREACH(const QJsonValue &v, pointsArray) {
        position = v.toObject().value("position").toInt();
        if(position == 0){
            row = new QSurfaceDataRow(ceil((abs(min2) + abs(max2))/rate));
        }
        (*row)[position].setPosition(QVector3D(v.toObject().value("x").toInt(),v.toObject().value("y").toInt(),v.toObject().value("z").toInt()));
        *array << row;
    }

    return *array;
}


/**
 * @brief DataManager::isExpressionValid checks if equation input is valid, if there is only one
 * operator in a row, if input field doesn't contain value from combobox
 * e.g. z = z + 4, and if there are no letters except ones used for operations.
 * @param expression input to check, text in field converted to QString
 * @param letter combobox input, QString type to check if expression contains it
 * @return true if expression valid, false otherwise
 */
bool DataManager::isExpressionValid(QString expression, QString letter) {
    QString operators = "+-/*^STCKLQ";
    QString binaryOperators= "+-*/^";
    QString variables = "xyz";
    if(expression.contains(letter[0])){
		return false;
	}
    if(binaryOperators.contains(expression[0]) && expression[0] != '-'){
        return false;
    }
    if(operators.contains(expression[expression.size()-1])){
        return false;
    }
    if(expression == ""){
        return false;
    }
    else{
        for(unsigned i = 0; i < expression.size(); i++) {
            if(binaryOperators.contains(expression[i]) && binaryOperators.contains(expression[i+1])) {
                return false;
            }
            if(expression[i].isLetter() && !operators.contains(expression[i]) && !variables.contains(expression[i])){
                return false;
            }
            if(variables.contains(expression[i]) && (expression[i-1].isNumber() || expression[i+1].isNumber())){
                return false;
            }
        }
    }
	return true;
}

// creates JSON file, in this method only user input is added to file,
// not the points. Equation id, range for both other coordinates,
// rate and two_part range (this one can be empty, others cannot).
/**
 * @brief DataManager::createJsonFile creates JSON file, in this method only user input is added to file,
 * not the points. Equation id, range for both other coordinates,
 * rate and two_part range (this one can be empty, others cannot).
 * @param valuesForJson list of data, type string, to write in file
 * @return JSON object that is created from those values
 */
QJsonObject DataManager::createJsonFile(QStringList valuesForJson){
    QFile jsonfile("parseddata.json");
    QJsonDocument jsondocument;
    QJsonObject jsonobject;

    jsonobject["equation_id"] = valuesForJson[0];
    jsonobject["coord1_min"] = valuesForJson[1].toDouble();
    jsonobject["coord1_max"] = valuesForJson[2].toDouble();
    jsonobject["coord2_min"] = valuesForJson[3].toDouble();
    jsonobject["coord2_max"] = valuesForJson[4].toDouble();
    jsonobject["rate"] = valuesForJson[5].toDouble();
    jsonobject["advanced"] = valuesForJson[6];

    jsondocument.setObject(jsonobject);
    QByteArray bytearray = jsondocument.toJson();
    if(jsonfile.open(QIODevice::WriteOnly)){
         jsonfile.write(bytearray);
         jsonfile.close();
    }
    return jsonobject;
}

/**
 * @brief DataManager::updateJsonFile overwrite existing data in JSON file with new one
 * @param jsonobject object containing data to write in file
 * @param pointsData list of coordinate triples to write in file
 */
void DataManager::updateJsonFile(QJsonObject jsonobject, QStringList pointsData)
{
    QFile jsonfile("parseddata.json");
    QJsonObject points;
    QJsonArray pointsArray;
    QByteArray bytearray;
    QJsonDocument jsondocument;

    for(int i = 0; i < pointsData.size(); i++){
        points["x"] = pointsData[i].split(";")[0].toDouble();
        points["y"] = pointsData[i].split(";")[1].toDouble();
        points["z"] = pointsData[i].split(";")[2].toDouble();
        points["position"] = pointsData[i].split(";")[3].toInt();
        pointsArray.push_back(points);
    }
    jsonobject.insert("points_on_graph",QJsonArray(pointsArray));
    jsondocument.setObject(jsonobject);
    if(jsonfile.open(QIODevice::WriteOnly)){
         bytearray = jsondocument.toJson();
         jsonfile.write(bytearray);
         jsonfile.close();
    }
}

/**
 * @brief DataManager::existsInJsonFile checks if JSON file contains entered data
 * @param values array of values entered in text fields to check
 * @return bool if all data in JSON file, false otherwise
 */
bool DataManager::existsInJsonFile(QStringList values){
    QFile jsonfile("parseddata.json");
    QJsonDocument jsondocument;
    QJsonObject jsonobject;
    QByteArray bytearray;
    bool exists = false;
    if(jsonfile.open(QIODevice::ReadOnly)){
        bytearray = jsonfile.readAll();
        jsondocument = jsondocument.fromJson(bytearray);
        jsonobject = jsondocument.object();

        if(jsonobject["equation_id"] == values[0] && jsonobject["coord1_min"] == values[1] && jsonobject["coord1_max"] == values[2] &&
        jsonobject["coord2_min"] == values[3] && jsonobject["coord2_max"] == values[4] && jsonobject["rate"] == values[5] &&
        jsonobject["advanced"] == values[6]){
            //QJsonValue value = jsonobject.value("points_on_graph");
            //pointsArray = value.toArray();
            exists = true;
        }
    }
    return exists;
}

