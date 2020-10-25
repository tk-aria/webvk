{
    'targets': [
        {
            'target_name': 'napi_zstd',
            'sources': ['src/napi-zstd.cc', 'src/zstd-wrapper.cc'],
            'include_dirs': [
                '<!@(node -p \'require("node-addon-api").include\')',
                '<(module_root_dir)/external/zstd/lib',
            ],
            'cflags!': ['-fno-exceptions'],
            'cflags_cc!': ['-fno-exceptions', '-std=c++14'],
            'defines': ['NAPI_DISABLE_CPP_EXCEPTIONS'],
            'libraries': ['<(module_root_dir)/external/zstd/lib/libzstd.a'],
            'conditions': [
                [
                    'OS=="mac"',
                    {
                        'xcode_settings': {
                            'GCC_ENABLE_CPP_EXCEPTIONS': 'NO',
                            'CLANG_CXX_LIBRARY': 'libc++',
                            'CLANG_CXX_LANGUAGE_STANDARD': 'c++14',
                            'MACOSX_DEPLOYMENT_TARGET': '10.13'
                        }
                    },
                ]
            ],
        }
    ]
}
