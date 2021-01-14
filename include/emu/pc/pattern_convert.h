#ifndef PATTERNCONVERT_H_
#define PATTERNCONVERT_H_

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <bitset>
#include <iomanip>
#include <cmath>
#include <ctime>
#include <vector>
#include <limits>       // std::numeric_limits

namespace emu { namespace pc {

class PatternConvert
{
  public:
  static int layerorder_all[6][6];
  unsigned int get_next(std::istream & is, char sep);
  bool clear_until(std::istream & is, char b_char) ;
  void shufflelayers(std::vector<unsigned int> &triads, int DCFEBs, int layerset);
  void convert(std::istream & is, int layerset, std::string prefix, char sep);
  void Pattern_Converter(char * textfilefull);

};
}}
#endif
