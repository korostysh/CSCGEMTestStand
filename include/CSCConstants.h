#ifndef CSCCONSTANTS_H_
#define CSCCONSTANTS_H_

#include <cmath>
namespace emu { namespace pc {

namespace CSCConstants
{
     enum dcfebs {NUM_DCFEBS = 7};
     enum layer_info  {NUM_LAYERS = 6, KeyLayer = 3};
     enum clctpatterns_info {NUM_PATTERNS = 11};
     enum strips_info {NUM_HSPERDCFEB = 32, NUM_HS_ME1B = 128, NUM_HS_ME1A = 96, NUM_HS = 224, NUM_STRIPS = 112, NUM_DISTRIPS = 56};
     extern const int CLCTPatterns[NUM_PATTERNS][NUM_LAYERS];
     extern const int CCLUT_offset[5][6];
     extern const int pat6[4096];
     extern const int pat7[4096];
     extern const int pat8[4096];
     extern const int pat9[4096];
     extern const int pat10[4096];
}
}}
#endif
