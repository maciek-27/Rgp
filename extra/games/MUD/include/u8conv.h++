#ifndef __U8CONV_H__
#define __U8CONV_H__
//// Aux namespace : namespace containing code for MUD project but not
//// directly connected to gameplay and potentially reusable (this is
//// excluding configuration files etc, which can be reused only after
//// modification - this includes general purpose classes and
//// subroutines only)

//// to_u8 is utility function, that is planned to be released for 1.2
//// version of RexIO library, but - as development of library is
//// somewhat stalled - for now it must be implemented right here
namespace MUD { namespace Aux { std::string to_u8(std::wstring text); }}

#endif
