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


### XDMA
# To build:
cd ~/git/kiflash/dma/XDMA/linux-kernel/xdma/
sudo make
cd ~/git/kiflash/dma/XDMA/linux-kernel/tools/
sudo make

# After Build, copy to Antman branch:
sudo cp ~/git/kiflash/dma/XDMA/linux-kernel/xdma/xdma.ko ~/git/antman/mmap/dma
sudo  cp ~/git/kiflash/dma/XDMA/linux-kernel/tools/dma_from_device ~/git/antman/mmap/dma
sudo  cp ~/git/kiflash/dma/XDMA/linux-kernel/tools/dma_to_device ~/git/antman/mmap/dma
sudo  cp ~/git/kiflash/dma/XDMA/linux-kernel/tools/reg_rw ~/git/antman/mmap/dma
sudo  cp ~/git/kiflash/dma/XDMA/linux-kernel/tools/performance ~/git/antman/mmap/dma

# To Install:
cd /opt/ixia/mmap/dma
sudo insmod ../xdma/xdma.ko poll_mode=1

# To Download:

Change the size follow the -s to match the byte size of the image under test
Change the path to the image file follow by -f
cd /opt/ixia/mmap/dma
sudo ./dma_to_device -d /dev/xdma0_h2c_1 -f /home/ixiaadmin/antman/ANT16400101S2_400G.bin -s 109366360 -c 1


# To Test:
The test code is in perforce
//isglabs/hardware/AntMan/SW/dma/XDMA/linux-kernel/tests
navigate to the tests folder and do

sudo bash run_test.py

