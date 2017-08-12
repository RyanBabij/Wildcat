#pragma once
#ifndef OUTPUT_THREADEDCOUT_HPP
#define OUTPUT_THREADEDCOUT_HPP

#include <Thread/Thread.hpp>

#include <Container/Vector/Vector.hpp>

#include <Sleep/Sleep.hpp>

class ThreadedCout
{
	public:
	ThreadLock vectorLock;
	int timesRun;
	Vector <std::string> vMessage;
	
	ThreadedCout()
	{
		timesRun=0;
	}
	
	void addMessage(std::string message)
	{
		vectorLock.lock();
		//if(vMessage.size()<2)
		if(vMessage.size()==0)
		{
			
			vMessage.push(message);
			//std::cout<<"Successful push.\n";
		}
		//else
		//{
			//std::cout<<"Message lost.\n";
		//}
		vectorLock.unlock();
	}
	
	void * printMessage()
	{
		++timesRun;
		//std::cout<<"Printing.\n";
		//while(true)
		//{
		bool sleepate=false;
			vectorLock.lock();
			if(vMessage.size()>0)
			{
				//vectorLock.lock();
				//std::cout<<timesRun<<"."<<vMessage(0);
				std::cout<<vMessage(0);
				vMessage.eraseSlot(0);
				//sleep(20);
				
			}
			else
			{
				//std::cout<<"No messages to print.\n";
				
				sleepate=true;
			}
			vectorLock.unlock();
			/* Some consoles seem to not print anything till I flush it. */
			if(timesRun>100) { std::cout.flush(); timesRun=0; }
			if( sleepate==true) { std::cout.flush(); sleep(100); }
		//}
		return 0;
	}
	
	
};

ThreadedCout threadedCout;

void out (std::string _str)
{
	threadedCout.addMessage(_str+"\n");
}

void * coutThread(void*)
{
	while (true)
	{
		threadedCout.printMessage();
	}
	return 0;
}

void startCoutThread()
{
	std::cout<<"Threaded cout thread is running.\n";
	ThreadManager _tg;
	_tg.add(&coutThread, 0);
	//_tg.finishAll();
}



#endif
