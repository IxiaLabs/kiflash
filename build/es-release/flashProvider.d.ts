export interface IFlashProviderOptions {
    app?: string;
    path?: string;
}
export declare class FlashProvider {
    private provider;
    private path;
    /**
     * Creates an instance of a FloLicenseProvider object.
     * @constructor
     * @param {IFloLicenseProviderOptions} options - specifies the app name or server path that serves licences.
     */
    /**
     * Returns the inventory of licenses.
     * @returns {Promise<ILicenseConfig[]>} Promise object that represents an array of the license inventory.
     */
    /**
     * Checks out the specified license.
     * @param {string} feature - name of the feature to check out.
     * @param {string} version - verison of the feature to check out.
     * @param {number} totalNumberOfLicenses - the number of licenses requested.
     * @returns {Promise<ILicenseConfig[]>} Promise object that represents an array of checked out licenses if method succeeds or an error.
     */
    /**
     * Checks in the specified license.
     * @param {string} feature - name of the feature to check in.
     * @returns {Promise<boolean>} Promise object that represents the success status or an error.
     */
    /**
     * Checks the server status of the server specified when you construct this object.
     * @returns {Promise<boolean>} Promise object that represents the server status or an error.
     */
    /**
     * Sets the license path.
     * @param {string} path - the license path.
     * @returns {Promise<boolean>} Promise object that represents the success status or an error.
     */
    /**
     * Gets the license path.
     * @returns {string} The license path.
     */
    getLicenseSearchPath(): string;
}
