#ifndef CHAIN_H
#define CHAIN_H

#include <QPushButton>

class Chain : public QPushButton {
  Q_OBJECT
 public:
  Chain(int len, QWidget *parent = nullptr);
  ~Chain();
  unsigned int len_;
  unsigned int pos_{0};
  unsigned int min_pos_{0};
  unsigned int max_pos_{0};
  unsigned int tmp_min_pos_{0};
  unsigned int tmp_max_pos_{0};

  //для дальнейшей оптимизации:
  //unsigned int color_;
  
 private:
 
};

#endif // CHAIN_H