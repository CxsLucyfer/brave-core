/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <string>
#include <utility>

#include "base/strings/strcat.h"
#include "base/test/task_environment.h"
#include "brave/components/brave_rewards/core/buildflags.h"
#include "brave/components/brave_rewards/core/common/random_util.h"
#include "brave/components/brave_rewards/core/gemini/gemini.h"
#include "brave/components/brave_rewards/core/gemini/gemini_util.h"
#include "brave/components/brave_rewards/core/global_constants.h"
#include "brave/components/brave_rewards/core/ledger_callbacks.h"
#include "brave/components/brave_rewards/core/ledger_client_mock.h"
#include "brave/components/brave_rewards/core/ledger_impl_mock.h"
#include "brave/components/brave_rewards/core/state/state_keys.h"
#include "brave/components/brave_rewards/core/test/test_ledger_client.h"
#include "testing/gmock/include/gmock/gmock.h"

// npm run test -- brave_unit_tests --filter=GeminiUtilTest.*

using ::testing::_;

namespace ledger {
namespace gemini {

class GeminiUtilTest : public testing::Test {};

TEST_F(GeminiUtilTest, GetClientId) {
  // production
  ledger::_environment = mojom::Environment::PRODUCTION;
  std::string result = gemini::GetClientId();
  ASSERT_EQ(result, BUILDFLAG(GEMINI_WALLET_CLIENT_ID));

  // staging
  ledger::_environment = mojom::Environment::STAGING;
  result = gemini::GetClientId();
  ASSERT_EQ(result, BUILDFLAG(GEMINI_WALLET_STAGING_CLIENT_ID));
}

TEST_F(GeminiUtilTest, GetClientSecret) {
  // production
  ledger::_environment = mojom::Environment::PRODUCTION;
  std::string result = gemini::GetClientSecret();
  ASSERT_EQ(result, BUILDFLAG(GEMINI_WALLET_CLIENT_SECRET));

  // staging
  ledger::_environment = mojom::Environment::STAGING;
  result = gemini::GetClientSecret();
  ASSERT_EQ(result, BUILDFLAG(GEMINI_WALLET_STAGING_CLIENT_SECRET));
}

TEST_F(GeminiUtilTest, GetFeeAddress) {
  // production
  ledger::_environment = mojom::Environment::PRODUCTION;
  std::string result = gemini::GetFeeAddress();
  ASSERT_EQ(result, kFeeAddressProduction);

  // staging
  ledger::_environment = mojom::Environment::STAGING;
  result = gemini::GetFeeAddress();
  ASSERT_EQ(result, kFeeAddressStaging);
}

TEST_F(GeminiUtilTest, GetLoginUrl) {
  // production
  ledger::_environment = mojom::Environment::PRODUCTION;
  std::string result = gemini::GetLoginUrl("my-state");
  ASSERT_EQ(result, base::StrCat({BUILDFLAG(GEMINI_OAUTH_URL),
                                  "/auth"
                                  "?client_id=",
                                  BUILDFLAG(GEMINI_WALLET_CLIENT_ID),
                                  "&scope="
                                  "balances:read,"
                                  "history:read,"
                                  "crypto:send,"
                                  "account:read,"
                                  "payments:create,"
                                  "payments:send,"
                                  "&redirect_uri=rewards://gemini/authorization"
                                  "&state=my-state"
                                  "&response_type=code"}));

  // staging
  ledger::_environment = mojom::Environment::STAGING;
  result = gemini::GetLoginUrl("my-state");
  ASSERT_EQ(result, base::StrCat({BUILDFLAG(GEMINI_OAUTH_STAGING_URL),
                                  "/auth"
                                  "?client_id=",
                                  BUILDFLAG(GEMINI_WALLET_STAGING_CLIENT_ID),
                                  "&scope="
                                  "balances:read,"
                                  "history:read,"
                                  "crypto:send,"
                                  "account:read,"
                                  "payments:create,"
                                  "payments:send,"
                                  "&redirect_uri=rewards://gemini/authorization"
                                  "&state=my-state"
                                  "&response_type=code"}));
}

TEST_F(GeminiUtilTest, GetActivityUrl) {
  // production
  ledger::_environment = mojom::Environment::PRODUCTION;
  std::string result = gemini::GetActivityUrl();
  ASSERT_EQ(result, base::StrCat({BUILDFLAG(GEMINI_OAUTH_URL), "/balances"}));

  // staging
  ledger::_environment = mojom::Environment::STAGING;
  result = gemini::GetActivityUrl();
  ASSERT_EQ(result,
            base::StrCat({BUILDFLAG(GEMINI_OAUTH_STAGING_URL), "/balances"}));
}

TEST_F(GeminiUtilTest, GetWallet) {
  base::test::TaskEnvironment task_environment_;
  MockLedgerImpl mock_ledger_impl_;

  // no wallet
  ON_CALL(*mock_ledger_impl_.mock_client(),
          GetStringState(state::kWalletGemini, _))
      .WillByDefault([](const std::string&, auto callback) {
        std::move(callback).Run("");
      });
  auto result = mock_ledger_impl_.gemini()->GetWallet();
  ASSERT_TRUE(!result);

  ON_CALL(*mock_ledger_impl_.mock_client(),
          GetStringState(state::kWalletGemini, _))
      .WillByDefault([](const std::string&, auto callback) {
        std::string wallet = FakeEncryption::Base64EncryptString(R"({
          "account_url": "https://exchange.sandbox.gemini.com",
          "address": "2323dff2ba-d0d1-4dfw-8e56-a2605bcaf4af",
          "fees": {},
          "login_url": "https://exchange.sandbox.gemini.com/auth",
          "one_time_string": "1F747AE0A708E47ED7E650BF1856B5A4EF7E36833BDB115",
          "status": 2,
          "token": "4c80232r219c30cdf112208890a32c7e00",
          "user_name": "test"
        })");
        std::move(callback).Run(std::move(wallet));
      });

  // Gemini wallet
  result = mock_ledger_impl_.gemini()->GetWallet();
  ASSERT_TRUE(result);
  ASSERT_EQ(result->address, "2323dff2ba-d0d1-4dfw-8e56-a2605bcaf4af");
  ASSERT_EQ(result->user_name, "test");
  ASSERT_EQ(result->token, "4c80232r219c30cdf112208890a32c7e00");
  ASSERT_EQ(result->status, mojom::WalletStatus::kConnected);

  task_environment_.RunUntilIdle();
}

TEST_F(GeminiUtilTest, GenerateRandomHexString) {
  // string for testing
  ledger::is_testing = true;
  auto result = ledger::util::GenerateRandomHexString();
  ASSERT_EQ(result, "123456789");

  // random string
  ledger::is_testing = false;
  ledger::_environment = mojom::Environment::STAGING;
  result = ledger::util::GenerateRandomHexString();
  ASSERT_EQ(result.length(), 64u);
}

TEST_F(GeminiUtilTest, GenerateLinks) {
  ledger::_environment = mojom::Environment::STAGING;

  auto wallet = mojom::ExternalWallet::New();
  wallet->address = "123123123124234234234";
  wallet->one_time_string = "aaabbbccc";

  // Not connected
  wallet->status = mojom::WalletStatus::kNotConnected;
  auto result = gemini::GenerateLinks(wallet->Clone());
  ASSERT_EQ(result->login_url,
            base::StrCat(
                {BUILDFLAG(GEMINI_OAUTH_STAGING_URL),
                 "/auth?client_id=", BUILDFLAG(GEMINI_WALLET_STAGING_CLIENT_ID),
                 "&scope="
                 "balances:read,"
                 "history:read,"
                 "crypto:send,"
                 "account:read,"
                 "payments:create,"
                 "payments:send,"
                 "&redirect_uri=rewards://gemini/authorization"
                 "&state=aaabbbccc"
                 "&response_type=code"}));
  ASSERT_EQ(result->account_url, BUILDFLAG(GEMINI_OAUTH_STAGING_URL));

  // Connected
  wallet->status = mojom::WalletStatus::kConnected;
  result = gemini::GenerateLinks(wallet->Clone());
  ASSERT_EQ(result->login_url,
            base::StrCat(
                {BUILDFLAG(GEMINI_OAUTH_STAGING_URL),
                 "/auth?client_id=", BUILDFLAG(GEMINI_WALLET_STAGING_CLIENT_ID),
                 "&scope="
                 "balances:read,"
                 "history:read,"
                 "crypto:send,"
                 "account:read,"
                 "payments:create,"
                 "payments:send,"
                 "&redirect_uri=rewards://gemini/authorization"
                 "&state=aaabbbccc"
                 "&response_type=code"}));
  ASSERT_EQ(result->account_url, BUILDFLAG(GEMINI_OAUTH_STAGING_URL));

  // Logged out
  wallet->status = mojom::WalletStatus::kLoggedOut;
  result = gemini::GenerateLinks(wallet->Clone());
  ASSERT_EQ(result->login_url,
            base::StrCat(
                {BUILDFLAG(GEMINI_OAUTH_STAGING_URL),
                 "/auth?client_id=", BUILDFLAG(GEMINI_WALLET_STAGING_CLIENT_ID),
                 "&scope="
                 "balances:read,"
                 "history:read,"
                 "crypto:send,"
                 "account:read,"
                 "payments:create,"
                 "payments:send,"
                 "&redirect_uri=rewards://gemini/authorization"
                 "&state=aaabbbccc"
                 "&response_type=code"}));
  ASSERT_EQ(result->account_url, BUILDFLAG(GEMINI_OAUTH_STAGING_URL));
}

}  // namespace gemini
}  // namespace ledger
