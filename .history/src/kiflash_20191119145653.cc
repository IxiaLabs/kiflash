/*
    Licensed under The MIT License (MIT)
    You will find the full license legal mumbo jumbo in file "LICENSE"

    Copyright (c) 2015 - 2018 Oscar Campbell

    Inspired by Ben Noordhuis module node-mmap - which does the same thing for older node
    versions, sans advise and sync.
*/
#include <nan.h>
#include <errno.h>
#include <string>
#include <algorithm>    // std::min
#include <dlfcn.h>

#ifdef _WIN32
#include <windows.h>
#include "mman.h"
#include "pcimem.h"
#else
#include <unistd.h>
#include <sys/mman.h>
#endif

/* #include <future> */

using namespace v8;


static const unsigned int kOffsetValueListEntries	= 0x00200000;
static const unsigned int kMaxNoOfRangeListEntries	= 1024;



// Just a bit more clear as to intent
#define JS_FN(a) NAN_METHOD(a)

// This lib is one of those pieces of code where clarity is better then puny micro opts (in
// comparison to the massive blocking that will occur when the data is first read from disk)
// Since casting `size` to `void*` feels a little "out there" considering that void* may be
// 32b or 64b (or, I dunno, 47b on some quant particle system), we throw this struct in.
struct MMap {
    MMap(char* data, size_t size) : data(data), size(size) {}
    char*   data = nullptr;
    size_t  size = 0;
};

static void* map_virtual_addr;


void do_mmap_cleanup(char* data, void* hint) {
    auto map_info = static_cast<MMap*>(hint);
    munmap(data, map_info->size);
    delete map_info;
}

inline int do_mmap_advice(void* addr, size_t length, int advise) {
    return madvise(addr, length, advise);
}

JS_FN(mmap_map) {
    Nan::HandleScope();
        
    if (info.Length() < 4 && info.Length() > 6) {
        return Nan::ThrowError(
            "map() takes 4, 5 or 6 arguments: (size :int, protection :int, flags :int, fd :int [, offset :int [, advise :int]])."
        );
    }

    // Try to be a little (motherly) helpful to us poor clueless developers
    if (!info[0]->IsNumber())    return Nan::ThrowError("mmap: size (arg[0]) must be an integer");
    if (!info[1]->IsNumber())    return Nan::ThrowError("mmap: protection_flags (arg[1]) must be an integer");
    if (!info[2]->IsNumber())    return Nan::ThrowError("mmap: flags (arg[2]) must be an integer");
    if (!info[3]->IsNumber())    return Nan::ThrowError("mmap: fd (arg[3]) must be an integer (a file descriptor)");
    // Offset and advise are optional

    constexpr void* hinted_address  = nullptr;  // Just making things uber-clear...
    const size_t    size            = Nan::To<int>(info[0]).FromJust(); //ToInteger()->Value();   // ToUint64()->Value();
    const int       protection      = info[1]->IntegerValue();
    const int       flags           = info[2]->ToInteger()->Value();
    const int       fd              = info[3]->ToInteger()->Value();
    const size_t    offset          = info[4]->ToInteger()->Value();   // ToInt64()->Value();
    const int       advise          = info[5]->ToInteger()->Value();

    void *handle;
    void (*func_test)();
    void (*func_flashtest)();
    void (*func_flashprog)();
    handle = dlopen("/home/ixiaadmin/git/kiflash/bin/Linux/libFlashProvider.so", RTLD_LAZY);

    if (!handle) {
        /* fail to load the library */
        printf("Fail to load the libary\n");
        return Nan::ThrowError("Fail to load the library");

    }

    *(void**)(&func_test) = dlsym(handle, "pcimem_test");
    if (!func_test) {
        /* no such symbol */
        printf("pcimem_test no such symbol\n");        
        return Nan::ThrowError("no such symbol pcimem_test");
    }    

    *(void**)(&func_flashtest) = dlsym(handle, "kiflash_test");
    if (!func_flashtest) {
        /* no such symbol */
        printf("kiflash_test no such symbol\n");        
        return Nan::ThrowError("no such symbol kiflash_test");
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
    // char* data = static_cast<char*>( mmap( hinted_address, size, protection, flags, fd, offset) );
    void *map_base = mmap( hinted_address, size, protection, flags, fd, offset);

    if (map_base == (void *) -1) {
        return Nan::ThrowError((std::string("mmap failed, ") + std::to_string(errno)).c_str());
    }
    else {
        if (advise != 0) {
            auto ret = do_mmap_advice(map_base, size, advise);
            if (ret) {
                return Nan::ThrowError((std::string("madvise() failed, ") + std::to_string(errno)).c_str());
            }
        }
        
        map_virtual_addr = map_base;
        double adddress_of_map = (unsigned long)map_virtual_addr;
        info.GetReturnValue().Set(adddress_of_map);
    }
   
}

JS_FN(mmap_advise) {
    Nan::HandleScope();

    if (info.Length() != 2 && info.Length() != 4) {
        return Nan::ThrowError(
            "advise() takes 2 or 4 arguments: (buffer :Buffer, advise :int) | (buffer :Buffer, offset :int, length :int, advise :int)."
        );
    }
    if (!info[0]->IsObject())    return Nan::ThrowError("advice(): buffer (arg[0]) must be a Buffer");
    if (!info[1]->IsNumber())    return Nan::ThrowError("advice(): (arg[1]) must be an integer");

    Local<Object>   buf     = info[0]->ToObject();
    char*           data    = node::Buffer::Data(buf);
    size_t          size    = node::Buffer::Length(buf);
    int ret;

    if (info.Length() == 2) {
        int advise = info[1]->ToInteger()->Value();
        ret = do_mmap_advice(data, size, advise);
    }
    else {
        int offset = info[1]->ToInteger()->Value();
        int length = info[2]->ToInteger()->Value();
        int advise = info[3]->ToInteger()->Value();
        ret = do_mmap_advice(data + offset, length, advise);
    }
    if (ret) {
        return Nan::ThrowError((std::string("madvise() failed, ") + std::to_string(errno)).c_str());
    }

    //Nan::ReturnUndefined();
}

JS_FN(mmap_incore) {
    Nan::HandleScope();

    if (info.Length() != 1) {
        return Nan::ThrowError(
            "incore() takes 1 argument: (buffer :Buffer) ."
        );
    }

    if (!info[0]->IsObject())    return Nan::ThrowError("advice(): buffer (arg[0]) must be a Buffer");

    Local<Object>   buf     = info[0]->ToObject();
    char*           data    = node::Buffer::Data(buf);
    size_t          size    = node::Buffer::Length(buf);

#ifdef _WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    size_t          page_size = sysinfo.dwPageSize;
#else
    size_t          page_size = sysconf(_SC_PAGESIZE);
#endif

    size_t          needed_bytes = (size+page_size-1) / page_size;
    size_t          pages = size / page_size;

#ifdef __APPLE__
    char*  resultData = (char *)malloc(needed_bytes);
#else
    unsigned char*  resultData = (unsigned char *)malloc(needed_bytes);
#endif

    if (size % page_size > 0) {
        pages++;
    }

    int ret = mincore(data, size, resultData);

    if (ret) {
        free(resultData);
        if (errno == ENOSYS) {
            return Nan::ThrowError("mincore() not implemented");
        } else {
            return Nan::ThrowError((std::string("mincore() failed, ") + std::to_string(errno)).c_str());
        }
    }

    // Now we want to check all of the pages
    uint32_t pages_mapped = 0;
    uint32_t pages_unmapped = 0;

    for(size_t i = 0; i < pages; i++) {
        if(!(resultData[i] & 0x1)) {
            pages_unmapped++;
        } else {
            pages_mapped++;
        }
    }

    free(resultData);

    v8::Local<v8::Array> arr = Nan::New<v8::Array>(2);
    Nan::Set(arr, 0, Nan::New(pages_unmapped));
    Nan::Set(arr, 1, Nan::New(pages_mapped));
    info.GetReturnValue().Set(arr);
}

static uint64_t fromBuffer(char* buffdata, int offset=0, bool littleEndian=true)
{
    int low = 0;
    int high = 0;
    int bufflength = strlen(buffdata);
    int end = std::min(bufflength, offset+4);
    
    if(littleEndian)
    {
        int i = 0;
        while(offset+i < end)
        {
            low |= buffdata[offset+i] << (i*8);
            ++i;
        }
        end = std::min(bufflength, offset+8);
        int start = offset+4;
        i = 0;
        while(start+i < end)
        {
            high |= buffdata[start+i] << (i*8);
            ++i;
        }
    }
    else
    {
        // for big endian, it has to be multiples of 8 bytes.
        if(bufflength >= offset+8) 
        {
            for(int i = 0; i <= 3; i++)
            {
                int n = (3-i)*8;
                high |= buffdata[offset+i]   << n;
                low  |= buffdata[offset+4+i] << n;
            }
        }
    }
    uint64_t dataToWrite  = (uint64_t)(high << 32) | (uint64_t)(low);

    return dataToWrite;
}

JS_FN(mmap_writebuffer) {
    Nan::HandleScope();

    if (info.Length() != 2) {
        return Nan::ThrowError(
            "write() takes 2 arguments: (offset :int, value :Buffer)."
        );
    }
    if (!info[0]->IsNumber())    return Nan::ThrowError("advice(): (arg[0]) must be a integer");
    if (!info[1]->IsObject())    return Nan::ThrowError("advice(): (arg[2]) must be a Buffer");

    int offset = info[0]->ToInteger()->Value();
    Local<Object>   valbuf     = info[1]->ToObject();
    char*           valdata    = node::Buffer::Data(valbuf);
    size_t          valsize    = node::Buffer::Length(valbuf);        

    if (map_virtual_addr == (void *) -1) {
        return Nan::ThrowError((std::string("mmap write failed, ") + std::to_string(errno)).c_str());
    }
    else {
        uint numberOfUInt64 = valsize / 8;
        uint64_t virtualaddr = (unsigned long)map_virtual_addr;
        for (int i = 0; i < numberOfUInt64; i++ ) {
            uint64_t virt_addr = virtualaddr + offset + i*8;
            int start = i * 8;
            *((uint64_t *) virt_addr) = fromBuffer(valdata, start);
        }
    }
    
    //Nan::ReturnUndefined();
}

JS_FN(mmap_write64) {
    Nan::HandleScope();

    if (info.Length() != 3) {
        return Nan::ThrowError(
            "write64() takes 3 arguments: (offset :int, hi_value :int, lo_value :int)."
        );
    }
    if (!info[0]->IsNumber())    return Nan::ThrowError("write64(): (arg[0]) must be a integer");
    if (!info[1]->IsNumber())    return Nan::ThrowError("write64(): (arg[1]) must be a integer");
    if (!info[2]->IsNumber())    return Nan::ThrowError("write64(): (arg[2]) must be a integer");

    int offset = info[0]->ToInteger()->Value();
    int hi_value = info[1]->ToInteger()->Value();
    int lo_value = info[2]->ToInteger()->Value();
    uint64_t write_value = ((uint64_t)hi_value << 32) | (uint64_t)lo_value;    

    if (map_virtual_addr == (void *) -1) {
        return Nan::ThrowError((std::string("mmap write64 failed, ") + std::to_string(errno)).c_str());
    }
    else {                
        void* virt_addr = map_virtual_addr + offset;
        *((uint64_t *) virt_addr) = write_value;
    }        
}

JS_FN(mmap_read64) {
    Nan::HandleScope();

    if (info.Length() != 1) {
        return Nan::ThrowError(
            "read64() takes 1 arguments: (offset :int)."
        );
    }
    if (!info[0]->IsNumber())    return Nan::ThrowError("read64(): (arg[0]) must be a integer");     

    int offset = info[0]->ToInteger()->Value();    
    uint64_t read_result = 0xDEADBEEFDEADBEEF;
    if (map_virtual_addr == (void *) -1) {
        return Nan::ThrowError((std::string("mmap read64 failed, ") + std::to_string(errno)).c_str());
    }
    else {        
        void* virt_addr = map_virtual_addr + offset;
        read_result = *((uint64_t *) virt_addr);                
    }
        
    uint32_t lo_result = (uint32_t)(read_result);
    uint32_t hi_result = (uint32_t)(read_result >> 32);
    v8::Local<v8::Array> arr = Nan::New<v8::Array>(2);    
    Nan::Set(arr, 0, Nan::New(lo_result));
    Nan::Set(arr, 1, Nan::New(hi_result));
    info.GetReturnValue().Set(arr);    
}


JS_FN(mmap_sync_lib_private_) {
    Nan::HandleScope();

    // I barfed at the thought of implementing all variants of info-combos in C++, so
    // the arg-shuffling and checking is done in a ES wrapper function - see "mmap-io.ts"
    if (info.Length() != 5) {
        return Nan::ThrowError(
            "sync() takes 5 arguments: (buffer :Buffer, offset :int, length :int, do_blocking_sync :bool, invalidate_pages_and_signal_refresh_to_consumers :bool)."
        );
    }

    if (!info[0]->IsObject())    return Nan::ThrowError("sync(): buffer (arg[0]) must be a Buffer");

    Local<Object>   buf             = info[0]->ToObject();
    char*           data            = node::Buffer::Data(buf);

    int             offset          = info[1]->ToInteger()->Value();
    size_t          length          = info[2]->ToInteger()->Value();
    bool            blocking_sync   = info[3]->ToBoolean()->Value();
    bool            invalidate      = info[4]->ToBoolean()->Value();
    int             flags           = ( (blocking_sync ? MS_SYNC : MS_ASYNC) | (invalidate ? MS_INVALIDATE : 0) );

    int ret = msync(data + offset, length, flags);

    if (ret) {
        return Nan::ThrowError((std::string("msync() failed, ") + std::to_string(errno)).c_str());
    }
    //Nan::ReturnUndefined();
}


NAN_MODULE_INIT(Init) {
    auto exports = target;

    constexpr auto property_attrs = static_cast<PropertyAttribute>(
        ReadOnly | DontDelete
    );

    using JsFnType = decltype(mmap_map);

    auto set_int_prop = [&](const char* key, int val) -> void {
        Nan::DefineOwnProperty(
            exports,
            Nan::New(key).ToLocalChecked(),
            Nan::New(val),
            property_attrs
        );
    };

    auto set_fn_prop = [&](const char* key, JsFnType fn) -> void {
        Nan::DefineOwnProperty(
            exports,
            Nan::New(key).ToLocalChecked(),
            Nan::New<FunctionTemplate>(fn)->GetFunction(),
            property_attrs
        );
    };

    set_int_prop("PROT_READ", PROT_READ);
    set_int_prop("PROT_WRITE", PROT_WRITE);
    set_int_prop("PROT_EXEC", PROT_EXEC);
    set_int_prop("PROT_NONE", PROT_NONE);

    set_int_prop("MAP_SHARED", MAP_SHARED);
    set_int_prop("MAP_PRIVATE", MAP_PRIVATE);

#ifdef MAP_NONBLOCK
    set_int_prop("MAP_NONBLOCK", MAP_NONBLOCK);
#endif

#ifdef MAP_POPULATE
    set_int_prop("MAP_POPULATE", MAP_POPULATE);
#endif

    set_int_prop("MADV_NORMAL", MADV_NORMAL);
    set_int_prop("MADV_RANDOM", MADV_RANDOM);
    set_int_prop("MADV_SEQUENTIAL", MADV_SEQUENTIAL);
    set_int_prop("MADV_WILLNEED", MADV_WILLNEED);
    set_int_prop("MADV_DONTNEED", MADV_DONTNEED);

    //set_int_prop("MS_ASYNC", MS_ASYNC);
    //set_int_prop("MS_SYNC", MS_SYNC);
    //set_int_prop("MS_INVALIDATE", MS_INVALIDATE);

#ifdef _WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    set_int_prop("PAGESIZE", sysinfo.dwPageSize);
#else
    set_int_prop("PAGESIZE", sysconf(_SC_PAGESIZE));
#endif


    set_fn_prop("map", mmap_map);
    set_fn_prop("advise", mmap_advise);
    set_fn_prop("incore", mmap_incore);
    set_fn_prop("writebuffer", mmap_writebuffer);
    set_fn_prop("write64", mmap_write64);
    set_fn_prop("read64", mmap_read64);
    set_fn_prop("programuser", mmap_programuser);

    // This one is wrapped by a JS-function and deleted from obj to hide from user
    Nan::DefineOwnProperty(
        exports,
        Nan::New("sync_lib_private__").ToLocalChecked(),
        Nan::New<FunctionTemplate>(mmap_sync_lib_private_)->GetFunction(),
        static_cast<PropertyAttribute>(0)
    );


}

NODE_MODULE(mmap_io, Init)
