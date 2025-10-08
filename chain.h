#ifndef CHAIN_H
#define CHAIN_H

#include <QPushButton>

#include <chain_val.h>

class Chain : public QPushButton {
  Q_OBJECT
 public:
  Chain(int len, QWidget *parent = nullptr);
  ~Chain();
  ChainVal val_;
  
 private:
 
};

#endif // CHAIN_H