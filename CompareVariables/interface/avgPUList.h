#ifndef avgPUList_h
#define avgPUList_h

#include <fstream>
#include <string>
#include <map>



class avgPUList
{
 public:
  
  //! ctor
  avgPUList(const std::string& listFileName);
  
  //! methods
  float GetAvgPU(const int& run, const int& lumi) { return (_map[run])[lumi]; };
  
  
 private:
  
  std::map<int, std::map<int,float> > _map;
};

#endif
