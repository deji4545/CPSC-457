// =============================================================================
// You must modify this file and then submit it for grading to D2L.
// =============================================================================


//Name - Ayodeji Osho
//Class - CPSC 457 T09
//Student ID -30071771

#include "detectPrimes.h"
#include <iostream>
#include <cmath>
#include <cstdio>
#include <stdio.h>
#include <cstdlib>
#include <atomic>
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>

using namespace std;

// REFERENCE - TA Code from Mohammadreza Osouli
// Barrier
class simple_barrier
{
  mutex m_;
  condition_variable cv_;
  int n_remaining_, count_;
  bool coin_;

public:
  simple_barrier(int count)
  {
    count_ = count;
    n_remaining_ = count_;
    coin_ = false;
  }
  bool wait()
  {
    unique_lock<mutex> lk(m_);
    if (n_remaining_ == 1)
    {
      coin_ = !coin_;
      n_remaining_ = count_;
      cv_.notify_all();
      return true;
    }
    auto old_coin = coin_;
    n_remaining_--;
    cv_.wait(lk, [&]()
             { return old_coin != coin_; });
    return false;
  }
};

// Declaring atomic variables
atomic<int64_t> max_val;
atomic<bool> prime;
std::vector<int64_t> result;
atomic<int64_t> totalDivisor;
atomic<int64_t> splitDivisor;
atomic<bool> cancel;

// Thread Function - Called By Threads
static void thread_function(simple_barrier &barrier,
                            vector<int64_t> vect_nums, int64_t num_size, int64_t t_num, int64_t n_threads)
{
  // Outer Loop
  for (int64_t it = 0; it < num_size; it++)
  {
    // Serial Work
    if (barrier.wait())
    {
      cancel = true;
      prime = true;
      int64_t n = vect_nums.at(it);

      if (n < 2)
      {
        prime = false;
      }
      else if (n <= 3)
      {
        prime = true;
      }
      else if (n % 2 == 0 || n % 3 == 0)
      {
        prime = false;
      }
      else
      {
        cancel = false;
        max_val = sqrt(vect_nums.at(it)) + 1;
        totalDivisor = ((max_val) / 6);
        splitDivisor = (totalDivisor / n_threads) + 1;
      }
    }
    barrier.wait();

    // Parallel Work
    // Splits the divisor up for the threads to work on from
    if (cancel == false)
    {
      int64_t start = (t_num * splitDivisor * 6) - 1 + 6;
      int64_t end = ((t_num + 1) * splitDivisor * 6) - 1;

      if (t_num == n_threads - 1)
      {
        end = (totalDivisor * 6) - 1;
      }

      if (end > max_val)
      {
        end = (totalDivisor * 6) - 1;
      }

      //Inner Loop
      for (int64_t k = start; k <= end; k += 6)
      {
        if (cancel == true)
        {
          break;
        }
        if (vect_nums.at(it) % k == 0 || (vect_nums.at(it) % (k + 2) == 0))
        {
          prime = false;
          cancel = true;
        }
      }
    }

    // Serial Work
    // Pushes the value into results if its a prime
    if (barrier.wait())
    {

      if (prime == true)
      {
        result.push_back(vect_nums.at(it));
      }
    }

    barrier.wait();
  }
}

std::vector<int64_t>
detect_primes(const std::vector<int64_t> &nums, int n_threads)
{

  thread threadPool[n_threads];
  simple_barrier barrier(n_threads);

  // Create threads that calls thread_function
  for (int k = 0; k < n_threads; k++)
  {
    threadPool[k] = thread(thread_function, ref(barrier), ref(nums), nums.size(), k, n_threads);
  }

  // Joint threads together
  for (int k = 0; k < n_threads; k++)
  {
    threadPool[k].join();
  }

  //print out the results
  return result;
}
