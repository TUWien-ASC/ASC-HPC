#include <iostream>
#include <sstream>
#include <taskmanager.h>
#include <timer.h>

using namespace ASC_HPC;
using namespace std;


int main()
{
  timeline = std::make_unique<TimeLine>("demo.trace");

  StartWorkers(3);

  RunParallel(100,  [] (size_t i, size_t size)
  {
    static Timer t("timer one");
    RegionTimer reg(t);
    cout << "I am task " << i << " out of " << size << endl;
  });

  RunParallel(100,  [] (size_t i, size_t size)
  {
    static Timer t("timer two", { 0, 0, 1});
    RegionTimer reg(t);
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

