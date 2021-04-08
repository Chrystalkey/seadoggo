# CMake generated Testfile for 
# Source directory: F:/Developement/active/seadoggo/dependencies/cJSON-1.7.14
# Build directory: F:/Developement/active/seadoggo/cmake-build-release/dependencies/cJSON-1.7.14
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(cJSON_test "F:/Developement/active/seadoggo/cmake-build-release/dependencies/cJSON-1.7.14/cJSON_test")
set_tests_properties(cJSON_test PROPERTIES  _BACKTRACE_TRIPLES "F:/Developement/active/seadoggo/dependencies/cJSON-1.7.14/CMakeLists.txt;242;add_test;F:/Developement/active/seadoggo/dependencies/cJSON-1.7.14/CMakeLists.txt;0;")
subdirs("tests")
subdirs("fuzzing")
