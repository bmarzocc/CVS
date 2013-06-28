#include "avgPUList.h"



avgPUList::avgPUList(const std::string& listFileName)
{
  std::ifstream listFile(listFileName.c_str(),std::ios::in);
  
  
  int run, lumi;
  float avgPU;
  
  while(1)
  {
    listFile >> run >> lumi >> avgPU;
    if( !listFile.good() ) break;
    
    (_map[run])[lumi] = avgPU;  
  }
}
