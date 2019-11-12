/**
 * Interface that represents a license.
 */
export interface ILicenseConfig {
    /**
     * Annotation data for the license.
     * @type {string}
     */
    annotation: string;
    /**
     * The feature name.
     * @type {string}
     */
    feature: string;
    /**
     * The version of the license.
     * @type {string}
     */
    version: string;
    /**
     * The version of the license as a number.
     * @type {number}
     */
    versionAsDouble: number;
    /**
     * The name of the server.
     * @type {string}
     */
    serverName: string;
    /**
     * The date the license expires.
     * @type {Date}
     */
    expirationDate: Date;
    /**
     * Indicates whether or not the license is counted.
     * @type {boolean}
     */
    isCounted: boolean;
    /**
     * The total number of licenses.
     * @type {number}
     */
    totalLicenseCount: number;
}
