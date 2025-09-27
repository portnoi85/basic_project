#include "crossword.h"
#include <fstream>
#include <QFileDialog>

Crossword::Crossword(QWidget *parent)
	: QWidget(parent) {
  grid_ = new QGridLayout();
  fields_ = new QGridLayout();
  left_values_ = new QVBoxLayout();
  top_values_ = new QHBoxLayout();
  QHBoxLayout *lower_layout = new QHBoxLayout();
  QPushButton *b_load = new QPushButton("Загрузить");
  QPushButton *b_run = new QPushButton("Решить");

  lower_layout->addWidget(b_load);
  lower_layout->addWidget(b_run);
  grid_->addLayout(lower_layout, 2, 1);
  grid_->addLayout(top_values_, 0, 1);
  grid_->addLayout(left_values_, 1, 0);
  grid_->addLayout(fields_, 1, 1);
  setLayout(grid_);
  connect(b_load, SIGNAL(clicked(bool)), SLOT(Load()));
  connect(b_run, SIGNAL(clicked(bool)), SLOT(Run()));
}

Crossword::~Crossword() {
}

void Crossword::SetSize(unsigned int h_size, unsigned int v_size) {
  h_size_ = h_size;
  v_size_ = v_size;
  Clear();
  CreateField();
}

void Crossword::Clear() {
  for (QPushButton * b : line_) {
	  delete b;
  }
  line_.clear();
}

void Crossword::CreateField() {
  line_.reserve(h_size_ * v_size_);
  for (unsigned int i = 0; i < v_size_; ++i) {
    for (unsigned int j = 0; j < h_size_; ++j) {
      Field * button = new Field();
      fields_->addWidget(button, i, j);
      line_.push_back(button);
	  }
  }
}

void Crossword::HCalc(unsigned int i) {
  std::vector<Field*> line;
  for (unsigned int j = 0; j < h_size_; ++j) {
      line.push_back(line_[i * h_size_ + j]);
    }
  Finder finder(line, v_chains_[i]);
  finder.FindResult();
  finder.GetResult();
}

void Crossword::VCalc(unsigned int i) {
  std::vector<Field*> line;
  std::vector<unsigned int > values;
  for(auto chain : h_chains_[i]) {
    values.push_back(chain->text().toInt());
  }
  for (unsigned int j = 0; j < v_size_; ++j) {
    line.push_back(line_[i + j * h_size_]);
  }
  Finder finder(line, h_chains_[i]);
  finder.FindResult();
  finder.GetResult();
}

void ClearLayout(QLayout * layout) {
  while (layout->count() > 0) {
    QLayoutItem *item = layout->takeAt(0);
    if (item->widget()) {
      QWidget * widget = item->widget();
      layout->removeWidget(widget);
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
  int h_size, v_size;
  int color_size, rc, gc, bc;
  stream >> h_size >> v_size;
  SetSize(h_size, v_size);
  stream >> color_size >> rc >> gc >> bc;
  h_chains_.clear();
  ClearLayout(top_values_);
  for (int i = 0; i < h_size; ++i) {
    std::vector<Chain*> chains;
    int chain_size = 0;
    QVBoxLayout *line = new QVBoxLayout();
    line->addStretch(1);
    stream >> chain_size;
    while (chain_size) {
      Chain *b1 = new Chain(chain_size);
      chains.push_back(b1);
      line->addWidget(b1);
      stream >> chain_size;
    }
    h_chains_.push_back(std::move(chains));
    top_values_->addLayout(line);
  }
  v_chains_.clear();
  ClearLayout(left_values_);
  for (int i = 0; i < v_size; ++i) {
    std::vector<Chain*> chains;
    int chain_size = 0;
    QHBoxLayout *line = new QHBoxLayout();
    line->addStretch(1);
    stream >> chain_size;
    while (chain_size) {
      Chain *b1 = new Chain(chain_size);
      chains.push_back(b1);
      line->addWidget(b1);
      stream >> chain_size;
    }
    v_chains_.push_back(std::move(chains));
    left_values_->addLayout(line);
  }
}

void Crossword::Run() {
  for (int k = 0; k < 4; ++k) {
    for (unsigned i = 0; i < v_size_; ++i) {
      HCalc(i);
    }
    for (unsigned i = 0; i < h_size_; ++i) {
      VCalc(i);
    }
  }
}