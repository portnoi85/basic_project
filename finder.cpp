#include "finder.h"
#include <QMessageBox>

Finder::Finder(const std::vector<Field*> &line, const std::vector<Chain*> &chains)
  : source_line_{line},
    chains_{chains} {
  size_t size = line.size();
  result_line_.resize(size, 0);
  filled_line_.resize(size, 1);
  empty_line_.resize(size, -1);
}

int Finder::GetResult() {
  unsigned int size = source_line_.size();
  for (unsigned int i = 0; i < size; ++i) {
    //проверить случай, если оба условия сработали?..
    if (empty_line_[i] == -1) {
      source_line_[i]->state_ = -1;
      source_line_[i]->setText("x");
    } else if (filled_line_[i] == 1) {
      source_line_[i]->state_ = 1;
      source_line_[i]->setText("█");
    } else {
      source_line_[i]->state_ = 0;
      source_line_[i]->setText("");
    }
  }
  return 0;
}

int Finder::FindResult() {
  int res = 0;
  unsigned int curent_pos = 0;
  unsigned int curent_chain = 0;
  unsigned int size = chains_.size();
  std::vector<unsigned int> foun(chains_.size());
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
      SaveChains();
    } else {
      if (curent_chain > 0) {
        --curent_chain;
        curent_pos = chains_[curent_chain]->position_ + 1;
        continue;
      }
      break;
    }
  }
  return 0;
}

int Finder::CheckChain(unsigned int pos, unsigned int chain_num) {
  unsigned int i = 0;
  unsigned int size = source_line_.size();
  unsigned int len = chains_[chain_num]->size_;
  if ((pos >= size) || (pos > 0 && source_line_[pos - 1]->state_ == 1)) {
    return -1;
  }
  // если цеопчка последняя, найти самую дальнюю закрашенную ячейку
  if (chain_num == chains_.size() - 1) {
    i = size - 1;
    while (i >= pos + len) {
      if (source_line_[i]->state_ == 1) {
        break;
      }
      i--;
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
    if (pos + len > size) {
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
  while (pos + i < size && source_line_[pos + i]->state_ == 1) {
    if (source_line_[pos]->state_ == 1) {
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
  QString str;
  for (Chain* chain : chains_) {
    for (unsigned int i = 0; i < chain->size_; ++i) {
      current_filled_[chain->position_ + i] = 1;
      empty_line_[chain->position_ + i] = 0;
    }
  }
  for (unsigned int i = 0; i < size; ++i) {
    filled_line_[i] = filled_line_[i] && current_filled_[i];
    str +=" "+ QString::number(current_filled_[i]);
  }
  return 0;
}

void Finder::Clear() {
  //chains_.clear();
  //source_line_.clear();
  result_line_.clear();
  filled_line_.clear();
  empty_line_.clear();
}
