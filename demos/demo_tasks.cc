#include <iostream>
#include <taskmanager.h>

using namespace ASC_HPC;
using namespace std;

int main()
{
  StartWorkers(4);

  RunParallel(4,  [] (size_t i, size_t nr)
  {
    cout << "I am task " << i << " out of " << nr << endl;
  });
  
  StopWorkers();
}

