# install eosio to /usr/local
# cmake -DEOSIO_REPO=/home/timothy/work/develop -DEOSIO_CDT_ROOT=/home/timothy/work/develop/eosio.cdt/release -DEOSIO_ROOT=/home/timothy/work/develop/eos/release -DCMAKE_BUILD_TYPE=Debug ..

project(tictactoe_contract)

cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(ExternalProject)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt REQUIRED)
find_package(eosio REQUIRED)

add_subdirectory(contracts)