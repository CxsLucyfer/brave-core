/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_ADS_CORE_INTERNAL_ACCOUNT_UTILITY_REDEEM_CONFIRMATION_REDEEM_OPTED_OUT_CONFIRMATION_H_
#define BRAVE_COMPONENTS_BRAVE_ADS_CORE_INTERNAL_ACCOUNT_UTILITY_REDEEM_CONFIRMATION_REDEEM_OPTED_OUT_CONFIRMATION_H_

#include "base/memory/weak_ptr.h"
#include "brave/components/brave_ads/common/interfaces/ads.mojom-forward.h"
#include "brave/components/brave_ads/core/internal/account/utility/redeem_confirmation/redeem_confirmation_delegate.h"

namespace brave_ads {

struct ConfirmationInfo;

// Self-destructs after calling |SuccessfullyRedeemedConfirmation| or
// |FailedToRedeemConfirmation|.
class RedeemOptedOutConfirmation final {
 public:
  RedeemOptedOutConfirmation(const RedeemOptedOutConfirmation&) = delete;
  RedeemOptedOutConfirmation& operator=(const RedeemOptedOutConfirmation&) =
      delete;

  RedeemOptedOutConfirmation(RedeemOptedOutConfirmation&&) noexcept = delete;
  RedeemOptedOutConfirmation& operator=(RedeemOptedOutConfirmation&&) noexcept =
      delete;

  ~RedeemOptedOutConfirmation();

  static void CreateAndRedeem(
      base::WeakPtr<RedeemConfirmationDelegate> delegate,
      const ConfirmationInfo& confirmation);

 private:
  explicit RedeemOptedOutConfirmation(
      base::WeakPtr<RedeemConfirmationDelegate> delegate);

  void Destroy();

  void Redeem(const ConfirmationInfo& confirmation);

  void CreateConfirmation(const ConfirmationInfo& confirmation);
  void OnCreateConfirmation(const ConfirmationInfo& confirmation,
                            const mojom::UrlResponseInfo& url_response);

  void SuccessfullyRedeemedConfirmation(const ConfirmationInfo& confirmation);
  void FailedToRedeemConfirmation(const ConfirmationInfo& confirmation,
                                  bool should_retry,
                                  bool should_backoff);

  base::WeakPtr<RedeemConfirmationDelegate> delegate_;
};

}  // namespace brave_ads

#endif  // BRAVE_COMPONENTS_BRAVE_ADS_CORE_INTERNAL_ACCOUNT_UTILITY_REDEEM_CONFIRMATION_REDEEM_OPTED_OUT_CONFIRMATION_H_
