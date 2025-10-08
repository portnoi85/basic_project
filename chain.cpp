#include "chain.h"

Chain::Chain(int len, QWidget *parent) 
  : QPushButton (QString::number(len), parent) {
  setFixedSize(15, 15);
  setStyleSheet("background-color: #AAAAAA; color: black;");
  val_.len = len;
}

Chain::~Chain() {
}