#ifndef _TLSFILTER_H_
#define _TLSFILTER_H_
#include <libstreamcon/stream.h++>
#include <openssl/bio.h>
#include <openssl/ssl.h>

namespace streamcon {


	class Connection;

	/*!
	  Class representing one separate logical data stream.
	  This particular Stream type specializes in data
	  encryption via the TLS method in OpenSSL library.

	  \note to use this Stream, it has to be initialized
	  via the \a InitializeCTX method.
	*/
	class TLSFilter : public Stream::IOFilter {
	private:

		BIO* bio, *io;
		SSL_CTX* ctx;
		std::string password;

	public:
		__DE(CTXInitError, Exception);
		__DE(BadCertificateFile, CTXInitError);
		__DE(BadKey, CTXInitError);
		__DE(BadCAList, CTXInitError);
		__DE(BadDHFile, CTXInitError);

		TLSFilter();
		virtual ~TLSFilter();

		/*!
		  \param keyfile path to key certificate.
		  \param _password password
		  \param calist path to file containing trusted CAs
		  Initialize SSL control structures on the client side.

		  \exception BadCertificateFile
		  \exception BadKey
		  \exception BadCAList
		 */
		void InitializeCTX(const std::string& keyfile,
						   const std::string& _password,
						   const std::string& calist)
			throw(BadCertificateFile, BadKey, BadCAList);

		/*!
		  \param keyfile path to key certificate.
		  \param _password password
		  \param calist path to file containing trusted CAs
		  Initialize SSL control structures on the server side.

		  \exception BadCertificateFile
		  \exception BadKey
		  \exception BadCAList
		  \exception BadDHFile
		 */
		void InitializeCTX(const std::string& keyfile,
						   const std::string& dhfile,
						   const std::string& _password,
						   const std::string& calist)
			throw(BadCertificateFile, BadKey, BadCAList,
				BadDHFile);

		/*!
		  Returns certificate password from the current TLS
		  context.
		 */		
		const std::string& GetPassword() const throw();
		
		__DE(NotInitialized, Exception);
		__DE(SessionNotStarted, Exception);
		__DE(NegotiationError, Exception);
		__DE(ConnectFailed, NegotiationError);
		__DE(AcceptFailed, NegotiationError);
		
		/*!
		  Start TLS negotiation. Other side has to call
		  \a AcceptTLS.
		  \exception NotInitialized is thrown if the
		  InitializeCTX hasn't been done.
		  \exception ConnectFailed is thrown upon SSL_connect()
		  function failure.
		 */
		void StartTLS() throw(NotInitialized, ConnectFailed);

		/*!
		  Accept TLS negotiation after the other side has
		  called \a StartTLS.
		  \exception NotInitialized is thrown if the
		  InitializeCTX hasn't been done.
		  \exception AcceptFailed is thrown upon SSL_accept()
		  function failure.
		 */
		void AcceptTLS() throw(NotInitialized, AcceptFailed);

		//! callback from C ssl functions
		static void __PassWrite(TLSFilter *obj, char *buffer, size_t size) {
			obj->IOFilter::WriteFilter(buffer, size);
		}
		//! callback from C ssl function
		static int __PassRead(TLSFilter *obj, char *buffer, size_t size) {
			return obj->IOFilter::ReadFilter(buffer, size);
		}


		void WriteFilter(char *buffer, size_t size)
			throw(DataBuffer::Interrupted, Stream::StreamError);
		
		int ReadFilter(char *buffer, size_t size)
			throw(DataBuffer::Interrupted, Stream::StreamError);
	};

}
#endif
