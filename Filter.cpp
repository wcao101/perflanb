#include "Filter.h"
#include <iostream>

Filter::Filter(int _dim)
{
  divisor = 1;
  dim = _dim;
  data = new int[_dim * _dim];
}

void Filter::info()
{
  cout << "Filter is.." << endl;
  for (int col = 0; col < dim; col++) {
    for (int row = 0; row < dim; row++) {
      int v = get(row, col);
      cout << v << " ";
    }
    cout << endl;
  }
}

