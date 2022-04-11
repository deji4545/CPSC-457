// this is the ONLY file you should edit and submit to D2L

// Name - Ayodeji Osho
// Class - CPSC 457 T09
// Student ID -30071771

#include "deadlock_detector.h"
#include "common.h"
#include <iostream>
using namespace std;



/// this is the function you need to (re)implement
///
/// parameter edges[] contains a list of request- and assignment- edges
///   example of a request edge, process "p1" resource "r1"
///     "p1 -> r1"
///   example of an assignment edge, process "XYz" resource "XYz"
///     "XYz <- XYz"
///
/// You need to process edges[] one edge at a time, and run a deadlock
/// detection after each edge. As soon as you detect a deadlock, your function
/// needs to stop processing edges and return an instance of Result structure
/// with edge_index set to the index that caused the deadlock, and dl_procs set
/// to contain with names of processes that are in the deadlock.
///
/// To indicate no deadlock was detected after processing all edges, you must
/// return Result with edge_index = -1 and empty dl_procs[].
///


class FastGraph {
public:
    vector<std::vector<int>> adj_list;
    vector<int> out_counts;
    vector<string> vect_conversion;

} fastgraph;

//Create fast graph objects
FastGraph fastCycleGraph;
vector<std::string> dl_process;
vector<int> vect_process;


//Carry out topological sort
int topologicalSort()
{
    vector<int> out;
    vector<int> zero;
    dl_process.clear();

    out = fastCycleGraph.out_counts;

    //find all nodes in graph with outdegree == 0
    for (unsigned long int i = 0; i < out.size(); i++) {
        if (out.at(i) == 0) {
            zero.push_back(i);
        }
    }

    //find all nodes in graph with outdegree == 0 and 
    while (zero.empty() == false) {
        int n = zero.back();
        zero.pop_back();
        vector<int> vect_int = fastCycleGraph.adj_list.at(n);
        for (unsigned long int i = 0; i < vect_int.size(); i++) {
            int n2 = vect_int.at(i);

            out.at(n2)--;

            if (out.at(n2) == 0) {
                zero.push_back(n2);
            }
        }
    }


    //dl_procs[] = all nodes n that represent a process and out[n]>0
    for (unsigned long int i = 0; i < vect_process.size(); i++) {

        if (out.at(vect_process.at(i)) > 0) {
            dl_process.push_back(
                split(fastCycleGraph.vect_conversion.at(vect_process.at(i)))
                    .at(0));
        }
    }

    return 0;
}

//Creates edges then call toplogical sort 
Result detect_deadlock(const std::vector<std::string> & edges)
{

    for (long unsigned int i = 0; i < edges.size() * 2; i++) {
        fastCycleGraph.vect_conversion.push_back("");
    }
    Result result;
    Word2Int wordInt;
    int count = 0;
    for (auto j : edges) {
        count++;
        result.dl_procs = split(j);
        result.edge_index = -1;

        int process = wordInt.get(result.dl_procs.at(0) + " (P)");

        string arrow = result.dl_procs.at(1);
        int resource = wordInt.get(result.dl_procs.at(2) + " (R)");

        //Add process to graph if it does not exist 
        if (fastCycleGraph.vect_conversion.at(process) == "") {
            fastCycleGraph.vect_conversion.at(process)
                = result.dl_procs.at(0) + " (P)";
            vector<int> vects;
            fastCycleGraph.adj_list.push_back(vects);
            fastCycleGraph.out_counts.push_back(0);
            vect_process.push_back(process);
        }

        //Add resource to graph if it does not exist 
        if (fastCycleGraph.vect_conversion.at(resource) == "") {
            fastCycleGraph.vect_conversion.at(resource)
                = result.dl_procs.at(2) + " (R)";
            vector<int> vects1;
            fastCycleGraph.adj_list.push_back(vects1);
            fastCycleGraph.out_counts.push_back(0);
        }

        //Add nodesto adjacent list and increase outward edges count depending on arrow position
        if (arrow == "<-") {
            fastCycleGraph.adj_list.at(process).push_back(resource);

            fastCycleGraph.out_counts.at(resource)
                = fastCycleGraph.out_counts.at(resource) + 1;

        } else {
            fastCycleGraph.adj_list.at(resource).push_back(process);

            fastCycleGraph.out_counts.at(process)
                = fastCycleGraph.out_counts.at(process) + 1;
        }

        //call toplogical sort
        topologicalSort();

        if (dl_process.empty() == false) {
            result.dl_procs = dl_process;
            result.edge_index = count - 1;
            break;
        }
    }


    if (dl_process.empty() == true) {
        result.dl_procs.clear();
        result.edge_index = -1;
    }
    return result;
}
