// Copyright (c) 2011-2016 The Cryptonote developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "common/ObserverManager.h"
#include "ITransfersSynchronizer.h"
#include "IBlockchainSynchronizer.h"
#include "TypeHelpers.h"

#include <unordered_map>
#include <memory>
#include <cstring>

namespace cryptonote {
class Currency;
}

namespace cryptonote {
 
class TransfersConsumer;
class INode;

class TransfersSyncronizer : public ITransfersSynchronizer, public IBlockchainConsumerObserver {
public:
  TransfersSyncronizer(const cryptonote::Currency& currency, IBlockchainSynchronizer& sync, INode& node);
  virtual ~TransfersSyncronizer();

  void initTransactionPool(const std::unordered_set<crypto::hash_t>& uncommitedTransactions);

  // ITransfersSynchronizer
  virtual ITransfersSubscription& addSubscription(const AccountSubscription& acc) override;
  virtual bool removeSubscription(const AccountPublicAddress& acc) override;
  virtual void getSubscriptions(std::vector<AccountPublicAddress>& subscriptions) override;
  virtual ITransfersSubscription* getSubscription(const AccountPublicAddress& acc) override;
  virtual std::vector<crypto::hash_t> getViewKeyKnownBlocks(const crypto::public_key_t& publicViewKey) override;

  void subscribeConsumerNotifications(const crypto::public_key_t& viewPublicKey, ITransfersSynchronizerObserver* observer);
  void unsubscribeConsumerNotifications(const crypto::public_key_t& viewPublicKey, ITransfersSynchronizerObserver* observer);

  // IStreamSerializable
  virtual void save(std::ostream& os) override;
  virtual void load(std::istream& in) override;

private:
  // map { view public key -> consumer }
  typedef std::unordered_map<crypto::public_key_t, std::unique_ptr<TransfersConsumer>> ConsumersContainer;
  ConsumersContainer m_consumers;

  typedef Tools::ObserverManager<ITransfersSynchronizerObserver> SubscribersNotifier;
  typedef std::unordered_map<crypto::public_key_t, std::unique_ptr<SubscribersNotifier>> SubscribersContainer;
  SubscribersContainer m_subscribers;

  // std::unordered_map<AccountAddress, std::unique_ptr<TransfersConsumer>> m_subscriptions;
  IBlockchainSynchronizer& m_sync;
  INode& m_node;
  const cryptonote::Currency& m_currency;

  virtual void onBlocksAdded(IBlockchainConsumer* consumer, const std::vector<crypto::hash_t>& blockHashes) override;
  virtual void onBlockchainDetach(IBlockchainConsumer* consumer, uint32_t blockIndex) override;
  virtual void onTransactionDeleteBegin(IBlockchainConsumer* consumer, crypto::hash_t transactionHash) override;
  virtual void onTransactionDeleteEnd(IBlockchainConsumer* consumer, crypto::hash_t transactionHash) override;
  virtual void onTransactionUpdated(IBlockchainConsumer* consumer, const crypto::hash_t& transactionHash,
    const std::vector<ITransfersContainer*>& containers) override;

  bool findViewKeyForConsumer(IBlockchainConsumer* consumer, crypto::public_key_t& viewKey) const;
  SubscribersContainer::const_iterator findSubscriberForConsumer(IBlockchainConsumer* consumer) const;
};

}
