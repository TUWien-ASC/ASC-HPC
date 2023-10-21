#include <chrono>
#include <thread>

#include <concurrentqueue.h>


namespace ASC_HPC
{
  struct Task
  {
    int nr, size;
    const std::function<void(size_t nr, size_t size)> * pfunc;
    std::atomic<int> * cnt;
  };

  
  typedef moodycamel::ConcurrentQueue<Task> TQueue; 
  typedef moodycamel::ProducerToken TPToken; 
  typedef moodycamel::ConsumerToken TCToken; 
  
  
  bool stop = false;
  TQueue queue;
  
  
  void StartWorkers(size_t num)
  {
    stop = false;
    for (int i = 0; i < num; i++)
      {
        std::thread([]()
                    {
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
                    }).detach();
      }
  }

  void StopWorkers()
  {
    stop = true;
  }

  
  void RunParallel (size_t num,
                    const std::function<void(size_t nr, size_t size)> & func)
  {
    TPToken ptoken(queue);
    TCToken ctoken(queue);
    
    std::atomic<int> cnt{0};
    for (size_t i = 0; i < num; i++)
      {
        Task task;
        task.nr = i;
        task.size=num;
        task.pfunc = &func;
        task.cnt = & cnt;
        queue.enqueue (ptoken, task);
      }
    
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
