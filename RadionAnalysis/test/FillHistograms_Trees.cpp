
#include "PUreweightingUtils.h"
#include "ConfigParser.h"
#include "ParserUtils.h"
#include "setTDRStyle.h"
#include "drawPlotsUtils.h"

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
#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "THStack.h"
#include "TPad.h"

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
  if(argc <= 2)
  {
    std::cerr << ">>>>> Program usage: " << argv[0] << " configFileName btagWP" << std::endl ;
    return 1;
  }
 
  /// Parse the config file
  parseConfigFile (argv[1]) ;
  
  std::string inputList = gConfigParser -> readStringOption("Input::inputList");
  std::string inputTree = gConfigParser -> readStringOption("Input::inputTree");
  
  std::string outputName = gConfigParser -> readStringOption("Output::outputName");
  
  bool isReweighted = gConfigParser -> readBoolOption("Input::isReweighted");
  std::string inputMCPUFile = gConfigParser -> readStringOption("Input::inputMCPUFile");
  std::string inputMCPUHisto = gConfigParser -> readStringOption("Input::inputMCPUHisto");
  std::string inputDataPUFile = gConfigParser -> readStringOption("Input::inputDataPUFile");
  std::string inputDataPUHisto = gConfigParser -> readStringOption("Input::inputDataPUHisto");

  char* btagWP = argv[2];
   
  std::cout << "btagWP: " << btagWP << std::endl;

  if ( strcmp(btagWP,"No")!=0 && strcmp(btagWP,"Loose")!=0 && strcmp(btagWP,"Medium")!=0 && strcmp(btagWP,"Tight")!=0   )
  {
    std::cout << "CHK-STB Error: unknown  btag working point " << btagWP << std::endl;
    std::cout << "CHK-STB Select among No, Loose, Medium and Tight! " << std::endl;
    return -1;
  }

  float btag_wp = -1;
  
  if ( strcmp(btagWP,"No") == 0 ) btag_wp = 0.;
  if ( strcmp(btagWP,"Loose") == 0 ) btag_wp = 0.244;
  if ( strcmp(btagWP,"Medium") == 0 ) btag_wp = 0.679;
  if ( strcmp(btagWP,"Tight") == 0 ) btag_wp = 0.898;

  std::map<int,TChain*> ntu;
  std::map<int, TFile*> Files;

  TFile* FileDataPU;
  TH1F* h_DA_PU;

  std::map<int, TH1F*> h_MC_PU;
  std::map<int, TH1F*> h_PU_Weight;
  std::map<int, std::map<int,float> > weight;
   
  char trees[500];
  FILE *f_trees;
  f_trees = fopen(inputList.c_str(),"r");
  int pos = 0;
  int pos_total = 0;
  
  while(fscanf(f_trees,"%s \n", trees) !=EOF ){
    std::string TREES = std::string(trees);
 
    if(TREES.find("#") != std::string::npos) continue;
    std::cout << "\nReading input: " << trees << std::endl;
    ntu[pos] = new TChain(inputTree.c_str());
    ntu[pos] -> Add(trees);
   
    if(isReweighted == true){
       //TREES.erase(0,23);
       Files[pos] = TFile::Open(TREES.c_str());
       h_MC_PU[pos] =(TH1F*)Files[pos]->Get(inputMCPUHisto.c_str());
    }
    pos++;
    
  }
  
  pos_total = pos;
  
  for(int ii = 0; ii < pos_total; ii++){
      if(ntu[ii]->GetEntries() == 0 )
      {
         std::cout << "Error: input file" << ii << " is empty" << std::endl; 
         return -1;
      }
  }
  
  TFile* PU_file = TFile::Open(inputMCPUFile.c_str());

  for(int ii = 0; ii < pos_total; ii++)
      h_MC_PU[ii] =(TH1F*)PU_file->Get(inputMCPUHisto.c_str());
   
  if(isReweighted == true){
     
     FileDataPU = new TFile(inputDataPUFile.c_str(),"READ");
     h_DA_PU = (TH1F*)FileDataPU->Get(inputDataPUHisto.c_str()); 
     
     TH1F* h_da = new TH1F("h_da","",100,-0.5,99.5);
     for(int bin = 1; bin <= h_DA_PU->GetNbinsX(); ++bin)
     {
         float binCenter  = h_DA_PU -> GetBinCenter(bin) - 0.5;
         float binContent = h_DA_PU -> GetBinContent(bin);
         //std::cout << "DATA: bin: " << bin << "   binCenter: " << binCenter << "   binContent: " << binContent << std::endl;
         h_da -> Fill(binCenter,binContent);
     }
     h_da -> Scale(1./h_da->Integral());
     //std::cout << "Integral DATA: " << h_da -> Integral() << std::endl;

     TH1F* h_mc;
     for(int ii = 0; ii < pos_total; ii++){
         h_mc = new TH1F("h_mc","",100,-0.5,99.5);
         for(int bin = 1; bin <= h_MC_PU[ii]->GetNbinsX(); ++bin)
         {
             float binCenter  = h_MC_PU[ii] -> GetBinCenter(bin)-0.5;
             float binContent = h_MC_PU[ii] -> GetBinContent(bin);
             //std::cout << "MC-" << ii << " bin: " << bin << "   binCenter: " << binCenter << "   binContent: " << binContent << std::endl;
    
             h_mc -> Fill(binCenter,binContent);
         }
         h_mc -> Scale(1./h_mc->Integral());
         //std::cout << "Integral MC-" << ii << ": " << h_mc -> Integral() << std::endl;
         
         h_PU_Weight[ii] = (TH1F*)h_da->Clone();
         h_PU_Weight[ii]->Divide(h_mc);

         for(int bin = 1; bin <= h_PU_Weight[ii]->GetNbinsX(); ++bin)
         weight[ii][bin-1] = h_PU_Weight[ii]->GetBinContent(bin);
         
         delete h_mc;
     }
    
  }
  
  float         pu_n;
  float         weight_an;
  float         evweight;
  float         pu_weight;
  float         ph1_e;
  float         ph2_e;
  float         ph1_pt;
  float         ph2_pt;
  float         ph1_phi;
  float         ph2_phi;
  float         ph1_eta;
  float         ph2_eta;
  float         ph1_r9;
  float         ph2_r9;
  float         ph1_hoe;
  float         ph2_hoe;
  float         ph1_sieie;
  float         ph2_sieie;
  float         ph1_sieip;
  float         ph2_sieip;
  float         ph1_sipip;
  float         ph2_sipip;
  float         ph1_isconv;
  float         ph2_isconv;
  float         PhotonsMass;
  float         dipho_E;
  float         dipho_pt;
  float         dipho_eta;
  float         dipho_phi;
  float         dipho_cosThetaStar_CS;
  float         dipho_tanhYStar;
  float         dipho_Y;
  int           vtx_ind;
  float         j1_e;
  float         j1_pt;
  float         j1_phi;
  float         j1_eta;
  float         j1_betaStarClassic;
  float         j1_csvBtag;
  float         j1_csvMvaBtag;
  float         j1_jetProbBtag;
  float         j1_tcheBtag;
  float         j1_radionMatched;
  float         j2_e;
  float         j2_pt;
  float         j2_phi;
  float         j2_eta;
  float         j2_betaStarClassic;
  float         j2_csvBtag;
  float         j2_csvMvaBtag;
  float         j2_jetProbBtag;
  float         j2_tcheBtag;
  float         j2_radionMatched;
  float         j3_e;
  float         j3_pt;
  float         j3_phi;
  float         j3_eta;
  float         j3_betaStarClassic;
  float         j3_csvBtag;
  float         j3_csvMvaBtag;
  float         j3_jetProbBtag;
  float         j3_tcheBtag;
  float         j3_radionMatched;
  float         j4_e;
  float         j4_pt;
  float         j4_phi;
  float         j4_eta;
  float         j4_betaStarClassic;
  float         j4_csvBtag;
  float         j4_csvMvaBtag;
  float         j4_jetProbBtag;
  float         j4_tcheBtag;
  float         j4_radionMatched;
  float         JetsMass;
  float         dijet_E;
  float         dijet_pt;
  float         dijet_eta;
  float         dijet_phi;
  float         RadMass;
  float         radion_E;
  float         radion_pt;
  float         radion_eta;
  float         radion_phi;
  
  for(int ii = 0; ii < pos_total; ii++){
      ntu[ii]->SetBranchStatus("*",0);
      ntu[ii]->SetBranchStatus("pu_n",1);
      ntu[ii]->SetBranchStatus("weight",1);
      ntu[ii]->SetBranchStatus("evweight",1);
      ntu[ii]->SetBranchStatus("pu_weight",1);
      ntu[ii]->SetBranchStatus("ph1_e",1);
      ntu[ii]->SetBranchStatus("ph2_e",1);
      ntu[ii]->SetBranchStatus("ph1_pt",1);
      ntu[ii]->SetBranchStatus("ph2_pt",1);
      ntu[ii]->SetBranchStatus("ph1_phi",1);
      ntu[ii]->SetBranchStatus("ph2_phi",1);
      ntu[ii]->SetBranchStatus("ph1_eta",1);
      ntu[ii]->SetBranchStatus("ph2_eta",1);
      ntu[ii]->SetBranchStatus("ph1_r9",1);
      ntu[ii]->SetBranchStatus("ph2_r9",1);
      ntu[ii]->SetBranchStatus("ph1_hoe",1);
      ntu[ii]->SetBranchStatus("ph2_hoe",1);
      ntu[ii]->SetBranchStatus("ph1_sieie",1);
      ntu[ii]->SetBranchStatus("ph2_sieie",1);
      ntu[ii]->SetBranchStatus("ph1_sieip",1);
      ntu[ii]->SetBranchStatus("ph2_sieip",1);
      ntu[ii]->SetBranchStatus("ph1_sipip",1);
      ntu[ii]->SetBranchStatus("ph2_sipip",1);
      ntu[ii]->SetBranchStatus("PhotonsMass",1);
      ntu[ii]->SetBranchStatus("dipho_E",1);
      ntu[ii]->SetBranchStatus("dipho_pt",1);
      ntu[ii]->SetBranchStatus("dipho_eta",1);
      ntu[ii]->SetBranchStatus("dipho_phi",1);
      ntu[ii]->SetBranchStatus("dipho_cosThetaStar_CS",1);
      ntu[ii]->SetBranchStatus("dipho_tanhYStar",1);
      ntu[ii]->SetBranchStatus("dipho_Y",1);
      ntu[ii]->SetBranchStatus("vtx_ind",1);
      ntu[ii]->SetBranchStatus("j1_e",1);
      ntu[ii]->SetBranchStatus("j1_pt",1);
      ntu[ii]->SetBranchStatus("j1_phi",1);
      ntu[ii]->SetBranchStatus("j1_eta",1);
      ntu[ii]->SetBranchStatus("j1_betaStarClassic",1);
      ntu[ii]->SetBranchStatus("j1_csvBtag",1);
      ntu[ii]->SetBranchStatus("j1_csvMvaBtag",1);
      ntu[ii]->SetBranchStatus("j1_jetProbBtag",1);
      ntu[ii]->SetBranchStatus("j1_tcheBtag",1);
      ntu[ii]->SetBranchStatus("j1_radionMatched",1);
      ntu[ii]->SetBranchStatus("j2_e",1);
      ntu[ii]->SetBranchStatus("j2_pt",1);
      ntu[ii]->SetBranchStatus("j2_phi",1);
      ntu[ii]->SetBranchStatus("j2_eta",1);
      ntu[ii]->SetBranchStatus("j2_betaStarClassic",1);
      ntu[ii]->SetBranchStatus("j2_csvBtag",1);
      ntu[ii]->SetBranchStatus("j2_csvMvaBtag",1);
      ntu[ii]->SetBranchStatus("j2_jetProbBtag",1);
      ntu[ii]->SetBranchStatus("j2_tcheBtag",1);
      ntu[ii]->SetBranchStatus("j2_radionMatched",1);
      ntu[ii]->SetBranchStatus("j3_e",1);
      ntu[ii]->SetBranchStatus("j3_pt",1);
      ntu[ii]->SetBranchStatus("j3_phi",1);
      ntu[ii]->SetBranchStatus("j3_eta",1);
      ntu[ii]->SetBranchStatus("j3_betaStarClassic",1);
      ntu[ii]->SetBranchStatus("j3_csvBtag",1);
      ntu[ii]->SetBranchStatus("j3_csvMvaBtag",1);
      ntu[ii]->SetBranchStatus("j3_jetProbBtag",1);
      ntu[ii]->SetBranchStatus("j3_tcheBtag",1);
      ntu[ii]->SetBranchStatus("j3_radionMatched",1);
      ntu[ii]->SetBranchStatus("j4_e",1);
      ntu[ii]->SetBranchStatus("j4_pt",1);
      ntu[ii]->SetBranchStatus("j4_phi",1);
      ntu[ii]->SetBranchStatus("j4_eta",1);
      ntu[ii]->SetBranchStatus("j4_betaStarClassic",1);
      ntu[ii]->SetBranchStatus("j4_csvBtag",1);
      ntu[ii]->SetBranchStatus("j4_csvMvaBtag",1);
      ntu[ii]->SetBranchStatus("j4_jetProbBtag",1);
      ntu[ii]->SetBranchStatus("j4_tcheBtag",1);
      ntu[ii]->SetBranchStatus("j4_radionMatched",1);
      ntu[ii]->SetBranchStatus("JetsMass",1);
      ntu[ii]->SetBranchStatus("dijet_E",1);
      ntu[ii]->SetBranchStatus("dijet_pt",1);
      ntu[ii]->SetBranchStatus("dijet_phi",1);
      ntu[ii]->SetBranchStatus("dijet_eta",1);
      ntu[ii]->SetBranchStatus("radion_E",1);
      ntu[ii]->SetBranchStatus("radion_pt",1);
      ntu[ii]->SetBranchStatus("radion_eta",1);
      ntu[ii]->SetBranchStatus("radion_phi",1);
      ntu[ii]->SetBranchAddress("pu_n",&pu_n);
      ntu[ii]->SetBranchAddress("weight",&weight_an);
      ntu[ii]->SetBranchAddress("evweight",&evweight);
      ntu[ii]->SetBranchAddress("pu_weight",&pu_weight);
      ntu[ii]->SetBranchAddress("ph1_e",&ph1_e);
      ntu[ii]->SetBranchAddress("ph2_e",&ph2_e);
      ntu[ii]->SetBranchAddress("ph1_pt",&ph1_pt);
      ntu[ii]->SetBranchAddress("ph2_pt",&ph2_pt);
      ntu[ii]->SetBranchAddress("ph1_phi",&ph1_phi);
      ntu[ii]->SetBranchAddress("ph2_phi",&ph2_phi);
      ntu[ii]->SetBranchAddress("ph1_eta",&ph1_eta);
      ntu[ii]->SetBranchAddress("ph2_eta",&ph2_eta);
      ntu[ii]->SetBranchAddress("ph1_r9",&ph1_r9);
      ntu[ii]->SetBranchAddress("ph2_r9",&ph2_r9);
      ntu[ii]->SetBranchAddress("ph1_hoe",&ph1_hoe);
      ntu[ii]->SetBranchAddress("ph2_hoe",&ph2_hoe);
      ntu[ii]->SetBranchAddress("ph1_sieie",&ph1_sieie);
      ntu[ii]->SetBranchAddress("ph2_sieie",&ph2_sieie);
      ntu[ii]->SetBranchAddress("ph1_sieip",&ph1_sieip);
      ntu[ii]->SetBranchAddress("ph2_sieip",&ph2_sieip);
      ntu[ii]->SetBranchAddress("ph1_sipip",&ph1_sipip);
      ntu[ii]->SetBranchAddress("ph2_sipip",&ph2_sipip);
      ntu[ii]->SetBranchAddress("PhotonsMass",&PhotonsMass);
      ntu[ii]->SetBranchAddress("dipho_E",&dipho_E);
      ntu[ii]->SetBranchAddress("dipho_pt",&dipho_pt);
      ntu[ii]->SetBranchAddress("dipho_eta",&dipho_eta);
      ntu[ii]->SetBranchAddress("dipho_phi",&dipho_phi);
      ntu[ii]->SetBranchAddress("dipho_cosThetaStar_CS",&dipho_cosThetaStar_CS);
      ntu[ii]->SetBranchAddress("dipho_tanhYStar",&dipho_tanhYStar);
      ntu[ii]->SetBranchAddress("dipho_Y",&dipho_Y);
      ntu[ii]->SetBranchAddress("vtx_ind",&vtx_ind);
      ntu[ii]->SetBranchAddress("j1_e",&j1_e);
      ntu[ii]->SetBranchAddress("j1_pt",&j1_pt);
      ntu[ii]->SetBranchAddress("j1_phi",&j1_phi);
      ntu[ii]->SetBranchAddress("j1_eta",&j1_eta);
      ntu[ii]->SetBranchAddress("j1_betaStarClassic",&j1_betaStarClassic);
      ntu[ii]->SetBranchAddress("j1_csvBtag",&j1_csvBtag);
      ntu[ii]->SetBranchAddress("j1_csvMvaBtag",&j1_csvMvaBtag);
      ntu[ii]->SetBranchAddress("j1_jetProbBtag",&j1_jetProbBtag);
      ntu[ii]->SetBranchAddress("j1_tcheBtag",&j1_tcheBtag);
      ntu[ii]->SetBranchAddress("j1_radionMatched",&j1_radionMatched);
      ntu[ii]->SetBranchAddress("j2_e",&j2_e);
      ntu[ii]->SetBranchAddress("j2_pt",&j2_pt);
      ntu[ii]->SetBranchAddress("j2_phi",&j2_phi);
      ntu[ii]->SetBranchAddress("j2_eta",&j2_eta);
      ntu[ii]->SetBranchAddress("j2_betaStarClassic",&j2_betaStarClassic);
      ntu[ii]->SetBranchAddress("j2_csvBtag",&j2_csvBtag);
      ntu[ii]->SetBranchAddress("j2_csvMvaBtag",&j2_csvMvaBtag);
      ntu[ii]->SetBranchAddress("j2_jetProbBtag",&j2_jetProbBtag);
      ntu[ii]->SetBranchAddress("j2_tcheBtag",&j2_tcheBtag);
      ntu[ii]->SetBranchAddress("j2_radionMatched",&j2_radionMatched);
      ntu[ii]->SetBranchAddress("j3_e",&j3_e);
      ntu[ii]->SetBranchAddress("j3_pt",&j3_pt);
      ntu[ii]->SetBranchAddress("j3_phi",&j3_phi);
      ntu[ii]->SetBranchAddress("j3_eta",&j3_eta);
      ntu[ii]->SetBranchAddress("j3_betaStarClassic",&j3_betaStarClassic);
      ntu[ii]->SetBranchAddress("j3_csvBtag",&j3_csvBtag);
      ntu[ii]->SetBranchAddress("j3_csvMvaBtag",&j3_csvMvaBtag);
      ntu[ii]->SetBranchAddress("j3_jetProbBtag",&j3_jetProbBtag);
      ntu[ii]->SetBranchAddress("j3_tcheBtag",&j3_tcheBtag);
      ntu[ii]->SetBranchAddress("j3_radionMatched",&j3_radionMatched);
      ntu[ii]->SetBranchAddress("j4_e",&j4_e);
      ntu[ii]->SetBranchAddress("j4_pt",&j4_pt);
      ntu[ii]->SetBranchAddress("j4_phi",&j4_phi);
      ntu[ii]->SetBranchAddress("j4_eta",&j4_eta);
      ntu[ii]->SetBranchAddress("j4_betaStarClassic",&j4_betaStarClassic);
      ntu[ii]->SetBranchAddress("j4_csvBtag",&j4_csvBtag);
      ntu[ii]->SetBranchAddress("j4_csvMvaBtag",&j4_csvMvaBtag);
      ntu[ii]->SetBranchAddress("j4_jetProbBtag",&j4_jetProbBtag);
      ntu[ii]->SetBranchAddress("j4_tcheBtag",&j4_tcheBtag);
      ntu[ii]->SetBranchAddress("j4_radionMatched",&j4_radionMatched);
      ntu[ii]->SetBranchAddress("JetsMass",&JetsMass);
      ntu[ii]->SetBranchAddress("dijet_E",&dijet_E);
      ntu[ii]->SetBranchAddress("dijet_pt",&dijet_pt);
      ntu[ii]->SetBranchAddress("dijet_phi",&dijet_phi);
      ntu[ii]->SetBranchAddress("dijet_eta",&dijet_eta);
      ntu[ii]->SetBranchAddress("radion_E",&radion_E);
      ntu[ii]->SetBranchAddress("radion_pt",&radion_pt);
      ntu[ii]->SetBranchAddress("radion_eta",&radion_eta);
      ntu[ii]->SetBranchAddress("radion_phi",&radion_phi);
      
  }

  TH1F* h_PhoR9 = new TH1F("h_PhoR9","h_PhoR9",50,0.2,1.);
  TH1F* h_PhoHoE = new TH1F("h_PhoHoE","h_PhoHoE",50,0.,0.1);
  TH1F* h_PhoSieie = new TH1F("h_PhoSieie","h_PhoSieie",50,0.,0.07);
  TH1F* h_PhoSipip = new TH1F("h_PhoSipip","h_PhoSipip",50,0.,0.0045);
  TH1F* h_PhoSieip = new TH1F("h_PhoSieip","h_PhoSieip",50,0.,0.0004);
  TH1F* h_PhoPt = new TH1F("h_PhoPt","h_PhoPt", 90,0.,180.);
  TH1F* h_PhoEta = new TH1F("h_PhoEta","h_PhoEta",70,-3.5,3.5);
  TH1F* h_PhoPhi = new TH1F("h_PhoPhi","h_PhoPhi",70,-3.5,3.5);
  TH1F* h_diPhoDeltaEta = new TH1F("h_diPhoDeltaEta","h_diPhoDeltaEta", 100,-5.,5.);
  TH1F* h_diPhoDeltaPhi = new TH1F("h_diPhoDeltaPhi","h_diPhoDeltaPhi", 100,-4.,4.);
  TH1F* h_diPhoDeltaR = new TH1F("h_diPhoDeltaR","h_diPhoDeltaR", 60,0.,6.);
  TH1F* h_diPhoInvMass = new TH1F("h_diPhoInvMass","h_diPhoInvMass", 300,0.,300.);
  TH1F* h_JetNum = new TH1F("h_JetNum","h_JetNum", 10,0.,10.);
  TH1F* h_JetNum_btagged = new TH1F("h_JetNum_btagged","h_JetNum_btagged", 10,0.,10.);
  TH1F* h_JetcsvBtag = new TH1F("h_JetcsvBtag","h_JetcsvBtag", 100,0.,1.);
  TH1F* h_JetcsvMvaBtag = new TH1F("h_JetcsvMvaBtag","h_JetcsvMvaBtag", 100,0.,1.);
  TH1F* h_JetProbBtag = new TH1F("h_JetProbBtag","h_JetProbBtag", 100,0.,1.);
  TH1F* h_JettcheBtag = new TH1F("h_JettcheBtag","h_JettcheBtag", 100,0.,1.);
  TH1F* h_JetPt = new TH1F("h_JetPt","h_JetPt", 90,0.,180.);
  TH1F* h_JetEta = new TH1F("h_JetEta","h_JetEta",80,-4,4);
  TH1F* h_JetPhi = new TH1F("h_JetPhi","h_JetPhi",70,-3.5,3.5);
  TH1F* h_diJetDeltaEta = new TH1F("h_diJetDeltaEta","h_diJetDeltaEta", 100,-7.,7.);
  TH1F* h_diJetDeltaPhi = new TH1F("h_diJetDeltaPhi","h_diJetDeltaPhi", 100,-4.,4.);
  TH1F* h_diJetDeltaR = new TH1F("h_diJetDeltaR","h_diJetDeltaR", 70,0.,7.);
  TH1F* h_diJetInvMass = new TH1F("h_diJetInvMass","h_diJetInvMass", 80,0.,800);
  TH1F* h_diJetDeltaEta_maxpt = new TH1F("h_diJetDeltaEta_maxpt","h_diJetDeltaEta_maxpt", 100,-7.,7.);
  TH1F* h_diJetDeltaPhi_maxpt = new TH1F("h_diJetDeltaPhi_maxpt","h_diJetDeltaPhi_maxpt", 100,-4.,4.);
  TH1F* h_diJetDeltaR_maxpt = new TH1F("h_diJetDeltaR_maxpt","h_diJetDeltaR_maxpt", 70,0.,7.);
  TH1F* h_diJetInvMass_maxpt = new TH1F("h_diJetInvMass_maxpt","h_diJetInvMass_maxpt", 80,0.,800);
  TH1F* h_PhoJetDeltaEta = new TH1F("h_PhoJetDeltaEta","h_PhoJetDeltaEta", 100,-5.,5.);
  TH1F* h_PhoJetDeltaPhi = new TH1F("h_PhoJetDeltaPhi","h_PhoJetDeltaPhi", 100,-4.,4.);
  TH1F* h_PhoJetDeltaR = new TH1F("h_PhoJetDeltaR","h_PhoJetDeltaR", 60,0.,6.);
  TH1F* h_PhoJetDeltaEta_maxpt = new TH1F("h_PhoJetDeltaEta_maxpt","h_PhoJetDeltaEta_maxpt", 100,-5.,5.);
  TH1F* h_PhoJetDeltaPhi_maxpt = new TH1F("h_PhoJetDeltaPhi_maxpt","h_PhoJetDeltaPhi_maxpt", 100,-4.,4.);
  TH1F* h_PhoJetDeltaR_maxpt = new TH1F("h_PhoJetDeltaR_maxpt","h_PhoJetDeltaR_maxpt", 60,0.,6.);
  TH1F* h_PhoJetDeltaEta_maxpt_btag = new TH1F("h_PhoJetDeltaEta_maxpt_btag","h_PhoJetDeltaEta_maxpt_btag", 100,-5.,5.);
  TH1F* h_PhoJetDeltaPhi_maxpt_btag = new TH1F("h_PhoJetDeltaPhi_maxpt_btag","h_PhoJetDeltaPhi_maxpt_btag", 100,-4.,4.);
  TH1F* h_PhoJetDeltaR_maxpt_btag = new TH1F("h_PhoJetDeltaR_maxpt_btag","h_PhoJetDeltaR_maxpt_btag", 60,0.,6.);
  TH1F* h_diPhodiJetDeltaEta = new TH1F("h_diPhodiJetDeltaEta","h_diPhodiJetDeltaEta", 120,-9.,9.);
  TH1F* h_diPhodiJetDeltaPhi = new TH1F("h_diPhodiJetDeltaPhi","h_diPhodiJetDeltaPhi", 100,-4.,4.);
  TH1F* h_diPhodiJetDeltaR = new TH1F("h_diPhodiJetDeltaR","h_diPhodiJetDeltaR", 90,0.,9.);
  TH1F* h_diPhodiJetInvMass = new TH1F("h_diPhodiJetInvMass","h_diPhodiJetInvMass", 120,0.,1200);
  TH1F* h_diPhodiJetInvMass_core = new TH1F("h_diPhodiJetInvMass_core","h_diPhodiJetInvMass_core", 120,0.,1200);
  TH1F* h_diPhodiJetInvMass_sidebands = new TH1F("h_diPhodiJetInvMass_sidebands","h_diPhodiJetInvMass_sidebands", 120,0.,1200);
  TH1F* h_diPhodiJetInvMass_maxpt = new TH1F("h_diPhodiJetInvMass_maxpt","h_diPhodiJetInvMass_maxpt", 120,0.,1200);
  TH1F* h_diPhodiJetInvMass_core_maxpt = new TH1F("h_diPhodiJetInvMass_core_maxpt","h_diPhodiJetInvMass_core_maxpt", 120,0.,1200);
  TH1F* h_diPhodiJetInvMass_sidebands_maxpt = new TH1F("h_diPhodiJetInvMass_sidebands_maxpt","h_diPhodiJetInvMass_sidebands_maxpt", 120,0.,1200);
  TH1F* h_diPhodiJetInvMass_btag = new TH1F("h_diPhodiJetInvMass_btag","h_diPhodiJetInvMass_btag", 120,0.,1200);
  TH1F* h_diPhodiJetInvMass_core_btag = new TH1F("h_diPhodiJetInvMass_core_btag","h_diPhodiJetInvMass_core_btag", 120,0.,1200);
  TH1F* h_diPhodiJetInvMass_sidebands_btag = new TH1F("h_diPhodiJetInvMass_sidebands_btag","h_diPhodiJetInvMass_sidebands_btag", 120,0.,1200);
  TH1F* h_diPhodiJetInvMass_bveto = new TH1F("h_diPhodiJetInvMass_bveto","h_diPhodiJetInvMass_bveto", 120,0.,1200);
  TH1F* h_diPhodiJetInvMass_core_bveto = new TH1F("h_diPhodiJetInvMass_core_bveto","h_diPhodiJetInvMass_core_bveto", 120,0.,1200);
  TH1F* h_diPhodiJetInvMass_sidebands_bveto = new TH1F("h_diPhodiJetInvMass_sidebands_bveto","h_diPhodiJetInvMass_sidebands_bveto", 120,0.,1200);
  
  std::map<int,TLorentzVector*> phoP4;
  std::map<int,TLorentzVector*> jetP4; 
  std::map<int,float> jet_csvBtag;
  std::map<int,float> jet_csvMvaBtag;
  std::map<int,float> jet_jetProbBtag;
  std::map<int,float> jet_tcheBtag;
  TLorentzVector* sumP4;
  TLorentzVector* PhoSumP4;
  std::map<int,TLorentzVector*> JetSumP4;

  std::map<int,bool> bad_jet;
  
  std::map<int,bool> isBTagCouple;
  std::map<int,bool> isBVetoCouple;

  std::vector<float>* pho_pt;
  std::vector<float>* jet_pt;

  std::map<float,int>* phoPt_map;
  std::map<float,int>* jetPt_map;

  int jet_couple_num; 

  for(int nn = 0; nn < pos_total; nn++){
      for(int ientry = 0; ientry < ntu[nn]->GetEntries(); ientry++){
          if(ientry%100000==0) std::cout<<"--- Reading file_" << nn << " entry = "<< ientry <<std::endl;
          ntu[nn]->GetEntry(ientry);

          float puRe = 1.;

          pho_pt = new std::vector<float>;
          jet_pt = new std::vector<float>;

          phoPt_map = new std::map<float,int>;
          jetPt_map = new std::map<float,int>;

          puRe = evweight;
         
          if(isReweighted == true) puRe = weight[nn][pu_n];;
          
          int n_bad_jets = 0;
          
          for(int ii = 0; ii < 4; ii++)
              bad_jet[ii] = false;
           
          phoP4[0] = new TLorentzVector();
          if(ph1_pt != -1001 && ph1_pt != 0) phoP4[0]->SetPtEtaPhiE(ph1_pt,ph1_eta,ph1_phi,ph1_e);
          else continue;
          
          phoP4[1] = new TLorentzVector();
          if(ph2_pt != -1001 && ph2_pt != 0) phoP4[1]->SetPtEtaPhiE(ph2_pt,ph2_eta,ph2_phi,ph2_e);
          else continue;
          
          jetP4[0] = new TLorentzVector();
          if(j1_pt != -1001 && j1_pt != 0) jetP4[0]->SetPtEtaPhiE(j1_pt,j1_eta,j1_phi,j1_e);
          else{
             bad_jet[0] = true;
             n_bad_jets++;
          }

          jetP4[1] = new TLorentzVector();
          if(j2_pt != -1001 && j2_pt != 0) jetP4[1]->SetPtEtaPhiE(j2_pt,j2_eta,j2_phi,j2_e);
          else{
             bad_jet[1] = true;
             n_bad_jets++;
          }

          jetP4[2] = new TLorentzVector();
          if(j3_pt != -1001 && j3_pt != 0) jetP4[2]->SetPtEtaPhiE(j3_pt,j3_eta,j3_phi,j3_e);
          else{
             bad_jet[2] = true;
             n_bad_jets++;
          }

          jetP4[3] = new TLorentzVector();
          if(j4_pt != -1001 && j4_pt != 0) jetP4[3]->SetPtEtaPhiE(j4_pt,j4_eta,j4_phi,j4_e);
          else{
             bad_jet[3] = true;
             n_bad_jets++;
          }

          if(n_bad_jets > 2) continue;

          jet_csvBtag[0] = j1_csvBtag;
          jet_csvMvaBtag[0] = j1_csvMvaBtag;
          jet_jetProbBtag[0] = j1_jetProbBtag;
          jet_tcheBtag[0] = j1_tcheBtag;

          jet_csvBtag[1] = j2_csvBtag;
          jet_csvMvaBtag[1] = j2_csvMvaBtag;
          jet_jetProbBtag[1] = j2_jetProbBtag;
          jet_tcheBtag[1] = j2_tcheBtag;

          jet_csvBtag[2] = j3_csvBtag;
          jet_csvMvaBtag[2] = j3_csvMvaBtag;
          jet_jetProbBtag[2] = j3_jetProbBtag;
          jet_tcheBtag[2] = j3_tcheBtag;

          jet_csvBtag[3] = j4_csvBtag;
          jet_csvMvaBtag[3] = j4_csvMvaBtag;
          jet_jetProbBtag[3] = j4_jetProbBtag;
          jet_tcheBtag[3] = j4_tcheBtag;

          for(int ii = 0; ii < 4; ii++)
              if(jet_csvBtag[ii] < 0) bad_jet[ii] = true;

          int jet_num = 0;
          for(int ii = 0; ii < 4; ii++)
              if(bad_jet[ii] == false) jet_num++;

          if(jet_num < 2) continue;

          h_JetNum->Fill(jet_num, puRe);

          int n_bjets = 0;
           
          for(int ii = 0; ii < jet_csvBtag.size(); ii++){
              if(bad_jet[ii] == true) continue;
              if(jet_csvBtag[ii] > btag_wp ) n_bjets++;
          }

          if(n_bjets < 1) continue;
            
          h_JetNum_btagged->Fill(n_bjets, puRe);
             
          h_PhoR9->Fill(ph1_r9,puRe);
          h_PhoR9->Fill(ph2_r9,puRe);

          h_PhoHoE->Fill(ph1_hoe,puRe);
          h_PhoHoE->Fill(ph2_hoe,puRe);
           
          h_PhoSieie->Fill(ph1_sieie,puRe);
          h_PhoSieie->Fill(ph2_sieie,puRe);

          h_PhoSipip->Fill(ph1_sipip,puRe);
          h_PhoSipip->Fill(ph2_sipip,puRe);

          h_PhoSieip->Fill(ph1_sieip,puRe);
          h_PhoSieip->Fill(ph1_sieip,puRe);
         
          for(int ii = 0; ii < 2; ii++){

              h_PhoPt->Fill(phoP4[ii]->Pt(),puRe); 
              h_PhoEta->Fill(phoP4[ii]->Eta(),puRe);
              h_PhoPhi->Fill(phoP4[ii]->Phi(),puRe);

              pho_pt->push_back(phoP4[ii]->Pt());
              phoPt_map->insert(std::pair<float,int>(phoP4[ii]->Pt(),ii));
          }
          
          h_diPhoDeltaEta->Fill(ph1_eta-ph2_eta,puRe);
       
          h_diPhoDeltaPhi->Fill(phoP4[0]->DeltaPhi(*phoP4[1]),puRe);

          h_diPhoDeltaR->Fill(phoP4[0]->DeltaR(*phoP4[1]),puRe); 
         
          h_diPhoInvMass->Fill(PhotonsMass,puRe);

          sumP4 = new TLorentzVector();

          *sumP4 = *phoP4[0]+*phoP4[1];

          PhoSumP4 = (TLorentzVector*)sumP4->Clone();

          delete sumP4;
        
          for(int ii = 0; ii < 4; ii++){

              if(bad_jet[ii] == true) continue;

              h_JetPt->Fill(jetP4[ii]->Pt(),puRe); 
              h_JetEta->Fill(jetP4[ii]->Eta(),puRe);
              h_JetPhi->Fill(jetP4[ii]->Phi(),puRe);
              
              if(jet_csvBtag[ii] > 0) h_JetcsvBtag->Fill(jet_csvBtag[ii],puRe);
              if(jet_csvMvaBtag[ii] > 0) h_JetcsvMvaBtag->Fill(jet_csvMvaBtag[ii],puRe);
              if(jet_jetProbBtag[ii] > 0) h_JetProbBtag->Fill(jet_jetProbBtag[ii],puRe);
              if(jet_tcheBtag[ii] > 0) h_JettcheBtag->Fill(jet_tcheBtag[ii],puRe);

              jet_pt->push_back(jetP4[ii]->Pt());
              jetPt_map->insert(std::pair<float,int>(jetP4[ii]->Pt(),ii));
          
          }

          jet_couple_num = 0;
          
          for(int ii = 0; ii < 4; ii++){

              if(bad_jet[ii] == true) continue;

              for(int jj = 0; jj < 4; jj++){

                  if(bad_jet[jj] == true) continue;     
                  if(ii == jj) continue;

                  h_diJetDeltaEta->Fill(jetP4[ii]->Eta()-jetP4[jj]->Eta(), puRe);
                  h_diJetDeltaPhi->Fill(jetP4[ii]->DeltaPhi(*jetP4[jj]),puRe);
                  h_diJetDeltaR->Fill(jetP4[ii]->DeltaR(*jetP4[jj]),puRe); 

                  sumP4 = new TLorentzVector();

                  *sumP4 = *jetP4[ii]+*jetP4[jj];
                  
                  JetSumP4[jet_couple_num] = (TLorentzVector*)sumP4->Clone();
                  h_diJetInvMass->Fill(sumP4->M(),puRe);

                  isBTagCouple[jet_couple_num] = false;
                  isBVetoCouple[jet_couple_num] = false;

                  if(jet_csvBtag[ii] > btag_wp) isBTagCouple[jet_couple_num] = true;
                  if(jet_csvBtag[jj] > btag_wp) isBTagCouple[jet_couple_num] = true;
                  if(jet_csvBtag[ii] <= 1-btag_wp && jet_csvBtag[jj] <= btag_wp) isBVetoCouple[jet_couple_num] = true;
                  if(jet_csvBtag[ii] <= btag_wp && jet_csvBtag[jj] <= 1-btag_wp) isBVetoCouple[jet_couple_num] = true;
                   
                  delete sumP4;
              
                 jet_couple_num++;
              }
          } 

          std::sort(pho_pt->begin(),pho_pt->end());
          std::sort(jet_pt->begin(),jet_pt->end());

          h_diJetDeltaEta_maxpt->Fill(jetP4[jetPt_map->at(jet_pt->at(jet_pt->size()-1))]->Eta()-jetP4[jetPt_map->at(jet_pt->at(jet_pt->size()-2))]->Eta(), puRe);
          h_diJetDeltaPhi_maxpt->Fill(jetP4[jetPt_map->at(jet_pt->at(jet_pt->size()-1))]->DeltaPhi(*jetP4[jetPt_map->at(jet_pt->at(jet_pt->size()-2))]),puRe);
          h_diJetDeltaR_maxpt->Fill(jetP4[jetPt_map->at(jet_pt->at(jet_pt->size()-1))]->DeltaR(*jetP4[jetPt_map->at(jet_pt->at(jet_pt->size()-2))]),puRe); 

          sumP4 = new TLorentzVector();

          *sumP4 = *jetP4[jetPt_map->at(jet_pt->at(jet_pt->size()-1))]+*jetP4[jetPt_map->at(jet_pt->at(jet_pt->size()-2))];
                  
          h_diJetInvMass_maxpt->Fill(sumP4->M(),puRe);

          delete sumP4;

          for(int ii = 0; ii < 2; ii++)
              for(int jj = 0; jj < 4; jj++){
                   
                  if(bad_jet[jj] == true) continue;

                  h_PhoJetDeltaEta->Fill(phoP4[ii]->Eta()-jetP4[jj]->Eta(),puRe);
                  h_PhoJetDeltaPhi->Fill(phoP4[ii]->DeltaPhi(*jetP4[jj]),puRe);
                  h_PhoJetDeltaR->Fill(phoP4[ii]->DeltaR(*jetP4[jj]),puRe);  
              }
           
          h_PhoJetDeltaEta_maxpt->Fill(phoP4[phoPt_map->at(pho_pt->at(pho_pt->size()-1))]->Eta()-jetP4[jetPt_map->at(jet_pt->at(jet_pt->size()-1))]->Eta(),puRe);
          h_PhoJetDeltaPhi_maxpt->Fill(phoP4[phoPt_map->at(pho_pt->at(pho_pt->size()-1))]->DeltaPhi(*jetP4[jetPt_map->at(jet_pt->at(jet_pt->size()-1))]),puRe);
          h_PhoJetDeltaR_maxpt->Fill(phoP4[phoPt_map->at(pho_pt->at(pho_pt->size()-1))]->DeltaR(*jetP4[jetPt_map->at(jet_pt->at(jet_pt->size()-1))]),puRe);
           
          for(int ii = 0; ii < 4; ii++){
               
              if(bad_jet[ii] == true) continue;
              if(jet_csvBtag[ii] < btag_wp) continue;

              h_PhoJetDeltaEta_maxpt_btag->Fill(phoP4[phoPt_map->at(pho_pt->at(pho_pt->size()-1))]->Eta() - jetP4[ii]->Eta(),puRe);
              h_PhoJetDeltaPhi_maxpt_btag->Fill(phoP4[phoPt_map->at(pho_pt->at(pho_pt->size()-1))]->DeltaPhi(*jetP4[ii]),puRe);
              h_PhoJetDeltaR_maxpt_btag->Fill(phoP4[phoPt_map->at(pho_pt->at(pho_pt->size()-1))]->DeltaR(*jetP4[ii]),puRe);

          }

          for(unsigned int jj = 0; jj < JetSumP4.size(); jj++){
      
                  h_diPhodiJetDeltaEta->Fill(PhoSumP4->Eta()-JetSumP4[jj]->Eta(),puRe);
                  h_diPhodiJetDeltaPhi->Fill(PhoSumP4->DeltaPhi(*JetSumP4[jj]),puRe);
                  h_diPhodiJetDeltaR->Fill(PhoSumP4->DeltaR(*JetSumP4[jj]),puRe); 

                  sumP4 = new TLorentzVector();
                   
                  *sumP4 = *PhoSumP4+*JetSumP4[jj];

                  h_diPhodiJetInvMass->Fill(sumP4->M(),puRe);

                  if(PhoSumP4->M() >= 123 && PhoSumP4->M() <= 127 ) h_diPhodiJetInvMass_core->Fill(sumP4->M(),puRe);
                  if(PhoSumP4->M() < 123 || PhoSumP4->M() > 127 ) h_diPhodiJetInvMass_sidebands->Fill(sumP4->M(),puRe);

                  if(isBTagCouple[jj] == true){
                     h_diPhodiJetInvMass_btag->Fill(sumP4->M(),puRe);
                     if(PhoSumP4->M() >= 123 && PhoSumP4->M() <= 127 ) h_diPhodiJetInvMass_core_btag->Fill(sumP4->M(),puRe);
                     if(PhoSumP4->M() < 123 || PhoSumP4->M() > 127 ) h_diPhodiJetInvMass_sidebands_btag->Fill(sumP4->M(),puRe);
                  }
                  
                  if(isBVetoCouple[jj] == true){
                     h_diPhodiJetInvMass_bveto->Fill(sumP4->M(),puRe);
                     if(PhoSumP4->M() >= 123 && PhoSumP4->M() <= 127 ) h_diPhodiJetInvMass_core_bveto->Fill(sumP4->M(),puRe);
                     if(PhoSumP4->M() < 123 || PhoSumP4->M() > 127 ) h_diPhodiJetInvMass_sidebands_bveto->Fill(sumP4->M(),puRe);
                  }
                  
                  delete sumP4;
          }

          sumP4 = new TLorentzVector();
           
          *sumP4 = *jetP4[jetPt_map->at(jet_pt->at(jet_pt->size()-1))] + *jetP4[jetPt_map->at(jet_pt->at(jet_pt->size()-2))] + *PhoSumP4;

          h_diPhodiJetInvMass_maxpt->Fill(sumP4->M(),puRe);

          if(PhoSumP4->M() >= 123 && PhoSumP4->M() <= 127 ) h_diPhodiJetInvMass_core_maxpt->Fill(sumP4->M(),puRe);
          if(PhoSumP4->M() < 123 || PhoSumP4->M() > 127 ) h_diPhodiJetInvMass_sidebands_maxpt->Fill(sumP4->M(),puRe);

          delete sumP4;
          
          for(int ii = 0; ii < 2; ii++)
              delete phoP4[ii];

          for(int ii = 0; ii < 4; ii++)
              delete jetP4[ii];

          for(int ii = 0; ii < 4; ii++)
              bad_jet[ii] = false;

          delete pho_pt; 
          delete jet_pt; 
          delete phoPt_map;
          delete jetPt_map;
         
              
      }
  }

 
  TFile* output = new TFile(outputName.c_str(),"RECREATE");
  output->cd();
  h_PhoR9->Write();
  h_PhoHoE->Write();
  h_PhoSieie->Write();
  h_PhoSipip->Write();
  h_PhoSieip->Write();
  h_PhoPt->Write();
  h_PhoEta->Write();
  h_PhoPhi->Write();
  h_diPhoDeltaEta->Write();
  h_diPhoDeltaPhi->Write();
  h_diPhoDeltaR->Write();
  h_diPhoInvMass->Write();
  h_JetNum->Write();
  h_JetNum_btagged->Write();
  h_JetcsvBtag->Write();
  h_JetcsvMvaBtag->Write();
  h_JetProbBtag->Write();
  h_JettcheBtag->Write();
  h_JetPt->Write();
  h_JetEta->Write();
  h_JetPhi->Write();
  h_diJetDeltaEta->Write();
  h_diJetDeltaPhi->Write();
  h_diJetDeltaR->Write();
  h_diJetInvMass->Write();
  h_diJetDeltaEta_maxpt->Write();
  h_diJetDeltaPhi_maxpt->Write();
  h_diJetDeltaR_maxpt->Write();
  h_diJetInvMass_maxpt->Write();
  h_PhoJetDeltaEta->Write();
  h_PhoJetDeltaPhi->Write();
  h_PhoJetDeltaR->Write();
  h_PhoJetDeltaEta_maxpt->Write();
  h_PhoJetDeltaPhi_maxpt->Write();
  h_PhoJetDeltaR_maxpt->Write(); 
  h_PhoJetDeltaEta_maxpt_btag->Write();
  h_PhoJetDeltaPhi_maxpt_btag->Write();
  h_PhoJetDeltaR_maxpt_btag->Write();
  h_diPhodiJetDeltaEta->Write();
  h_diPhodiJetDeltaPhi->Write();
  h_diPhodiJetDeltaR->Write();
  h_diPhodiJetInvMass->Write();
  h_diPhodiJetInvMass_core->Write();
  h_diPhodiJetInvMass_sidebands->Write();
  h_diPhodiJetInvMass_maxpt->Write();
  h_diPhodiJetInvMass_core_maxpt->Write();
  h_diPhodiJetInvMass_sidebands_maxpt->Write();
  h_diPhodiJetInvMass_btag->Write();
  h_diPhodiJetInvMass_core_btag->Write();
  h_diPhodiJetInvMass_sidebands_btag->Write();
  h_diPhodiJetInvMass_bveto->Write();
  h_diPhodiJetInvMass_core_bveto->Write();
  h_diPhodiJetInvMass_sidebands_bveto->Write();
  output->Close();
  
}


















