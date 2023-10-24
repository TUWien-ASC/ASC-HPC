#ifndef TIMER_H
#define TIMER_H

#include<iostream>
#include<string>
#include<memory>
#include <vector>
#include <array>
#include <chrono>
#include <mutex>
#include <thread>
#include <functional>
#include <algorithm>



#if defined(__APPLE__)
#include <mach/mach_time.h>
#endif

#if defined(__amd64__) || defined(_M_AMD64)
#ifdef WIN32
#include <intrin.h>   // for __rdtsc()  CPU time step counter
#else
#include <x86intrin.h>   // for __rdtsc()  CPU time step counter
#endif // WIN32
#endif




namespace ASC_HPC
{


  inline size_t GetTimeCounter() 
  {
#if defined(__APPLE__)
    return mach_absolute_time();
#endif
    
#if defined(__amd64__) || defined(_M_AMD64)
    return __rdtsc();    
#endif
    
    return std::chrono::high_resolution_clock::now().time_since_epoch().count();
    
    /*
    // copied from ngcore/utils
    #if defined(__APPLE__) && defined(NETGEN_ARCH_ARM64)
    return mach_absolute_time();
    #elif defined(NETGEN_ARCH_AMD64)
    return __rdtsc();
    #elif defined(NETGEN_ARCH_ARM64) && defined(__GNUC__)
    // __GNUC__ is also defined by CLANG. Use inline asm to read Generic Timer
    unsigned long long tics;
    __asm __volatile("mrs %0, CNTVCT_EL0" : "=&r" (tics));
    return tics;
    #elif defined(__EMSCRIPTEN__)
    return std::chrono::high_resolution_clock::now().time_since_epoch().count();
    #else
    #warning "Unsupported CPU architecture"
    return 0;
    #endif
    */
  }


  struct Event
  {
    size_t when;
    int timer;
    int what; // 0..start, 1..stop
  };


  class TimeLine
  {
    size_t start;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    std::vector<Event> events;
    std::vector<TimeLine> subtl;
    static std::mutex timeline_mutex;
    std::string filename;
  public:
    TimeLine(std::string _filename = "");
    TimeLine(const TimeLine&) = default;    
    TimeLine(TimeLine&&) = default;
    ~TimeLine();
    
    void Add (Event event)
    {
      events.push_back(event);
    }

    void AddTimeLine (TimeLine sub)
    {
      std::lock_guard<std::mutex> lock(timeline_mutex);      
      subtl.push_back(std::move(sub));
    }

    void Print (std::ostream & ost) const;
  };
  
  extern thread_local std::unique_ptr<TimeLine> timeline;

  
  class Timer
  {
    int nr;
    static std::vector<std::string> names;
    static std::vector<std::array<float,3>> cols;    
    static std::mutex m;
  public:     
    Timer(const std::string & name, std::array<float,3> col = { 0, 1, 0})
    {
      std::lock_guard<std::mutex> lock(m);            
      nr = names.size();
      names.push_back(name);
      cols.push_back(col);
    }

    void Start()
    {
      if (timeline)
        timeline->Add (Event{GetTimeCounter(), nr, 0});
    }

    void Stop()
    {
      if (timeline)
        timeline->Add(Event{GetTimeCounter(), nr, 1});
    }
    friend TimeLine;
  };


  class RegionTimer
  {
    Timer & t;
  public:
    RegionTimer (Timer & _t) : t(_t)
    {
      t.Start();
    }
    ~RegionTimer ()
    {
      t.Stop();
    }
  };

}

#endif
