#include "emu/pc/pattern_convert.h"

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <bitset>
#include <cstdlib>

#define RAMPAGE_SIZE 4*1024 
//DCFEB count from 0 
// Expects next char to be beginning of byte

namespace emu { namespace pc {


int PatternConvert::layerorder_all[6][6] = {{3,1,5,6,4,2},//layerorder1
                      {4,2,5,6,3,1},
                      {1,3,6,5,2,4},
                      {2,4,6,5,1,3},
                      {4,6,2,1,3,5},
                      {1,2,3,4,5,6}
                      };
unsigned int PatternConvert::get_next(std::istream & is, char sep = '|') {
    unsigned int n = 0;
    char c;
    unsigned int next = 0;
    unsigned int i = 0 ;
    while(is) {
        if(!is.get(c))
            break;
        if(c == '0')
            next = 0;
        else if(c == '1')
            next = 1;
        else if(c == sep)
            break;
        else
            continue;
        n =  (next << i)| n;
        i++;
    }
//    std::cout <<" return n " << (std::bitset<8>) n << std::endl;
    return n;
}

bool PatternConvert::clear_until(std::istream & is, char b_char) {
    char c = 0;
    while(is) {
        if(!is.get(c) || c == b_char)
            break;
       // std::cout <<" c in clear_until " << c << std::endl;
    }
    return is;
}

void PatternConvert::shufflelayers(std::vector<unsigned int> &triads, int DCFEBs, int layerset){

    int size= triads.size();
   
     
    if (size%DCFEBs != 0 || size%6 != 0) std::cout << "error " << std::endl;
   
    for (int j=0; j < size/(DCFEBs*6) ; j++){
    	for (int i=0 ; i < DCFEBs ; i++){
		unsigned int layers[6] ={0};
		for (int k=0; k<6 ; k++){
                        //old layer layerorder[k] to
			layers[k] = triads[j*DCFEBs*6 + (layerorder_all[layerset-1][k]-1)*DCFEBs + i];	
		}
		for (int k=0; k<6 ; k++){
                        //new layer k 
			triads[j*DCFEBs*6 + k*DCFEBs + i] = layers[k];	
		}
	}


    }



}


void PatternConvert::convert(std::istream & is, int layerset, std::string prefix = "", char sep = '|') {
    std::vector<std::fstream *> oss;
    std::vector<unsigned int > triads;
    int l;// layer
    int n = 0;//DCFEB
    int DCFEBs = 0;
    int bx;
    char c = 0;
    
    
    if (layerset > sizeof(layerorder_all)/sizeof(layerorder_all[0])) std::cout << " no such layerset, error " << std::endl; 
    std::vector<int> c_count;//count bits for each DCFEB
   
//here is the first row  
    is >> bx >> l ;
    clear_until(is, sep);
    while(is) {
        c = get_next(is, sep);
        //std::cout <<"BX " << bx << " DCFEB_n " <<n << " layer " << l << " c= " << (std::bitset<8>) c << std::endl;
    	std::stringstream ss;
    	ss << prefix  <<"layerorder" << layerset << "_CFEB" << n << ".pat";
        std::cout <<" pattern file for DCFEB "<< n <<"  "<<ss.str() << std::endl;
    	oss.push_back(new std::fstream(ss.str().c_str(), std::ios_base::out));
        if(!is)
            break;
        //(*(oss[n])) << c;
        triads.push_back(c);
        c_count.push_back(1);//1 means bits of one layer, 
        n++;
        if(is.peek() == '\n' || is.peek() == '\r')
            break;
    }

    DCFEBs = n;//record num of DCRFEBs in txt file
//
    n = 0;
    while(is) {
        is >> bx >> l;
       	clear_until(is, sep);
        //std::cout <<"==================  " << std::endl;
        while(is){
        	c = get_next(is, sep);
        	if(!is)
              		break;
        	//(*(oss[n])) << c;
        //	std::cout <<"BX " << bx << " DCFEB_n " <<n << " layer " << l << " c= " << (std::bitset<8>) c << std::endl;
        	if (c_count[n] % 6 != l ) std::cout <<" something is wrong " << std::endl ;
        	++c_count[n];
        	triads.push_back(c);
        	//if(c_count[n] % 6 == 0)
             	//	(*(oss[n])) << char(0) << char(0);
        	n = (n+1)%DCFEBs;
                if (is.peek() == '\n' || is.peek() == '\r')
			break;
       }
      //std::cout <<"==================  " << std::endl;
    }
    //std::cout <<" before shufflelayers " << std::endl;
    //std::cout << std::hex << triads << std::endl;
    shufflelayers(triads, DCFEBs, layerset);
    //std::cout <<" after shufflelayers " << std::endl;
    for (int i= 0; i < triads.size()/DCFEBs; i++){
        for (int j=0; j < DCFEBs; j++){
    	 	//std::cout << (std::bitset<8>)triads[i*DCFEBs+j] <<"\t";
            c = triads[i*DCFEBs+j];
		    (*(oss[j])) << c;
		    if ((i+1) %6 == 0) 
			    (*(oss[j])) << char(0) << char(0);
	    }
    //    std::cout << std::endl;

    }
    int totbytes = 0;
    for(int i=0; i<DCFEBs; ++i) {
        totbytes = c_count[i] + (c_count[i]/6)*2;
        //std::cout << "bits for DCFEB_n " <<i << " " <<  c_count[i]*8 + (c_count[i]/6)*16 << std::endl;
        while (  totbytes < RAMPAGE_SIZE ) {
		(*(oss[i])) << char(0) ;
		totbytes++;
        }// fill 0 if the bits is not large enough
        (*(oss[i])) << std::flush;
    }

}

void PatternConvert::Pattern_Converter(char * textfilefull) { //textfilefull is the directory and the filename for the text file
    std::fstream text_file(textfilefull, std::ios_base::in);
    std::string file_name(textfilefull);
    int layerset = 1;

    if (layerset > sizeof(layerorder_all)/sizeof(layerorder_all[0])) std::exit(0); 
    for(int i=file_name.size() - 1; i>=0; --i) {
        if(file_name[i] == '.') {
            file_name = file_name.substr(0, i);
            if(file_name != "")
                file_name = file_name + "_";
            break;
        }
    }
    convert(text_file, layerset , file_name);
}
}}//namespaces
