include(FetchContent)
include(CheckCXXCompilerFlag)

add_executable(tests)
target_include_directories(tests PRIVATE "include")
target_compile_features(tests PRIVATE cxx_std_20)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.7.0
)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(Catch2 nlohmann_json)

target_compile_features(Catch2 PRIVATE cxx_std_17)
target_compile_definitions(Catch2 PUBLIC CATCH_CONFIG_ENABLE_OPTIONAL_STRINGMAKER)

target_link_libraries(tests PRIVATE lahzam::lahzam Catch2::Catch2WithMain nlohmann_json::nlohmann_json)
  
if(MSVC)
  target_compile_options(tests PRIVATE /WX /W4 /Za /permissive-)
else()
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 22)
    target_compile_options(tests PRIVATE -Wno-c2y-extensions -Wno-error=c2y-extensions)
  endif()
  target_compile_options(tests PRIVATE -Werror -Wall -Wextra -Wshadow -Wconversion -Wpedantic)
endif()

file(GLOB SRCS "*.cpp")

target_sources(tests PRIVATE ${SRCS})

include(CTest)
include(Catch)
catch_discover_tests(tests)
