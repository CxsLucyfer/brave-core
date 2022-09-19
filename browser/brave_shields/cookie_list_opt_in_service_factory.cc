/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/brave_shields/cookie_list_opt_in_service_factory.h"

#include "brave/components/brave_shields/browser/cookie_list_opt_in_service.h"
#include "chrome/browser/profiles/incognito_helpers.h"
#include "components/keyed_service/content/browser_context_dependency_manager.h"

namespace brave_shields {

// static
CookieListOptInServiceFactory* CookieListOptInServiceFactory::GetInstance() {
  return base::Singleton<CookieListOptInServiceFactory>::get();
}

// static
mojo::PendingRemote<mojom::CookieListOptInPageAndroidHandler>
CookieListOptInServiceFactory::GetForContext(content::BrowserContext* context) {
  return static_cast<CookieListOptInService*>(
             GetInstance()->GetServiceForBrowserContext(context, true))
      ->MakeRemote();
}

// static
CookieListOptInService* CookieListOptInServiceFactory::GetServiceForContext(
    content::BrowserContext* context) {
  return static_cast<CookieListOptInService*>(
      GetInstance()->GetServiceForBrowserContext(context, true));
}

// static
void CookieListOptInServiceFactory::BindForContext(
    content::BrowserContext* context,
    mojo::PendingReceiver<mojom::CookieListOptInPageAndroidHandler> receiver) {
  auto* keyring_service =
      CookieListOptInServiceFactory::GetServiceForContext(context);
  if (keyring_service) {
    keyring_service->Bind(std::move(receiver));
  }
}

CookieListOptInServiceFactory::CookieListOptInServiceFactory()
    : BrowserContextKeyedServiceFactory(
          "CookieListOptInService",
          BrowserContextDependencyManager::GetInstance()) {}

CookieListOptInServiceFactory::~CookieListOptInServiceFactory() = default;

KeyedService* CookieListOptInServiceFactory::BuildServiceInstanceFor(
    content::BrowserContext* context) const {
  return new CookieListOptInService();
}

content::BrowserContext* CookieListOptInServiceFactory::GetBrowserContextToUse(
    content::BrowserContext* context) const {
  return chrome::GetBrowserContextRedirectedInIncognito(context);
}

}  // namespace brave_shields