project(dusty)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( dusty dusty dusty.cpp )
target_include_directories( dusty PUBLIC ${CMAKE_SOURCE_DIR}/../include )
target_ricardian_directory( dusty ${CMAKE_SOURCE_DIR}/../ricardian )