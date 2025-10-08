#include "field.h"

Field::Field(QWidget *parent)
  : QPushButton(parent) {
  setFixedSize(15, 15);
}

Field::~Field() {
}

bool Field::SetVal(int val) {
    if (val == -1 && state_ != -1) {
      state_ = -1;
      setText("x");
      checked_ = false;
    } else if (val == 1 && state_ != 1) {
      state_ = 1;
      setText("█");
      checked_ = false;
    } else {
      checked_ = true;
    }
    return checked_;
}