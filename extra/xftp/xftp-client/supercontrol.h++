#ifndef ___XFTPCLIENT__REAL_APP__SUPERCONTROL_
#define ___XFTPCLIENT__REAL_APP__SUPERCONTROL_
#include<boost/tokenizer.hpp>
#include <libxftp/clientcontrol.h++>
#include <libxftp/server/list.h++>
#include "client.h++"
namespace xftpclient
{
	/*!
	 * Implementation of xftp::ClientControl.
	 * handles control thread and command processing
	 */
	class SuperControl : public xftp::ClientControl
	{
	private:
		//! for recursive directory printing
		int recurse;
		//! for data transfer
		bool has_dstream;

		//! when one dstream used - refer to it w/ this id.
		static const int single_data = 4;
		//! request dstream if it does not exist
		xftp::DataThread* AssureDT (int n);
	public:
		//! default constructor
		SuperControl (xftp::ClientBase& client, streamcon::Stream& _stream) :
		ClientControl (client, _stream), recurse (0),
				has_dstream (false)
		{;}
		//! main function
		void Run ();
	protected:
		
		
		
		//! kills client program (exit command)
		//! \param optns Commandline parameters (without command itself)
		void Exit (Tokenizer &	optns);
		
		//! displays hints (help commands)
		//! \param optns Commandline parameters (without command itself)
		void Help (Tokenizer &	optns);
		
		
		//! changes remote working directory (cd command)
		//! \param optns Commandline parameters (without command itself)
		void ChDir (Tokenizer &	optns);
		//! changes local working directory (lcd command)
		//! \param optns Commandline parameters (without command itself)
		void LChDir (Tokenizer &	optns);
		
		//! lists remote working directory (ls command)
		//! \param optns Commandline parameters (without command itself)
		void List (Tokenizer &	optns);
		//! lists local working directory (lls command)
		//! \param optns Commandline parameters (without command itself)
		void LList (Tokenizer &	optns);
		
		//! Sends file
		//! \param optns Commandline parameters (without command itself)
		void Put (Tokenizer &	optns);
		//! gets file
		//! \param optns Commandline parameters (without command itself)
		void Get (Tokenizer &	optns);
		
		//! deletes remote file
		//! \param optns Commandline parameters (without command itself)
		void Remove (Tokenizer &	optns);
		//!deletes local file
		//! \param optns Commandline parameters (without command itself)
		void LRemove (Tokenizer &optns);
		
		//! creates remote directory
		//! \param optns Commandline parameters (without command itself)
		void MkDir (Tokenizer &	optns);
		//! creates local directory
		//! \param optns Commandline parameters (without command itself)
		void LMkDir (Tokenizer & optns);

		//! when asking for command
		const std::string GeneratePrompt() const;



	private:
		std::string PromptTemplate;
		//! Called by List and LList
		//! \param node - node to print
		void PrintNode (xftp::server::Node  & node);
		//! called by Help for verbose description
		//! \param i - iterator to command to describe
		void DescribeCommand (CommandIterator & i);
	};//SuperControl~~
}

#endif
