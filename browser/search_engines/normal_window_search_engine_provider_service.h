/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_SEARCH_ENGINES_NORMAL_WINDOW_SEARCH_ENGINE_PROVIDER_SERVICE_H_
#define BRAVE_BROWSER_SEARCH_ENGINES_NORMAL_WINDOW_SEARCH_ENGINE_PROVIDER_SERVICE_H_

#include "base/callback_list.h"
#include "base/memory/raw_ptr.h"
#include "base/scoped_observation.h"
#include "components/keyed_service/core/keyed_service.h"
#include "components/prefs/pref_member.h"
#include "components/search_engines/template_url_service_observer.h"

class Profile;
class TemplateURLService;

// Set default prefs for private search provider as it's stored in normal
// profile. And update TemplateURLData for private search provider whenever
// user changes private window's search provider. This cached TemplateURLData
// can be used when default provider list updated. When list is updated,
// new default provider list doesn't include previous default provider.
// In this situation, previous default provider should be default one with
// new list. To do that, cached TemplateURLData can be added to
// TemplateURLService.
class NormalWindowSearchEngineProviderService
    : public KeyedService,
      public TemplateURLServiceObserver {
 public:
  explicit NormalWindowSearchEngineProviderService(Profile* profile);
  ~NormalWindowSearchEngineProviderService() override;

  NormalWindowSearchEngineProviderService(
      const NormalWindowSearchEngineProviderService&) = delete;
  NormalWindowSearchEngineProviderService& operator=(
      const NormalWindowSearchEngineProviderService&) = delete;

 private:
  // KeyedService overrides:
  void Shutdown() override;

  // TemplateURLServiceObserver overrides:
  void OnTemplateURLServiceChanged() override;
  void OnTemplateURLServiceShuttingDown() override;

  void OnTemplateURLServiceLoaded(Profile* profile);
  void PrepareInitialPrivateSearchProvider();
  void OnPreferenceChanged();
  void UpdateSearchSuggestionsDefaultValue();

  raw_ptr<Profile> profile_ = nullptr;
  StringPrefMember private_search_provider_guid_;
  base::CallbackListSubscription template_url_service_subscription_;
  base::ScopedObservation<TemplateURLService, TemplateURLServiceObserver>
      observation_{this};
};

#endif  // BRAVE_BROWSER_SEARCH_ENGINES_NORMAL_WINDOW_SEARCH_ENGINE_PROVIDER_SERVICE_H_
