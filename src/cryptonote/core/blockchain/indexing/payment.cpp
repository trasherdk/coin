#include "payment.h"
#include "cryptonote/core/CryptoNoteTools.h"
#include "blockchain_explorer/BlockchainExplorerDataBuilder.h"

namespace cryptonote {

bool PaymentIdIndex::add(const Transaction& transaction) {
  crypto::hash_t paymentId;
  crypto::hash_t transactionHash = getObjectHash(transaction);
  if (!BlockchainExplorerDataBuilder::getPaymentId(transaction, paymentId)) {
    return false;
  }

  index.emplace(paymentId, transactionHash);

  return true;
}

bool PaymentIdIndex::remove(const Transaction& transaction) {
  crypto::hash_t paymentId;
  crypto::hash_t transactionHash = getObjectHash(transaction);
  if (!BlockchainExplorerDataBuilder::getPaymentId(transaction, paymentId)) {
    return false;
  }

  auto range = index.equal_range(paymentId);
  for (auto iter = range.first; iter != range.second; ++iter){
    if (iter->second == transactionHash) {
      index.erase(iter);
      return true;
    }
  }

  return false;
}

bool PaymentIdIndex::find(const crypto::hash_t& paymentId, std::vector<crypto::hash_t>& transactionHashes) {
  bool found = false;
  auto range = index.equal_range(paymentId);
  for (auto iter = range.first; iter != range.second; ++iter){
    found = true;
    transactionHashes.emplace_back(iter->second);
  }
  return found;
}

void PaymentIdIndex::clear() {
  index.clear();
}


void PaymentIdIndex::serialize(ISerializer& s) {
  s(index, "index");
}
}