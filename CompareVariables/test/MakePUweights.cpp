//g++ -Wall -o MakePUweights `root-config --cflags --glibs` PUreweightingUtils.cc MakePUweights.cpp

#include "ConfigParser.h"
#include "ParserUtils.h"
#include "PUreweightingUtils.h"

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



int main(int argc, char** argv)
{

  std::cout << "\n*******************************************************************************************************************" << std::endl;
  std::cout << "arcg: " << argc << std::endl;
  
  /// Parse the config file
  parseConfigFile (argv[1]) ;
  
  std::string inputTree = gConfigParser -> readStringOption("Input::inputTree");
  std::string inputRootFile = gConfigParser -> readStringOption("Input::inputRootFile");
  std::string inputBranch = gConfigParser -> readStringOption("Input::inputBranch");

  std::string outputNameMC = gConfigParser -> readStringOption("Output::outputNameMC");
  
  std::string inputFileDA = gConfigParser -> readStringOption("Input::inputFileDA");
  std::string inputFileMC = gConfigParser -> readStringOption("Input::inputFileMC");

  std::string inputHistoNameDA = gConfigParser -> readStringOption("Input::inputHistoNameDA");
  std::string inputHistoNameMC = gConfigParser -> readStringOption("Input::inputHistoNameMC");
  
  std::string outputNamePUre = gConfigParser -> readStringOption("Output::outputNamePUre");
  
  makePUDistributionMC(inputTree, inputRootFile, inputBranch, outputNameMC ); 
  
  ComputePUweights(inputFileDA, inputFileMC, inputHistoNameDA, inputHistoNameMC, outputNamePUre);
}


