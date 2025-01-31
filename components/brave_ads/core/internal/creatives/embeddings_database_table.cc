/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_ads/core/internal/creatives/embeddings_database_table.h"

#include <string>
#include <utility>
#include <vector>

#include "base/check.h"
#include "base/functional/bind.h"
#include "base/strings/string_util.h"
#include "brave/components/brave_ads/core/internal/ads_client_helper.h"
#include "brave/components/brave_ads/core/internal/common/database/database_bind_util.h"
#include "brave/components/brave_ads/core/internal/common/database/database_table_util.h"
#include "brave/components/brave_ads/core/internal/common/database/database_transaction_util.h"
#include "brave/components/brave_ads/core/internal/common/strings/string_conversions_util.h"

namespace brave_ads::database::table {

namespace {

constexpr char kTableName[] = "embeddings";

int BindParameters(mojom::DBCommandInfo* command,
                   const CreativeAdList& creative_ads) {
  DCHECK(command);

  int count = 0;

  int index = 0;
  for (const auto& creative_ad : creative_ads) {
    BindString(command, index++, creative_ad.creative_set_id);
    BindString(command, index++,
               base::ToLowerASCII(VectorToDelimitedString(
                   creative_ad.embedding, kEmbeddingStringDelimiter)));

    count++;
  }

  return count;
}

void MigrateToV27(mojom::DBTransactionInfo* transaction) {
  DCHECK(transaction);

  const std::string query =
      "CREATE TABLE IF NOT EXISTS embeddings"
      "(creative_set_id TEXT NOT NULL, "
      "embedding TEXT NOT NULL, "
      "PRIMARY KEY (creative_set_id), "
      "UNIQUE(creative_set_id) ON CONFLICT REPLACE)";

  mojom::DBCommandInfoPtr command = mojom::DBCommandInfo::New();
  command->type = mojom::DBCommandInfo::Type::EXECUTE;
  command->command = query;

  transaction->commands.push_back(std::move(command));
}

}  // namespace

void Embeddings::InsertOrUpdate(mojom::DBTransactionInfo* transaction,
                                const CreativeAdList& creative_ads) {
  DCHECK(transaction);

  if (creative_ads.empty()) {
    return;
  }

  mojom::DBCommandInfoPtr command = mojom::DBCommandInfo::New();
  command->type = mojom::DBCommandInfo::Type::RUN;
  command->command = BuildInsertOrUpdateQuery(command.get(), creative_ads);

  transaction->commands.push_back(std::move(command));
}

void Embeddings::Delete(ResultCallback callback) const {
  mojom::DBTransactionInfoPtr transaction = mojom::DBTransactionInfo::New();

  DeleteTable(transaction.get(), GetTableName());

  AdsClientHelper::GetInstance()->RunDBTransaction(
      std::move(transaction),
      base::BindOnce(&OnResultCallback, std::move(callback)));
}

std::string Embeddings::GetTableName() const {
  return kTableName;
}

void Embeddings::Migrate(mojom::DBTransactionInfo* transaction,
                         const int to_version) {
  DCHECK(transaction);

  switch (to_version) {
    case 27: {
      MigrateToV27(transaction);
      break;
    }

    default: {
      break;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////

std::string Embeddings::BuildInsertOrUpdateQuery(
    mojom::DBCommandInfo* command,
    const CreativeAdList& creative_ads) const {
  DCHECK(command);

  const int binded_parameters_count = BindParameters(command, creative_ads);

  return base::ReplaceStringPlaceholders(
      "INSERT OR REPLACE INTO $1 (creative_set_id, embedding) VALUES $2",
      {GetTableName(), BuildBindingParameterPlaceholders(
                           /*parameters_count*/ 2, binded_parameters_count)},
      nullptr);
}

}  // namespace brave_ads::database::table
