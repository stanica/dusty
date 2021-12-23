#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/transaction.hpp>
#include <eosio/crypto.hpp>
#include <eosio/system.hpp>
#include <math.h>
#include <string>

using namespace eosio;

CONTRACT dusty : public contract {
   public:
      using contract::contract;

      dusty(name receiver, eosio::name code, eosio::datastream<const char*> ds):
      contract(receiver, code, ds),
      wax_currency("WAX", 8),
      pgl_currency("PGL", 4),
      pool(receiver, receiver.value),
      config(receiver, receiver.value),
      whitelist(receiver, receiver.value),
      users(receiver, receiver.value),
      pglwinner(receiver, receiver.value),
      waxwinner(receiver, receiver.value) {}

      [[eosio::on_notify("eosio.token::transfer")]]
      void buytickets(name from, name to, asset quantity, std::string memo);
      [[eosio::on_notify("prospectorsw::transfer")]]
      void buypgl(name from, name to, asset quantity, std::string memo);
      ACTION pause(bool paused);
      ACTION delwinner();
      ACTION receiverand(uint64_t customer_id, const eosio::checksum256& random_value);
      ACTION getwinner(uint64_t assoc);
      ACTION delusers();

      TABLE users_table {
         name nm;
         uint32_t wax_tickets;
         uint32_t pgl_tickets;
         uint64_t primary_key() const { return nm.value; }
      };

      TABLE pool_table {
         uint16_t id;
         uint64_t wax_total;
         uint64_t pgl_total;
         uint32_t wax_tickets;
         uint32_t pgl_tickets;
         uint16_t primary_key() const { return id; }
      };

       TABLE whitelist_table {
         name nm;
         uint64_t primary_key() const { return nm.value; }
      };

      TABLE config_a {
         uint64_t name;
         int64_t  value;
         auto primary_key() const { return name; }
      };

      TABLE wax_winner_table {
         name nm;
         uint64_t value;
         uint64_t primary_key() const { return nm.value; }
      };

      TABLE pgl_winner_table {
         name nm;
         uint64_t value;
         uint64_t primary_key() const { return nm.value; }
      };

      typedef multi_index<"users"_n, users_table> users_index;
      typedef multi_index<"config.a"_n, config_a> config_index;
      typedef multi_index<"pool"_n, pool_table> pool_index;
      typedef multi_index<"whitelist"_n, whitelist_table> whitelist_index;
      typedef multi_index<"waxwinner"_n, wax_winner_table> wax_winner_index;
      typedef multi_index<"pglwinner"_n, pgl_winner_table> pgl_winner_index;

      users_index users;
      config_index config;
      pool_index pool;
      whitelist_index whitelist;
      wax_winner_index waxwinner;
      pgl_winner_index pglwinner;

      using buytickets_action = eosio::action_wrapper<"buytickets"_n, &dusty::buytickets>;
      using buypgl_action = eosio::action_wrapper<"buypgl"_n, &dusty::buypgl>;
      using pause_action = action_wrapper<"pause"_n, &dusty::pause>;
      using getwinner_action = action_wrapper<"getwinner"_n, &dusty::getwinner>;
      using receiverand_action = eosio::action_wrapper<"receiverand"_n, & dusty::receiverand>;
      using delwinner_action = action_wrapper<"delwinner"_n, &dusty::delwinner>;
      using delusers_action = action_wrapper<"delusers"_n, &dusty::delusers>;
      
      static constexpr uint64_t paused_row = "paused"_n.value;

      // Helpers
      bool is_paused() const;
      void set_config(uint64_t name, int64_t value);
      int64_t get_config(uint64_t name, int64_t default_value) const;
   
   private:
      const eosio::symbol wax_currency;
      const eosio::symbol pgl_currency;

      inline uint32_t now() const {
         return current_time_point().sec_since_epoch();
      }
};