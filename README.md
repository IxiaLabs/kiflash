# Mmap for Node.js
mmap(2) / madvise(2) / msync(2) / mincore(2) / mwrite(2) for node.js revisited.

Pulled the mmap-io code from https://www.npmjs.com/package/mmap-io. This version of mmap-io allowed it to work with node js version that is later than version 8.
There are some minor tweak to the mmap-io.

Comment from mmap-io page, "It should be noted that mem-mapping is by nature potentially blocking, and _should not be used in concurrent serving/processing applications_, but rather has it's niche where multiple processes are working on the same giant sets of data (thereby sparing physical memory, and load times if the kernel does it's job for read ahead), preferably multiple readers and single or none concurrent writer, to not spoil the gains by shitloads of spin-locks, mutexes or such. And your noble specific use case of course."

# News and Updates

### 2019-04-29: version 0.1.0
- Initial version of ki_mmap created


# Install
Use npm as this is local private npm package

```
sudo git clone https://github.com/IxiaLabs/ki_mmap.git    
cd ki_mmap
sudo npm install -g yarn
sudo npm install -g bindings
sudo npm install -g livescript
sudo npm install -g nan
sudo npm install -g graceful-fs
sudo apt install node-gyp

sudo npm run-script build
```



# Usage

```typescript
under the src directory there is the testpcie.ts

const mmap = require('../node_modules/mmap-io/mmap-io');
const fs = require("fs");
const filename = "/sys/devices/pci0000\:00/0000\:00\:01.0/0000\:01\:00.0/resource1";
var fd = fs.openSync(filename, "rs+");

console.log(`$filename opened`);

async function main() {
    // Create an instance of the FloLicenseProvider and get licenses inventory.
    //const options = { path: "27009@2ua7343281.lbj.is.keysight.com" };
    //let floLicenseProvider = new FloLicenseProvider(options);

    const map_size = 1024 * 1024 * 16;    

    try {
        console.log(`Entered try `);
        // Get license inventory
        //let licenseConfigs = await floLicenseProvider.getAvailableFeatureList();
        //for (const licConfig of licenseConfigs) {
        //    console.log(JSON.stringify(licConfig));
        //}

        buffer = mmap.map(
            map_size,
            mmap.PROT_WRITE | mmap.PROT_READ,
            mmap.MAP_SHARED,
            fd,
            0,
            mmap.MADV_SEQUENTIAL
        )

        console.log(buffer[0]);
        console.log(buffer[1]);
        console.log(buffer[2]);
        console.log(buffer[3]);
        console.log(buffer[4]);
        console.log(buffer[5]);
        console.log(buffer[6]);
        console.log(buffer[7]);
        //mmap.munmap(buffer, map_size);
    } catch (err) {
        console.log(err);
        console.trace();
        // More error handling logic
    }

    
}

main();
```

### Good to Know (TM)

```the following are taken from https://www.npmjs.com/package/mmap-io
- Checkout man pages mmap(2), madvise(2), msync(2), mincore(2), mwrite(2) for more detailed intell.
- The mappings are automatically unmapped when the buffer is garbage collected.
- Write-mappings need the fd to be opened with "r+", or you'll get a permission error (13).
- If you make a read-only mapping and then ignorantly set a value in the buffer, all hell previously unknown to a JS'er breaks loose (segmentation fault). It is possible to write some devilous code to intercept the SIGSEGV and throw an exception, but let's not do that!
- `Offset`, and in some cases `length` needs to be a multiple of mmap-io.PAGESIZE (which commonly is 4096)
- Huge pages are only supported for anonymous / private mappings (well, in Linux), so I didn't throw in flags for that since I found no use.
- As Ben Noordhuis previously has stated: Supplying hint for a fixed virtual memory adress is kinda moot point in JS, so not supported.
- If you miss a feature - contribute! Or request it in an issue.
- If documentation isn't clear, make an issue.

    `

# Tests
```
node ./src/testpcie.ts
```
