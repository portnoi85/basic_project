#ifndef CHAIN_H
#define CHAIN_H

#include <QPushButton>

class Chain : public QPushButton {
  Q_OBJECT
 public:
  Chain(int len, QWidget *parent = nullptr);
  ~Chain();
  unsigned int size_;
  unsigned int position_;
  //для дальнейшей оптимизации:
  unsigned int min_pos_;
  unsigned int max_pos_;
  //unsigned int color_;
  
 private:
 
};

#endif // CHAIN_H