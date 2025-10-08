#ifndef FINDER_H
#define FINDER_H

#include <vector>

#include "chain_val.h"

class Finder {
 public:
  Finder(const std::vector<int> &line, const std::vector<ChainVal*> &chains);
  std::vector<int> GetResult();
  void FindResult(bool all = true);
  int FastFind();
  void Clear();
 private:
  int CheckChain(unsigned int pos, unsigned int len);
  int FindNextChain(bool all);
  bool CheckLastPos(unsigned int &pos);
  bool CheckLastFill(unsigned int &pos, unsigned int chain_num);
  int SaveChains();
  const std::vector<int> &source_line_;
  const std::vector<ChainVal*> &chains_;
  std::vector<int> filled_line_;
  std::vector<int> empty_line_;
  unsigned int curent_pos{0};
  unsigned int curent_chain{0};
  unsigned int size{0};
};
#endif // FINDER_H