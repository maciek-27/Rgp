
#include <libxftp/xftpclient.h++>
#include <string>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include "superdata.h++"
#include<cmath>
using namespace xftp;
using namespace streamcon;
using namespace std;

using namespace xftpclient;

namespace {
	unsigned long gettv ()
	{
		struct timeval tvstart;
		gettimeofday (&tvstart, 0);
		return 1000000*tvstart.tv_sec+tvstart.tv_usec;
	}
}

SuperData::SuperData (ClientBase& client, Stream& stream) : DataThread (client, stream)
{
	file_size=0;
}

void SuperData::ExecuteCommand	(Command c, string &p, unsigned int s)
{
	ScopedLock lock (LocalMTX);
	C=c;
	path.assign (p);
	file_size=s;
}

void SuperData::Run ()
{
	while (1)
	{
		bool cnt;
		{
			ScopedLock lock (LocalMTX);
			cnt = (file_size==0);
		}
		if (cnt)
		{
			
			//std::cout << "no download" <<endl;
			usleep (20000);
			continue;
		}
		int old_file_size=file_size;
		// update progressbar
#define progress \
				{\
					float progress = 100.0*(old_file_size-file_size)/old_file_size;\
					float time_left=0.000001*(100.0-progress)*(gettv()-t_start)/progress;\
					float speed=1000.0*(old_file_size-file_size)/(gettv()-t_start);\
					cout << "\r[ ";\
					fill_n (ostream_iterator<char>(cout), trunc(0.39*progress), '#');\
					fill_n (ostream_iterator<char>(cout), 40-trunc(0.39*progress), ' ');\
					cout << "] " <<  setiosflags(ios::fixed)\
						<< setprecision(1) << setw(4) <<progress<< "%, " \
						<< setprecision(2) << setw(9)  <<speed << "kB/s, "\
						<< setprecision(1) << setw(5)  <<time_left << "s. left"	<<flush;\
				}
		unsigned long t_start = gettv();
		
		if (C==Get)
		{
			std::cout << "Downloading " << file_size << "b file" <<endl;
			FILE * f = fopen (path.c_str (), "w");
			if (!f)
			{
				std::cout << "failed open " << path
				<< stringError (errno)
				<<std::endl;
				exit (5);
			}
			Stream & s = GetClient ()[4];
			unsigned int result;
			char buf[FIO_BUFSIZE];
			while ( (result=s.GetData (buf, min<int>(sizeof(buf), file_size) )) )
			{
				file_size-=result;
				fwrite (buf, 1, result, f);
				progress;
				if (!file_size)
					break;
			}
			fclose (f);
			std::cout << "\nDone downloading" <<endl;
		}
		else
		{
			std::cout << "Uploading " << file_size << "b file" <<endl;
			FILE * f = fopen (path.c_str (), "r");
			Stream & s = GetClient ()[4];
			unsigned int result;
			char buf[FIO_BUFSIZE];
			while ( (result=fread (buf, 1, min<int>(sizeof(buf), file_size) , f)) )
			{
				file_size-=result;
				;
				s.SendData (buf, result );
				progress;
				//std::cout << file_size  << "b left." <<endl;
				if (!file_size)
					break;
			}
			fclose (f);
			std::cout << "\nDone uploading" <<endl;
			
		}
	}
	//t.GetClient()[streamid].SendData(buf,result);;
}
