cmd_Release/obj.target/kiflash.node := g++ -shared -pthread -rdynamic -m64  -Wl,-soname=kiflash.node -o Release/obj.target/kiflash.node -Wl,--start-group Release/obj.target/kiflash/src/kiflash.o -Wl,--end-group 
