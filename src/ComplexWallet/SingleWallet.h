// Copyright (c) 2011-2016 The Cryptonote developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <list>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>

#include "IWalletLegacy.h"
#include "INode.h"
#include "wallet/WalletErrors.h"
#include "wallet/WalletAsyncContextCounter.h"
#include "common/ObserverManager.h"
#include "cryptonote/core/TransactionExtra.h"
#include "cryptonote/core/CryptoNoteFormatUtils.h"
#include "cryptonote/core/Currency.h"
#include "wallet_legacy/WalletUserTransactionsCache.h"
#include "wallet_legacy/WalletUnconfirmedTransactions.h"

#include "wallet_legacy/WalletTransactionSender.h"
#include "wallet_legacy/WalletRequest.h"

#include "transfers/BlockchainSynchronizer.h"
#include "transfers/TransfersSynchronizer.h"


using namespace cryptonote;

namespace ComplexWallet {

class SyncStarter;

class SingleWallet : 
  public IWalletLegacy, 
  IBlockchainSynchronizerObserver,  
  ITransfersObserver {

public:
  SingleWallet(const cryptonote::Currency& currency, INode& node);
  virtual ~SingleWallet();

  virtual void addObserver(IWalletLegacyObserver* observer) override;
  virtual void removeObserver(IWalletLegacyObserver* observer) override;

  virtual void initAndGenerate(const std::string& password) override;
  virtual void initAndLoad(std::istream& source, const std::string& password) override;
  virtual void initWithKeys(const account_keys_t& accountKeys, const std::string& password) override;
  virtual void shutdown() override;
  virtual void reset() override;

  virtual void save(std::ostream& destination, bool saveDetailed = true, bool saveCache = true) override;

  virtual std::error_code changePassword(const std::string& oldPassword, const std::string& newPassword) override;

  virtual std::string getAddress() override;

  virtual uint64_t actualBalance() override;
  virtual uint64_t pendingBalance() override;

  virtual size_t getTransactionCount() override;
  virtual size_t getTransferCount() override;

  virtual TransactionId findTransactionByTransferId(TransferId transferId) override;

  virtual bool getTransaction(TransactionId transactionId, WalletLegacyTransaction& transaction) override;
  virtual bool getTransfer(TransferId transferId, WalletLegacyTransfer& transfer) override;

  virtual TransactionId sendTransaction(const WalletLegacyTransfer& transfer, uint64_t fee, const std::string& extra = "", uint64_t mixIn = 0, uint64_t unlockTimestamp = 0) override;
  virtual TransactionId sendTransaction(const std::vector<WalletLegacyTransfer>& transfers, uint64_t fee, const std::string& extra = "", uint64_t mixIn = 0, uint64_t unlockTimestamp = 0) override;
  virtual std::error_code cancelTransaction(size_t transactionId) override;

  virtual void getAccountKeys(account_keys_t& keys) override;

private:

  // IBlockchainSynchronizerObserver
  virtual void synchronizationProgressUpdated(uint32_t current, uint32_t total) override;
  virtual void synchronizationCompleted(std::error_code result) override;

  // ITransfersObserver
  virtual void onTransactionUpdated(ITransfersSubscription* object, const crypto::Hash& transactionHash) override;
  virtual void onTransactionDeleted(ITransfersSubscription* object, const crypto::Hash& transactionHash) override;

  void initSync();
  void throwIfNotInitialised();

  void doSave(std::ostream& destination, bool saveDetailed, bool saveCache);
  void doLoad(std::istream& source);

  void synchronizationCallback(WalletRequest::Callback callback, std::error_code ec);
  void sendTransactionCallback(WalletRequest::Callback callback, std::error_code ec);
  void notifyClients(std::deque<std::shared_ptr<WalletLegacyEvent> >& events);
  void notifyIfBalanceChanged();

  std::vector<TransactionId> deleteOutdatedUnconfirmedTransactions();

  enum WalletState
  {
    NOT_INITIALIZED = 0,
    INITIALIZED,
    LOADING,
    SAVING
  };

  WalletState m_state;
  std::mutex m_cacheMutex;
  cryptonote::Account m_account;
  std::string m_password;
  const cryptonote::Currency& m_currency;
  INode& m_node;
  bool m_isStopping;

  std::atomic<uint64_t> m_lastNotifiedActualBalance;
  std::atomic<uint64_t> m_lastNotifiedPendingBalance;

  BlockchainSynchronizer m_blockchainSync;
  TransfersSyncronizer m_transfersSync;
  ITransfersContainer* m_transferDetails;

  WalletUserTransactionsCache m_transactionsCache;
  std::unique_ptr<WalletTransactionSender> m_sender;

  WalletAsyncContextCounter m_asyncContextCounter;
  Tools::ObserverManager<cryptonote::IWalletLegacyObserver> m_observerManager;

  std::unique_ptr<SyncStarter> m_onInitSyncStarter;
};

} //namespace cryptonote
