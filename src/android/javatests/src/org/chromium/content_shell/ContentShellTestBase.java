// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.content_shell;

import android.content.ComponentName;
import android.content.Intent;
import android.net.Uri;
import android.test.ActivityInstrumentationTestCase2;

/**
 * Base test class for all ContentShell based tests.
 */
public class ContentShellTestBase extends ActivityInstrumentationTestCase2<ContentShellActivity> {

    public ContentShellTestBase() {
        super(ContentShellActivity.class);
    }

    /**
     * Starts the ContentShell activity and loads the given URL.
     */
    protected ContentShellActivity launchContentShellWithUrl(String url) {
        Intent intent = new Intent(Intent.ACTION_MAIN);
        intent.addCategory(Intent.CATEGORY_LAUNCHER);
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        intent.setData(Uri.parse(url));
        intent.setComponent(new ComponentName(getInstrumentation().getTargetContext(),
              ContentShellActivity.class));
        return (ContentShellActivity) getInstrumentation().startActivitySync(intent);
    }
}
