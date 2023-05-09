{
	"targets": [{
		"target_name": "binding",
		"cflags!": [ "-fno-exceptions" ], # 忽略编译时异常
      	"cflags_cc!": [ "-fno-exceptions" ],  # 忽略编译时异常
		"sources": [
			"src/main.cc"
		],
		"include_dirs": [
			 "<!@(node -p \"require('node-addon-api').include\")"
			],
        "conditions": [
			['OS=="mac"',
			{
            "sources":[
                "src/clip_osx.h",
                "src/clip_osx.mm"
            ],
			'defines': [
				'__MACOSX_CORE__',
                '__MAC__'
			],
			'link_settings': {
				'libraries': [
                    '-framework Cocoa',
					'-framework CoreFoundation',
				]
			},
			'xcode_settings': {
				'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
				'OTHER_CFLAGS': [
					'-ObjC++',
					'-std=c++17'
				]
			    }
			}
			],
			['OS=="win"',
			{
            "sources":[
                "src/clip_win.h",
                "src/clip_win.cc"
            ],
            'defines': [
				'__WIN32__',
				'NAPI_DISABLE_CPP_EXCEPTIONS' # 禁用 N-API 中的 C++ 异常处理机制, N-API 函数在发生错误时会返回一个错误码，而不是抛出异常
			]
            }
			],
			['OS=="linux"',
				{}
			]
		]
	}]
}
