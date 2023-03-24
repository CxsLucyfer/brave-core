/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "brave/components/brave_rewards/core/endpoint/promotion/post_suggestions_claim/post_suggestions_claim.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "base/test/task_environment.h"
#include "brave/components/brave_rewards/core/ledger.h"
#include "brave/components/brave_rewards/core/ledger_client_mock.h"
#include "brave/components/brave_rewards/core/ledger_impl_mock.h"
#include "brave/components/brave_rewards/core/state/state_keys.h"
#include "net/http/http_status_code.h"
#include "testing/gtest/include/gtest/gtest.h"

// npm run test -- brave_unit_tests --filter=PostSuggestionsClaimTest.*

using ::testing::_;
using ::testing::Invoke;

namespace brave_rewards::core {
namespace endpoint {
namespace promotion {

class PostSuggestionsClaimTest : public testing::Test {
 private:
  base::test::TaskEnvironment scoped_task_environment_;

 protected:
  std::unique_ptr<MockLedgerClient> mock_ledger_client_;
  std::unique_ptr<MockLedgerImpl> mock_ledger_impl_;
  std::unique_ptr<PostSuggestionsClaim> claim_;
  std::unique_ptr<credential::CredentialsRedeem> redeem_;

  PostSuggestionsClaimTest() {
    mock_ledger_client_ = std::make_unique<MockLedgerClient>();
    mock_ledger_impl_ =
        std::make_unique<MockLedgerImpl>(mock_ledger_client_.get());
    claim_ = std::make_unique<PostSuggestionsClaim>(mock_ledger_impl_.get());

    mojom::UnblindedToken token;
    token.token_value =
        "s1OrSZUvo/33u3Y866mQaG/"
        "b6d94TqMThLal4+DSX4UrR4jT+GtTErim+"
        "FtEyZ7nebNGRoUDxObiUni9u8BB0DIT2aya6rYWko64IrXJWpbf0SVHnQFVYNyX64NjW9R"
        "6";  // NOLINT
    token.public_key = "dvpysTSiJdZUPihius7pvGOfngRWfDiIbrowykgMi1I=";
    redeem_ = std::make_unique<credential::CredentialsRedeem>();
    redeem_->publisher_key = "brave.com";
    redeem_->type = mojom::RewardsType::ONE_TIME_TIP;
    redeem_->processor = mojom::ContributionProcessor::BRAVE_TOKENS;
    redeem_->token_list = {token};
    redeem_->order_id = "c4645786-052f-402f-8593-56af2f7a21ce";
    redeem_->contribution_id = "83b3b77b-e7c3-455b-adda-e476fa0656d2";
  }

  void SetUp() override {
    const std::string wallet = R"({
      "payment_id":"fa5dea51-6af4-44ca-801b-07b6df3dcfe4",
      "recovery_seed":"AN6DLuI2iZzzDxpzywf+IKmK1nzFRarNswbaIDI3pQg="
    })";
    ON_CALL(*mock_ledger_client_, GetStringState(state::kWalletBrave))
        .WillByDefault(testing::Return(wallet));
  }
};

TEST_F(PostSuggestionsClaimTest, ServerOK) {
  ON_CALL(*mock_ledger_client_, LoadURL(_, _))
      .WillByDefault(
          Invoke([](mojom::UrlRequestPtr request, LoadURLCallback callback) {
            mojom::UrlResponse response;
            response.status_code = 200;
            response.url = request->url;
            response.body = R"(
              {"drainId": "1af0bf71-c81c-4b18-9188-a0d3c4a1b53b"}
            )";
            std::move(callback).Run(response);
          }));

  claim_->Request(
      *redeem_, base::BindOnce([](mojom::Result result, std::string drain_id) {
        EXPECT_EQ(result, mojom::Result::LEDGER_OK);
        EXPECT_EQ(drain_id, "1af0bf71-c81c-4b18-9188-a0d3c4a1b53b");
      }));
}

TEST_F(PostSuggestionsClaimTest, ServerNeedsRetry) {
  ON_CALL(*mock_ledger_client_, LoadURL(_, _))
      .WillByDefault(
          Invoke([](mojom::UrlRequestPtr request, LoadURLCallback callback) {
            mojom::UrlResponse response;
            response.status_code = 200;
            response.url = request->url;
            response.body = "";
            std::move(callback).Run(response);
          }));

  claim_->Request(
      *redeem_, base::BindOnce([](mojom::Result result, std::string drain_id) {
        EXPECT_EQ(result, mojom::Result::LEDGER_ERROR);
        EXPECT_EQ(drain_id, "");
      }));
}

TEST_F(PostSuggestionsClaimTest, ServerError400) {
  ON_CALL(*mock_ledger_client_, LoadURL(_, _))
      .WillByDefault(
          Invoke([](mojom::UrlRequestPtr request, LoadURLCallback callback) {
            mojom::UrlResponse response;
            response.status_code = 400;
            response.url = request->url;
            response.body = "";
            std::move(callback).Run(response);
          }));

  claim_->Request(
      *redeem_, base::BindOnce([](mojom::Result result, std::string drain_id) {
        EXPECT_EQ(result, mojom::Result::LEDGER_ERROR);
        EXPECT_EQ(drain_id, "");
      }));
}

TEST_F(PostSuggestionsClaimTest, ServerError500) {
  ON_CALL(*mock_ledger_client_, LoadURL(_, _))
      .WillByDefault(
          Invoke([](mojom::UrlRequestPtr request, LoadURLCallback callback) {
            mojom::UrlResponse response;
            response.status_code = 500;
            response.url = request->url;
            response.body = "";
            std::move(callback).Run(response);
          }));

  claim_->Request(
      *redeem_, base::BindOnce([](mojom::Result result, std::string drain_id) {
        EXPECT_EQ(result, mojom::Result::LEDGER_ERROR);
        EXPECT_EQ(drain_id, "");
      }));
}

}  // namespace promotion
}  // namespace endpoint
}  // namespace brave_rewards::core
