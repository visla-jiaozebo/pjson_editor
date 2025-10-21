

# Doctest based tests
add_executable(test_scene
    test_scene.cpp
    impl/server_login.cpp
)

target_link_libraries(test_scene
    PRIVATE
    pjson_editor
    doctest::doctest
)

if(EMSCRIPTEN)
    # add impl/fetch_http_client.cpp into test_scene
    target_sources(test_scene PRIVATE impl/fetch_http_client.cpp)
    set_target_properties(test_scene PROPERTIES SUFFIX ".html")
    target_compile_options(pjson_editor PRIVATE
        "-mreference-types"
        "-pthread"
        # "-sWASM_WORKERS"
        "-Os"
        "-msimd128"
        "-O0"
        "-gsource-map"
        "-sFETCH=1"
    )
    target_compile_options(test_scene PRIVATE
        "-mreference-types"
        "-pthread"
        # "-sWASM_WORKERS"
        "-Os"
        "-msimd128"
        "-O0"
        "-gsource-map"
        "-sFETCH=1"
    )
    target_link_options(test_scene PRIVATE
        "-std=c++17"
        "-lembind"
        "-lpthread"
        "--proxy-to-worker"
        # "-lGL"
        # "-sOFFSCREENCANVAS_SUPPORT"
        "-sPTHREAD_POOL_SIZE=16"
        "-sPTHREAD_POOL_SIZE_STRICT=16"
        "-sINITIAL_MEMORY=128MB"
        "-sALLOW_MEMORY_GROWTH"
        "-sALLOW_TABLE_GROWTH"
        "-sNO_DISABLE_EXCEPTION_CATCHING"
        "-sWASM_BIGINT=1"
        # "-lidbfs.js"
        "-sENVIRONMENT=web,worker"
        "-Wno-pthreads-mem-growth"
        "-msimd128"
        # "-sUSE_WEBGPU=1" 
        # "-sUSE_WEBGL2=1"
        # "-sMAX_WEBGL_VERSION=2"
        # "-sASYNCIFY_ADVISE"
        # https://github.com/emscripten-core/emscripten/issues/18034#issuecomment-1278778061
        "-sTEXTDECODER=0"
        # for release
        # "-Os"
        # for debug
        "-sASSERTIONS=1"
        "-O0" 
        "-gsource-map" 
        "-sUSE_PTHREADS=1"
        # "-sWASM_WORKERS"
        "-sFETCH=1"
        )
else()
    # add impl/curl_http_client.cpp into test_scene
    target_sources(test_scene PRIVATE impl/curl_http_client.cpp)
endif()

# Register the test with CTest
# add_test(NAME test_addscene_doctest COMMAND test_addscene_doctest)