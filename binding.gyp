{
	"targets": [{
		"target_name": "binding",
		"sources": [
			"src/main.cc"
		],
		"include_dirs": ["<!(node -e \"require('nan')\")"],
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
					'-std=c++14'
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
				'__WIN32__'
			]
            }
			],
			['OS=="linux"',
				{}
			]
		]
	}, {
		"target_name": "action_after_build",
		"type": "none",
		"dependencies": ["<(module_name)"],
		"copies": [{
			"files": [ "<(PRODUCT_DIR)/<(module_name).node" ],
			"destination": "<(module_path)"
		}]
	}]
}