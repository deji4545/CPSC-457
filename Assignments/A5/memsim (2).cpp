// -------------------------------------------------------------------------------------
// this is the only file you need to edit
// -------------------------------------------------------------------------------------
//
// (c) 2021, Pavol Federl, pfederl@ucalgary.ca
// Do not distribute this file.

// Name - Ayodeji Osho
// Class - CPSC 457 T09
// Student ID -30071771

#include "memsim.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <list>
#include <set>
#include <unordered_map>

using namespace std;

struct Partition {
  int tag;
  int64_t size, addr;

  Partition(int tag1, int64_t size1, int64_t addr1)
  {
    tag = tag1;
    size = size1;
    addr = addr1;
  }
};

// An iterator object for the partision list
typedef std::list<Partition>::iterator PartitionRef;
// An iterator object for the free block set
typedef std::set<PartitionRef>::iterator Partition_Ref_Set;

struct scmp {
  bool operator()(const PartitionRef & c1, const PartitionRef & c2) const
  {
    if (c1->size == c2->size) return c1->addr < c2->addr;
    else
      return c1->size > c2->size;
  }
};


struct Simulator {
  // all partitions, in a linked list
  std::list<Partition> all_blocks;
  // sorted partitions by size/address
  std::set<PartitionRef, scmp> free_blocks;
  // quick access to all tagged partitions
  std::unordered_map<long, std::vector<PartitionRef>> tagged_blocks;
  int64_t the_page_size;
  int64_t n_pages_requested = 0;
  Simulator(int64_t page_size)
  {
    // constructor
    the_page_size = page_size;
  }

  //Merge of Free Blocks
  void merger(PartitionRef deallocated_partition)
  {

    PartitionRef prev_ref = prev(deallocated_partition);
    PartitionRef next_ref = next(deallocated_partition);

    free_blocks.erase(deallocated_partition);

    // Case 1- Merge with left free block
    if (prev_ref->tag < 0) {
      free_blocks.erase(prev_ref);
      deallocated_partition->size = deallocated_partition->size + prev_ref->size;
      deallocated_partition->addr = prev_ref->addr;
      all_blocks.erase(prev_ref);
    }

    // Case 2- Merge with right free block
    if (next_ref->tag < 0) {
      free_blocks.erase(next_ref);
      deallocated_partition->size = deallocated_partition->size + next_ref->size;
      all_blocks.erase(next_ref);
    }

    free_blocks.insert(deallocated_partition);
  }

  //Adds more memory that is the minimum multiple of page size 
  int addMoreMemory(int tag, int size)
  {
    int64_t size_requested = size;
    int64_t memory = 0;

    //Addition of memory when all_block size is empty
    if (all_blocks.size() == 0) {
      memory = (size_requested / the_page_size) * the_page_size;
      while (memory < size_requested) { memory = memory + the_page_size; }
      n_pages_requested = n_pages_requested + (memory / the_page_size);
      Partition hole(-1, memory, 0);
      all_blocks.push_back(hole);
      free_blocks.insert(all_blocks.begin());
      return 0;
    }

    //Addition of memory when all_block size is not empty 
    if (all_blocks.size() > 0) {
      int64_t memory = size;

      if (size_requested > the_page_size) {
        PartitionRef lastPart = all_blocks.end();
        lastPart--;
        int64_t request = size_requested - lastPart->size;
        memory = (request / the_page_size) * the_page_size;
        while (request > memory) { memory = memory + the_page_size; }
      }

      if (size_requested < the_page_size) { memory = the_page_size; }

      n_pages_requested = n_pages_requested + (memory / the_page_size);

      Partition hole(-1, memory, 0);
      all_blocks.push_back(hole);
      PartitionRef endList = all_blocks.end();
      endList--;

      endList->addr = prev(endList)->addr + prev(endList)->size;
      free_blocks.insert(endList);
      merger(endList);
    }

    return 0;
  }

  //Gives each request a segment of memory from the largest partition
  void allocate(int tag, int size)
  {
    Partition_Ref_Set set_iterator = free_blocks.begin();
    PartitionRef free_space = *set_iterator;


    if (all_blocks.size() == 0) {
      addMoreMemory(tag, size);
    } else if (free_blocks.size() == 0) {
      addMoreMemory(tag, size);
    // Check if size is bigger than the biggest free_blocks size
    // Add More Memory if that is the case
    } else if (size > free_space->size) {
      addMoreMemory(tag, size);
    }

    set_iterator = free_blocks.begin();
    free_space = *set_iterator;
    free_blocks.erase(free_space);

    // Creating a new allocate partion object to be inserted into the list
    int64_t address = free_space->addr;
    Partition parition_allocated(tag, size, address);
    all_blocks.insert(free_space, parition_allocated);

    // Add the allocated partition to the unordered map
    std::vector<PartitionRef> & vect_partition_ref = tagged_blocks[tag];
    vect_partition_ref.push_back(std::prev(free_space));

    // Update the address and size of the free space
    free_space->addr = std::prev(free_space)->addr + std::prev(free_space)->size;
    free_space->size = free_space->size - std::prev(free_space)->size;

    free_blocks.insert(free_space);
  }

  //Uses map to determine which blocks to deallocate
  int deallocate(int tag)
  {
    std::vector<PartitionRef> & vect_partition_ref = tagged_blocks[tag];

    if (vect_partition_ref.size() == 0) { return 0; }

    for (long unsigned int i = 0; i < vect_partition_ref.size(); i++) {
      vect_partition_ref.at(i)->tag = -1 * (vect_partition_ref.at(i)->tag);
      free_blocks.insert(vect_partition_ref.at(i));
      merger(vect_partition_ref.at(i));
    }

    vect_partition_ref.clear();

    return 0;
  }

  //Retrieve and store results for max free partition size, address and pages requested  
  MemSimResult getStats()
  {
    MemSimResult result;
    if(all_blocks.size() == 0 && free_blocks.size() == 0){
      result.max_free_partition_size = 0;
      result.max_free_partition_address = 0;
      result.n_pages_requested = 0;
      return result;
    }

    Partition_Ref_Set set_ref = free_blocks.begin();
    PartitionRef free_space = *set_ref;

    result.max_free_partition_size = free_space->size;
    result.max_free_partition_address = free_space->addr;
    result.n_pages_requested = n_pages_requested;
    return result;
  }
 
};


MemSimResult mem_sim(int64_t page_size, const std::vector<Request> & requests)
{

  Simulator sim(page_size);
  //Allocate and deallocate memory based on request
  for (const auto & req : requests) {
    if (req.tag < 0) {

      sim.deallocate(-req.tag);
    } else {
    
      sim.allocate(req.tag, req.size);
    }
 
  }
  return sim.getStats();
}
