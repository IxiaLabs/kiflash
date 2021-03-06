sudo gcc -pthread -fPIC -fno-strict-aliasing -g -O2 -DNDEBUG -g -fwrapv -O3 -Wall -Wstrict-prototypes -I. -IInclude -I./Include -I/usr/include/x86_64-linux-gnu -I/usr/local/include -I/home/ixiaadmin/git/kiflashcore/src -c /home/ixiaadmin/git/kiflash/kiflashcore/src/pcimem.c -o build/pcimem.o
sudo gcc -pthread -fPIC -fno-strict-aliasing -g -O2 -DNDEBUG -g -fwrapv -O3 -Wall -Wstrict-prototypes -I. -IInclude -I./Include -I/usr/include/x86_64-linux-gnu -I/usr/local/include -I/home/ixiaadmin/git/kiflashcore/src -c /home/ixiaadmin/git/kiflash/kiflashcore/src/xspi_sinit.c -o build/xspi_sinit.o
sudo gcc -pthread -fPIC -fno-strict-aliasing -g -O2 -DNDEBUG -g -fwrapv -O3 -Wall -Wstrict-prototypes -I. -IInclude -I./Include -I/usr/include/x86_64-linux-gnu -I/usr/local/include -I/home/ixiaadmin/git/kiflashcore/src -c /home/ixiaadmin/git/kiflash/kiflashcore/src/xspi_options.c -o build/xspi_options.o
sudo gcc -pthread -fPIC -fno-strict-aliasing -g -O2 -DNDEBUG -g -fwrapv -O3 -Wall -Wstrict-prototypes -I. -IInclude -I./Include -I/usr/include/x86_64-linux-gnu -I/usr/local/include -I/home/ixiaadmin/git/kiflashcore/src -c /home/ixiaadmin/git/kiflash/kiflashcore/src/xspi_g.c -o build/xspi_g.o
sudo gcc -pthread -fPIC -fno-strict-aliasing -g -O2 -DNDEBUG -g -fwrapv -O3 -Wall -Wstrict-prototypes -I. -IInclude -I./Include -I/usr/include/x86_64-linux-gnu -I/usr/local/include -I/home/ixiaadmin/git/kiflashcore/src -c /home/ixiaadmin/git/kiflash/kiflashcore/src/xspi.c -o build/xspi.o
sudo gcc -pthread -fPIC -fno-strict-aliasing -g -O2 -DNDEBUG -g -fwrapv -O3 -Wall -Wstrict-prototypes -I. -IInclude -I./Include -I/usr/include/x86_64-linux-gnu -I/usr/local/include -I/home/ixiaadmin/git/kiflashcore/src -c /home/ixiaadmin/git/kiflash/kiflashcore/src/flash_qspi_rw.c -o build/flash_qspi_rw.o

sudo gcc -pthread -shared build/pcimem.o build/xspi_sinit.o build/xspi_options.o build/xspi_g.o build/xspi.o build/flash_qspi_rw.o -L/usr/lib/x86_64-linux-gnu -L/usr/local/lib -o libFlashProvider.so

sudo cp *.so ../../bin/Linux

