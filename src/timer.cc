#include <fstream>

#include "timer.h"
#include "taskmanager.h"


namespace ASC_HPC
{
  thread_local std::unique_ptr<TimeLine> timeline;
  std::mutex TimeLine::timeline_mutex;
  std::mutex Timer::m;
  std::vector<std::string> Timer::names;
  std::vector<std::array<float,3>> Timer::cols;      
  // int Timer::cnt = 0;


  TimeLine :: TimeLine(std::string _filename)
    : filename(_filename)
  {
    events.reserve(1000*1000);
    start = GetTimeCounter();
    start_time = std::chrono::high_resolution_clock::now();
  }
  
  TimeLine :: ~TimeLine()
  {
    if (filename != "")
      {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto end = GetTimeCounter();
        auto duration = end_time-start_time;
        
        std::cout << "total time = "
                  << std::chrono::duration_cast<std::chrono::microseconds>(duration).count()
                  << " microsec" << std::endl;

        double fac = 1e-3*double(std::chrono::duration_cast<std::chrono::microseconds>(duration).count()) / double(end-start);
        
        std::cout << "write pajefile '" << filename << "'" << std::endl;

        /*
          documentation of paje-format:
          https://paje.sourceforge.net/download/publication/lang-paje.pdf
         */
        
        std::ofstream file(filename);
        file << R"(
%EventDef PajeDefineContainerType 0 
%       Alias string 
%       Type string 
%       Name string 
%EndEventDef 
%EventDef PajeDefineVariableType 1 
%       Alias string 
%       Type string 
%       Name string 
%       Color color 
%EndEventDef 
%EventDef PajeDefineStateType 2 
%       Alias string 
%       Type string 
%       Name string 
%EndEventDef 
%EventDef PajeDefineEventType 3 
%       Alias string 
%       Type string 
%       Name string 
%       Color color 
%EndEventDef 
%EventDef PajeDefineLinkType 4 
%       Alias string 
%       Type string 
%       StartContainerType string 
%       EndContainerType string 
%       Name string 
%EndEventDef 
%EventDef PajeDefineEntityValue 5 
%       Alias string 
%       Type string 
%       Name string 
%       Color color 
%EndEventDef 
%EventDef PajeCreateContainer 6 
%       Time date 
%       Alias string 
%       Type string 
%       Container string 
%       Name string 
%EndEventDef 
%EventDef PajeDestroyContainer 7 
%       Time date 
%       Type string 
%       Name string 
%EndEventDef 
%EventDef PajeSetVariable 8 
%       Time date 
%       Type string 
%       Container string 
%       Value double 
%EndEventDef
%EventDef PajeAddVariable 9 
%       Time date 
%       Type string 
%       Container string 
%       Value double 
%EndEventDef
%EventDef PajeSubVariable 10 
%       Time date 
%       Type string 
%       Container string 
%       Value double 
%EndEventDef
%EventDef PajeSetState 11 
%       Time date 
%       Type string 
%       Container string 
%       Value string 
%EndEventDef
%EventDef PajePushState 12 
%       Time date 
%       Type string 
%       Container string 
%       Value string 
%       Id string 
%EndEventDef
%EventDef PajePopState 13 
%       Time date 
%       Type string 
%       Container string 
%EndEventDef
%EventDef PajeResetState 14 
%       Time date 
%       Type string 
%       Container string 
%EndEventDef
%EventDef PajeStartLink 15 
%       Time date 
%       Type string 
%       Container string 
%       Value string 
%       StartContainer string 
%       Key string 
%EndEventDef
%EventDef PajeEndLink 16 
%       Time date 
%       Type string 
%       Container string 
%       Value string 
%       EndContainer string 
%       Key string 
%EndEventDef
%EventDef PajeNewEvent 17 
%       Time date 
%       Type string 
%       Container string 
%       Value string 
%EndEventDef


0	main	0	"Task Manager"
0	thds	main	"Thread"
2	thdstate 	thds	"Task"
6	0	a9	main	0	"Paje"
)";

        for (int i = 0; i <= subtl.size(); i++)
          file << "6 0 th" << i << " thds a9 \"Thread " << i << "\"" << std::endl;

        for (size_t i = 0; i < Timer::names.size(); i++)
          {
            auto col = Timer::cols[i];
            file << "5 timer" << i << " thdstate \"" << Timer::names[i] << "\"  \"" << col[0] << " " << col[1] << " " << col[2] << "\"" << std::endl;
          }
        
        for (int i = 0; i <= subtl.size(); i++)
          {
            auto * tl = (i==0) ? this : &subtl[i-1];
            for (auto e : tl->events)
              {
                file << ((e.what==0) ? 12 : 13) << " ";
                file << fac*(e.when-start) << " thdstate th" << i << " ";
                if (e.what == 0)
                  file << "timer" << e.timer << " idx ";
                file << std::endl;
              }
          }
      }
  }

  
  
  void TimeLine :: Print (std::ostream & ost) const
  {
    ost << "ending timeline:" << std::endl;
    for (auto e : events)
      ost << e.when-start << ", " << e.timer << ", " << e.what << std::endl;
  }
}
