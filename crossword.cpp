#include "crossword.h"
#include <fstream>
#include <QFileDialog>

Crossword::Crossword(QWidget *parent)
	: QWidget(parent) {
  QGridLayout *grid_ = new QGridLayout();
  fields_layout_ = new QGridLayout();
  left_chains_layout_ = new QVBoxLayout();
  top_chains_layout_ = new QHBoxLayout();
  QHBoxLayout *lower_layout = new QHBoxLayout();
  QPushButton *b_load = new QPushButton("Загрузить");
  QPushButton *b_run = new QPushButton("Решить");
  grid_->setSpacing(1);
  fields_layout_->setSpacing(1);
  left_chains_layout_->setSpacing(1);
  top_chains_layout_->setSpacing(1);
  lower_layout->addWidget(b_load);
  lower_layout->addWidget(b_run);
  grid_->addLayout(lower_layout, 2, 1);
  grid_->addLayout(top_chains_layout_, 0, 1);
  grid_->addLayout(left_chains_layout_, 1, 0);
  grid_->addLayout(fields_layout_, 1, 1);
  setLayout(grid_);
  connect(b_load, SIGNAL(clicked(bool)), SLOT(Load()));
  connect(b_run, SIGNAL(clicked(bool)), SLOT(Run()));
}

Crossword::~Crossword() {
}

void Crossword::ResizeFields(unsigned int h_size, unsigned int v_size) {
  h_size_ = h_size;
  v_size_ = v_size;
  for (Field *field : fields_) {
	  delete field;
  }
  fields_.clear();
  fields_.reserve(h_size_ * v_size_);
  for (unsigned int i = 0; i < v_size_; ++i) {
    for (unsigned int j = 0; j < h_size_; ++j) {
      Field *field = new Field();
      fields_layout_->addWidget(field, i, j);
      fields_.push_back(field);
	  }
  }
}

int  Crossword::HCalc(unsigned int i, unsigned int optimize) {
  std::vector<Field*> line;
  for (unsigned int j = 0; j < h_size_; ++j) {
    line.push_back(fields_[i * h_size_ + j]);
  }
  if (CheckLine(line)) {
    return 0;
  }
  Finder finder(line, left_chains_[i]);
  switch (optimize) {
    case 5:
      finder.FastFind();
      break;
    default:
      finder.FindResult();
      break;
  }
  return finder.GetResult();
}

int Crossword::VCalc(unsigned int i, unsigned int optimize) {
  std::vector<Field*> line;
  for (unsigned int j = 0; j < v_size_; ++j) {
    line.push_back(fields_[i + j * h_size_]);
  }
  if (CheckLine(line)) {
    return 0;
  }
  Finder finder(line, top_chains_[i]);
  switch (optimize) {
    case 5:
      finder.FastFind();
      break;
    default:
      finder.FindResult();
      break;
  }
  return finder.GetResult();
}

void ClearLayout(QLayout * layout) {
  while (layout->count() > 0) {
    QLayoutItem *item = layout->takeAt(0);
     if (item->widget()) {
       QWidget * widget = item->widget();
    //   layout->removeWidget(widget);
       delete widget;      
    }
    if (item->layout()) {
      QLayout *layout1 = item->layout();
      ClearLayout(layout1);
    }
    delete item;
  }
}

void Crossword::Load() {
 QString fname = QFileDialog::getOpenFileName(nullptr, "Открыть кроссворд", "", "Кроссворды (*.jcs);;Все файлы (*)");
  if (fname == "") return;
  std::ifstream stream(fname.toStdString());
  unsigned int h_size, v_size;
  int color_size, rc, gc, bc;
  stream >> h_size >> v_size;
  ResizeFields(h_size, v_size);
  stream >> color_size >> rc >> gc >> bc;
  setVisible(false);
  if (LoadTopChains(stream) != LoadLeftChains(stream)) {
    QMessageBox::information(nullptr, "Ошибка загрузки", "не совпадает количество закрашенных\nячеек в строках и столбцах.");
    
  }
  setVisible(true);
}

unsigned int Crossword::LoadTopChains(std::ifstream &stream) {
  unsigned int size = 0;
  top_chains_.clear();
  ClearLayout(top_chains_layout_);
  for (unsigned int i = 0; i < h_size_; ++i) {
    std::vector<Chain*> chains;
    int chain_len = 0;
    QVBoxLayout *line = new QVBoxLayout();
    line->addStretch(1);
    stream >> chain_len;
    while (chain_len) {
      size += chain_len;
      Chain *chain = new Chain(chain_len);
      chain->max_pos_ = v_size_ - chain_len; 
      chains.push_back(chain);
      line->addWidget(chain);
      stream >> chain_len;
    }
    top_chains_.push_back(std::move(chains));
    top_chains_layout_->addLayout(line);
  }
  return size;
}

unsigned int Crossword::LoadLeftChains(std::ifstream &stream) {
  unsigned int size = 0;
  left_chains_.clear();
  ClearLayout(left_chains_layout_);
  for (unsigned int i = 0; i < v_size_; ++i) {
    std::vector<Chain*> chains;
    int chain_len = 0;
    QHBoxLayout *line = new QHBoxLayout();
    line->addStretch(1);
    stream >> chain_len;
    while (chain_len) {
      size += chain_len;
      Chain *chain = new Chain(chain_len);
      chain->max_pos_ = h_size_ - chain_len; 
      chains.push_back(chain);
      line->addWidget(chain);
      stream >> chain_len;
    }
    left_chains_.push_back(std::move(chains));
    left_chains_layout_->addLayout(line);
  }
  return size;
}

bool Crossword::CheckLine(const std::vector<Field*> &line) {
  for (auto field : line) {
    if (field->checked_ == false) {
      return false;
    }
  }
  return true;
}

bool Crossword::CheckChains(const std::vector<std::vector<Chain*>> &v_chains) {
  for (auto chains : v_chains) {
    for (auto chain : chains) {
      if (chain->min_pos_ != chain->max_pos_) {
        return false;
      }
    }
  }
  return true;
}

void Crossword::Run() {
  if (v_size_ == 0 || h_size_ == 0) {
    QMessageBox::information(nullptr, "результат поиска", "Сначала загрузите кросворд для решения");
    return;
  }
  for (unsigned i = 0; i < v_size_; ++i) {
    HCalc(i, 5);
  }
  for (unsigned i = 0; i < h_size_; ++i) {
    VCalc(i, 5);
  }
  bool next = true;
  while (next) {
    next = false;
    for (unsigned i = 0; i < v_size_; ++i) {
      switch (HCalc(i)) {
        case -1:
          QMessageBox::information(nullptr, "результат поиска", "Ошибка при решении строки " + QString::number(i));
          return;
        case 1:
          next = true;
          break;
      }
    }
    for (unsigned i = 0; i < h_size_; ++i) {
      switch (VCalc(i)) {
        case -1:
          QMessageBox::information(nullptr, "результат поиска", "Ошибка при решении столбца " + QString::number(i));
          return;
        case 1:
          next = true;
          break;
      }
    }
  }
  if (CheckChains(top_chains_) == false || CheckChains(left_chains_) == false) {
    QMessageBox::information(nullptr, "результат поиска:", "Не удалось найти решение:(");
    return;
  }
  QMessageBox::information(nullptr, "результат поиска:", "Решение найдено!");
}