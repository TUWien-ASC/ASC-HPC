
namespace ASC_HPC
{
  void StartWorkers(size_t num)
  {
    ;
  }

  void StopWorkers()
  {
    ;
  }
  
  void RunParallel (size_t num,
                    std::function<void(size_t nr, size_t size)> func)
  {
    for (size_t i = 0; i < num; i++)
      func(i, num);
  }
  
}
