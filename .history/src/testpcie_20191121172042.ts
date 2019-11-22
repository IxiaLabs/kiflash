"use strict"; 

const worker = require("streaming-worker");
const path = require("path");

// var addon_path = path.join(__dirname, "/home/ixiaadmin/git/kiflash/build/Release/kiflash");
var addon_path = "/home/ixiaadmin/git/kiflash/build/Release/kiflash";
console.log(addon_path);



// const mmap = require("./kiflash");



async function localprog() {
    const kiflash = worker(addon_path);

    var factor: string  = '';
    let e: string  = '';
    kiflash.from.on('factor', function(factor){
        console.log("Factor:  " + factor);
    });
    
    kiflash.from.on('error', function(e) {
        console.log(e);
    });
        
    // let returnValue = await mmap.programuser();
    return new Promise<number>(resolve => {resolve(0)});    
}
async function printNumber1() {
    return new Promise((resolve) => {
       setTimeout(() => {
       console.log("----- Number1 is done\n");
       resolve(10);
       },1000);
    });
 }

async function printNumber2() {
    return new Promise((resolve,) => {
       setTimeout(() => {
       console.log("----- Number2 is done\n");
       resolve(20);
       },500);
    });
 }
 
async function main() {
    // Create an instance of the FloLicenseProvider and get licenses inventory.
    //const options = { path: "27009@2ua7343281.lbj.is.keysight.com" };
    //let floLicenseProvider = new FloLicenseProvider(options);


   //  const stats = fs.statSync(filename);
    //const map_size = stats.size;    
    // buf1 = Buffer.alloc(map_size);    
   // let buf_addr = 0;    
    // let resultlist = [0, 0];

    try {
        console.log(`Entered try `);
        // Get license inventory
        //let licenseConfigs = await floLicenseProvider.getAvailableFeatureList();
        //for (const licConfig of licenseConfigs) {
        //    console.log(JSON.stringify(licConfig));
        //}
        
        const number1 = await localprog();


        
        
        // let returnValue = 1;
        // returnValue = await mmap.programuser();

        
        // const returnValue = await mmap.programuser();

/*        let increment = 0;
        while(returnValue != 0)
        {
            console.log("%d\r", increment++);
        }        
*/        
        console.log('** programuser ended with returnvalue %d\n', number1);
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
