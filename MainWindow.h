#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPen>
#include <QMap>
#include <QSet>
#include <QDebug>

#include "Function.h"
#include "RuntimeProvider.h"
#include "CodeEngine.h"
#include "TextEditWithCodeCompletion.h"

#include <functional>

class QGraphicsScene;
class MyGraphicsView;

namespace Ui {
class MainWindow;
}

struct DrawnLine
{
    QLineF line;
    QPen pen;
};

struct DrawnPoint
{
    QPointF point;
    QPen pen;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void reportError(QString err);
    QVector<QPointF> drawPlot(QSharedPointer<Plot>);
    void setCurrentStatement(const QString &newCurrentStatement);
    void drawPoint(double x, double y);

private slots:
    void on_toolButton_clicked();
    void dockWidgetTopLevelChanged(bool);

private:
    void drawCoordinateSystem();
    void setDrawingPen(int, int, int, int);

    int sceneX(double x);
    int sceneY(double y);
    double coordStartX();
    double coordEndX();
    double coordStartY();
    double coordEndY();
    double zoomFactor();
    double rotationAngle();
    QPoint toScene(QPointF);

    template<class E>
    void genericPlotIterator(QSharedPointer<Plot> plot, E executor)
    {
        rp.genericPlotIterator(plot, executor);
    }

    void resizeEvent(QResizeEvent* event);
    void redrawEverything();

    Ui::MainWindow *ui;
    QGraphicsScene* sc = nullptr;

    // in case we resize/zoom/scroll the window, these objects will be used to redraw the scene
    QVector<DrawnLine> drawnLines;
    QVector<DrawnPoint> drawnPoints;
    friend class Sett;
    friend class Assignment;
    QPen drawingPen;
    QString currentStatement;
    RuntimeProvider rp;

    MyGraphicsView* graphicsView = nullptr;
    QDockWidget *dock = nullptr;
    TextEditWithCodeCompletion *textEdit = nullptr;
    FrameForLineNumbers* frmLineNrs = nullptr;
};

#endif // MAINWINDOW_H
