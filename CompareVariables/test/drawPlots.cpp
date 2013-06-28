
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
  
  std::string inputVariables = gConfigParser -> readStringOption("Input::inputVariables");
  
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
  
  std::map<int,TH1F*> histo_DA_EB;
  std::map<int,TH1F*> histo_MC_EB;
  std::map<int,TH1F*> histo_DA_EE;
  std::map<int,TH1F*> histo_MC_EE;
  std::map<int,int> int_variables;
  std::map<int,float> float_variables;
  std::map<int,std::string> variables_name;
  std::map<std::string,int> variables_pos;
  std::map<int,int> map_ww;
  std::map<int,int> map_int;
  std::map<int,int> map_float;
  std::map<int,bool> variables_isGood;
  std::map<int,bool> variables_isFloat;
  std::map<int,bool> variables_isInt; 
  std::map<int,bool> variables_isDouble;
 
  
  
  int pos_int = 0;
  int pos_float = 0;
  int pos = 0;
  int pos_total = 0;
  char branch[200];
  char type[20];
  int ww = 0;
  int nbins_EB, nbins_EE;
  float bin_min_EB, bin_min_EE;
  float bin_max_EB, bin_max_EE;
  
  int ele1_isEB;
  int ele2_isEB;
  float npu;
  int isZ, runId;
  std::vector<float> puRe;
  
  ntu_DA->SetBranchStatus("*",0);
  ntu_DA->SetBranchStatus("runId",1);
  ntu_DA->SetBranchAddress("runId",&runId);
  ntu_DA->SetBranchStatus("ele1_isEB",1);
  ntu_DA->SetBranchStatus("ele2_isEB",1);
  ntu_DA->SetBranchAddress("ele1_isEB", &ele1_isEB);
  ntu_DA->SetBranchAddress("ele2_isEB", &ele2_isEB);
  ntu_DA->SetBranchStatus("isZ",1);
  ntu_DA->SetBranchAddress("isZ", &isZ);
  

  ntu_MC->SetBranchStatus("*",0); 
  ntu_MC->SetBranchStatus("runId",1);
  ntu_MC->SetBranchAddress("runId",&runId);
  ntu_MC->SetBranchStatus("isZ",1);
  ntu_MC->SetBranchAddress("isZ", &isZ);
  if(isReweighted == true){
     ntu_MC->SetBranchStatus("PUit_TrueNumInteractions",1); 
     ntu_MC->SetBranchAddress("PUit_TrueNumInteractions", &npu);
  }
  ntu_MC->SetBranchStatus("ele1_isEB",1);
  ntu_MC->SetBranchAddress("ele1_isEB", &ele1_isEB);
  ntu_MC->SetBranchStatus("ele2_isEB",1);
  ntu_MC->SetBranchAddress("ele2_isEB", &ele2_isEB);
  
  FILE *f_variables;
  f_variables = fopen(inputVariables.c_str(),"r");
  std::cout << "\nReading input_variables " << std::endl;
  while(fscanf(f_variables,"%s %s %d %d %f %f %d %f %f\n", type , branch, &ww, &nbins_EB, &bin_min_EB, &bin_max_EB, &nbins_EE, &bin_min_EE, &bin_max_EE) !=EOF ){
    
    std::string TYPE = std::string(type);
    if(TYPE.find("#") != std::string::npos) continue;
    if(nbins_EB != 0) std::cout << "Branch: " << branch << std::endl;
    
    ntu_DA->SetBranchStatus(branch,1);
    ntu_MC->SetBranchStatus(branch,1);

    map_ww[pos] = ww;
    
    if(TYPE.find("int") != std::string::npos){
       ntu_DA->SetBranchAddress(branch,&int_variables[pos_int]); 
       ntu_MC->SetBranchAddress(branch,&int_variables[pos_int]); 
       variables_isFloat[pos] = false;
       variables_isInt[pos] = true;
       map_int[pos] = pos_int;
       pos_int++;
    }

    if(TYPE.find("float") != std::string::npos){
       ntu_DA->SetBranchAddress(branch,&float_variables[pos_float]);
       ntu_MC->SetBranchAddress(branch,&float_variables[pos_float]); 
       variables_isFloat[pos] = true;
       variables_isInt[pos] = false;
       map_float[pos] = pos_float;
       pos_float++;
    }
    
    variables_name[pos] = std::string(branch);
    variables_pos[std::string(branch)] = pos;
    
    std::string histo_name_DA_EB = "DA_"+std::string(branch)+"_EB";
    std::string histo_name_MC_EB = "MC_"+std::string(branch)+"_EB";
    std::string histo_name_DA_EE = "DA_"+std::string(branch)+"_EE";
    std::string histo_name_MC_EE = "MC_"+std::string(branch)+"_EE";

    if(nbins_EB == 0){
       histo_DA_EB[pos] = new TH1F(histo_name_DA_EB.c_str(),histo_name_DA_EB.c_str(),0, 0, 0);
       histo_MC_EB[pos] = new TH1F(histo_name_MC_EB.c_str(),histo_name_MC_EB.c_str(),0, 0, 0);
       histo_DA_EE[pos] = new TH1F(histo_name_DA_EE.c_str(),histo_name_DA_EE.c_str(),0, 0, 0);
       histo_MC_EE[pos] = new TH1F(histo_name_MC_EE.c_str(),histo_name_MC_EE.c_str(),0, 0, 0);
       variables_isGood[pos] = false;
    }
    else{
       histo_DA_EB[pos] = new TH1F(histo_name_DA_EB.c_str(),histo_name_DA_EB.c_str(),nbins_EB, bin_min_EB, bin_max_EB);
       histo_MC_EB[pos] = new TH1F(histo_name_MC_EB.c_str(),histo_name_MC_EB.c_str(),nbins_EB, bin_min_EB, bin_max_EB);
       histo_DA_EE[pos] = new TH1F(histo_name_DA_EE.c_str(),histo_name_DA_EE.c_str(),nbins_EE, bin_min_EE, bin_max_EE);
       histo_MC_EE[pos] = new TH1F(histo_name_MC_EE.c_str(),histo_name_MC_EE.c_str(),nbins_EE, bin_min_EE, bin_max_EE);
       variables_isGood[pos] = true;
    }
    pos++;
  } 
  
  pos_total = pos;
 
  for(int pos = 0; pos < pos_total; pos++){
      
      variables_isDouble[pos] = false;
      
      f_variables = fopen(inputVariables.c_str(),"r");
      while(fscanf(f_variables,"%s %s %d %d %f %f %d %f %f\n", type , branch, &ww, &nbins_EB, &bin_min_EB, &bin_max_EB, &nbins_EE, &bin_min_EE, &bin_max_EE) !=EOF ){

        std::string TYPE = std::string(type);
        if(TYPE.find("#") != std::string::npos) continue;
       
        if(variables_isGood[pos] == false)continue;
      
        if(variables_name[pos].find("ele1") == std::string::npos) continue;
       
        variables_isDouble[pos] = true;
      }
  }
  
  for(int entry = 0; entry < ntu_DA->GetEntries(); entry++){
     if(entry%100000==0) std::cout<<"--- Reading DATA_entry = "<<entry<<std::endl;
     ntu_DA->GetEntry(entry);
     
     if(runId < inputRunIdCutMinDA) continue;
     if(runId > inputRunIdCutMaxDA) continue;
      
     if(isZ == 0) continue;
     
     for(int pos = 0; pos < pos_total; pos++){

         if(variables_isGood[pos] == false) continue;
         
         int isEB = -1;
         if(variables_name[pos].find("ele1") != std::string::npos) isEB = ele1_isEB;
         if(variables_name[pos].find("ele2") != std::string::npos) isEB = ele2_isEB;
         if(variables_name[pos].find("ele1") == std::string::npos && variables_name[pos].find("ele2") == std::string::npos) continue; 

         if(isEB == 1){
         	if(variables_isInt[pos] == true) histo_DA_EB[pos] -> Fill(int_variables[map_int[pos]]);
         	if(variables_isFloat[pos] == true) histo_DA_EB[pos] -> Fill(float_variables[map_float[pos]]);
         }

         if(isEB == 0){
         	if(variables_isInt[pos] == true) histo_DA_EE[pos] -> Fill(int_variables[map_int[pos]]);
         	if(variables_isFloat[pos] == true) histo_DA_EE[pos] -> Fill(float_variables[map_float[pos]]);
         }
         
     }
     
  }
  
  for(int entry = 0; entry < ntu_MC->GetEntries(); entry++){
     if(entry%100000==0) std::cout<<"--- Reading MC_entry = "<<entry<<std::endl;
     ntu_MC->GetEntry(entry);

     if(runId < inputRunIdCutMinMC) continue;
     if(runId > inputRunIdCutMaxMC) continue;
     
     if(isZ == 0) continue;
     
     float puRe = 1.;
     
     if(isReweighted == true) puRe = puReweighting->GetWeight(int(npu));
     //std::cout << puRe << std::endl;
    
     for(int pos = 0; pos < pos_total; pos++){

         if(variables_isGood[pos] == false) continue;
          
         int isEB = -1;
         if(variables_name[pos].find("ele1") != std::string::npos) isEB = ele1_isEB;
         if(variables_name[pos].find("ele2") != std::string::npos) isEB = ele2_isEB;
         if(variables_name[pos].find("ele1") == std::string::npos && variables_name[pos].find("ele2") == std::string::npos) continue;

         if(isEB == 1){

            if(map_ww[pos] == 1 && isReweighted == true){
               if(variables_isInt[pos] == true) histo_MC_EB[pos] -> Fill(int_variables[map_int[pos]],puRe);
               if(variables_isFloat[pos] == true) histo_MC_EB[pos] -> Fill(float_variables[map_float[pos]],puRe);
            }
          
            if(map_ww[pos] == 0 && isReweighted == false){
               if(variables_isInt[pos] == true) histo_MC_EB[pos] -> Fill(int_variables[map_int[pos]]);
               if(variables_isFloat[pos] == true) histo_MC_EB[pos] -> Fill(float_variables[map_float[pos]]);
            }
         } 
         
         if(isEB == 0){

            if(map_ww[pos] == 1 && isReweighted == true){
               if(variables_isInt[pos] == true) histo_MC_EE[pos] -> Fill(int_variables[map_int[pos]],puRe);
               if(variables_isFloat[pos] == true) histo_MC_EE[pos] -> Fill(float_variables[map_float[pos]],puRe);
            }
          
            if(map_ww[pos] == 0 && isReweighted == false){
               if(variables_isInt[pos] == true) histo_MC_EE[pos] -> Fill(int_variables[map_int[pos]]);
               if(variables_isFloat[pos] == true) histo_MC_EE[pos] -> Fill(float_variables[map_float[pos]]);
            }
         }

    
     }
         
  }
  
  std::string comp_type = inputCompType;
  
  TFile* o = new TFile((outputDir+"compareVariables.root").c_str(), "RECREATE");
  
  o -> cd();
 
  for(int pos = 0; pos < pos_total; pos++){

      if(variables_isGood[pos] == false) continue;
  
      compareHistos(histo_DA_EB[pos], histo_MC_EB[pos], inputLabelData, inputLabelMC, comp_type , outputDir);
     
      compareHistos(histo_DA_EE[pos], histo_MC_EE[pos], inputLabelData, inputLabelMC, comp_type , outputDir);
      
      histo_DA_EB[pos] -> Write();
      histo_DA_EE[pos] -> Write();
      histo_MC_EB[pos] -> Write();
      histo_MC_EE[pos] -> Write();
      
  }

  for(int pos = 0; pos < pos_total; pos++){
       
        if(variables_isDouble[pos] == false) continue;
        
        std::string newName = std::string("ele2_"+variables_name[pos].erase(0,5));
       
  	histo_DA_EB[pos]-> Add(histo_DA_EB[variables_pos[newName]]);
  	histo_DA_EB[pos]->SetName(("DA_"+variables_name[pos]+"_total_EB").c_str());
        histo_DA_EE[pos]-> Add(histo_DA_EE[variables_pos[newName]]);
  	histo_DA_EE[pos]->SetName(("DA_"+variables_name[pos]+"_total_EE").c_str());
        histo_MC_EB[pos]-> Add(histo_MC_EB[variables_pos[newName]]);
  	histo_MC_EB[pos]->SetName(("MC_"+variables_name[pos]+"_total_EB").c_str());
        histo_MC_EE[pos]-> Add(histo_MC_EE[variables_pos[newName]]);
  	histo_MC_EE[pos]->SetName(("MC_"+variables_name[pos]+"_total_EE").c_str());
  	
  
        compareHistos(histo_DA_EB[pos], histo_MC_EB[pos], inputLabelData, inputLabelMC, comp_type , outputDir);
     
        compareHistos(histo_DA_EE[pos], histo_MC_EE[pos], inputLabelData, inputLabelMC, comp_type , outputDir);
        
        histo_DA_EB[pos] -> Write();
        histo_DA_EE[pos] -> Write();
        histo_MC_EB[pos] -> Write();
        histo_MC_EE[pos] -> Write();
  }
  
  o-> Close();
  
}


















