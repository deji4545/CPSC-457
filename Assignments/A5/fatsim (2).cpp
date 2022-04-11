// -------------------------------------------------------------------------------------
// this is the only file you need to edit
// -------------------------------------------------------------------------------------
//
// (c) 2021, Pavol Federl, pfederl@ucalgary.ca
// Do not distribute this file.

// Name - Ayodeji Osho
// Class - CPSC 457 T09
// Student ID -30071771

#include "fatsim.h"
#include <bits/stdc++.h>
#include <cstdio>
#include <iostream>
#include <list>
#include <stack>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;
int arr_size;
unordered_map<int, vector<long>> adjMap;

//Uses iterative DFS to find the longest chain from null pointer node
long depthFirstSearch(long node)
{

  long distance = 0;
  distance++;
  long max_distance = 0;
  stack<pair<long, long>> theStack;
  //Node are pairs, first pair for node number, second for distance from null pointer
  pair<long, long> nodePair;
  //The stack keep track of unvisited node
  theStack.push({ node, distance });

  //Loop to add more unvisited node to the stack
  while (true) {
    nodePair = theStack.top();
    theStack.pop();
    node = nodePair.first;
    distance = nodePair.second;

    if (max_distance < distance) { max_distance = distance; }

    if (theStack.empty() == true && adjMap[node].size() == 0) { break; }

    for (auto j : adjMap[node]) {
      pair<int, int> thePair;
      thePair.first = j;
      thePair.second = distance + 1;
      theStack.push(thePair);
    }
  }
  return max_distance;
}


std::vector<long> fat_check(const std::vector<long> & fat)
{

  arr_size = fat.size();
  vector<long> null_adj;

  // Graph is represented using an unordered map
  for (long unsigned int i = 0; i < fat.size(); i++) {

    long fat_element = fat.at(i);
    adjMap[fat_element].push_back(i);

    if (fat_element == -1) { null_adj.push_back(i); }
  }

  vector<long> result_vect;

  //Run DFS for each node beside the null pointer node
  for (auto j : null_adj) { result_vect.push_back(depthFirstSearch(j)); }
  sort(result_vect.begin(), result_vect.end());

  return result_vect;
}
