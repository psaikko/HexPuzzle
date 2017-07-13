#pragma once 

#include <QWidget>

class HexDisplay : public QWidget {
    
  public:
    HexDisplay(std::vector<uint64_t> &gameStates, QWidget *parent = 0);

    void prev();
    void next();

  protected:
    void paintEvent(QPaintEvent *e);

  private:
  	unsigned stateIndex;
  	std::vector<uint64_t> gameStates;

  	void drawState(QPainter &painter, uint64_t state);
    void drawHex(QPainter &painter);
};