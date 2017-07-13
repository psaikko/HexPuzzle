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
  
  drawState(painter);
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

void HexDisplay::drawState(QPainter &painter) {

  // colors from http://colorbrewer2.org
  static const string colors[10] {
    "#8dd3c7",
    "#ffffb3",
    "#bebada",
    "#fb8072",
    "#80b1d3",
    "#fdb462",
    "#b3de69",
    "#fccde5",
    "#d9d9d9",
    "#bc80bd"
  };

  painter.setRenderHint(QPainter::Antialiasing, true);
  QPen pen(QColor("#7f7f7f"));
  pen.setWidth(0);
  painter.setPen(pen);

  // draw "empty" filled grid piece with white for background
  drawPiece(painter, 0x1FFFFFFFFFFFFFFF, "#FFFFFF");

  // draw each piece placed up to stateIndex
  for (unsigned i = 0; i < stateIndex; ++i) {
    // xor with next state to get only the placed piece
    uint64_t piece = gameStates[i] ^ gameStates[i + 1];
    drawPiece(painter, piece, colors[i]);
  }
}

void HexDisplay::drawPiece(QPainter &painter, uint64_t piece, const string color) {

  // scale to fit the widget's size
  float scale = min(width() / double(((2 * GRID_SIZE - 1) * SQRT3)), height() / double((2 * GRID_SIZE + (GRID_SIZE - 1))));

  uint64_t i = 1;
  // loop over hex grid indices (x,y) and bit indices (i)
  for (int y = 1; y < 2*GRID_SIZE; ++y) {
    for (int x = max(1, y - (GRID_SIZE - 1)); 
       x < min(2*GRID_SIZE, y + GRID_SIZE);
       x++, i <<= 1) 
    {
      // if piece is filled
      if (piece & i) {
        painter.resetTransform();

        // compute center of hex
        QPointF center(SQRT3 * x - SQRT3 /2*(y-4), 1.5 * y - 0.5);
        // set transformation
        painter.scale(scale, scale);
        painter.translate(center);
        // set color
        painter.setBrush(QBrush(color.c_str()));

        drawHex(painter);
      }
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