{
    "targets": [{
        "target_name": "binding",
        "cflags!": ["-fno-exceptions"],
        "cflags_cc!": ["-fno-exceptions"],
        "sources": [
            "src/main.cc"
        ],
        "include_dirs": [
            "<!@(node -p \"require('node-addon-api').include\")"
        ],
        "defines": [
            "NAPI_DISABLE_CPP_EXCEPTIONS"
        ],
        "conditions": [
            ['OS=="mac"', {
                "sources": [
                    "src/clip_osx.mm"
                ],
                "xcode_settings": {
                    "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
                    "CLANG_CXX_LIBRARY": "libc++",
                    "MACOSX_DEPLOYMENT_TARGET": "10.9"
                },
                "link_settings": {
                    "libraries": [
                        "-framework Cocoa"
                    ]
                }
            }],
            ['OS=="win"', {
                "sources": [
                    "src/clip_win.cc"
                ],
                "msvs_settings": {
                    "VCCLCompilerTool": {
                        "ExceptionHandling": 1
                    }
                }
            }],
            ['OS=="linux"', {
                "sources": [
                    "src/clip_linux.cc"
                ],
                "libraries": [
                    "-lX11"
                ]
            }]
        ]
    }]
}
