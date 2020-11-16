#include "window.h"
#include <QPainter>
#include <iostream>
#include <QPen>
#include <QVector3D>
void Window::redrawOnValueChange(double){
 update();
}
void Window::redrawOnValueChange(bool){
 update();
}

void Window::normal(QVector4D& vec){
    if(vec[3]==0)
        return;
    vec[0]/=vec[3];
    vec[1]/=vec[3];
    vec[2]/=vec[3];
    vec[3]/=vec[3];

}

double Window::scalar(const QVector3D& a , const QVector3D& b){
    return a.x()*b.x() + a.y()*b.y() + a.z()*b.z();
}

bool Window::visible_at_oz(const QVector4D& a, const QVector4D& b){
    return /*true; //*/(scalar(QVector3D::normal(a.toVector3D(), b.toVector3D()), QVector3D(0, 0, 1)))>=0;

}

double Window::max_coordinate(const std::vector<QVector4D>& vec){
    double max1=0, max=(
     (vec[0].x()>vec[0].y())?vec[0].x():vec[0].y() > vec[0].z())?
      (vec[0].x()>vec[0].y())?vec[0].x():vec[0].y()
       :vec[0].z(); //max(x[0],y[0],z[0]);
    for(int i=1; i<vec.size(); ++i){
        max1=((vec[i].x()>vec[i].y())?vec[i].x():vec[i].y() > vec[i].z())?
              (vec[i].x()>vec[i].y())?vec[i].x():vec[i].y()
               :vec[i].z(); //max(x[i], y[i], z[i])
        max=(max>max1)?max:max1;
    }
    return max;

}

Window::Window(QWidget *parent)
    : QMainWindow(parent){
     double PI =4*atan(1);
     LTETA= new QLabel("teta(rotate ox)", this);
     LTETA->move(10, 35);
     TETA = new QDoubleSpinBox(this);
     TETA -> setRange(-PI, PI);
     TETA -> setSingleStep(PI/60);
     TETA -> setValue(PI/4);
     TETA -> move(10, 70);
     connect(TETA, SIGNAL(valueChanged(double)), this, SLOT(redrawOnValueChange(double)));

     LPSI= new QLabel("psi (rotate oy)", this);
     LPSI->move(10, 105);
     PSI = new QDoubleSpinBox(this);
     PSI -> setRange(-PI, PI);
     PSI -> setSingleStep(PI/60);
     PSI -> setValue(PI/4);
     PSI -> move(10, 140);
     connect(PSI, SIGNAL(valueChanged(double)), this, SLOT(redrawOnValueChange(double)));

     LFI= new QLabel("fi (rotate oz)", this);
     LFI->move(10, 175);
     FI  = new QDoubleSpinBox(this);
     FI -> setRange(-PI, PI);
     FI -> setSingleStep(PI/60);
     FI -> setValue(0.0);
     FI -> move(10, 210);
     connect(FI, SIGNAL(valueChanged(double)), this, SLOT(redrawOnValueChange(double)));

     LSCALE= new QLabel("Scale", this);
     LSCALE->move(10, 245);
     SCALE = new QDoubleSpinBox(this);
     SCALE -> setRange(1, MAX_SCALE);
     SCALE -> setSingleStep(1);
     SCALE -> setValue(150.);
     SCALE -> move(10, 280);
     connect(SCALE, SIGNAL(valueChanged(double)), this, SLOT(redrawOnValueChange(double)));

     Box_Vis = new QCheckBox("Back lines", this);
     Box_Vis->move(5, 310);
     connect(Box_Vis, SIGNAL(clicked(bool)), this, SLOT(redrawOnValueChange(bool)));

     Box_izo = new QCheckBox("Izometric", this);
     Box_izo->move(5, 340);
     connect(Box_izo, SIGNAL(clicked(bool)), this, SLOT(redrawOnValueChange(bool)));

}

void Window::cocentrate(std::vector<QVector4D>& pnts){
    QVector4D centr(0, 0, 0, 0);
    for(auto pnt: pnts)
        centr +=pnt;
    centr/=pnts.size();
    for(auto& pnt: pnts)
        pnt-=centr-QVector4D(0, 0, 0, 1);
}

Window::~Window(){}

void Window::paintEvent(QPaintEvent* ){
 bool draw_invisible= Box_Vis->isChecked();
 bool draw_izo = Box_izo->isChecked();
 int x=1, y=1, z=4;
 //vertexes of polygon
 std::vector<QVector4D> Vertexes{
    QVector4D(-x, y, 0, 1),
    QVector4D(-x,-y, 0, 1),
    QVector4D( x, y, 0, 1),
    QVector4D( x,-y, 0, 1),
    QVector4D(-x, 0, z, 1),
    QVector4D( x, 0, z, 1)};
 cocentrate(Vertexes); //move center of figure to (0,0,0)

 double fi   = FI   -> value() /*oz o*/,
        psi  = PSI  -> value() /*oy ^*/,
        teta = TETA -> value() /*ox ->*/,
        scale= SCALE-> value() / (max_coordinate(Vertexes)*MAX_SCALE) * ((height()<width())?height():width())/2;

 //polygon edges
 std::vector<std::vector<char>> polygs {
     std::vector<char>{1, 0, 2, 3},
     std::vector<char>{0, 1, 4},
     std::vector<char>{1, 3, 5, 4},
     std::vector<char>{0, 4, 5, 2},
     std::vector<char>{5, 3, 2}
 };
 double zv=15.0, zs=10.0;
 double izo_scale=2.8;
 //Transformation Matrixes
 QMatrix4x4
   rotate_oz(cos(fi), -sin(fi), 0, 0,
             sin(fi),  cos(fi), 0, 0,
                   0,        0, 1, 0,
                   0,        0, 0 ,1),
   rotate_oy(cos(psi), 0,-sin(psi), 0,
                    0, 1,        0, 0,
             sin(psi), 0, cos(psi), 0,
                    0, 0,        0, 1),
   rotate_ox(1,         0,          0, 0,
             0, cos(teta), -sin(teta), 0,
             0, sin(teta),  cos(teta), 0,
             0,         0,          0, 1),
   move_to_center_window(        1,          0, 0, 0,
                                 0,          1, 0, 0,
                                 0,          0, 1, 0,
                         width()/2, height()/2, 0, 1),
   increase(scale, 0, 0, 0,
                0, scale, 0, 0,
                0, 0, scale, 0,
                0, 0,     0, 1),
     izometric((zv-zs), 0, 0, 0,
               0, (zv-zs), 0, 0,
               0, 0, 1, -1,
               0, 0, 0, zv),
     izometric_scale(izo_scale, 0, 0, 0,
                     0, izo_scale, 0, 0,
                     0, 0, izo_scale, 0,
                     0, 0, 0, 1);
 //Tranform all vertexes at (0,0,0) and move to center of window
  for(auto& vert: Vertexes){
     vert = vert * rotate_ox * rotate_oy *rotate_oz;
     if(draw_izo){
        vert = vert * izometric;
         normal(vert);
         vert = vert * izometric_scale;
     }
     vert = vert * increase * move_to_center_window;
  }
 //prepare QPainter
 QPainter pntr(this);
 QPen vis_pen(Qt::blue, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
 QPen invis_pen(Qt::blue, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
 pntr.setPen(vis_pen);
  /*//testing sector
  QVector3D Ver=QVector3D::normal(QVector3D(0, 1, 0), QVector3D(1, 0, 0));
  std::cout<<'['
      <<Ver.x()<<' '
      <<Ver.y()<<' '
      <<Ver.z()<<']'<<' '<<scalar(QVector3D(-1, 1, 0), QVector3D(1, 0, 0))<<std::endl;
  //======================================*/
 //std::cout<<((draw_invisible)?"visible":"invisible")<<std::endl;

  //drawing figure
  for(auto polyg :polygs){
      if(visible_at_oz(Vertexes[polyg[1]]-Vertexes[polyg[0]],
                         Vertexes[polyg[2]]-Vertexes[polyg[0]])){
          pntr.setPen(vis_pen);
      }else {
          pntr.setPen(((draw_invisible)?invis_pen:vis_pen));
      }
      if(visible_at_oz(Vertexes[polyg[1]]-Vertexes[polyg[0]],
                       Vertexes[polyg[2]]-Vertexes[polyg[0]]) || draw_invisible)
      for(int i=0; i<polyg.size(); ++i){
         pntr.drawLine(
            QPointF(Vertexes[polyg[i]].x(),
                    Vertexes[polyg[i]].y()),
            QPointF(Vertexes[polyg[(i+1)%polyg.size()]].x(),
                    Vertexes[polyg[(i+1)%polyg.size()]].y())
             );
       }
  }



}
