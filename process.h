#include <string>
#include <iostream>
#include <vector>

using namespace std;

#ifndef PROCESS
#define PROCESS

class process
{
    private:
        
    string processName;                 //cpu name                      -- set through constructor
    int p_id;                           //process id                    -- set through constructor
    int cpuTimer;                       //counts clock ticks for the process (cpu)
    int ioTimer;                        //counts clock ticks for process (io)
    int cpuTotal;                       //number of clock ticks that process is active (accumulator)
    int ioTotal;                        //number of clock ticks that process is io (accumulator)
    int cpuCount;                       //counts the CPU bursts (accumulator)
    int ioCount;                        //counts the I/O burts (accumulator)
    int start;                          //what time the process started
    
    public:
        
    vector <int> cycleType;             //cycle type - cpu burst(positive num) or io burst (negative num)
    int sub;                            //subscript into the array cycle type

    
    //methods
    
    //constructor
    process(string, int);               //constructor used in all processes
    
    //setter methods
    void set_cpuTimer(int);             //sets the cpu cpuTimer
    void set_ioTimer(int);              //sets the ioTimer
    void set_start(int);                //set start
    
    //getter methods
    string getProcessName();            //returns process name
    int getP_id();                      //returns the process ID
    int getCpuTimer();                  //return cpu cpuTimer
    int getIoTimer();                   //returns the ioTimer
    int getCpuTotal();                  //returns the cpu cpuTotal
    int getIoTotal();                   //returns the ioTotal
    int getCpuCount();                  //returns cpuCount
    int getIoCount();                   //returns ioCount
    int getStart();                     //returns start
    
    //incrementer methods
    void inc_cpuTimer();                //increments the cpu cpuTimer
    void inc_ioTimer();                 //increments the ioTimer
    void inc_cpuTotal();                //increments cpuTotal
    void inc_ioTotal();                 //increment ioTotal
    void inc_cpuCount();                //increment cpuCount
    void inc_ioCount();                 //increment ioCount
    void inc_sub();                     //increment the subscript
    
    
};

#endif