// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.content_shell;

import android.app.Application;

import org.chromium.base.PathUtils;
import org.chromium.content.app.LibraryLoader;
import org.chromium.content.browser.ResourceExtractor;

/**
 * Entry point for the content shell application.  Handles initialization of information that needs
 * to be shared across the main activity and the sandbox services created.
 */
public class ContentShellApplication extends Application {

    private static final String NATIVE_LIBRARY = "content_shell_content_view";
    private static final String[] MANDATORY_PAK_FILES = new String[] {"content_shell.pak"};
    private static final String PRIVATE_DATA_DIRECTORY_SUFFIX = "content_shell";

    @Override
    public void onCreate() {
        super.onCreate();
        ResourceExtractor.setMandatoryPaksToExtract(MANDATORY_PAK_FILES);
        LibraryLoader.setLibraryToLoad(NATIVE_LIBRARY);
        PathUtils.setPrivateDataDirectorySuffix(PRIVATE_DATA_DIRECTORY_SUFFIX);
    }

}
