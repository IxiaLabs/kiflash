# Kiflash for Node.js



### Good to Know (TM)

There are two components to this.  A command line utility and a core components that is used by antman.

The build instruction for the command line component:<br />
cd /home/ixiaadmin/git/kiflash/kiflashcmd<br />
sudo make<br />

The build instruction for core component:<br />
cd /home/ixiaadmin/git/kiflash/kiflashcore/src/build<br />
sudo rm *.o<br />
cd /home/ixiaadmin/git/kiflash/kiflashcore/src<br />
sudo ./build.sh<br />
cd /home/ixiaadmin/git/kiflash<br />
sudo npm install --unsafe-perm<br />


sudo cp /home/ixiaadmin/git/kiflash/build/Release/kiflash.node /home/ixiaadmin/git/antman/server<br />
sudo cp /home/ixiaadmin/git/kiflash/build/Release/kiflash.node /opt/ixia/mmap<br />
sudo cp /home/ixiaadmin/git/kiflash/build/Release/kiflash.node /home/ixiaadmin/git/antman/local_modules/streaming-worker<br />
sudo cp /home/ixiaadmin/git/kiflash/build/Release/kiflash.node /home/ixiaadmin/git/antman/mmap<br />
sudo cp /home/ixiaadmin/git/kiflash/build/Release/kiflash.node /home/ixiaadmin/git/antman/dist/A400G<br />
