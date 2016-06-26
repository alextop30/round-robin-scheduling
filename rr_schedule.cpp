#include "process.h"
#include <iomanip>
#include <cstdlib>
#include <queue>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>

//use standard library
using namespace std;

/*
 * 
 * Description: This program simulates round robin scheduling using preemption and time
 * slice - of time_slice -- set to 5. It uses standard queues to achieve the priority
 * the program is set to accpet IN_Use number of processes combined in the queues and
 * the active and io pointers to proccesses. The program prints the state of the 
 * schduler every HOW_Often times and it runs for maximum time MAX_TIME
 * 
 * No command line arguments are requred. The input file is opened from the string file
 */


//error class
class noFile
{} noFileError;


//global constants
const int MAX_TIME = 500;
const int TIME_SLICE = 5;
const int IN_USE = 5;
const int HOW_OFTEN = 25;
const int QUEUE_SIZE = 20;

//file name
const string file = "input.txt";

//start of the process ids
int PROCESS_ID = 100;

//start of the time
int Global_time = 0;

//total time the system spent in idle
int totalIdleTime = 0;

//total processes executed by the system;
int totalProcesses = 0;

//currently active processes
process * active = NULL;

process * ioActive = NULL;

//queues for the program global vars
//containing pointer to objects
queue<process *> entry;

queue<process *> ready;

queue<process *> io;


//function prototypes

//read the input file
process * readFile(ifstream*);

//create entry queue
void createEntry(ifstream*);

//create initial ready queue
void create_initial_ready();

//run the execution of the processes
void execute();

//print function that prints stats
void print();

//terminate an active or
//io active process
void terminateProcess();

//performs cpu bursts
void doCPU(bool&);

//performs io bursts
void doIo();

//switches the active process
void switch_active();

//switches the io active process
void switch_io();

//prints the final report
//after the program has eneded
void finalReport();

int main(void)
{
    //welcome message
    cout << "Round Robin process scheduling program!" << endl << endl;
    
    //input file stream
    ifstream * inputfile = new ifstream; 
    
    //handle an exception 
    //if the file does not exist
    try
    {
        //open the file 
        inputfile->open(file);
        
        if(inputfile->fail())
        {
            //throw the error back to main
            throw noFileError;
        }
        else
        {
            //file is open successfully message
            cout << endl;
            cout << "****************************************" << endl;
            cout << file << " has successfully been opened!" << endl;
            cout << "****************************************" << endl << endl << endl;
        }
    }
    catch (noFile noFileError)
    {
        //show the error to the console
        cerr << "The specified file: " <<file <<" is not accepssible!" << endl;
        
        //terminate the program
        exit(EXIT_FAILURE);
    }
    
    //create the entry queue
    createEntry(inputfile);
    
    //run the program
    execute();
    
    //give the final report
    finalReport();
    
    //successfully terminate the program
    return 0;
}

/*Function read file is responsible for reading the inputfile
 * and taking in each process from the input file stopping at
 * proccess STOPHERE as this is the stopping location.
 * 
 * The function takes in an a pointer to a standard input file
 * stream which has been opened successfully by Main. The function
 * will return a pointer to a new proccess.
 */
process * readFile(ifstream * inputfile)
{
    //declare the new process pointer -- initialize to null
    process * newprocess = NULL;
    
    //dynamically allocated string process name
    string * p_name = new string;
    
    //temporary integer
    int * temp = new int;
    
    //dynamically allocate temporary string to
    //hold line of input from file
    string* tempLine = new string;
     
     
    //increment the proccess id
    PROCESS_ID += 1;
    
    //obtain a line of input
    getline(*inputfile,*tempLine);
    
    istringstream * parse = new istringstream(*tempLine);
    
    //store the process name into p_name
    (*parse) >> *p_name;
    
    //dynamically create the new process
    newprocess = new process (*p_name, PROCESS_ID);
     
    //push the inforamtion into the vector
    while(!(((*parse) >> *temp).eof()))
    {
        newprocess ->cycleType.push_back(*temp);
    }
    
    //deallocate the dynamic variables used
    delete p_name;
    delete temp;
    delete tempLine;
    delete parse;
    
    
    //return a pointer to the new object
    return newprocess;
        
}

/* Function create entry is responsible for creating the entry queue
 * The function requires an input file stream be passed in so that
 * it could be passed on into the read file function. Read file is
 * called so that it can provide the new processes. This is repeated
 * for as many processes are contained in the input file. They
 * are brought into memory and put into a new queue -- entry queue
 * 
 * Requred a pointer to an input file stream opened in Main.
 * The function does not return nor output anything to the console
 */
void createEntry(ifstream* inputfile)
{
    
    // create entry que
    process * myprocess = readFile(inputfile);
    
    //set the start time for the entry ques
    myprocess->set_start(0);
    
    //make sure that the very last record is not included in the
    //program - the stop here record
    while ((myprocess->getProcessName().compare("STOPHERE") != 0))
    {
        //push the processes innto the entry queue
        entry.push(myprocess);
        myprocess = readFile(inputfile);
        myprocess->set_start(0);
        
    }
    
}

/* Function create initial ready queue takes
 * the entry queue and pops off number of IN_USE
 * elements (pointers to processes) and pushes them
 * into a standard entry queue. This is to get the scheduler
 * into a ready state so that when the system starts proccessing
 * is ready at time 0.
 * 
 * The function does not return anything nor does it print
 * anything to the console.
 */
void create_initial_ready()
{
    //pointer to hold the popped elements from the
    //entry queue and put them into the ready que
    process * curprocess = NULL;
    
    while (ready.size() < IN_USE)
    {
        //set the top element to 
        curprocess = entry.front();
        entry.pop();
        
        //push on to the ready queue
        ready.push(curprocess);
    }
    
}

/* Function Execute starts the scheduler and runs for MAX_TIME or
 * if all the processes have been executed. The function uses
 * a while loop which calls functions doCPU and doIo which takes
 * care of the cpu and io bursts of the processes respectivelly.
 * The function is also reposible for keeping a total of 5 
 * processes into the two queues and the two active pointers.
 * Periodically print function runs and it prints the state
 * of the scheduler. The perioud is HOW_OFTEN.
 * 
 * Function itself does not take any arguments and it does
 * not print anything to the console.
 */
void execute()
{  
    //pointer that moves processes from the entry queue
    //to the ready queue
    process * toready = NULL;
        
    //check if we are at the beginninng of the program
    if(ready.size() == 0 && io.size() == 0 
        && active == NULL && ioActive == NULL)
    {
        //at the begin of the program set up the ready 
        create_initial_ready();
    }
    
    //flag controlling the entry of process into
    //the ready queue
    bool flag = true;
    
    //idle flag for cpu print idle messge
    bool idleFlag = true;
   
    while(true)
    {
      //how often to print
      if((Global_time % HOW_OFTEN) == 0)
	print();
        
        //2 exit conditions
        //entry.size() == 0 &&
        if(active == NULL && ioActive == NULL && ready.size() ==0 && io.size() == 0)
        {
            goto endwhile;
        }
        else if(Global_time == MAX_TIME)
        {
            goto endwhile;
        }
            
       
        //maintain queues, active and io active at a total of 4 processes
        while(flag && Global_time != 0)
        {
	    //if active and io active are not null - make sure that the ques are topped off at 3
            if(active != NULL && ioActive != NULL && ((ready.size()+ io.size()) < IN_USE-2))
            {
             
                    toready = entry.front();
            
                    entry.pop();
            
                    ready.push(toready);
                    
                    toready->set_start(Global_time);
		    
		    cout << endl << endl;
                    
		    cout << "Process: " << toready->getProcessName() << endl;
		    cout << "ID: " << toready->getP_id() << endl;
		    cout << "Has entered the Ready Queue" << endl;
		    cout << "Time: " << Global_time << endl;
		    
		    toready = NULL;
		    
                    flag = false;
                    
            }
            else if(active == NULL && ioActive != NULL && ((ready.size()+ io.size()) < IN_USE-1))
            {
		    //if active is null but io active is not make sure the queues are topped off
		    //at 4
                    toready = entry.front();
            
                    entry.pop();
            
                    ready.push(toready);
                    
                    toready->set_start(Global_time);
		    
		    cout << "Process: " << toready->getProcessName() << endl;
		    cout << "ID: " << toready->getP_id() << endl;
		    cout << "Has entered the Ready Queue" << endl;
		    cout << "Time: " << Global_time <<endl << endl << endl;
		    
		    toready = NULL;
                    
                    flag = false;
            }
            else if(active != NULL && ioActive == NULL && ((ready.size()+ io.size()) < IN_USE-1))
            {       
                    // if active is processing and io is null 
                    // keep processes to 4 in the queues
                    toready = entry.front();
            
                    entry.pop();
            
                    ready.push(toready);
                    
                    toready->set_start(Global_time);
                    
		    cout << "Process: " << toready->getProcessName() << endl;
		    cout << "ID: " << toready->getP_id() << endl;
		    cout << "Has entered the Ready Queue" << endl; 
		    cout << "Time: " << Global_time <<endl << endl << endl;
		    
		    toready = NULL;
		    
                    flag = false;
            }
           
           //get out of loop flag
            flag = false;

        }
        
        //set the flag to true if entry size is not 0 so
        //that the loop would be activated
        if(entry.size() != 0)
            flag = true;
       
	//call to do cpu function -- passing the idle flag
	//to signal when idle message is to be printed
        doCPU(idleFlag); 
       
	//do io after the cpu
        doIo();
                         
        //increment the time
        ++Global_time;
        
        
    }
    //stop the while loop and exit the loop
    endwhile: return;    
    
}

/* Function print is responsible for printing the state of the 
 * scheduler. Active pointers values are printed at all times
 * even if they are 0. In addition to this the active and io
 * queues are also printed and each element pushed back in 
 * resulting in the same queue.
 * 
 * Function does not take arguments but it prints everything
 * to the console. HOW_OFTEN tells how many times the function
 * will be executed.
 */
void print()
{
    //temporary pointer used to display the contents of the queues
    process * current = NULL;
    cout << "Top of the main loop at time " << Global_time << endl;
    
    //if active is null print so
    if(active == NULL)
    {
        cout << "Active is: " << " 0" << endl;
        
    }
    if(ioActive == NULL)
    {
        //if io active is null print so
        cout << "I/O Active is: " << " 0" << endl;
    }
    if(active != NULL)
    {
        //if the active is not null print the process id
        cout << "Active is: " << active->getP_id() << endl; 
    }
    if(ioActive != NULL)
    {
        //if io active is not null print the proccess id
        cout << "I/O Active is: " << ioActive->getP_id() << endl;
    }
    
    //if the ready queue is empty print so
    cout << "Contents of the Ready Queue: " << endl;
    if(ready.empty())
      cout<< "(Empty)" << endl;
    
    //if the ready queue is not empty print the contents
    for(unsigned int i = 0; i < ready.size(); i++)
    {
        current = ready.front();
        ready.pop();
        cout << "Q["<< i << "] = " << current->getP_id() << endl;
        
        //add the contents back to the queue in the same order
        ready.push(current);
    }
    
    //reset the pointer back to nothing
    current = NULL;
    
    //contents of the I/O Queue 
    cout << "Contents of the I/O Queue: " << endl;
    
    //if the IO queue is empty print so
    if(io.empty())
      cout << "(Empty)" << endl;
    
    //if the io queue is not empty print it
    for(unsigned int i = 0; i < io.size(); i++)
    {
        current = io.front();
        io.pop();
        cout << "Q["<< i << "] = " << current->getP_id() << endl;
        
        //put the contents back in at the same order.
        io.push(current);
    }
    
    //add two blank lines
    cout << endl << endl;
}    

/* Function Terminate Process is executed when a process has
 * executed its final cycle and all of the statistics for that
 * process are printed to the console. The function is also
 * responsible for counting the number of processes succeessfully
 * executed.
 * 
 * Function does not take in any arguments but does print to the
 * console.
 */
void terminateProcess()
{
  //print out all required statistics from the process
  //that just terminated
    cout << "Process has ended:" << endl;
    cout << "Process: " << active->getP_id() << endl;
    cout << "Name: " << active->getProcessName() << endl;
    cout << "Started at time " << active->getStart()
         << " and ended at time " << Global_time << endl;
    cout << "Total CPU time = " << active->getCpuTotal()
         << " in " << active->getCpuCount() 
         << " CPU bursts" << endl;
    cout << "Total I/O time = " << active->getIoTotal()
         << " in " << active->getIoCount() 
         << " I/O bursts" << endl;
    cout << "Time spent in waiting: " 
         << (Global_time - active->getStart()) - (active->getIoTotal() + active->getCpuTotal())
         << endl << endl << endl;
	 
	 //total processes terminated
	 totalProcesses++;
    
}

/* Function doCPU is responsible for performing the actions for each
 * cpu burst. This involves incrementing timers and deciding when it is
 * time to change the active process and where it should go. If end of
 * time slice has occurd the proccess goes back into the ready queue
 * while if it is end of cpu burst the process is placed into the 
 * io queue waiting for io. 
 * 
 * The function takes in an idle flag -- which signals the function
 * when it is time to print the idle information to the screen. This
 * happens only once per idle period. The function does print idle
 * information to the console.
 */
void doCPU(bool& idleFlag)
{
    //get an active process
    //if one cannot be found do idle time
    switch_active();
          
    if(active == NULL && ready.size() == 0 && idleFlag)
    {
        cout << "At time " << Global_time << ", Active is 0"
            << ", so we have idle time for a while" << endl;
	    
	cout << endl << endl;
	
	idleFlag = false;
	
	totalIdleTime ++;
	
        return;
    }
    else if (active == NULL && ready.size() == 0)
    {
      totalIdleTime ++;
      
      return;
    }
     
     //set idle back to true
     idleFlag = true;
     
     //normal procedure
     active->inc_cpuTimer();
        
     //increment the total cpu bursts
     active->inc_cpuTotal();
     
    //is it time to change because I ran out of time?
    //are we at the end of the time slice
    if((active->getCpuTotal() !=0) && ((active->getCpuTotal()%TIME_SLICE) == 0))
    {   
        //if end of cpu burst --- put into io queue
        if(active->cycleType[active->sub] == active->getCpuTimer())
        {
            //increment the subscript 
            active->inc_sub();
            
             //increment the total cpu bursts
            active->inc_cpuCount();
            
            //if the next value of subscript is 0 terminate
            if(active->cycleType[active->sub] == 0)
            {
                terminateProcess();
                
                active = NULL;
		return;
            }
            else
            {
                //put in io queue ready to do io
                io.push(active);
            
                //reset cpu timer
                active->set_cpuTimer(0);
            
                //reset active to nothing
                active = NULL;
                
		return;
            }
            
            
        }
        else
        {
            //if we are at the end of a slice
            //and not at the end of the cpu burst
            //put back in to ready queue
            ready.push(active);
            
            //active is now at the end.
            active = NULL;
	    
	    return;
           
        }
        
     
        
    }
     
    //if not at the end of the time slice
    if((active->getCpuTotal() == 0)|| (active->getCpuTotal()%TIME_SLICE) != 0)
    {
        //if end of cpu burst --- put into io queue
        if(active->cycleType[active->sub] == active->getCpuTimer())
        {
            
            //increment the subscript 
            active->inc_sub();
            
            //incrment total cpu count 
            active->inc_cpuCount();
            
            //check if it is time to terminate
            if(active->cycleType[active->sub] == 0)
            {
                terminateProcess();
                
                //set active NULL
                active = NULL;
                
		return;
               
            }
            else
            {

                //put in io queue ready to do io
                io.push(active);
            
                //reset cpu timer
                active->set_cpuTimer(0);
		
                //reset active to nothing
                active = NULL;
		
		return;
            }
            
            
         }

    }
        

}

/* Function doIo is responsible for performinng actions for each proccess
 * that is its io burst section. The fuction keeps io counters and items in
 * the io queue. Once a process becomes io active it completes the entire
 * burst at once and it is pushed into the entry queue. The function does
 * check if a proccess is in need of termination and will call terminate
 * if it does.
 * 
 * The fucntion does nnot take in any arguments and it does not write
 * anything to standard out.
 */ 
void doIo()
{
            
    //get new io process if there is none
    switch_io();
    
    //if we do not have io process wait
    if(ioActive == NULL)
        return;
        
    //normal operation
    //increment io timer
    ioActive->inc_ioTimer();
    
    //increment totat time io
    ioActive->inc_ioTotal();
    
    if((ioActive->cycleType[ioActive->sub]*(-1)) == ioActive->getIoTimer())
    {
	
	//inccrement the subscript to next burst or 
	//terminate the proccess
	ioActive->inc_sub();
	
	//increment the total ios done
	ioActive->inc_ioCount();
	
	//is it time to terminate
	if(ioActive->cycleType[ioActive->sub] == 0)
	{
	    terminateProcess();
	    
	    ioActive = NULL;
	    
	    return;
	    
	}
	else
	{
		
	    //move into the ready queue
	    ready.push(ioActive);
	
	    //reset the io getCpuCount
	    ioActive->set_ioTimer(0);
	
	    //set io active to nothing
	    ioActive = NULL;
	    return;
	}
            
            
    }
     
        
    
    

}

/* Switch active is respocible for finding an active process from the ready queue
 * the function does make sure that the subscipt of the new processes is valid
 * and that the process is not at the end and needs to be terminated. If it does
 * need to be terminated -- the terminate function is called from within. This procedure
 * is done until a suitable process is found and the active pointer is set to it. If one
 * is not found -- active is not changed from a value of NULL.
 * 
 * Function does not take in any arguments nor does it print to the screen.
 */
void switch_active()
{
    //get an active process
    //if one cannot be found do idle time
    if(active == NULL && ready.size() != 0)
    {
        active = ready.front();
        ready.pop();
        
        //if proccess is at termination stage
        while(active->cycleType[active->sub] == 0)
        {
            //terminate the process
            terminateProcess();
            
            active = NULL;
            
            if(ready.front() != 0)
            {
                active = ready.front();
                ready.pop();
            }

        }
        
    }
    
}

/* Switch I/O function is responsible for looking at the io queue for
 * a new io process. The function will examine the subscript and will
 * terminate a process which has expired if not previously terminated
 * by the doCPU function. This will be done in a loop until a suitable
 * process is found or if one is found ioative will remain at NULL.
 * 
 * The functiond not return nor does it take any arguments or print
 * to the console.
 */
void switch_io()
{
    //no active io no but processe waiting  
    if(ioActive == NULL && io.size() != 0)
    {
        ioActive = io.front();
        io.pop();
        
        while(ioActive->cycleType[ioActive->sub] == 0)
        {
            
            //terminate the process
            terminateProcess();
            
            ioActive = NULL;
            
            if(io.size() != 0)
            {
                //move on to next process
                ioActive = io.front();
                io.pop();
            }
            
        }
    }
}

/* Function Final Report - is called after exeute has finished and
 * control is given back to main. This function is responsible for
 * printing the main statistics collected during the run including
 * the final clock where the execute function ended. The functon
 * also checks each queue including the entry queue and if they 
 * contain processes the queues are printed or else empty message
 * is printed to the screen.
 * 
 * The function does ont take arguemnts but it doe print to the
 * console.
 */
void finalReport()
{
  //temporary pointer used to print the queues  
  process * current = NULL;
  
  cout << endl;
  
  //messagess
  cout << "The system has terminated its execution!" << endl;
  
  cout << "The system ended at final time: " << Global_time << endl;
  
  cout << "The total amount of time the system was idle: " << totalIdleTime << endl;
  
  cout << "The number of processes successfully executed: " << totalProcesses << endl;
  
  cout << "Contents of the Entry Queue" << endl;
  
  //checck if the entry queue is empty
  if(entry.empty())
  {
      cout << "(Empty)" << endl;
  }
  else
  {
      //if the queue is not empty it is print it
      for(unsigned int i = 0; i < entry.size(); i++)
      {
	  current = entry.front();
	  entry.pop();
	  cout << "Q["<< i << "] = " << current->getP_id() << endl;
	  
          //keep items in order back into it
	  entry.push(current);
      }
  }
  
  //reset the temp pointer to NULL to be used with the other queues
  current = NULL;
  
  //ready queue message
  cout << "Contents of the Ready Queue" << endl;
  
  //if the ready queue is empty print so
  if(ready.empty())
  {
      cout << "(Empty)" << endl;
  }
  else
  {
      //if the ready queue is not empty print it 
      for(unsigned int i = 0; i < entry.size(); i++)
      {
	  current = ready.front();
	  ready.pop();
	  cout << "Q["<< i << "] = " << current->getP_id() << endl;
      
	  ready.push(current);
      }
  }
  
  //reset the pointer back to null
  current = NULL;
  
  //message for I/O queue
  cout << "Contents of the I/O Queue" << endl;
  
  //is the io queue empty and if yes print message
  if(io.empty())
  {
      cout << "(Empty)" << endl;
  }
  else
  {
      //if it is not empty print the queue
      for(unsigned int i = 0; i < entry.size(); i++)
      {
	  current = io.front();
	  io.pop();
	  cout << "Q["<< i << "] = " << current->getP_id() << endl;
      
	  io.push(current);
      }
  }
  
  //reset the pointer to NULL
  current = NULL;
}
