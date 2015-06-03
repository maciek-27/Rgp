#ifndef ___XFTPCLIENT__REAL_APP__SUPERCONTROL_INT_
#define ___XFTPCLIENT__REAL_APP__SUPERCONTROL_INT_

#include<xercesc/util/XMLString.hpp>
#include<xercesc/util/XMLUniDefs.hpp>
#include <libxftp/server/error.h++>
#define XFTP_DIE(t,d)														 \
							{												 \
								std::cout << "\x1b[36m"<< t <<"\x1b[0m"		 \
									<<	std::endl;							 \
								d;											 \
							}

#define XFTP_ASSERT(M,t,d)													 \
		if (xercesc::XMLString::equals((XMLCh[]){xercesc::chLatin_e,		 \
							xercesc::chLatin_r,	xercesc::chLatin_r,			 \
							xercesc::chLatin_o,xercesc::chLatin_r,			 \
						    xercesc::chNull}, M->GetName()))				 \
								XFTP_DIE(t<<"\n" <<							 \
									reinterpret_cast<xftp::server::MsgError*>\
									(M.get())->GetTextMsg(),d)



#endif
