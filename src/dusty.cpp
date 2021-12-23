#include <dusty.hpp>
#include <vector>

using namespace eosio;

void dusty::buytickets(name from, name to, asset quantity, std::string memo) {
   check(!is_paused(), "Contract is paused");
   check( memo.size() <= 256, "memo has more than 256 bytes" );
   if(memo.length() == 0 || memo.empty()) {
      return;
   }
   if(memo != "buytickets") {
      return;
   }
   if(from == get_self() || to != get_self()) {
      return;
   }
   uint64_t multiplier = pow(10, 8);

   if(quantity.amount != (1 * multiplier) && quantity.amount != (3 * multiplier) && quantity.amount != (10 * multiplier)) {
      return;
   }
  
   check(quantity.is_valid(), "Invalid transfer amount");
   if(quantity.symbol != wax_currency) {
      return;
   }

   uint64_t tickets = 0;

   if(quantity.amount == 1 * multiplier) {
      tickets = 1;
   }
   else if(quantity.amount == 3 * multiplier){
      tickets = 4;
   }
   else if(quantity.amount == 10 * multiplier){
      tickets = 15;
   }
   else {
      return;
   }
   
   auto it = users.find(from.value);
   if(it == users.end()) {
      users.emplace(get_self(), [&](auto& rec) {
         rec.nm = from;
         rec.wax_tickets = tickets;
      });
   }
   else {
       users.modify(it, _self, [&](auto& rec) {
         rec.wax_tickets += tickets;
      });
   }

   auto it2 = pool.find(1);
   if(it2 == pool.end()) {
      pool.emplace(get_self(), [&](auto& rec) {
         rec.id = 1;
         rec.wax_total = quantity.amount;
         rec.wax_tickets = tickets;
      });
   }
   else {
      pool.modify(it2, _self, [&](auto& rec) {
         rec.wax_total += quantity.amount;
         rec.wax_tickets += tickets;
      });
   }

   auto it3 = whitelist.find(from.value);
   if(it3 == whitelist.end()) {
      whitelist.emplace(get_self(), [&](auto& rec) {
         rec.nm = from;
      });
   }

}

void dusty::buypgl(name from, name to, asset quantity, std::string memo) {
   check(!is_paused(), "Contract is paused");
   check( memo.size() <= 256, "memo has more than 256 bytes" );
   if(memo.length() == 0 || memo.empty()) {
      return;
   }
   if(memo != "buytickets") {
      return;
   }
   if(from == get_self() || to != get_self()) {
      return;
   }
   uint64_t multiplier = pow(10, 4);

   if(quantity.amount != (1 * multiplier) && quantity.amount != (3 * multiplier) && quantity.amount != (10 * multiplier)) {
      return;
   }
  
   check(quantity.is_valid(), "Invalid transfer amount");
   if(quantity.symbol != pgl_currency) {
      return;
   }

   uint64_t tickets = 0;

   if(quantity.amount == 1 * multiplier) {
      tickets = 1;
   }
   else if(quantity.amount == 3 * multiplier){
      tickets = 4;
   }
   else if(quantity.amount == 10 * multiplier){
      tickets = 15;
   }
   else {
      return;
   }
   
   auto it = users.find(from.value);
   if(it == users.end()) {
      users.emplace(get_self(), [&](auto& rec) {
         rec.nm = from;
         rec.pgl_tickets = tickets;
      });
   }
   else {
       users.modify(it, _self, [&](auto& rec) {
         rec.pgl_tickets += tickets;
      });
   }

   auto it2 = pool.find(1);
   if(it2 == pool.end()) {
      pool.emplace(get_self(), [&](auto& rec) {
         rec.id = 1;
         rec.pgl_total = quantity.amount;
         rec.pgl_tickets = tickets;
      });
   }
   else {
      pool.modify(it2, _self, [&](auto& rec) {
         rec.pgl_total += quantity.amount;
         rec.pgl_tickets += tickets;
      });
   }

   auto it3 = whitelist.find(from.value);
   if(it3 == whitelist.end()) {
      whitelist.emplace(get_self(), [&](auto& rec) {
         rec.nm = from;
      });
   }
}

void dusty::getwinner(uint64_t assoc) {
   require_auth(get_self());
   check(!is_paused(), "Contract is paused");

   //create singing value
   auto size = eosio::transaction_size();
   char buf[size];

   auto read = eosio::read_transaction(buf, size);
   eosio::check(size == read, "read_transaction() has failed.");

   auto tx_signing_value = eosio::sha256(buf, size);

   auto byte_array = tx_signing_value.extract_as_byte_array();
   uint64_t signing_value = 0;
   for (int k = 0; k < 8; k++)
   {
      signing_value <<= 8;
      signing_value |= (uint64_t)byte_array[k];
   }
   uint64_t time = current_time_point().time_since_epoch().count();

   action(
      permission_level{get_self(), "active"_n},
      "orng.wax"_n,
      "requestrand"_n,
      std::tuple{ time, signing_value, get_self() }
   ).send();
}

ACTION dusty::receiverand(uint64_t customer_id, const checksum256& random_value) {
   require_auth(name("orng.wax"));
   check(!is_paused(), "Contract is paused");

   uint32_t wax_tickets = 0;
   uint32_t pgl_tickets = 0;
   uint32_t wax_total = 0;
   uint32_t pgl_total = 0;

   auto it = pool.find(1);
   check(it != pool.end(), "No users registered");
   wax_tickets = it->wax_tickets;
   pgl_tickets = it->pgl_tickets;
   wax_total = it->wax_total;
   pgl_total = it->pgl_total;

   std::vector<uint32_t> wax_counts;
   std::vector<uint32_t> pgl_counts;
   std::vector<name> wax_names;
   std::vector<name> pgl_names;
   uint32_t wax_count = 0;
   uint32_t pgl_count = 0;

   auto it2 = users.begin();
   
   while (it2 != users.end()) {
      if(it2->wax_tickets > 0) {
         wax_count += it2->wax_tickets;
         wax_counts.push_back(wax_count);
         wax_names.push_back(it2->nm);
      }
      if(it2->pgl_tickets > 0) {
         pgl_count += it2->pgl_tickets;
         pgl_counts.push_back(pgl_count);
         pgl_names.push_back(it2->nm);
      }
      it2++;
   }

   auto byte_array = random_value.extract_as_byte_array();

   uint64_t random_int = 0;
   for (int i = 0; i < 8; i++) {
      random_int <<= 8;
      random_int |= (uint64_t)byte_array[i];
   }

   //Get WAX winner
   uint64_t wax_num = random_int % wax_tickets;
   int win = 0;
   for(int i=0; i<wax_counts.size(); i++) {
      if(wax_counts[i] > wax_num) {
         win = i;
         break;
      }
   }
   name nm = wax_names[win];
   auto it3 = waxwinner.begin();
   if(it3 == waxwinner.end()) {
      waxwinner.emplace(get_self(), [&](auto& rec) {
         rec.nm = wax_names[win];
         rec.value = wax_num;
      });
   }
   else {
      it3 = waxwinner.begin();
      waxwinner.erase(it3);
      it3 = waxwinner.begin();
      waxwinner.emplace(get_self(), [&](auto& rec) {
         rec.nm = wax_names[win];
         rec.value = wax_num;
      });
   }
      
   //Get PGL winner
   uint64_t pgl_num = random_int % pgl_tickets;
   win = 0;
   for(int i=0; i<pgl_counts.size(); i++) {
      if(pgl_counts[i] > pgl_num) {
         win = i;
         break;
      }
   }
   nm = pgl_names[win];
   auto it4 = pglwinner.begin();
   if(it4 == pglwinner.end()) {
      pglwinner.emplace(get_self(), [&](auto& rec) {
         rec.nm = pgl_names[win];
         rec.value = pgl_num;
      });
   }
   else {
      it4 = pglwinner.begin();
      pglwinner.erase(it4);
      it4 = pglwinner.begin();
      pglwinner.emplace(get_self(), [&](auto& rec) {
         rec.nm = pgl_names[win];
         rec.value = pgl_num;
      });
   }
}

ACTION dusty::delwinner() {
   require_auth(get_self());
   auto it = waxwinner.begin();
   if(it != waxwinner.end()){
      waxwinner.erase(it);
   }

   auto it2 = pglwinner.begin();
   if(it2 != pglwinner.end()){
      pglwinner.erase(it2);
   }
}

ACTION dusty::delusers() {
   require_auth(get_self());
   auto it = users.begin();
   while(it != users.end()){
      if(it != users.end()){
         it = users.erase(it++);
      }
   }

   auto it2 = pool.find(1);
   pool.erase(it2);
}

void dusty::pause(bool paused) {
    require_auth(get_self());
    set_config(paused_row, uint64_t(paused));
}

bool dusty::is_paused() const {
    return get_config(paused_row, false);
}

void dusty::set_config(uint64_t name, int64_t value) {
    auto it = config.find(name);
    if (it == config.end()) {
        config.emplace(get_self(), [&](auto& rec) {
            rec.name = name;
            rec.value = value;
        });
    } 
    else {
        config.modify(it, get_self(), [&](auto& rec) {
            rec.value = value;
        });
    }
}

int64_t dusty::get_config(uint64_t name, int64_t default_value) const {
    auto it = config.find(name);
    if (it == config.end()) 
        return default_value;
    return it->value;
}