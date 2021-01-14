
//#include <emu/pc/CSCConstants.h>
#include "emu/pc/GEMPatternGen.h"
//#include <random>
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
#define RAMPAGE_SIZE 4*1024 

namespace emu { namespace pc {

int GEMPatternGen::Sum(int x, int y)
{
  sum = x + y;
  return sum;
}




void GEMPatternGen::writenbytes(std::fstream* output, unsigned int n){

    std::cout <<" write n bytes to oss:   " << n << std::endl;
   for (unsigned int i=1;i <= n; i++)
        (*output) << char(0);

}

void GEMPatternGen::GEMPatternGenerator(char * textfile, char * patfiledir) {
    std::vector<GEMPad> input_pads;
    unsigned int tmb_compile_type = 0xc;
    
    //std::fstream text_file(argv[1], std::ios_base::in);
    //std::cout << "*******This is a test of the char * textfile: " << textfile << std::endl;
    std::fstream text_file(textfile);

    unsigned int bx;
    unsigned int pad;
    unsigned int etapartition;
    unsigned int n=0;
    //ignore first line
    std::string str;
    std::string prefix;

   // text_file.ignore ( std::numeric_limits<std::streamsize>::max(), '\n' );
    std::getline(text_file, prefix);
    std::getline(text_file, str);
    prefix = prefix.substr(7,prefix.length()-6);
    std::cout <<"prefix "<< prefix  <<" second line " << str << std::endl;

    while(!text_file.eof()){
        text_file>>bx>>pad>>etapartition;
        input_pads.push_back(GEMPad());
        input_pads[n].bx = bx;
        input_pads[n].pad = pad;
        input_pads[n].etapartition = etapartition;
        //std::cout <<"text_file " << text_file << std::endl;
        std::cout << "bx " << bx <<" GEM pad " << pad << "  etapartition " << etapartition  << std::endl;     
        n++; 
    }

    std::fstream* oss;
    char c;
    int totbytes;
    //char tmbtype = tmb_compile_type- 0xa + 'a';
    std::stringstream ss;
    ss << patfiledir << "/" << prefix <<"_GEMPads.pat";
    oss =  new std::fstream(ss.str().c_str(), std::ios_base::out);
    
    totbytes = 0;
    unsigned int remainbits = 0;
    unsigned int remaininfo = 0;
    unsigned int npads_bx = 0;// 4 or less than 4 
    unsigned int lastbx = 0;

    for (unsigned int i = 0; i < input_pads.size() ; i++){
        std::cout  <<"i " << i << " bx " << input_pads[i].bx << "  etapartition " << (std::bitset<3>)input_pads[i].etapartition 
            <<" GEM pad " << (std::bitset<8>)input_pads[i].pad << std::endl;     
        if (lastbx < input_pads[i].bx ){
            (*oss) << char(remaininfo << (8-remainbits));
            std::cout <<" lastbx " << lastbx <<" input_pads bx " << input_pads[i].bx << " bits " << (std::bitset<8>)(remaininfo << (8-remainbits)) << std::endl;
            writenbytes(oss, 7-npads_bx*11/8+8*(input_pads[i].bx-1-lastbx));
            lastbx = input_pads[i].bx;
            npads_bx = 0;
            remainbits = 0;
            remaininfo = 0;

        }
       if (lastbx == input_pads[i].bx){//npads_bx could be 0,1,2,3,4
            if (npads_bx == 4) {
                continue;
            }
            else if (npads_bx > 4) continue;
            unsigned int info = ((input_pads[i].etapartition << 8) | input_pads[i].pad);
            (*oss) << char(((remaininfo << (8-remainbits)) | (info >> (3+remainbits))));
            remainbits = 3+remainbits;
            unsigned int x = std::pow(2, remainbits) -1;
            remaininfo = (info & x);
            //std::cout <<"info  " <<(std::bitset<11>)info << "  remainbits "<< remainbits <<" x " << (std::bitset<8>) x << "  remaininfo " <<(std::bitset<8>)remaininfo <<std::endl;
            if (remainbits >= 8){
                (*oss) << char((remaininfo >> (remainbits-8)));
                remainbits = remainbits - 8;
                x = std::pow(2,remainbits)-1;
                remaininfo = (remaininfo & x);
            //std::cout <<"info  " <<(std::bitset<11>)info << "  remainbits "<< remainbits <<" x " << (std::bitset<8>) x << "  remaininfo " <<(std::bitset<8>)remaininfo <<std::endl;
            }
            npads_bx ++;
        }
       
       
    }

    (*oss) << char(remaininfo << (8-remainbits));
    writenbytes(oss, 7-npads_bx*11/8);
    unsigned int size = input_pads.size();
    writenbytes(oss, RAMPAGE_SIZE-input_pads[size-1].bx*8-8);
    
    (*oss) << std::flush;
}
}}
