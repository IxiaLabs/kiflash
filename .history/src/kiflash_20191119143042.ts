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

    programuser(): Promise<int>; 

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

export class KiFlash {

    private provider: any;
    private path: string;

/*    private static findProviderNativeLibrary(): string {
        let location = path.join(__dirname, '..', 'build', 'Release');
        if (fs.existsSync(location)) {
            return location;
        }

        location = path.join(__dirname, '..', '..', 'build', 'Release');

        if (fs.existsSync(location)) {
            return location;
        }

        throw new Error("Can't find provider library!");
    }*/

    /**
     * Creates an instance of a FloLicenseProvider object.
     * @constructor
     * @param {IFloLicenseProviderOptions} options - specifies the app name or server path that serves licences.
     */
    constructor() {        
        try {
            this.provider = mmap;
//            const library = FloLicenseProvider.findProviderNativeLibrary();
//            this.provider = new floLicProvNativeExt.FloLicenseProviderWrapper(options, library);
        }
        catch (err) {
            throw new Error(err);
        }
    }

    /**
     * Returns the inventory of licenses.
     * @returns {Promise<ILicenseConfig[]>} Promise object that represents an array of the license inventory.
     */
    public programuser(): Promise<int> {
        return new Promise<int>((resolve: Function, reject: Function) => {
            this.provider.getAvailableFeatureList((error: Error, licenseConfigs: ILicenseConfig[]) => {
                if (error) {
                    reject(error);
                } else {
                    resolve(licenseConfigs);
                }
            });
        });
    }

    /**
     * Checks out the specified license.
     * @param {string} feature - name of the feature to check out.
     * @param {string} version - verison of the feature to check out.
     * @param {number} totalNumberOfLicenses - the number of licenses requested.
     * @returns {Promise<ILicenseConfig[]>} Promise object that represents an array of checked out licenses if method succeeds or an error.
     */
    public checkOut(feature: string, version: string, totalNumberOfLicenses: number): Promise<ILicenseConfig> {
        return new Promise<ILicenseConfig>((resolve: Function, reject: Function) => {
            this.provider.checkOut(feature, version, totalNumberOfLicenses, (error: Error, lc: ILicenseConfig) => {
                if (error) {
                    reject(error);
                } else {
                    resolve(lc);
                }
            });
        });
    }

    /**
     * Checks in the specified license.
     * @param {string} feature - name of the feature to check in.
     * @returns {Promise<boolean>} Promise object that represents the success status or an error.
     */
    public checkIn(feature: string): Promise<boolean> {
        return new Promise<boolean>((resolve: Function, reject: Function) => {
            this.provider.checkIn(feature, (error: Error, result: boolean) => {
                if (error) {
                    reject(error);
                } else {
                    resolve(result);
                }
            });
        });
    }

    /**
     * Checks the server status of the server specified when you construct this object.
     * @returns {Promise<boolean>} Promise object that represents the server status or an error.
     */
    public getLicenseServerStatus(): Promise<boolean> {
        return new Promise<boolean>((resolve: Function, reject: Function) => {
            this.provider.getLicenseServerStatus((error: Error, status: boolean) => {
                if (error) {
                    reject(error);
                } else {
                    resolve(status);
                }
            });
        });
    }

    /**
     * Sets the license path.
     * @param {string} path - the license path.
     * @returns {Promise<boolean>} Promise object that represents the success status or an error.
     */
    public setLicenseSearchPath(path: string): Promise<boolean> {
        return new Promise<boolean>((resolve: Function, reject: Function) => {
            this.provider.setLicenseSearchPath(path, (error: Error, status: boolean) => {
                if (error) {
                    reject(error);
                } else {
                    this.path = path;
                    resolve(status);
                }
            });
        });
    }

    /**
     * Gets the license path.
     * @returns {string} The license path.
     */
    public getLicenseSearchPath(): string {
        return this.path;
    }
}