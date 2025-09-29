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

int Finder::FindResult(bool all) {
  int res = 0;
  unsigned int curent_pos = 0;
  unsigned int curent_chain = 0;
  unsigned int size = chains_.size();
  std::vector<unsigned int> foun(chains_.size());
  if (all) {
    for (auto chain : chains_) {
      chain->find_max_pos_ = chain->min_pos_;
      chain->find_min_pos_ = chain->max_pos_;
    }
  }
  while (true) {
    res = CheckChain(curent_pos, curent_chain);
    if (res >= 0) {
      chains_[curent_chain]->position_ = res;
      if (curent_chain < size - 1) {
        curent_pos = res + 1 + chains_[curent_chain]->size_;
        ++curent_chain;
        continue;
      }
      curent_pos = res + 1;
      if (all) {
        SaveChains();
        for (auto chain : chains_) {
          if (chain->find_max_pos_ < chain->position_) {
            chain->find_max_pos_ = chain->position_;
          }
          if (chain->find_min_pos_ > chain->position_) {
            chain->find_min_pos_ = chain->position_;
          }
        }
      } else {
        return 0;
      }
    } else {
      if (curent_chain > 0) {
        --curent_chain;
        curent_pos = chains_[curent_chain]->position_ + 1;
        continue;
      }
      break;
    }
  }
  if (all) {
    for (auto chain : chains_) {
      chain->min_pos_ = chain->find_min_pos_;
      chain->max_pos_ = chain->find_max_pos_;
      if (chain->max_pos_ == chain->min_pos_) {
        chain->setStyleSheet("background-color: #AAFFAA; color: black;");
      }
    }
  }
  return 0;
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
  unsigned int min_len = chains_[0]->size_;
  for (unsigned int i = 1; i < size; ++i) {
    min_len += chains_[i]->size_;
    min_len += 1;//закладка на цветной вариант.
  }
  diff = source_line_.size() - min_len;
  for (unsigned int i = 0; i < size; ++i) {
    for (int j = 0; j < int(chains_[i]->size_) - diff; ++j) {
      res = 1;
      filled_line_[curent_pos + diff + j] = 1;
    };
    chains_[i]->min_pos_= curent_pos;
    chains_[i]->max_pos_= curent_pos+ diff;
    if (diff == 0) {
      chains_[i]->setStyleSheet("background-color: #AAFFAA; color: black;");
    }
    curent_pos += chains_[i]->size_;
    curent_pos += 1;
  }
  return res;
}

int Finder::CheckChain(unsigned int pos, unsigned int chain_num) {
  unsigned int i = 0;
  unsigned int size = source_line_.size();
  unsigned int len = chains_[chain_num]->size_;
  if ((pos > chains_[chain_num]->max_pos_) || (pos > 0 && source_line_[pos - 1]->state_ == 1)) {
    return -1;
  }
  while (pos < chains_[chain_num]->min_pos_) {
    if (source_line_[pos]->state_ == 1) {
      return -1;
    }
    ++pos;
  }
  // если цеопчка последняя, найти самую дальнюю закрашенную ячейку
  if (chain_num == chains_.size() - 1) {
    i = size - 1;
    while (i >= pos + len) {
      if (source_line_[i]->state_ > 0) {
        break;
      }
      i--;
    }
    if (i > chains_[chain_num]->max_pos_ + chains_[chain_num]->size_) {
      return -1;
    }
    //не должно быть закрашенных клеток дальше последней цепочки.
    while (pos + len <= i) {
      if (source_line_[pos]->state_ <= 0) {
        ++pos;
      } else {
        return -1;
      }
    }
  }
  i = 0;
  while (i < len) {
    if (pos + len > chains_[chain_num]->max_pos_ + chains_[chain_num]->size_) {
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
  while (pos + len < size && source_line_[pos + len]->state_ == 1) {
    if (source_line_[pos]->state_ == 1) {
      return -1;
    }
    if (pos + len >= chains_[chain_num]->max_pos_ + chains_[chain_num]->size_) {
      return -1;
    }
    ++pos;
  }
  return pos;
}

int Finder::SaveChains() {
  size_t size = source_line_.size();
  std::vector<int> current_filled_;
  current_filled_.resize(size, 0);
  for (Chain* chain : chains_) {
    for (unsigned int i = 0; i < chain->size_; ++i) {
      current_filled_[chain->position_ + i] = 1;
      empty_line_[chain->position_ + i] = 0;
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
