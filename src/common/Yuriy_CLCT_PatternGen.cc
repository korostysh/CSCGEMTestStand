
#include "emu/pc/Yuriy_CLCT_PatternGen.h"
//#include "CSCConstants.h"
//#include <random>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <bitset>
#include <iomanip>
#include <cmath>
#include <ctime>
#include <vector>

#define RAMPAGE_SIZE 4*1024 

namespace emu { namespace pc {

int Yuriy_CLCT_PatternGen::layerorder_all[6][6] = {{3,1,5,6,4,2},//layerorder1
                      {4,2,5,6,3,1},
                      {1,3,6,5,2,4},
                      {2,4,6,5,1,3},
                      {4,6,2,1,3,5},
                      {1,2,3,4,5,6}
                      };

template <size_t XS_per_CFEB> int Yuriy_CLCT_PatternGen::GetOutputXStrip(int cfeb, int input_xstrip, int tmb_compile_type) {
  const int Bad_HS = -1;
  const int Min_CFEB_in_non_me11 = 0; 
  const int Max_CFEB_in_non_me11 = 4; 
  const int N_CFEB_in_non_me11 = Max_CFEB_in_non_me11 - Min_CFEB_in_non_me11 + 1; 
  const int Min_XS_in_non_me11 = 0; 
  const int Max_XS_in_non_me11 = N_CFEB_in_non_me11*XS_per_CFEB - 1; 
  const int Min_CFEB_in_region_a = 0; 
  const int Max_CFEB_in_region_a = 3; 
  const int Min_XS_in_region_a = Min_CFEB_in_region_a*XS_per_CFEB;
  const int Max_XS_in_region_a = (Max_CFEB_in_region_a + 1)*XS_per_CFEB - 1; 
  const int Min_CFEB_in_region_b = 4; 
  const int Max_CFEB_in_region_b = 6; 
  const int Min_XS_in_region_b = Min_CFEB_in_region_b*XS_per_CFEB;
  const int Max_XS_in_region_b = (Max_CFEB_in_region_b + 1)*XS_per_CFEB - 1; 
  int output_xs = Bad_HS;
  if(tmb_compile_type == 0xa) {
    if(cfeb >= Min_CFEB_in_non_me11 && cfeb <= Max_CFEB_in_non_me11)
      output_xs = Min_XS_in_non_me11 + (XS_per_CFEB*cfeb + input_xstrip);
    if(!(output_xs >= Min_XS_in_non_me11 && output_xs <= Max_XS_in_non_me11))
      output_xs = Bad_HS;
  }
  else if(tmb_compile_type == 0xb) {
    if(cfeb >= Min_CFEB_in_non_me11 && cfeb <= Max_CFEB_in_non_me11)
      output_xs = Max_XS_in_non_me11 - (XS_per_CFEB*cfeb + input_xstrip); 
    if(!(output_xs >= Min_XS_in_non_me11 && output_xs <= Max_XS_in_non_me11))
      output_xs = Bad_HS;
  }
  else if(tmb_compile_type == 0xc) {
    if(cfeb >= Min_CFEB_in_region_a && cfeb <= Max_CFEB_in_region_a)
      output_xs = Min_XS_in_region_a + (XS_per_CFEB*(cfeb - Min_CFEB_in_region_a) + input_xstrip);
    else if(cfeb >= Min_CFEB_in_region_b && cfeb <= Max_CFEB_in_region_b)
      output_xs = Max_XS_in_region_b - (XS_per_CFEB*(cfeb - Min_CFEB_in_region_b) + input_xstrip);
    if(!(output_xs >= Min_XS_in_region_a && output_xs <= Max_XS_in_region_b))
      output_xs = Bad_HS;
  }
  else if(tmb_compile_type == 0xd) {
    if(cfeb >= Min_CFEB_in_region_a && cfeb <= Max_CFEB_in_region_a)
      output_xs = Max_XS_in_region_a - (XS_per_CFEB*(cfeb - Min_CFEB_in_region_a) + input_xstrip);
    else if(cfeb >= Min_CFEB_in_region_b && cfeb <= Max_CFEB_in_region_b)
      output_xs = Min_XS_in_region_b + (XS_per_CFEB*(cfeb - Min_CFEB_in_region_b) + input_xstrip);
    if(!(output_xs >= Min_XS_in_region_a && output_xs <= Max_XS_in_region_b))
      output_xs = Bad_HS;
  }
  
  return output_xs;
}





template <size_t XS_per_CFEB> int Yuriy_CLCT_PatternGen::GetInputXStrip(int output_xstrip, int tmb_compile_type) {
  const int Min_CFEB_in_non_me11 = 0;
  const int Max_CFEB_in_non_me11 = 4;
  const int Min_CFEB_in_region_a = 0;
  const int Max_CFEB_in_region_a = 3;
  const int Min_CFEB_in_region_b = 4;
  const int Max_CFEB_in_region_b = 6;
  const int Max_XS_in_CFEB = XS_per_CFEB - 1;
  
  int region = output_xstrip / XS_per_CFEB;
  int phase = output_xstrip % XS_per_CFEB;
  int input_xs = -1;
  if(tmb_compile_type == 0xa) {
    if(region >= Min_CFEB_in_non_me11 && region <= Max_CFEB_in_non_me11)
      input_xs = phase;
  }
  else if(tmb_compile_type == 0xb) {
    if(region >= Min_CFEB_in_non_me11 && region <= Max_CFEB_in_non_me11)
      input_xs = Max_XS_in_CFEB - phase;
  }
  else if(tmb_compile_type == 0xc) {
    if(region >= Min_CFEB_in_region_a && region <= Max_CFEB_in_region_a) {
      input_xs = phase;
    }
    else if(region >= Min_CFEB_in_region_b && region <= Max_CFEB_in_region_b) {
      input_xs = Max_XS_in_CFEB - phase;
    }
  }
  else if(tmb_compile_type == 0xd) {
    if(region >= Min_CFEB_in_region_a && region <= Max_CFEB_in_region_a) {
      input_xs = Max_XS_in_CFEB - phase;
    }
    else if(region >= Min_CFEB_in_region_b && region <= Max_CFEB_in_region_b) {
      input_xs = phase;
    }
  }
  return input_xs;
}
    //
template <size_t XS_per_CFEB>  int Yuriy_CLCT_PatternGen::GetInputCFEBByX(int output_xstrip, int tmb_compile_type) {
  const int Min_CFEB_in_non_me11 = 0;
  const int Max_CFEB_in_non_me11 = 4;
  const int Min_CFEB_in_region_a = 0;
  const int Max_CFEB_in_region_a = 3;
  const int Min_CFEB_in_region_b = 4;
  const int Max_CFEB_in_region_b = 6;
  
  int region = output_xstrip / XS_per_CFEB;
  int cfeb = -1;
  if(tmb_compile_type == 0xa) {
    if(region >= Min_CFEB_in_non_me11 && region <= Max_CFEB_in_non_me11)
      cfeb = region;
  }
  else if(tmb_compile_type == 0xb) {
    if(region >= Min_CFEB_in_non_me11 && region <= Max_CFEB_in_non_me11)
      cfeb = Max_CFEB_in_non_me11 - region;
  }
  else if(tmb_compile_type == 0xc) {
    if(region >= Min_CFEB_in_region_a && region <= Max_CFEB_in_region_a) {
      cfeb = region;
    }
    else if(region >= Min_CFEB_in_region_b && region <= Max_CFEB_in_region_b) {
      cfeb = Min_CFEB_in_region_b + (Max_CFEB_in_region_b - region);
    }
  }
  else if(tmb_compile_type == 0xd) {
    if(region >= Min_CFEB_in_region_a && region <= Max_CFEB_in_region_a) {
      cfeb = Min_CFEB_in_region_a + (Max_CFEB_in_region_a - region);
    }
    else if(region >= Min_CFEB_in_region_b && region <= Max_CFEB_in_region_b) {
      cfeb = region;
    }
  }
  return cfeb;
}
    
bool Yuriy_CLCT_PatternGen::hits_Generation(int strip, int pattern, int Nhits, unsigned int hits[CSCConstants::NUM_DCFEBS]){

    //
    if (pattern<2 || pattern >= CSCConstants::NUM_PATTERNS) return false;
    if (Nhits < 1 || Nhits > CSCConstants::NUM_LAYERS) return false;
    std::srand(std::time(0)); // use current time as seed for random generator
    //int random_variable = std::rand();
    for (int i = 0; i < Nhits; i++){
        int layer = std::abs(std::rand())%6;
        while ( hits[layer] != 999)   layer = std::abs(std::rand())%6;
        int hs  = strip+CSCConstants::CLCTPatterns[pattern][layer];
        if (hs < 0 || hs > CSCConstants::NUM_HS) return false;
        hits[layer] = hs;
        //std::cout << "i " << i << " layer " << layer <<"  hs " << hs << std::endl;
    }
    return true;
}

void Yuriy_CLCT_PatternGen::triad_Generation( unsigned int (&triads)[CSCConstants::NUM_DCFEBS][3][CSCConstants::NUM_LAYERS], int strip, int pattern, int Nhits , int tmb_compile_type){

   unsigned int hits[6] = {999, 999, 999, 999, 999, 999};
//unsigned int triads[CSCConstants::NUM_DCFEBS][3][CSCConstants::NUM_LAYERS] = {0};
   hits_Generation(strip, pattern, Nhits, hits);
   for (int j=0; j < CSCConstants::NUM_LAYERS; j++){
        if (hits[j] == 999) continue;//default is 0
        int dcfeb = GetInputCFEBByX<32>(hits[j], tmb_compile_type);
        int localhs = GetInputXStrip<32>(hits[j], tmb_compile_type);
        int distrip = localhs/4;
        unsigned int n = 1 << (distrip);
        bool leftstrip = (localhs%4 < 2);//? triad rules for different dcfeb
        bool lefths = ((localhs%4)%2 == 0);
        triads[dcfeb][0][j] = n;
        triads[dcfeb][1][j] = leftstrip? 0 : n;
        triads[dcfeb][2][j] = lefths? 0 : n;
        std::cout <<" layer " << j <<"  dcfeb " << dcfeb << " hs "<< hits[j] <<"  localhs " << localhs << " distrip " << distrip 
            << " 1st " << (std::bitset<8>)n << " 2nd " <<(std::bitset<8>)triads[dcfeb][1][j] << " 3rd " << (std::bitset<8>)triads[dcfeb][2][j] <<std::endl;     
  }
   std::cout << std::endl; 
}

/*
bool triad_Generation( unsigned int (&triads)[CSCConstants::NUM_DCFEBS][3][CSCConstants::NUM_LAYERS], CLCT clct, int tmb_compile_type){

    return triad_Generation(triads, clct.keystrip, clct.pattern, clct.nhits, tmb_compile_type);
}
*/  

void Yuriy_CLCT_PatternGen::dumptriads(unsigned int triads[CSCConstants::NUM_DCFEBS][3][CSCConstants::NUM_LAYERS]){


    for (int j = 0; j < 3; j++){
        for (int k = 0; k < CSCConstants::NUM_LAYERS; k++){
            for (int i = 0; i < CSCConstants::NUM_DCFEBS; i++)
                std::cout <<"  " << (std::bitset<8>)triads[i][j][k];     
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;

}


void Yuriy_CLCT_PatternGen::finalizetriads(unsigned int (&triads)[CSCConstants::NUM_DCFEBS][3][CSCConstants::NUM_LAYERS], unsigned int (&triads_final)[CSCConstants::NUM_DCFEBS][3][CSCConstants::NUM_LAYERS]){


    for (int j = 0; j < 3; j++){
        for (int k = 0; k < CSCConstants::NUM_LAYERS; k++){
            for (int i = 0; i < CSCConstants::NUM_DCFEBS; i++){
                triads_final[i][j][k] = triads[i][j][k] | triads_final[i][j][k];
                triads[i][j][k] =0;
            }
        }
    }
   
}

void Yuriy_CLCT_PatternGen::writetriads(unsigned int (&triads)[CSCConstants::NUM_DCFEBS][3][CSCConstants::NUM_LAYERS], std::fstream* oss, int bx){
   
    //std::cout <<" triads written into pattern file " << std::endl;
    //dumptriads(triads);
    for (int j = 0; j < 3; j++){
        for (int k = 0; k < CSCConstants::NUM_LAYERS; k++){
                (*oss) << std::setfill('0') << std::setw(2) <<bx+j<<" "<<k<<" ";
            for (int i = 0; i < 5; i++){
                (*oss) <<"|"<< (std::bitset<8>)triads[i][j][k];
                triads[i][j][k] = 0;//clear final triads
            }
            (*oss) <<"|" << std::endl;
        }            
    }

   
}


void Yuriy_CLCT_PatternGen::fillonebx(std::fstream* oss,unsigned int bx){


    for (int k = 0; k < CSCConstants::NUM_LAYERS; k++){
            (*oss) << std::setfill('0') << std::setw(2) <<bx<<" "<<k<<" ";
        for (int i = 0; i < 5; i++){
            (*oss) <<"|"<< (std::bitset<8>)0;
        }
            (*oss) <<"|" << std::endl;
    }            

}

void Yuriy_CLCT_PatternGen::reversion(unsigned int &num, unsigned int nbit){

    unsigned int i=0;
    unsigned int tmp=0;;
    while (i < nbit){
        unsigned int x=(unsigned int)std::pow(2,nbit-i-1);
        if ((num&x) > 0) tmp = (tmp | 1<<(i));
        i++;
    }
   num=tmp;

}


void Yuriy_CLCT_PatternGen::triadreversion(unsigned int (&triads)[CSCConstants::NUM_DCFEBS][3][CSCConstants::NUM_LAYERS], unsigned int nbit=8){

    for (int i=0; i < CSCConstants::NUM_DCFEBS ; i++){
        for( int j = 0; j < 3; j++){
		    for (int k=0; k < CSCConstants::NUM_LAYERS ; k++){
                reversion( triads[i][j][k],nbit);
		    }
	    }
    }
}


void Yuriy_CLCT_PatternGen::Yuriy_CLCT_PatternGenerator(char * textfile, char * patfiledir) {
    unsigned int input_hs = 0;
    unsigned int input_pattern = 0;
    unsigned int input_nhits =0;
    unsigned int input_bx =0;
    unsigned int tmb_compile_type = 0xc;
    std::vector<CLCT> clcts;
    std::fstream text_file(textfile, std::ios_base::in);
    /*if(argc > 1)
        input_hs = atoi(argv[1]);
    if(argc > 2)
        input_pattern = atoi(argv[2]);
    if(argc > 3)
        input_nhits = atoi(argv[3]);
    */
    unsigned int triads[CSCConstants::NUM_DCFEBS][3][CSCConstants::NUM_LAYERS] = { { {0} } };
    unsigned int triads_final[CSCConstants::NUM_DCFEBS][3][CSCConstants::NUM_LAYERS] = { { {0} } };

    unsigned int n=0;
    std::string str;
    std::string prefix;

   // text_file.ignore ( std::numeric_limits<std::streamsize>::max(), '\n' );
    std::getline(text_file, prefix);
    std::getline(text_file, str);
    prefix = prefix.substr(7,prefix.length()-7);
    std::cout <<"prefix "<<prefix <<" second line  " << str << std::endl;
    //while(!text_file.eof()){
      while (text_file>>input_bx>>input_hs>>input_pattern>>input_nhits){
        clcts.push_back(CLCT());
        clcts[n].bx = input_bx;
        clcts[n].keystrip = input_hs;
        clcts[n].pattern = input_pattern;
        clcts[n].nhits = input_nhits;
        clcts[n].dcfeb = GetInputCFEBByX<32>(input_hs, tmb_compile_type);//get dcfeb
        clcts[n].localhs = GetInputXStrip<32>(input_hs, tmb_compile_type);//get localhs
        
        //std::cout <<"text_file " << text_file << std::endl;
        std::cout <<"bx " << input_bx <<" CLCT hs " << input_hs << "  pattern " << input_pattern << " input_nhits " << input_nhits << std::endl;     
        n++; 
    }

    std::fstream* oss;
    unsigned int size = clcts.size();
    char tmbtype = tmb_compile_type- 0xa + 'a';
    std::stringstream ss;
    ss << patfiledir << "/" << prefix <<"_ClctPattern" << "_tmb"<< tmbtype << ".txt";
    oss = new std::fstream(ss.str().c_str(), std::ios_base::out);
    unsigned int lastbx = 0;
    for (unsigned int i=0; i<clcts.size(); i++){
        if (lastbx ==0 && clcts[i].bx>0){
            for (; lastbx<clcts[i].bx; lastbx++)
                fillonebx(oss, lastbx);
        }
        if (lastbx < clcts[i].bx){
            if ( i != 0 and clcts[i].bx < lastbx+3) continue;// ignore the next two bxs if lastbx has a valid pattern
            //std::cout <<"final triads for bx "<< lastbx << std::endl;
            //dumptriads(triads_final);
            triadreversion(triads_final);
            std::cout <<"after reversion " << std::endl;
            dumptriads(triads_final);
           //write lastbx into oss
            writetriads(triads_final, oss, lastbx); 
            lastbx = lastbx+3;
            for (;lastbx < clcts[i].bx; lastbx++)
                fillonebx(oss, lastbx);//fill each dcfeb 8bytes
        }
     if (lastbx == clcts[i].bx){
          //get final triads
          triad_Generation(triads, clcts[i].keystrip, clcts[i].pattern, clcts[i].nhits, tmb_compile_type);
          //std::cout <<"i "<< i <<" triad for hs"<< clcts[i].keystrip << " pattern "<< clcts[i].pattern <<" nhits "<< clcts[i].nhits <<std::endl;
          //dumptriads(triads);
          finalizetriads(triads, triads_final);
          //std::cout << "final triads at this moment " << std::endl;
          //  dumptriads(triads_final);
        }
    }
        
    triadreversion(triads_final);
    std::cout <<"after reversion " << std::endl;
    dumptriads(triads_final);
    writetriads(triads_final, oss, lastbx);
    for (; lastbx<12; lastbx++)
        fillonebx(oss, lastbx);
    //for (unsigned int i=0; i< oss.size() ; i++)
    //    (*(oss[i])) << std::flush;
    oss->close();
}
}}//namespaces
