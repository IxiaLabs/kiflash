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
            Message tosend("error", "Fail to load the libary");
            writeToNode(progress, tosend);            
        }

        *(void**)(&func_test) = dlsym(handle, "pcimem_test");
        if (!func_test) {
            /* no such symbol */
            printf("pcimem_test no such symbol\n");        
            Message tosend("error", "pcimem_test no such symbol");
            writeToNode(progress, tosend);            
        }    

        *(void**)(&func_flashtest) = dlsym(handle, "kiflash_test");
        if (!func_flashtest) {
            /* no such symbol */
            printf("kiflash_test no such symbol\n");        
            Message tosend("error", "kiflash_test no such symbol");
            writeToNode(progress, tosend);            
        }    

        *(void**)(&func_flashprog) = dlsym(handle, "kiflash_program_user");
        if (!func_flashprog) {
            /* no such symbol */
            printf("kiflash_test no such symbol\n");        
            return Nan::ThrowError("no such symbol kiflash_program_user");
        }  

        func_test();
        func_flashtest();        
        func_flashprog();
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
      //long long n = N;
      long long n = 100000000000000;
      while (n%2 == 0)
      {
        send_factor(progress, 2);
        n = n/2;
      }
 
      for (long long i = 3; i <= n; i = i+2) {
          // while (n%i == 0) {
            send_factor(progress, i);
    //            n = n/i;
          //}
      }
    }
  private:
    long long N;
};

// Important:  You MUST include this function, and you cannot alter
//             the signature at all.  The base wrapper class calls this
//             to build your particular worker.  The prototype for this
//             function is defined in addon-streams.h
StreamingWorker * create_worker(Callback *data
    , Callback *complete
    , Callback *error_callback, v8::Local<v8::Object> & options) {
 return new Factorization(data, complete, error_callback, options);
}

// Don't forget this!  You can change the name of your module, 
// but the second parameter should always be as shown.
NODE_MODULE(factorization, StreamWorkerWrapper::Init)
