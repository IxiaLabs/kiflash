const mmap = require("./kiflash");

const fs = require("fs");
const filename = "/sys/devices/pci0000\:00/0000\:00\:01.0/0000\:01\:00.0/resource1";
var fd = fs.openSync(filename, "rs+");
var buf1;

console.log(filename + " opened");

async function main() {
    // Create an instance of the FloLicenseProvider and get licenses inventory.
    //const options = { path: "27009@2ua7343281.lbj.is.keysight.com" };
    //let floLicenseProvider = new FloLicenseProvider(options);


    const stats = fs.statSync(filename);
    const map_size = stats.size;    
    // buf1 = Buffer.alloc(map_size);    
    let buf_addr = 0;    
    let resultlist = [0, 0];

    try {
        console.log(`Entered try `);
        // Get license inventory
        //let licenseConfigs = await floLicenseProvider.getAvailableFeatureList();
        //for (const licConfig of licenseConfigs) {
        //    console.log(JSON.stringify(licConfig));
        //}

        buf_addr = mmap.map(
            map_size,
            mmap.PROT_WRITE | mmap.PROT_READ,
            mmap.MAP_SHARED,
            fd,
            0
        )

        console.log('virtual mapped address is: 0x' + buf_addr.toString(16));
        
        const stringdata = new String('123456789ABCDEF01');
        const valbuf = Buffer.from(stringdata.toString(), 'hex');
        valbuf.swap64();


        // mmap.writebuffer(0x722018, valbuf);
        let addr_to_try = 0x01C10000 + 4096;
        mmap.write64(addr_to_try, 0x89ABCDEF, 0x12345670);
        
        resultlist = mmap.read64(addr_to_try);
        
        console.log('Wrote to: 0x' + addr_to_try.toString(16) + ' with value: ' + ' hi ' + resultlist[1].toString(16) + ' lo ' + resultlist[0].toString(16));

        console.log('programuser entered\n');
        
        let returnValue = 1;
        returnValue = await mmap.programuser();
        console.log('programuser ended with returnvalue %d\n', returnValue);
        // buf1[0x722018] = 1;
        // console.log(buf1[0]);
        // console.log(buf1[1]);
        // console.log(buf1[2]);
        // console.log(buf1[3]);
        // console.log(buf1[4]);
        // console.log(buf1[5]);
        // console.log(buf1[6]);
        // console.log(buf1[7]);
        //mmap.munmap(buffer, map_size);
    } catch (err) {
        console.log(err);
        console.trace();
        // More error handling logic
    }

    
}

main();
