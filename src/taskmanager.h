#ifndef TASKMANAGER_H
#define TASKMANAGER_H



namespace ASC_HPC
{
  
  void StartWorkers(size_t num);
  void StopWorkers();
  
  void RunParallel (size_t num,
                    const std::function<void(size_t nr, size_t size)> & func);

  extern std::mutex output_mutex;
}



#endif
