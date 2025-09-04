add_executable(tests)
target_include_directories(tests PRIVATE "include")
target_compile_features(tests PRIVATE cxx_std_20)
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.7.0
)
FetchContent_MakeAvailable(Catch2)
target_compile_features(Catch2 PRIVATE cxx_std_17)
target_compile_definitions(Catch2 PUBLIC CATCH_CONFIG_ENABLE_OPTIONAL_STRINGMAKER)

target_link_libraries(tests PRIVATE lahzam::lahzam Catch2::Catch2WithMain)
  
if(MSVC)
  target_compile_options(tests PRIVATE /Za /permissive-)
  target_compile_options(tests PRIVATE /WX /W4)
else()
  target_compile_options(tests PRIVATE -Werror -Wall -Wextra -Wshadow -Wconversion -Wpedantic)
endif()

file(GLOB_RECURSE SRCS "main.cpp")

target_sources(tests PRIVATE ${SRCS})

include(CTest)
include(Catch)
catch_discover_tests(tests)
