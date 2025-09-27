#ifndef CROSSWORD_H
#define CROSSWORD_H

#include <QWidget>
#include <QPushButton>
#include <QLayout>

#include "chain.h"
#include "finder.h"
#include "field.h"

class Crossword : public QWidget
{
  Q_OBJECT
 public:
  Crossword(QWidget *parent = nullptr);
  ~Crossword();
  void SetSize(unsigned int h_size, unsigned int v_size);
  void Clear();
  void CreateField();
  void VCalc(unsigned int row);
  void HCalc(unsigned int col);
 public slots:
  void Load();
  void Run();
 private:
  std::vector<Field*> line_;
  unsigned int h_size_;
  unsigned int v_size_;
  std::vector<std::vector<Chain*>> h_chains_;
  std::vector<std::vector<Chain*>> v_chains_;
  QGridLayout *grid_;
  QGridLayout *fields_;
  QVBoxLayout *left_values_;
  QHBoxLayout *top_values_;
};
#endif // CROSSWORD_H
