# This file is used to manage the dependencies of the Chromium src repo. It is
# used by gclient to determine what version of each dependency to check out, and
# where.
#
# For more information, please refer to the official documentation:
#   https://sites.google.com/a/chromium.org/dev/developers/how-tos/get-the-code
#
# When adding a new dependency, please update the top-level .gitignore file
# to list the dependency's destination directory.
#
# -----------------------------------------------------------------------------
# Rolling deps
# -----------------------------------------------------------------------------
# All repositories in this file are git-based, using Chromium git mirrors where
# necessary (e.g., a git mirror is used when the source project is SVN-based).
# To update the revision that Chromium pulls for a given dependency:
#
#  # Create and switch to a new branch
#  git new-branch depsroll
#  # Run roll-dep (provided by depot_tools) giving the dep's path and optionally
#  # a regex that will match the line in this file that contains the current
#  # revision. The script ALWAYS rolls the dependency to the latest revision
#  # in origin/master. The path for the dep should start with src/.
#  roll-dep src/third_party/foo_package/src foo_package.git
#  # You should now have a modified DEPS file; commit and upload as normal
#  git commit -aspv_he
#  git cl upload
#
# For more on the syntax and semantics of this file, see:
#   https://bit.ly/chromium-gclient-conditionals
#
# which is a bit incomplete but the best documentation we have at the
# moment.

gclient_gn_args_file = 'src/build/config/gclient_args.gni'
gclient_gn_args = [
  'build_with_chromium',
  'checkout_android',
  'checkout_android_prebuilts_build_tools',
  'checkout_android_native_support',
  'checkout_ios_webkit',
  'checkout_nacl',
  'checkout_openxr',
  'checkout_rts_model',
  'checkout_src_internal',
  'cros_boards',
  'cros_boards_with_qemu_images',
  'generate_location_tags',
]


vars = {
  'nw_src_revision': '54b2042c2032ed2c44744e326ee3bb1accfb7e83',
  'nw_v8_revision': 'b4dfc21e530c939daaf44d9d960a8c81acf4872b',
  'nw_node_revision': '2439895727995a9dc1cc9de59948e9fa7c798d78',
  # Variable that can be used to support multiple build scenarios, like having
  # Chromium specific targets in a client project's GN file or sync dependencies
  # conditionally etc.
  'build_with_chromium': True,

  # By default, we should check out everything needed to run on the main
  # chromium waterfalls. This var can be also be set to "small", in order
  # to skip things are not strictly needed to build chromium for development
  # purposes, by adding the following line to src.git's .gclient entry:
  #      "custom_vars": { "checkout_configuration": "small" },
  'checkout_configuration': 'default',

  # By default, don't check out android. Will be overridden by gclient
  # variables.
  # TODO(ehmaldonado): Remove this once the bug in gclient is fixed.
  'checkout_android': False,

  # By default, don't check out Fuchsia. Will be overridden by gclient
  # variables.
  # TODO(ehmaldonado): Remove this once the bug in gclient is fixed.
  'checkout_fuchsia': False,

  # Used for downloading the Fuchsia SDK without running hooks.
  'checkout_fuchsia_no_hooks': False,

  # Pull in Android prebuilts build tools so we can create Java xrefs
  'checkout_android_prebuilts_build_tools': False,

  # Pull in Android native toolchain dependencies for Chrome OS too, so we can
  # build ARC++ support libraries.
  'checkout_android_native_support': 'checkout_android or checkout_chromeos',

  # By default, do not check out Cast3P.
  'checkout_cast3p': False,

  # By default, do not check out Chromium autofill captured sites test
  # dependencies. These dependencies include very large numbers of very
  # large web capture files. Captured sites test dependencies are also
  # restricted to Googlers only.
  'checkout_chromium_autofill_test_dependencies': False,

  # By default, do not check out Chromium password manager captured sites test
  # dependencies. These dependencies include very large numbers of very
  # large web capture files. Captured sites test dependencies are also
  # restricted to Googlers only.
  'checkout_chromium_password_manager_test_dependencies': False,

  # Checkout fuzz archive. Should not need in builders.
  'checkout_clusterfuzz_data': False,

  # By default, checkout JavaScript coverage node modules. These packages
  # are used to post-process raw v8 coverage reports into IstanbulJS compliant
  # output.
  'checkout_js_coverage_modules': True,

  # Check out and download nacl by default, unless on an arm mac.
  # This can be disabled e.g. with custom_vars.
  'checkout_nacl': 'not (host_os == "mac" and host_cpu == "arm64")',

  # By default, do not check out src-internal. This can be overridden e.g. with
  # custom_vars.
  'checkout_src_internal': False,

  # Fetch the additional packages and files needed to run all of the
  # telemetry tests. This is false by default as some stuff is only
  # privately accessible.
  'checkout_telemetry_dependencies': False,

  # Bots that don't consume WPR archives can skip downloading
  # them.
  'skip_wpr_archives_download': False,

  # Fetch the prebuilt binaries for llvm-cov and llvm-profdata. Needed to
  # process the raw profiles produced by instrumented targets (built with
  # the gn arg 'use_clang_coverage').
  'checkout_clang_coverage_tools': False,

  # Fetch the pgo profiles to optimize official builds.
  'checkout_pgo_profiles': False,

  # Fetch clang-tidy into the same bin/ directory as our clang binary.
  'checkout_clang_tidy': False,

  # Fetch clang libraries and headers in order to build clang tooling.
  'checkout_clang_libs': False,

  # Fetch clangd into the same bin/ directory as our clang binary.
  'checkout_clangd': False,

  # Fetch prebuilt and prepackaged Bazel binary/executable. Bazel is currently
  # only needed by `chromium/src/tools/rust/build_crubit.py` and therefore
  # shouldn't be used outside of Chromium Rust Experiments project.
  # Furthermore note that Bazel is only needed when building Crubit during Rust
  # toolchain build (and is *not* needed during regular Chromium builds).
  'checkout_bazel': False,

  # Fetch Crubit support libraries in order to build ..._rs_api.rs and
  # ..._rs_api_impl.cc that are generated by prebuilt (see
  # tools/rust/build_crubit.py) Crubit tools during Chromium build (see
  # also //build/rust/rs_bindings_from_cc.gni).
  'checkout_crubit': False,

  # By default checkout the OpenXR loader library only on Windows. The OpenXR
  # backend for VR in Chromium is currently only supported for Windows, but
  # support for other platforms may be added in the future.
  'checkout_openxr' : 'checkout_win',

  'checkout_instrumented_libraries': 'checkout_linux and checkout_configuration != "small"',

  # By default bot checkouts the WPR archive files only when this
  # flag is set True.
  'checkout_wpr_archives': False,

  # By default, do not check out WebKit for iOS, as it is not needed unless
  # running against ToT WebKit rather than system WebKit. This can be overridden
  # e.g. with custom_vars.
  'checkout_ios_webkit': False,

  # Fetches only the SDK boot images that match at least one of the
  # entries in a comma-separated list.
  #
  # Available images:
  #   Emulation:
  #   - core.x64-dfv2
  #   - terminal.qemu-x64
  #   - terminal.qemu-arm64
  #   - workstation.qemu-x64
  #   Hardware:
  #   - workstation_eng.chromebook-x64
  #   - workstation_eng.chromebook-x64-dfv2
  #
  # Since the images are hundreds of MB, default to only downloading the image
  # most commonly useful for developers. Bots and developers that need to use
  # other images can override this with additional images.
  'checkout_fuchsia_boot_images': "terminal.qemu-x64",
  'checkout_fuchsia_product_bundles': '"{checkout_fuchsia_boot_images}" != ""',

  # By default, do not check out files required to run fuchsia tests in
  # qemu on linux-arm64 machines.
  'checkout_fuchsia_for_arm64_host': False,

  # Revision of Crubit (trunk on 2022-10-15).  This should typically be the
  # same as the revision specified in CRUBIT_REVISION in
  # tools/rust/update_rust.py.  More details and roll instructions can be
  # found in tools/rust/README.md.
  'crubit_revision': 'f5cbdf4b54b0e6b9f63a4464a2c901c82e0f0209',

  # By default, download the fuchsia sdk from the public sdk directory.
  'fuchsia_sdk_cipd_prefix': 'fuchsia/sdk/gn/',

  # By default, download the fuchsia images from the fuchsia GCS bucket.
  'fuchsia_images_bucket': 'fuchsia',

  # Default to the empty board. Desktop Chrome OS builds don't need cros SDK
  # dependencies. Other Chrome OS builds should always define this explicitly.
  'cros_boards': Str(''),
  'cros_boards_with_qemu_images': Str(''),
  # Building for CrOS is only supported on linux currently.
  'checkout_simplechrome': '"{cros_boards}" != ""',
  'checkout_simplechrome_with_vms': '"{cros_boards_with_qemu_images}" != ""',

  # By default, do not check out versions of toolschains and sdks that are
  # specifically only needed by Lacros.
  'checkout_lacros_sdk': False,
  # To update the sdk version:
  # 1 Choose a version that's not newer than the Ash side so it's thoroughly
  #   tested:
  #   https://chromium-review.googlesource.com/q/%2522Automated+Commit:+LKGM%2522+status:merged
  # 2 CL description:
  # Lacros SDK: Update version <version>
  #
  # CQ_INCLUDE_TRYBOTS=luci.chrome.try:lacros-amd64-generic-chrome-skylab
  # CQ_INCLUDE_TRYBOTS=luci.chrome.try:lacros-arm-generic-chrome-skylab
  'lacros_sdk_version': '15380.0.0',

  # Generate location tag metadata to include in tests result data uploaded
  # to ResultDB. This isn't needed on some configs and the tool that generates
  # the data may not run on them, so we make it possible for this to be
  # turned off. Note that you also generate the metadata but not include it
  # via a GN build arg (tests_have_location_tags).
  'generate_location_tags': True,

  # luci-go CIPD package version.
  # Make sure the revision is uploaded by infra-packagers builder.
  # https://ci.chromium.org/p/infra-internal/g/infra-packagers/console
  'luci_go': 'git_revision:320bf3ed60cd4d24549d0ea9ee3a94394f2665ce',

  # This can be overridden, e.g. with custom_vars, to build clang from HEAD
  # instead of downloading the prebuilt pinned revision.
  'llvm_force_head_revision': False,

  # Fetch Rust toolchain built against our LLVM revision instead of the Android
  # Rust toolchain. Experimental. The corresponding GN arg
  # use_chromium_rust_toolchain directs the build to use this toolchain instead
  # of the Android toolchain.
  #
  # We avoid doing this on toolchain build bots (where
  # `checkout_rust_toolchain_deps` is set) since they are building the Rust
  # toolchain.
  'checkout_rust': 'host_os == "linux" and not checkout_rust_toolchain_deps',

  # Fetch the Android team's Rust toolchain.
  'fetch_android_chromium_rust_toolchain': False,

  # Build in-tree Rust toolchain. checkout_clang_libs must also be True. The
  # corresponding GN arg use_chromium_rust_toolchain directs the build to use
  # the in-tree toolchain instead of the Android toolchain.
  #
  # This is not intended for local development.
  # Prefer using //tools/rust/build_rust.py directly.
  'build_chromium_rust_toolchain': False,

  # See //docs/testing/regression-test-selection.md
  # for info on RTS
  'checkout_rts_model': False,

  # Use the experimental version of the RTS model
  'checkout_rts_experimental_model': False,

  # Make Dawn skip its standalone dependencies
  'dawn_standalone': False,

  # Fetch configuration files required for the 'use_remoteexec' gn arg
  'download_remoteexec_cfg': False,
  # RBE instance to use for running remote builds
  'rbe_instance': Str('projects/rbe-chrome-untrusted/instances/default_instance'),
  # RBE project to download rewrapper config files for. Only needed if
  # different from the project used in 'rbe_instance'
  'rewrapper_cfg_project': Str(''),
  # reclient CIPD package
  'reclient_package': 'infra/rbe/client/',
  # reclient CIPD package version
  'reclient_version': 're_client_version:0.99.0.3f95625-gomaip',

  # Fetch dependencies needed to build Rust toolchain. Not needed if developing
  # Rust code in Chromium; instead enable checkout_rust
  # (which is gradually being made the default across different platforms).
  # Only use if building the Rust toolchain.
  'checkout_rust_toolchain_deps': False,

  'android_git': 'https://android.googlesource.com',
  'aomedia_git': 'https://aomedia.googlesource.com',
  'boringssl_git': 'https://boringssl.googlesource.com',
  'chrome_git': 'https://chrome-internal.googlesource.com',
  'chromium_git': 'https://chromium.googlesource.com',
  'dawn_git': 'https://dawn.googlesource.com',
  'nwjs_git': 'https://github.com/nwjs',
  'pdfium_git': 'https://pdfium.googlesource.com',
  'quiche_git': 'https://quiche.googlesource.com',
  'skia_git': 'https://skia.googlesource.com',
  'swiftshader_git': 'https://swiftshader.googlesource.com',
  'webrtc_git': 'https://webrtc.googlesource.com',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling Skia
  # and whatever else without interference from each other.
  'skia_revision': '1195e70d671947af02a6a5b0ddc65806b9645252',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling V8
  # and whatever else without interference from each other.
  'v8_revision': '0e75d85d8e3467a536bca01d89d8a180a8bcffca',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling ANGLE
  # and whatever else without interference from each other.
  'angle_revision': 'c2efe6fcdbc548c5ba6c1fa9c5415e256c7211ba',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling SwiftShader
  # and whatever else without interference from each other.
  'swiftshader_revision': 'c85d70d97009a264fc5e7747316743a1abac5f67',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling PDFium
  # and whatever else without interference from each other.
  'pdfium_revision': '7c9b2b33ac5759b0443d8f6e01f07432ff034c12',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling BoringSSL
  # and whatever else without interference from each other.
  #
  # Note this revision should be updated with
  # third_party/boringssl/roll_boringssl.py, not roll-dep.
  'boringssl_revision': '74646566e93de7551bfdfc5f49de7462f13d1d05',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling Fuchsia sdk
  # and whatever else without interference from each other.
  'fuchsia_version': 'version:12.20230323.1.1',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling google-toolbox-for-mac
  # and whatever else without interference from each other.
  'google_toolbox_for_mac_revision': '42b12f10cd8342f5cb41a1e3e3a2f13fd9943b0d',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling googletest
  # and whatever else without interference from each other.
  'googletest_revision': 'af29db7ec28d6df1c7f0f745186884091e602e07',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling lighttpd
  # and whatever else without interference from each other.
  'lighttpd_revision': '9dfa55d15937a688a92cbf2b7a8621b0927d06eb',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling lss
  # and whatever else without interference from each other.
  'lss_revision': 'ce877209e11aa69dcfffbd53ef90ea1d07136521',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling NaCl
  # and whatever else without interference from each other.
  'nacl_revision': 'd2f043255597fe84888bbece2909f342f39b36c6',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling freetype
  # and whatever else without interference from each other.
  'freetype_revision': '4d8db130ea4342317581bab65fc96365ce806b77',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling freetype
  # and whatever else without interference from each other.
  'freetype_testing_revision': '7a69b1a2b028476f840ab7d4a2ffdfe4eb2c389f',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling HarfBuzz
  # and whatever else without interference from each other.
  'harfbuzz_revision': '2822b589bc837fae6f66233e2cf2eef0f6ce8470',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling Emoji Segmenter
  # and whatever else without interference from each other.
  'emoji_segmenter_revision': '9ba6d25d0d9313569665d4a9d2b34f0f39f9a50e',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling OTS
  # and whatever else without interference from each other.
  'ots_revision': '46bea9879127d0ff1c6601b078e2ce98e83fcd33',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling catapult
  # and whatever else without interference from each other.
  'catapult_revision': 'fb540cd80e69889f9d241a5b1946d9147c3f6c20',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling CrossBench
  # and whatever else without interference from each other.
  'crossbench_revision': '0941f19d9b1bab30137d9fcad6ee2ee44d338913',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling libFuzzer
  # and whatever else without interference from each other.
  'libfuzzer_revision': 'debe7d2d1982e540fbd6bd78604bf001753f9e74',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling centipede
  # and whatever else without interference from each other.
  'centipede_revision': 'a5a9071410e6e8134855b88461d0eb2c77d48cdd',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling devtools-frontend
  # and whatever else without interference from each other.
  'devtools_frontend_revision': 'e24710d614f3f16412444484e1f64b4439b965d8',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling libprotobuf-mutator
  # and whatever else without interference from each other.
  'libprotobuf-mutator': 'a304ec48dcf15d942607032151f7e9ee504b5dcf',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling android_sdk_build-tools_version
  # and whatever else without interference from each other.
  'android_sdk_build-tools_version': '-VRKr36Uw8L_iFqqo9nevIBgNMggND5iWxjidyjnCgsC',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling android_sdk_emulator_version
  # and whatever else without interference from each other.
  'android_sdk_emulator_version': '9lGp8nTUCRRWGMnI_96HcKfzjnxEJKUcfvfwmA3wXNkC',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling android_sdk_extras_version
  # and whatever else without interference from each other.
  'android_sdk_extras_version': 'ppQ4TnqDvBHQ3lXx5KPq97egzF5X2FFyOrVHkGmiTMQC',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling android_sdk_patcher_version
  # and whatever else without interference from each other.
  'android_sdk_patcher_version': 'I6FNMhrXlpB-E1lOhMlvld7xt9lBVNOO83KIluXDyA0C',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling android_sdk_platform-tools_version
  # and whatever else without interference from each other.
  'android_sdk_platform-tools_version': 'RSI3iwryh7URLGRgJHsCvUxj092woTPnKt4pwFcJ6L8C',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling android_sdk_platforms_version
  # and whatever else without interference from each other.
  'android_sdk_platforms_preview_version': 'YWMYkzyxGBgVsty0GhXL1oxbY0pGXQIgFc0Rh7ZMRPYC',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling android_sdk_platforms_version
  # and whatever else without interference from each other.
  'android_sdk_platforms_version': 'eo5KvW6UVor92LwZai8Zulc624BQZoCu-yn7wa1z_YcC',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling android_sdk_sources_version
  # and whatever else without interference from each other.
  'android_sdk_sources_version': '_a_BcnANjPYw5mSKlNHa7GFY8yc1kdqj2rmQgac7yUcC',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'dawn_revision': '069e2441956bc1d74670d2faa746534601c683c3',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'quiche_revision': '7e67f01d296f9a57f63ca41af665f7b354078772',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling ios_webkit
  # and whatever else without interference from each other.
  'ios_webkit_revision': '59e9de61b7b36507836fa8b098e8839d7d995b13',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling libexpat
  # and whatever else without interference from each other.
  'libexpat_revision': '441f98d02deafd9b090aea568282b28f66a50e36',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling wuffs
  # and whatever else without interference from each other.
  'wuffs_revision': 'fe9d08f2b6e80af691bfb1a718e144c49a1b9eba',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling libavif
  # and whatever else without interference from each other.
  'libavif_revision': '129a59047ea2b31f21109b4ed07dde32662f1b4c',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling nearby
  # and whatever else without interference from each other.
  'nearby_revision': '3f1b0c81e3c441f89b94843a60461677972f163e',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling securemessage
  # and whatever else without interference from each other.
  'securemessage_revision': 'fa07beb12babc3b25e0c5b1f38c16aa8cb6b8f84',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling ukey2
  # and whatever else without interference from each other.
  'ukey2_revision': '0275885d8e6038c39b8a8ca55e75d1d4d1727f47',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'cros_components_revision': '0631a9fae2b5f424dd1f938ba03a422b22755c90',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'resultdb_version': 'git_revision:ebc74d10fa0d64057daa6f128e89f3672eeeec95',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'libcxxabi_revision':    '9643f2cf13d6935a84a30b7da7de53327733e190',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'libunwind_revision':    '29a6dda8c6588ba4abeafdb21be531e757983e31',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'clang_format_revision':    'f97059df7f8b205064625cdb5f97b56668a125ef',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'highway_revision': '8f20644eca693cfb74aa795b0006b6779c370e7a',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling ffmpeg
  # and whatever else without interference from each other.
  'ffmpeg_revision': 'a51c75b09b3f54ab2d3efe583dcc89ba84d24c0d',

  # If you change this, also update the libc++ revision in
  # //buildtools/deps_revisions.gni.
  'libcxx_revision':       '5622befaf8a9d539bc94c9f1341b8e76065334db',

  # GN CIPD package version.
  'gn_version': 'git_revision:41fef642de70ecdcaaa26be96d56a0398f95abd4',

  # ninja CIPD package version.
  # https://chrome-infra-packages.appspot.com/p/infra/3pp/tools/ninja
  # This has to stay in sync with the version in src/third_party/ninja/README.chromium.
  'ninja_version': 'version:2@1.11.1.chromium.6',
}

# Only these hosts are allowed for dependencies in this DEPS file.
# If you need to add a new host, contact chrome infrastracture team.
allowed_hosts = [
  'android.googlesource.com',
  'aomedia.googlesource.com',
  'boringssl.googlesource.com',
  'chrome-infra-packages.appspot.com',
  'chrome-internal.googlesource.com',
  'chromium.googlesource.com',
  'dawn.googlesource.com',
  'pdfium.googlesource.com',
  'quiche.googlesource.com',
  'skia.googlesource.com',
  'swiftshader.googlesource.com',
  'webrtc.googlesource.com',
]

deps = {
  'src/tools/gyp':
    Var('chromium_git') + '/external/gyp.git' + '@' + 'd61a9397e668fa9843c4aa7da9e79460fe590bfb',

  'src/buildtools/clang_format/script':
    Var('chromium_git') +
    '/external/github.com/llvm/llvm-project/clang/tools/clang-format.git@' +
    Var('clang_format_revision'),
  'src/buildtools/linux64': {
    'packages': [
      {
        'package': 'gn/gn/linux-${{arch}}',
        'version': Var('gn_version'),
      }
    ],
    'dep_type': 'cipd',
    'condition': 'host_os == "linux"',
  },
  'src/buildtools/mac': {
    'packages': [
      {
        'package': 'gn/gn/mac-${{arch}}',
        'version': Var('gn_version'),
      }
    ],
    'dep_type': 'cipd',
    'condition': 'host_os == "mac"',
  },
  'src/buildtools/third_party/libc++/trunk':
    Var('chromium_git') +
    '/external/github.com/llvm/llvm-project/libcxx.git' + '@' +
    Var('libcxx_revision'),
  'src/buildtools/third_party/libc++abi/trunk':
    Var('chromium_git') +
    '/external/github.com/llvm/llvm-project/libcxxabi.git' + '@' +
    Var('libcxxabi_revision'),
  'src/buildtools/third_party/libunwind/trunk':
    Var('chromium_git') +
    '/external/github.com/llvm/llvm-project/libunwind.git' + '@' +
    Var('libunwind_revision'),
  'src/buildtools/win': {
    'packages': [
      {
        'package': 'gn/gn/windows-amd64',
        'version': Var('gn_version'),
      }
    ],
    'dep_type': 'cipd',
    'condition': 'host_os == "win"',
  },
  'src/buildtools/reclient': {
    'packages': [
      {
        'package': Var('reclient_package') + '${{platform}}',
        'version': Var('reclient_version'),
      }
    ],
    'dep_type': 'cipd',
  },
  'src/third_party/android_rust_toolchain/toolchain': {
    'packages': [
      {
        'package': 'chromium/third_party/android_rust_toolchain/linux-amd64',
        'version': 'version:2@1.64.0.cr2',
      },
    ],
    'dep_type': 'cipd',
    'condition': 'fetch_android_chromium_rust_toolchain',
  },

  # We don't know target_cpu at deps time. At least until there's a universal
  # binary of httpd-php, pull both intel and arm versions in DEPS and then pick
  # the right one at runtime.
  'src/third_party/apache-mac': {
    'packages': [
      {
        'package': 'infra/3pp/tools/httpd-php/mac-amd64',
        'version': 'version:2@httpd2.4.38.php7.3.31.chromium.3',
      },
    ],
    'dep_type': 'cipd',
    'condition': '(host_os == "mac")',
  },
  'src/third_party/apache-mac-arm64': {
    'packages': [
      {
        'package': 'infra/3pp/tools/httpd-php/mac-arm64',
        'version': 'version:2@httpd2.4.38.php7.3.31.chromium.3',
      },
    ],
    'dep_type': 'cipd',
    'condition': '(host_os == "mac")',
  },

  'src/third_party/apache-linux': {
    'packages': [
      {
        'package': 'infra/3pp/tools/httpd-php/linux-amd64',
        'version': 'version:2@httpd2.4.38.php7.3.31.chromium.3',
      },
    ],
    'dep_type': 'cipd',
    'condition': '(host_os == "linux")',
  },

  'src/third_party/aosp_dalvik': {
      'packages': [
          {
              'package': 'chromium/third_party/aosp_dalvik/linux-amd64',
              'version': 'version:2@13.0.0_r24.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/android_webview/tools/cts_archive': {
      'packages': [
          {
              'package': 'chromium/android_webview/tools/cts_archive',
              'version': 'D9ziv4Z61qHwterMTFEoPeCZzFNyUfMnWAlYuxEKlM8C',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/chrome/browser/resources/preinstalled_web_apps/internal': {
    'url': Var('chrome_git') + '/chrome/components/default_apps.git' + '@' + '3610b316398d68e3ae89388c03cb8bd8eb30e76a',
    'condition': 'checkout_src_internal',
  },

  'src/chrome/installer/mac/third_party/xz/xz': {
      'url': Var('chromium_git') + '/chromium/deps/xz.git' + '@' + 'eecaf55632ca72e90eb2641376bce7cdbc7284f7',
      'condition': 'checkout_mac',
  },

  'src/third_party/updater/chrome_linux64': {
      'dep_type': 'cipd',
      'condition': 'checkout_linux',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chrome_linux64',
          'version': 'iSlHWT_rV1izyFEkrQSJUVMZ_Z_8yUezA1tZGxQGsyQC',
        },
      ],
  },

  'src/third_party/updater/chrome_mac_universal': {
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chrome_mac_universal',
          'version': '7VhToYn-E3LXpYnm9s9jYdxNXke02DtWaJDqYrYRnNUC',
        },
      ],
  },

  'src/third_party/updater/chrome_mac_universal_prod': {
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chrome_mac_universal_prod',
          'version': 'qDhUmRj82uhWqE2eVqt12e1eJKWKgRpRjgQrBSrdyP4C',
        },
      ],
  },

  'src/third_party/updater/chrome_win_x86': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chrome_win_x86',
          'version': 'tZsZTfw7VWDV34gVY9dbe0GfdIpzyEiI8OixEeNiZa8C',
        },
      ],
  },

  'src/third_party/updater/chrome_win_x86_64': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chrome_win_x86_64',
          'version': 'WmP4Vc7qJey93eEtaErCNDpQisABBTDHw4TWPOJyFU4C',
        },
      ],
  },

  # A somewhat recent Chromium-branded updater build. (x86_64)
  'src/third_party/updater/chromium_mac_amd64': {
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chromium_mac_amd64',
          'version': '181zBl1aAUqG8duH_W8VOA1eT009_B8SX-Wevln3sI4C',
        },
      ],
  },

  # A somewhat recent Chromium-branded updater build. (ARM64)
  'src/third_party/updater/chromium_mac_arm64': {
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chromium_mac_arm64',
          'version': 'BdTuSFp4NP_JTwW5UBQtYiCIECiizKxOXuOvJBrkzuMC',
        },
      ],
  },

  'src/third_party/updater/chromium_win_x86': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chromium_win_x86',
          'version': '7axlCTeGu9zVyvMFjl_VGHdOeChRxuMVUg9lJB0t0y0C',
        },
      ],
  },

  'src/third_party/updater/chromium_win_x86_64': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chromium_win_x86_64',
          'version': 'bg3EWZJ5FdbYY7I949RV5oo782KRGxW4c9Qx2QHVW5oC',
        },
      ],
  },

  'src/chrome/test/data/autofill/captured_sites/artifacts': {
    'url': Var('chrome_git') + '/chrome/test/captured_sites/autofill.git' + '@' + 'a38d810c87a18582e986b94650d9cfa4b67be12c',
    'condition': 'checkout_chromium_autofill_test_dependencies',
  },

  'src/chrome/test/data/password/captured_sites/artifacts': {
    'url': Var('chrome_git') + '/chrome/test/captured_sites/password.git' + '@' + '04b3ea663adf745c52831650e2426b54bc94e65d',
    'condition': 'checkout_chromium_password_manager_test_dependencies',
  },

  'src/chrome/test/data/perf/canvas_bench':
    Var('chromium_git') + '/chromium/canvas_bench.git' + '@' + 'a7b40ea5ae0239517d78845a5fc9b12976bfc732',

  'src/chrome/test/data/perf/frame_rate/content':
    Var('chromium_git') + '/chromium/frame_rate/content.git' + '@' + 'c10272c88463efeef6bb19c9ec07c42bc8fe22b9',

  'src/chrome/test/data/safe_browsing/dmg': {
    'packages': [
      {
        'package': 'chromium/chrome/test/data/safe_browsing/dmg',
        'version': '03TLfNQgc59nHmyWtYWJfFaUrEW8QDJJzXwm-672m-QC',
      },
    ],
    'condition': 'checkout_mac',
    'dep_type': 'cipd',
  },

  'src/chrome/test/data/xr/webvr_info':
    Var('chromium_git') + '/external/github.com/toji/webvr.info.git' + '@' + 'c58ae99b9ff9e2aa4c524633519570bf33536248',

  'src/clank': {
    'url': Var('chrome_git') + '/clank/internal/apps.git' + '@' +
    '6275e53840a0ff9ce76ec3205cf65a6e4db38a99',
    'condition': 'checkout_android and checkout_src_internal',
  },

  'src/docs/website': {
    'url': Var('chromium_git') + '/website.git' + '@' + '049c271a3726ba530df735240bddd8cc5d63ba14',
  },

  'src/ios/third_party/earl_grey2/src': {
      'url': Var('chromium_git') + '/external/github.com/google/EarlGrey.git' + '@' + '740b64276df73d896a9db6011a3515d20eac30cc',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/edo/src': {
      'url': Var('chromium_git') + '/external/github.com/google/eDistantObject.git' + '@' + '51058a369f943064dc6db4f38dca32263d584ea5',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/gtx/src': {
      'url': Var('chromium_git') + '/external/github.com/google/GTXiLib.git' + '@' + '0e6d6628c5b4d733dfc8f605ab576dcbb72aeeb9',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/lottie/src': {
      'url': Var('chromium_git') + '/external/github.com/airbnb/lottie-ios.git' + '@' + '4a4367659c0b8576d4a106669ff2ba129026085f',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/material_components_ios/src': {
      'url': Var('chromium_git') + '/external/github.com/material-components/material-components-ios.git' + '@' + 'ba1e10d39bb1fa6086410c0a86d6b06604d082f5',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/material_font_disk_loader_ios/src': {
      'url': Var('chromium_git') + '/external/github.com/material-foundation/material-font-disk-loader-ios.git' + '@' + '93acc021e3034898716028822cb802a3a816be7e',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/material_internationalization_ios/src': {
      'url': Var('chromium_git') + '/external/github.com/material-foundation/material-internationalization-ios.git' + '@' + '305aa8d276f5137c98c5c1c888efc22e02251ee7',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/material_roboto_font_loader_ios/src': {
      'url': Var('chromium_git') + '/external/github.com/material-foundation/material-roboto-font-loader-ios.git' + '@' + '4be05d4676645febc453a6cde7f5adfb1b785dc1',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/material_sprited_animation_view_ios/src': {
      'url': Var('chromium_git') + '/external/github.com/material-foundation/material-sprited-animation-view-ios.git' + '@' + '8af9adaa182044cf2920dfb620b863669e1aeb7c',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/material_text_accessibility_ios/src': {
      'url': Var('chromium_git') + '/external/github.com/material-foundation/material-text-accessibility-ios.git' + '@' + '8cd910c1c8bbae261ae0d7e873ed96c69a386448',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/motion_interchange_objc/src': {
      'url': Var('chromium_git') + '/external/github.com/material-motion/motion-interchange-objc.git' + '@' + '2f8b548f74c52f71d4c2160715a4ba9c887321dd',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/motion_animator_objc/src': {
      'url': Var('chromium_git') + '/external/github.com/material-motion/motion-animator-objc.git' + '@' + '296f529321dd7c59c6284c7ccd85dec978c225cc',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/motion_transitioning_objc/src': {
      'url': Var('chromium_git') + '/external/github.com/material-motion/motion-transitioning-objc.git' + '@' + '1fe4a9d81433c1d43e54b118f29642e9b233907b',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/ochamcrest/src': {
      'url': Var('chromium_git') + '/external/github.com/hamcrest/OCHamcrest.git' + '@' + '92d9c14d13bb864255e65c09383564653896916b',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/webkit/src': {
      'url': Var('chromium_git') + '/external/github.com/WebKit/webkit.git' +
             '@' + Var('ios_webkit_revision'),
      'condition': 'checkout_ios and checkout_ios_webkit'
  },

  'src/media/cdm/api':
    Var('chromium_git') + '/chromium/cdm.git' + '@' + 'fef0b5aa1bd31efb88dfab804bdbe614f3d54f28',

  'src/native_client': {
      'url': Var('chromium_git') + '/native_client/src/native_client.git' + '@' + Var('nacl_revision'),
      'condition': 'checkout_nacl',
  },

  'src/net/third_party/quiche/src':
    Var('quiche_git') + '/quiche.git' + '@' +  Var('quiche_revision'),

  'src/testing/rts/linux-amd64': {
      'packages': [
        {
          'package': 'chromium/rts/model/linux-amd64',
          'version': 'HwyxrY2NcW8Ji7-YLiyZCRwd0EcBDtBITwoLgKCZyDYC',
        },
      ],
      'dep_type': 'cipd',
      'condition': 'checkout_rts_model and checkout_linux',
  },

  'src/testing/rts/mac-amd64': {
      'packages': [
        {
          'package': 'chromium/rts/model/mac-amd64',
          'version': 'GXdygJ5Lqx05fwk4gzmN04ydfOcnrBySGlK_fc5s2pcC',
        },
      ],
      'dep_type': 'cipd',
      'condition': 'checkout_rts_model and checkout_mac',
  },

  'src/testing/rts/windows-amd64': {
      'packages': [
        {
          'package': 'chromium/rts/model/windows-amd64',
          'version': 'sJ-03ctmnf-O_OKaRdFJ2es8jg1DIs96Vv4tMNx6t58C',
        },
      ],
      'dep_type': 'cipd',
      'condition': 'checkout_rts_model and checkout_win',
  },

  'src/testing/rts/rts-ml-chromium/linux-amd64': {
      'packages': [
        {
          'package': 'experimental/google.com/sshrimp/chromium/rts/model/linux-amd64',
          'version': '61RZ40XmEInOMhnIJMEqdOjocdCHqodmEJcDxu4x7uIC',
        },
      ],
      'dep_type': 'cipd',
      'condition': 'checkout_rts_experimental_model and checkout_linux',
  },

  'src/tools/luci-go': {
      'packages': [
        {
          'package': 'infra/tools/luci/isolate/${{platform}}',
          'version': Var('luci_go'),
        },
        {
          'package': 'infra/tools/luci/swarming/${{platform}}',
          'version': Var('luci_go'),
        },
      ],
      'dep_type': 'cipd',
  },

  'src/third_party/accessibility_test_framework': {
      'packages': [
          {
              'package': 'chromium/third_party/accessibility-test-framework',
              'version': 'b5ec1e56e58e56bc1a0c77d43111c37f9b512c8a',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_protobuf/src': {
      'url': Var('android_git') + '/platform/external/protobuf.git' + '@' + '7fca48d8ce97f7ba3ab8eea5c472f1ad3711762f',
      'condition': 'checkout_android',
  },

  'src/third_party/android_protoc': {
      'packages': [
          {
              'package': 'chromium/third_party/android_protoc',
              'version': 'sLsJWojddBL2u8NYwNh6pJsqp_bL1ttmYIlBnhiIQ1QC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_ndk': {
      'url': Var('chromium_git') + '/android_ndk.git' + '@' + '8388a2be5421311dc75c5f937aae13d821a27f3d',
      'condition': 'checkout_android_native_support',
  },

  'src/third_party/androidx': {
    'packages': [
      {
          'package': 'chromium/third_party/androidx',
          'version': 'Bd5GuYdu-KY7DAsP-QU0XofyaFYU7ySZX8QQpca7j3sC',
      },
    ],
    'condition': 'checkout_android',
    'dep_type': 'cipd',
  },

  'src/third_party/androidx_javascriptengine/src': {
      'url': Var('chromium_git') + '/aosp/platform/frameworks/support/javascriptengine/javascriptengine/src.git' + '@' + '8fc6d80d8f5fcb120f9a5074bbf2e4df02e23d9f',
      'condition': 'checkout_android',
  },

  'src/third_party/android_support_test_runner': {
      'packages': [
          {
              'package': 'chromium/third_party/android_support_test_runner',
              'version': '96d4bf848cd210fdcbca6bcc8c1b4b39cbd93141',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_system_sdk': {
      'packages': [
          {
              'package': 'chromium/third_party/android_system_sdk/public',
              'version': 'RGY8Vyf8jjszRIJRFxZj7beXSUEHTQM90MtYejUvdMgC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_build_tools/aapt2': {
      'packages': [
          {
              'package': 'chromium/third_party/android_build_tools/aapt2',
              'version': '36NqCian2RIwuM6SFfizdUgKoXyZhy3q6pFfsws0szYC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_build_tools/apkanalyzer': {
      'packages': [
          {
              'package': 'chromium/third_party/android_build_tools/apkanalyzer',
              'version': '10nnMvzw4UjIVSBd4yJQRjTp92hbnNpbuG34FzNeLIYC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_build_tools/bundletool': {
      'packages': [
          {
               'package': 'chromium/third_party/android_build_tools/bundletool',
               'version': 'TpDdbF-PPgwL0iOVsdLM07L-DUp2DV3hgzCMmPd2_GUC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_build_tools/lint': {
      'packages': [
          {
               'package': 'chromium/third_party/android_build_tools/lint',
               'version': 'E86vFKi4re9HwIfUW9yq_Ig_hc7Vr0lcl-bO3BtPQLYC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_build_tools/manifest_merger': {
      'packages': [
          {
               'package': 'chromium/third_party/android_build_tools/manifest_merger',
               'version': '_aoHU11YhUwqKZXVXsn5otnhI-ZVGFT7h1Z9eCcAZM0C',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_sdk/androidx_browser/src': {
      'url': Var('chromium_git') + '/external/gob/android/platform/frameworks/support/browser.git' + '@' + '65086eb5e52c16778fa7b4f157156d17b176fcb3',
      'condition': 'checkout_android',
  },

  'src/third_party/android_sdk/public': {
      'packages': [
          {
              'package': 'chromium/third_party/android_sdk/public/build-tools/33.0.0',
              'version': Var('android_sdk_build-tools_version'),
          },
          {
              'package': 'chromium/third_party/android_sdk/public/emulator',
              'version': Var('android_sdk_emulator_version'),
          },
          {
              'package': 'chromium/third_party/android_sdk/public/patcher',
              'version': Var('android_sdk_patcher_version'),
          },
          {
              'package': 'chromium/third_party/android_sdk/public/platform-tools',
              'version': Var('android_sdk_platform-tools_version'),
          },
          {
              'package': 'chromium/third_party/android_sdk/public/platforms/android-33',
              'version': Var('android_sdk_platforms_version'),
          },
          {
              'package': 'chromium/third_party/android_sdk/public/platforms/android-tiramisuprivacysandbox',
              'version': Var('android_sdk_platforms_preview_version'),
          },
          {
              'package': 'chromium/third_party/android_sdk/public/sources/android-31',
              'version': Var('android_sdk_sources_version'),
          },
          {
              'package': 'chromium/third_party/android_sdk/public/cmdline-tools',
              'version': '3Yn5Sn7BMObm8gsoZCF0loJMKg9_PpgU07G9DObCLdQC',
          },
      ],
      'condition': 'checkout_android_native_support',
      'dep_type': 'cipd',
  },

  'src/third_party/angle':
    Var('chromium_git') + '/angle/angle.git' + '@' +  Var('angle_revision'),

  'src/third_party/content_analysis_sdk/src':
    Var('chromium_git') + '/external/github.com/chromium/content_analysis_sdk.git' + '@' + 'dbcaea90641fab03fb7f157a200f53cf148e4542',

  'src/third_party/dav1d/libdav1d':
    Var('chromium_git') + '/external/github.com/videolan/dav1d.git' + '@' + 'd426d1c91075b9c552b12dd052af1cd0368f05a2',

  'src/third_party/dawn':
    Var('dawn_git') + '/dawn.git' + '@' +  Var('dawn_revision'),

  'src/third_party/highway/src':
    Var('chromium_git') + '/external/github.com/google/highway.git' + '@' + Var('highway_revision'),

  'src/third_party/apache-portable-runtime/src': {
      'url': Var('chromium_git') + '/external/apache-portable-runtime.git' + '@' + 'c3f11fcd86b42922834cae91103cf068246c6bb6',
      'condition': 'checkout_android',
  },

  'src/third_party/barhopper': {
      'url': Var('chrome_git') + '/chrome/deps/barhopper.git' + '@' + '865bd06ef4a839b0a15d17e38e25f8911e4cdf9f',
      'condition': 'checkout_src_internal and checkout_chromeos',
  },

  'src/third_party/google_benchmark/src':
    Var('chromium_git') + '/external/github.com/google/benchmark.git' + '@' + 'efc89f0b524780b1994d5dddd83a92718e5be492',

  'src/third_party/boringssl/src':
    Var('boringssl_git') + '/boringssl.git' + '@' +  Var('boringssl_revision'),

  'src/third_party/breakpad/breakpad':
    Var('chromium_git') + '/breakpad/breakpad.git' + '@' + 'abb105db21e962eda5b7d9b7a0ac8dd701e0b987',

  'src/third_party/byte_buddy': {
      'packages': [
          {
              'package': 'chromium/third_party/byte_buddy',
              'version': 'c9b53316603fc2d997c899c7ca1707f809b918cd',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/byte_buddy/android_sdk_build_tools_25_0_2': {
      'packages': [
          {
              'package': 'chromium/third_party/android_sdk/public/build-tools',
              'version': 'kwIs2vdfTm93yEP8LG5aSnchN4BVEdVxbqQtF4XpPdkC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/cast_core/public/src':
    Var('chromium_git') + '/cast_core/public' + '@' + 'e42ef68aa05ac0c163805f60b9b19284f3c2dee3',

  'src/third_party/catapult':
    Var('chromium_git') + '/catapult.git' + '@' + Var('catapult_revision'),

  'src/third_party/ced/src':
    Var('chromium_git') + '/external/github.com/google/compact_enc_det.git' + '@' + 'ba412eaaacd3186085babcd901679a48863c7dd5',

  'src/third_party/checkstyle': {
      'packages': [
          {
              'package': 'chromium/third_party/checkstyle',
              'version': 'UAf8iarsiPx9B6ClHuyeRNM6py76TUVdylyGLTmpb4IC',
          },
      ],
      # Needed on Linux for use on chromium_presubmit.
      'condition': 'checkout_android or checkout_linux',
      'dep_type': 'cipd',
  },

  # Dependency for ChromeVox.
  'src/third_party/chromevox/third_party/sre/src': {
      'url': Var('chromium_git') + '/external/github.com/zorkow/speech-rule-engine.git' + '@' + '5a56d4d33d67dc7c692da032d2ebbdefd7de780e',
      'condition': 'checkout_chromeos',
  },

  # Tools used when building Chrome for Chrome OS. This affects both the Simple
  # Chrome workflow, as well as the chromeos-chrome ebuild.
  'src/third_party/chromite': {
      'url': Var('chromium_git') + '/chromiumos/chromite.git' + '@' + '324dfc74b569a67598de148e85b2eb833bda1ecb',
      'condition': 'checkout_chromeos',
  },

  'src/third_party/cld_3/src':
    Var('chromium_git') + '/external/github.com/google/cld_3.git' + '@' + 'b48dc46512566f5a2d41118c8c1116c4f96dc661',

  'src/third_party/colorama/src':
    Var('chromium_git') + '/external/colorama.git' + '@' + '3de9f013df4b470069d03d250224062e8cf15c49',

  'src/third_party/cpuinfo/src':
    Var('chromium_git') + '/external/github.com/pytorch/cpuinfo.git' + '@' + 'beb46ca0319882f262e682dd596880c92830687f',

  'src/third_party/crc32c/src':
    Var('chromium_git') + '/external/github.com/google/crc32c.git' + '@' + 'fa5ade41ee480003d9c5af6f43567ba22e4e17e6',

  # For Linux and Chromium OS.
  'src/third_party/cros_system_api': {
      'url': Var('chromium_git') + '/chromiumos/platform2/system_api.git' + '@' + '445f3d2fc9a681fafa5ea6657dc18b11be470306',
      'condition': 'checkout_linux',
  },

  'src/third_party/crossbench':
    Var('chromium_git') + '/crossbench.git' + '@' + Var('crossbench_revision'),


  'src/third_party/crubit/src': {
      'url': Var('chromium_git') + '/external/github.com/google/crubit.git' + '@' + Var('crubit_revision'),
      'condition': 'checkout_crubit',
  },

  'src/third_party/depot_tools':
    Var('chromium_git') + '/chromium/tools/depot_tools.git' + '@' + 'aa3d37f8c29c170566c26a2eedeefb96c62b9dd3',

  'src/third_party/devtools-frontend/src':
    Var('chromium_git') + '/devtools/devtools-frontend' + '@' + Var('devtools_frontend_revision'),

  'src/third_party/devtools-frontend-internal': {
      'url': Var('chrome_git') + '/devtools/devtools-internal.git' + '@' + 'c4a651868ae181415ac9803c9e1778a786190a2c',
    'condition': 'checkout_src_internal',
  },

  'src/third_party/dom_distiller_js/dist':
    Var('chromium_git') + '/chromium/dom-distiller/dist.git' + '@' + '199de96b345ada7c6e7e6ba3d2fa7a6911b8767d',

  'src/third_party/eigen3/src':
    Var('chromium_git') + '/external/gitlab.com/libeigen/eigen.git' + '@' + 'f02856c6406cd36b5d7f1b98e27d85df5af08d06',

  'src/third_party/emoji-metadata/src': {
    'url': Var('chromium_git') + '/external/github.com/googlefonts/emoji-metadata' + '@' + '045f146fca682a836e01cd265171312bfb300e06',
    'condition': 'checkout_chromeos',
  },

  'src/third_party/espresso': {
      'packages': [
          {
              'package': 'chromium/third_party/espresso',
              'version': '5LoBT0j383h_4dXbnap7gnNQMtMjpbMJD1JaGIYNj-IC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/farmhash/src':
    Var('chromium_git') + '/external/github.com/google/farmhash.git' + '@' + '816a4ae622e964763ca0862d9dbd19324a1eaf45',

  'src/third_party/ffmpeg':
    Var('chromium_git') + '/chromium/third_party/ffmpeg.git' + '@' + Var('ffmpeg_revision'),

  'src/third_party/flac':
    Var('chromium_git') + '/chromium/deps/flac.git' + '@' + '689da3a7ed50af7448c3f1961d1791c7c1d9c85c',

  'src/third_party/flatbuffers/src':
    Var('chromium_git') + '/external/github.com/google/flatbuffers.git' + '@' + 'a56f9ec50e908362e20254fcef28e62a2f148d91',

  # Used for embedded builds. CrOS & Linux use the system version.
  'src/third_party/fontconfig/src': {
      'url': Var('chromium_git') + '/external/fontconfig.git' + '@' + '06929a556fdc39c8fe12965b69070c8df520a33e',
      'condition': 'checkout_linux',
  },

  'src/third_party/fp16/src':
    Var('chromium_git') + '/external/github.com/Maratyszcza/FP16.git' + '@' + '0a92994d729ff76a58f692d3028ca1b64b145d91',

  'src/third_party/gemmlowp/src':
    Var('chromium_git') + '/external/github.com/google/gemmlowp.git' + '@' + '13d57703abca3005d97b19df1f2db731607a7dc2',

  'src/third_party/grpc/src': {
      'url': Var('chromium_git') + '/external/github.com/grpc/grpc.git' + '@' + '822dab21d9995c5cf942476b35ca12a1aa9d2737',
  },

  'src/third_party/freetype/src':
    Var('chromium_git') + '/chromium/src/third_party/freetype2.git' + '@' + Var('freetype_revision'),

  'src/third_party/freetype-testing/src':
    Var('chromium_git') + '/external/github.com/freetype/freetype2-testing.git' + '@' + Var('freetype_testing_revision'),

  'src/third_party/fxdiv/src':
    Var('chromium_git') + '/external/github.com/Maratyszcza/FXdiv.git' + '@' + '63058eff77e11aa15bf531df5dd34395ec3017c8',

  'src/third_party/harfbuzz-ng/src':
    Var('chromium_git') + '/external/github.com/harfbuzz/harfbuzz.git' + '@' + Var('harfbuzz_revision'),

  'src/third_party/emoji-segmenter/src':
    Var('chromium_git') + '/external/github.com/google/emoji-segmenter.git' + '@' + Var('emoji_segmenter_revision'),

  'src/third_party/ots/src':
    Var('chromium_git') + '/external/github.com/khaledhosny/ots.git' + '@' + Var('ots_revision'),

  'src/third_party/libgav1/src':
    Var('chromium_git') + '/codecs/libgav1.git' + '@' + 'cd53f7c0d6a1c005e38874d143c8876d375bae70',

  'src/third_party/google_toolbox_for_mac/src': {
      'url': Var('chromium_git') + '/external/github.com/google/google-toolbox-for-mac.git' + '@' + Var('google_toolbox_for_mac_revision'),
      'condition': 'checkout_ios or checkout_mac',
  },

  'src/third_party/google-truth': {
      'packages': [
          {
              'package': 'chromium/third_party/google-truth',
              'version': 'u8oovXxp24lStqX4d54htRovta-75Sy2w7ijg1TL07gC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/googletest/src':
    Var('chromium_git') + '/external/github.com/google/googletest.git' + '@' + Var('googletest_revision'),

  # GNU binutils assembler for x86-32.
  'src/third_party/gnu_binutils': {
      'url': Var('chromium_git') + '/native_client/deps/third_party/gnu_binutils.git' + '@' + 'f4003433b61b25666565690caf3d7a7a1a4ec436',
      'condition': 'checkout_nacl and checkout_win',
  },

  'src/third_party/gperf': {
      'url': Var('chromium_git') + '/chromium/deps/gperf.git' + '@' + 'd892d79f64f9449770443fb06da49b5a1e5d33c1',
      'condition': 'checkout_win',
  },

  'src/third_party/gvr-android-sdk/src': {
      'url': Var('chromium_git') + '/external/github.com/googlevr/gvr-android-sdk.git' + '@' + '233e7fe922a543e0bc55382d64cacd047307d0e7',
      'condition': 'checkout_android',
  },

  'src/third_party/cardboard/src' : {
      'url': Var('chromium_git') + '/external/github.com/googlevr/cardboard/' + '@' + 'e17632c2f6e5ffe3c807bb138153a05a35f7a6fe',
      'condition': 'checkout_android',
  },

  'src/third_party/arcore-android-sdk/src': {
      'url': Var('chromium_git') + '/external/github.com/google-ar/arcore-android-sdk.git' + '@' + 'd197af6b38f98d2344bc7f76326c4aa0c4f2fb90',
      'condition': 'checkout_android',
  },

  'src/third_party/arcore-android-sdk-client': {
      'packages': [
        {
          'package': 'chromium/third_party/arcore-android-sdk-client',
          'version': 'NYf1qvBS9hNVK-6exAl-MaVbQsRKCDYl5Br936GtcU0C',
        },
      ],

      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  # Exists for rolling the Fuchsia SDK. Check out of the SDK should always
  # rely on the hook running |update_sdk.py| script below.
  'src/third_party/fuchsia-sdk/sdk': {
      'packages': [
          {
              'package': Var('fuchsia_sdk_cipd_prefix') + '${{platform}}',
              'version': Var('fuchsia_version'),
          },
      ],
      'condition': 'checkout_fuchsia_no_hooks',
      'dep_type': 'cipd',
  },

  'src/third_party/hamcrest': {
      'packages': [
          {
              'package': 'chromium/third_party/hamcrest',
              'version': '37eccfc658fe79695d6abb6dd497463c4372032f',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/hunspell_dictionaries':
    Var('chromium_git') + '/chromium/deps/hunspell_dictionaries.git' + '@' + '41cdffd71c9948f63c7ad36e1fb0ff519aa7a37e',

  'src/third_party/icu':
    Var('chromium_git') + '/chromium/deps/icu.git' + '@' + 'e3b6a4c334b9cdbe2d54476c7ac4eb98900d1144',

  'src/third_party/icu4j': {
      'packages': [
          {
              'package': 'chromium/third_party/icu4j',
              'version': 'e87e5bed2b4935913ee26a3ebd0b723ee2344354',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/jacoco': {
      'packages': [
          {
              'package': 'chromium/third_party/jacoco',
              'version': 'O8mNUqIbFxvOcBsSNfbvpdUFvY4nfrPY0QA2kHoO2pQC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/javalang/src': {
      'url': Var('chromium_git') + '/external/github.com/c2nes/javalang.git' + '@' + '0664afb7f4d40254312693f2e833c1ed4ac551c7',
      'condition': 'checkout_android',
  },

  'src/third_party/jdk': {
      'packages': [
          {
              'package': 'chromium/third_party/jdk',
              'version': '-FR8HTNcMfxy7J2HUaWVa0QmEE4f68iotzvFbqOj2LEC',
          },
      ],
      # Needed on Linux for use on chromium_presubmit (for checkstyle).
      'condition': 'checkout_android or checkout_linux',
      'dep_type': 'cipd',
  },

  # Deprecated - only use for tools which are broken our real JDK.
  'src/third_party/jdk11': {
      'packages': [
          {
              'package': 'chromium/third_party/jdk',
              # Do not update this hash - any newer hash will point to JDK17+.
              'version': 'egbcSHbmF1XZQbKxp_PQiGLFWlQK65krTGqQE-Bj4j8C',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/jsoncpp/source':
    Var('chromium_git') + '/external/github.com/open-source-parsers/jsoncpp.git'
      + '@' + '42e892d96e47b1f6e29844cc705e148ec4856448', # release 1.9.4

  'src/third_party/junit/src': {
      'url': Var('chromium_git') + '/external/junit.git' + '@' + '05fe2a64f59127c02135be22f416e91260d6ede6',
      'condition': 'checkout_android',
  },

  'src/third_party/kotlin_stdlib': {
      'packages': [
          {
              'package': 'chromium/third_party/kotlin_stdlib',
              'version': 'XON2v801ZWS7FjApXO8Ev7Me7cOsIAnmqzyCXJuMwJ0C',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/kotlinc/current': {
      'packages': [
          {
              'package': 'chromium/third_party/kotlinc',
              'version': 'bCZedwoM-hb1pP1QKzA3P5aR4zjZltqLj4JQpmQsHuUC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/leveldatabase/src':
    Var('chromium_git') + '/external/leveldb.git' + '@' + 'd019e3605f222ebc5a3a2484a2cb29db537551dd',

  'src/third_party/libFuzzer/src':
    Var('chromium_git') + '/chromium/llvm-project/compiler-rt/lib/fuzzer.git' + '@' +  Var('libfuzzer_revision'),

  'src/third_party/centipede/src':
    Var('chromium_git') + '/external/github.com/google/centipede.git' + '@' + Var('centipede_revision'),

  'src/third_party/libaddressinput/src':
    Var('chromium_git') + '/external/libaddressinput.git' + '@' + 'e8712e415627f22d0b00ebee8db99547077f39bd',

  'src/third_party/libaom/source/libaom':
    Var('aomedia_git') + '/aom.git' + '@' +  '16e24831397a22504541e8ec2674e3cf219e0ac5',

  'src/third_party/libavif/src':
    Var('chromium_git') + '/external/github.com/AOMediaCodec/libavif.git' + '@' + Var('libavif_revision'),

  'src/third_party/nearby/src':
    Var('chromium_git') + '/external/github.com/google/nearby-connections.git' + '@' + Var('nearby_revision'),

  'src/third_party/securemessage/src':
    Var('chromium_git') + '/external/github.com/google/securemessage.git' + '@' + Var('securemessage_revision'),

  'src/third_party/ukey2/src':
    Var('chromium_git') + '/external/github.com/google/ukey2.git' + '@' + Var('ukey2_revision'),

  'src/third_party/cros-components/src':
    Var('chromium_git') + '/external/google3/cros_components.git' + '@' + Var('cros_components_revision'),

  # Userspace interface to kernel DRM services.
  'src/third_party/libdrm/src': {
      'url': Var('chromium_git') + '/chromiumos/third_party/libdrm.git' + '@' + 'b9ca37b3134861048986b75896c0915cbf2e97f9',
      'condition': 'checkout_linux',
  },

  'src/third_party/expat/src':
    Var('chromium_git') + '/external/github.com/libexpat/libexpat.git' + '@' + Var('libexpat_revision'),

  # The library for IPP protocol (Chrome OS).
  'src/third_party/libipp/libipp': {
      'url': Var('chromium_git') + '/chromiumos/platform2/libipp.git' + '@' + '2209bb84a8e122dab7c02fe66cc61a7b42873d7f',
      'condition': 'checkout_linux',
  },

  'src/third_party/libjpeg_turbo':
    Var('chromium_git') + '/chromium/deps/libjpeg_turbo.git' + '@' + 'aa4075f116e4312537d0d3e9dbd5e31096539f94',

  'src/third_party/liblouis/src': {
      'url': Var('chromium_git') + '/external/liblouis-github.git' + '@' + '9700847afb92cb35969bdfcbbfbbb74b9c7b3376',
      'condition': 'checkout_linux',
  },

  'src/third_party/libphonenumber/dist':
    Var('chromium_git') + '/external/libphonenumber.git' + '@' + '140dfeb81b753388e8a672900fb7a971e9a0d362',

  'src/third_party/libprotobuf-mutator/src':
    Var('chromium_git') + '/external/github.com/google/libprotobuf-mutator.git' + '@' +  Var('libprotobuf-mutator'),

  'src/third_party/libsrtp':
    Var('chromium_git') + '/chromium/deps/libsrtp.git' + '@' + '5b7c744eb8310250ccc534f3f86a2015b3887a0a',

  # Android Explicit Synchronization.
  'src/third_party/libsync/src': {
      'url': Var('chromium_git') + '/aosp/platform/system/core/libsync.git' + '@' + 'f4f4387b6bf2387efbcfd1453af4892e8982faf6',
      'condition': 'checkout_linux',
  },

  'src/third_party/libunwindstack': {
      'url': Var('chromium_git') + '/chromium/src/third_party/libunwindstack.git' + '@' + '4dbfa0e8c844c8e243b297bc185e54a99ff94f9e',
      'condition': 'checkout_android',
  },

  'src/third_party/libvpx/source/libvpx':
    Var('chromium_git') + '/webm/libvpx.git' + '@' +  '394de691a0ef570fc49943f565ad53ee0d22a7f3',

  'src/third_party/libwebm/source':
    Var('chromium_git') + '/webm/libwebm.git' + '@' + 'e4fbea0c9751ae8aa86629b197a28d8276a2b0da',

  'src/third_party/libwebp/src':
    Var('chromium_git') + '/webm/libwebp.git' + '@' +  'fd7b5d48464475408d32d2611bdb6947d4246b97',

  'src/third_party/libyuv':
    Var('chromium_git') + '/libyuv/libyuv.git' + '@' + '464c51a0353c71f08fe45f683d6a97a638d47833',

  'src/third_party/lighttpd': {
      'url': Var('chromium_git') + '/chromium/deps/lighttpd.git' + '@' + Var('lighttpd_revision'),
      'condition': 'checkout_mac or checkout_win',
  },

  'src/third_party/lss': {
      'url': Var('chromium_git') + '/linux-syscall-support.git' + '@' + Var('lss_revision'),
      'condition': 'checkout_android or checkout_linux',
  },

  'src/third_party/lzma_sdk/bin/host_platform': {
      'packages': [
          {
              'package': 'infra/3pp/tools/7z/${{platform}}',
              'version': 'version:2@22.01',
          },
      ],
      'condition': 'checkout_win',
      'dep_type': 'cipd',
  },

  'src/third_party/lzma_sdk/bin/win64': {
      'packages': [
          {
              'package': 'infra/3pp/tools/7z/windows-amd64',
              'version': 'version:2@22.01',
          },
      ],
      'condition': 'checkout_win',
      'dep_type': 'cipd',
  },

  'src/third_party/material_color_utilities/src': {
      'url': Var('chromium_git') + '/external/github.com/material-foundation/material-color-utilities.git' + '@' + 'bd6537fb1c4aa2164d97f96e78a9c826e360a0ed',
  },

  'src/third_party/material_design_icons/src': {
      'url': Var('chromium_git') + '/external/github.com/google/material-design-icons.git' + '@' +
          '5ab428852e35dc177a8c37a2df9dc9ccf768c65a',
      'condition': 'checkout_ios',
  },

  'src/third_party/mig/bin': {
      'packages': [
          {
              'package': 'chromium/third_party/mig/${{platform}}',
              'version': '4wxov_ILjFdgBumBqgUgOgIcr4kcMh7i4b4oJi_cLjcC',
          },
      ],
      'condition': 'host_os == "linux" and checkout_mac',
      'dep_type': 'cipd',
  },

  # GNU binutils assembler for x86-64.
  'src/third_party/mingw-w64/mingw/bin': {
      'url': Var('chromium_git') + '/native_client/deps/third_party/mingw-w64/mingw/bin.git' + '@' + '3cc8b140b883a9fe4986d12cfd46c16a093d3527',
      'condition': 'checkout_nacl and checkout_win',
  },

  # Graphics buffer allocator for Chrome OS.
  'src/third_party/minigbm/src': {
      'url': Var('chromium_git') + '/chromiumos/platform/minigbm.git' + '@' + '3018207f4d89395cc271278fb9a6558b660885f5',
      'condition': 'checkout_linux',
  },

  'src/third_party/mockito/src': {
      'url': Var('chromium_git') + '/external/mockito/mockito.git' + '@' + '04a2a289a4222f80ad20717c25144981210d2eac',
      'condition': 'checkout_android',
  },

  # Binaries for nacl sdk.
  'src/third_party/nacl_sdk_binaries': {
      'url': Var('chromium_git') + '/chromium/deps/nacl_sdk_binaries.git' + '@' + '759dfca03bdc774da7ecbf974f6e2b84f43699a5',
      'condition': 'checkout_nacl and checkout_win',
  },

  'src/third_party/nasm': {
      'url': Var('chromium_git') + '/chromium/deps/nasm.git' + '@' +
      '7fc833e889d1afda72c06220e5bed8fb43b2e5ce'
  },

  'src/third_party/neon_2_sse/src':
    Var('chromium_git') + '/external/github.com/intel/ARM_NEON_2_x86_SSE.git' + '@' + 'a15b489e1222b2087007546b4912e21293ea86ff',

  'src/third_party/netty-tcnative/src': {
      'url': Var('chromium_git') + '/external/netty-tcnative.git' + '@' + '035726f76293d142ec3c4464be0703605feb4d02',
      'condition': 'checkout_android',
  },

  'src/third_party/netty4/src': {
      'url': Var('chromium_git') + '/external/netty4.git' + '@' + 'cc4420b13bb4eeea5b1cf4f93b2755644cd3b120',
      'condition': 'checkout_android',
  },

  'src/third_party/ninja': {
    'packages': [
      {
        'package': 'infra/3pp/tools/ninja/${{platform}}',
        'version': Var('ninja_version'),
      }
    ],
    'dep_type': 'cipd',
  },

  'src/third_party/objenesis': {
      'packages': [
          {
              'package': 'chromium/third_party/objenesis',
              'version': 'tknDblENYi8IaJYyD6tUahUyHYZlzJ_Y74_QZSz4DpIC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/openh264/src':
    Var('chromium_git') + '/external/github.com/cisco/openh264' + '@' + 'db956674bbdfbaab5acdd3fdb4117c2fef5527e9',

  'src/third_party/openscreen/src':
    Var('chromium_git') + '/openscreen' + '@' + '9fa061bbb71807041927af9952aae249014160f1',

  'src/third_party/openxr/src': {
    'url': Var('chromium_git') + '/external/github.com/KhronosGroup/OpenXR-SDK' + '@' + 'bf21ccb1007bb531b45d9978919a56ea5059c245',
    'condition': 'checkout_openxr',
  },

  'src/third_party/pdfium':
    Var('pdfium_git') + '/pdfium.git' + '@' +  Var('pdfium_revision'),

  # Parses Windows PE/COFF executable format.
  'src/third_party/pefile': {
      'url': Var('chromium_git') + '/external/pefile.git' + '@' + '72c6ae42396cb913bcab63c15585dc3b5c3f92f1',
      'condition': 'checkout_win',
  },

  'src/third_party/perfetto':
    Var('android_git') + '/platform/external/perfetto.git' + '@' + 'bfdb3fc5cc0fc2ea9324caef920ceb30c1e3d9f5',

  'src/third_party/perl': {
      'url': Var('chromium_git') + '/chromium/deps/perl.git' + '@' + '6f3e5028eb65d0b4c5fdd792106ac4c84eee1eb3',
      'condition': 'checkout_win',
  },

  'src/third_party/pthreadpool/src':
    Var('chromium_git') + '/external/github.com/Maratyszcza/pthreadpool.git' + '@' + '1787867f6183f056420e532eec640cba25efafea',

  # Dependency of skia.
  'src/third_party/pyelftools': {
      'url': Var('chromium_git') + '/chromiumos/third_party/pyelftools.git' + '@' + '19b3e610c86fcadb837d252c794cb5e8008826ae',
      'condition': 'checkout_linux',
  },

  'src/third_party/quic_trace/src':
    Var('chromium_git') + '/external/github.com/google/quic-trace.git' + '@' + 'caa0a6eaba816ecb737f9a70782b7c80b8ac8dbc',

  'src/third_party/pywebsocket3/src':
    Var('chromium_git') + '/external/github.com/GoogleChromeLabs/pywebsocket3.git' + '@' + '50602a14f1b6da17e0b619833a13addc6ea78bc2',

  'src/third_party/qemu-linux-arm64': {
      'packages': [
          {
              'package': 'fuchsia/third_party/qemu/linux-arm64',
              'version': 'BpnoBb2d44_SOm9toN6Lju5a2RLGAc1TPUO6xyijoP8C'
          },
      ],
      'condition': 'host_os == "linux" and checkout_fuchsia and checkout_fuchsia_for_arm64_host',
      'dep_type': 'cipd',
  },

  'src/third_party/re2/src':
    Var('chromium_git') + '/external/github.com/google/re2.git' + '@' + '11073deb73b3d01018308863c0bcdfd0d51d3e70',

  'src/third_party/r8': {
      'packages': [
          {
              'package': 'chromium/third_party/r8',
              'version': 'EGf7RQo3stt-vPTw69TaMGuNtnOx0Dbk1O-MBquwswYC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  # This duplication is intentional, so we avoid updating the r8.jar used by
  # dexing unless necessary, since each update invalidates all incremental
  # dexing and unnecessarily slows down all bots.
  'src/third_party/r8/d8': {
      'packages': [
          {
              'package': 'chromium/third_party/r8',
              'version': 'PwglNZFRNPkBBXdnY9NfrZFk2ULWDTRxhV9rl2kvkpUC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/requests/src': {
      'url': Var('chromium_git') + '/external/github.com/kennethreitz/requests.git' + '@' + 'c7e0fc087ceeadb8b4c84a0953a422c474093d6d',
      'condition': 'checkout_android',
  },

  'src/third_party/robolectric': {
      'packages': [
          {
              'package': 'chromium/third_party/robolectric',
              'version': 'hzetqh1qFI32FOgQroZvGcGdomrgVBJ6WKRnl1KFw6EC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/ruy/src':
    Var('chromium_git') + '/external/github.com/google/ruy.git' + '@' + '21a85fef159f9942f636a43b14c64b481c2a05b2',

  'src/third_party/skia':
    Var('skia_git') + '/skia.git' + '@' +  Var('skia_revision'),

  'src/third_party/smhasher/src':
    Var('chromium_git') + '/external/smhasher.git' + '@' + 'e87738e57558e0ec472b2fc3a643b838e5b6e88f',

  'src/third_party/snappy/src':
    Var('chromium_git') + '/external/github.com/google/snappy.git' + '@' + '65dc7b383985eb4f63cd3e752136db8d9b4be8c0',

  'src/third_party/sqlite/src':
    Var('chromium_git') + '/chromium/deps/sqlite.git' + '@' + 'f6752b7ed1fe3cc1491c0c47ec5804ee2bd0e59b',

  'src/third_party/sqlite4java': {
      'packages': [
          {
              'package': 'chromium/third_party/sqlite4java',
              'version': 'LofjKH9dgXIAJhRYCPQlMFywSwxYimrfDeBmaHc-Z5EC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/swift-format': {
      'packages': [
          {
              'package': 'infra/3pp/tools/swift-format/${{platform}}',
              'version': 'version:2@505.chromium.1',
          },
      ],
      'condition': 'host_os == mac',
      'dep_type': 'cipd',
  },

  'src/third_party/swiftshader':
    Var('swiftshader_git') + '/SwiftShader.git' + '@' +  Var('swiftshader_revision'),

  'src/third_party/swift-toolchain': {
      'packages': [
          {
              'package': 'chromium/tools/swift-toolchain/mac-amd64',
              'version': 'version:2@5.7-release',
          },
      ],
      'condition': 'host_os == mac',
      'dep_type': 'cipd',
  },

  'src/third_party/text-fragments-polyfill/src':
    Var('chromium_git') + '/external/github.com/GoogleChromeLabs/text-fragments-polyfill.git' + '@' + 'c036420683f672d685e27415de0a5f5e85bdc23f',

  'src/third_party/tflite/src':
    Var('chromium_git') + '/external/github.com/tensorflow/tensorflow.git' + '@' + 'ca388405c617415aebe5669a52f51d36fdb5f255',

  'src/third_party/turbine': {
      'packages': [
          {
              'package': 'chromium/third_party/turbine',
              'version': 'epnqx7Yf9QxgyDaU87KJ1fLQvdZ_Mho_JjfpjmIBYWYC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/vulkan-deps': '{chromium_git}/vulkan-deps@7fa3159d78e09b7761f1cc5558bcd014529b4630',

  'src/third_party/vulkan_memory_allocator':
    Var('chromium_git') + '/external/github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git' + '@' + 'ebe84bec02c041d28f902da0214bf442743fc907',

  # Display server protocol for Linux.
  'src/third_party/wayland/src': {
      'url': Var('chromium_git') + '/external/anongit.freedesktop.org/git/wayland/wayland.git' + '@' + 'c7473676b8abc682e820546287044cee3bca9147',
      'condition': 'checkout_linux',
  },

  # Wayland protocols that add functionality not available in the core protocol.
  'src/third_party/wayland-protocols/src': {
      'url': Var('chromium_git') + '/external/anongit.freedesktop.org/git/wayland/wayland-protocols.git' + '@' + '4624cfaaf563cd7be5e2e2087c8de6d3a48ea867',
      'condition': 'checkout_linux',
  },

  # Additional Wayland protocols specific for KDE Plasma desktop environment.
  'src/third_party/wayland-protocols/kde': {
      'url': Var('chromium_git') + '/external/github.com/KDE/plasma-wayland-protocols.git' + '@' + '0b07950714b3a36c9b9f71fc025fc7783e82926e',
      'condition': 'checkout_linux',
  },

  # Additional Wayland protocols specific for GNOME desktop environment.
  'src/third_party/wayland-protocols/gtk': {
      'url': Var('chromium_git') + '/external/github.com/GNOME/gtk.git' + '@' + '40ebed3a03aef096addc0af09fec4ec529d882a0',
      'condition': 'checkout_linux',
  },

  # Keep this to the same revision as the one .vpython3.
  'src/third_party/webdriver/pylib':
    Var('chromium_git') + '/external/github.com/SeleniumHQ/selenium/py.git' + '@' + 'fc5e7e70c098bfb189a9a74746809ad3c5c34e04',

  'src/third_party/webgl/src':
    Var('chromium_git') + '/external/khronosgroup/webgl.git' + '@' + 'd1b65aa5a88f6efd900604dfcda840154e9f16e2',

  'src/third_party/webgpu-cts/src':
    Var('chromium_git') + '/external/github.com/gpuweb/cts.git' + '@' + '07ce6fa80556ad9ea530f68bd82ffcafede40728',

  'src/third_party/webrtc':
    Var('webrtc_git') + '/src.git' + '@' + 'f6ab0b438e22ea30db7ad3fbf9f870b0d4506235',

  # Wuffs' canonical repository is at github.com/google/wuffs, but we use
  # Skia's mirror of Wuffs, the same as in upstream Skia's DEPS file.
  'src/third_party/wuffs/src':
    Var('skia_git') + '/external/github.com/google/wuffs-mirror-release-c.git' + '@' +  Var('wuffs_revision'),

  'src/third_party/weston/src': {
      'url': Var('chromium_git') + '/external/anongit.freedesktop.org/git/wayland/weston.git' + '@' + '420cfd00f2de91de74bd9d4d8348e43c582d29f0',
      'condition': 'checkout_linux',
  },

  # A conformance-suite developed by canonical for the mir wayland server.
  # Required to compile exo_wlcs on chromeos checkouts.
  'src/third_party/wlcs/src': {
      'url': Var('chromium_git') + '/external/github.com/MirServer/wlcs.git' + '@' + '2930ad4b5ca602446ad211b49fb1827303ce9f4b',
      'condition': 'checkout_chromeos',
  },

  'src/third_party/xdg-utils': {
      'url': Var('chromium_git') + '/chromium/deps/xdg-utils.git' + '@' + 'd80274d5869b17b8c9067a1022e4416ee7ed5e0d',
      'condition': 'checkout_linux',
  },

  'src/third_party/xnnpack/src':
    Var('chromium_git') + '/external/github.com/google/XNNPACK.git' + '@' + '06b2705f1b3e1ba0f161dd2979e2901ce93014e3',

  'src/tools/page_cycler/acid3':
    Var('chromium_git') + '/chromium/deps/acid3.git' + '@' + '6be0a66a1ebd7ebc5abc1b2f405a945f6d871521',

  'src/tools/skia_goldctl/linux': {
      'packages': [
        {
          'package': 'skia/tools/goldctl/linux-amd64',
          'version': 'iQ7zKud-gha6r9hEdwqYWRdOpeAs6gFfDxnviDUt4FQC',
        },
      ],
      'dep_type': 'cipd',
      'condition': 'checkout_linux',
  },
  'src/tools/skia_goldctl/win': {
      'packages': [
        {
          'package': 'skia/tools/goldctl/windows-amd64',
          'version': 'we56UJIWxJJ2GkQ_ne0o3oGAr7FBJa5T5Jr1xguLn-gC',
        },
      ],
      'dep_type': 'cipd',
      'condition': 'checkout_win',
  },

  'src/tools/skia_goldctl/mac_amd64': {
      'packages': [
        {
          'package': 'skia/tools/goldctl/mac-amd64',
          'version': '9Wfje1bt82IO9pJokAt9lboy59X_Pe-s0b4EpmH7RT4C',
        },
      ],
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
  },

  'src/tools/skia_goldctl/mac_arm64': {
      'packages': [
        {
          'package': 'skia/tools/goldctl/mac-arm64',
          'version': 'zihT2Lk2afg0XzIZozuGcZXWv7RJujaDEi_6q7QL4DgC',
        },
      ],
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
  },
  'src/v8':
    Var('nwjs_git') + '/v8.git' + '@' +  Var('nw_v8_revision'),

  'src/third_party/node-nw':
    Var('nwjs_git') + '/node.git' + '@' +  Var('nw_node_revision'),

  #'src/v8':
  #  Var('chromium_git') + '/v8/v8.git' + '@' +  Var('v8_revision'),

  'src-internal': {
    'url': Var('chrome_git') + '/chrome/src-internal.git@af713d37d81fcded988dce25a3284d2763c2beeb',
    'condition': 'checkout_src_internal',
  },

  'src/ash/ambient/resources': {
    'packages': [
      {
        'package': 'chromeos_internal/assistant/ambient',
        'version': 'version:feel_the_breeze_with_frame_rate_markers',
      },
    ],
    'condition': 'checkout_chromeos and checkout_src_internal',
    'dep_type': 'cipd',
  },

  'src/ash/webui/eche_app_ui/resources/prod': {
    'packages': [
      {
        'package': 'chromeos_internal/apps/eche_app/app',
        'version': 'ZwywifbvYt1337HGe0M8wkopVNNF-F8iB56x76CHhPUC',
      },
    ],
    'condition': 'checkout_chromeos and checkout_src_internal',
    'dep_type': 'cipd',
  },

  'src/ash/webui/help_app_ui/resources/prod': {
    'packages': [
      {
        'package': 'chromeos_internal/apps/help_app/app',
        'version': 'AJctvg8Rp2HdxwonQDlXAJotQAKs3UOuvLPDpQz0DBYC',
      },
    ],
    'condition': 'checkout_chromeos and checkout_src_internal',
    'dep_type': 'cipd',
  },

  'src/ash/webui/media_app_ui/resources/prod': {
    'packages': [
      {
        'package': 'chromeos_internal/apps/media_app/app',
        'version': 'jBqCvHK_nRzSlQ39oGemZF_hSmtPKGRom2wqsPvdJpMC',
      },
    ],
    'condition': 'checkout_chromeos and checkout_src_internal',
    'dep_type': 'cipd',
  },

  'src/ash/webui/projector_app/resources/prod': {
    'packages': [
      {
        'package': 'chromeos_internal/apps/projector_app/app',
        'version': 'kYrwWa00iq2L4oUZsXLbRYMP_ffVg025R6d6W00qKrcC',
      },
    ],
    'condition': 'checkout_chromeos and checkout_src_internal',
    'dep_type': 'cipd',
  },

  'src/third_party/webpagereplay': {
      'packages' : [
          {
              'package': 'infra/tools/wpr/linux_x86_64',
              'version': 'y28SfbEF6nHSkZ1eHysM1t711zpOCmtk7jUdxZB-QSMC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_prebuilts/build_tools': {
      'url': Var('android_git') + '/platform/prebuilts/build-tools.git' + '@' + '673c20b524a83b662d8c1057fd3eec8fd0f93f9d',
      'condition': 'checkout_android_prebuilts_build_tools',
  },

  # TODO(crbug.com/1290782): This should ideally be replaced with recursdeps,
  # but that arises concern which revision of devtools-frontend we should use
  # between public and internal build. So we have copy of CIPD deps from
  # devtools-frontend for now here.
  'src/third_party/devtools-frontend/src/third_party/esbuild': {
      'packages': [
          {
              'package': 'infra/3pp/tools/esbuild/${{platform}}',
              # This should be synced with devtools-frontend DEPS.
              'version': 'version:2@0.14.13.chromium.2',
          },
      ],
      'dep_type': 'cipd',
  },

  # === ANDROID_DEPS Generated Code Start ===
  # Generated by //third_party/android_deps/fetch_all.py
  'src/third_party/android_deps/libs/android_arch_core_common': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/android_arch_core_common',
              'version': 'version:2@1.1.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/android_arch_core_runtime': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/android_arch_core_runtime',
              'version': 'version:2@1.1.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/android_arch_lifecycle_common': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/android_arch_lifecycle_common',
              'version': 'version:2@1.1.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/android_arch_lifecycle_common_java8': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/android_arch_lifecycle_common_java8',
              'version': 'version:2@1.1.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/android_arch_lifecycle_livedata': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/android_arch_lifecycle_livedata',
              'version': 'version:2@1.1.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/android_arch_lifecycle_livedata_core': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/android_arch_lifecycle_livedata_core',
              'version': 'version:2@1.1.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/android_arch_lifecycle_runtime': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/android_arch_lifecycle_runtime',
              'version': 'version:2@1.1.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/android_arch_lifecycle_viewmodel': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/android_arch_lifecycle_viewmodel',
              'version': 'version:2@1.1.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_animated_vector_drawable': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_animated_vector_drawable',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_appcompat_v7': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_appcompat_v7',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_asynclayoutinflater': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_asynclayoutinflater',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_cardview_v7': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_cardview_v7',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_collections': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_collections',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_coordinatorlayout': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_coordinatorlayout',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_cursoradapter': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_cursoradapter',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_customview': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_customview',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_design': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_design',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_documentfile': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_documentfile',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_drawerlayout': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_drawerlayout',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_interpolator': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_interpolator',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_loader': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_loader',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_localbroadcastmanager': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_localbroadcastmanager',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_multidex': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_multidex',
              'version': 'version:2@1.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_print': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_print',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_recyclerview_v7': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_recyclerview_v7',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_slidingpanelayout': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_slidingpanelayout',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_support_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_support_annotations',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_support_compat': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_support_compat',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_support_core_ui': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_support_core_ui',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_support_core_utils': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_support_core_utils',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_support_fragment': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_support_fragment',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_support_media_compat': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_support_media_compat',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_support_v4': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_support_v4',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_support_vector_drawable': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_support_vector_drawable',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_swiperefreshlayout': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_swiperefreshlayout',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_transition': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_transition',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_versionedparcelable': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_versionedparcelable',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_viewpager': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_viewpager',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_tools_common': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_tools_common',
              'version': 'version:2@30.2.0-beta01.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_tools_layoutlib_layoutlib_api': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_tools_layoutlib_layoutlib_api',
              'version': 'version:2@30.2.0-beta01.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_tools_sdk_common': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_tools_sdk_common',
              'version': 'version:2@30.2.0-beta01.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_github_ben_manes_caffeine_caffeine': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_github_ben_manes_caffeine_caffeine',
              'version': 'version:2@2.8.8.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_github_kevinstern_software_and_algorithms': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_github_kevinstern_software_and_algorithms',
              'version': 'version:2@1.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_annotations',
              'version': 'version:2@4.1.1.4.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_apps_common_testing_accessibility_framework_accessibility_test_framework': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_apps_common_testing_accessibility_framework_accessibility_test_framework',
              'version': 'version:2@4.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_datatransport_transport_api': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_datatransport_transport_api',
              'version': 'version:2@2.2.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_auth': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_auth',
              'version': 'version:2@20.1.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_auth_api_phone': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_auth_api_phone',
              'version': 'version:2@18.0.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_auth_base': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_auth_base',
              'version': 'version:2@18.0.2.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_base': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_base',
              'version': 'version:2@18.0.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_basement': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_basement',
              'version': 'version:2@18.1.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_cast': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_cast',
              'version': 'version:2@17.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_cast_framework': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_cast_framework',
              'version': 'version:2@17.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_clearcut': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_clearcut',
              'version': 'version:2@17.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_cloud_messaging': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_cloud_messaging',
              'version': 'version:2@16.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_fido': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_fido',
              'version': 'version:2@19.0.0-beta.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_flags': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_flags',
              'version': 'version:2@17.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_gcm': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_gcm',
              'version': 'version:2@17.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_iid': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_iid',
              'version': 'version:2@17.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_instantapps': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_instantapps',
              'version': 'version:2@18.0.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_location': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_location',
              'version': 'version:2@19.0.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_phenotype': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_phenotype',
              'version': 'version:2@17.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_places_placereport': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_places_placereport',
              'version': 'version:2@17.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_stats': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_stats',
              'version': 'version:2@17.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_tasks': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_tasks',
              'version': 'version:2@18.0.2.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_vision': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_vision',
              'version': 'version:2@20.1.3.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_vision_common': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_vision_common',
              'version': 'version:2@19.1.3.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_material_material': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_material_material',
              'version': 'version:2@1.7.0-alpha02.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_play_core_common': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_play_core_common',
              'version': 'version:2@2.0.2.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_play_feature_delivery': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_play_feature_delivery',
              'version': 'version:2@2.0.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_auto_auto_common': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_auto_auto_common',
              'version': 'version:2@1.2.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_auto_service_auto_service': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_auto_service_auto_service',
              'version': 'version:2@1.0-rc6.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_auto_service_auto_service_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_auto_service_auto_service_annotations',
              'version': 'version:2@1.0-rc6.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_auto_value_auto_value_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_auto_value_auto_value_annotations',
              'version': 'version:2@1.9.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_code_findbugs_jsr305': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_code_findbugs_jsr305',
              'version': 'version:2@3.0.2.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_code_gson_gson': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_code_gson_gson',
              'version': 'version:2@2.9.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_dagger_dagger': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_dagger_dagger',
              'version': 'version:2@2.30.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_dagger_dagger_compiler': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_dagger_dagger_compiler',
              'version': 'version:2@2.30.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_dagger_dagger_producers': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_dagger_dagger_producers',
              'version': 'version:2@2.30.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_dagger_dagger_spi': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_dagger_dagger_spi',
              'version': 'version:2@2.30.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_errorprone_error_prone_annotation': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_errorprone_error_prone_annotation',
              'version': 'version:2@2.11.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_errorprone_error_prone_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_errorprone_error_prone_annotations',
              'version': 'version:2@2.14.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_errorprone_error_prone_check_api': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_errorprone_error_prone_check_api',
              'version': 'version:2@2.11.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_errorprone_error_prone_core': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_errorprone_error_prone_core',
              'version': 'version:2@2.11.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_errorprone_error_prone_type_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_errorprone_error_prone_type_annotations',
              'version': 'version:2@2.11.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_errorprone_javac': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_errorprone_javac',
              'version': 'version:2@9+181-r4173-1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_errorprone_javac_shaded': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_errorprone_javac_shaded',
              'version': 'version:2@9-dev-r4023-3.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_firebase_firebase_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_firebase_firebase_annotations',
              'version': 'version:2@16.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_firebase_firebase_common': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_firebase_firebase_common',
              'version': 'version:2@19.5.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_firebase_firebase_components': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_firebase_firebase_components',
              'version': 'version:2@16.1.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_firebase_firebase_encoders': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_firebase_firebase_encoders',
              'version': 'version:2@16.1.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_firebase_firebase_encoders_json': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_firebase_firebase_encoders_json',
              'version': 'version:2@17.1.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_firebase_firebase_iid': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_firebase_firebase_iid',
              'version': 'version:2@21.0.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_firebase_firebase_iid_interop': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_firebase_firebase_iid_interop',
              'version': 'version:2@17.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_firebase_firebase_installations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_firebase_firebase_installations',
              'version': 'version:2@16.3.5.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_firebase_firebase_installations_interop': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_firebase_firebase_installations_interop',
              'version': 'version:2@16.0.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_firebase_firebase_measurement_connector': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_firebase_firebase_measurement_connector',
              'version': 'version:2@18.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_firebase_firebase_messaging': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_firebase_firebase_messaging',
              'version': 'version:2@21.0.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_googlejavaformat_google_java_format': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_googlejavaformat_google_java_format',
              'version': 'version:2@1.5.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_guava_failureaccess': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_guava_failureaccess',
              'version': 'version:2@1.0.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_guava_guava': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_guava_guava',
              'version': 'version:2@31.0.1-jre.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_guava_guava_android': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_guava_guava_android',
              'version': 'version:2@31.1-android.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_guava_listenablefuture': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_guava_listenablefuture',
              'version': 'version:2@1.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_j2objc_j2objc_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_j2objc_j2objc_annotations',
              'version': 'version:2@1.3.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_protobuf_protobuf_java': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_protobuf_protobuf_java',
              'version': 'version:2@3.19.2.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_protobuf_protobuf_javalite': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_protobuf_protobuf_javalite',
              'version': 'version:2@3.21.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_googlecode_java_diff_utils_diffutils': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_googlecode_java_diff_utils_diffutils',
              'version': 'version:2@1.3.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_squareup_javapoet': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_squareup_javapoet',
              'version': 'version:2@1.13.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_squareup_javawriter': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_squareup_javawriter',
              'version': 'version:2@2.1.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_squareup_okio_okio_jvm': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_squareup_okio_okio_jvm',
              'version': 'version:2@3.0.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_squareup_wire_wire_runtime_jvm': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_squareup_wire_wire_runtime_jvm',
              'version': 'version:2@4.4.3.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/io_github_java_diff_utils_java_diff_utils': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/io_github_java_diff_utils_java_diff_utils',
              'version': 'version:2@4.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/io_grpc_grpc_api': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/io_grpc_grpc_api',
              'version': 'version:2@1.49.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/io_grpc_grpc_binder': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/io_grpc_grpc_binder',
              'version': 'version:2@1.49.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/io_grpc_grpc_context': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/io_grpc_grpc_context',
              'version': 'version:2@1.49.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/io_grpc_grpc_core': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/io_grpc_grpc_core',
              'version': 'version:2@1.49.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/io_grpc_grpc_protobuf_lite': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/io_grpc_grpc_protobuf_lite',
              'version': 'version:2@1.49.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/io_grpc_grpc_stub': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/io_grpc_grpc_stub',
              'version': 'version:2@1.49.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/io_perfmark_perfmark_api': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/io_perfmark_perfmark_api',
              'version': 'version:2@0.25.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/javax_annotation_javax_annotation_api': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/javax_annotation_javax_annotation_api',
              'version': 'version:2@1.3.2.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/javax_annotation_jsr250_api': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/javax_annotation_jsr250_api',
              'version': 'version:2@1.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/javax_inject_javax_inject': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/javax_inject_javax_inject',
              'version': 'version:2@1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/net_bytebuddy_byte_buddy': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/net_bytebuddy_byte_buddy',
              'version': 'version:2@1.12.22.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/net_bytebuddy_byte_buddy_agent': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/net_bytebuddy_byte_buddy_agent',
              'version': 'version:2@1.12.22.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/net_bytebuddy_byte_buddy_android': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/net_bytebuddy_byte_buddy_android',
              'version': 'version:2@1.12.22.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/net_ltgt_gradle_incap_incap': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/net_ltgt_gradle_incap_incap',
              'version': 'version:2@0.2.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_bouncycastle_bcprov_jdk15on': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_bouncycastle_bcprov_jdk15on',
              'version': 'version:2@1.68.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_ccil_cowan_tagsoup_tagsoup': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_ccil_cowan_tagsoup_tagsoup',
              'version': 'version:2@1.2.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_checkerframework_checker_compat_qual': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_checkerframework_checker_compat_qual',
              'version': 'version:2@2.5.5.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_checkerframework_checker_qual': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_checkerframework_checker_qual',
              'version': 'version:2@3.25.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_checkerframework_checker_util': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_checkerframework_checker_util',
              'version': 'version:2@3.25.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_checkerframework_dataflow_errorprone': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_checkerframework_dataflow_errorprone',
              'version': 'version:2@3.15.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_codehaus_mojo_animal_sniffer_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_codehaus_mojo_animal_sniffer_annotations',
              'version': 'version:2@1.21.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_conscrypt_conscrypt_openjdk_uber': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_conscrypt_conscrypt_openjdk_uber',
              'version': 'version:2@2.5.2.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_eclipse_jgit_org_eclipse_jgit': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_eclipse_jgit_org_eclipse_jgit',
              'version': 'version:2@4.4.1.201607150455-r.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_hamcrest_hamcrest': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_hamcrest_hamcrest',
              'version': 'version:2@2.2.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_jetbrains_kotlin_kotlin_stdlib_jdk7': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_jetbrains_kotlin_kotlin_stdlib_jdk7',
              'version': 'version:2@1.8.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_jetbrains_kotlin_kotlin_stdlib_jdk8': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_jetbrains_kotlin_kotlin_stdlib_jdk8',
              'version': 'version:2@1.8.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_jetbrains_kotlinx_kotlinx_coroutines_android': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_jetbrains_kotlinx_kotlinx_coroutines_android',
              'version': 'version:2@1.6.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_jetbrains_kotlinx_kotlinx_coroutines_core_jvm': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_jetbrains_kotlinx_kotlinx_coroutines_core_jvm',
              'version': 'version:2@1.6.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_jetbrains_kotlinx_kotlinx_metadata_jvm': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_jetbrains_kotlinx_kotlinx_metadata_jvm',
              'version': 'version:2@0.1.0.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_jsoup_jsoup': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_jsoup_jsoup',
              'version': 'version:2@1.15.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_mockito_mockito_android': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_mockito_mockito_android',
              'version': 'version:2@5.1.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_mockito_mockito_core': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_mockito_mockito_core',
              'version': 'version:2@5.1.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_mockito_mockito_subclass': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_mockito_mockito_subclass',
              'version': 'version:2@5.1.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_objenesis_objenesis': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_objenesis_objenesis',
              'version': 'version:2@3.3.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_ow2_asm_asm': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_ow2_asm_asm',
              'version': 'version:2@9.2.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_ow2_asm_asm_analysis': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_ow2_asm_asm_analysis',
              'version': 'version:2@9.2.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_ow2_asm_asm_commons': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_ow2_asm_asm_commons',
              'version': 'version:2@9.2.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_ow2_asm_asm_tree': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_ow2_asm_asm_tree',
              'version': 'version:2@9.2.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_ow2_asm_asm_util': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_ow2_asm_asm_util',
              'version': 'version:2@9.2.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_pcollections_pcollections': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_pcollections_pcollections',
              'version': 'version:2@3.1.4.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_robolectric_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_annotations',
              'version': 'version:2@4.8.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_robolectric_junit': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_junit',
              'version': 'version:2@4.8.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_robolectric_nativeruntime': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_nativeruntime',
              'version': 'version:2@4.8.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_robolectric_pluginapi': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_pluginapi',
              'version': 'version:2@4.8.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_robolectric_plugins_maven_dependency_resolver': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_plugins_maven_dependency_resolver',
              'version': 'version:2@4.8.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_robolectric_resources': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_resources',
              'version': 'version:2@4.8.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_robolectric_robolectric': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_robolectric',
              'version': 'version:2@4.8.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_robolectric_sandbox': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_sandbox',
              'version': 'version:2@4.8.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_robolectric_shadowapi': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_shadowapi',
              'version': 'version:2@4.8.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_robolectric_shadows_framework': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_shadows_framework',
              'version': 'version:2@4.8.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_robolectric_shadows_playservices': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_shadows_playservices',
              'version': 'version:2@4.8.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_robolectric_utils': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_utils',
              'version': 'version:2@4.8.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_robolectric_utils_reflector': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_utils_reflector',
              'version': 'version:2@4.8.1.cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  # === ANDROID_DEPS Generated Code End ===

  'src/tools/resultdb': {
      'packages': [
        {
          'package': 'infra/tools/result_adapter/${{platform}}',
          'version': Var('resultdb_version'),
        },
      ],
      'dep_type': 'cipd',
  },

  'src/tools/bazel/linux-amd64': {
    'packages': [{
       'package': 'infra/3pp/tools/bazel_bootstrap/linux-amd64',
       'version': 'version:2@5.3.2.1',
    }],
    'dep_type': 'cipd',
    'condition': 'checkout_linux and checkout_bazel',
  },

  'src/tools/bazel/mac-amd64': {
    'packages': [{
       'package': 'infra/3pp/tools/bazel_bootstrap/mac-amd64',
       'version': 'version:2@5.3.2.1',
    }],
    'dep_type': 'cipd',
    'condition': 'checkout_mac and checkout_bazel',
  },

  'src/tools/bazel/mac-arm64': {
    'packages': [{
       'package': 'infra/3pp/tools/bazel_bootstrap/mac-arm64',
       'version': 'version:2@5.3.2.1',
    }],
    'dep_type': 'cipd',
    'condition': 'checkout_mac and checkout_bazel',
  },

  'src/tools/bazel/windows-amd64': {
    'packages': [{
       'package': 'infra/3pp/tools/bazel_bootstrap/windows-amd64',
       'version': 'version:2@5.3.2',
    }],
    'dep_type': 'cipd',
    'condition': 'checkout_win and checkout_bazel',
  },
}


include_rules = [
  # Everybody can use some things.
  # NOTE: THIS HAS TO STAY IN SYNC WITH third_party/DEPS which disallows these.
  '+base',
  '+build',
  '+ipc',
  # perfetto is base's public dependency.
  '+third_party/perfetto/include/perfetto/tracing',
  '+third_party/perfetto/include/perfetto/test',

  # Everybody can use headers generated by tools/generate_library_loader.
  '+library_loaders',

  '+testing',
  '+third_party/google_benchmark/src/include/benchmark/benchmark.h',
  '+third_party/icu/source/common/unicode',
  '+third_party/icu/source/i18n/unicode',
  '+url',

  # Abseil is allowed by default, but some features are banned. See
  # //styleguide/c++/c++-features.md.
  '+third_party/abseil-cpp',
  '-third_party/abseil-cpp/absl/algorithm/container.h',
  '-third_party/abseil-cpp/absl/container',
  '-third_party/abseil-cpp/absl/crc',
  '-third_party/abseil-cpp/absl/flags',
  '-third_party/abseil-cpp/absl/functional/any_invocable.h',
  '-third_party/abseil-cpp/absl/functional/bind_front.h',
  '-third_party/abseil-cpp/absl/functional/function_ref.h',
  '-third_party/abseil-cpp/absl/hash',
  '-third_party/abseil-cpp/absl/log',
  '-third_party/abseil-cpp/absl/random',
  '-third_party/abseil-cpp/absl/status/statusor.h',
  '-third_party/abseil-cpp/absl/strings',
  '+third_party/abseil-cpp/absl/strings/cord.h',
  '-third_party/abseil-cpp/absl/synchronization',
  '-third_party/abseil-cpp/absl/time',
  '-third_party/abseil-cpp/absl/types/any.h',
  '-third_party/abseil-cpp/absl/types/span.h',
]


# checkdeps.py shouldn't check include paths for files in these dirs:
skip_child_includes = [
  'native_client_sdk',
  'out',
  'skia',
  'testing',
  'third_party/abseil-cpp',
  'v8',
]


hooks = [
  # Download and initialize "vpython" VirtualEnv environment packages for
  # Python3. We do this before running any other hooks so that any other
  # hooks that might use vpython don't trip over unexpected issues and
  # don't run slower than they might otherwise need to.
  {
    'name': 'vpython3_common',
    'pattern': '.',
    'action': [ 'vpython3',
                '-vpython-spec', 'src/.vpython3',
                '-vpython-tool', 'install',
    ],
  },
  {
    # This clobbers when necessary (based on get_landmines.py). This should
    # run as early as possible so that other things that get/generate into the
    # output directory will not subsequently be clobbered.
    'name': 'landmines',
    'pattern': '.',
    'action': [
        'python3',
        'src/build/landmines.py',
    ],
  },
  {
    # This clobbers when necessary (based on the internal ios version of
    # get_landmines.py). This should run as early as possible so that
    # other things that get/generate into the output directory will not
    # subsequently be clobbered. This script is only run# for iOS build
    # with src_internal.
    'name': 'landmines_ios_internal',
    'pattern': '.',
    'condition': 'checkout_ios and checkout_src_internal',
    'action': [
        'python3',
        'src/build/landmines.py',
        '--landmine-scripts',
        'src/ios_internal/build/get_landmines.py',
        '--landmines-path',
        'src/ios_internal/.landmines',
    ],
  },
  {
    # Ensure that the DEPS'd "depot_tools" has its self-update capability
    # disabled.
    'name': 'disable_depot_tools_selfupdate',
    'pattern': '.',
    'action': [
        'python3',
        'src/third_party/depot_tools/update_depot_tools_toggle.py',
        '--disable',
    ],
  },
  {
    # Ensure that we don't accidentally reference any .pyc files whose
    # corresponding .py files have since been deleted.
    # We could actually try to avoid generating .pyc files, crbug.com/500078.
    'name': 'remove_stale_pyc_files',
    'pattern': '.',
    'action': [
        'python3',
        'src/tools/remove_stale_pyc_files.py',
        'src/android_webview/tools',
        'src/build/android',
        'src/gpu/gles2_conform_support',
        'src/infra',
        'src/ppapi',
        'src/printing',
        'src/third_party/blink/renderer/build/scripts',
        'src/third_party/blink/tools',  # See http://crbug.com/625877.
        'src/third_party/catapult',
        'src/third_party/mako', # Some failures triggered by crrev.com/c/3686969
        'src/tools',
    ],
  },
  {
    # This downloads binaries for Native Client's newlib toolchain.
    # Done in lieu of building the toolchain from scratch as it can take
    # anywhere from 30 minutes to 4 hours depending on platform to build.
    'name': 'nacltools',
    'pattern': '.',
    'condition': 'checkout_nacl',
    'action': [
        'python3',
        'src/build/download_nacl_toolchains.py',
        '--mode', 'nacl_core_sdk',
        'sync', '--extract',
    ],
  },
  {
    'name': 'sysroot_arm',
    'pattern': '.',
    'condition': 'checkout_linux and checkout_arm',
    'action': ['python3', 'src/build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=arm'],
  },
  {
    'name': 'sysroot_arm64',
    'pattern': '.',
    'condition': 'checkout_linux and checkout_arm64',
    'action': ['python3', 'src/build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=arm64'],
  },
  {
    'name': 'sysroot_x86',
    'pattern': '.',
    'condition': 'checkout_linux and (checkout_x86 or checkout_x64)',
    'action': ['python3', 'src/build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=x86'],
  },
  {
    'name': 'sysroot_mips',
    'pattern': '.',
    'condition': 'checkout_linux and checkout_mips',
    'action': ['python3', 'src/build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=mips'],
  },
  {
    'name': 'sysroot_mips64',
    'pattern': '.',
    'condition': 'checkout_linux and checkout_mips64',
    'action': ['python3', 'src/build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=mips64el'],
  },

  {
    'name': 'sysroot_x64',
    'pattern': '.',
    'condition': 'checkout_linux and checkout_x64',
    'action': ['python3', 'src/build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=x64'],
  },
  {
    # Case-insensitivity for the Win SDK. Must run before win_toolchain below.
    'name': 'ciopfs_linux',
    'pattern': '.',
    'condition': 'checkout_win and host_os == "linux"',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-browser-clang/ciopfs',
                '-s', 'src/build/ciopfs.sha1',
    ]
  },
  {
    # Update the Windows toolchain if necessary.  Must run before 'clang' below.
    'name': 'win_toolchain',
    'pattern': '.',
    'condition': 'checkout_win',
    'action': ['python3', 'src/build/vs_toolchain.py', 'update', '--force'],
  },
  {
    # Update the Mac toolchain if necessary.
    'name': 'mac_toolchain',
    'pattern': '.',
    'condition': 'checkout_mac or checkout_ios',
    'action': ['python3', 'src/build/mac_toolchain.py'],
  },
  {
    # Update the prebuilt clang toolchain.
    # Note: On Win, this should run after win_toolchain, as it may use it.
    'name': 'clang',
    'pattern': '.',
    'condition': 'not llvm_force_head_revision',
    'action': ['python3', 'src/tools/clang/scripts/update.py'],
  },
  {
    # Update prebuilt Rust toolchain.
    'name': 'rust-toolchain',
    'pattern': '.',
    'condition': 'checkout_rust',
    'action': ['python3', 'src/tools/rust/update_rust.py'],
  },
  {
    # Build the clang toolchain from tip-of-tree.
    # Note: On Win, this should run after win_toolchain, as it may use it.
    'name': 'clang_tot',
    'pattern': '.',
    'condition': 'llvm_force_head_revision',
    'action': ['python3', 'src/tools/clang/scripts/build.py',
               '--llvm-force-head-revision',
               '--with-android={checkout_android}',
               '--with-fuchsia={checkout_fuchsia}'],
  },
  {
    # This is supposed to support the same set of platforms as 'clang' above.
    'name': 'clang_coverage',
    'pattern': '.',
    'condition': 'checkout_clang_coverage_tools',
    'action': ['python3', 'src/tools/clang/scripts/update.py',
               '--package=coverage_tools'],
  },
  {
    # This is also supposed to support the same set of platforms as 'clang'
    # above. LLVM ToT support isn't provided at the moment.
    'name': 'clang_tidy',
    'pattern': '.',
    'condition': 'checkout_clang_tidy',
    'action': ['python3', 'src/tools/clang/scripts/update.py',
               '--package=clang-tidy'],
  },
  {
    # Grab the libraries and header files of the clang compiler that will be
    # used to build Chromium. These can be used to build clang tooling for
    # static analysis or codegen.
    'name': 'clang_libs',
    'pattern': '.',
    'condition': 'checkout_clang_libs',
    'action': ['python3', 'src/tools/clang/scripts/update.py',
               '--package=clang-libs'],
  },
  {
    # This is also supposed to support the same set of platforms as 'clang'
    # above. LLVM ToT support isn't provided at the moment.
    'name': 'clangd',
    'pattern': '.',
    'condition': 'checkout_clangd',
    'action': ['python3', 'src/tools/clang/scripts/update.py',
               '--package=clangd'],
  },
  {
    # Build experimental in-tree Rust toolchain. Must run after clang_libs or
    # clang_tot hook. This should only be run on bots. Syncing clang-libs above
    # and passing `--use-final-llvm-build-dir` links rustc against the LLVM libs
    # in clang-libs
    'name': 'build_rust',
    'pattern': '.',
    'condition': 'build_chromium_rust_toolchain',
    'action': ['python3', 'src/tools/rust/build_rust.py',
               '--use-final-llvm-build-dir'],
  },
  {
    # Should run after the clang hook. Used on mac, as well as for orderfile
    # generation and size tooling on Android. Used by
    # dump-static-initializers.py on linux.
    'name': 'objdump',
    'pattern': '.',
    'condition': 'checkout_linux or checkout_mac or checkout_android and host_os != "mac"',
    'action': ['python3', 'src/tools/clang/scripts/update.py',
               '--package=objdump'],
  },
  {
    # Update LASTCHANGE.
    'name': 'lastchange',
    'pattern': '.',
    'action': ['python3', 'src/build/util/lastchange.py',
               '-o', 'src/build/util/LASTCHANGE'],
  },
  {
    # Update GPU lists version string (for gpu/config).
    'name': 'gpu_lists_version',
    'pattern': '.',
    'action': ['python3', 'src/build/util/lastchange.py',
               '-m', 'GPU_LISTS_VERSION',
               '--revision-id-only',
               '--header', 'src/gpu/config/gpu_lists_version.h'],
  },
  {
    # Update skia_commit_hash.h.
    'name': 'lastchange_skia',
    'pattern': '.',
    'action': ['python3', 'src/build/util/lastchange.py',
               '-m', 'SKIA_COMMIT_HASH',
               '-s', 'src/third_party/skia',
               '--header', 'src/skia/ext/skia_commit_hash.h'],
  },
  {
    # Update dawn_version.h.
    'name': 'lastchange_dawn',
    'pattern': '.',
    'action': ['python3', 'src/build/util/lastchange.py',
               '-s', 'src/third_party/dawn',
               '--revision', 'src/gpu/webgpu/DAWN_VERSION'],
  },
  # Pull dsymutil binaries using checked-in hashes.
  {
    'name': 'dsymutil_mac_arm64',
    'pattern': '.',
    'condition': 'host_os == "mac" and host_cpu == "arm64"',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-browser-clang',
                '-s', 'src/tools/clang/dsymutil/bin/dsymutil.arm64.sha1',
                '-o', 'src/tools/clang/dsymutil/bin/dsymutil',
    ],
  },
  {
    'name': 'dsymutil_mac_x64',
    'pattern': '.',
    'condition': 'host_os == "mac" and host_cpu == "x64"',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-browser-clang',
                '-s', 'src/tools/clang/dsymutil/bin/dsymutil.x64.sha1',
                '-o', 'src/tools/clang/dsymutil/bin/dsymutil',
    ],
  },

  # Pull clang-format binaries using checked-in hashes.
  {
    'name': 'clang_format_win',
    'pattern': '.',
    'condition': 'host_os == "win"',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-clang-format',
                '-s', 'src/buildtools/win/clang-format.exe.sha1',
    ],
  },
  {
    'name': 'clang_format_mac_x64',
    'pattern': '.',
    'condition': 'host_os == "mac" and host_cpu == "x64"',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-clang-format',
                '-s', 'src/buildtools/mac/clang-format.x64.sha1',
                '-o', 'src/buildtools/mac/clang-format',
    ],
  },
  {
    'name': 'clang_format_mac_arm64',
    'pattern': '.',
    'condition': 'host_os == "mac" and host_cpu == "arm64"',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-clang-format',
                '-s', 'src/buildtools/mac/clang-format.arm64.sha1',
                '-o', 'src/buildtools/mac/clang-format',
    ],
  },
  {
    'name': 'clang_format_linux',
    'pattern': '.',
    'condition': 'host_os == "linux"',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-clang-format',
                '-s', 'src/buildtools/linux64/clang-format.sha1',
    ],
  },
  # Pull rc binaries using checked-in hashes.
  {
    'name': 'rc_win',
    'pattern': '.',
    'condition': 'checkout_win and host_os == "win"',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-browser-clang/rc',
                '-s', 'src/build/toolchain/win/rc/win/rc.exe.sha1',
    ],
  },
  {
    'name': 'rc_mac',
    'pattern': '.',
    'condition': 'checkout_win and host_os == "mac"',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-browser-clang/rc',
                '-s', 'src/build/toolchain/win/rc/mac/rc.sha1',
    ],
  },
  {
    'name': 'rc_linux',
    'pattern': '.',
    'condition': 'checkout_win and host_os == "linux"',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-browser-clang/rc',
                '-s', 'src/build/toolchain/win/rc/linux64/rc.sha1',
    ]
  },
 {
    'name': 'test_fonts',
    'pattern': '.',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--extract',
                '--no_auth',
                '--bucket', 'chromium-fonts',
                '-s', 'src/third_party/test_fonts/test_fonts.tar.gz.sha1',
    ],
  },
  # Download test resources for opus, i.e. audio files.
  {
    'name': 'opus_test_files',
    'pattern': '.',
    'action': ['python3',
               'src/third_party/depot_tools/download_from_google_storage.py',
               '--no_auth',
               '--quiet',
               '--bucket', 'chromium-webrtc-resources',
               '-d', 'src/third_party/opus/tests/resources'],
  },
  {
    'name': 'apache_win32',
    'pattern': '\\.sha1',
    'condition': 'checkout_win',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--directory',
                '--recursive',
                '--no_auth',
                '--num_threads=16',
                '--bucket', 'chromium-apache-win32',
                'src/third_party/apache-win32',
    ],
  },
  {
    'name': 'msan_chained_origins_focal',
    'pattern': '.',
    'condition': 'checkout_instrumented_libraries',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-instrumented-libraries',
                '-s', 'src/third_party/instrumented_libraries/binaries/msan-chained-origins-focal.tgz.sha1',
              ],
  },
  {
    'name': 'msan_no_origins_focal',
    'pattern': '.',
    'condition': 'checkout_instrumented_libraries',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-instrumented-libraries',
                '-s', 'src/third_party/instrumented_libraries/binaries/msan-no-origins-focal.tgz.sha1',
              ],
  },
  {
    'name': 'wasm_fuzzer',
    'pattern': '.',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '-u',
                '--bucket', 'v8-wasm-fuzzer',
                '-s', 'src/v8/test/fuzzer/wasm_corpus.tar.gz.sha1',
    ],
  },

  # Pull down Node binaries for WebUI toolchain.
  {
    'name': 'node_linux64',
    'pattern': '.',
    'condition': 'host_os == "linux"',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--extract',
                '--no_auth',
                '--bucket', 'chromium-nodejs/16.13.0',
                '-s', 'src/third_party/node/linux/node-linux-x64.tar.gz.sha1',
                ],
  },
  {
    'name': 'nw_patch',
    'pattern': '.',
    'action': [
      'python',
      'src/content/nw/tools/patcher.py'
      ],
  },
  # The Mac x64/arm64 binaries are downloaded regardless of host architecture
  # since it's possible to cross-compile for the other architecture. This can
  # cause problems for tests that use node if the test device architecture does
  # not match the architecture of the compile machine.
  {
    'name': 'node_mac',
    'pattern': '.',
    'condition': 'host_os == "mac"',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--extract',
                '--no_auth',
                '--bucket', 'chromium-nodejs/16.13.0',
                '-s', 'src/third_party/node/mac/node-darwin-x64.tar.gz.sha1',
    ],
  },
  {
    'name': 'node_mac_arm64',
    'pattern': '.',
    'condition': 'host_os == "mac"',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--extract',
                '--no_auth',
                '--bucket', 'chromium-nodejs/16.13.0',
                '-s', 'src/third_party/node/mac/node-darwin-arm64.tar.gz.sha1',
    ],
  },
  {
    'name': 'node_win',
    'pattern': '.',
    'condition': 'host_os == "win"',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-nodejs/16.13.0',
                '-s', 'src/third_party/node/win/node.exe.sha1',
    ],
  },

  # Pull down NPM dependencies for WebUI toolchain.
  {
    'name': 'webui_node_modules',
    'pattern': '.',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--extract',
                '--no_auth',
                '--bucket', 'chromium-nodejs',
                '-s', 'src/third_party/node/node_modules.tar.gz.sha1',
    ],
  },

  # NPM dependencies for JavaScript code coverage.
  {
    'name': 'js_coverage_node_modules',
    'condition': 'checkout_js_coverage_modules',
    'pattern': '.',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--extract',
                '--no_auth',
                '--bucket', 'chromium-nodejs/js_code_coverage',
                '-s', 'src/third_party/js_code_coverage/node_modules.tar.gz.sha1',
    ],
  },

  # Download Telemetry's binary dependencies via conditionals
  {
    'name': 'checkout_telemetry_binary_dependencies',
    'condition': 'checkout_telemetry_dependencies',
    'pattern': '.',
    'action': [ 'vpython3',
                'src/third_party/catapult/telemetry/bin/fetch_telemetry_binary_dependencies',
    ],
  },

  # Download Telemetry's benchmark binary dependencies via conditionals
  {
    'name': 'checkout_telemetry_benchmark_deps',
    'condition': 'checkout_telemetry_dependencies and checkout_linux and not checkout_android and not skip_wpr_archives_download',
    'pattern': '.',
    'action': [ 'vpython3',
                'src/tools/perf/fetch_benchmark_deps.py',
                '-f',
                '-p',
                'linux'
    ],
  },
  {
    'name': 'checkout_telemetry_benchmark_deps',
    'condition': 'checkout_telemetry_dependencies and checkout_win and not skip_wpr_archives_download',
    'pattern': '.',
    'action': [ 'vpython3',
                'src/tools/perf/fetch_benchmark_deps.py',
                '-f',
                '-p',
                'win'
    ],
  },
  {
    'name': 'checkout_telemetry_benchmark_deps',
    'condition': 'checkout_telemetry_dependencies and checkout_mac and not skip_wpr_archives_download',
    'pattern': '.',
    'action': [ 'vpython3',
                'src/tools/perf/fetch_benchmark_deps.py',
                '-f',
                '-p',
                'mac'
    ],
  },
  {
    'name': 'checkout_telemetry_benchmark_deps',
    'condition': 'checkout_telemetry_dependencies and checkout_android and not skip_wpr_archives_download',
    'pattern': '.',
    'action': [ 'vpython3',
                'src/tools/perf/fetch_benchmark_deps.py',
                '-f',
                '-p',
                'android'
    ],
  },

  # Download test data for Maps telemetry_gpu_integration_test.
  {
    'name': 'maps_perf_test_load_dataset',
    'pattern': '\\.sha1',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-telemetry',
                '-s', 'src/tools/perf/page_sets/maps_perf_test/load_dataset.sha1',
    ],
  },

  # Pull down Zucchini test data.
  {
    'name': 'zucchini_testdata',
    'pattern': '.',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--num_threads=4',
                '--bucket', 'chromium-binary-patching/zucchini_testdata',
                '--recursive',
                '-d', 'src/components/zucchini',
    ],
  },
  # Pull down WPR Archive files
  {
    'name': 'Fetch WPR archive files',
    'pattern': '.',
    'condition': 'checkout_android and (checkout_wpr_archives or checkout_src_internal)',
    'action': [ 'python3',
                'src/chrome/test/data/android/manage_wpr_archives.py',
                'download',
    ],
  },
  {
    'name': 'Fetch Android AFDO profile',
    'pattern': '.',
    'condition': 'checkout_android',
    'action': [ 'python3',
                'src/tools/download_optimization_profile.py',
                '--newest_state=src/chrome/android/profiles/newest.txt',
                '--local_state=src/chrome/android/profiles/local.txt',
                '--output_name=src/chrome/android/profiles/afdo.prof',
                '--gs_url_base=chromeos-prebuilt/afdo-job/llvm',
    ],
  },
  {
    'name': 'Fetch Android Arm AFDO profile',
    'pattern': '.',
    'condition': 'checkout_android',
    'action': [ 'python3',
                'src/tools/download_optimization_profile.py',
                '--newest_state=src/chrome/android/profiles/arm.newest.txt',
                '--local_state=src/chrome/android/profiles/arm.local.txt',
                '--output_name=src/chrome/android/profiles/arm.afdo.prof',
                '--gs_url_base=chromeos-prebuilt/afdo-job/llvm',
    ],
  },
  {
    'name': 'gvr_static_shim_android',
    'pattern': '\\.sha1',
    'condition': 'checkout_android',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-gvr-static-shim',
                '-d', 'src/third_party/gvr-android-sdk',
    ],
  },
  {
    'name': 'vr_controller_test_api',
    'pattern': '\\.sha1',
    'condition': 'checkout_android',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-gvr-static-shim/controller_test_api',
                '-s', 'src/third_party/gvr-android-sdk/test-libraries/controller_test_api.aar.sha1',
    ],
  },
  # Download and unpack MediaPipe Integration tests.
  {
    'name': 'mediapipe_integration_testdata',
    'pattern': '.',
    'action': [ 'python3',
                'src/content/test/gpu/gpu_tests/mediapipe_update.py',
    ],
  },
  # Download VR test APKs only if the environment variable is set
  {
    'name': 'vr_test_apks',
    'pattern': '.',
    'condition': 'checkout_android',
    'action': [ 'python3',
                'src/third_party/gvr-android-sdk/test-apks/update.py',
    ],
  },
  # DOWNLOAD AR test APKs only if the environment variable is set
  {
    'name': 'ar_test_apks',
    'pattern': '.',
    'condition': 'checkout_android',
    'action': [ 'python3',
                'src/third_party/arcore-android-sdk/test-apks/update.py',
    ],
  },
  {
    'name': 'subresource-filter-ruleset',
    'pattern': '.',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-ads-detection',
                '-s', 'src/third_party/subresource-filter-ruleset/data/UnindexedRules.sha1',
    ],
  },
  {
    'name': 'Fetch ChromeOS-specific orderfile for Chrome',
    'pattern': '.',
    'condition': 'checkout_chromeos or checkout_simplechrome',
    'action': [ 'python3',
                'src/tools/download_optimization_profile.py',
                '--newest_state=src/chromeos/profiles/orderfile.newest.txt',
                '--local_state=src/chromeos/profiles/orderfile.local.txt',
                '--output_name=src/chromeos/profiles/chromeos.orderfile.txt',
                '--gs_url_base=chromeos-prebuilt/afdo-job/orderfiles/vetted',
    ],
  },
  # Download AFDO profiles for ChromeOS for each architecture.
  {
    'name': 'Fetch ChromeOS AFDO profiles (from Intel Atom cores)',
    'pattern': '.',
    'condition': 'checkout_chromeos or checkout_simplechrome',
    'action': [ 'python3',
                'src/tools/download_optimization_profile.py',
                '--newest_state=src/chromeos/profiles/atom.afdo.newest.txt',
                '--local_state=src/chromeos/profiles/atom.afdo.local.txt',
                '--output_name=src/chromeos/profiles/atom.afdo.prof',
                '--gs_url_base=chromeos-prebuilt/afdo-job/vetted/release',
    ],
  },
  {
    'name': 'Fetch ChromeOS AFDO profiles (from Intel Big cores)',
    'pattern': '.',
    'condition': 'checkout_chromeos or checkout_simplechrome',
    'action': [ 'python3',
                'src/tools/download_optimization_profile.py',
                '--newest_state=src/chromeos/profiles/bigcore.afdo.newest.txt',
                '--local_state=src/chromeos/profiles/bigcore.afdo.local.txt',
                '--output_name=src/chromeos/profiles/bigcore.afdo.prof',
                '--gs_url_base=chromeos-prebuilt/afdo-job/vetted/release',
    ],
  },
  {
    'name': 'Fetch ChromeOS AFDO profiles (from Arm)',
    'pattern': '.',
    'condition': 'checkout_chromeos or checkout_simplechrome',
    'action': [ 'python3',
                'src/tools/download_optimization_profile.py',
                '--newest_state=src/chromeos/profiles/arm.afdo.newest.txt',
                '--local_state=src/chromeos/profiles/arm.afdo.local.txt',
                '--output_name=src/chromeos/profiles/arm.afdo.prof',
                '--gs_url_base=chromeos-prebuilt/afdo-job/vetted/release',
    ],
  },
  {
    'name': 'Fetch ChromeOS AFDO profiles (experimental Arm profile)',
    'pattern': '.',
    'condition': 'checkout_chromeos or checkout_simplechrome',
    'action': [ 'python3',
                'src/tools/download_optimization_profile.py',
                '--newest_state=src/chromeos/profiles/arm-exp.afdo.newest.txt',
                '--local_state=src/chromeos/profiles/arm-exp.afdo.local.txt',
                '--output_name=src/chromeos/profiles/arm-exp.afdo.prof',
                '--gs_url_base=chromeos-prebuilt/afdo-job/vetted/release',
    ],
  },
  {
    # Pull doclava binaries if building for Android.
    'name': 'doclava',
    'pattern': '.',
    'condition': 'checkout_android',
    'action': [ 'python3',
                'src/build/android/download_doclava.py',
    ],
  },

  {
    'name': 'Download Fuchsia SDK from GCS',
    'pattern': '.',
    'condition': 'checkout_fuchsia',
    'action': [
      'python3',
      'src/build/fuchsia/update_sdk.py',
      '--cipd-prefix={fuchsia_sdk_cipd_prefix}',
      '--version={fuchsia_version}',
    ],
  },

  {
    'name': 'Download Fuchsia system images',
    'pattern': '.',
    'condition': 'checkout_fuchsia and checkout_fuchsia_product_bundles',
    'action': [
      'python3',
      'src/build/fuchsia/update_product_bundles.py',
      '{checkout_fuchsia_boot_images}',
    ],
  },

  {
    'name': 'cros_simplechrome_artifacts_with_vm',
    'pattern': '.',
    'condition': 'checkout_simplechrome_with_vms and not checkout_src_internal',
    'action': [
      'vpython3',
      'src/third_party/chromite/bin/cros',
      'chrome-sdk',
      '--fallback-versions=20',
      '--nogoma',
      '--nogn-gen',
      '--no-shell',
      '--log-level=warning',
      '--cache-dir=src/build/cros_cache/',
      '--use-external-config',
      '--boards={cros_boards_with_qemu_images}',
      '--download-vm',
    ],
  },
  {
    'name': 'cros_simplechrome_artifacts_with_no_vm',
    'pattern': '.',
    'condition': 'checkout_simplechrome and not checkout_src_internal',
    'action': [
      'vpython3',
      'src/third_party/chromite/bin/cros',
      'chrome-sdk',
      '--fallback-versions=20',
      '--nogoma',
      '--nogn-gen',
      '--no-shell',
      '--log-level=warning',
      '--cache-dir=src/build/cros_cache/',
      '--use-external-config',
      '--boards={cros_boards}',
    ],
  },
  {
    'name': 'cros_simplechrome_artifacts_with_no_vm_internal',
    'pattern': '.',
    'condition': 'checkout_simplechrome and checkout_src_internal',
    'action': [
      'vpython3',
      'src/third_party/chromite/bin/cros',
      'chrome-sdk',
      '--fallback-versions=20',
      '--nogoma',
      '--nogn-gen',
      '--no-shell',
      '--log-level=warning',
      '--cache-dir=src/build/cros_cache/',
      '--boards={cros_boards}',
    ],
  },
  {
    'name': 'cros_simplechrome_artifacts_with_vm_internal',
    'pattern': '.',
    'condition': 'checkout_simplechrome_with_vms and checkout_src_internal',
    'action': [
      'vpython3',
      'src/third_party/chromite/bin/cros',
      'chrome-sdk',
      '--fallback-versions=20',
      '--nogoma',
      '--nogn-gen',
      '--no-shell',
      '--log-level=warning',
      '--cache-dir=src/build/cros_cache/',
      '--boards={cros_boards_with_qemu_images}',
      '--download-vm',
    ],
  },
  # Download Lacros's version of the simplechrome sdks. VMs are disregarded
  # because this version of sdk is only used for compiling Lacros.
  {
    'name': 'cros_simplechrome_artifacts_with_vm for lacros',
    'pattern': '.',
    'condition': 'checkout_simplechrome_with_vms and not checkout_src_internal and checkout_lacros_sdk',
    'action': [
      'vpython3',
      'src/third_party/chromite/bin/cros',
      'chrome-sdk',
      '--fallback-versions=20',
      '--nogoma',
      '--nogn-gen',
      '--no-shell',
      '--log-level=warning',
      '--cache-dir=src/build/cros_cache/',
      '--use-external-config',
      '--boards={cros_boards_with_qemu_images}',
      '--is-lacros',
      '--version={lacros_sdk_version}',
    ],
  },
  {
    'name': 'cros_simplechrome_artifacts_with_no_vm for lacros',
    'pattern': '.',
    'condition': 'checkout_simplechrome and not checkout_src_internal and checkout_lacros_sdk',
    'action': [
      'vpython3',
      'src/third_party/chromite/bin/cros',
      'chrome-sdk',
      '--fallback-versions=20',
      '--nogoma',
      '--nogn-gen',
      '--no-shell',
      '--log-level=warning',
      '--cache-dir=src/build/cros_cache/',
      '--use-external-config',
      '--boards={cros_boards}',
      '--is-lacros',
      '--version={lacros_sdk_version}',
    ],
  },
  {
    'name': 'cros_simplechrome_artifacts_with_vm_internal for lacros',
    'pattern': '.',
    'condition': 'checkout_simplechrome_with_vms and checkout_src_internal and checkout_lacros_sdk',
    'action': [
      'vpython3',
      'src/third_party/chromite/bin/cros',
      'chrome-sdk',
      '--fallback-versions=20',
      '--nogoma',
      '--nogn-gen',
      '--no-shell',
      '--log-level=warning',
      '--cache-dir=src/build/cros_cache/',
      '--boards={cros_boards_with_qemu_images}',
      '--is-lacros',
      '--version={lacros_sdk_version}',
    ],
  },
  {
    'name': 'cros_simplechrome_artifacts_with_no_vm_internal for lacros',
    'pattern': '.',
    'condition': 'checkout_simplechrome and checkout_src_internal and checkout_lacros_sdk',
    'action': [
      'vpython3',
      'src/third_party/chromite/bin/cros',
      'chrome-sdk',
      '--fallback-versions=20',
      '--nogoma',
      '--nogn-gen',
      '--no-shell',
      '--log-level=warning',
      '--cache-dir=src/build/cros_cache/',
      '--boards={cros_boards}',
      '--is-lacros',
      '--version={lacros_sdk_version}',
    ],
  },

  # Download PGO profiles.
  {
    'name': 'Fetch PGO profiles for win32',
    'pattern': '.',
    'condition': 'checkout_pgo_profiles and checkout_win',
    'action': [ 'python3',
                'src/tools/update_pgo_profiles.py',
                '--target=win32',
                'update',
                '--gs-url-base=chromium-optimization-profiles/pgo_profiles',
    ],
  },
  {
    'name': 'Fetch PGO profiles for win64',
    'pattern': '.',
    'condition': 'checkout_pgo_profiles and checkout_win',
    'action': [ 'python3',
                'src/tools/update_pgo_profiles.py',
                '--target=win64',
                'update',
                '--gs-url-base=chromium-optimization-profiles/pgo_profiles',
    ],
  },
  {
    'name': 'Fetch PGO profiles for mac',
    'pattern': '.',
    'condition': 'checkout_pgo_profiles and (checkout_mac or checkout_fuchsia)',
    'action': [ 'python3',
                'src/tools/update_pgo_profiles.py',
                '--target=mac',
                'update',
                '--gs-url-base=chromium-optimization-profiles/pgo_profiles',
    ],
  },
  {
    'name': 'Fetch PGO profiles for mac arm',
    'pattern': '.',
    'condition': 'checkout_pgo_profiles and (checkout_mac or checkout_android or checkout_fuchsia)',
    'action': [ 'python3',
                'src/tools/update_pgo_profiles.py',
                '--target=mac-arm',
                'update',
                '--gs-url-base=chromium-optimization-profiles/pgo_profiles',
    ],
  },
  {
    'name': 'Fetch PGO profiles for linux',
    'pattern': '.',
    'condition': 'checkout_pgo_profiles and checkout_linux',
    'action': [ 'python3',
                'src/tools/update_pgo_profiles.py',
                '--target=linux',
                'update',
                '--gs-url-base=chromium-optimization-profiles/pgo_profiles',
    ],
  },
  {
    'name': 'Fetch PGO profiles for V8 builtins',
    'pattern': '.',
    # Always download profiles on Android builds. The GN arg `is_official_build`
    # is required to consider the profiles during build time.
    'condition': 'checkout_pgo_profiles or checkout_android',
    'action': [ 'python3',
                'src/v8/tools/builtins-pgo/download_profiles.py',
                'download',
                '--depot-tools',
                'src/third_party/depot_tools',
    ],
  },

  # Download Cast3p Binaries
  {
    'name': 'cast3p_binaries',
    'pattern': '.',
    'action': [
      'python3',
      'src/tools/cast3p/update_binaries.py',
    ],
    'condition': 'checkout_cast3p',
  },

  {
    'name': 'Generate location tags for tests',
    'pattern': '.',
    'action': [
      'python3',
      'src/testing/generate_location_tags.py',
      '--out',
      'src/testing/location_tags.json',
    ],
    'condition': 'generate_location_tags',
  },

  {
    # Clean up build dirs for crbug.com/1337238.
    # After a libc++ roll and revert, .ninja_deps would get into a state
    # that breaks Ninja on Windows.
    # TODO(crbug.com/1337238): Remove in a month or so.
    'name': 'del_ninja_deps_cache',
    'pattern': '.',
    'condition': 'host_os == "win"',
    'action': ['python3', 'src/build/del_ninja_deps_cache.py'],
  },
  # Download test resources for the style perftest.
  {
    'name': 'style_perftest_files',
    'pattern': '.',
    'action': ['python3',
               'src/third_party/depot_tools/download_from_google_storage.py',
               '--no_auth',
               '--quiet',
               '--bucket', 'chromium-style-perftest',
               '-d', 'src/third_party/blink/renderer/core/css/perftest_data'],
  },
  # Download remote exec cfg files
  {
    'name': 'fetch_reclient_cfgs',
    'pattern': '.',
    'condition': 'download_remoteexec_cfg',
    'action': ['python3',
               'src/buildtools/reclient_cfgs/fetch_reclient_cfgs.py',
               '--rbe_instance',
               Var('rbe_instance'),
               '--reproxy_cfg_template',
               'reproxy.cfg.template',
               '--rewrapper_cfg_project',
               Var('rewrapper_cfg_project'),
               '--quiet',
               '--hook',
               ],
  },
]

# Add any corresponding DEPS files from this list to chromium.exclusions in
# //testing/buildbot/trybot_analyze_config.json
# ctx: https://crbug.com/1201994
recursedeps = [
  # ANGLE manages DEPS that it also owns the build files for, such as dEQP.
  'src/third_party/angle',
  # Dawn manages DEPS for its copy of the WebGPU CTS as well as GLFW for which
  # it has build files.
  'src/third_party/dawn',
  'src/third_party/devtools-frontend-internal',
  'src/third_party/openscreen/src',
  'src/third_party/vulkan-deps',
  # src-internal has its own DEPS file to pull additional internal repos
  'src-internal',
  # clank has its own DEPS file, does not need to be in trybot_analyze_config
  # since the roller does not run tests.
  'src/clank',
]
