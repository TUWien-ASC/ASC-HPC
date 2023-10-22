#include <chrono>
#include <thread>

#include <concurrentqueue.h>

#include "taskmanager.h"
#include "timer.h"


namespace ASC_HPC
{

  std::mutex output_mutex;

  
  class Task
  {
  public:
    int nr, size;
    const std::function<void(size_t nr, size_t size)> * pfunc;
    std::atomic<int> * cnt;

    Task & operator++(int)
    {
      nr++;
      return *this;
    }
    Task & operator*() { return *this; }
  };

  
  typedef moodycamel::ConcurrentQueue<Task> TQueue; 
  typedef moodycamel::ProducerToken TPToken; 
  typedef moodycamel::ConsumerToken TCToken; 
  
  
  bool stop = false;
  std::atomic<int> running{0};
  TQueue queue;
  
  
  void StartWorkers(size_t num)
  {
    // if (!timeline)
    // timeline = std::make_unique<TimeLine>();
        
    stop = false;
    for (int i = 0; i < num; i++)
      {
        TimeLine * patl = timeline.get();
        std::thread([patl]()
        {
          running++;
          if (patl)
            timeline = std::make_unique<TimeLine>();
          
          TPToken ptoken(queue); 
          TCToken ctoken(queue); 
                      
          while(true)
            {
              if (stop) break;

              Task task;
              if(!queue.try_dequeue_from_producer(ptoken, task)) 
                if(!queue.try_dequeue(ctoken, task))  
                  continue; 
              
              (*task.pfunc)(task.nr, task.size);
              (*task.cnt)++;
            }

          if (timeline)
            {
              TimeLine tmp = std::move(*timeline);
              if (patl)
                patl -> AddTimeLine(std::move(tmp));
            }
          running--;
        }).detach();
      }
  }

  void StopWorkers()
  {
    stop = true;
    while (running);
  }

  
  void RunParallel (size_t num,
                    const std::function<void(size_t nr, size_t size)> & func)
  {
    TPToken ptoken(queue);
    TCToken ctoken(queue);
    
    std::atomic<int> cnt{0};

    /*
    for (size_t i = 0; i < num; i++)
      {
        Task task;
        task.nr = i;
        task.size=num;
        task.pfunc = &func;
        task.cnt = & cnt;
        queue.enqueue (ptoken, task);
      }
    */

    Task firsttask;
    firsttask.nr = 0;
    firsttask.size = num;
    firsttask.pfunc=&func;
    firsttask.cnt = &cnt;
    queue.enqueue_bulk (ptoken, firsttask, num);    

    
    while (cnt < num)
      {
        Task task;
        if(!queue.try_dequeue_from_producer(ptoken, task)) 
          if(!queue.try_dequeue(ctoken, task))
            continue; 
        
        (*task.pfunc)(task.nr, task.size);
        (*task.cnt)++;
      }
  }
  
}
