#include "finder.h"
#include "field.h"

Finder::Finder(const std::vector<int> &line, const std::vector<ChainVal*> &chains)
  : source_line_{line},
    chains_{chains} {
  size_t size = line.size();
  filled_line_.resize(size, 1);
  empty_line_.resize(size, -1);
}

std::vector<int> Finder::GetResult() {
  std::vector<int> res;
  unsigned int size = source_line_.size();
  for (unsigned int i = 0; i < size; ++i) {
    if (empty_line_[i] == -1 && filled_line_[i] == 1) {
      return res;
    }
  }
  res.reserve(size);
  for (unsigned int i = 0; i < size; ++i) {
    if (empty_line_[i] == -1 && source_line_[i] != -1) {
      res.push_back(-1);
    } else if (filled_line_[i] == 1 && source_line_[i] != 1) {
      res.push_back(1);
    } else {
      res.push_back(0);
    }
  }
  return res;
}

int Finder::FindNextChain(bool all) {
  int res = CheckChain(curent_pos, curent_chain);
  if (res < 0) {
    if (curent_chain == 0) {
      return -1;
    }
    --curent_chain;
    curent_pos = chains_[curent_chain]->pos + 1;
    return 0;
  }
  chains_[curent_chain]->pos = res;
  if (curent_chain < size - 1) {
    curent_pos = res + 1 + chains_[curent_chain]->len;
    ++curent_chain;
    return 0;
  }
  curent_pos = res + 1;
  if (all == 0) {
    return 1;
  }
  SaveChains();
  for (auto chain : chains_) {
    if (chain->tmp_max_pos < chain->pos) {
      chain->tmp_max_pos = chain->pos;
    }
    if (chain->tmp_min_pos > chain->pos) {
      chain->tmp_min_pos = chain->pos;
    }
  }
  return 0;
}

void Finder::FindResult(bool all) {
  curent_pos = 0;
  curent_chain = 0;
  size = chains_.size();
  if (all) {
    for (auto chain : chains_) {
      chain->tmp_max_pos = chain->min_pos;
      chain->tmp_min_pos = chain->max_pos;
    }
  }
  while (FindNextChain(all) == 0);
  if (all) {
    for (auto chain : chains_) {
      chain->min_pos = chain->tmp_min_pos;
      chain->max_pos = chain->tmp_max_pos;
    }
  }
  return;
}

int Finder::FastFind() {
  int res = 0;
  unsigned int curent_pos = 0;
  unsigned int size = chains_.size();
  int diff = 0;
  for (unsigned int i = 0; i< source_line_.size(); ++i) {
    //подумать над значениями по умолчанию 
    if (source_line_[i] >= -1) {
      empty_line_[i] = 0;
    }
    if (source_line_[i] <= 0) {
      filled_line_[i] = 0;
    }
  }
  if (size == 0) return 0;
  unsigned int min_len = chains_[0]->len;
  for (unsigned int i = 1; i < size; ++i) {
    min_len += chains_[i]->len;
    min_len += 1;//закладка на цветной вариант.
  }
  diff = source_line_.size() - min_len;
  for (unsigned int i = 0; i < size; ++i) {
    for (int j = 0; j < int(chains_[i]->len) - diff; ++j) {
      res = 1;
      filled_line_[curent_pos + diff + j] = 1;
    };
    chains_[i]->min_pos= curent_pos;
    chains_[i]->max_pos= curent_pos+ diff;
    curent_pos += chains_[i]->len;
    curent_pos += 1;
  }
  return res;
}

bool Finder::CheckLastPos(unsigned int &pos) {
  unsigned int i = source_line_.size() -1;
  unsigned int chain_num = chains_.size() - 1;
  unsigned int len = chains_[chain_num]->len;
  while (i >= pos + len) {
    if (source_line_[i] > 0) {
      break;
    }
    i--;
  }
  if (i > chains_[chain_num]->max_pos + chains_[chain_num]->len) {
    return false;
  }
  //не должно быть закрашенных клеток дальше последней цепочки.
  while (pos + len <= i) {
    if (source_line_[pos] <= 0) {
      ++pos;
    } else {
      return false;
    }
  }
  return true;
}

bool Finder::CheckLastFill(unsigned int &pos, unsigned int chain_num) {
  unsigned int len = chains_[chain_num]->len;
  while (pos + len < size && source_line_[pos + len] == 1) {
    if (source_line_[pos] == 1) {
      return false;
    }
    if (pos + len >= chains_[chain_num]->max_pos + chains_[chain_num]->len) {
      return false;
    }
    ++pos;
  }
  return true;
}

int Finder::CheckChain(unsigned int pos, unsigned int chain_num) {
  unsigned int i = 0;
  unsigned int len = chains_[chain_num]->len;
  if ((pos > chains_[chain_num]->max_pos) || (pos > 0 && source_line_[pos - 1] == 1)) {
    return -1;
  }
  while (pos < chains_[chain_num]->min_pos) {
    if (source_line_[pos] == 1 ) {
      return -1;
    }
    ++pos;
  }
  // если цеопчка последняя, найти самую дальнюю закрашенную ячейку
  if (chain_num == chains_.size() - 1 && !CheckLastPos(pos)) {
    return -1;
  }
  while (i < len) {
    if (pos + len > chains_[chain_num]->max_pos + chains_[chain_num]->len) {
      return -1;
    }
    //клетка не определена или нужного цвета
    if (source_line_[pos + i] == 0 || source_line_[pos + i] == 1) {
      ++i;
      continue;
    }
    //попытка пропустить пустую клетку
    if (source_line_[pos + i] == -1) {
      while (i) {
        if (source_line_[pos] == 0) {
          ++pos;
          --i;
        } else {
          return -1;
        }
      }
      ++pos;
      continue;
    }
    return -1;
  }
  if (CheckLastFill(pos, chain_num) == false) {
    return -1;
  }
  return pos;
}

int Finder::SaveChains() {
  size_t size = source_line_.size();
  std::vector<int> current_filled_;
  current_filled_.resize(size, 0);
  for (ChainVal* chain : chains_) {
    for (unsigned int i = 0; i < chain->len; ++i) {
      current_filled_[chain->pos + i] = 1;
      empty_line_[chain->pos + i] = 0;
    }
  }
  for (unsigned int i = 0; i < size; ++i) {
    filled_line_[i] = filled_line_[i] && current_filled_[i];
  }
  return 0;
}

void Finder::Clear() {
  //chains_.clear();
  //source_line_.clear();
  filled_line_.clear();
  empty_line_.clear();
}
