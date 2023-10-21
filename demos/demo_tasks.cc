#include <iostream>
#include <sstream>
#include <taskmanager.h>

using namespace ASC_HPC;
using namespace std;


int main()
{
  StartWorkers(3);

  RunParallel(10,  [] (size_t i, size_t size)
  {
    cout << "I am task " << i << " out of " << size << endl;
  });


  RunParallel(6, [] (size_t i, size_t s)
  {
    RunParallel(6, [i](size_t j, size_t s2)
    {
        stringstream str;
        str << "nested, i,j = " << i << "," << j << "\n";
        cout << str.str();
    });
  });

  
  StopWorkers();
}

