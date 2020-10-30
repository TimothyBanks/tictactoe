#include <eosio/eosio.hpp>

#include <string>

CONTRACT kv_example : public eosio::contract {
public:
    using contract::contract;

    struct row {
        std::string key;
        std::string value;
    };

    TABLE kv_table : eosio::kv::table<row, "kvdata"_n> {
        KV_NAMED_INDEX("key"_n, key);
        
        kv_table(eosio::name contract_name) {
            init(contract_name, key);
        }
    };

public:
    kv_example(eosio::name receiver, eosio::name code, eosio::datastream<const char*> ds)
    : contract{receiver, code, ds} {}

    ACTION upsert(std::string key, std::string value) {
        eosio::check(!key.empty(), "key cannot be empty");
        my_table.put({std::move(key), std::move(value)}, get_self());
    }

    ACTION erase(const std::string& key) {
        eosio::check(!key.empty(), "key cannot be empty");
        my_table.erase(key, get_self());
    }

    ACTION printv(const std::string& key) {
        eosio::check(!key.empty(), "key cannot be empty");
        auto it = my_table.key_uidx.find(key);
        if (it == std::end(my_table.key_uidx)) {
            eosio::print(key, " not found");
            return;
        }
        eosio::print("value = ", it.value().value);
    }

    ACTION printa() {
        for (const auto& kv : my_table.key_uidx) {
            eosio::print("{", kv.value().key, ", ", kv.valuue().value, "}");
        }
    }

private:
    kv_table my_table{"kvdata"_n};
};