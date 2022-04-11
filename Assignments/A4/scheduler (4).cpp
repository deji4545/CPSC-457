// Name - Ayodeji Osho
// Class - CPSC 457 T09
// Student ID -30071771

#include "scheduler.h"
#include "common.h"
#include "iostream"
using namespace std;

// this is the function you should edit
//
// runs Round-Robin scheduling simulator
// input:
//   quantum = time slice
//   max_seq_len = maximum length of the reported executing sequence
//   processes[] = list of process with populated IDs, arrival_times, and bursts
// output:
//   seq[] - will contain the execution sequence but trimmed to max_seq_len size
//         - idle CPU will be denoted by -1
//         - other entries will be from processes[].id
//         - sequence will be compressed, i.e. no repeated consecutive numbers
//   processes[]
//         - adjust finish_time and start_time for each process
//         - do not adjust other fields
//

void pushJobs(
    std::vector<Process> & processes, int curr_time, std::vector<int> & jq, std::vector<int> & rq)
{

    //  push jobs into ready queue
    int count = 0;
    for (auto j : jq) {
        if (processes.at(j).arrival_time == curr_time) {
            rq.push_back(j);
            count++;
        }
    }
    for (int i = 0; i < count; i++) { jq.erase(jq.begin()); }
}

void simulate_rr(
    int64_t quantum, int64_t max_seq_len, std::vector<Process> & processes, std::vector<int> & seq)
{

    // Assigning Variables
    int64_t curr_time = 0;
    int64_t remaining_slice = quantum;
    int cpu = -1;
    std::vector<int> rq, jq;
    std::vector<int64_t> remaining_bursts;
    seq.clear();
    seq.push_back(-2);

    for (size_t i = 0; i < processes.size(); i++) { jq.push_back(i); }

    // Keeping track of all process remaining burst rate
    for (size_t i = 0; i < processes.size(); i++) {
        remaining_bursts.push_back(processes.at(i).burst);
    }

    //  push jobs into ready queue
    pushJobs(processes, curr_time, jq, rq);

    // Loop to simulate round robin
    while (true) {
        if (processes.size() == 0) { break; }

        cpu = -1;
        //  if cpu is idle and rq is empty then lets skip current time to arrival
        if (cpu == -1 && rq.empty() == true && jq.empty() == false) {
            seq.push_back(-1);
            curr_time = processes.at(jq.front()).arrival_time;
            rq.push_back(jq.front());
            jq.erase(jq.begin());
            pushJobs(processes, curr_time, jq, rq);
        }

        // If cpu is idle and RQ is not empty move process from RQ to CPU
        if (cpu == -1 && rq.empty() == false) {
            cpu = rq.front();
            rq.erase(rq.begin());
            // Assign the arrival time
            if (processes.at(cpu).burst == remaining_bursts.at(cpu)) {
                processes.at(cpu).start_time = curr_time;
            }
            pushJobs(processes, curr_time, jq, rq);
        }

        // break out of the loop if the job and ready queue are both empty
        if (jq.empty() == true && rq.empty() == true) {
            seq.push_back(cpu);
            processes.at(cpu).finish_time = curr_time + remaining_bursts.at(cpu);
            break;
        }

        // RQ is empty and CPU has a process, so  execute multiple time slices
        if (cpu != -1 && rq.empty() == true) {
            int next_arrival_time = processes.at(jq.front()).arrival_time;
            int N_value = (next_arrival_time - curr_time) / quantum;
            int increment = N_value * quantum;
            if (quantum >= next_arrival_time) { increment = 0; }
            curr_time = curr_time + increment;

            remaining_bursts.at(cpu) = remaining_bursts.at(cpu) - increment;
        }

        // decreases the remaining burst for a process
        if (cpu != -1) {

            int64_t seq_size = seq.size();

            if (seq.back() != cpu && seq_size <= max_seq_len + 1) { seq.push_back(cpu); }

            remaining_slice = quantum;

            if (remaining_bursts.at(cpu) > remaining_slice) {
                remaining_bursts.at(cpu) = remaining_bursts.at(cpu) - remaining_slice;
                curr_time = curr_time + remaining_slice;

            } else {
                curr_time = curr_time + remaining_bursts.at(cpu);
                remaining_bursts.at(cpu) = 0;
            }

            //  push jobs into ready queue
            int count = 0;
            for (auto j : jq) {
                if (processes.at(j).arrival_time < curr_time) {
                    rq.push_back(j);
                    count++;
                }
            }
            for (int i = 0; i < count; i++) { jq.erase(jq.begin()); }

            // Set a finished time for finished process
            if (remaining_bursts.at(cpu) <= 0) {
                processes.at(cpu).finish_time = curr_time;
            } else {
                rq.push_back(cpu);
            }
        }
    }
    seq.erase(seq.begin());
}
