# Kiflash for Node.js



### Good to Know (TM)

There are two components to this.  A command line utility and a core components that is used by antman.

The build instruction for the command line component:
cd /home/ixiaadmin/git/kiflash/kiflashcmd
sudo make

The build instruction for core component:
cd /home/ixiaadmin/git/kiflash/kiflashcore/src/build
sudo rm *.o
cd /home/ixiaadmin/git/kiflash/kiflashcore/src
sudo ./build.sh
cd /home/ixiaadmin/git/kiflash
sudo npm install --unsafe-perm


sudo cp /home/ixiaadmin/git/kiflash/build/Release/kiflash.node /home/ixiaadmin/git/antman/server
sudo cp /home/ixiaadmin/git/kiflash/build/Release/kiflash.node /opt/ixia/mmap
sudo cp /home/ixiaadmin/git/kiflash/build/Release/kiflash.node /home/ixiaadmin/git/antman/local_modules/streaming-worker
sudo cp /home/ixiaadmin/git/kiflash/build/Release/kiflash.node /home/ixiaadmin/git/antman/mmap
sudo cp /home/ixiaadmin/git/kiflash/build/Release/kiflash.node /home/ixiaadmin/git/antman/dist/A400G
