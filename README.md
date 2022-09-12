# Тестовое Задание:

* сделать поддержку westend (testnet) для polkadot в wallet-core
* https://wiki.polkadot.network/docs/maintain-networks#westend-test-network
* https://github.com/trustwallet/wallet-core
* wallet-core должен генерировать адреса, валидировать и создавать транзакции в тестовой сети Westend

# Решение

## Разбор
1. TWBlockchain.h
    * Добавил новый id инкрементально(может быть конфликт из-за TWBlockchainWestend=43)
2. TWCoinType.h
    * Добавил новый id (взял последний +1 https://github.com/satoshilabs/slips/blob/master/slip-0044.md)(может быть конфликт из-за TWCoinTypeWestend = 1179993421)
3. TWSS58AddressType.h
    * Добавил новый тип адреса для Westend(https://wiki.polkadot.network/docs/learn-accounts#:~:text=the%20address%20type%2C-,42,-is%20in%20decimal)
4. registry.json
    * Аналогично Kusama
5. Coin.cpp
    * Добавления Westend для выбора
6. Extrinsic.cpp, Address.h, Entry.cpp, Entry.h
    * Аналогично Kusama
7. Polkadot.proto
    * добавил WESTEND для генерации