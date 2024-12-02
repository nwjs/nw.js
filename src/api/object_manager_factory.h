// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NWJS_OBJECT_MANAGER_FACTORY_H_
#define NWJS_OBJECT_MANAGER_FACTORY_H_

#include "base/memory/singleton.h"
#include "components/keyed_service/content/browser_context_keyed_service_factory.h"

namespace content {
class BrowserContext;
}

namespace nw {
class ObjectManager;

class ObjectManagerFactory : public BrowserContextKeyedServiceFactory {
 public:
  static ObjectManager* GetForBrowserContext(content::BrowserContext* context);

  static ObjectManagerFactory* GetInstance();

 protected:
  std::unique_ptr<KeyedService> BuildServiceInstanceForBrowserContext(
      content::BrowserContext* profile) const override;

 private:
  friend struct base::DefaultSingletonTraits<ObjectManagerFactory>;

  ObjectManagerFactory();
  ~ObjectManagerFactory() override;

  content::BrowserContext* GetBrowserContextToUse(
      content::BrowserContext* context) const override;
  bool ServiceIsCreatedWithBrowserContext() const override;
  bool ServiceIsNULLWhileTesting() const override;
};

}  // namespace extensions

#endif  // CHROME_BROWSER_EXTENSIONS_MENU_MANAGER_FACTORY_H_
