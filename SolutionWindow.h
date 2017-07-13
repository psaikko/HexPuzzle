#pragma once 

#include <QWidget>

class SolutionWindow : public QWidget {
    
  public:
    SolutionWindow(std::vector<uint64_t> &gameStates, QWidget *parent = 0);

  
};