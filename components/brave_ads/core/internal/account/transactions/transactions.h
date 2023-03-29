/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_ADS_CORE_INTERNAL_ACCOUNT_TRANSACTIONS_TRANSACTIONS_H_
#define BRAVE_COMPONENTS_BRAVE_ADS_CORE_INTERNAL_ACCOUNT_TRANSACTIONS_TRANSACTIONS_H_

#include <string>

#include "base/functional/callback_forward.h"
#include "brave/components/brave_ads/core/internal/account/transactions/transaction_info.h"

namespace base {
class Time;
}  // namespace base

namespace brave_ads {

class AdType;
class ConfirmationType;

namespace transactions {

using AddCallback =
    base::OnceCallback<void(const bool, const TransactionInfo& transaction)>;

using GetCallback =
    base::OnceCallback<void(const bool, const TransactionList&)>;

using RemoveAllCallback = base::OnceCallback<void(const bool)>;

TransactionInfo Get(const std::string& creative_instance_id,
                    const AdType& ad_type,
                    const ConfirmationType& confirmation_type,
                    double value);

TransactionInfo Add(const std::string& creative_instance_id,
                    const AdType& ad_type,
                    const ConfirmationType& confirmation_type,
                    double value,
                    AddCallback callback);

void GetForDateRange(base::Time from_time,
                     base::Time to_time,
                     GetCallback callback);

void RemoveAll(RemoveAllCallback callback);

}  // namespace transactions
}  // namespace brave_ads

#endif  // BRAVE_COMPONENTS_BRAVE_ADS_CORE_INTERNAL_ACCOUNT_TRANSACTIONS_TRANSACTIONS_H_
