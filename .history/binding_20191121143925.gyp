{
    "targets": [{
        "target_name": "kiflash",
        "sources": [ "src/kiflash.cc" ],
        "include_dirs": [
            "<!(node -e \"require('nan')\")",
            "kiflashcore/src",
            "<!(node -e \"require('streaming-worker-sdk')\")"
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
