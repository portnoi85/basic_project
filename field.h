#ifndef FIELD_H
#define FIELD_H

#include <QPushButton>

class Field : public QPushButton {
  Q_OBJECT
 public:
  Field(QWidget *parent = nullptr);
  ~Field();
  bool SetVal(int val);
  int state_{0};
  bool checked_{false};
  //для дальнейшей оптимизации:
  // unsigned int color_;

 private:
 
};

#endif // FIELD_H