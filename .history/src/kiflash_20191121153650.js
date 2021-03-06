"use strict"; 

const worker = require("streaming-worker");
const path = require("path");

var addon_path = path.join(__dirname, "../build/Release/kiflash");

const kiflash = worker(addon_path);

kiflash.from.on('factor', function(factor){
    console.log("Factor:  " + factor);
});

kiflash.from.on('error', function(e) {
    console.log(e);
});


/*
const mmap_lib_raw_ = require("../build/Release/kiflash")

type FileDescriptor = number

type MapProtectionFlags = 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | number

// TODO: neat shit!
type MapProtectionFlagsList = Array<
    | MmapIo["PROT_NONE"]
    | MmapIo["PROT_READ"]
    | MmapIo["PROT_WRITE"]
    | MmapIo["PROT_EXEC"]
>

type MapFlags =
    | MmapIo["MAP_PRIVATE"]
    | MmapIo["MAP_SHARED"]
    | MmapIo["MAP_NONBLOCK"]
    | MmapIo["MAP_POPULATE"]
    | number

type MapAdvise =
    | MmapIo["MADV_NORMAL"]
    | MmapIo["MADV_RANDOM"]
    | MmapIo["MADV_SEQUENTIAL"]
    | MmapIo["MADV_WILLNEED"]
    | MmapIo["MADV_DONTNEED"]

type MmapIo = {
    map(
        size: number,
        protection: MapProtectionFlagsList | MapProtectionFlags,
        flags: MapFlags,
        fd: FileDescriptor,
        offset?: number,
        advise?: MapAdvise
    ): number

    advise(
        buffer: Buffer,
        offset: number,
        length: number,
        advise: MapAdvise
    ): void
    advise(buffer: Buffer, advise: MapAdvise): void

    /// Returns tuple of [ unmapped-pages-count, mapped-pages-count ]
    incore(buffer: Buffer): [number, number]

    sync(
        buffer: Buffer,
        offset?: number,
        size?: number,
        blocking_sync?: boolean,
        invalidate_pages?: boolean
    ): void



    sync(
        buffer: Buffer,
        blocking_sync: boolean,
        invalidate_pages?: boolean
    ): void

    writebuffer(
        offset: number,
        value: Buffer
    ): void

    programuser(): Promise<number>

    write64(
        offset: number,
        hi_value: number,
         lo_value: number
    ): void;

    read64(
        offset: number
    ): [number, number];    

    readonly PROT_READ: 1 // & ProtTag
    readonly PROT_WRITE: 2 // & ProtTag
    readonly PROT_EXEC: 4 // & ProtTag
    readonly PROT_NONE: 0 // & ProtTag
    readonly MAP_SHARED: 1
    readonly MAP_PRIVATE: 2
    readonly MAP_NONBLOCK: 65536
    readonly MAP_POPULATE: 32768
    readonly MADV_NORMAL: 0
    readonly MADV_RANDOM: 1
    readonly MADV_SEQUENTIAL: 2
    readonly MADV_WILLNEED: 3
    readonly MADV_DONTNEED: 4

    readonly PAGESIZE: number
}

// snatch the raw C++-sync func
const raw_sync_fn_ = mmap_lib_raw_.sync_lib_private__

// Hide the original C++11 func from users
delete mmap_lib_raw_.sync_lib_private__

// Take care of all the param juggling here instead of in C++ code, by making
// some overloads, and doing some argument defaults /ozra
mmap_lib_raw_.sync = function(
    buf: Buffer,
    par_a?: any,
    par_b?: any,
    par_c?: any,
    par_d?: any
): void {
    if (typeof par_a === "boolean") {
        raw_sync_fn_(buf, 0, buf.length, par_a, par_b || false)
    } else {
        raw_sync_fn_(
            buf,
            par_a || 0,
            par_b || buf.length,
            par_c || false,
            par_d || false
        )
    }
}

// mmap_lib_raw_.sync = sync_

const mmap = mmap_lib_raw_ as MmapIo
module.exports = mmap
export default mmap
*/
