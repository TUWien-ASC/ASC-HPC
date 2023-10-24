#ifndef TASKMANAGER_H
#define TASKMANAGER_H



namespace ASC_HPC
{
  
  void StartWorkers(int num);
  void StopWorkers();
  
  void RunParallel (int num,
                    const std::function<void(int nr, int size)> & func);
  
}



#endif
