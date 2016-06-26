#include "process.h"


//constructor
process::process(string name, int id)
{
    //constructor that takes in the process
    //name and id and initializes the values
    //this is the constructor that is used in
    //the program -- default constructor is not
    //used
    processName = name;
    p_id = id;
    cpuTimer = 0;
    ioTimer = 0;
    cpuTotal = 0;
    ioTotal = 0;
    cpuCount = 0;
    ioCount = 0;
    sub = 0;
    start = -999;
}


//setters
void process:: set_cpuTimer(int c_t)
{
    //set the cpuTimer
    
    cpuTimer = c_t;
}

void process::set_ioTimer(int io_t)
{
    //set the ioTimer
    
    ioTimer = io_t;
}

void process::set_start(int st)
{
    //set the start time
    
    start = st; 
}


//getters
string process::getProcessName()
{
    //get the process name
    
    return processName;
}

int process::getP_id()
{
    //get the process id
    
    return p_id;
}

int process::getCpuTimer()
{
    //get the cpu timer
    
    return cpuTimer;
}

int process::getIoTimer()
{
    //get the io timer
    
    return ioTimer;
}

int process::getCpuTotal()
{
    //get the total cpu time
    
    return cpuTotal;
}

int process::getIoTotal()
{
    //get the total io time
    
    return ioTotal;
}

int process::getCpuCount()
{
    //get the number of cpu bursts
    
    return cpuCount;
}

int process::getIoCount()
{
    //get the number of io bursts
    
    return ioCount;
}
int process::getStart()
{
    //get the start time
    
    return start;
}


//incrementers
void process::inc_cpuTimer()
{
    //increment the cpu timer
    
    cpuTimer = cpuTimer + 1;
}

void process::inc_ioTimer()
{
    //increment the io counter
    
    ioTimer = ioTimer + 1;
}

void process::inc_cpuTotal()
{
    //increment the cpu total
    
    cpuTotal = cpuTotal + 1;
}


void process::inc_ioTotal()
{
    //increment the io total
    
    ioTotal = ioTotal + 1;
}

void process::inc_cpuCount()
{
    //increment cpu count
    
    cpuCount = cpuCount + 1;
}

void process::inc_ioCount()
{
    //increment the io count
    
    ioCount = ioCount + 1;
}

void process::inc_sub()
{
    //increment the sub script
    
    sub = sub + 1;
}

                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                        