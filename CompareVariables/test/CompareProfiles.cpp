#include "setTDRStyle.h"
#include "PUreweightingUtils.h"
#include "drawPlotsUtils.h"
#include "ConfigParser.h"
#include "ParserUtils.h"

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

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <ctime>
#include <map>
#include <algorithm>
#include <math.h>
#include <vector>

int main(int argc, char** argv)
{
  // Set style options
  setTDRStyle();
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetOptTitle(0); 
  gStyle->SetOptStat(1110); 
  //gStyle->SetOptStat(0000); 
  gStyle->SetOptFit(1); 

  // Input parameters
  
  std::cout << "\n*******************************************************************************************************************" << std::endl;
  std::cout << "argc: " << argc << std::endl;

  //Check if all nedeed arguments to parse are there
  if(argc != 2)
  {
    std::cerr << ">>>>> CompareVariables::usage: " << argv[0] << " configFileName" << std::endl ;
    return 1;
  }

  /// Parse the config file
  parseConfigFile (argv[1]) ;
  
  std::string inputHistoVariables = gConfigParser -> readStringOption("Input::inputProfileVariables");
  
  float XMin = gConfigParser -> readFloatOption("Input::XMin");
  float XMax = gConfigParser -> readFloatOption("Input::XMax");
   
  float YMin = gConfigParser -> readFloatOption("Input::YMin");
  float YMax = gConfigParser -> readFloatOption("Input::YMax");
   
  std::string xTitle = gConfigParser -> readStringOption("Input::xTitle");
  std::string yTitle = gConfigParser -> readStringOption("Input::yTitle");

  std::string outputName = gConfigParser -> readStringOption("Output::outputName");
   
  compareNProfiles(inputHistoVariables,XMin,XMax,YMin,YMax,xTitle,yTitle,outputName);

}
