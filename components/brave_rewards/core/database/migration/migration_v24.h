/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_REWARDS_CORE_DATABASE_MIGRATION_MIGRATION_V24_H_
#define BRAVE_COMPONENTS_BRAVE_REWARDS_CORE_DATABASE_MIGRATION_MIGRATION_V24_H_

namespace brave_rewards::core {
namespace database {
namespace migration {

const char v24[] = R"(
  ALTER TABLE contribution_queue ADD completed_at TIMESTAMP NOT NULL DEFAULT 0;
)";

}  // namespace migration
}  // namespace database
}  // namespace brave_rewards::core

#endif  // BRAVE_COMPONENTS_BRAVE_REWARDS_CORE_DATABASE_MIGRATION_MIGRATION_V24_H_
