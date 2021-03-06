
cmake_minimum_required(VERSION 3.5)

project(tictactoe)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt REQUIRED)
find_package(eosio REQUIRED)

include(${EOSIO_CDT_ROOT}/lib/cmake/eosio.cdt/EosioWasmToolchain.cmake)

add_contract(tictactoe tictactoe tictactoe.cpp)
add_contract(kvexample kvexample kv_example.cpp)

message(STATUS "EOSIO_CDT_ROOT = ${EOSIO_CDT_ROOT}")

target_include_directories(${PROJECT_NAME} 
	PUBLIC 
	${CMAKE_SOURCE_DIR}/include
	${EOSIO_REPO}/eosio.contracts/contracts/eosio.token/include
  ${EOSIO_REPO}/eos/libraries/fc/include
  /usr/local/include/
)

set_target_properties(tictactoe
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tictactoe"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

#target_ricardian_directory(accounting ${CMAKE_SOURCE_DIR}/../ricardian)