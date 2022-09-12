#include <TrustWalletCore/TWHDWallet.h>
#include <TrustWalletCore/TWPrivateKey.h>
#include <TrustWalletCore/TWString.h>
#include "uint256.h"
#include "HexCoding.h"
#include "Polkadot/Signer.h"
#include "Polkadot/Extrinsic.h"
#include "Westend/Address.h"
#include "Polkadot/SS58Address.h"
#include "HexCoding.h"
#include "PrivateKey.h"
#include "PublicKey.h"
#include "proto/Polkadot.pb.h"
#include "Hash.h"
#include <iostream>
#include <string>

using namespace std;

int main() {
    try {
        {
            TWHDWallet* wallet1 = TWHDWalletCreate(128, TWStringCreateWithUTF8Bytes(""));
            TWHDWallet* wallet2 = TWHDWalletCreate(128, TWStringCreateWithUTF8Bytes(""));
            
            cout << "Secret mnemonic for new wallet: '";
            cout << TWStringUTF8Bytes(TWHDWalletMnemonic(wallet1)) << "'." << endl;

            const TWCoinType coinType = TWCoinType::TWCoinTypeWestend;
            // Создание аккаунта
            string address = TWStringUTF8Bytes(TWHDWalletGetAddressForCoin(wallet1, coinType)); 
            string addressTo = TWStringUTF8Bytes(TWHDWalletGetAddressForCoin(wallet2, coinType));
            cout << "Address:            '" << address << "'" << endl;
            
            TWPrivateKey* secretPrivateKeyDefault = TWHDWalletGetKeyForCoin(wallet1, coinType);
            auto secretPrivKey = TWPrivateKeyData(secretPrivateKeyDefault);

            auto genesisBlockHash = TW::parse_hex("0xe143f23803ac50e8f6f8e62695d1ce9e4e1d68aa36c1cd2cfd15340213f3423e");
            auto blockHash = TW::parse_hex("0x937f46e8285574bdfa66cea9777eb462b1cb07f4dfa2c8967ab2b4bd716fd80b");
            auto input = TW::Polkadot::Proto::SigningInput();
            input.set_genesis_hash(genesisBlockHash.data(), genesisBlockHash.size());
            input.set_block_hash(blockHash.data(), blockHash.size());
            input.set_nonce(1);
            input.set_spec_version(28);
            input.set_private_key(TWDataBytes(secretPrivKey), static_cast<int>(TWDataSize(secretPrivKey)));
            input.set_network(TW::Polkadot::Proto::Network::WESTEND);
            input.set_transaction_version(5);

            auto era = input.mutable_era();
            era->set_block_number(12582123);
            era->set_period(64);

            auto balanceCall = input.mutable_balance_call();
            auto transfer = balanceCall->mutable_transfer();
            auto value = TW::store(TW::uint256_t(2000000000));
            transfer->set_to_address(addressTo);
            transfer->set_value(value.data(), value.size());

            auto privateKey = TW::PrivateKey(TW::Data(input.private_key().begin(), input.private_key().end()));
            auto publicKey = privateKey.getPublicKey(TWPublicKeyTypeED25519);
            auto extrinsic = TW::Polkadot::Extrinsic(input);
            auto payload = extrinsic.encodePayload();

            if (payload.size() > 256) {
                payload = TW::Hash::blake2b(payload, 32);
            }
            auto signature = privateKey.sign(payload, TWCurveED25519);
            auto encoded = extrinsic.encodeSignature(publicKey, signature);
            auto protoOutput = TW::Polkadot::Proto::SigningOutput();
            protoOutput.set_encoded(encoded.data(), encoded.size());

            cout << "Tx: " << TW::hex(protoOutput.encoded()) << endl;
            cout << "isVerify: " << publicKey.verify(signature, payload) << endl;
        }
    } catch (const exception& ex) {
        cout << "EXCEPTION: " << ex.what() << endl;
        throw ex;
    }
}
