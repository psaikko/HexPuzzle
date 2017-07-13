#include <QPainter>
#include <QPen>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
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

  QHBoxLayout *btnbox = new QHBoxLayout(this);

  vbox->setSpacing(1);
  btnbox->setSpacing(1);

  btnbox->addWidget(next);
  btnbox->addWidget(prev);

  vbox->addWidget(grid);
  vbox->addLayout(btnbox);
}
