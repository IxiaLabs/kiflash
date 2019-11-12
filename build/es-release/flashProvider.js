"use strict";
/*
 * Copyright Keysight Technologies, All Rights Reserved
 * Keysight Confidential
 */
Object.defineProperty(exports, "__esModule", { value: true });
// Pulls in native extension
const flashProvNativeExt = require('./native');
class FlashProvider {
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
        }
        */
    /**
     * Creates an instance of a FloLicenseProvider object.
     * @constructor
     * @param {IFloLicenseProviderOptions} options - specifies the app name or server path that serves licences.
     */
    /*    constructor(options: IFloLicenseProviderOptions) {
            if (options.app == undefined &&
                options.path == undefined) {
                    throw new Error("Invalid options specified");
            }
    
            this.path = options.path;
    
            try {
                const library = FloLicenseProvider.findProviderNativeLibrary();
                this.provider = new floLicProvNativeExt.FloLicenseProviderWrapper(options, library);
            }
            catch (err) {
                throw new Error(err);
            }
        }*/
    /**
     * Returns the inventory of licenses.
     * @returns {Promise<ILicenseConfig[]>} Promise object that represents an array of the license inventory.
     */
    /*    public getAvailableFeatureList(): Promise<ILicenseConfig[]> {
            return new Promise<ILicenseConfig[]>((resolve: Function, reject: Function) => {
                this.provider.getAvailableFeatureList((error: Error, licenseConfigs: ILicenseConfig[]) => {
                    if (error) {
                        reject(error);
                    } else {
                        resolve(licenseConfigs);
                    }
                });
            });
        }*/
    /**
     * Checks out the specified license.
     * @param {string} feature - name of the feature to check out.
     * @param {string} version - verison of the feature to check out.
     * @param {number} totalNumberOfLicenses - the number of licenses requested.
     * @returns {Promise<ILicenseConfig[]>} Promise object that represents an array of checked out licenses if method succeeds or an error.
     */
    /*    public checkOut(feature: string, version: string, totalNumberOfLicenses: number): Promise<ILicenseConfig> {
            return new Promise<ILicenseConfig>((resolve: Function, reject: Function) => {
                this.provider.checkOut(feature, version, totalNumberOfLicenses, (error: Error, lc: ILicenseConfig) => {
                    if (error) {
                        reject(error);
                    } else {
                        resolve(lc);
                    }
                });
            });
        }*/
    /**
     * Checks in the specified license.
     * @param {string} feature - name of the feature to check in.
     * @returns {Promise<boolean>} Promise object that represents the success status or an error.
     */
    /*    public checkIn(feature: string): Promise<boolean> {
            return new Promise<boolean>((resolve: Function, reject: Function) => {
                this.provider.checkIn(feature, (error: Error, result: boolean) => {
                    if (error) {
                        reject(error);
                    } else {
                        resolve(result);
                    }
                });
            });
        }*/
    /**
     * Checks the server status of the server specified when you construct this object.
     * @returns {Promise<boolean>} Promise object that represents the server status or an error.
     */
    /*    public getLicenseServerStatus(): Promise<boolean> {
            return new Promise<boolean>((resolve: Function, reject: Function) => {
                this.provider.getLicenseServerStatus((error: Error, status: boolean) => {
                    if (error) {
                        reject(error);
                    } else {
                        resolve(status);
                    }
                });
            });
        }*/
    /**
     * Sets the license path.
     * @param {string} path - the license path.
     * @returns {Promise<boolean>} Promise object that represents the success status or an error.
     */
    /*    public setLicenseSearchPath(path: string): Promise<boolean> {
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
        }*/
    /**
     * Gets the license path.
     * @returns {string} The license path.
     */
    getLicenseSearchPath() {
        return this.path;
    }
}
exports.FlashProvider = FlashProvider;
