{
    "targets": [{
        "target_name": "ki_mmap",
        "sources": [ "src/mmap-io.cc" ],
        "include_dirs": [
            "<!(node -e \"require('nan')\")",
            "kiflashcore/src",
        ],
        "libraries": [
            "-lpcimem", "-L/home/ixiaadmin/git/kiflash/src"
        ],      
        "cflags_cc": [
            "-std=c++11"
        ],
        "conditions": [
            [ 'OS=="mac"',
                { "xcode_settings": {
                    'OTHER_CPLUSPLUSFLAGS' : ['-std=c++11','-stdlib=libc++'],
                    'OTHER_LDFLAGS': ['-stdlib=libc++'],
                    'MACOSX_DEPLOYMENT_TARGET': '10.8'
                }}
            ]
        ]
    }]        
}
