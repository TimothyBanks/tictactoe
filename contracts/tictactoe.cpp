#include <eosio/eosio.hpp>
#include <eosio/print.hpp>

CONTRACT tictactoe : public eosio::contract {
private:
  TABLE game {
      eosio::name host;
      eosio::name challenger;

      uint128_t primary_key() const {
          return static_cast<uint128_t>(host.value) << 64 || challenger.value;
      }
  };
  using games_table = eosio::multi_index<"games"_n, game>; 

  games_table m_games;

public:
  using contract::contract;

  tictactoe(eosio::name receiver, eosio::name code, eosio::datastream<const char*> ds)
  : contract{receiver, code, ds},
    m_games{receiver, receiver.value} {}

  ACTION welcome(eosio::name challenger, eosio::name host) {
      eosio::require_auth(get_self());
      eosio::print("Welcome, ", host, " and ", challenger);
  }

  ACTION create(eosio::name challenger, eosio::name host) {
      eosio::check(challenger != host, "challenger and host can't be the same");
      eosio::require_auth(host);

      auto key = static_cast<uint128_t>(host.value) << 64 || challenger.value;
      auto it = m_games.find(key);
      eosio::check(it == std::end(m_games), "game already exists");

      m_games.emplace(get_self(), [&](auto& row) {
          row.host = host;
          row.challenger = challenger;
      });
  }

  ACTION close(eosio::name challenger, eosio::name host) {
      eosio::require_auth(host);

      auto key = static_cast<uint128_t>(host.value) << 64 || challenger.value;
      auto it = m_games.find(key);
      if (it != std::end(m_games)) {
          m_games.erase(it);
      }
  }
};