project(tictactoe_contract)

cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(ExternalProject)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
#set(EOSIO_CDT_REPO "$ENV{EOSIO_CDT_ROOT}")
set(EOSIO_CDT_REPO "/home/timothy/work/develop/eosio.cdt/release")
find_package(eosio.cdt PATHS ${EOSIO_CDT_REPO}/../modules REQUIRED)

#set(EOSIO_REPO "$ENV{EOSIO_ROOT}")
set(EOSIO_REPO "/home/timothy/work/develop")
find_package(eosio PATHS ${EOSIO_REPO}/eos/CMakeModules REQUIRED)

add_subdirectory(contracts)