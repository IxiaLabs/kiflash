{
    "targets": [{
        "target_name": "FlashProviderExtension",
        "conditions": [            
            [
                'OS == "linux"', {
                    "defines": [
                        "_HAS_EXCEPTIONS=1"
                    ],
                    'configurations': {
                        'Release': {
                            "defines": [
                                "NDEBUG"
                            ],
                        }
                    },
                    "cflags!": [ '-fno-exceptions' ],
                    "cflags_cc!": [ '-fno-exceptions' ],
                    "sources": [
                        "src/native/Linux/FlashProviderWrapperLinux.cpp"
                    ]
                }
            ]
        ],        
        "sources": [
            "src/native/LicenseConfig.cpp",
            "src/native/FloLicenseProviderNativeExtension.cpp",
            "src/native/FloLicenseProviderWrapper.cpp",
            "src/native/FloLicenseProviderNodeAdapter.cpp",
            "src/native/NanGetAvailableFeatureListAsyncWorker.cpp",
            "src/native/NanGetLicenseServerStatusAsyncWorker.cpp",
            "src/native/NanCheckoutAsyncWorker.cpp",
            "src/native/NanCheckinAsyncWorker.cpp",
            "src/native/NanSetLicenseSearchPathAsyncWorker.cpp"
        ],
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

    }]
}
