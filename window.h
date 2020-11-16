#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QApplication>

#include <QPaintEvent>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QAbstractButton>

#include <QMatrix4x4>
#include <QVector4D>
#include <QVector3D>

#include <QPainter>
#include <QPen>

#include <vector>
#include <cmath>
#include <iostream>


class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window(QWidget *parent = nullptr);
    ~Window();
private:
    void cocentrate(std::vector<QVector4D>& pnts);
    double scalar(const QVector3D& , const QVector3D& );
    double max_coordinate(const std::vector<QVector4D>& vec);
    bool visible_at_oz(const QVector4D& , const QVector4D& );

    QLabel *LFI, *LPSI, *LTETA, *LSCALE;
    QDoubleSpinBox *FI, *PSI, *TETA, *SCALE;
    QCheckBox *Box_Vis, *Box_izo;
    double MAX_SCALE=300;
    void normal(QVector4D& vec);
protected:
    void paintEvent(QPaintEvent*);

private slots:
    void redrawOnValueChange(double);
    void redrawOnValueChange(bool);

};


#endif // WINDOW_H
