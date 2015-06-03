#ifndef __TELNET_H__
#define __TELNET_H__

//! Telnet control codes
/*!
  Whole set of constants useful for telnet negotiations as server or
  client. All of them are declared in apropriate RFC's.
*/
namespace TELNET
{
 
    
    //!\brief Binary mode
    //!
    const unsigned char   BINARY = 0x00;
    //!\brief Local/remote echo mode
    //!
    //!IAC WILL ECHO sent by server disables local echo
    //!\sa RFC 857
    //!\latexonly \index{RFC, reference to!857}\endlatexonly
    const unsigned char     ECHO = 0x01;
    //!\brief Suppress go ahead
    //!
    //!\sa RFC 858
    //!\latexonly \index{RFC, reference to!858}\endlatexonly
    const unsigned char      SGA = 0x03; 
    //!\brief Terminal Type negotiation
    //!
    //!Detect terminal type and - possibly - detect it's additional
    //!emulation modes and switch between them. Documentation for this
    //!feature described in appropriate RFC.
    //!
    //!\sa RFC 1091
    //!\latexonly \index{RFC, reference to!1091}\endlatexonly
    const unsigned char    TTYPE = 0x18; 

    //!\brief request terminal type information
    //!
    //!Command code used by server while requesting TTYPE
    //!\sa RFC 1091
    const unsigned char     SEND = 0x01;

    //!\brief inform about terminal type
    //!
    //!Command code used by client while informing about terminal type
    //!during TTYPE subnegotiation
    //!\sa RFC 1091
    const unsigned char       IS = 0x00;
    
    //!\brief Negotiate about Window Size
    //!
    //!\sa RFC 1073
    //!\latexonly \index{RFC, reference to!1073}\endlatexonly
    const unsigned char     NAWS = 0x1F;

	
    //!\brief Line mode negotiation
    //!
    //!For description of this feature refer to appropriate RFC
    //!\sa RFC 1184
    //!\latexonly \index{RFC, reference to!1184}\endlatexonly
    const unsigned char LINEMODE = 0x24;

    //!\brief Subnegotiation end
    //!
    //!Special code inserted at the end of subnegotiation block
    const unsigned char       SE = 0xF0;
    //!\brief No operation
    //!
    //!Do not do anything
    const unsigned char      NOP = 0xF1;
    //!\brief Data mark
    //!
    const unsigned char       DM = 0xF2;
    //! Break
    const unsigned char      BRK = 0xF3;
    //! Interrupt Process
    const unsigned char       IP = 0xF4;
    //! Abort Output
    const unsigned char       AO = 0xF5;
    //! Are you there?
    const unsigned char      AYT = 0xF6;
    //! Erase character
    const unsigned char       EC = 0xF7;
    //! Erase line
    const unsigned char       EL = 0xF8;
    //! Go ahead (allow other end to transmit)
    const unsigned char       GA = 0xF9;
    //! Subnegotiation begin
    const unsigned char       SB = 0xFA;
    //! Will (meaning depends on feature, we negotiate)
	const unsigned char     WILL = 0xFB; 
    //! Won't (meaning depends on feature, we negotiate)
    const unsigned char     WONT = 0xFC;
    //! Do (meaning depends on feature, we negotiate)
    const unsigned char       DO = 0xFD;
    //! Don't (meaning depends on feature, we negotiate)
    const unsigned char     DONT = 0xFE;
    //!\brief Interpret as command 
    //!
    //!Special code in the beginning of all control sequences.
    const unsigned char      IAC = 0xFF;
}

#endif /* __TELNET_H__ */
