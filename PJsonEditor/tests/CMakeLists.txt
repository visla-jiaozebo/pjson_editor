

# Doctest based tests
add_executable(test_scene
    test_scene.cpp
    impl/server_login.cpp
    impl/curl_http_client.cpp
)

target_link_libraries(test_scene
    PRIVATE
    pjson_editor
    doctest::doctest
)

# Register the test with CTest
add_test(NAME test_addscene_doctest COMMAND test_addscene_doctest)