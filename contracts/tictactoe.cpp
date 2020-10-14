#include <eosio/eosio.hpp>
#include <eosio/print.hpp>

CONTRACT tictactoe : public eosio::contract {
public:
  using contract::contract;

  ACTION welcome(eosio::name host, eosio::name opponent) {
      eosio::require_auth(get_self());
      eosio::print("Welcome, ", host, " and ", opponent);
  }
};