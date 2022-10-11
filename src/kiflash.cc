#include <iostream>
#include <chrono>
#include <thread>
#include <dlfcn.h>
#include "streaming-worker.h"
#include <future>  
const int version = 0x01;
using namespace std;
void *handle = NULL;
void (*func_test)();
void (*func_flashtest)();
int (*func_flashprog)();
int (*func_flashprogress)();

static void icap(const v8::FunctionCallbackInfo<v8::Value>& args);

class Kiflash : public StreamingWorker {
  public:
    static void Init(v8::Local<v8::Object> exports) {
        StreamWorkerWrapper::Init(exports);
        NODE_SET_METHOD(exports, "icap", icap);
    }

    Kiflash(Callback *data, Callback *complete, Callback *error_callback,  v8::Local<v8::Object> & options) 
          : StreamingWorker(data, complete, error_callback){
        printf("Kiflash entered with version %d\n", version);
        N = -1;        
        handle = dlopen("/opt/ixia/mmap/libFlashProvider.so", RTLD_LAZY);

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

        *(void**)(&func_flashprogress) = dlsym(handle, "kiflash_getprogress");
        if (!func_flashprogress) {
            /* no such symbol */
            printf("kiflash_getprogress no such symbol\n");        
            SetErrorMessage("kiflash_getprogress no such symbol");  
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
      // int n = N;
      
      N = 0;
      send_factor(progress, N);
      std::future<int> fut = std::async(std::launch::async, func_flashprog);            
      // printf("ffffff after func_flashprog\n");             

      // std::async(std::launch::async, func_flashprog);
      // printf("****kiflash Execute after flashprog\n");        

      // while (!closed()) {
      while (N != 100) {
        int n = func_flashprogress();
        if (n != N)
        {
          N = n;
          send_factor(progress, N);
        }
        // std::this_thread::sleep_for(chrono::milliseconds(100));
      }    

      if (N == 100)  
      {
        send_factor(progress, 100);
      }
    
      int ret = fut.get(); 
      // int result = r.get();
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

void icap(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    if (!handle) {
        handle = dlopen("/opt/ixia/mmap/libFlashProvider.so", RTLD_LAZY);
    }
    if (!handle) {
        printf("failed to load libFlashProvider.so\n");
        fflush(stdout);
        return;
    }
    int (*f_icap)(uint32_t die);
    *(void**)(&f_icap) = dlsym(handle, "kiflash_icap");
    if (!f_icap) {
        printf("cannot find icap in libFlashProvider.so\n");
        fflush(stdout);
        return;
    }
    f_icap(0);
}

// Don't forget this!  You can change the name of your module, 
// but the second parameter should always be StreamWorkerWrapper::Init
// or call StreamWorkerWrapper::Init.
NODE_MODULE(kiflash, Kiflash::Init)
