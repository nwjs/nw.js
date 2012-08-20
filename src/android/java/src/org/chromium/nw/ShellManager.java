// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.content_shell;

import android.content.Context;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.SurfaceHolder;
import android.widget.FrameLayout;

import org.chromium.base.CalledByNative;
import org.chromium.base.JNINamespace;

/**
 * Container and generator of ShellViews.
 */
@JNINamespace("content")
public class ShellManager extends FrameLayout {

    private Shell mActiveShell;

    private String mStartupUrl = ContentShellActivity.DEFAULT_SHELL_URL;

    // The target for all content rendering.
    private SurfaceView mSurfaceView;

    /**
     * Constructor for inflating via XML.
     */
    public ShellManager(Context context, AttributeSet attrs) {
        super(context, attrs);
        nativeInit(this);

        mSurfaceView = new SurfaceView(context);
        mSurfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                nativeSurfaceSetSize(width, height);
            }

            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                nativeSurfaceCreated(holder.getSurface());
                mActiveShell.loadUrl(mStartupUrl);
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                nativeSurfaceDestroyed();
            }
        });
    }

    /**
     * Sets the startup URL for new shell windows.
     */
    public void setStartupUrl(String url) {
        mStartupUrl = url;
    }

    /**
     * @return The currently visible shell view or null if one is not showing.
     */
    protected Shell getActiveShell() {
        return mActiveShell;
    }

    /**
     * Creates a new shell pointing to the specified URL.
     * @param url The URL the shell should load upon creation.
     */
    public void launchShell(String url) {
        nativeLaunchShell(url);
    }

    @SuppressWarnings("unused")
    @CalledByNative
    private Object createShell() {
        LayoutInflater inflater =
                (LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        Shell shellView = (Shell) inflater.inflate(R.layout.shell_view, null);
        shellView.setSurfaceView(mSurfaceView);

        removeAllViews();
        if (mActiveShell != null && mActiveShell.getContentView() != null) {
            mActiveShell.getContentView().onHide();
        }

        addView(shellView, new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.MATCH_PARENT));
        mActiveShell = shellView;
        if (mActiveShell.getContentView() != null) mActiveShell.getContentView().onShow();

        return shellView;
    }

    private static native void nativeInit(Object shellManagerInstance);
    private static native void nativeLaunchShell(String url);
    private static native void nativeSurfaceCreated(Surface surface);
    private static native void nativeSurfaceDestroyed();
    private static native void nativeSurfaceSetSize(int width, int height);
}
