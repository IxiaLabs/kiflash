#include <iostream>
#include <chrono>
#include <thread>
#include <dlfcn.h>
#include "streaming-worker.h"

using namespace std;
void *handle;
void (*func_test)();
void (*func_flashtest)();
void (*func_flashprog)();

class Kiflash : public StreamingWorker {
  public:
    Kiflash(Callback *data, Callback *complete, Callback *error_callback,  v8::Local<v8::Object> & options) 
          : StreamingWorker(data, complete, error_callback){

        N = -1;
        handle = dlopen("/home/ixiaadmin/git/kiflash/bin/Linux/libFlashProvider.so", RTLD_LAZY);

        if (!handle) {
            /* fail to load the library */
            printf("Fail to load the libary\n");
            SetErrorMessage("Fail to load the libary");
            // Message tosend("error", "Fail to load the libary");
            // writeToNode(progress, tosend);            
        }

        *(void**)(&func_test) = dlsym(handle, "pcimem_test");
        if (!func_test) {
            /* no such symbol */
            printf("pcimem_test no such symbol\n");
            SetErrorMessage("pcimem_test no such symbol");
            // Message tosend("error", "pcimem_test no such symbol");
            // writeToNode(progress, tosend);            
        }    

        *(void**)(&func_flashtest) = dlsym(handle, "kiflash_test");
        if (!func_flashtest) {
            /* no such symbol */
            printf("kiflash_test no such symbol\n");
            SetErrorMessage("kiflash_test no such symbol");  
            // Message tosend("error", "kiflash_test no such symbol");
            // writeToNode(progress, tosend);            
        }    

        *(void**)(&func_flashprog) = dlsym(handle, "kiflash_program_user");
        if (!func_flashprog) {
            /* no such symbol */
            printf("kiflash_program_user no such symbol\n");        
            SetErrorMessage("kiflash_program_user no such symbol");  
        }  

        func_test();
        func_flashtest();                
/*        if (options->IsObject() ) {
          v8::Local<v8::Value> n_ = options->Get(New<v8::String>("n").ToLocalChecked());
          if ( n_->IsNumber() ) {
            N = n_->NumberValue();
          }
        }
*/        
/*
        if ( N < 0 ) {
          SetErrorMessage("Cannot compute prime factorization of negative numbers (overflowed long long?)!");
        }
*/        
    }
     
    void send_factor(const AsyncProgressWorker::ExecutionProgress& progress, long long factor) {
        Message tosend("factor", std::to_string(factor));
        writeToNode(progress, tosend);
    }

    void Execute (const AsyncProgressWorker::ExecutionProgress& progress) {
      int n = N;
      
      send_factor(progress, 2);
      func_flashprog();

      send_factor(progress, 100);
    }
  private:
    int N;
};

// Important:  You MUST include this function, and you cannot alter
//             the signature at all.  The base wrapper class calls this
//             to build your particular worker.  The prototype for this
//             function is defined in addon-streams.h
StreamingWorker * create_worker(Callback *data
    , Callback *complete
    , Callback *error_callback, v8::Local<v8::Object> & options) {
 return new Kiflash(data, complete, error_callback, options);
}

// Don't forget this!  You can change the name of your module, 
// but the second parameter should always be as shown.
NODE_MODULE(kiflash, StreamWorkerWrapper::Init)
