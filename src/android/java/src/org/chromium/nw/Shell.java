// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.content_shell;

import android.content.Context;
import android.graphics.drawable.ClipDrawable;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;

import org.chromium.base.CalledByNative;
import org.chromium.base.JNINamespace;
import org.chromium.content.browser.ContentView;

/**
 * Container for the various UI components that make up a shell window.
 */
@JNINamespace("content")
public class Shell extends LinearLayout {

    private static final long COMPLETED_PROGRESS_TIMEOUT_MS = 200;

    private Runnable mClearProgressRunnable = new Runnable() {
        @Override
        public void run() {
            mProgressDrawable.setLevel(0);
        }
    };

    // TODO(jrg): a mContentView.destroy() call is needed, both upstream and downstream.
    private ContentView mContentView;
    private EditText mUrlTextView;
    private ImageButton mPrevButton;
    private ImageButton mNextButton;

    private ClipDrawable mProgressDrawable;

    private View mSurfaceView;

    /**
     * Constructor for inflating via XML.
     */
    public Shell(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    /**
     * Set the SurfaceView being renderered to as soon as it is available.
     */
    public void setSurfaceView(View surfaceView) {
        mSurfaceView = surfaceView;
        ((FrameLayout) findViewById(R.id.contentview_holder)).addView(mSurfaceView,
                new FrameLayout.LayoutParams(
                        FrameLayout.LayoutParams.MATCH_PARENT,
                        FrameLayout.LayoutParams.MATCH_PARENT));
    }

    @Override
    protected void onFinishInflate() {
        super.onFinishInflate();

        mProgressDrawable = (ClipDrawable) findViewById(R.id.toolbar).getBackground();
        initializeUrlField();
        initializeNavigationButtons();
    }

    private void initializeUrlField() {
        mUrlTextView = (EditText) findViewById(R.id.url);
        mUrlTextView.setOnEditorActionListener(new OnEditorActionListener() {
            @Override
            public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
                if ((actionId != EditorInfo.IME_ACTION_GO) && (event == null ||
                        event.getKeyCode() != KeyEvent.KEYCODE_ENTER ||
                        event.getAction() != KeyEvent.ACTION_UP)) {
                    return false;
                }
                loadUrl(mUrlTextView.getText().toString());
                setKeyboardVisibilityForUrl(false);
                mContentView.requestFocus();
                return true;
            }
        });
        mUrlTextView.setOnFocusChangeListener(new OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                setKeyboardVisibilityForUrl(hasFocus);
                mNextButton.setVisibility(hasFocus ? GONE : VISIBLE);
                mPrevButton.setVisibility(hasFocus ? GONE : VISIBLE);
                if (!hasFocus) {
                    mUrlTextView.setText(mContentView.getUrl());
                }
            }
        });
    }

    /**
     * Loads an URL.  This will perform minimal amounts of sanitizing of the URL to attempt to
     * make it valid.
     *
     * @param url The URL to be loaded by the shell.
     */
    public void loadUrl(String url) {
        if (url == null) return;

        if (TextUtils.equals(url, mContentView.getUrl())) {
            mContentView.reload();
        } else {
            mContentView.loadUrlWithoutUrlSanitization(sanitizeUrl(url));
        }
        mUrlTextView.clearFocus();
    }

    /**
     * Given an URL, this performs minimal sanitizing to ensure it will be valid.
     * @param url The url to be sanitized.
     * @return The sanitized URL.
     */
    public static String sanitizeUrl(String url) {
        if (url == null) return url;
        if (url.startsWith("www.") || url.indexOf(":") == -1) url = "http://" + url;
        return url;
    }

    private void initializeNavigationButtons() {
        mPrevButton = (ImageButton) findViewById(R.id.prev);
        mPrevButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mContentView.canGoBack()) mContentView.goBack();
            }
        });

        mNextButton = (ImageButton) findViewById(R.id.next);
        mNextButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mContentView.canGoForward()) mContentView.goForward();
            }
        });
    }

    @SuppressWarnings("unused")
    @CalledByNative
    private void onUpdateUrl(String url) {
        mUrlTextView.setText(url);
    }

    @SuppressWarnings("unused")
    @CalledByNative
    private void onLoadProgressChanged(double progress) {
        removeCallbacks(mClearProgressRunnable);
        mProgressDrawable.setLevel((int) (10000.0 * progress));
        if (progress == 1.0) postDelayed(mClearProgressRunnable, COMPLETED_PROGRESS_TIMEOUT_MS);
    }

    /**
     * Initializes the ContentView based on the native tab contents pointer passed in.
     * @param nativeTabContents The pointer to the native tab contents object.
     */
    @SuppressWarnings("unused")
    @CalledByNative
    private void initFromNativeTabContents(int nativeTabContents) {
        mContentView = ContentView.newInstance(
                getContext(), nativeTabContents, ContentView.PERSONALITY_CHROME);
        if (mContentView.getUrl() != null) mUrlTextView.setText(mContentView.getUrl());
        ((FrameLayout) findViewById(R.id.contentview_holder)).addView(mContentView,
                new FrameLayout.LayoutParams(
                        FrameLayout.LayoutParams.MATCH_PARENT,
                        FrameLayout.LayoutParams.MATCH_PARENT));
        mContentView.requestFocus();
    }

    /**
     * @return The {@link ContentView} currently shown by this Shell.
     */
    public ContentView getContentView() {
        return mContentView;
    }

    private void setKeyboardVisibilityForUrl(boolean visible) {
        InputMethodManager imm = (InputMethodManager) getContext().getSystemService(
                Context.INPUT_METHOD_SERVICE);
        if (visible) {
            imm.showSoftInput(mUrlTextView, InputMethodManager.SHOW_IMPLICIT);
        } else {
            imm.hideSoftInputFromWindow(mUrlTextView.getWindowToken(), 0);
        }
    }
}
