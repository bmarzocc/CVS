#include"TEndcapRings.h"



//-----
// ctor

TEndcapRings::TEndcapRings(const std::string& input)
{
  // initialization
  for(int ix = 1; ix <= 100; ++ix)
    for(int iy = 1; iy <= 100; ++iy)
      for(int iz = 0; iz <= 1; ++iz)
        iEndcapRing[ix][iy][iz] = -1;
  
  FILE *fRing;
  fRing = fopen(input.c_str(),"r");
  std::cout << "Inizializing endcap geometry from: eerings.dat" << std::endl;
  int ix,iy,iz,ir;
  while(fscanf(fRing,"(%d,%d,%d) %d \n",&ix,&iy,&iz,&ir) !=EOF )
    {
      if( iz < 0 ) iz = 0; 
      iEndcapRing[ix][iy][iz] = ir;
    }
  
  return;
}

//-----
// dtor

TEndcapRings::~TEndcapRings()
{
  return;
}



//--------
// methods

int TEndcapRings::GetEndcapRing(int ix, int iy, int iz)
{
  int iSide = iz; 
  if( iSide < 0 ) iSide = 0; 
  return iEndcapRing[ix][iy][iSide];
}

int TEndcapRings::GetEndcapIeta(int ix, int iy, int iz)
{
  int iSide = iz; 
  if( iSide < 0 ) iSide = 0;
  int iEtaOffset = 86*iz; 
  int iEta = iEtaOffset + iz*iEndcapRing[ix][iy][iSide];
  
  return iEta;
}

int TEndcapRings::GetEndcapIphi(int ix, int iy, int iz)
{
  double iX = ix-50.;
  double iY = iy-50.;
  int iPhi;
  if( iY >= 0 ) iPhi =  90 + int( TMath::ATan(iY/iX)*360. / (2.*TMath::Pi()) );
  else          iPhi = 270 + int( TMath::ATan(iY/iX)*360. / (2.*TMath::Pi()) );
  
  return iPhi;
}
