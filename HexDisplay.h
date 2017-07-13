#pragma once 

#include <QWidget>
#include <string>

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

  	void drawState(QPainter &painter);
    void drawPiece(QPainter &painter, uint64_t piece, const std::string color);
    void drawHex(QPainter &painter);
};