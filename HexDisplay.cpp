#include <QPainter>
#include <QTimer>
#include "HexDisplay.h"
#include <bits/stdc++.h>

using namespace std;

HexDisplay::HexDisplay(vector<uint64_t> &states, QWidget *parent)
    : QWidget(parent), stateIndex(0), gameStates(states)
{ 
  QTimer *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &HexDisplay::tick);
  timer->start(16);
}

void HexDisplay::paintEvent(QPaintEvent *e) {
    
  Q_UNUSED(e);  

  QPainter painter(this);
  
  drawState(painter, gameStates[stateIndex]);
}

void HexDisplay::tick() {
  update();
  if (stateIndex < gameStates.size() - 1) stateIndex++;
}

void HexDisplay::drawState(QPainter &painter, uint64_t state) {
  int GRID_SIZE = 5;
  float scale = 30;

  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setPen(QColor("#7f7f7f"));

  uint64_t i = 1;

  for (int y = 1; y < 2*GRID_SIZE; ++y) {
    for (int x = max(1, y - (GRID_SIZE - 1)); 
       x < min(2*GRID_SIZE, y + GRID_SIZE);
       x++) 
    {
      QPointF center(1.732 * x - 1.732/2*(y-4), 1.5 * y);
      center *= scale;

      if (state & i)
        painter.setBrush(QBrush("#000000"));
      else 
        painter.setBrush(QBrush("#FFFFFF"));

      drawHex(painter, center, scale);

      i <<= 1;
    }
  }
}

void HexDisplay::drawHex(QPainter &painter, QPointF &center, float scale) {
  QPolygonF hex;

  hex << QPointF(0, 1)*scale + center 
      << QPointF(1.732 / 2, 0.5)*scale + center 
      << QPointF(1.732 / 2, -0.5)*scale + center 
      << QPointF(0, -1)*scale + center 
      << QPointF(-1.732 / 2, -0.5)*scale + center 
      << QPointF(-1.732 / 2, 0.5)*scale + center;

  painter.drawConvexPolygon(hex);
}