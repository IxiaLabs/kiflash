

const worker = require("streaming-worker");
const path = require("path");

var addon_path = path.join(__dirname, "/home/ixiaadmin/git/kiflash/build/Release/kiflash");

const kiflash = worker(addon_path);

kiflash.from.on('factor', function(factor){
    console.log("Factor:  " + factor);
});

kiflash.from.on('error', function(e) {
    console.log(e);
});

