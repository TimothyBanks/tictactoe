#include <eosio/eosio.hpp>
#include <eosio/print.hpp>
#include <eosio/singleton.hpp>

CONTRACT tictactoe : public eosio::contract {
private:
  TABLE system {
      eosio::name account;
      uint64_t next_key{ 0 };

      uint64_t primary_key() const {
          return account.value;
      }
  };
  using system_table = eosio::singleton<"ttt.system"_n, system>;

  TABLE game {
      uint64_t key;
      eosio::name host;
      eosio::name challenger;

      uint64_t primary_key() const {
          return key;
      }

      uint128_t by_host_and_challenger() const {
          return static_cast<uint128_t>(host.value) << 64 | static_cast<uint128_t>(challenger.value);
      }

      uint64_t by_host() const {
          return host.value;
      }

      uint64_t by_challenger() const {
          return challenger.value;
      }
  };
  using games_table = eosio::multi_index<"games"_n, game, 
    eosio::indexed_by<"hostchal"_n, eosio::const_mem_fun<game, uint128_t, &game::by_host_and_challenger>>,
    eosio::indexed_by<"host"_n, eosio::const_mem_fun<game, uint64_t, &game::by_host>>, 
    eosio::indexed_by<"chal"_n, eosio::const_mem_fun<game, uint64_t, &game::by_challenger>>>; 

  games_table m_games;
  system_table m_system;

public:
  using contract::contract;

  tictactoe(eosio::name receiver, eosio::name code, eosio::datastream<const char*> ds)
  : contract{receiver, code, ds},
    m_games{receiver, receiver.value},
    m_system{receiver, receiver.value} {}

  ACTION welcome(eosio::name challenger, eosio::name host) {
      eosio::require_auth(get_self());
      eosio::print("Welcome, ", host, " and ", challenger);
  }

  ACTION create(eosio::name challenger, eosio::name host) {
      eosio::check(challenger != host, "challenger and host can't be the same");
      eosio::require_auth(host);

      auto key = static_cast<uint128_t>(host.value) << 64 | static_cast<uint128_t>(challenger.value);
      auto idx = m_games.get_index<"hostchal"_n>();
      auto it = idx.find(key);
      eosio::check(it == std::end(idx), "game already exists");

      auto system_info = m_system.get_or_create(get_self(), {.account = get_self(), .next_key = 0});
      m_games.emplace(get_self(), [&](auto& row) {
          row.key = system_info.next_key++;
          row.host = host;
          row.challenger = challenger;
      });
      m_system.set(system_info, get_self());
  }

  ACTION close(eosio::name challenger, eosio::name host) {
      eosio::require_auth(host);

      auto key = static_cast<uint128_t>(host.value) << 64 | static_cast<uint128_t>(challenger.value);
      auto idx = m_games.get_index<"hostchal"_n>();
      auto it = idx.find(key);
      if (it != std::end(idx)) {
          m_games.erase(m_games.find(it->key));
      }
  }
};