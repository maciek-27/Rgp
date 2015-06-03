#include "tlsfilter.h++"

#include <openssl/err.h>

using namespace streamcon;

static bool SSLInitialized;

static int filter_write(BIO* h, const char* buf, int num)
{
	if(!buf) return 0;

	try {
		TLSFilter::__PassWrite(reinterpret_cast<TLSFilter*>(h->ptr), 
							   const_cast<char *>(buf), num);
		return num;
	}
	catch(...)
	{
		return 0;
	}
}

static int filter_read(BIO* h, char* buf, int num)
{
	if(!buf) return 0;

	try {
		return TLSFilter::__PassRead(reinterpret_cast<TLSFilter*>(h->ptr), 
									 buf, num);
	}
	catch(...) {
		return 0;
	}			
}

static int filter_puts(BIO* h, const char* buf)
{
	if(!buf) return 0;

	const char *buf2 = buf;
	while(*buf2) {
		if(*buf2 == '\n') {
			TLSFilter::__PassWrite(reinterpret_cast<TLSFilter*>(h->ptr), 
								   const_cast<char *>(buf), buf2-buf);
			break;
		}
		
		buf2++;
	}
	return buf2 - buf;
}

static long filter_ctrl(BIO* h, int cmd, long arg1, void* arg2)
{
	return 1;
}

static int filter_new(BIO* h)
{
	h->init = 1;
	h->num = 0;
	h->flags = 0;
	return 1;
}

static int filter_free(BIO* h)
{
	return 0;
}


static int password_cb(char* buf, int size, int rwflag, void* userdata)
{
	const std::string &password =
		reinterpret_cast<TLSFilter*>(userdata)->GetPassword();
	
	strncpy(buf, password.c_str(), size -1);
	buf[size-1] = 0; // buffer has to be null terminated

	// size is either max buffer length-1 or the actual length
	return std::min(static_cast<int>(password.length()), size-1);
}

static BIO_METHOD filter_method = {
	BIO_TYPE_NONE, "stream",
	filter_write,
	filter_read,
	filter_puts,
	NULL,
	filter_ctrl,
	filter_new,
	filter_free	
};


BIO *BIO_new_filter(TLSFilter *filter)
{
	BIO *newbio = BIO_new(&filter_method);
	if(!newbio) {
		return NULL;
	}
	newbio->ptr = reinterpret_cast<char*>(filter);
	return newbio;
}

const std::string& TLSFilter::GetPassword() const throw()
{
	return password;
}

void TLSFilter::InitializeCTX(const std::string& keyfile,
							  const std::string& _password,
							  const std::string& calist)
	throw(BadCertificateFile, BadKey, BadCAList)
{
//	std::cout << "InitializeCTX " << keyfile << "|" <<
//		 _password << "|" << calist << std::endl;
	SSL_METHOD* meth = SSLv3_method();
	ctx = SSL_CTX_new(meth);

	password = _password;

	/* Load our keys and certificates*/
    if(!(SSL_CTX_use_certificate_chain_file(ctx, keyfile.c_str())))
		THROW(BadCertificateFile);
	SSL_CTX_set_default_passwd_cb(ctx, password_cb);
	SSL_CTX_set_default_passwd_cb_userdata(
		ctx, reinterpret_cast<void*>(this));
	
	if(!(SSL_CTX_use_PrivateKey_file(
			 ctx, keyfile.c_str(),SSL_FILETYPE_PEM))) {
		SSL_CTX_free(ctx);
		ctx = NULL;
		THROW(BadKey);
	}

    /* Load the CAs we trust*/
    if(!(SSL_CTX_load_verify_locations(ctx, calist.c_str(),0))) {
		SSL_CTX_free(ctx);
		ctx = NULL;
		THROW(BadCAList);
	}
	
#if (OPENSSL_VERSION_NUMBER < 0x00905100L)
    SSL_CTX_set_verify_depth(ctx,1);
#endif	
}

void TLSFilter::InitializeCTX(const std::string& keyfile,
							  const std::string& dhfile,
							  const std::string& _password,
							  const std::string& calist)
	throw(BadCertificateFile, BadKey, BadCAList, BadDHFile)
{
	InitializeCTX(keyfile, _password, calist);
	{
		DH *ret=0;
		BIO *biof;
		
		if ((biof=BIO_new_file(dhfile.c_str(),"r")) == NULL) {
			SSL_CTX_free(ctx);
			ctx = NULL;
			THROW(BadDHFile);
		}
		
		ret=PEM_read_bio_DHparams(biof,NULL,NULL,
								  NULL);
		BIO_free(biof);
		if(SSL_CTX_set_tmp_dh(ctx,ret) < 0) {
			SSL_CTX_free(ctx);
			ctx = NULL;
			THROW(BadDHFile);
		}
	}

}

TLSFilter::TLSFilter()
{
	static Mutex InitMutex;
	ScopedLock lock(InitMutex);
	
	if(!SSLInitialized) {
		SSL_library_init();
		SSL_load_error_strings();
		SSLInitialized = true;
	}
}

void TLSFilter::StartTLS() throw(NotInitialized, ConnectFailed)
{
	if(!ctx)
		THROW(NotInitialized);

	bio = BIO_new_filter(this);

	SSL* ssl = SSL_new(ctx);
	SSL_set_bio(ssl, bio, bio);
	

	io = BIO_new(BIO_f_buffer());
	BIO* ssl_bio = BIO_new(BIO_f_ssl());
	BIO_set_ssl(ssl_bio,ssl,BIO_CLOSE);
	BIO_push(io,ssl_bio);
	

	int err;
	if((err = SSL_connect(ssl) <= 0)) {
		std::string errors;
		while((err = ERR_get_error())) {
			char buffer[120];
			ERR_error_string(err, buffer);
			errors = errors + ", " + std::string(buffer);
		}
		THROWP(ConnectFailed, errors);
	}
}

void TLSFilter::AcceptTLS() throw(NotInitialized, AcceptFailed)
{
	if(!ctx)
		THROW(NotInitialized);

	bio = BIO_new_filter(this);
	SSL* ssl = SSL_new(ctx);
	SSL_set_bio(ssl, bio, bio);	
	

	io=BIO_new(BIO_f_buffer());
    BIO* ssl_bio = BIO_new(BIO_f_ssl());
	BIO_set_ssl(ssl_bio,ssl,BIO_CLOSE);
	BIO_push(io,ssl_bio);
	
	int err;
	if((err = SSL_accept(ssl)) <= 0) {
		std::string errors;
		while((err = ERR_get_error())) {
			char buffer[120];
			ERR_error_string(err, buffer);
			errors = errors + ", " + std::string(buffer);
		}
		THROWP(AcceptFailed, errors);
	}
}

void TLSFilter::WriteFilter(char *buffer, size_t size)
	throw(DataBuffer::Interrupted, Stream::StreamError)
{
	if(!ctx)
		THROW(NotInitialized);

	BIO_write(io, buffer, size);
}

int TLSFilter::ReadFilter(char *buffer, size_t size)
	throw(DataBuffer::Interrupted, Stream::StreamError)
{
	if(!ctx)
		THROW(NotInitialized);

	return BIO_read(io, buffer, size);
}

TLSFilter::~TLSFilter()
{
	SSL_CTX_free(ctx);
	BIO_free(bio);
}
