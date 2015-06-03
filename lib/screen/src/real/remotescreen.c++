#include"fileno_hack.h++"
#include"screen.h++"
#include"screenbuffer.h++"
#include"genericscreen.h++"
#include"remotescreen.h++"
#include"telnet.h++"
#include"throw.h++"
#include <iostream>
#include <queue>
#include <cassert>
#include <sys/select.h>
Scr::RemoteScreen::RemoteScreen(Connection & _connection,
								std::istream & _input,
								std::ostream & _output)throw()
    :GenericScreen(_input,_output),__ScreenConnection(_connection,_input),
     requestedSize(25,80),resizeRequestPending(false),counter(0),
	 telnetSettings(0)
{
    using namespace std;
    using namespace TELNET;
 
    clientname.reserve(64);
	
    //following two lines enable char-by-char mode for all
    //standard-compliant telnet clients by forcing them to disable
    //local echo and not waiting for GA (therefore each character
    //typed by user will be delivered to server as soon as possible,
    //and displayed only if server will allow it to)
    output << IAC << WILL << ECHO
		   << IAC << WILL << SGA 
	
	//Request TTYPE information. In this application specific case,
	//client MUST answer IAC WILL TTYPE. Refusal or ignoring this
	//request will result in considering client dumb terminal and
	//breaking connection, as no graphic advanced capabilities
	//provided disable usage of screen interfaces
		   << IAC << DO << TTYPE//< enable TTYPE mode
		   << IAC << SB << TTYPE << SEND
		   << IAC << SE // < request TTYPE info RIGHT NOW
			
	//suggest negotiation about window size
	//client must answet  IAC WILL NAWS, and then supply proper number
	//to use this feature: otherwise default 24 row / 80column  will
	//be assumed (refer to GenericScreen constructor)
		   << IAC << DO << NAWS << flush;
	
    //diagnostics
    RexIOLog(LogLevelModerate)<<"RemoteScreen(std::ostream & _output)"<<endl;
}

Scr::Key Scr::RemoteScreen::DecodeKeyPressedHandleTelnet()
{
	Uint result;
    try
    {
		result = DecodeKeyPressed();
	}
	catch(Scr::Connection::UnsupportedKey)
	{
		while (input.HasBufferedText())
			result=input.Get();
	}
	if (result == 0xd ) // Carriage return - ignore LF
	{
		Uint i = input.Get(); // ignore next
		if (i == 0 or i == 0xa)
			return Key::Enter; // some TELNET clients don't really provide line
						// feed after CR. 
		else
			THROW(LogicError);
	}
	return result;
}

#define SUBNDESC(feature) case feature: RexIOLog(LogLevelModerate) << #feature "\n"; break
void Scr::RemoteScreen::AnswerCommand()
{
    using namespace TELNET;
    using namespace std;
    unsigned char c[2];
    c[0]=input.Get();
	assert(c[0]==IAC);
    c[0]=input.Get();
    c[1]=input.Get();
    RexIOLog(LogLevelModerate) << "Client says "<< static_cast<int>(c[0]) <<" what means, it ";
    switch (c[0])
    {
    case WILL:
    case DO:
		RexIOLog(LogLevelModerate) << "agrees on (or requests) ";
		switch(c[1])
		{
			SUBNDESC(SGA);
			SUBNDESC(NAWS);
			SUBNDESC(TTYPE);
			SUBNDESC(ECHO);
			SUBNDESC(LINEMODE);
		default:
			RexIOLog(LogLevelModerate) << "unsupported feature\n";
		}
		break;
    case WONT:
    case DONT:
		RexIOLog(LogLevelModerate) << "disagrees on (or requests not to use) ";
		switch(c[1])
		{
			SUBNDESC(SGA);
			SUBNDESC(NAWS);
			SUBNDESC(TTYPE);
			SUBNDESC(ECHO);
			SUBNDESC(LINEMODE);
		default:
			RexIOLog(LogLevelModerate) << "unsupported feature\n";
		}
		break;
    case SB:
		RexIOLog(LogLevelModerate) << "wants to subnegotiate ";
		switch(c[1])
		{
		case NAWS:
			RexIOLog(LogLevelModerate) << "Window size\n";
			SubnegotiateWindowSize();
			break;
		case TTYPE:
			RexIOLog(LogLevelModerate) << "Terminal type\n";
			SubnegotiateTerminalType();
			break;
		default:
			THROW(Scr::Connection::IllegalTelnetAction);
			// if what client says is agreement or disagreement, just
			// ignore it., but subnegotiations are way too
			// unpredictable: even waiting for SE would not be sufficient.
			// threrefore: we have to support as much of them as possible,
			// and throw exception on unsupported!
		}
		break;
    default:
		THROW(Scr::Connection::UnsupportedTelnetFeature);
    }
    
}
#undef SUBNDESC

void Scr::RemoteScreen::SubnegotiateWindowSize()
{
    
    using namespace TELNET;
    using namespace std;
    requestedSize.width = input.Get();
    requestedSize.width <<=8;
    requestedSize.width += input.Get();
    requestedSize.height = input.Get();
    requestedSize.height <<=8;
    requestedSize.height += input.Get();
    RexIOLog(LogLevelModerate) << "New height is "<<requestedSize.height << ", width is "<<requestedSize.width<<"\n";

    unsigned char c;
    c = input.Get();
    if (c!= IAC)
		THROW(Scr::Connection::IncorrectWindowSizeSubnegotiation);
    c = input.Get();
    if (c!= SE)
		THROW(Scr::Connection::IncorrectWindowSizeSubnegotiation);

    RexIOLog(LogLevelModerate) << "Subnegotiation correct - setting\n";

	if (requestedSize.height!=GetHeight() || requestedSize.width!=GetWidth())
	{
		if (!resizeRequestPending)
		{
			counter=-10;// ok! first request should be supported instantly!
			resizeRequestPending=true;
		}
		else
			counter = 100;//app is requesting too frequent resize!
		//wait at least 0.156 s for refresh to prevent byte-flood DoS.
	}
	telnetSettings|=windowSize;
}

const char * Scr::RemoteScreen::GetType() const throw(TerminalTypeUnknown)
{
	EASSERT(!clientname.empty(),TerminalTypeUnknown);
	return clientname.c_str();
}
static const std::size_t maxInputExcess = 1024;// yet another anti-DoS insurance
int Scr::RemoteScreen::ProcessConnection()
{
    using namespace TELNET;

    active=true;
	// initialization block: first initialize TELNET session, then
	// start parsing input (keyboard input is meaningless unless we
	// know term type).
	{
		std::queue<char> toDecode;
		while (input.Stream().good() and active)
		{
			input.Buffer();
			while (input.HasBufferedText())
			{
				int c = input.Peek();
				if (c == -1)
				{
					THROW(Scr::Connection::Broken);			
				}
				if (c==IAC)
					AnswerCommand();
				else
					toDecode.push(input.Get());
			}
			if (// OK. setup is finished.
				(telnetSettings & (windowSize|terminalType) )
				== (windowSize|terminalType)
				)
				break;
			if (toDecode.size()>maxInputExcess)
				THROWP(Scr::Connection::Broken,
						"Maximum input lag limit exceeeded");
		}
		
		connection.OnStart(); // now app is initialized and may
							  // successfully be started. 

		if (resizeRequestPending) // request may have been sent during initialization
		{
			Resize(requestedSize.height,requestedSize.width);
			connection.OnResize(GetHeight(),GetWidth());
			resizeRequestPending=false;
		}

		// if some chars other than TELNET negotiations were recieved,
		// now is time to parse 'em
		while  (!toDecode.empty())
			connection.OnKeyDown(DecodeKeyPressedHandleTelnet());
		
	}// free stack resources allocated for initialization (in
	 // particular: queue toDecode, which now is not needed.

	

	int inFD=input.FD();
    struct timeval t;
    fd_set fds;// to allow instant Exit() without waiting for char
	           // application must handle asynchronous input
	           // processing correctly, what isn't supported by
	           // standart libstdc++ in linux.

    FD_ZERO(&fds);    
    while (input.Stream().good() and active)
    {
		FD_SET(inFD,&fds);
		t.tv_sec=0;
		t.tv_usec=1000;
		// 1+onlyFD = maximum FD
		if (select (1+inFD, &fds, 0, 0, &t))
		{
			if (static_cast<std::size_t>
					(input.Stream().rdbuf()->in_avail())
					>maxInputExcess)
				THROWP(Scr::Connection::Broken,
						"Maximum input lag limit exceeeded");
			input.Buffer();
			while (input.HasBufferedText())
			{
				int c = input.Peek();
				if (c == -1)
				{
					THROW(Scr::Connection::Broken);			
				}
				if (c==IAC)
					AnswerCommand();
				else
				{
					connection.OnKeyDown(DecodeKeyPressedHandleTelnet());
				}
			}
		}
		if (!counter)
		{// check every 0.256 s. if resize request is pending.
			if (resizeRequestPending)
			{
				Resize(requestedSize.height,requestedSize.width);
				connection.OnResize(GetHeight(),GetWidth());
				resizeRequestPending=false;
			}
		}
		counter++; // as counter is one-byte, it is equivalent to code:
		           // if (counter == 255)
		           //     counter = 0;
		           // else
		           //     counter++;
	}	           //

	EASSERT(!active,FatalException);

    CleanUp();
    
    return exitcode; // OnExit is called by Connection::Start().
}

//SUBNEGOTIATE TTYPE  (assume, IAC SB TTYPE  already recvd)
void Scr::RemoteScreen::SubnegotiateTerminalType()
{
	using namespace TELNET;
//IAC SB TERMINAL-TYPE IS ... IAC SE
	unsigned char c;
	c=input.Get();
	EASSERT(c==IS,Scr::Connection::IncorrectTerminalTypeSubnegotiation);

	Uint i=0;
	while (  (c=input.Get())!=IAC )
	{
		clientname+=tolower(c);
		i++;
		EASSERT(i<clientname.capacity(),
				Scr::Connection::IncorrectTerminalTypeSubnegotiation);
		
	}
	c=input.Get();
	EASSERT(c==SE,Scr::Connection::IncorrectTerminalTypeSubnegotiation);
	std::cout << "Client term type is: " << clientname << '\n';
	telnetSettings|=terminalType;
}

Scr::RemoteScreen::~RemoteScreen()throw()
{
    using namespace std;
    RexIOLog(LogLevelModerate)<<"~RemoteScreen()"<<endl;
}

