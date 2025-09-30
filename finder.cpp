#include "finder.h"
#include <QCoreApplication>

Finder::Finder(const std::vector<Field*> &line, const std::vector<Chain*> &chains)
  : source_line_{line},
    chains_{chains} {
  size_t size = line.size();
  filled_line_.resize(size, 1);
  empty_line_.resize(size, -1);
}

int Finder::GetResult() {
  int res = 0;
  unsigned int size = source_line_.size();
  for (unsigned int i = 0; i < size; ++i) {
    if (empty_line_[i] == -1 && filled_line_[i] == 1) {
      return -1;
    }
  }
  for (unsigned int i = 0; i < size; ++i) {
    if (empty_line_[i] == -1 && source_line_[i]->state_ != -1) {
      source_line_[i]->state_ = -1;
      source_line_[i]->setText("x");
      res = 1;
      source_line_[i]->checked_ = false;
    } else if (filled_line_[i] == 1 && source_line_[i]->state_ != 1) {
      source_line_[i]->state_ = 1;
      source_line_[i]->setText("█");
      res = 1;
      source_line_[i]->checked_ = false;
    } else {
      source_line_[i]->checked_ = true;
    }
  }
  QCoreApplication::processEvents();
  return res;
}

int Finder::FindNextChain(bool all) {
  int res = CheckChain(curent_pos, curent_chain);
  if (res < 0) {
    if (curent_chain == 0) {
      return -1;
    }
    --curent_chain;
    curent_pos = chains_[curent_chain]->pos_ + 1;
    return 0;
  }
  chains_[curent_chain]->pos_ = res;
  if (curent_chain < size - 1) {
    curent_pos = res + 1 + chains_[curent_chain]->len_;
    ++curent_chain;
    return 0;
  }
  curent_pos = res + 1;
  if (!all) {
    return 1;
  }
  SaveChains();
  for (auto chain : chains_) {
    if (chain->tmp_max_pos_ < chain->pos_) {
      chain->tmp_max_pos_ = chain->pos_;
    }
    if (chain->tmp_min_pos_ > chain->pos_) {
      chain->tmp_min_pos_ = chain->pos_;
    }
  }
  return 0;
}

void Finder::FindResult(bool all) {
  curent_pos = 0;
  curent_chain = 0;
  size = chains_.size();
//  std::vector<unsigned int> foun(chains_.size());
  if (all) {
    for (auto chain : chains_) {
      chain->tmp_max_pos_ = chain->min_pos_;
      chain->tmp_min_pos_ = chain->max_pos_;
    }
  }
  while (FindNextChain(all) == 0);
  if (all) {
    for (auto chain : chains_) {
      chain->min_pos_ = chain->tmp_min_pos_;
      chain->max_pos_ = chain->tmp_max_pos_;
      if (chain->max_pos_ == chain->min_pos_) {
        chain->setStyleSheet("background-color: #AAFFAA; color: black;");
      }
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
    if (source_line_[i]->state_ >= -1) {
      empty_line_[i] = 0;
    }
    if (source_line_[i]->state_ <= 0) {
      filled_line_[i] = 0;
    }
  }
  if (size == 0) return 0;
  unsigned int min_len = chains_[0]->len_;
  for (unsigned int i = 1; i < size; ++i) {
    min_len += chains_[i]->len_;
    min_len += 1;//закладка на цветной вариант.
  }
  diff = source_line_.size() - min_len;
  for (unsigned int i = 0; i < size; ++i) {
    for (int j = 0; j < int(chains_[i]->len_) - diff; ++j) {
      res = 1;
      filled_line_[curent_pos + diff + j] = 1;
    };
    chains_[i]->min_pos_= curent_pos;
    chains_[i]->max_pos_= curent_pos+ diff;
    if (diff == 0) {
      chains_[i]->setStyleSheet("background-color: #AAFFAA; color: black;");
    }
    curent_pos += chains_[i]->len_;
    curent_pos += 1;
  }
  return res;
}

bool Finder::CheckLastPos(unsigned int &pos) {
  unsigned int i = source_line_.size() -1;
  unsigned int chain_num = chains_.size() - 1;
  unsigned int len = chains_[chain_num]->len_;
  while (i >= pos + len) {
    if (source_line_[i]->state_ > 0) {
      break;
    }
    i--;
  }
  if (i > chains_[chain_num]->max_pos_ + chains_[chain_num]->len_) {
    return false;
  }
  //не должно быть закрашенных клеток дальше последней цепочки.
  while (pos + len <= i) {
    if (source_line_[pos]->state_ <= 0) {
      ++pos;
    } else {
      return false;
    }
  }
  return true;
}

bool Finder::CheckLastFill(unsigned int &pos, unsigned int chain_num) {
  unsigned int len = chains_[chain_num]->len_;
  while (pos + len < size && source_line_[pos + len]->state_ == 1) {
    if (source_line_[pos]->state_ == 1) {
      return false;
    }
    if (pos + len >= chains_[chain_num]->max_pos_ + chains_[chain_num]->len_) {
      return false;
    }
    ++pos;
  }
  return true;
}

int Finder::CheckChain(unsigned int pos, unsigned int chain_num) {
  unsigned int i = 0;
  unsigned int len = chains_[chain_num]->len_;
  if ((pos > chains_[chain_num]->max_pos_) || (pos > 0 && source_line_[pos - 1]->state_ == 1)) {
    return -1;
  }
  while (pos < chains_[chain_num]->min_pos_) {
    if (source_line_[pos]->state_ == 1 ) {
      return -1;
    }
    ++pos;
  }
  // если цеопчка последняя, найти самую дальнюю закрашенную ячейку
  if (chain_num == chains_.size() - 1 && !CheckLastPos(pos)) {
    return -1;
  }
  while (i < len) {
    if (pos + len > chains_[chain_num]->max_pos_ + chains_[chain_num]->len_) {
      return -1;
    }
    //клетка не определена или нужного цвета
    if (source_line_[pos + i]->state_ == 0 || source_line_[pos + i]->state_ == 1) {
      ++i;
      continue;
    }
    //попытка пропустить пустую клетку
    if (source_line_[pos + i]->state_ == -1) {
      while (i) {
        if (source_line_[pos]->state_ == 0) {
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
  for (Chain* chain : chains_) {
    for (unsigned int i = 0; i < chain->len_; ++i) {
      current_filled_[chain->pos_ + i] = 1;
      empty_line_[chain->pos_ + i] = 0;
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
