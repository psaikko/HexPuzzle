#include <QPainter>
#include <QPen>
#include <QPushButton>
#include <QVBoxLayout>
#include "SolutionWindow.h"
#include "HexDisplay.h"
#include <bits/stdc++.h>

using namespace std;

#define GRID_SIZE 5
#define W 500
#define H 500
#define SQRT3 1.732

SolutionWindow::SolutionWindow(vector<uint64_t> &states, QWidget *parent)
    : QWidget(parent)
{ 
  HexDisplay * grid = new HexDisplay(states, this);

  QPushButton * next = new QPushButton("Next", this);

  QPushButton * prev = new QPushButton("Prev", this);

  connect(next, &QPushButton::clicked, grid, &HexDisplay::next);

  connect(prev, &QPushButton::clicked, grid, &HexDisplay::prev);

  QVBoxLayout *vbox = new QVBoxLayout(this);

  vbox->setSpacing(1);

  vbox->addWidget(grid);
  vbox->addWidget(next);
  vbox->addWidget(prev);
}
