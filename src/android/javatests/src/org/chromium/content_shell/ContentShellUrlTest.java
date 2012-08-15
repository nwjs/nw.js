// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.content_shell;

import android.app.Activity;
import android.test.ActivityInstrumentationTestCase2;
import android.test.suitebuilder.annotation.SmallTest;

import org.chromium.base.test.DisabledTest;

/**
 * Example test that just starts the content shell.
 */
public class ContentShellUrlTest extends ContentShellTestBase {
    // URL used for base tests.
    private static final String URL = "data:text";

    @DisabledTest
    public void testBaseStartup() throws Exception {
        ContentShellActivity activity = launchContentShellWithUrl(URL);

        // Make sure the activity was created as expected.
        assertNotNull(activity);

        // Make sure that the URL is set as expected.
        assertEquals(URL, activity.getActiveShell().getContentView().getUrl());
    }
}
