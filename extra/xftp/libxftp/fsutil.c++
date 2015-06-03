#include "fsutil.h++"
#include "libstreamcon/throw.h++"


namespace xftp
{
	void SwitchDirectory (boost::filesystem::path& newdir, 
			const boost::filesystem::path& olddir)
	{
		
		if (newdir.root_directory ().empty ())
		{
			
			boost::filesystem::path tmp (olddir);
			tmp/=newdir;
			newdir=boost::filesystem::path ();
			for (boost::filesystem::path::iterator i = tmp.begin () ;
			i!= tmp.end () ; ++i)
			{
				if (*i!="..")
				{// process path
					if (*i!=".")
						newdir /=*i;
				}
				else// process directory-up
				{
					if (newdir.empty () or newdir == "/")
						THROW (InvalidDir);
					else
					{
						newdir.remove_leaf ();
						newdir.remove_leaf ();
					}
				}
			}
			if (newdir.empty())
				newdir="/";					
		}
		else
		{
			if (newdir.leaf()==".")
				newdir.remove_leaf ();// simplify				
		}
	}
}
