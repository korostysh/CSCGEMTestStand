#ifndef GEMPATTERNGEN_H_
#define GEMPATTERNGEN_H_

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

class GEMPatternGen 
{
public:
    int sum;
    struct GEMPad{
        unsigned int bx;
        unsigned int pad;
        unsigned int etapartition;

    };
    void writenbytes(std::fstream* output, unsigned int n);
    void GEMPatternGenerator(char * textfile, char * patfiledir);


    int Sum(int, int);

};
}}
#endif
