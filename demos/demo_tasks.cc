#include <iostream>
#include <taskmanager.h>

using namespace ASC_HPC;
using namespace std;


int main()
{
  StartWorkers(3);

  RunParallel(10,  [] (size_t i, size_t nr)
  {
    cout << "I am task " << i << " out of " << nr << endl;
  });


  RunParallel(10,  [] (size_t i, size_t nr)
  {
    cout << "round 2, I am task " << i << " out of " << nr << endl;
  });

  
  StopWorkers();
}

