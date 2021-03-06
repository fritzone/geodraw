#ifndef DRAWINGFORM_H
#define DRAWINGFORM_H

#include "CodeEngine.h"
#include "RuntimeProvider.h"

#include <QPen>
#include <QWidget>


class MyGraphicsView;
class PlotRenderer;
class AbstractDrawer;
class QGraphicsScene;
class ProjectedRenderer;

namespace Ui {
class DrawingForm;
}



class DrawingForm : public QWidget
{
    Q_OBJECT

public:
    explicit DrawingForm(RuntimeProvider *rp, QWidget *parent = nullptr);
    ~DrawingForm();

    void drawCoordinateSystem();



    void setDrawingPen(int r, int g, int b, int a);

    QVector<QPointF> drawPlot(QSharedPointer<Plot> plot);

    void reset();

    void resizeEvent(QResizeEvent *event);

    void drawPoint(double x, double y);

signals:
    void contentChanged();

private:


    template<class E>
    void genericPlotIterator(QSharedPointer<Plot> plot, E executor)
    {
        m_rp->genericPlotIterator(plot, executor);
    }


    Ui::DrawingForm *ui;

    QPen drawingPen;
    RuntimeProvider* m_rp;

    AbstractDrawer* m_ar = nullptr;

    PlotRenderer* m_pr = nullptr;
    MyGraphicsView* graphicsView = nullptr;
    ProjectedRenderer *view = nullptr;

};

#endif // DRAWINGFORM_H
