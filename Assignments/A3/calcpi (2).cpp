// ======================================================================
// You must modify this file and then submit it for grading to D2L.
// ======================================================================
//
// count_pi() calculates the number of pixels that fall into a circle
// using the algorithm explained here:
//
// https://en.wikipedia.org/wiki/Approximations_of_%CF%80
//
// count_pixels() takes 2 paramters:
//  r         =  the radius of the circle
//  n_threads =  the number of threads you should create
//
// Currently the function ignores the n_threads parameter. Your job is to
// parallelize the function so that it uses n_threads threads to do
// the computation.


//Name - Ayodeji Osho
//Class - CPSC 457 T09
//Student ID -30071771

#include "calcpi.h"
#include <iostream>
#include <thread>

using namespace std;

struct Task
{
  int start_x, end_x, r_value;
  uint64_t partial_count;
};

//Each Threads calls this function with their divided parts and determine partial_count
void calc_partial_count(Task &someTask)
{
  int start = someTask.start_x;
  int end = someTask.end_x;
  int r = someTask.r_value;

  double rsq = double(r) * r;
  uint64_t count = 0;
  for (double x = start; x <= end; x++)
    for (double y = 0; y <= r; y++)
      if (x * x + y * y <= rsq)
        count++;
  someTask.partial_count = count * 4 + 1;
}

//Count pixels to determine the value of pi using multi-threading
uint64_t count_pixels(int r, int n_threads)
{

  Task tasks[n_threads];
  thread threadPool[n_threads];
  uint64_t sum = 0;

  //Dividing the work for each thread and storing values in task struct
  int workload = r / n_threads;
  int i;

  //Reference - Similar to TA provided code
  for (i = 0; i < n_threads; i++)
  {
    tasks[i].start_x = (i * workload) + 1;
    tasks[i].end_x = (tasks[i].start_x + workload) - 1;
    tasks[i].partial_count = 0;
    tasks[i].r_value = r;

    if (tasks[i].end_x > r)
    {
      tasks[i].end_x = r;
    }
    else if (tasks[i].end_x < r && i == n_threads - 1)
    {
      tasks[i].end_x = r;
    }
  }

  //Creating Threads
  for (i = 0; i < n_threads; i++)
  {
    threadPool[i] = thread(calc_partial_count, ref(tasks[i]));
  }

  //Joining threads
  for (i = 0; i < n_threads; i++)
  {
    threadPool[i].join();
  }
  
  //Adding up results of each threads
  for (i = 0; i < n_threads; i++)
  {
    sum = sum + tasks[i].partial_count;
  };

  return sum;
}
