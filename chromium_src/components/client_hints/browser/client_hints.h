/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_COMPONENTS_CLIENT_HINTS_CLIENT_HINTS_H_
#define BRAVE_CHROMIUM_SRC_COMPONENTS_CLIENT_HINTS_CLIENT_HINTS_H_

#include "content/public/browser/client_hints_controller_delegate.h"

#define GetUserAgentMetadata           \
  GetUserAgentMetadata_ChromiumImpl(); \
  blink::UserAgentMetadata GetUserAgentMetadata

#include "src/components/client_hints/browser/client_hints.h"
#undef GetUserAgentMetadata

#endif  // BRAVE_CHROMIUM_SRC_COMPONENTS_CLIENT_HINTS_CLIENT_HINTS_H_
