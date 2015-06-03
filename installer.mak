install:
	install lib/screen/librexio.so /usr/lib
	install lib/toolkit/librexiotk.so /usr/lib
	rm -fr /usr/include/rexio
	mkdir /usr/include/rexio
	install include/rexio/*.h++ /usr/include/rexio
	mkdir /usr/include/rexio/tk
	install include/rexio/tk/*.h++ /usr/include/rexio/tk
	ldconfig

uninstall:
	rm -f /usr/lib/librexio.so	
	rm -f /usr/lib/librexiotk.so
	rm -rf /usr/include/rexio
