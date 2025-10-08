#ifndef CROSSWORD_H
#define CROSSWORD_H

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
 public slots:
  void Load();
  void Run();
 private:
  void ResizeFields(unsigned int h_size, unsigned int v_size);
  unsigned int LoadTopChains(std::ifstream &stream);
  unsigned int LoadLeftChains(std::ifstream &stream);
  int VCalc(unsigned int row, unsigned int optimize = 0);
  int HCalc(unsigned int col, unsigned int optimize= 0);
  bool CheckLine(unsigned int i, bool orientation);
  bool CheckChains(const std::vector<std::vector<Chain*>> &v_chains);
  unsigned int h_size_;
  unsigned int v_size_;
  std::vector<Field*> fields_;
  std::vector<std::vector<Chain*>> top_chains_;
  std::vector<std::vector<Chain*>> left_chains_;
  QGridLayout *fields_layout_;
  QHBoxLayout *top_chains_layout_;
  QVBoxLayout *left_chains_layout_;
};
#endif // CROSSWORD_H
