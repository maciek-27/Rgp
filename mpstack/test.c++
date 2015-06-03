#include<rexio/tk/toolkit.h++>
#include<iostream>
#include <unistd.h>
#include<cstdlib>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<fstream>
#include<set>
#include<algorithm>
using namespace std;
using namespace Scr;
using namespace Scr::Tk;

Scr::Uint labelWidth = 60;


string TempFN;
ifstream TempF;

class SampleLabel: public Label
{
public:
  
  SampleLabel(const std::string& label)throw();
  void OnResize()throw();
};

class ProcessUI:public VerticalGroup
{
  
private:
  float prev_cumul;

  int PID;
  int TID;
  int numLabels;		
  vector<SampleLabel *>stack_n;
public:
  ProcessUI(int PID, int TID, const DisplayStyle &  _style);
  ~ProcessUI()throw();
  void PrintStack();
void OnFocus(FocusPolicy focustype)throw()
    {
      ArrangeContents();
      stack_n[0]->SetStyle(DisplayStyle(Fg::Yellow, Fg::Bright, Bg::Blue));
    }
  void OnUnFocus(FocusPolicy focustype)throw()
  {
   stack_n[0]->SetStyle(DisplayStyle(Fg::White, Fg::Bright, Bg::Blue));
   
  }
};

  class MultiGroup: public VerticalGroup, public HorizontalGroup
  {
  private:
    int rows;
    int columns;

    int defcolumns;
    HorizontalGroup HG; 
    vector <VerticalGroup*> VG;
    bool horizontal;
    int num_elem;
  public:
    MultiGroup(Uint _height,	
	       Uint _width,
	       const DisplayStyle &  _style)throw()
      :BoxGroup(_height, _width,_style),
       VerticalGroup(_height, _width,_style),
       HorizontalGroup(_height, _width,_style),
       horizontal(true),
       HG(_height,_width,_style)
       //VG(_height,_width,_style)
    {
      rows=0;
      columns=0;
      defcolumns=-1;
      num_elem=0;
    }
    void OnStart()throw()
    {
      BoxGroup::AddWidget(HG,1);
      BoxGroup::OnStart();
      BoxGroup::SetActiveWidget(HG);
    }
    
    virtual void AddWidget(Widget& widget, Uint stretchFactor)throw()
    {
      HG.GetNumElements();
      for (int i = 0 ; i!= columns; ++i)
	{
	  if (VG[i]->GetNumElements()<rows)
	    {
	      num_elem++;
	      VG[i]->AddWidget(widget, 1);
	      if (columns==1 && rows==1)
		{
		  HG.SetActiveWidget(*VG[i]);
		  VG[i]->SetActiveWidget(widget);
		}
	      return;
	    }
	}
      if (columns==defcolumns || rows<columns)
	{
	  rows++;
	}
      else
	{
	  VG.push_back(new VerticalGroup(GetHeight(),GetWidth(),GetStyle()));
	  HG.AddWidget(*VG.back(),1);
	  columns++;
	}
      AddWidget(widget,stretchFactor);
    }
    void SetNumColumns(int col)throw()
    {
      defcolumns=col;
      rows=num_elem/defcolumns;
      if (num_elem>rows*defcolumns)
	rows++;
      if (columns>col)
	{
	  for (int i = col ; i!= columns; ++i)
	    {
	      while ( 1 )
		{
		  try 
		    {
		      //VG[1]->OnFocus(TabFocus);
		      
		      Widget& w = *VG[i]->GetFirstElement();
		      VG[i]->DelWidget(w);
		      AddWidget(w,1);
		      num_elem--;
		    }
		  catch (...)
		    {
		      break;
		    }
		    
		}
	      HG.DelWidget(*VG[i]);
	      VG.erase(VG.end()-1, VG.end());
	    }
	  columns=col;
	}
      else
	{
	  int oldcolumns=columns;
	  for (int i = oldcolumns ; i!= col; ++i)
	    {
	      VG.push_back(new VerticalGroup(GetHeight(),GetWidth(),GetStyle()));
	      HG.AddWidget(*VG.back(),1);
	      columns++;
	    }
	  for (int i = 0; i!=oldcolumns; ++i)
	    {
	      while (VG[i]->GetNumElements()>rows)
		{
		  Widget& w = *VG[i]->GetFirstElement();
		  VG[i]->DelWidget(w);
		  num_elem--;
		  AddWidget(w,1);
		}
	    }


	}

      ArrangeContents();
    }
    void ArrangeContents()throw()
    {
      //WidgetGroup::ArrangeContents();
      HorizontalGroup::ArrangeContents();
      //BoxGroup::ArrangeContents();
      // if (horizontal)
      // 	HorizontalGroup::ArrangeContents();
      // else
      // 	VerticalGroup::ArrangeContents();
    }
    RTTI_OBJ2(MultiGroup, HorizontalGroup, VerticalGroup);
  };



class MPStackUI:public RootWindow
{

public:
  MPStackUI(std::string & pname)throw();
  void OnStart()throw();
  void OnResize()throw();
  void OnKeyDown(Key code)throw();
  ~MPStackUI()throw();

	
private:
  string pname;
  MultiGroup *group;
  std::set<ProcessUI*> process;
};

SampleLabel::SampleLabel(const std::string& label)throw()
  :Label(label, DisplayStyle(Fg::White, Fg::Dark, Bg::Transparent))
{
;

}

void SampleLabel::OnResize()throw()
{
  ;
}

MPStackUI::MPStackUI(string & pn)throw()
  :RootWindow(std::cin,std::cout,
	      Scr::DisplayStyle(Scr::Fg::White,
				Scr::Fg::Bright,
				Scr::Bg::Cyan)),
   pname(pn)
{
  // configure temporary file
  char fn[] = "/tmp/mpstack.XXXXXX";
  mktemp(fn);
  
  mkfifo(fn,0700);
  TempFN.assign(fn);
  system( (string("sleep 1; echo >")+TempFN+"&").c_str());
  TempF.open(fn);
  ;


}

void MPStackUI::OnKeyDown(Key code)throw()
{
  if (code == Key::EoF)
    Exit(1);
  if (code == 'r')
    {
      for (auto it = process.begin(); it!=process.end(); ++it)
	{
	  ProcessUI * nprocess = *it;
	  try
	    {
	      //cerr << "printing process " << nprocess << endl;
	      nprocess->PrintStack();
	    }
	  catch (...){}
	}

    }
  // if (code == Key::Tab)
  //   {
  //     group->PassFocusRequest(TabFocus);
  //   }
  if (code >='1' && code <='6')
    {
      group->SetNumColumns(code-'0');
    }
  // if(code == 'v') {
  //   group->ToggleHorizontal();
  //   RedrawRequest();
  //   return;
  // }


}
ProcessUI::ProcessUI(int _PID, int _TID, const DisplayStyle &  _style):
  BoxGroup(0, 0, _style),
  VerticalGroup(0, 0, _style),
  numLabels(0)
{
  PID=_PID;
  TID=_TID;
  stack_n.resize(100);
  SetAlignPolicy(BoxGroup::Begin);
  std::stringstream strst;
  numLabels=0;
  for (int i = 0 ; i!=100; ++i)
    stack_n[i]=0;
}
void ProcessUI::PrintStack()
{
  for (int i = 2 ; i!=100; ++i)
    {
      if (stack_n[i])
	{
	  DelWidget(*stack_n[i]);
	  delete stack_n[i];
	  stack_n[i]=0;
	}
    }
  //elements.clear();
  stringstream strst;
  strst.str("");
  strst<< "/proc/" << PID <<"/task/" << TID<<"/comm";
  //  cerr<<strst.str();
  ifstream StatC(strst.str().c_str());
  string name;
  getline(StatC, name);
  //StatC >> name;
  StatC.close();
  strst.str("");
  strst << "["<< PID << "/"<<TID <<" : " << name <<"]";
  if (numLabels==0)
    {
      stack_n[0] = new SampleLabel(strst.str());
      stack_n[0]->SetStyle(
      DisplayStyle(Fg::White,Fg::Dark,
		   Bg::Blue));
    }
  // get usage
  strst.str("");
  strst<< "/proc/" << PID <<"/task/" << TID<<"/stat";// | cut -d ' ' -f 14,15 > " << TempFN;
  //  cout<< "executing "<< strst.str() << endl;

  ifstream psstat(strst.str().c_str()); 
  if (!psstat.is_open())
    {
	stack_n[2] = new SampleLabel("died");
	
	AddWidget(*stack_n[2]);

    throw 0;
    }
  for (int i = 0 ; i!=13; ++i)
    {
      string s;
      psstat >> s;
    }
  // open temporary file
  
  int utime;
  int stime;
  psstat >> utime >> stime;
  psstat.close();
  
  float cumul_time;
  
  cumul_time = float(utime+stime)/ sysconf(_SC_CLK_TCK);
  float util = (cumul_time-prev_cumul)*100;
  prev_cumul = cumul_time;
  
  strst.str("");
  strst<< "cpu: "<< 
    (float(utime)/ sysconf(_SC_CLK_TCK)) << "s U, "<<

    (float(stime)/ sysconf(_SC_CLK_TCK)) << "s K, "<<
    util << "% util";// << " file: " << TempF.eof();
  if (numLabels==0)
    stack_n[1] = new SampleLabel(strst.str());
  else
    stack_n[1]->SetText(strst.str()+"+");
  if (numLabels==0)
    {
      AddWidget(*stack_n[0]);
      AddWidget(*stack_n[1]);
    }
      numLabels=2;
  // get stack
 
  strst.str("");
  strst<< "/proc/" << PID <<"/task/" << TID<<"/stack";
  //  cerr<<strst.str();
  ifstream StatF(strst.str().c_str());
  if (!StatF.is_open())
    {
      numLabels++;
      stack_n[numLabels-1] = new SampleLabel("DIED");
      AddWidget(*stack_n[numLabels-1]);      
      return;
    }
  while(!StatF.eof())
    {
      string addr;
      string ident;
      StatF>>addr;
      //atF>>ident;
      getline(StatF, ident);
  
      if (ident.empty())
	break;
      numLabels++;
      replace(ident.begin(), ident.end(), '+' , '\0' );
      stack_n[numLabels-1] = new SampleLabel(ident);
      AddWidget(*stack_n[numLabels-1]);      
    }
  StatF.close();

}
ProcessUI::~ProcessUI()throw()
{
  //  cerr << "killing process " << this << endl;
  for (int i = 0 ; numLabels>i ; i++) {
    delete stack_n[i];
  }

}
#include <stdio.h>
#include <time.h>
#include <signal.h>

timer_t gTimerid;

void start_timer(void)
{


  struct itimerspec value;

  value.it_value.tv_sec = 1;
  value.it_value.tv_nsec = 0;

  value.it_interval.tv_sec = 1;
  value.it_interval.tv_nsec = 0;

  timer_create (CLOCK_REALTIME, NULL, &gTimerid);

  timer_settime (gTimerid, 0, &value, NULL);

}

void stop_timer(void)
{


  struct itimerspec value;

  value.it_value.tv_sec = 0;
  value.it_value.tv_nsec = 0;

  value.it_interval.tv_sec = 0;
  value.it_interval.tv_nsec = 0;

  timer_settime (gTimerid, 0, &value, NULL);


}

MPStackUI * a;
void timer_callback(int sig) {
  a->OnKeyDown('r');
  a->RedrawRequest();
  //printf(" Catched timer signal: %d ... !!\n", sig);

}
void MPStackUI::OnStart()throw()
{
  group = new MultiGroup(size.height-2, size.width -2,
			 Scr::DisplayStyle(Scr::Fg::White,
					   Scr::Fg::Bright,
					   Scr::Bg::Yellow));

  // get usage
  stringstream strst;
  strst.str("");
  strst<< 
    "("
    "for j in `pgrep " << pname << " ` ; "
    "  do for i in `ls /proc/$j/task/`; "
    "    do echo $j $i ;"
    "  done ;"
    "done ; "
    "echo 0 0 ) >" << TempFN;


    //"( pgrep " << pname <<" ; echo 0 ) > " << TempFN;
  cout<< "executing "<< strst.str() << endl;
  system(strst.str().c_str());
  bool first=false;
  while ( 1 )
    {

      int pid;
      int tid;
      TempF>>pid;
      TempF>>tid;
      if (!pid)
	break;


      ProcessUI * nprocess;
      nprocess = new ProcessUI (pid, tid , 
				Scr::DisplayStyle(Scr::Fg::Red,
						  Scr::Fg::Bright,
						  Scr::Bg::Black));
      
      process.insert(nprocess);
      //cerr << "spawning process " << nprocess << endl;
    }

  for  (  std::set<ProcessUI*>::iterator it = process.begin(); it!=process.end(); ++it)
    {
      ProcessUI * nprocess = *it;
      try
	{
	  nprocess->PrintStack();
	}
      catch (...)
	{
	  // //process.erase(nprocess);
	  // delete nprocess;
	  // *it = 0;
	  //cerr << "failing process " << nprocess << endl;
	  continue;
	}
      group->AddWidget(*nprocess, 4);
      first=true;
    }
  process.erase(0);
  AddWidget(*group);
  SetActiveWidget(*group);
  group->SetPosition(1, 1);
	
  RootWindow::OnStart();


   (void) signal(SIGALRM, timer_callback);
  start_timer();
}

void MPStackUI::OnResize()throw()
{
  group->SetSize(Size(size.height-2, size.width-2));	
  //1  std::cout << size.height-2 << " " <<  size.width-2;

  RootWindow::OnResize();
}

MPStackUI::~MPStackUI()throw()
{
  delete group;
  for (auto i = process.begin(); i!=process.end(); ++i)
    delete *i;
  TempF.close();
  unlink(TempFN.c_str());
}

int main (int argc,char ** argv)
{    
  if (argc<2)
    {
      puts("usage: mpstack [pname]");
      return 1;
    }
  string name = argv[1];
  MPStackUI app(name);
  a=&app;
  return app.Start();
}
/*end of main function of program*/

