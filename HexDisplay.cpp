#include <QPainter>
#include <QPen>
#include "HexDisplay.h"
#include <bits/stdc++.h>

using namespace std;

#define GRID_SIZE 5
#define W 500
#define H 500
#define SQRT3 1.732

HexDisplay::HexDisplay(vector<uint64_t> &states, QWidget *parent)
    : QWidget(parent), stateIndex(0), gameStates(states)
{ 
  resize(W, H);
}

void HexDisplay::paintEvent(QPaintEvent *e) {
    
  Q_UNUSED(e);  

  QPainter painter(this);
  
  drawState(painter, gameStates[stateIndex]);
}

void HexDisplay::prev() {
  if (stateIndex > 0) 
    stateIndex--;
  update();
}

void HexDisplay::next() {
  if (stateIndex < gameStates.size() - 1) 
    stateIndex++;
  update();
}

void HexDisplay::drawState(QPainter &painter, uint64_t state) {

  painter.setRenderHint(QPainter::Antialiasing, true);
  QPen pen(QColor("#7f7f7f"));
  pen.setWidth(0);
  painter.setPen(pen);

  uint64_t i = 1;

  float scale = min(width() / double(((2 * GRID_SIZE - 1) * SQRT3)), height() / double((2 * GRID_SIZE + (GRID_SIZE - 1))));

  for (int y = 1; y < 2*GRID_SIZE; ++y) {
    for (int x = max(1, y - (GRID_SIZE - 1)); 
       x < min(2*GRID_SIZE, y + GRID_SIZE);
       x++) 
    {
      painter.resetTransform();

      QPointF center(SQRT3 * x - SQRT3 /2*(y-4), 1.5 * y - 0.5);

      painter.scale(scale, scale);
      painter.translate(center);

      if (state & i)
        painter.setBrush(QBrush("#000000"));
      else 
        painter.setBrush(QBrush("#FFFFFF"));

      drawHex(painter);

      i <<= 1;
    }
  }
}

void HexDisplay::drawHex(QPainter &painter) {

  static const QPointF hexPoints[6] {
    QPointF(0, 1),
    QPointF(SQRT3 / 2, 0.5),
    QPointF(SQRT3 / 2, -0.5),
    QPointF(0, -1),
    QPointF(-SQRT3 / 2, -0.5),
    QPointF(-SQRT3 / 2, 0.5)
  };

  painter.drawConvexPolygon(hexPoints, 6);
}