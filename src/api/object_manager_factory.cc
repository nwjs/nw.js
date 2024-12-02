// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/api/object_manager_factory.h"

#include "content/nw/src/api/object_manager.h"
#include "components/keyed_service/content/browser_context_dependency_manager.h"
#include "extensions/browser/extension_system.h"
#include "extensions/browser/extension_system_provider.h"
#include "extensions/browser/extensions_browser_client.h"

namespace nw {

// static
ObjectManager* ObjectManagerFactory::GetForBrowserContext(
    content::BrowserContext* context) {
  return static_cast<ObjectManager*>(
      GetInstance()->GetServiceForBrowserContext(context, true));
}

// static
ObjectManagerFactory* ObjectManagerFactory::GetInstance() {
  return base::Singleton<ObjectManagerFactory>::get();
}

ObjectManagerFactory::ObjectManagerFactory()
    : BrowserContextKeyedServiceFactory(
        "ObjectManager",
        BrowserContextDependencyManager::GetInstance()) {
  DependsOn(extensions::ExtensionsBrowserClient::Get()->GetExtensionSystemFactory());
}

ObjectManagerFactory::~ObjectManagerFactory() {}

std::unique_ptr<KeyedService>
ObjectManagerFactory::BuildServiceInstanceForBrowserContext(
    content::BrowserContext* context) const {
  return std::make_unique<ObjectManager>(context);
}

content::BrowserContext* ObjectManagerFactory::GetBrowserContextToUse(
    content::BrowserContext* context) const {
  return extensions::ExtensionsBrowserClient::Get()->GetOriginalContext(context);
}

bool ObjectManagerFactory::ServiceIsCreatedWithBrowserContext() const {
  return true;
}

bool ObjectManagerFactory::ServiceIsNULLWhileTesting() const {
  return true;
}

}  // namespace nw
