cmake_minimum_required(VERSION 3.4)

# bitprim-node
#==============================================================================
project(bitprim-node
        VERSION 3.0.0
        LANGUAGES CXX C)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Check for baseline language coverage in the compiler for the C++11 standard.
#------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Process options.
#==============================================================================
# Implement --with-remote-blockchain and declare WITH_REMOTE_BLOCKCHAIN.
#------------------------------------------------------------------------------
option(WITH_REMOTE_BLOCKCHAIN "Compile with remote bitprim-blockchain (aka. bitprim-blockchain-requester)." ON)

# Implement --with-remote-database and declare WITH_REMOTE_DATABASE.
#------------------------------------------------------------------------------
option(WITH_REMOTE_DATABASE "Compile with remote bitprim-database (aka. bitprim-database-requester)." ON)


# Implement --with-tests and declare WITH_TESTS.
#------------------------------------------------------------------------------
option(WITH_TESTS "Compile with unit tests." ON)

# Implement --with-console and declare WITH_CONSOLE.
#------------------------------------------------------------------------------
option(WITH_CONSOLE "Compile console application." ON)

# Inherit --enable-shared and define BOOST_TEST_DYN_LINK.
#------------------------------------------------------------------------------
option(ENABLE_SHARED "" OFF)

# Check dependencies.
#==============================================================================
# Require Boost of at least version 1.56.0 and output ${boost_CPPFLAGS/LDFLAGS}.
#------------------------------------------------------------------------------
if (NOT ENABLE_SHARED)
  set(Boost_USE_STATIC_LIBS ON)
endif()

find_package(
        Boost 1.56.0 REQUIRED
        COMPONENTS unit_test_framework)

# Require bitprim-protocol of at least version 3.0.0 and output ${bitprim_protocol_CPPFLAGS/LIBS/PKG}.
#------------------------------------------------------------------------------
if (NOT TARGET bitprim-protocol)
  find_package(bitprim-protocol 3.0.0 REQUIRED)
endif()

# Require bitprim-consensus of at least version 2.0.0 and output ${bitprim_consensus_CPPFLAGS/LIBS/PKG}.
#------------------------------------------------------------------------------
if (WITH_CONSENSUS)
  if (NOT TARGET bitprim-consensus)
    find_package(bitprim-consensus 2.0.0 REQUIRED)
  endif()
endif()


# Require bitprim-blockchain of at least version 3.0.0 and output ${bitprim_node_CPPFLAGS/LIBS/PKG}.
#------------------------------------------------------------------------------
if(WITH_REMOTE_BLOCKCHAIN)
  if (NOT TARGET bitprim-blockchain-requester)
    find_package(bitprim-blockchain-requester 3.0.0 REQUIRED)
  endif()
else()
  if (NOT TARGET bitprim-blockchain)
    find_package(bitprim-blockchain 3.0.0 REQUIRED)
  endif()
endif()

if (NOT TARGET bitprim-blockchain)
  find_package(bitprim-blockchain 3.0.0 REQUIRED)
endif()



# Require bitprim-network of at least version 3.0.0 and output ${bitprim_network_CPPFLAGS/LIBS/PKG}.
#------------------------------------------------------------------------------
if (NOT TARGET bitprim-network)
  find_package(bitprim-network 3.0.0 REQUIRED)
endif()

# Set flags.
#==============================================================================
include(CheckCXXCompilerFlag)

function(_add_c_compile_flag _Flag _Var)
  check_cxx_compiler_flag(${_Flag} ${_Var})
  if (${_Var})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${_Flag}" )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_Flag}" )
  endif()
endfunction()

function(_add_cxx_compile_flag _Flag _Var)
  check_cxx_compiler_flag(${_Flag} ${_Var})
  if (${_Var})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_Flag}" )
  endif()
endfunction()

function(_add_link_flag _Flag _Var)
  check_cxx_compiler_flag(${_Flag} ${_Var})
  if (${_Var})
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${_Flag}" )
    set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} ${_Flag}" )
  endif()
endfunction()

# Warn on all stuff.
#------------------------------------------------------------------------------
if (NOT MSVC)
  _add_c_compile_flag(-Wall _has_all_warning_flag)
else()
  _add_c_compile_flag(-W4 _has_all_warning_flag)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
endif()

# Warn on extra stuff.
#------------------------------------------------------------------------------
if (NOT MSVC)
  _add_c_compile_flag(-Wextra _has_extra_warning_flag)
endif()

# Be really annoying.
#------------------------------------------------------------------------------
_add_c_compile_flag(-Wpedantic _has_pedantic_warning_flag)
if (_has_pedantic_warning_flag)
  _add_c_compile_flag(-pedantic _has_pedantic_flag)
endif()

# Conform to style.
#------------------------------------------------------------------------------
_add_cxx_compile_flag(-Wno-missing-braces _has_no_missing_braces_warning_flag)

# Conflict in stdlib under clang. Enabled in clang only.
#------------------------------------------------------------------------------
_add_cxx_compile_flag(-Wno-mismatched-tags _has_no_mismatched_tags_warning_flag)

# Clean up boost 1.55 headers. Enabled in gcc only.
#------------------------------------------------------------------------------
_add_c_compile_flag(-Wno-deprecated-declarations _has_no_deprecated_declarations_warning_flag)

# Protect stack.
#------------------------------------------------------------------------------
_add_link_flag(-fstack-protector _has_stack_protector_flag)

# Protect stack comprehensively.
#------------------------------------------------------------------------------
_add_link_flag(-fstack-protector-all _has_stack_protector_all_flag)

# Hide internal functions from external libs. Enabled in gcc only.
#------------------------------------------------------------------------------
_add_cxx_compile_flag(-fvisibility-hidden _has_visibility_hidden_flag)

# Hide inlines from external libs. Enabled in gcc only.
#------------------------------------------------------------------------------
_add_cxx_compile_flag(-fvisibility-inlines-hidden _has_visibility_inlines_hidden_flag)

# Target Windows Vista. Enabled in msvc only.
#------------------------------------------------------------------------------
if (MSVC)
  add_definitions(-D_WIN32_WINNT=0x0600)
endif()

# Build
#==============================================================================
function(_group_sources target sources_dir)
  file(GLOB_RECURSE _headers
          ${sources_dir}/include/*.h ${sources_dir}/include/*.hpp)
  target_sources(${target} PRIVATE ${_headers})

  get_target_property(sources ${target} SOURCES)
  foreach (source ${sources})
    get_filename_component(group ${source} ABSOLUTE)
    get_filename_component(group ${group} DIRECTORY)
    file(RELATIVE_PATH group "${sources_dir}" "${group}")
    if (group)
      if (MSVC)
        string(REPLACE "/" "\\" group "${group}")
      endif()
      source_group("${group}" FILES "${source}")
    endif()
  endforeach()

  set_target_properties(${target} PROPERTIES FOLDER "node")
endfunction()

# src/bitprim-node.la => ${libdir}
#------------------------------------------------------------------------------
set(MODE STATIC)
if (ENABLE_SHARED)
  set(MODE SHARED)
endif()

add_library(bitprim-node ${MODE}
        src/configuration.cpp
        src/full_node.cpp
        src/parser.cpp
        src/protocols/protocol_block_in.cpp
        src/protocols/protocol_block_out.cpp
        src/protocols/protocol_block_sync.cpp
        src/protocols/protocol_header_sync.cpp
        src/protocols/protocol_transaction_in.cpp
        src/protocols/protocol_transaction_out.cpp
        src/sessions/session_block_sync.cpp
        src/sessions/session_header_sync.cpp
        src/sessions/session_inbound.cpp
        src/sessions/session_manual.cpp
        src/sessions/session_outbound.cpp
        src/settings.cpp
        src/utility/check_list.cpp
        src/utility/header_list.cpp
        src/utility/performance.cpp
        src/utility/reservation.cpp
        src/utility/reservations.cpp)
target_include_directories(bitprim-node PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

target_compile_definitions(bitprim-node PUBLIC -DBCN_STATIC)
if (NOT MSVC)
  target_compile_definitions(bitprim-node PUBLIC -DSYSCONFDIR=\"${SYSCONFDIR}\")
endif()

if (WITH_REMOTE_BLOCKCHAIN)
  target_link_libraries(bitprim-node bitprim-blockchain-requester bitprim-network)
  target_compile_definitions(bitprim-node PUBLIC -DWITH_REMOTE_BLOCKCHAIN)
else()
  target_link_libraries(bitprim-node bitprim-blockchain bitprim-network)
endif()

if (WITH_REMOTE_DATABASE)
  target_compile_definitions(bitprim-node PUBLIC -DWITH_REMOTE_DATABASE)
endif()

_group_sources(bitprim-node "${CMAKE_CURRENT_LIST_DIR}")

# src/bitprim-node-requester.la => ${libdir}
#------------------------------------------------------------------------------
if(WITH_REMOTE_BLOCKCHAIN)
  add_library(bitprim-node-requester ${MODE}
    src/configuration.cpp
    src/full_node.cpp
    src/parser.cpp
    src/protocols/protocol_block_in.cpp
    src/protocols/protocol_block_out.cpp
    src/protocols/protocol_block_sync.cpp
    src/protocols/protocol_header_sync.cpp
    src/protocols/protocol_transaction_in.cpp
    src/protocols/protocol_transaction_out.cpp
    src/sessions/session_block_sync.cpp
    src/sessions/session_header_sync.cpp
    src/sessions/session_inbound.cpp
    src/sessions/session_manual.cpp
    src/sessions/session_outbound.cpp
    src/settings.cpp
    src/utility/check_list.cpp
    src/utility/header_list.cpp
    src/utility/performance.cpp
    src/utility/reservation.cpp
    src/utility/reservations.cpp)
  target_include_directories(bitprim-node-requester PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

  target_compile_definitions(bitprim-node-requester PUBLIC -DBCN_STATIC)
  target_compile_definitions(bitprim-node-requester PUBLIC -DWITH_NODE_REQUESTER)
  if (NOT MSVC)
    target_compile_definitions(bitprim-node-requester PUBLIC -DSYSCONFDIR=\"${SYSCONFDIR}\")
  endif()

  target_link_libraries(bitprim-node-requester bitprim-blockchain-requester bitprim-network)

  _group_sources(bitprim-node-requester "${CMAKE_CURRENT_LIST_DIR}/requester")
endif()
# Tests
#==============================================================================
function(_add_tests target)
  if (ENABLE_SHARED)
    target_compile_definitions(${target} PRIVATE -DBOOST_TEST_DYN_LINK)
  endif()
  target_include_directories(${target} SYSTEM PUBLIC ${Boost_INCLUDE_DIR})
  target_link_libraries(${target} PUBLIC ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

  foreach (_test_name ${ARGN})
    add_test(
            NAME test.node.${_test_name}
            COMMAND ${target}
            --run_test=${_test_name}
            --show_progress=no
            --detect_memory_leak=0
            --report_level=no
            --build_info=yes)
  endforeach()
endfunction()

if (WITH_TESTS)
  enable_testing()
endif()

# local: test/bitprim_node_test
#------------------------------------------------------------------------------
if (WITH_TESTS)
  add_executable(bitprim_node_test
          test/check_list.cpp
          test/configuration.cpp
          test/header_list.cpp
          test/main.cpp
          test/node.cpp
          test/performance.cpp
          test/reservation.cpp
          test/reservations.cpp
          test/settings.cpp
          test/utility.cpp
          test/utility.hpp)
  target_link_libraries(bitprim_node_test PUBLIC bitprim-node)
  _group_sources(bitprim_node_test "${CMAKE_CURRENT_LIST_DIR}/test")

  _add_tests(bitprim_node_test
          configuration_tests
          node_tests
          header_queue_tests
          performance_tests
          reservation_tests
          reservations_tests
          settings_tests)
endif()

# console/bn => ${bindir}
#------------------------------------------------------------------------------
if (WITH_CONSOLE)
  add_executable(console.bn
          console/executor.cpp
          console/executor.hpp
          console/main.cpp)
  target_link_libraries(console.bn bitprim-node)

  set_target_properties(
          console.bn PROPERTIES
          FOLDER "node"
          OUTPUT_NAME bn)
endif()

# console/bnr => ${bindir}
#------------------------------------------------------------------------------
if (WITH_CONSOLE)
  if(WITH_REMOTE_BLOCKCHAIN)
    add_executable(console.bnr
      console/executor.cpp
      console/executor.hpp
      console/main.cpp)
    target_link_libraries(console.bn bitprim-node)

    set_target_properties(
      console.bnr PROPERTIES
      FOLDER "node"
      OUTPUT_NAME bnr)
    target_link_libraries(console.bnr bitprim-node-requester)
  endif()
endif()

# Install
#==============================================================================
if(WITH_REMOTE_BLOCKCHAIN)
  install(TARGETS bitprim-node bitprim-node-requester
          EXPORT bitprim-node
          ARCHIVE DESTINATION lib)
else()
  install(TARGETS bitprim-node
          EXPORT bitprim-node
          ARCHIVE DESTINATION lib)
endif()

if (WITH_CONSOLE)
  install(TARGETS console.bn
          RUNTIME DESTINATION bin)
endif()

set(_bitprim_headers
        # include_bitcoin_HEADERS =
        include/bitcoin/node.hpp
        # include_bitcoin_node_HEADERS =
        include/bitcoin/node/configuration.hpp
        include/bitcoin/node/define.hpp
        include/bitcoin/node/full_node.hpp
        include/bitcoin/node/parser.hpp
        include/bitcoin/node/settings.hpp
        include/bitcoin/node/version.hpp
        # include_bitcoin_node_protocols_HEADERS =
        include/bitcoin/node/protocols/protocol_block_in.hpp
        include/bitcoin/node/protocols/protocol_block_out.hpp
        include/bitcoin/node/protocols/protocol_block_sync.hpp
        include/bitcoin/node/protocols/protocol_header_sync.hpp
        include/bitcoin/node/protocols/protocol_transaction_in.hpp
        include/bitcoin/node/protocols/protocol_transaction_out.hpp
        # include_bitcoin_node_sessions_HEADERS =
        include/bitcoin/node/sessions/session.hpp
        include/bitcoin/node/sessions/session_block_sync.hpp
        include/bitcoin/node/sessions/session_header_sync.hpp
        include/bitcoin/node/sessions/session_inbound.hpp
        include/bitcoin/node/sessions/session_manual.hpp
        include/bitcoin/node/sessions/session_outbound.hpp
        # include_bitcoin_node_utility_HEADERS =
        include/bitcoin/node/utility/check_list.hpp
        include/bitcoin/node/utility/header_list.hpp
        include/bitcoin/node/utility/performance.hpp
        include/bitcoin/node/utility/reservation.hpp
        include/bitcoin/node/utility/reservations.hpp)
foreach (_header ${_bitprim_headers})
  get_filename_component(_directory "${_header}" DIRECTORY)
  install(FILES "include/${_header}" DESTINATION "include/${_directory}")
endforeach()

# Export
#==============================================================================
include(CMakePackageConfigHelpers)

export(EXPORT bitprim-node
        FILE "${CMAKE_CURRENT_BINARY_DIR}/bitprim-nodeTargets.cmake")
export(PACKAGE bitprim-node)

if (UNIX)
  set(_config_package_location "lib/cmake")
elseif (WIN32)
  set(_config_package_location "cmake")
endif()

install(EXPORT bitprim-node
        FILE bitprim-nodeTargets.cmake
        DESTINATION ${_config_package_location})

configure_file(bitprim-nodeConfig.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/bitprim-nodeConfig.cmake"
        @ONLY)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/bitprim-nodeConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion)

install(
        FILES
        "${CMAKE_CURRENT_BINARY_DIR}/bitprim-nodeConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/bitprim-nodeConfigVersion.cmake"
        DESTINATION ${_config_package_location})