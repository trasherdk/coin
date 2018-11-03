// Copyright (c) 2011-2016 The Cryptonote developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <cstdint>

namespace crypto {

struct Hash {
  uint8_t data[32];
};

struct PublicKey {
  uint8_t data[32];
};

struct SecretKey {
  uint8_t data[32];
};

struct key_derivation_t {
  uint8_t data[32];
};

struct KeyImage {
  uint8_t data[32];
};

struct signature_t {
  uint8_t data[64];
};

}
