#include <eosio/eosio.hpp>
#include <eosio/print.hpp>
#include <eosio/singleton.hpp>

#include <vector>

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
      uint64_t key{ 0 };
      eosio::name host;
      eosio::name challenger;
      eosio::name turn;
      eosio::name winner;
      std::vector<eosio::name> board;

      void move(uint8_t row, uint8_t column, eosio::name by) {
          eosio::check(by == turn, "invalid turn");
          eosio::check(winner.value == 0, "game over");

          if (row >= 3 || column >= 3) {
              eosio::check(false, "out of bounds");
          }

          if (board[row * 3 + column].value != 0) {
              eosio::check(false, "position already taken");
          }

          board[row * 3 + column] = by;

          auto the_winner = by;
          for (size_t i = 0; i < 3; ++i) {
              // Check column for winner
              if (board[i * 3 + column] != by) {
                  the_winner = eosio::name{};
                  break;
              }
          }

          if (the_winner.value == 0) {
              // check row for winner
              the_winner = by;
              for (size_t i = 0; i < 3; ++i) {
                if (board[row * 3 + i] != by) {
                    the_winner = eosio::name{};
                    break;
                }
             }
          }

          if (the_winner.value == 0) {
            // check diagonal for winner
            the_winner = by;
            for (size_t i = 0; i < 3; ++i) {
              if (board[i * 3 + i] != by) {
                  the_winner = eosio::name{};
                  break;
              }
            }
          }

          if (the_winner.value == 0) {
            // check diagonal for winner
            the_winner = by;
            for (size_t i = 0; i < 3; ++i) {
              if (board[i * 3 + (3 - i - 1)] != by) {
                  the_winner = eosio::name{};
                  break;
              }
            }
          }

          winner = the_winner;
      }

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

private:
  auto find_(eosio::name challenger, eosio::name host, bool enforce_exists = true) {
      auto key = static_cast<uint128_t>(host.value) << 64 | static_cast<uint128_t>(challenger.value);
      auto idx = m_games.get_index<"hostchal"_n>();
      auto it = idx.find(key);
      if (enforce_exists) {
        eosio::check(it != std::end(idx), "No game exists");
        return m_games.find(it->key);
      } else {
        eosio::check(it == std::end(idx), "game already exists");
        return std::end(m_games);
      }
  }

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

      auto it = find_(challenger, host, false);
      auto system_info = m_system.get_or_create(get_self(), {.account = get_self(), .next_key = 0});
      m_games.emplace(get_self(), [&](auto& row) {
          row.key = system_info.next_key++;
          row.host = host;
          row.challenger = challenger;
          row.turn = host;
          row.winner = eosio::name{};
          row.board = std::vector<eosio::name>(9, eosio::name{});
      });
      m_system.set(system_info, get_self());
  }

  ACTION close(eosio::name challenger, eosio::name host) {
      eosio::require_auth(host);
      auto it = find_(challenger, host);
      m_games.erase(it);
  }

  ACTION restart(eosio::name challenger, eosio::name host, eosio::name by) {
      eosio::require_auth(by);
      eosio::check(challenger == by || host == by, "by must be either the host or challenger");
      auto it = find_(challenger, host);
      m_games.modify(it, get_self(), [&](auto& row){
          row.winner = eosio::name{};
          row.turn = host;
          row.board = std::vector<eosio::name>(9, eosio::name{});
      });
  }

  ACTION move(eosio::name challenger, eosio::name host, eosio::name by, uint8_t row, uint8_t column) {
      eosio::require_auth(by);
      eosio::check(challenger == by || host == by, "by must be either the host or challenger");
      auto it = find_(challenger, host);
      auto winner = eosio::name{};
      m_games.modify(it, get_self(), [&](auto& table_row) {
          table_row.move(row, column, by);
          table_row.turn = by == challenger ? host : challenger;
          winner = table_row.winner;
      });
      if (winner.value != 0) {
          eosio::print("Winner: ", winner);
      }
  }
};