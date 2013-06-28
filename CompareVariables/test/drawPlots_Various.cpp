//g++ -Wall -o drawPlots_Various `root-config --cflags --glibs` setTDRStyle.cc drawPlotsUtils.cc PUreweightingUtils.cc drawPlots_Various.cpp

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
#include "TProfile.h"

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
  std::cout << "arcg: " << argc << std::endl;
  
  //Check if all nedeed arguments to parse are there
  if(argc != 2)
  {
    std::cerr << ">>>>> CompareVariables::usage: " << argv[0] << " configFileName" << std::endl ;
    return 1;
  }
 
  /// Parse the config file
  parseConfigFile (argv[1]) ;
  
  std::string inputListData = gConfigParser -> readStringOption("Input::inputListData");
  std::string inputListMC = gConfigParser -> readStringOption("Input::inputListMC");
  std::string inputTreeData = gConfigParser -> readStringOption("Input::inputTreeData");
  std::string inputTreeMC = gConfigParser -> readStringOption("Input::inputTreeMC");

  std::string inputPuRe = gConfigParser -> readStringOption("Input::inputPuRe");
  std::string inputPuReVar = gConfigParser -> readStringOption("Input::inputPuReVar");

  int inputRunIdCutMinDA = gConfigParser -> readIntOption("Input::inputRunIdCutMinDA");
  int inputRunIdCutMaxDA = gConfigParser -> readIntOption("Input::inputRunIdCutMaxDA");
  int inputRunIdCutMinMC = gConfigParser -> readIntOption("Input::inputRunIdCutMinMC");
  int inputRunIdCutMaxMC = gConfigParser -> readIntOption("Input::inputRunIdCutMaxMC");
  
  std::string inputLabelData = gConfigParser -> readStringOption("Input::inputLabelData");
  std::string inputLabelMC = gConfigParser -> readStringOption("Input::inputLabelMC");
  
  std::string inputCompType = gConfigParser -> readStringOption("Input::inputCompType");
  
  std::string outputDir = gConfigParser -> readStringOption("Output::outputDir");
  system(("mkdir "+outputDir).c_str());
  
  TPileupReweighting* puReweighting;
  bool isReweighted = gConfigParser -> readBoolOption("Input::isReweighted");
  if(isReweighted == true) puReweighting = new TPileupReweighting(inputPuRe.c_str(),inputPuReVar.c_str());
  
  TChain* ntu_DA = new TChain(inputTreeData.c_str());
  TChain* ntu_MC = new TChain(inputTreeMC.c_str());
  
  char trees[500];
  FILE *f_trees_DA;
  FILE *f_trees_MC;
  f_trees_DA = fopen(inputListData.c_str(),"r");
  f_trees_MC = fopen(inputListMC.c_str(),"r");

  while(fscanf(f_trees_DA,"%s \n", trees) !=EOF ){
    std::cout << "\nReading input_DATA: " << trees << std::endl;
    std::string TREES = std::string(trees);

    if(TREES.find("#") != std::string::npos) continue;
    
    ntu_DA -> Add(trees);
    
  }
  
  while(fscanf(f_trees_MC,"%s \n", trees) !=EOF ){
    std::cout << "\nReading input_MC: " << trees << std::endl;
    std::string TREES = std::string(trees);

    if(TREES.find("#") != std::string::npos) continue;

    ntu_MC -> Add(trees);
  
  }
   
  if (ntu_DA->GetEntries() == 0 )
  {
    std::cout << "Error: DATA file is empty" << std::endl; 
    return -1;
  }
   
  if (ntu_MC->GetEntries() == 0 )
  {
    std::cout << "Error: MC file is empty" << std::endl; 
    return -1;
  }
  
  float ele1_scLaserCorr, ele2_scLaserCorr;
  float ele1ele2_scM, ele1ele2_scM_regression;
  float ele1_scEta, ele2_scEta;
  float ele1_scE_regression, ele2_scE_regression;
  float ele1_scERaw, ele2_scERaw;
  float ele1_es, ele2_es;
  float ele1_e3x3, ele2_e3x3;
  float rho;
  int ele1_isEB;
  int ele2_isEB;
  float npu;
  int PV_n, runId;
  float PUit_TrueNumInteractions;
  int isZ;
  
  ntu_DA->SetBranchStatus("*",0);
  ntu_DA->SetBranchStatus("runId",1);
  ntu_DA->SetBranchStatus("rho",1);
  ntu_DA->SetBranchStatus("PV_n",1);
  ntu_DA->SetBranchStatus("isZ",1);
  ntu_DA->SetBranchStatus("ele1_isEB",1);
  ntu_DA->SetBranchStatus("ele2_isEB",1);
  ntu_DA->SetBranchStatus("ele1_scLaserCorr",1);
  ntu_DA->SetBranchStatus("ele2_scLaserCorr",1);
  ntu_DA->SetBranchStatus("ele1_scEta",1);
  ntu_DA->SetBranchStatus("ele2_scEta",1);
  ntu_DA->SetBranchStatus("ele1_scE_regression",1);
  ntu_DA->SetBranchStatus("ele2_scE_regression",1);
  ntu_DA->SetBranchStatus("ele1_scERaw",1);
  ntu_DA->SetBranchStatus("ele2_scERaw",1);
  ntu_DA->SetBranchStatus("ele1_e3x3",1);
  ntu_DA->SetBranchStatus("ele2_e3x3",1);
  ntu_DA->SetBranchStatus("ele1_es",1);
  ntu_DA->SetBranchStatus("ele2_es",1);
  ntu_DA->SetBranchStatus("ele1ele2_scM",1);
  ntu_DA->SetBranchStatus("ele1ele2_scM_regression",1);
  if(inputCompType == "MC_vs_MC"){
     ntu_DA->SetBranchStatus("PUit_TrueNumInteractions",1);
     ntu_DA->SetBranchAddress("PUit_TrueNumInteractions",&PUit_TrueNumInteractions);
  }
  
  ntu_MC->SetBranchStatus("runId",1);
  ntu_MC->SetBranchAddress("runId",&runId);
  ntu_MC->SetBranchStatus("rho",1);
  ntu_MC->SetBranchStatus("PV_n",1);
  ntu_MC->SetBranchStatus("isZ",1);
  ntu_MC->SetBranchStatus("ele1_isEB",1);
  ntu_MC->SetBranchStatus("ele2_isEB",1);
  ntu_MC->SetBranchStatus("ele1_scLaserCorr",1);
  ntu_MC->SetBranchStatus("ele2_scLaserCorr",1);
  ntu_MC->SetBranchStatus("ele1_scEta",1);
  ntu_MC->SetBranchStatus("ele2_scEta",1);
  ntu_MC->SetBranchStatus("ele1_scE_regression",1);
  ntu_MC->SetBranchStatus("ele2_scE_regression",1);
  ntu_MC->SetBranchStatus("ele1_scERaw",1);
  ntu_MC->SetBranchStatus("ele2_scERaw",1);
  ntu_MC->SetBranchStatus("ele1_e3x3",1);
  ntu_MC->SetBranchStatus("ele2_e3x3",1);
  ntu_MC->SetBranchStatus("ele1_es",1);
  ntu_MC->SetBranchStatus("ele2_es",1);
  ntu_MC->SetBranchStatus("ele1ele2_scM",1);
  ntu_MC->SetBranchStatus("ele1ele2_scM_regression",1);

  ntu_DA->SetBranchAddress("runId", &runId);
  ntu_DA->SetBranchAddress("rho", &rho);
  ntu_DA->SetBranchAddress("PV_n", &PV_n);
  ntu_DA->SetBranchAddress("isZ", &isZ);
  ntu_DA->SetBranchAddress("ele1_isEB", &ele1_isEB);
  ntu_DA->SetBranchAddress("ele2_isEB", &ele2_isEB);
  ntu_DA->SetBranchAddress("ele1_scLaserCorr", &ele1_scLaserCorr);
  ntu_DA->SetBranchAddress("ele2_scLaserCorr", &ele2_scLaserCorr);
  ntu_DA->SetBranchAddress("ele1_scEta", &ele1_scEta);
  ntu_DA->SetBranchAddress("ele2_scEta", &ele2_scEta);
  ntu_DA->SetBranchAddress("ele1_scE_regression", &ele1_scE_regression);
  ntu_DA->SetBranchAddress("ele2_scE_regression", &ele2_scE_regression);
  ntu_DA->SetBranchAddress("ele1_scERaw", &ele1_scERaw);
  ntu_DA->SetBranchAddress("ele2_scERaw", &ele2_scERaw);
  ntu_DA->SetBranchAddress("ele1_e3x3", &ele1_e3x3);
  ntu_DA->SetBranchAddress("ele2_e3x3", &ele2_e3x3);
  ntu_DA->SetBranchAddress("ele1_es", &ele1_es);
  ntu_DA->SetBranchAddress("ele2_es", &ele2_es);
  ntu_DA->SetBranchAddress("ele1ele2_scM", &ele1ele2_scM);
  ntu_DA->SetBranchAddress("ele1ele2_scM_regression", &ele1ele2_scM_regression);
  ntu_MC->SetBranchAddress("rho", &rho);
  ntu_MC->SetBranchAddress("PV_n", &PV_n);
  ntu_MC->SetBranchAddress("isZ", &isZ);
  ntu_MC->SetBranchAddress("ele1_isEB", &ele1_isEB);
  ntu_MC->SetBranchAddress("ele2_isEB", &ele2_isEB);
  ntu_MC->SetBranchAddress("ele1_scLaserCorr", &ele1_scLaserCorr);
  ntu_MC->SetBranchAddress("ele2_scLaserCorr", &ele2_scLaserCorr);
  ntu_MC->SetBranchAddress("ele1_scEta", &ele1_scEta);
  ntu_MC->SetBranchAddress("ele2_scEta", &ele2_scEta);
  ntu_MC->SetBranchAddress("ele1_scE_regression", &ele1_scE_regression);
  ntu_MC->SetBranchAddress("ele2_scE_regression", &ele2_scE_regression);
  ntu_MC->SetBranchAddress("ele1_scERaw", &ele1_scERaw);
  ntu_MC->SetBranchAddress("ele2_scERaw", &ele2_scERaw);
  ntu_MC->SetBranchAddress("ele1_e3x3", &ele1_e3x3);
  ntu_MC->SetBranchAddress("ele2_e3x3", &ele2_e3x3);
  ntu_MC->SetBranchAddress("ele1_es", &ele1_es);
  ntu_MC->SetBranchAddress("ele2_es", &ele2_es);
  ntu_MC->SetBranchAddress("ele1ele2_scM", &ele1ele2_scM);
  ntu_MC->SetBranchAddress("ele1ele2_scM_regression", &ele1ele2_scM_regression);
  if(isReweighted == true){
     ntu_MC->SetBranchStatus("PUit_TrueNumInteractions",1); 
     ntu_MC->SetBranchAddress("PUit_TrueNumInteractions", &npu);
  }
  if(inputCompType == "MC_vs_MC"){
     ntu_MC->SetBranchStatus("PUit_TrueNumInteractions",1);
     ntu_MC->SetBranchAddress("PUit_TrueNumInteractions",&PUit_TrueNumInteractions);
  }
  

  TProfile *h_LC_vs_scEta_DA = new TProfile("h_LC_vs_scEta_DA","h_LC_vs_scEta_DA",50, -3, 3);
  TProfile *h_LC_vs_scEta_MC = new TProfile("h_LC_vs_scEta_MC","h_LC_vs_scEta_MC",50, -3, 3);
  TProfile *h_scCorrection_vs_scEta_DA = new TProfile("h_scCorrection_vs_scEta_DA","h_scCorrection_vs_scEta_DA",50,-3,3);
  TProfile *h_scCorrection_vs_scEta_MC = new TProfile("h_scCorrection_vs_scEta_MC","h_scCorrection_vs_scEta_MC",50,-3,3);
  
  TH1F* h_rho_DA = new TH1F("DA_rho_total","",50 , 0 ,25 );
  TH1F* h_rho_MC = new TH1F("MC_rho_total","",50 , 0 ,25 );
  TH1F* h_rho_EBEB_DA = new TH1F("DA_rho_EBEB","",50 , 0 ,25 );
  TH1F* h_rho_EBEB_MC = new TH1F("MC_rho_EBEB","",50 , 0 ,25 );
  TH1F* h_rho_EEEE_DA = new TH1F("DA_rho_EEEE","",50 , 0 ,25 );
  TH1F* h_rho_EEEE_MC = new TH1F("MC_rho_EEEE","",50 , 0 ,25 );
  TH1F* h_rho_EBEE_DA = new TH1F("DA_rho_EBEE","",50 , 0 ,25 );
  TH1F* h_rho_EBEE_MC = new TH1F("MC_rho_EBEE","",50 , 0 ,25 );
  
  TH1F* h_PV_n_DA = new TH1F("DA_PV_n","",100 , -0.5 ,99.5);
  TH1F* h_PV_n_MC = new TH1F("MC_PV_n","",100 , -0.5 ,99.5 );
  TH1F* h_PUit_TrueNumInteractions_DA = new TH1F("DA_PUit_TrueNumInteractions","",60 , -0.5 ,59.5);
  TH1F* h_PUit_TrueNumInteractions_MC = new TH1F("MC_PUit_TrueNumInteractions","",60 , -0.5 ,59.5 );
  
  TH1F* h_R9_EB_DA = new TH1F("DA_R9_EB","",50, 0.5, 1);
  TH1F* h_R9_EB_MC = new TH1F("MC_R9_EB","",50, 0.5, 1);
  TH1F* h_R9_EE_DA = new TH1F("DA_R9_EE","",50, 0.5, 1);
  TH1F* h_R9_EE_MC = new TH1F("MC_R9_EE","",50, 0.5, 1);
  
  TH1F* h_ele1ele2_scM_total_DA = new TH1F("DA_ele1ele2_scM_total","",60 , 60.1 ,120 );
  TH1F* h_ele1ele2_scM_total_MC = new TH1F("MC_ele1ele2_scM_total","",60 , 60.1 ,120 );
  TH1F* h_ele1ele2_scM_EBEB_DA = new TH1F("DA_ele1ele2_scM_EBEB","",60 , 60.1 ,120 );
  TH1F* h_ele1ele2_scM_EBEB_MC = new TH1F("MC_ele1ele2_scM_EBEB","",60 , 60.1 ,120 );
  TH1F* h_ele1ele2_scM_EEEE_DA = new TH1F("DA_ele1ele2_scM_EEEE","",75 , 60.1 ,150 );
  TH1F* h_ele1ele2_scM_EEEE_MC = new TH1F("MC_ele1ele2_scM_EEEE","",75 , 60.1 ,150 );
  TH1F* h_ele1ele2_scM_EBEE_DA = new TH1F("DA_ele1ele2_scM_EBEE","",75 , 60.1 ,150 );
  TH1F* h_ele1ele2_scM_EBEE_MC = new TH1F("MC_ele1ele2_scM_EBEE","",75 , 60.1 ,150 );
  
  TH1F* h_ele1ele2_scM_regression_total_DA = new TH1F("DA_ele1ele2_scM_regression_total","",60 , 60.1 ,120 );
  TH1F* h_ele1ele2_scM_regression_total_MC = new TH1F("MC_ele1ele2_scM_regression_total","",60 , 60.1 ,120 );
  TH1F* h_ele1ele2_scM_regression_EBEB_DA = new TH1F("DA_ele1ele2_scM_regression_EBEB","",60 , 60.1 ,120 );
  TH1F* h_ele1ele2_scM_regression_EBEB_MC = new TH1F("MC_ele1ele2_scM_regression_EBEB","",60 , 60.1 ,120 );
  TH1F* h_ele1ele2_scM_regression_EEEE_DA = new TH1F("DA_ele1ele2_scM_regression_EEEE","",75 , 60.1 ,150 );
  TH1F* h_ele1ele2_scM_regression_EEEE_MC = new TH1F("MC_ele1ele2_scM_regression_EEEE","",75 , 60.1 ,150 );
  TH1F* h_ele1ele2_scM_regression_EBEE_DA = new TH1F("DA_ele1ele2_scM_regression_EBEE","",75 , 60.1 ,150 );
  TH1F* h_ele1ele2_scM_regression_EBEE_MC = new TH1F("MC_ele1ele2_scM_regression_EBEE","",75 , 60.1 ,150 );
  
  for(int entry = 0; entry < ntu_DA->GetEntries(); entry++){
     if(entry%100000==0) std::cout<<"--- Reading DATA_entry = "<<entry<<std::endl;
     ntu_DA->GetEntry(entry);
     
     if(runId < inputRunIdCutMinDA) continue;
     if(runId > inputRunIdCutMaxDA) continue;
     
     if(isZ == 0) continue;
     
     h_rho_DA->Fill(rho);
     h_PV_n_DA->Fill(PV_n);
     if(inputCompType == "MC_vs_MC") h_PUit_TrueNumInteractions_DA->Fill(PUit_TrueNumInteractions);
     
     h_ele1ele2_scM_total_DA->Fill(ele1ele2_scM);
     h_ele1ele2_scM_regression_total_DA->Fill(ele1ele2_scM_regression);

     if(ele1_isEB == 1 && ele2_isEB == 1 ){

        h_ele1ele2_scM_EBEB_DA->Fill(ele1ele2_scM);
        h_ele1ele2_scM_regression_EBEB_DA->Fill(ele1ele2_scM_regression);

        h_rho_EBEB_DA->Fill(rho); 
     }
     
     if(ele1_isEB == 0 && ele2_isEB == 0 ){

        h_ele1ele2_scM_EEEE_DA->Fill(ele1ele2_scM);
        h_ele1ele2_scM_regression_EEEE_DA->Fill(ele1ele2_scM_regression);

        h_rho_EEEE_DA->Fill(rho); 
     }

     if((ele1_isEB == 1 && ele2_isEB == 0) || (ele1_isEB == 0 && ele2_isEB == 1) ){

        h_ele1ele2_scM_EBEE_DA->Fill(ele1ele2_scM);
        h_ele1ele2_scM_regression_EBEE_DA->Fill(ele1ele2_scM_regression);
       
        h_rho_EBEE_DA->Fill(rho); 
     }
     
     float R9_1 = ele1_e3x3/ele1_scERaw;
     float R9_2 = ele2_e3x3/ele2_scERaw;
     
     if(ele1_isEB == 1) h_R9_EB_DA -> Fill(R9_1);
     if(ele2_isEB == 1) h_R9_EB_DA -> Fill(R9_2);
     
     if(ele1_isEB == 0) h_R9_EE_DA -> Fill(R9_1);
     if(ele2_isEB == 0) h_R9_EE_DA -> Fill(R9_2);
     
     h_LC_vs_scEta_DA ->Fill(ele1_scEta, ele1_scLaserCorr);
     h_LC_vs_scEta_DA ->Fill(ele2_scEta, ele2_scLaserCorr);
     
     float scCorrection_1 = ele1_scE_regression/(ele1_scERaw+ele1_es);
     float scCorrection_2 = ele2_scE_regression/(ele2_scERaw+ele2_es);

     h_scCorrection_vs_scEta_DA->Fill(ele1_scEta, scCorrection_1);
     h_scCorrection_vs_scEta_DA->Fill(ele2_scEta, scCorrection_2);
     
  }

  for(int entry = 0; entry < ntu_MC->GetEntries(); entry++){
     if(entry%100000==0) std::cout<<"--- Reading MC_entry = "<<entry<<std::endl;
     ntu_MC->GetEntry(entry);

     if(runId < inputRunIdCutMinMC) continue;
     if(runId > inputRunIdCutMaxMC) continue;
     
     if(isZ == 0) continue;
     
     float puRe = 1.;
     
     if(isReweighted == true) puRe = puReweighting->GetWeight(int(npu));
     
     //puRe = 1;
    
     h_rho_MC->Fill(rho, puRe);
     h_PV_n_MC->Fill(PV_n, puRe);

     if(inputCompType == "MC_vs_MC") h_PUit_TrueNumInteractions_MC->Fill(PUit_TrueNumInteractions);
     
     h_ele1ele2_scM_total_MC->Fill(ele1ele2_scM, puRe);
     h_ele1ele2_scM_regression_total_MC->Fill(ele1ele2_scM_regression, puRe);

     if(ele1_isEB == 1 && ele2_isEB == 1 ){

        h_ele1ele2_scM_EBEB_MC->Fill(ele1ele2_scM, puRe);
        h_ele1ele2_scM_regression_EBEB_MC->Fill(ele1ele2_scM_regression, puRe);
        
        h_rho_EBEB_MC->Fill(rho); 
     }
     
     if(ele1_isEB == 0 && ele2_isEB == 0 ){

        h_ele1ele2_scM_EEEE_MC->Fill(ele1ele2_scM, puRe);
        h_ele1ele2_scM_regression_EEEE_MC->Fill(ele1ele2_scM_regression, puRe);
   
        h_rho_EEEE_MC->Fill(rho); 
     }

     if((ele1_isEB == 1 && ele2_isEB == 0) || (ele1_isEB == 0 && ele2_isEB == 1) ){

        h_ele1ele2_scM_EBEE_MC->Fill(ele1ele2_scM, puRe);
        h_ele1ele2_scM_regression_EBEE_MC->Fill(ele1ele2_scM_regression, puRe);
          
        h_rho_EBEE_MC->Fill(rho); 
     }

     float R9_1 = ele1_e3x3/ele1_scERaw;
     float R9_2 = ele2_e3x3/ele2_scERaw;
     
     if(ele1_isEB == 1) h_R9_EB_MC -> Fill(R9_1, puRe);
     if(ele2_isEB == 1) h_R9_EB_MC -> Fill(R9_2, puRe);
     
     if(ele1_isEB == 0) h_R9_EE_MC -> Fill(R9_1, puRe);
     if(ele2_isEB == 0) h_R9_EE_MC -> Fill(R9_2, puRe);
     
     h_LC_vs_scEta_MC ->Fill(ele1_scEta, ele1_scLaserCorr, puRe);
     h_LC_vs_scEta_MC ->Fill(ele2_scEta, ele2_scLaserCorr, puRe);
     
     float scCorrection_1 = ele1_scE_regression/(ele1_scERaw+ele1_es);
     float scCorrection_2 = ele2_scE_regression/(ele2_scERaw+ele2_es);

     h_scCorrection_vs_scEta_MC->Fill(ele1_scEta, scCorrection_1, puRe);
     h_scCorrection_vs_scEta_MC->Fill(ele2_scEta, scCorrection_2, puRe);
     
  }

  std::string label_DA = inputLabelData;
  std::string label_MC = inputLabelMC;
  std::string comp_type = inputCompType;
  
  compareHistos(h_rho_DA, h_rho_MC, label_DA, label_MC, comp_type , outputDir);
  compareHistos(h_rho_EBEB_DA, h_rho_EBEB_MC, label_DA, label_MC, comp_type , outputDir);
  compareHistos(h_rho_EEEE_DA, h_rho_EEEE_MC, label_DA, label_MC, comp_type , outputDir);
  compareHistos(h_rho_EBEE_DA, h_rho_EBEE_MC, label_DA, label_MC, comp_type , outputDir);
  
  if(inputCompType == "MC_vs_MC") compareHistos(h_PUit_TrueNumInteractions_DA, h_PUit_TrueNumInteractions_MC, label_DA, label_MC, comp_type , outputDir);
  
  compareHistos(h_PV_n_DA, h_PV_n_MC, label_DA, label_MC, comp_type , outputDir);
  
  compareHistos(h_R9_EB_DA, h_R9_EB_MC, label_DA, label_MC, comp_type , outputDir);
  compareHistos(h_R9_EE_DA, h_R9_EE_MC, label_DA, label_MC, comp_type , outputDir);
  
  compareHistos(h_ele1ele2_scM_total_DA, h_ele1ele2_scM_total_MC, label_DA, label_MC, comp_type , outputDir);
  compareHistos(h_ele1ele2_scM_EBEB_DA, h_ele1ele2_scM_EBEB_MC, label_DA, label_MC, comp_type , outputDir);
  compareHistos(h_ele1ele2_scM_EEEE_DA, h_ele1ele2_scM_EEEE_MC, label_DA, label_MC, comp_type , outputDir);
  compareHistos(h_ele1ele2_scM_EBEE_DA, h_ele1ele2_scM_EBEE_MC,label_DA, label_MC, comp_type , outputDir);
   
  compareHistos(h_ele1ele2_scM_regression_total_DA, h_ele1ele2_scM_regression_total_MC,label_DA, label_MC, comp_type , outputDir);
  compareHistos(h_ele1ele2_scM_regression_EBEB_DA, h_ele1ele2_scM_regression_EBEB_MC,label_DA, label_MC, comp_type , outputDir);
  compareHistos(h_ele1ele2_scM_regression_EEEE_DA, h_ele1ele2_scM_regression_EEEE_MC,label_DA, label_MC, comp_type , outputDir);
  compareHistos(h_ele1ele2_scM_regression_EBEE_DA, h_ele1ele2_scM_regression_EBEE_MC,label_DA, label_MC, comp_type , outputDir);

  std::string yTitle = std::string("scE_regression/(scERaw + E_ES)");
  
  //drawProfile(h_LC_vs_scEta, 0, 0, 1., 1.9, std::string("LCD") , std::string("scEta"), std::string("scLaserCorr"), outputDir);
  
  //drawProfile(h_scCorrection_vs_scEta_DA, 0, 0, 0.98, 1.1, std::string("LCD") , std::string("scEta"), yTitle, outputDir);
  //drawProfile(h_scCorrection_vs_scEta_MC, 0, 0, 0.98, 1.1, std::string("LC1") , std::string("scEta"), yTitle, outputDir);
  
  compareProfile(h_LC_vs_scEta_DA, h_LC_vs_scEta_MC, std::string("scEta"), std::string("scLaserCorr") , label_DA, label_MC, 0.9, 1.9, comp_type , outputDir);
   
  compareProfile(h_scCorrection_vs_scEta_DA, h_scCorrection_vs_scEta_MC, std::string("scEta"), yTitle, label_DA, label_MC, 0.98, 1.1, comp_type , outputDir);
  
  TFile* o = new TFile((outputDir+"compareVariables_Various.root").c_str(), "RECREATE" );
   
  o->cd();

  h_rho_DA->Write();
  h_rho_MC->Write();
  h_rho_EBEB_DA->Write();
  h_rho_EBEB_MC->Write();
  h_rho_EEEE_DA->Write();
  h_rho_EEEE_MC->Write();
  h_rho_EBEE_DA->Write();
  h_rho_EBEE_MC->Write();
  h_PV_n_DA->Write();
  h_PV_n_MC->Write();
  h_R9_EB_DA -> Write();
  h_R9_EE_DA -> Write();
  h_R9_EB_MC -> Write();
  h_R9_EE_MC -> Write();
  h_ele1ele2_scM_total_DA->Write();
  h_ele1ele2_scM_total_MC->Write();
  h_ele1ele2_scM_EBEB_DA->Write();
  h_ele1ele2_scM_EBEB_MC->Write();
  h_ele1ele2_scM_EEEE_DA->Write();
  h_ele1ele2_scM_EEEE_MC->Write();
  h_ele1ele2_scM_EBEE_DA->Write();
  h_ele1ele2_scM_EBEE_MC->Write();
  h_ele1ele2_scM_regression_total_DA->Write();
  h_ele1ele2_scM_regression_total_MC->Write();
  h_ele1ele2_scM_regression_EBEB_DA->Write();
  h_ele1ele2_scM_regression_EBEB_MC->Write();
  h_ele1ele2_scM_regression_EEEE_DA->Write();
  h_ele1ele2_scM_regression_EEEE_MC->Write();
  h_ele1ele2_scM_regression_EBEE_DA->Write();
  h_ele1ele2_scM_regression_EBEE_MC->Write();
  h_LC_vs_scEta_DA->Write();
  h_LC_vs_scEta_MC->Write();
  h_scCorrection_vs_scEta_DA->Write();
  h_scCorrection_vs_scEta_MC->Write();
  
  o->Close();
  
}


















