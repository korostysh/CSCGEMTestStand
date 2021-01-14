#include "Set.h"
#include "eth_lib.h"
#include "commands.h"
#include "utils.h"
//
//#include "CSCConstants.h"

namespace cw {

const std::string default_dir = "/home/cscdev/Sasha/pats/tmp/";
const std::string default_ps_output_file = "/home/cscdev/Sasha/pattern_studies/paramScan.log";

const int tmb_counters[2] = {
					29,	// clct0 sent to TMB matching section
					30	// clct1 sent to TMB matching section

				      };

using namespace std;
using namespace emu;
using namespace pc;

int patFile_to_pageID[11] = {// id	 .pat file
				1,	// CFEB 0
				2,	// CFEB 1
				3,	// CFEB 2
				8,	// CFEB 3
				9,	// CFEB 4
				10,	// CFEB 5
				11,	// CFEB 6
				4,	// GEM 1
				5,	// GEM 2
				6,	// GEM 3
				7	// GEM 4
			    };


//~~~~~~~~~ TMB Readout Manager ~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TMBresponse::TMBresponse(int clct_0, int clct_1, int delta_clct0, int delta_clct1, int N_occr){
	CLCT_0 = clct_0;
	CLCT_1 = clct_1;
	delta_CLCT0 = delta_clct0;
	delta_CLCT1 = delta_clct1;
	occurCount = N_occr;
}

void TMBresponse::operator++(){
	occurCount += 1;
	return;
}

bool operator==(const TMBresponse& a, const TMBresponse& b){
	return ((a.CLCT_0 == b.CLCT_0) && (a.CLCT_1 == b.CLCT_1) && (a.delta_CLCT0 == b.delta_CLCT0) && (a.delta_CLCT1 == b.delta_CLCT1));
}

std::ostream& operator<<(std::ostream& oss, const TMBresponse& tmbr_o){
	oss << "( " << std::hex << tmbr_o.CLCT_0 << " " << std::hex << tmbr_o.CLCT_1 << " " << std::dec << tmbr_o.delta_CLCT0 << " " << tmbr_o.delta_CLCT1 << " | " << std::dec << tmbr_o.occurCount << " )";
	return oss;
}

std::istream& operator>>(std::istream& iss, TMBresponse& tmbr_i){
	char tmp;
	int  clct_0, clct_1, delta_clct0, delta_clct1, n_occr;
	iss >> tmp >> clct_0 >> clct_1 >> delta_clct0 >> tmp >> delta_clct1 >> tmp >> n_occr >> tmp;
	tmbr_i = TMBresponse(clct_0, clct_1, delta_clct0, delta_clct1, n_occr);
	return iss;
}


//~~~~~~~~~ TMB Readout Manager LONG ~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TMBresponse_long::TMBresponse_long (int clct_nhit_0, int clct_pid_0, int clct_key_0, int clct_nhit_1, int clct_pid_1, int clct_key_1, int delta_clct0, int delta_clct1, int N_occr){
        CLCT_nhit_0 = clct_nhit_0;
	CLCT_pid_0 = clct_pid_0;
	CLCT_key_0 = clct_key_0;
        CLCT_nhit_1 = clct_nhit_1;
	CLCT_pid_1 = clct_pid_1;
	CLCT_key_1 = clct_key_1;
        delta_CLCT0 = delta_clct0;
	delta_CLCT1 = delta_clct1;
        occurCount = N_occr;
}

void TMBresponse_long::operator++(){
        occurCount += 1;
        return;
}

bool operator==(const TMBresponse_long& a, const TMBresponse_long& b){
        bool tf = ((a.CLCT_nhit_0 == b.CLCT_nhit_0) && (a.CLCT_pid_0 == b.CLCT_pid_0) && (a.CLCT_key_0 == b.CLCT_key_0));
	tf = tf && ((a.CLCT_nhit_1 == b.CLCT_nhit_1) && (a.CLCT_pid_1 == b.CLCT_pid_1) && (a.CLCT_key_1 == b.CLCT_key_1));
	tf = tf && ((a.delta_CLCT0 == b.delta_CLCT0) && (a.delta_CLCT1 == b.delta_CLCT1));
}

std::ostream& operator<<(std::ostream& oss, const TMBresponse_long& tmbr_o){
        oss << "( " << tmbr_o.CLCT_nhit_0 << " " << tmbr_o.CLCT_pid_0 << " " << tmbr_o.CLCT_key_0
	    << "  " << tmbr_o.CLCT_nhit_1 << " " << tmbr_o.CLCT_pid_1 << " " << tmbr_o.CLCT_key_1
	    << "; " << tmbr_o.delta_CLCT0 << " " << tmbr_o.delta_CLCT1 << " | " << std::dec << tmbr_o.occurCount << " )";
        return oss;
}

//~~~~~~~~~ Pattern Sets ~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

std::ostream& operator<<(std::ostream& oss, const RangeParam& rp){
	oss << "clct" << rp.clct << " : ";
	switch(rp.param){
	case 0:
		oss << "BX ";
		break;
	case 1:
		oss << "KEY";
		break;
	case 2:
		oss << "PID";
		break;
	case 3:
		oss << "NHIT";
		break;
	defalut:
		oss << "INVALID";
		break;
	}

	oss <<" Range [" << rp.min << " , " << rp.max << "] Step Size : " << rp.step_size;
	return oss;
}

std::ostream& operator<<(std::ostream& oss, const RangeParamCCLUT& rp){
	oss << "cclut" << rp.cclut << " : ";
	switch(rp.param){
	case 0:
		oss << "BX ";
		break;
	case 1:
		oss << "KEY";
		break;
	case 2:
		oss << "PID";
		break;
	case 3:
		oss << "CC_CODE";
		break;
	defalut:
		oss << "INVALID";
		break;
	}

	oss <<" Range [" << rp.min << " , " << rp.max << "] Step Size : " << rp.step_size;
	return oss;
}


bool Increment(std::vector<CLCT>& muons, std::vector<RangeParam>& free_pars){
	int ind = 0;
	while(true){

		int max_val = free_pars[ind].max;
		int curr_val = muons[free_pars[ind].clct].GetParam(free_pars[ind].param);

		cout << "Current Val = " << curr_val << endl;
		cout << "Maximum Val = " << max_val << endl;
		cout << "clct" << free_pars[ind].clct << "Parameter : " << free_pars[ind].param << endl;
		if(curr_val < max_val){
			muons[free_pars[ind].clct].SetParam(free_pars[ind].param, (curr_val + free_pars[ind].step_size) );	// Increment value
			muons[free_pars[ind].clct].RegenHits();									// Regnerate Triad Data
			return true;												// Continue Stepping
		}
		else if( (curr_val >= max_val) && (ind != (free_pars.size()-1)) ){	// reach max value AND not the last parameter
			muons[free_pars[ind].clct].SetParam(free_pars[ind].param, free_pars[ind].min );
			++ind;
		}
		else{
			return false;
		}

	}
	return false;
}


bool Increment(std::vector<CCLUT>& muons, std::vector<RangeParamCCLUT>& free_pars){
	int ind = 0;
	while(true){

		int max_val = free_pars[ind].max;
		int curr_val = muons[free_pars[ind].cclut].GetParam(free_pars[ind].param);

		cout << "Current Val = " << curr_val << endl;
		cout << "Maximum Val = " << max_val << endl;
		cout << "cclut" << free_pars[ind].cclut << "Parameter : " << free_pars[ind].param << endl;
		if(curr_val < max_val){
			muons[free_pars[ind].cclut].SetParam(free_pars[ind].param, (curr_val + free_pars[ind].step_size) );	// Increment value
			muons[free_pars[ind].cclut].RegenHits();									// Regnerate Triad Data
			return true;												// Continue Stepping
		}
		else if( (curr_val >= max_val) && (ind != (free_pars.size()-1)) ){	// reach max value AND not the last parameter
			muons[free_pars[ind].cclut].SetParam(free_pars[ind].param, free_pars[ind].min );
			++ind;
		}
		else{
			return false;
		}

	}
	return false;
}

/*
std::istream& operator>>(std::istream& iss, TMBresponse_long& tmbr_i){
        char tmp;
        int  clct_0, clct_1, delta_lct, n_occr;
        iss >> tmp >> clct_0 >> clct_1 >> delta_lct >> tmp >> n_occr >> tmp;
        tmbr_i = TMBresponse_long(clct_0, clct_1, delta_lct, n_occr);
        return iss;
}
*/

//~~~~~~~~~ Overall Sets ~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Set::Set(void){
	for(int i=0; i < 5; i++){
		Stats.push_back(0);
	}
	//Stats = {0, 0, 0, 0, 0};
	N_trials = 0;
	Tmb_type = COMPILE_TYPE - 0xa + 'a';
	Firmware = "XXX";
	Prefix = "default";
}

Set::Set(std::string& path)
{
	for(int i=0; i < 5; i++){
		Stats.push_back(0);
	}
	//Stats = {0, 0, 0, 0, 0};
	N_trials = 0;
	Tmb_type = COMPILE_TYPE - 0xa + 'a';
	Firmware = "XXX";
	Prefix = ReadTxt(path, CSC, GEM);
  sort(CSC.begin(), CSC.end(), compareCLCT);
}

bool Set::AddCLCT(CLCT clct_in)
{
	CSC.push_back(clct_in);
	return true;
}
bool Set::AddGEM(Cluster clust_in)
{
	GEM.push_back(clust_in);
	return true;
}
bool Set::AddCCLUT(CCLUT lut_in)
{
	LUT.push_back(lut_in);
	return true;
}

bool Set::RemoveCLCT(int i)
{
	if(i >=0 && i < CSC.size()){ return false; }
	CSC.erase(CSC.begin() + i);
	return true;
}
bool Set::RemoveGEM(int i)
{
	if(i>=0 && i < GEM.size()){ return false; }
	GEM.erase(GEM.begin() + i);
	return true;
}
bool Set::RemoveCCLUT(int i)
{
	if(i>=0 && i < LUT.size()){ return false; }
	LUT.erase(LUT.begin() + i);
	return true;
}


void Set::Clear(void)
{
	CSC.clear();
	GEM.clear();
	LUT.clear();
	Results.clear();
	Results_l.clear();
	Stats.clear();
	return;
}

bool Set::WritePatterns(std::string opt_path)
{
	std::cout << "Wrong pattern tool  *************************************************************************************************" << endl;
	std::string full_path;
	if(opt_path.empty()){
		full_path = default_dir + Prefix;
	}
	else{
		full_path = opt_path + Prefix;
	}
	return (WritePat(full_path, CSC)) && (WritePat(full_path, GEM));
}
bool Set::WritePatterns_LUT(std::string opt_path)
{
	std::cout << "Correct pattern tool  *************************************************************************************************" << endl;
	std::string full_path;
	if(opt_path.empty()){
		full_path = default_dir + Prefix;
	}
	else{
		full_path = opt_path + Prefix;
	}
	return (WritePat(full_path, LUT)) && (WritePat(full_path, GEM));
}


bool Set::LoadEmuBoard(std::string opt_path, int Mute)
{
	std::cout << Mute << "************************* Is it Muted *****************" << std::endl;
	if(opt_path.empty()) opt_path = default_dir;
	std::cout << " send on cmslab1" << std::endl;
	char block[RAMPAGE_SIZE];
	std::vector<FILE*> pat_files;

	for (int i = 0; i < CSCConstants::NUM_DCFEBS; i++)
        {
        	std::stringstream ss;
        	ss << opt_path << Prefix << "_cfeb" << i << "_tmb" << Tmb_type << ".pat";
          pat_files.push_back( fopen(ss.str().c_str(), "r") );
					if (Mute == 0) std::cout << "Opening File: " << ss.str() << std::endl;
         	//oss.push_back(new std::fstream(ss.str().c_str(), std::ios_base::out) );
        }
	for (int i=0; i < GEM_FIBERS; i++)
				{
					std::stringstream ss;
					ss << opt_path << Prefix << "_GEM"<<i<< "_tmb" << Tmb_type << ".pat";
					pat_files.push_back( fopen(ss.str().c_str(), "r") );
					if (Mute == 0)  std::cout << "Opening File: " << ss.str() << std::endl;
				}

	// Open device (i.e. Emulator Board)
	if(eth_open_device() == -1) {
		return false;

	}

	eth_reset();

	for (int i=0; i < pat_files.size(); i++)
	{
		if (Mute == 0) std::cout << "pageid = " << patFile_to_pageID[i] << std::endl;
		fread(block, sizeof(char), RAMPAGE_SIZE, pat_files[i]);
		memcpy(wdat,block,RAMPAGE_SIZE);
		if (Mute == 0) std::cout << " block = " << block << endl;

		int e = write_command(7,patFile_to_pageID[i], block, Mute);
		if (Mute == 0) std::cout << "write command 7 status = " << e << std::endl;

		char* pkt;
		e = read_command(&pkt, Mute);
		if (Mute == 0) {std::cout << "read command 7 status = " << e << std::endl;}
		if(false){
		char tmp[2];
		tmp[0] = patFile_to_pageID[i] & 0x00ff;
		tmp[1] = (patFile_to_pageID[i] & 0xff00)>>8;
		if (Mute == 0) std::cout << "DEBUG[commands.cpp] send_RAMPage, compare address bytes: "<<std::endl;
		if (Mute == 0) std::cout << HEX(tmp[0])<<" "<<HEX(pkt[2]&0xff)<<std::endl;
		if (Mute == 0) std::cout << HEX(tmp[1])<<" "<<HEX(pkt[3]&0xff - 0xd0)<<std::endl;
		if (Mute == 0) std::cout << DEC();
		}

		fclose(pat_files[i]);

	}

	pat_files.clear();


	eth_close();

	return true;		// ****************************************************
}

void Set::ReadEmuBoard()
{
	return;
}

void Set::DeleteCurrentSet()
{
	for (int i = 0; i < CSCConstants::NUM_DCFEBS; i++)
        {
        	std::stringstream ss;
                ss << default_dir << Prefix << "_cfeb" << i << "_tmb" << Tmb_type << ".pat";
                //pat_files.push_back( fopen(ss.str().c_str(), "r") );
                if(std::remove(ss.str().c_str())) std::cout << "Deleting File: " << ss.str() << std::endl;
	}
	for (int i=0; i < GEM_FIBERS; i++)
        {
                std::stringstream ss;
                ss << default_dir << Prefix << "_GEM"<<i<< "_tmb" << Tmb_type << ".pat";
                //pat_files.push_back( fopen(ss.str().c_str(), "r") );
                if(std::remove(ss.str().c_str())) std::cout << "Deleting File: " << ss.str() << std::endl;
        }


	return;
}

void Set::ClearEmuBoard(){
	return;
}


void Set::Dump(int Mute, char opt)
{
	char mode = 0x0;
	if(opt == 'c'){
		mode = 0xc;
	}
	else{
		mode = 0x0;
	}

	// open Device
	eth_open_device();
	eth_reset();

	int e = write_command(0xd, Mute);
	if (Mute == 0) std::cout << "write command d status = " << e << std::endl;

	char* pkt;
	e = read_command(&pkt, Mute);
	if (Mute == 0) cout << "read command d status = " << e << std::endl;

	if (Mute == 0) dumphex(e, pkt, &std::cout);

	eth_close();

	return;					// ****************************************************
}

void Set::ViewContents(std::ostream& oss)
{
  oss << "******************************************************\n"
            << "                   Muon Tracks in Set\n"
            << "******************************************************\n";
  for(int i=0; i < CSC.size(); i++){
    oss << '\n';
    PrintCLCT(CSC[i],oss,false);
  }

	oss << "\n" << "LUT DEFINED CLCT's::" << "\n";
	for(int i=0; i < LUT.size(); i++){
    oss << '\n';
    PrintCCLUT(LUT[i],oss,false);
  }

  oss << "\n******************************************************\n"
            << "******************************************************\n";
  return;
}

void Set::SaveResults(std::string opt_path)
{
	if(opt_path.empty()){
		opt_path = Prefix + ".log";
	}

	std::fstream ofs(opt_path.c_str(), std::ios_base::out);

	// Header
	{	// Record Time of Experiment
		time_t rawtime;
		struct tm * timeinfo;
		time( &rawtime );
		timeinfo = localtime( &rawtime );

		ofs << "Date: " << asctime(timeinfo);
		ofs << "TMB Compile Type: " << Tmb_type << std::endl;
		ofs << "Firmware Vers.: " << Firmware << std::endl << std::endl;
	}

	ofs << "# Trials  : \t\t" << N_trials << std::endl;
	ofs << "# Success : \t\t" << Stats[0] << std::endl;
	ofs << "# Errors  : \t\t" << N_trials - Stats[0] << std::endl << std::endl;

	int N_err = 0;
	for(int i=1; i < Stats.size(); i++){
		ofs << "Error " << i << ": " << Stats[i] << std::endl;
		N_err += Stats[i];
	}

	if(N_err != (N_trials - Stats[0])){
		ofs << "\nSomething Went Wrong!\nCorrect # Errors = " << N_err << std::endl;
	}

	ofs << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
	ofs << "~~~~~~~~~~Original Pattern Set~~~~~~~~~~~~~~~~\n";
	ofs << "prefix: " << Prefix << std::endl;
	ofs << "\nCSC\nbx\tkey\tpid\tNhit\t\t{ (bx,ly,hs) }\n";

	for(int i=0; i < CSC.size(); i++){
    		CSC[i].RelativeSpaceTime();
		ofs << CSC[i] << std::endl;
	}

	ofs << "\nGEM\nbx\troll\tpad\tsize\tlayer\n";
	for(int i=0; i < GEM.size(); i++){
		ofs << GEM[i] << std::endl;
	}

	ofs << "\nLUT\nbx\tkey\tpid\tCCODE\n";
	for(int i=0; i < LUT.size(); i++){
		LUT[i].RelativeSpaceTime();
		ofs << LUT[i] << std::endl;
	}

	ofs.close();
	return;
}

std::ostream& operator<<(std::ostream& oss, const Set& set){

	for(int i=0; i < set.CSC.size(); i++){
		oss << "<C " << set.CSC[i] << ">";	//	C ==> clct
	}
	for(int i=0; i < set.GEM.size(); i++){		//	G ==> gem cluster
		oss << "<G " << set.GEM[i] << ">";
	}
	for(int i=0; i < set.LUT.size(); i++){		//	G ==> gem cluster
		oss << "<G " << set.LUT[i] << ">";
	}
	for(int i=0; i < set.Results.size(); i++){	//	R ==> Expr. Results
		oss << "<R-" << i << " " << set.Results[i] << ">";
	}
	return oss;
}

}
