#ifndef drawPlotsUtils_h
#define drawPlotsUtils_h

#include "setTDRStyle.h"

#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TPaveStats.h"
#include "TLegend.h"
#include "TChain.h"
#include "TRandom.h"
#include "TVirtualFitter.h"
#include "TLatex.h"
#include "TProfile.h"
#include "TPaveLabel.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <ctime>
#include <map>
#include <algorithm>
#include <math.h>
#include <vector>


void compareHistos(TH1F* , TH1F* , std::string , std::string, std::string , std::string);

void compareNHistos(std::string input_histo_variables, const float x_min, const float x_max ,std::string xTitle , std::string outputName);

void compareProfile(TProfile*, TProfile*, std::string ,std::string, std::string, std::string , const float y_min, const float y_max, std::string , std::string);

void compareNProfiles(std::string, const float, const float, const float, const float, std::string, std::string, std::string);

void drawHisto(TH1F* , const float , const float , std::string , std::string , std::string, std::string  );

void drawProfile(TProfile*, const float , const float, const float , const float , std::string , std::string, std::string, std::string );

double myChiSquareTest(TH1F*);

#endif
