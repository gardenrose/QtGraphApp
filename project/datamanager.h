#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QtWidgets>
#include <QtDataVisualization/Q3DSurface>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>
#include <QByteArray>

using namespace QtDataVisualization;
class DataManager : public QObject
{
    Q_OBJECT
public:
    explicit DataManager(QObject *parent = nullptr);
    QSurfaceDataArray representJsonDataOnGraph(Q3DSurface *graph);
    QSurfaceDataArray representDataOnGraph(std::string letter, QList<QString> values, Q3DSurface *graph, QJsonObject jsonobject);
    QJsonObject createJsonFile(QStringList valuesForJson);
    void updateJsonFile(QJsonObject jsonobject, QStringList pointsData);
    bool isExpressionValid(QString equation, QString letter);
    bool existsInJsonFile(QStringList values);

signals:

public slots:
};

#endif // DATAMANAGER_H
