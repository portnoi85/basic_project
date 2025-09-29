#ifndef FINDER_H
#define FINDER_H

#include <vector>
#include <QMessageBox>
#include "chain.h"
#include "field.h"

class Finder {
 public:
  Finder(const std::vector<Field*> &line, const std::vector<Chain*> &chains);
  int GetResult();
  int FindResult(bool all = true);
  int FastFind();
  void Clear();
 private:
  const std::vector<Field*> &source_line_;
  const std::vector<Chain*> &chains_;
  std::vector<int> filled_line_;
  std::vector<int> empty_line_;
  int CheckChain(unsigned int pos, unsigned int len);
  int SaveChains();
};
#endif // FINDER_H