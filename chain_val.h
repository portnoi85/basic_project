#ifndef CHAIN_VAL_H
#define CHAIN_VAL_H

struct ChainVal {
  unsigned int len;
  unsigned int pos{0};
  unsigned int min_pos{0};
  unsigned int max_pos{0};
  unsigned int tmp_min_pos{0};
  unsigned int tmp_max_pos{0};
  //для дальнейшей оптимизации:
  //unsigned int color_;

};

#endif // CHAIN_VAL_H