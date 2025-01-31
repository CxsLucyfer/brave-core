/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_REWARDS_CORE_DATABASE_DATABASE_RECURRING_TIP_H_
#define BRAVE_COMPONENTS_BRAVE_REWARDS_CORE_DATABASE_DATABASE_RECURRING_TIP_H_

#include <string>
#include <vector>

#include "base/functional/callback_forward.h"
#include "base/time/time.h"
#include "brave/components/brave_rewards/core/database/database_table.h"

namespace ledger {
namespace database {

class DatabaseRecurringTip : public DatabaseTable {
 public:
  explicit DatabaseRecurringTip(LedgerImpl& ledger);
  ~DatabaseRecurringTip() override;

  // DEPRECATED
  void InsertOrUpdate(mojom::RecurringTipPtr info,
                      ledger::LegacyResultCallback callback);

  void InsertOrUpdate(const std::string& publisher_id,
                      double amount,
                      base::OnceCallback<void(bool)> callback);

  void AdvanceMonthlyContributionDates(
      const std::vector<std::string>& publisher_ids,
      base::OnceCallback<void(bool)> callback);

  void GetNextMonthlyContributionTime(
      base::OnceCallback<void(absl::optional<base::Time>)> callback);

  void GetAllRecords(ledger::GetRecurringTipsCallback callback);

  void DeleteRecord(const std::string& publisher_key,
                    ledger::LegacyResultCallback callback);

 private:
  void OnGetAllRecords(mojom::DBCommandResponsePtr response,
                       ledger::GetRecurringTipsCallback callback);
};

}  // namespace database
}  // namespace ledger

#endif  // BRAVE_COMPONENTS_BRAVE_REWARDS_CORE_DATABASE_DATABASE_RECURRING_TIP_H_
