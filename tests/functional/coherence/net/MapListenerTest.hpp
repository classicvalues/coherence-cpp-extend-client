/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/net/CacheFactory.hpp"

#include "common/TestMapListener.hpp"
#include "common/TestUtils.hpp"

using namespace coherence::lang;

using coherence::net::CacheFactory;

using common::test::TestMapListener;

/**
* MapListener Test Suite.
*/
class MapListenerTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test defining a MapListener in the config file.
        */
        void testMapListener()
            {
            NamedCache::Handle hCache = ensureCleanCache("local-test", "defaultconfig/extend-maplistener-cache-config.xml");

            // create a key, and value
            String::View vsKey   = "hello";
            String::View vsValue = "grid";

            // insert the pair into the cache
            hCache->put(vsKey, vsValue);
            Thread::sleep(1000);

            // read back the value, casting to the expected value type
            String::View vsGet = cast<String::View>(hCache->get(vsKey));

            TS_ASSERT(TestMapListener::getEvent()->equals("insert"));
            TS_ASSERT(vsGet->equals("grid"));

            // update entry
            String::View vsValue2 = "newgrid";
            hCache->put(vsKey, vsValue2);
            Thread::sleep(1000);

            // read back the value, casting to the expected value type
            vsGet = cast<String::View>(hCache->get(vsKey));

            TS_ASSERT(TestMapListener::getEvent()->equals("update"));
            TS_ASSERT(vsGet->equals("newgrid"));

            // delete entry
            hCache->remove(vsKey);
            Thread::sleep(1000);

            TS_ASSERT(TestMapListener::getEvent()->equals("delete"));

            CacheFactory::destroyCache(hCache);

            setDefaultCacheConfigure();
            }

        /**
        * Test defining a MapListener in the config file, with parameters.
        */
        void testMapListenerWithParameters()
            {
            String::View       vsCacheName = String::create("configured-test");
            NamedCache::Handle hCache      = ensureCleanCache(vsCacheName, "defaultconfig/extend-maplistener-cache-config.xml");

            // create a key, and value
            String::View vsKey   = "testing_map_listener_config";
            String::View vsValue = vsCacheName;

            // insert the pair into the cache
            hCache->put(vsKey, vsValue);
            Thread::sleep(1000);

            // read back the value, casting to the expected value type
            String::View vsGet = cast<String::View>(hCache->get(vsKey));
            TS_ASSERT(vsGet->equals(vsCacheName));

            TS_ASSERT(TestMapListener::getEvent()->equals(vsCacheName));

            // test that listener still registered after cache truncate
            hCache->truncate();

            hCache->put(vsKey, vsValue);
            Thread::sleep(1000);
           
            TS_ASSERT(TestMapListener::getEvent()->equals(vsCacheName));
             
            CacheFactory::destroyCache(hCache);

            setDefaultCacheConfigure();
            }

        /**
        * Test events generated by NearCache priming from
        * {@link com.tangosol.net.NamedCache#get(Object)} should be ignored.
        * See COH-18376.
        */
        void testNearCacheMapListener()
            {
            NamedCache::Handle hCache = ensureCleanCache("near-cache-test", "defaultconfig/extend-maplistener-cache-config.xml");

            // create a key, and value
            String::View            vsKey     = "key";
            String::View            vsValue   = "value";
            TestMapListener::Handle hListener = TestMapListener::create("near-cache-test");

            hCache->addMapListener(hListener);

            // insert the pair into the cache
            hCache->put(vsKey, vsValue);
            Thread::sleep(1000);

            TS_ASSERT(hListener->getEvent()->equals("insert"));

            // update entry
            String::View vsValue2 = "value1";
            hCache->put(vsKey, vsValue2);
            Thread::sleep(1000);

            TS_ASSERT(hListener->getEvent()->equals("update"));

            hListener->clearEvent();

            bool fResult = hListener->getMapEvent() == NULL;
            TS_ASSERT(fResult);

            // read back the value, casting to the expected value type
            String::View vsGet = cast<String::View>(hCache->get(vsKey));
            Thread::sleep(1000);

            fResult = hListener->getMapEvent() == NULL;
            TS_ASSERT(fResult);

            // delete entry
            hCache->remove(vsKey);
            Thread::sleep(1000);

            TS_ASSERT(hListener->getEvent()->equals("delete"));

            CacheFactory::destroyCache(hCache);

            setDefaultCacheConfigure();
            }

        /**
        * Clean up after the tests - Sunpro compiler does not like cxxtest
        * createSuite() and destroySuite() methods so need to do it this way
        */
        void testCleanup()
            {
            CacheFactory::shutdown();
            }
    };
