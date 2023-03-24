/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_IOS_BROWSER_API_LEDGER_LEDGER_CLIENT_IOS_H_
#define BRAVE_IOS_BROWSER_API_LEDGER_LEDGER_CLIENT_IOS_H_

#import <Foundation/Foundation.h>
#include <string>
#include <vector>
#import "brave/components/brave_rewards/core/ledger_client.h"

@protocol LedgerClientBridge;

class LedgerClientIOS : public brave_rewards::core::LedgerClient {
 public:
  explicit LedgerClientIOS(id<LedgerClientBridge> bridge);
  ~LedgerClientIOS() override;

 private:
  __unsafe_unretained id<LedgerClientBridge> bridge_;

  void FetchFavIcon(const std::string& url,
                    const std::string& favicon_key,
                    brave_rewards::core::FetchIconCallback callback) override;
  void LoadLedgerState(brave_rewards::core::OnLoadCallback callback) override;
  void LoadPublisherState(
      brave_rewards::core::OnLoadCallback callback) override;
  void LoadURL(brave_rewards::mojom::UrlRequestPtr request,
               brave_rewards::core::LoadURLCallback callback) override;
  void Log(const char* file,
           const int line,
           const int verbose_level,
           const std::string& message) override;
  void OnPanelPublisherInfo(
      brave_rewards::mojom::Result result,
      brave_rewards::mojom::PublisherInfoPtr publisher_info,
      uint64_t windowId) override;
  void OnPublisherRegistryUpdated() override;
  void OnPublisherUpdated(const std::string& publisher_id) override;
  void OnReconcileComplete(
      brave_rewards::mojom::Result result,
      brave_rewards::mojom::ContributionInfoPtr contribution) override;
  void PublisherListNormalized(
      std::vector<brave_rewards::mojom::PublisherInfoPtr> list) override;
  std::string URIEncode(const std::string& value) override;
  void OnContributeUnverifiedPublishers(
      brave_rewards::mojom::Result result,
      const std::string& publisher_key,
      const std::string& publisher_name) override;
  void SetBooleanState(const std::string& name, bool value) override;
  bool GetBooleanState(const std::string& name) const override;
  void SetIntegerState(const std::string& name, int value) override;
  int GetIntegerState(const std::string& name) const override;
  void SetDoubleState(const std::string& name, double value) override;
  double GetDoubleState(const std::string& name) const override;
  void SetStringState(const std::string& name,
                      const std::string& value) override;
  std::string GetStringState(const std::string& name) const override;
  void SetInt64State(const std::string& name, int64_t value) override;
  int64_t GetInt64State(const std::string& name) const override;
  void SetUint64State(const std::string& name, uint64_t value) override;
  uint64_t GetUint64State(const std::string& name) const override;
  void SetValueState(const std::string& name, base::Value value) override;
  base::Value GetValueState(const std::string& name) const override;
  void SetTimeState(const std::string& name, base::Time time) override;
  base::Time GetTimeState(const std::string& name) const override;
  void ClearState(const std::string& name) override;
  std::string GetLegacyWallet() override;
  void ShowNotification(
      const std::string& type,
      const std::vector<std::string>& args,
      brave_rewards::core::LegacyResultCallback callback) override;
  bool GetBooleanOption(const std::string& name) const override;
  int GetIntegerOption(const std::string& name) const override;
  double GetDoubleOption(const std::string& name) const override;
  std::string GetStringOption(const std::string& name) const override;
  int64_t GetInt64Option(const std::string& name) const override;
  uint64_t GetUint64Option(const std::string& name) const override;
  brave_rewards::mojom::ClientInfoPtr GetClientInfo() override;
  void UnblindedTokensReady() override;
  void ReconcileStampReset() override;
  void RunDBTransaction(
      brave_rewards::mojom::DBTransactionPtr transaction,
      brave_rewards::core::RunDBTransactionCallback callback) override;
  void GetCreateScript(
      brave_rewards::core::GetCreateScriptCallback callback) override;
  void PendingContributionSaved(
      const brave_rewards::mojom::Result result) override;
  void ClearAllNotifications() override;
  void ExternalWalletConnected() const override;
  void ExternalWalletLoggedOut() const override;
  void ExternalWalletReconnected() const override;
  void DeleteLog(brave_rewards::core::LegacyResultCallback callback) override;
  absl::optional<std::string> EncryptString(const std::string& value) override;
  absl::optional<std::string> DecryptString(const std::string& value) override;
};

#endif  // BRAVE_IOS_BROWSER_API_LEDGER_LEDGER_CLIENT_IOS_H_
