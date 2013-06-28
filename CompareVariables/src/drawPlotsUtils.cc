#include "drawPlotsUtils.h"


void compareHistos(TH1F* histo_DA, TH1F* histo_MC, std::string label_DA, std::string label_MC, std::string comp_type , std::string outputDir){

    double xNorm = histo_DA->Integral()/histo_MC->Integral()*histo_DA->GetBinWidth(1)/histo_MC->GetBinWidth(1);   
    
    histo_MC -> Scale(xNorm);
    
    histo_DA -> GetXaxis() -> SetLabelSize(0);
    histo_DA -> GetYaxis() -> SetLabelSize(0.04);
    histo_DA -> GetXaxis() -> SetTitleSize(0.05);
    histo_DA -> GetYaxis() -> SetTitleSize(0.05);
    histo_DA -> GetYaxis() -> SetTitleOffset(1.25);

    histo_DA -> GetXaxis() -> SetTitle("");
    histo_MC -> GetXaxis() -> SetTitle("");
    
    std::string xTitle = std::string(histo_DA->GetName()).erase(0,3);
   
    char yTitle[50];
    sprintf(yTitle,"events");
    histo_DA -> GetYaxis() -> SetTitle(yTitle);
    histo_MC -> GetYaxis() -> SetTitle(yTitle);
    
    histo_MC -> SetLineColor(kRed);
    histo_DA -> SetMarkerColor(kBlack);
    histo_DA -> SetMarkerSize(0.5);
    
    histo_MC -> SetFillColor(kRed);
    histo_MC -> SetFillStyle(3003);
        
    histo_MC -> SetLineWidth(2);
    histo_DA -> SetLineWidth(1);
    
    float maximum = -1.;
    if( histo_DA -> GetMaximum() > maximum ) maximum = histo_DA -> GetMaximum();
    //if( histo_MC -> GetMaximum() > maximum ) maximum = histo_MC -> GetMaximum();
    histo_DA -> SetMaximum( 1.2*maximum );
    histo_DA -> SetMinimum( 0. );
      
    TCanvas* c1 = new TCanvas("c1","c1");
    c1 -> cd();
    
    TPaveStats* st_DA = new TPaveStats();
    TPaveStats* st_MC = new TPaveStats();
    TPaveStats* st_ratio = new TPaveStats();
    
    TPad *cUp  = new TPad("pad_0","pad_0",0.00,0.36,1.00,1.00);
    TPad *cDown = new TPad("pad_1","pad_1",0.00,0.00,1.00,0.36);

    cUp->SetBottomMargin(0.01); 
    cDown->SetTopMargin(0.01); 
    
    cUp->Draw();
    cDown->Draw();
     
    cUp->cd();
    
    histo_DA -> Draw("e");
    gPad -> Update();
    st_DA= (TPaveStats*)(histo_DA->GetListOfFunctions()->FindObject("stats"));
    st_DA->SetX1NDC(0.82); //new x start position
    st_DA->SetX2NDC(0.99); //new x end position
    st_DA->SetY1NDC(0.70); //new y start position
    st_DA->SetY2NDC(0.82); //new y end position
    st_DA->SetTextColor(kBlack);
    st_DA->Draw("sames");
    
    histo_MC -> Draw("sames");
    gPad -> Update();
    st_MC= (TPaveStats*)(histo_MC->GetListOfFunctions()->FindObject("stats"));
    st_MC->SetX1NDC(0.82); //new x start position
    st_MC->SetX2NDC(0.99); //new x end position
    st_MC->SetY1NDC(0.56); //new y start position
    st_MC->SetY2NDC(0.68); //new y end position
    st_MC->SetTextColor(kRed);
    st_MC->Draw("sames");
   
    TLegend* legend = new TLegend(0.82, 0.82, 0.99, 0.94);
    legend -> SetFillColor(kWhite);
    legend -> SetFillStyle(1000);
    legend -> SetLineWidth(0);
    legend -> SetLineColor(kWhite);
    legend -> SetTextFont(42);  
    legend -> SetTextSize(0.04);
    
    legend -> AddEntry(histo_DA,label_DA.c_str(),"P");
    legend -> AddEntry(histo_MC,label_MC.c_str(),"F");
    legend -> Draw("same");
    
    TLatex* latex = new TLatex(0.026, 0.97, xTitle.c_str());
    latex -> SetNDC();
    latex -> SetTextFont(kBlack);
    latex -> SetTextSize(0.04);
    latex -> Draw("same");
    
    cDown->cd();
    
    TH1F* histo_ratio=(TH1F*)histo_DA->Clone("histo_ratio");
    histo_ratio->Sumw2();
    histo_ratio->Divide(histo_MC);
    
    std::string histo_ratio_label = label_DA+"/"+label_MC;

    histo_ratio -> GetXaxis() -> SetTitle(xTitle.c_str());
    histo_ratio -> GetYaxis() -> SetTitle(histo_ratio_label.c_str());
    histo_ratio -> SetMaximum(2);
    histo_ratio -> SetMinimum(0);
    histo_ratio -> SetMarkerColor(kBlack);
    histo_ratio -> SetMarkerSize(0.5);
    histo_ratio -> GetXaxis() -> SetLabelSize(0.07);
    histo_ratio -> GetYaxis() -> SetLabelSize(0.07);
    histo_ratio -> GetXaxis() -> SetTitleSize(0.07);
    histo_ratio -> GetYaxis() -> SetTitleSize(0.07);
    histo_ratio -> GetYaxis() -> SetTitleOffset(0.7);
    histo_ratio -> Draw("e");
    TF1* f_const = new TF1("f_1", "[0]",histo_ratio->GetBinCenter(1)-histo_ratio->GetBinWidth(1)/2, histo_ratio->GetBinCenter(histo_ratio->GetNbinsX())+histo_ratio->GetBinWidth(histo_ratio->GetNbinsX())/2);
    f_const -> FixParameter(0,1);
    f_const -> SetLineColor(kRed);
    f_const -> SetLineWidth(2);
    f_const -> Draw("same");
    
    gPad -> Update();
    st_ratio= (TPaveStats*)(histo_ratio->GetListOfFunctions()->FindObject("stats"));
    st_ratio->SetX1NDC(0.); //new x start position
    st_ratio->SetX2NDC(0.); //new x end position
    st_ratio->SetY1NDC(0.); //new y start position
    st_ratio->SetY2NDC(0.); //new y end position
    st_ratio->SetTextColor(kBlack);
    st_ratio->Draw("sames");
    
    cUp->cd();
    
    double KSprob = histo_DA -> KolmogorovTest(histo_MC,"");
    char KSbuffer[50];
    sprintf(KSbuffer,"prob_{K.S.} = %1.2e",KSprob);
    
    latex = new TLatex(0.4, 0.97, KSbuffer);
    latex -> SetNDC();
    latex -> SetTextFont(42);
    latex -> SetTextSize(0.04);
    latex -> Draw("same");
      
    double myChi2 = myChiSquareTest(histo_ratio);
    char myChi2buffer[50];
    sprintf(myChi2buffer,"#chi^{2} = %1.2e",myChi2);
    
    latex = new TLatex(0.62, 0.97, myChi2buffer);
    latex -> SetNDC();
    latex -> SetTextFont(42);
    latex -> SetTextSize(0.04);
    latex -> Draw("same");
     
    c1 -> Print((outputDir+"comp_h_"+xTitle+"_"+comp_type+".png").c_str(),"png");
    
    delete c1;
    delete histo_ratio;
}


void compareNHistos(std::string input_histo_variables, const float x_min, const float x_max ,std::string xTitle , std::string outputName){

    char file_name[1000];
    char histo_name[1000];
    char label[200];
    int color;
    float s_x_min = 0;
    float s_x_max = 0;
    float s_y_min = 0;
    float s_y_max = 0;
    
    std::map <int,TFile*> Files;
    std::map <int,TH1F*> Histos;
    std::map <int,std::string> LABEL;
    std::map <int,int> COLOR;
    std::map <int,float> Stat_x_min;
    std::map <int,float> Stat_x_max;
    std::map <int,float> Stat_y_min; 
    std::map <int,float> Stat_y_max;
    

    int pos = 0;
    int pos_total = 0;

    FILE *f_variables;
    
    f_variables = fopen(input_histo_variables.c_str(),"r");

    std::cout << "\nReading input_histo_variables: " << std::endl;

    while(fscanf(f_variables,"%s %s %s %d %f %f %f %f \n", file_name, histo_name, label, &color, &s_x_min, &s_x_max, &s_y_min, &s_y_max) !=EOF ){
          
          if(std::string(file_name).find("#") != std::string::npos) continue;
          
          Files[pos] = new TFile(file_name, "READ");
          Histos[pos] = (TH1F*)Files[pos] -> Get(histo_name);
          LABEL[pos] = label;
          COLOR[pos] = color;
          Stat_x_min[pos] = s_x_min;
          Stat_x_max[pos] = s_x_max;
          Stat_y_min[pos] = s_y_min;
          Stat_y_max[pos] = s_y_max;
          pos++;
    }
    
    pos_total = pos;
    
    double xNorm[pos_total-1];
    for(int ii = 1; ii < pos_total; ii++){
        xNorm[ii-1] = Histos[0]->Integral()/Histos[ii]->Integral()*Histos[0]->GetBinWidth(1)/Histos[ii]->GetBinWidth(1);  
        Histos[ii]->Scale(xNorm[ii-1]);
    }
    
    Histos[0] -> GetXaxis() -> SetLabelSize(0);
    Histos[0] -> GetYaxis() -> SetLabelSize(0.04);
    Histos[0] -> GetXaxis() -> SetTitleSize(0.05);
    Histos[0] -> GetYaxis() -> SetTitleSize(0.05);
    Histos[0] -> GetYaxis() -> SetTitleOffset(1.25);

    for(int ii = 0; ii < pos_total; ii++)
        Histos[ii] -> GetXaxis() -> SetTitle("");
        
    if(x_max != x_min) Histos[0] -> GetXaxis() -> SetRangeUser(x_min, x_max);
    
    char yTitle[50];
    sprintf(yTitle,"events");
    for(int ii = 0; ii < pos_total; ii++) 
        Histos[ii]-> GetYaxis() -> SetTitle(yTitle);
    
    Histos[0] -> SetMarkerColor(COLOR[0]);
    Histos[0] -> SetMarkerSize(0.5);
    Histos[0] -> SetMarkerStyle(20); 
    Histos[0] -> SetLineWidth(1);
    Histos[0] -> SetLineColor(COLOR[0]);
    
    Histos[1] -> SetLineColor(COLOR[1]);
    Histos[1] -> SetFillColor(COLOR[1]);
    Histos[1] -> SetFillStyle(3003);
    Histos[1] -> SetLineWidth(2);
    
    for(int ii = 2; ii < pos_total; ii++){ 

        Histos[ii] -> SetLineColor(COLOR[ii]);
        Histos[ii] -> SetLineWidth(2);
        Histos[ii] -> SetFillColor(0);
    }
    
    float maximum = -1.;
    if( Histos[0] -> GetMaximum() > maximum ) maximum = Histos[0] -> GetMaximum();
    Histos[0] -> SetMaximum( 1.1*maximum );
    Histos[0] -> SetMinimum( 0. );
    
    std::map<int,TPaveStats*> stat;
    std::map<int,TPaveStats*> stat_ratio;
    
    for(int ii = 0; ii < pos_total; ii++)
        stat[ii] = new TPaveStats();
    for(int ii = 0; ii < pos_total-1; ii++)
        stat_ratio[ii] = new TPaveStats();
    
    TCanvas* c1 = new TCanvas("c1","c1");
    c1 -> cd();
    
    TPad *cUp  = new TPad("pad_0","pad_0",0.00,0.36,1.00,1.00);
    TPad *cDown = new TPad("pad_1","pad_1",0.00,0.00,1.00,0.36);
    
    cUp->SetBottomMargin(0.01); 
    cDown->SetTopMargin(0.01); 
     
    cUp->Draw();
    cDown->Draw();
     
    cUp->cd();
   
    Histos[0] -> Draw("e");
    for(int ii = 1; ii < pos_total; ii++)
        Histos[ii] -> Draw("sames");
    
    for(int ii = 0; ii < pos_total; ii++){

        gPad -> Update();
        
        stat[ii] = (TPaveStats*)(Histos[ii]->GetListOfFunctions()->FindObject("stats"));
        stat[ii] ->SetX1NDC(Stat_x_min[ii]); //new x start position
        stat[ii] ->SetX2NDC(Stat_x_max[ii]); //new x end position
        stat[ii] ->SetY1NDC(Stat_y_min[ii]); //new y start position
        stat[ii] ->SetY2NDC(Stat_y_max[ii]); //new y end position
        stat[ii] ->SetTextColor(COLOR[ii]);
        stat[ii] ->Draw("sames");
    }
   
    TLegend* legend = new TLegend(0.82, 0.82, 0.99, 0.94);
    legend -> SetFillColor(kWhite);
    legend -> SetFillStyle(1000);
    legend -> SetLineWidth(0);
    legend -> SetLineColor(kWhite);
    legend -> SetTextFont(42);  
    legend -> SetTextSize(0.04);
    
    legend -> AddEntry(Histos[0],LABEL[0].c_str(),"P");
    legend -> AddEntry(Histos[1],LABEL[1].c_str(),"F");
    for(int ii = 2; ii < pos_total; ii++)
        legend -> AddEntry(Histos[ii],LABEL[ii].c_str(),"L");
    legend -> Draw("same");
    
    TLatex* latex = new TLatex(0.026, 0.97, xTitle.c_str());
    latex -> SetNDC();
    latex -> SetTextFont(kBlack);
    latex -> SetTextSize(0.04);
    latex -> Draw("same");
    
    cDown->cd();
    
    std::map<int,TH1F*> HistoRatios;
    
     
    for(int ii = 0; ii < pos_total-1; ii++){
        HistoRatios[ii] = (TH1F*)Histos[0]->Clone();
        HistoRatios[ii]->Sumw2();
        HistoRatios[ii]->Divide(Histos[ii+1]);
    }

    HistoRatios[0] -> GetXaxis() -> SetTitle(xTitle.c_str());
    HistoRatios[0] -> GetYaxis() -> SetTitle("ratio");
    HistoRatios[0] -> SetMaximum(2);
    HistoRatios[0] -> SetMinimum(0);
    HistoRatios[0] -> SetMarkerColor(COLOR[1]);
    HistoRatios[0] -> SetLineColor(COLOR[1]);
    HistoRatios[0] -> SetMarkerSize(0.5);
    HistoRatios[0] -> SetMarkerStyle(20);
    HistoRatios[0] -> GetXaxis() -> SetLabelSize(0.07);
    HistoRatios[0] -> GetYaxis() -> SetLabelSize(0.07);
    HistoRatios[0] -> GetXaxis() -> SetTitleSize(0.07);
    HistoRatios[0] -> GetYaxis() -> SetTitleSize(0.07);
    HistoRatios[0] -> GetYaxis() -> SetTitleOffset(0.7);
    HistoRatios[0] -> Draw("e");
    
    for(int ii = 1; ii < pos_total-1; ii++){
        HistoRatios[ii] -> SetMaximum(2);
        HistoRatios[ii] -> SetMinimum(0);
        HistoRatios[ii] -> SetMarkerColor(COLOR[ii+1]);
        HistoRatios[ii] -> SetLineColor(COLOR[ii+1]);
        HistoRatios[ii] -> SetMarkerSize(0.5);
        HistoRatios[ii] -> SetMarkerStyle(20);
        HistoRatios[ii] -> Draw("esame");
    }
   
    TF1* f_const = new TF1("f_1", "[0]",HistoRatios[0]->GetBinCenter(1)-HistoRatios[0]->GetBinWidth(1)/2, HistoRatios[0]->GetBinCenter(HistoRatios[0]->GetNbinsX())+HistoRatios[0]->GetBinWidth(HistoRatios[0]->GetNbinsX())/2);
    f_const -> FixParameter(0,1);
    f_const -> SetLineColor(kBlack);
    f_const -> SetLineWidth(2);
    f_const -> Draw("same");
    
    for(int ii = 0; ii < pos_total-1; ii++){

        gPad -> Update();
        stat_ratio[ii]= (TPaveStats*)(HistoRatios[ii]->GetListOfFunctions()->FindObject("stats"));
        stat_ratio[ii]->SetX1NDC(0.); //new x start position
        stat_ratio[ii]->SetX2NDC(0.); //new x end position
        stat_ratio[ii]->SetY1NDC(0.); //new y start position
        stat_ratio[ii]->SetY2NDC(0.); //new y end position
        stat_ratio[ii]->Draw("sames");
    }
    
    c1 -> Print(outputName.c_str(),"png");
   
}

void compareProfile(TProfile* h1, TProfile* h2, std::string xTitle ,std::string yTitle, std::string label_DA, std::string label_MC, const float y_min, const float y_max, std::string comp_type ,std::string outputDir){
    
    std::string Name_1 = std::string(h1->GetName())+"_DA";
    int nBins_1 = h1 -> GetNbinsX(); 
    float bin_min_1 = h1 -> GetXaxis() ->GetXmin();
    float bin_max_1 = h1 -> GetXaxis() ->GetXmax();
    TH1F* histo_DA = new TH1F(Name_1.c_str(), Name_1.c_str(), nBins_1, bin_min_1, bin_max_1 );
    
    std::string Name_2 = std::string(h2 ->GetName())+"_MC";
    int nBins_2 = h2 -> GetNbinsX(); 
    float bin_min_2 = h2 -> GetXaxis() ->GetXmin();
    float bin_max_2 = h2 -> GetXaxis() ->GetXmax();
    TH1F* histo_MC = new TH1F(Name_2.c_str(), Name_2.c_str(), nBins_2,bin_min_2, bin_max_2 );
 
    bool isDrawP_DA = true;
    bool isDrawP_MC = true;

    for(int ii = 1; ii <= h1 -> GetNbinsX(); ii++) histo_DA->SetBinContent(ii, h1->GetBinContent(ii));
    for(int ii = 1; ii <= h1 -> GetNbinsX(); ii++) histo_DA->SetBinError(ii,   h1->GetBinError(ii));
    for(int ii = 1; ii <= h2 -> GetNbinsX(); ii++) histo_MC->SetBinContent(ii, h2->GetBinContent(ii));
    for(int ii = 1; ii <= h2 -> GetNbinsX(); ii++) histo_MC->SetBinError(ii,   h2->GetBinError(ii));
     
    for(int ii = 1; ii <= histo_DA -> GetNbinsX(); ii++) 
        if(histo_DA->GetBinError(ii) != 0) isDrawP_DA = false;

    for(int ii = 1; ii <= histo_MC -> GetNbinsX(); ii++) 
        if(histo_MC->GetBinError(ii) != 0) isDrawP_MC = false;
    
    histo_DA -> GetXaxis() -> SetLabelSize(0);
    histo_DA -> GetYaxis() -> SetLabelSize(0.04);
    histo_DA -> GetXaxis() -> SetTitleSize(0.05);
    histo_DA -> GetYaxis() -> SetTitleSize(0.05);
    histo_DA -> GetYaxis() -> SetTitleOffset(1.25);

    histo_DA -> GetXaxis() -> SetTitle("");
    histo_MC -> GetXaxis() -> SetTitle("");
    
    histo_DA -> GetYaxis() -> SetTitle(yTitle.c_str());
    histo_MC -> GetYaxis() -> SetTitle(yTitle.c_str());
   
    histo_DA -> GetYaxis() -> SetRangeUser(y_min,y_max);
    histo_MC -> GetYaxis() -> SetRangeUser(y_min, y_max);
    
    histo_MC -> SetLineColor(kRed);
    histo_MC -> SetMarkerColor(kRed);
    histo_MC -> SetMarkerSize(0.5);

    histo_DA -> SetMarkerColor(kBlack);
    histo_DA -> SetMarkerSize(0.5);
    
    histo_MC -> SetLineWidth(2);
    histo_DA -> SetLineWidth(1);
    
    TCanvas* c1 = new TCanvas("c1","c1");
    c1 -> cd();
    
    
    TPaveStats* st_DA = new TPaveStats();
    TPaveStats* st_MC = new TPaveStats();
    TPaveStats* st_ratio = new TPaveStats();

    TPad *cUp  = new TPad("pad_0","pad_0",0.00,0.36,1.00,1.00);
    TPad *cDown = new TPad("pad_1","pad_1",0.00,0.00,1.00,0.36);

    cUp->SetBottomMargin(0.01); 
    cDown->SetTopMargin(0.01); 
    
    cUp->Draw();
    cDown->Draw();
     
    cUp->cd();
    
    if(isDrawP_DA == false) histo_DA -> Draw("e");
    else histo_DA -> Draw("p");
    gPad -> Update();
    st_DA= (TPaveStats*)(histo_DA->GetListOfFunctions()->FindObject("stats"));
    st_DA->SetX1NDC(0.82); //new x start position
    st_DA->SetX2NDC(0.99); //new x end position
    st_DA->SetY1NDC(0.70); //new y start position
    st_DA->SetY2NDC(0.82); //new y end position
    st_DA->SetTextColor(kBlack);
    st_DA->Draw("sames");
    
    if(isDrawP_MC == false) histo_MC -> Draw("esames");
    else histo_MC -> Draw("psames");
    gPad -> Update();
    st_MC= (TPaveStats*)(histo_MC->GetListOfFunctions()->FindObject("stats"));
    st_MC->SetX1NDC(0.82); //new x start position
    st_MC->SetX2NDC(0.99); //new x end position
    st_MC->SetY1NDC(0.56); //new y start position
    st_MC->SetY2NDC(0.68); //new y end position
    st_MC->SetTextColor(kRed);
    st_MC->Draw("sames");
    
    TLegend* legend = new TLegend(0.82, 0.82, 0.99, 0.94);
    legend -> SetFillColor(kWhite);
    legend -> SetFillStyle(1000);
    legend -> SetLineWidth(0);
    legend -> SetLineColor(kWhite);
    legend -> SetTextFont(42);  
    legend -> SetTextSize(0.04);
    
    legend -> AddEntry(histo_DA,label_DA.c_str(),"P");
    legend -> AddEntry(histo_MC,label_MC.c_str(),"P");
    legend -> Draw("same");
    
    std::string title = std::string(histo_DA->GetName());

    title.erase(title.size()-6,6);
    title.erase(0,2);
    
    TLatex* latex = new TLatex(0.026, 0.97, title.c_str());
    latex -> SetNDC();
    latex -> SetTextFont(kBlack);
    latex -> SetTextSize(0.04);
    latex -> Draw("same");
    
    cDown->cd();
    
    std::string Name_r = std::string("histo_ratio");
    int nBins_r = histo_DA -> GetNbinsX(); 
    float bin_min_r = histo_DA -> GetXaxis() ->GetXmin();
    float bin_max_r = histo_DA -> GetXaxis() ->GetXmax();   

    TH1F* histo_ratio = new TH1F(Name_r.c_str(), Name_r.c_str(), nBins_r, bin_min_r, bin_max_r );
 
    for(int ii = 1; ii <= histo_DA -> GetNbinsX(); ii++){

        if(histo_MC->GetBinContent(ii) == 0) histo_ratio->SetBinContent(ii,-1);
        else histo_ratio->SetBinContent(ii, histo_DA->GetBinContent(ii)/histo_MC->GetBinContent(ii));
    }

    for(int ii = 1; ii <= histo_DA -> GetNbinsX(); ii++){

        float error = sqrt(pow(histo_DA->GetBinError(ii)/histo_MC->GetBinContent(ii),2)+pow(histo_MC->GetBinError(ii),2)*pow(histo_DA->GetBinContent(ii)/(histo_MC->GetBinContent(ii)*histo_MC->GetBinContent(ii)),2));

        if(histo_MC->GetBinContent(ii) == 0) histo_ratio->SetBinError(ii,0);
        else histo_ratio->SetBinError(ii,error );
    }
 
    std::string histo_ratio_label = label_DA+"/"+label_MC;

    histo_ratio -> GetXaxis() -> SetTitle(xTitle.c_str());
    histo_ratio -> GetYaxis() -> SetTitle(histo_ratio_label.c_str());
    histo_ratio -> SetMaximum(2);
    histo_ratio -> SetMinimum(0);
    histo_ratio -> SetMarkerColor(kBlack);
    histo_ratio -> SetMarkerSize(0.5);
    histo_ratio -> GetXaxis() -> SetLabelSize(0.07);
    histo_ratio -> GetYaxis() -> SetLabelSize(0.07);
    histo_ratio -> GetXaxis() -> SetTitleSize(0.07);
    histo_ratio -> GetYaxis() -> SetTitleSize(0.07);
    histo_ratio -> GetYaxis() -> SetTitleOffset(0.7);
    histo_ratio -> Draw("e");
    TF1* f_const = new TF1("f_1", "[0]",histo_ratio->GetBinCenter(1)-histo_ratio->GetBinWidth(1)/2, histo_ratio->GetBinCenter(histo_ratio->GetNbinsX())+histo_ratio->GetBinWidth(histo_ratio->GetNbinsX())/2);
    f_const -> FixParameter(0,1);
    f_const -> SetLineColor(kRed);
    f_const -> SetLineWidth(2);
    f_const -> Draw("same");

    gPad -> Update();
    st_ratio= (TPaveStats*)(histo_ratio->GetListOfFunctions()->FindObject("stats"));
    st_ratio->SetX1NDC(0.); //new x start position
    st_ratio->SetX2NDC(0.); //new x end position
    st_ratio->SetY1NDC(0.); //new y start position
    st_ratio->SetY2NDC(0.); //new y end position
    st_ratio->SetTextColor(kBlack);
    st_ratio->Draw("sames");

    cUp->cd();
    double myChi2 = myChiSquareTest(histo_ratio);
    char myChi2buffer[50];
    sprintf(myChi2buffer,"#chi^{2} = %1.2e",myChi2);
    
    latex = new TLatex(0.62, 0.97, myChi2buffer);
    latex -> SetNDC();
    latex -> SetTextFont(42);
    latex -> SetTextSize(0.04);
    latex -> Draw("same");
    
    c1 -> Print((outputDir+"comp_h_"+title+"_"+comp_type+".png").c_str(),"png");
    
    delete c1;
    delete histo_ratio;
    
}

void compareNProfiles(std::string input_profile_variables, const float x_min, const float x_max ,const float y_min, const float y_max, std::string xTitle , std::string yTitle, std::string outputName){

    char file_name[1000];
    char profile_name[1000];
    char label[200];
    int color;
    float s_x_min = 0;
    float s_x_max = 0;
    float s_y_min = 0;
    float s_y_max = 0;
    
    std::map <int,TFile*> Files;
    std::map <int,TProfile*> Profiles;
    std::map <int,std::string> LABEL;
    std::map <int,int> COLOR;
    std::map <int,float> Stat_x_min;
    std::map <int,float> Stat_x_max;
    std::map <int,float> Stat_y_min; 
    std::map <int,float> Stat_y_max;
    

    int pos = 0;
    int pos_total = 0;

    FILE *f_variables;
    
    f_variables = fopen(input_profile_variables.c_str(),"r");

    std::cout << "\nReading input_profile_variables: " << std::endl;

    while(fscanf(f_variables,"%s %s %s %d %f %f %f %f \n", file_name, profile_name, label, &color, &s_x_min, &s_x_max, &s_y_min, &s_y_max) !=EOF ){
          
          if(std::string(file_name).find("#") != std::string::npos) continue;
          
          Files[pos] = new TFile(file_name, "READ");
          Profiles[pos] = (TProfile*)Files[pos] -> Get(profile_name);
          LABEL[pos] = label;
          COLOR[pos] = color;
          Stat_x_min[pos] = s_x_min;
          Stat_x_max[pos] = s_x_max;
          Stat_y_min[pos] = s_y_min;
          Stat_y_max[pos] = s_y_max;
          pos++;
    }
    
    pos_total = pos;
    
    std::map<int,TH1F*> Histos;
    std::map<int,bool> isDrawP;
    
    for(int ii = 0; ii < pos_total; ii++)
        isDrawP[ii] = true;
    
    for(int ii = 0; ii < pos_total; ii++){

        int nBins = Profiles[ii] -> GetNbinsX(); 
        float bin_min = Profiles[ii] -> GetXaxis() ->GetXmin();
        float bin_max = Profiles[ii] -> GetXaxis() ->GetXmax();
        Histos[ii] = new TH1F("", "" , nBins, bin_min, bin_max);
    }
    
    for(int ii = 0; ii < pos_total; ii++){
        
        for(int jj = 1; jj <= Profiles[ii] -> GetNbinsX(); jj++) 
            Histos[ii]->SetBinContent(jj,Profiles[ii]->GetBinContent(jj));
        for(int jj = 1; jj <= Profiles[ii] -> GetNbinsX(); jj++) 
            Histos[ii]->SetBinError(jj,Profiles[ii]->GetBinError(jj));
    }
    
    for(int ii = 0; ii < pos_total; ii++)
        for(int jj = 1; jj <= Profiles[ii] -> GetNbinsX(); jj++)
            if(Histos[ii]->GetBinError(jj) != 0) isDrawP[ii] = false;    
        
    Histos[0] -> GetXaxis() -> SetLabelSize(0);
    Histos[0] -> GetYaxis() -> SetLabelSize(0.04);
    Histos[0] -> GetXaxis() -> SetTitleSize(0.05);
    Histos[0] -> GetYaxis() -> SetTitleSize(0.05);
    Histos[0] -> GetYaxis() -> SetTitleOffset(1.25);
   
    for(int ii = 0; ii < pos_total; ii++){
 
        Histos[ii] -> GetXaxis() -> SetTitle("");
        Histos[ii] -> GetYaxis() -> SetTitle(yTitle.c_str());
        Histos[ii] -> SetMarkerColor(COLOR[ii]);
        Histos[ii] -> SetLineColor(COLOR[ii]);
        Histos[ii] -> SetMarkerSize(0.5);
        Histos[ii] -> SetMarkerStyle(20); 
    }

    if(x_max != x_min) Histos[0] -> GetXaxis() -> SetRangeUser(x_min, x_max);
    if(y_max != y_min) Histos[0] -> GetYaxis() -> SetRangeUser(y_min, y_max);  
    std::map<int,TPaveStats*> stat;
    std::map<int,TPaveStats*> stat_ratio;
    
    for(int ii = 0; ii < pos_total; ii++)
        stat[ii] = new TPaveStats();
    for(int ii = 0; ii < pos_total-1; ii++)
        stat_ratio[ii] = new TPaveStats();
    
    TCanvas* c1 = new TCanvas("c1","c1");
    c1 -> cd();
    
    TPad *cUp  = new TPad("pad_0","pad_0",0.00,0.36,1.00,1.00);
    TPad *cDown = new TPad("pad_1","pad_1",0.00,0.00,1.00,0.36);
    
    cUp->SetBottomMargin(0.01); 
    cDown->SetTopMargin(0.01); 
     
    cUp->Draw();
    cDown->Draw();
     
    cUp->cd();
   
    if(isDrawP[0] == false) Histos[0] -> Draw("e");
    else Histos[0] -> Draw("P");
    for(int ii = 1; ii < pos_total; ii++){
        
        if(isDrawP[ii] == false) Histos[ii] -> Draw("esames");
        else Histos[ii] -> Draw("psames");
    }
    
    for(int ii = 0; ii < pos_total; ii++){

        gPad -> Update();
        
        stat[ii] = (TPaveStats*)(Histos[ii]->GetListOfFunctions()->FindObject("stats"));
        stat[ii] ->SetX1NDC(Stat_x_min[ii]); //new x start position
        stat[ii] ->SetX2NDC(Stat_x_max[ii]); //new x end position
        stat[ii] ->SetY1NDC(Stat_y_min[ii]); //new y start position
        stat[ii] ->SetY2NDC(Stat_y_max[ii]); //new y end position
        stat[ii] ->SetTextColor(COLOR[ii]);
        stat[ii] ->Draw("sames");
    }
   
    TLegend* legend = new TLegend(0.82, 0.82, 0.99, 0.94);
    legend -> SetFillColor(kWhite);
    legend -> SetFillStyle(1000);
    legend -> SetLineWidth(0);
    legend -> SetLineColor(kWhite);
    legend -> SetTextFont(42);  
    legend -> SetTextSize(0.04);
    
    for(int ii = 0; ii < pos_total; ii++)
        legend -> AddEntry(Histos[ii],LABEL[ii].c_str(),"P");
    legend -> Draw("same");
    
    TLatex* latex = new TLatex(0.026, 0.97, yTitle.c_str());
    latex -> SetNDC();
    latex -> SetTextFont(kBlack);
    latex -> SetTextSize(0.04);
    latex -> Draw("same");
    
    cDown->cd();
    
    std::map<int,TH1F*> HistoRatios;
    
    for(int ii = 0; ii < pos_total-1; ii++){
        int nBins_1 = Histos[0] -> GetNbinsX(); 
        float bin_min_1 = Histos[0] -> GetXaxis() ->GetXmin();
        float bin_max_1 = Histos[0] -> GetXaxis() ->GetXmax();
        HistoRatios[ii] = new TH1F("", "" , nBins_1, bin_min_1, bin_max_1 );
    } 
  
    for(int ii = 0; ii < pos_total-1; ii++){
        for(int jj = 1; jj <= HistoRatios[ii] -> GetNbinsX(); jj++){
            if(Histos[ii+1]->GetBinContent(jj) == 0) HistoRatios[ii]->SetBinContent(jj,-1);
            else HistoRatios[ii]->SetBinContent(jj, Histos[0]->GetBinContent(jj)/Histos[ii+1]->GetBinContent(jj));
        }
    }
  
    for(int ii = 0; ii < pos_total-1; ii++){
        for(int jj = 1; jj <= HistoRatios[ii] -> GetNbinsX(); jj++){
            float error = sqrt(pow(Histos[0]->GetBinError(jj)/Histos[ii+1]->GetBinContent(jj),2)+pow(Histos[ii+1]->GetBinError(jj),2)*pow(Histos[0]->GetBinContent(jj)/(Histos[ii+1]->GetBinContent(jj)*Histos[ii+1]->GetBinContent(jj)),2));
             
             if(Histos[ii+1]->GetBinContent(jj) == 0) HistoRatios[ii]->SetBinError(jj,0);
             else HistoRatios[ii]->SetBinError(jj,error );
            
        }
    }
   
    HistoRatios[0] -> GetXaxis() -> SetTitle(xTitle.c_str());
    HistoRatios[0] -> GetYaxis() -> SetTitle("ratio");
    HistoRatios[0] -> GetXaxis() -> SetLabelSize(0.07);
    HistoRatios[0] -> GetYaxis() -> SetLabelSize(0.07);
    HistoRatios[0] -> GetXaxis() -> SetTitleSize(0.07);
    HistoRatios[0] -> GetYaxis() -> SetTitleSize(0.07);
    HistoRatios[0] -> GetYaxis() -> SetTitleOffset(0.7);
    HistoRatios[0] -> SetMaximum(2);
    HistoRatios[0] -> SetMinimum(0);
    for(int ii = 0; ii < pos_total-1; ii++){

        HistoRatios[ii] -> SetMarkerColor(COLOR[ii+1]);
        HistoRatios[ii] -> SetLineColor(COLOR[ii+1]);
        HistoRatios[ii] -> SetMarkerSize(0.5);
        HistoRatios[ii] -> SetMarkerStyle(20);
    }
    
    HistoRatios[0] -> Draw("e");
    for(int ii = 1; ii < pos_total-1; ii++)
        HistoRatios[ii] -> Draw("esames");
    
    TF1* f_const = new TF1("f_1", "[0]",HistoRatios[0]->GetBinCenter(1)-HistoRatios[0]->GetBinWidth(1)/2, HistoRatios[0]->GetBinCenter(HistoRatios[0]->GetNbinsX())+HistoRatios[0]->GetBinWidth(HistoRatios[0]->GetNbinsX())/2);
    f_const -> FixParameter(0,1);
    f_const -> SetLineColor(kBlack);
    f_const -> SetLineWidth(2);
    f_const -> Draw("same");
    
    for(int ii = 0; ii < pos_total-1; ii++){

        gPad -> Update();
        stat_ratio[ii]= (TPaveStats*)(HistoRatios[ii]->GetListOfFunctions()->FindObject("stats"));
        stat_ratio[ii]->SetX1NDC(0.); //new x start position
        stat_ratio[ii]->SetX2NDC(0.); //new x end position
        stat_ratio[ii]->SetY1NDC(0.); //new y start position
        stat_ratio[ii]->SetY2NDC(0.); //new y end position
        stat_ratio[ii]->Draw("sames");
    }
    
    c1 -> Print(outputName.c_str(),"png");
   
}

void drawHisto(TH1F* h1, const float x_min , const float x_max, std::string label, std::string xTitle, std::string yTitle, std::string outputDir){
	
    TCanvas* c1 = new TCanvas("c1","c1");
    c1 -> cd();
    c1 -> SetGridx();
    c1 -> SetGridy();
    
    h1 -> GetXaxis() -> SetLabelSize(0.04);
    h1 -> GetYaxis() -> SetLabelSize(0.04);
    h1 -> GetXaxis() -> SetTitleSize(0.05);
    h1 -> GetYaxis() -> SetTitleSize(0.05);
    h1 -> GetYaxis() -> SetTitleOffset(1.25);

    h1 -> GetXaxis() -> SetTitle(xTitle.c_str());
    if(x_min != 0 && x_max != 0) h1 -> GetXaxis() -> SetRangeUser(x_min,x_max);

    h1 -> GetYaxis() -> SetTitle(yTitle.c_str());
    
    h1 -> SetLineColor(kBlack);
    h1 -> SetMarkerColor(kBlack);
    
    h1 -> Draw("e");
    
    TLegend* legend = new TLegend(0.82, 0.82, 0.99, 0.94);
    legend -> SetFillColor(kWhite);
    legend -> SetFillStyle(1000);
    legend -> SetLineWidth(0);
    legend -> SetLineColor(kWhite);
    legend -> SetTextFont(42);  
    legend -> SetTextSize(0.04);
    
    legend -> AddEntry(h1,label.c_str(),"L");
    legend -> Draw("same");
    
    
    c1 -> Print((outputDir+std::string(h1->GetName())+".png").c_str(),"png");
    
    delete c1;
}

void drawProfile(TProfile* h1, const float x_min , const float x_max , const float y_min , const float y_max , std::string label, std::string xTitle, std::string yTitle, std::string outputDir ){
	
    TCanvas* c1 = new TCanvas("c1","c1"); 
    c1 -> cd();
    c1 -> SetGridx();
    c1 -> SetGridy();
    
    h1 -> GetXaxis() -> SetLabelSize(0.04);
    h1 -> GetYaxis() -> SetLabelSize(0.04);
    h1 -> GetXaxis() -> SetTitleSize(0.05);
    h1 -> GetYaxis() -> SetTitleSize(0.05);
    h1 -> GetYaxis() -> SetTitleOffset(1.25);

    h1 -> GetXaxis() -> SetTitle(xTitle.c_str());
    if(x_min != 0 && x_max != 0) h1 -> GetXaxis() -> SetRangeUser(x_min,x_max);
    
    h1 -> GetYaxis() -> SetTitle(yTitle.c_str());
    if(y_min != 0 && y_max != 0) h1 -> GetYaxis() -> SetRangeUser(y_min,y_max);
    
    h1 -> SetLineColor(kBlack);
    h1 -> SetMarkerColor(kBlack);
    
    h1 -> Draw("e");
    
    TLegend* legend = new TLegend(0.82, 0.82, 0.99, 0.94);
    legend -> SetFillColor(kWhite);
    legend -> SetFillStyle(1000);
    legend -> SetLineWidth(0);
    legend -> SetLineColor(kWhite);
    legend -> SetTextFont(42);  
    legend -> SetTextSize(0.04);
    
    legend -> AddEntry(h1,label.c_str(),"L");
    legend -> Draw("same");
    
    
    c1 -> Print((outputDir+std::string(h1->GetName())+".png").c_str(),"png");
    
    delete c1;
}

double myChiSquareTest(TH1F* h1)
{
  double val = 0.;
  int n_points = 0;
  
  for(int bin = 1; bin <= h1->GetNbinsX(); ++bin)
  {
    if( h1->GetBinContent(bin) <= 0.) continue;
    val += (fabs(h1->GetBinContent(bin)-1)) / h1->GetBinError(bin);
    n_points++;
  }

  if(n_points++ != 0) return val/n_points++;
  else return 0;
}

