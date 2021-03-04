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
  'checkout_fuchsia_for_arm64_host',
  'checkout_google_benchmark',
  'checkout_ios_webkit',
  'checkout_nacl',
  'checkout_openxr',
  'cros_boards',
  'cros_boards_with_qemu_images',
]


vars = {
  "buildspec_platforms": "linux64, mac64, win, win64, android",
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

  # Pull in Android prebuilts build tools so we can create Java xrefs
  'checkout_android_prebuilts_build_tools': False,

  # Pull in Android native toolchain dependencies for Chrome OS too, so we can
  # build ARC++ support libraries.
  'checkout_android_native_support': 'checkout_android or checkout_chromeos',

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

  # By default, do not check out Google Benchmark. The library is only used by a
  # few specialized benchmarks that most developers do not interact with. Will
  # be overridden by gclient variables.
  'checkout_google_benchmark': False,

  # Check out and download nacl by default. This can be disabled e.g. with
  # custom_vars.
  'checkout_nacl': True,

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

  # By default checkout the OpenXR loader library only on Windows. The OpenXR
  # backend for VR in Chromium is currently only supported for Windows, but
  # support for other platforms may be added in the future.
  'checkout_openxr' : 'checkout_win',

  'checkout_traffic_annotation_tools': 'checkout_configuration != "small"',
  'checkout_instrumented_libraries': 'checkout_linux and checkout_configuration != "small"',

  # By default bot checkouts the WPR archive files only when this
  # flag is set True.
  'checkout_wpr_archives': False,

  # By default, do not check out WebKit for iOS, as it is not needed unless
  # running against ToT WebKit rather than system WebKit. This can be overridden
  # e.g. with custom_vars.
  'checkout_ios_webkit': False,

  # By default, do not checkout the re-client binaries.
  'checkout_reclient': False,

  # Fetches only the SDK boot images which match at least one of the whitelist
  # entries in a comma-separated list.
  #
  # Only the X64 and ARM64 QEMU images are downloaded by default. Developers
  # that need to boot on other target architectures or devices can opt to
  # download more boot images. Example of images include:
  #
  # Emulation:
  #   qemu.x64, qemu.arm64
  # Hardware:
  #   generic.x64, generic.arm64
  #
  # Wildcards are supported (e.g. "qemu.*").
  'checkout_fuchsia_boot_images': "qemu.x64,qemu.arm64",

  # By default, do not check out files required to run fuchsia tests in
  # qemu on linux-arm64 machines.
  'checkout_fuchsia_for_arm64_host': False,

  # By default, download the fuchsia sdk from the fuchsia GCS bucket.
  'fuchsia_sdk_bucket': 'fuchsia',

  # By default, download the fuchsia images from the fuchsia GCS bucket.
  'fuchsia_images_bucket': 'fuchsia',

  # Default to the empty board. Desktop Chrome OS builds don't need cros SDK
  # dependencies. Other Chrome OS builds should always define this explicitly.
  'cros_boards': Str(''),
  'cros_boards_with_qemu_images': Str(''),
  # Building for CrOS is only supported on linux currently.
  'checkout_simplechrome': '"{cros_boards}" != ""',
  'checkout_simplechrome_with_vms': '"{cros_boards_with_qemu_images}" != ""',

  'nw_src_revision': '14a57fa62f70b608b9fb4369817264a1b004ad78',
  'nw_v8_revision': '46b67d7a72dfad348dd975110b1180b9564c8537',
  'nw_node_revision': 'a68cb77e5fb163988418f253ae76e45b9284cf52',

  # ANGLE's deps are relative to the angle_root variable.
  'angle_root': 'src/third_party/angle',

  # luci-go CIPD package version.
  # Make sure the revision is uploaded by infra-packagers builder.
  # https://ci.chromium.org/p/infra-internal/g/infra-packagers/console
  'luci_go': 'git_revision:77944aa535e42e29faadf6cfa81aee252807d468',

  # This can be overridden, e.g. with custom_vars, to build clang from HEAD
  # instead of downloading the prebuilt pinned revision.
  'llvm_force_head_revision': False,

  # reclient CIPD package version
  'reclient_version': 're_client_version:0.17.0.7d283ef',

  'android_git': 'https://android.googlesource.com',
  'aomedia_git': 'https://aomedia.googlesource.com',
  'boringssl_git': 'https://boringssl.googlesource.com',
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
  'skia_revision': '109bfc9052ce1bde7acf07321d605601d7b7ec24',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling V8
  # and whatever else without interference from each other.
  'v8_revision': '6fbcc11c530bb348e805f6c5cfcd15c337640dc8',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling swarming_client
  # and whatever else without interference from each other.
  'swarming_revision': '1a072711d4388c62e02480fabc26c68c24494be9',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling ANGLE
  # and whatever else without interference from each other.
  'angle_revision': 'f25a0b5b404b0be266f9e312ea960a180b1a26e0',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling SwiftShader
  # and whatever else without interference from each other.
  'swiftshader_revision': '5255c465bbbab020b2fa98f818a705e764865869',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling PDFium
  # and whatever else without interference from each other.
  'pdfium_revision': '157601b9b923c7f8a32ffb770e1c97a89337b31d',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling BoringSSL
  # and whatever else without interference from each other.
  #
  # Note this revision should be updated with
  # third_party/boringssl/roll_boringssl.py, not roll-dep.
  'boringssl_revision': 'f8f35c95550e6796ab8b2c0f8e37e7ce4bc67302',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling google-toolbox-for-mac
  # and whatever else without interference from each other.
  'google_toolbox_for_mac_revision': 'aa1a3d2d447905999f119efbb70b3786c5eafa13',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling googletest
  # and whatever else without interference from each other.
  'googletest_revision': '1b0cdaae57c046c87fb99cb4f69c312a7e794adb',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling lighttpd
  # and whatever else without interference from each other.
  'lighttpd_revision': '9dfa55d15937a688a92cbf2b7a8621b0927d06eb',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling lss
  # and whatever else without interference from each other.
  'lss_revision': '29f7c7e018f4ce706a709f0b0afbf8bacf869480',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling NaCl
  # and whatever else without interference from each other.
  'nacl_revision': 'd28e6c52e05ea2f1b3231de473b6dbfce59bce40',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling freetype
  # and whatever else without interference from each other.
  'freetype_revision': '03ceda9701cd8c08ea5b4ee0c2d558a98fc4ed7d',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling freetype
  # and whatever else without interference from each other.
  'freetype_testing_revision': 'bf5001a755f7a1196c19ddb13b2a14cb5dfe10b7',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling HarfBuzz
  # and whatever else without interference from each other.
  'harfbuzz_revision': '53806e5b83cee0e275eac038d0780f95ac56588c',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling Emoji Segmenter
  # and whatever else without interference from each other.
  'emoji_segmenter_revision': '9ba6d25d0d9313569665d4a9d2b34f0f39f9a50e',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling catapult
  # and whatever else without interference from each other.
  'catapult_revision': '7231b2e72311bd24f5eaa2cc681d6c447f42fa68',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling libFuzzer
  # and whatever else without interference from each other.
  'libfuzzer_revision': 'debe7d2d1982e540fbd6bd78604bf001753f9e74',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling devtools-frontend
  # and whatever else without interference from each other.
  'devtools_frontend_revision': '0bc409770c3c442258f8dff63d7a2b7fcb05a9ab',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling libprotobuf-mutator
  # and whatever else without interference from each other.
  'libprotobuf-mutator': '8942a9ba43d8bb196230c321d46d6a137957a719',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling android_sdk_build-tools_version
  # and whatever else without interference from each other.
  'android_sdk_build-tools_version': '8LZujEmLjSh0g3JciDA3cslSptxKs9HOa_iUPXkOeYQC',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling android_sdk_emulator_version
  # and whatever else without interference from each other.
  'android_sdk_emulator_version': 'A4EvXZUIuQho0QRDJopMUpgyp6NA3aiDQjGKPUKbowMC',
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
  'android_sdk_platform-tools_version': '8tF0AOj7Dwlv4j7_nfkhxWB0jzrvWWYjEIpirt8FIWYC',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling android_sdk_platforms_version
  # and whatever else without interference from each other.
  'android_sdk_platforms_version': 'YMUu9EHNZ__2Xcxl-KsaSf-dI5TMt_P62IseUVsxktMC',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling android_sdk_sources_version
  # and whatever else without interference from each other.
  'android_sdk_sources_version': '4gxhM8E62bvZpQs7Q3d0DinQaW0RLCIefhXrQBFkNy8C',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'shaderc_revision': '61649cc4962e3fe042894d82f60ce7177050da61',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'dawn_revision': 'd2fb128052aec0a98a696c841ab6232c7323423d',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'quiche_revision': 'e84d83393be4536291ce719787f6a275db07b733',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling ios_webkit
  # and whatever else without interference from each other.
  'ios_webkit_revision': '59e9de61b7b36507836fa8b098e8839d7d995b13',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling libexpat
  # and whatever else without interference from each other.
  'libexpat_revision': 'e976867fb57a0cd87e3b0fe05d59e0ed63c6febb',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling wuffs
  # and whatever else without interference from each other.
  'wuffs_revision': 'aa46859ea40c72516deffa1b146121952d6dfd3b',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling libgifcodec
  # and whatever else without interference from each other.
  'libgifcodec_revision': 'd06d2a6d42baf6c0c91cacc28df2542a911d05fe',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling libavif
  # and whatever else without interference from each other.
  'libavif_revision': '93d5bf907aec1dec842b4b1dc81c44972129c411',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling nearby
  # and whatever else without interference from each other.
  'nearby_revision': '437f5f6f8dee3ec4ec886939b87ab4ab9ccbe3d0',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling securemessage
  # and whatever else without interference from each other.
  'securemessage_revision': '3955ec1700b9381a2562df35d48e42487a33d7a1',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling ukey2
  # and whatever else without interference from each other.
  'ukey2_revision': '0275885d8e6038c39b8a8ca55e75d1d4d1727f47',
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'tint_revision': '0f4638b9e499da6f012eb987679ceb37cda24213',

  # TODO(crbug.com/941824): The values below need to be kept in sync
  # between //DEPS and //buildtools/DEPS, so if you're updating one,
  # update the other. There is a presubmit check that checks that
  # you've done so; if you are adding new tools to //buildtools and
  # hence new revisions to this list, make sure you update the
  # _CheckBuildtoolsRevsAreInSync in PRESUBMIT.py to include the additional
  # revisions.

  # GN CIPD package version.
  'gn_version': 'git_revision:595e3be7c8381d4eeefce62a63ec12bae9ce5140',

  # Also, if you change these, update buildtools/DEPS too. Also update the
  # libc++ svn_revision in //buildtools/deps_revisions.gni.
  'clang_format_revision': '96636aa0e9f047f17447f2d45a094d0b59ed7917',
  'libcxx_revision':       'd9040c75cfea5928c804ab7c235fed06a63f743a',
  'libcxxabi_revision':    '196ba1aaa8ac285d94f4ea8d9836390a45360533',
  'libunwind_revision':    'd999d54f4bca789543a2eb6c995af2d9b5a1f3ed',

  # resultdb CIPD package version.
  'resultdb_version': 'git_revision:708b8c96ec364e58a15db4a9b5b89a9c7a78bcfb',
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
    Var('chromium_git') + '/chromium/llvm-project/cfe/tools/clang-format.git@' +
    Var('clang_format_revision'),
  'src/buildtools/linux64': {
    'packages': [
      {
        'package': 'gn/gn/linux-amd64',
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
        'package': 'infra/rbe/client/${{platform}}',
        'version': Var('reclient_version'),
      }
    ],
    'dep_type': 'cipd',
    'condition': '(host_os == "linux" or host_os == "windows") and checkout_reclient',
  },
  'src/chrome/browser/resources/media_router/extension/src':
    Var('chromium_git') + '/media_router.git' + '@' + 'd4389c097c61fa8c5aa5eb7c3ba34ee203dd62e3',

  'src/android_webview/tools/cts_archive': {
      'packages': [
          {
              'package': 'chromium/android_webview/tools/cts_archive',
              'version': 'ai8Ig4HlO0vG6aP_JP2uhyruE2yPzze8PFP1g8Z4_hgC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/chrome/installer/mac/third_party/xz/xz': {
      'url': Var('chromium_git') + '/chromium/deps/xz.git' + '@' + 'eecaf55632ca72e90eb2641376bce7cdbc7284f7',
      'condition': 'checkout_mac',
  },

  'src/tools/clang/dsymutil': {
    'packages': [
      {
        'package': 'chromium/llvm-build-tools/dsymutil',
        'version': 'M56jPzDv1620Rnm__jTMYS62Zi8rxHVq7yw0qeBFEgkC',
      }
    ],
    'condition': 'checkout_mac or checkout_ios',
    'dep_type': 'cipd',
  },

  'src/chrome/test/data/autofill/captured_sites': {
    'packages': [
      {
        'package': 'chromium/chrome/test/data/autofill/captured_sites',
        'version': 'd452hxzAmfQ3083xJD-zkRa9oE9kzWJUANy_T5-gt1IC',
      }
    ],
    'condition': 'checkout_chromium_autofill_test_dependencies',
    'dep_type': 'cipd',
  },

  'src/chrome/test/data/password/captured_sites': {
    'packages': [
      {
        'package': 'chromium/chrome/test/data/password/captured_sites',
        'version': 'MuT6UWjyB52nWFDuu4RCv4o_vMPIZdI4P2m2YsC66fAC',
      }
    ],
    'condition': 'checkout_chromium_password_manager_test_dependencies',
    'dep_type': 'cipd',
  },

  'src/chrome/test/data/perf/canvas_bench':
    Var('chromium_git') + '/chromium/canvas_bench.git' + '@' + 'a7b40ea5ae0239517d78845a5fc9b12976bfc732',

  'src/chrome/test/data/perf/frame_rate/content':
    Var('chromium_git') + '/chromium/frame_rate/content.git' + '@' + 'c10272c88463efeef6bb19c9ec07c42bc8fe22b9',

  'src/chrome/test/data/safe_browsing/dmg': {
    'packages': [
      {
        'package': 'chromium/chrome/test/data/safe_browsing/dmg',
        'version': 'a543ae3f0b3e67dd5a1c75f63317231a1d242912',
      },
    ],
    'condition': 'checkout_mac',
    'dep_type': 'cipd',
  },

  'src/chrome/test/data/xr/webvr_info':
    Var('chromium_git') + '/external/github.com/toji/webvr.info.git' + '@' + 'c58ae99b9ff9e2aa4c524633519570bf33536248',

  'src/ios/third_party/earl_grey/src': {
      'url': Var('chromium_git') + '/external/github.com/google/EarlGrey.git' + '@' + 'ff524fb646253b0cbc05aa6d0b327b6b3ddd0385',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/earl_grey2/src': {
      'url': Var('chromium_git') + '/external/github.com/google/EarlGrey.git' + '@' + '891b1b683a7ed73f98f8064672a95ca77060df42',
      'condition': 'checkout_ios',
  },

    'src/ios/third_party/edo/src': {
      'url': Var('chromium_git') + '/external/github.com/google/eDistantObject.git' + '@' + '91c532d921e1f37f7e0f213c534df16df6061ec1',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/gtx/src': {
      'url': Var('chromium_git') + '/external/github.com/google/GTXiLib.git' + '@' + '3e09baa61b2c13fe98029d53b1783f4ca9edaabf',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/fishhook/src': {
      'url': Var('chromium_git') + '/external/github.com/facebook/fishhook.git' + '@' + '66315a9c251edfe92f669ae2deeac0d75374c948',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/gcdwebserver/src': {
      'url': Var('chromium_git') + '/external/github.com/swisspol/GCDWebServer.git' + '@' + '43555c66627f6ed44817855a0f6d465f559d30e0',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/material_components_ios/src': {
      'url': Var('chromium_git') + '/external/github.com/material-components/material-components-ios.git' + '@' + '738a505dacefbb2371b5e0ae04989192eaa56370',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/material_font_disk_loader_ios/src': {
      'url': Var('chromium_git') + '/external/github.com/material-foundation/material-font-disk-loader-ios.git' + '@' + '93acc021e3034898716028822cb802a3a816be7e',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/material_internationalization_ios/src': {
      'url': Var('chromium_git') + '/external/github.com/material-foundation/material-internationalization-ios.git' + '@' + 'd8f32464ac97d5551c8db19de916170ae87cfa90',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/material_roboto_font_loader_ios/src': {
      'url': Var('chromium_git') + '/external/github.com/material-foundation/material-roboto-font-loader-ios.git' + '@' + '4aa51e906e5671c71d24e991f1f10d782a58409f',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/material_sprited_animation_view_ios/src': {
      'url': Var('chromium_git') + '/external/github.com/material-foundation/material-sprited-animation-view-ios.git' + '@' + '8af9adaa182044cf2920dfb620b863669e1aeb7c',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/material_text_accessibility_ios/src': {
      'url': Var('chromium_git') + '/external/github.com/material-foundation/material-text-accessibility-ios.git' + '@' + '499b45d1895b565096302a209c0c728df03b95d4',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/motion_interchange_objc/src': {
      'url': Var('chromium_git') + '/external/github.com/material-motion/motion-interchange-objc.git' + '@' + '8e25f1ca599543dc77e4016b588763dcba6cbac8',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/motion_animator_objc/src': {
      'url': Var('chromium_git') + '/external/github.com/material-motion/motion-animator-objc.git' + '@' + '0581bc3dfacd34e40381e9cf45a15070b3868ba7',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/motion_transitioning_objc/src': {
      'url': Var('chromium_git') + '/external/github.com/material-motion/motion-transitioning-objc.git' + '@' + '5bb0d577dd78472536480496ace115bc593ed0e1',
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
    Var('chromium_git') + '/chromium/cdm.git' + '@' + 'fc5afac6847dc61addc1177103aa602e71a9ecac',

  'src/native_client': {
      'url': Var('chromium_git') + '/native_client/src/native_client.git' + '@' + Var('nacl_revision'),
      'condition': 'checkout_nacl',
  },

  'src/net/third_party/quiche/src':
    Var('quiche_git') + '/quiche.git' + '@' +  Var('quiche_revision'),

  'src/tools/luci-go': {
      'packages': [
        {
          'package': 'infra/tools/luci/isolate/${{platform}}',
          'version': Var('luci_go'),
        },
        {
          'package': 'infra/tools/luci/isolated/${{platform}}',
          'version': Var('luci_go'),
        },
        {
          'package': 'infra/tools/luci/swarming/${{platform}}',
          'version': Var('luci_go'),
        },
      ],
      'dep_type': 'cipd',
  },

  'src/third_party/shaderc/src':
    Var('chromium_git') + '/external/github.com/google/shaderc.git@' +
        Var('shaderc_revision'),

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
      'url': Var('chromium_git') + '/android_ndk.git' + '@' + '27c0a8d090c666a50e40fceb4ee5b40b1a2d3f87',
      'condition': 'checkout_android_native_support',
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
              'package': 'chromium/third_party/android_system_sdk',
              'version': 'no8ss5nRg6uYDM08HboypuIQuix7bS1kVqRGyWmwP-YC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_build_tools/aapt2': {
      'packages': [
          {
              'package': 'chromium/third_party/android_build_tools/aapt2',
              'version': 'R2k5wwOlIaS6sjv2TIyHotiPJod-6KqnZO8NH-KFK8sC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_build_tools/art': {
      'packages': [
          {
              'package': 'chromium/third_party/android_build_tools/art',
              'version': '87169fbc701d244c311e6aa8843591a7f1710bc0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_build_tools/bundletool': {
      'packages': [
          {
               'package': 'chromium/third_party/android_tools_bundletool',
               'version': 'FIj7ed-law2zMv41QhjEXabkaJ7aN2ztmE0Tv3Z_gFUC',
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
              'package': 'chromium/third_party/android_sdk/public/build-tools/30.0.1',
              'version': Var('android_sdk_build-tools_version'),
          },
          {
              'package': 'chromium/third_party/android_sdk/public/emulator',
              'version': Var('android_sdk_emulator_version'),
          },
          {
              'package': 'chromium/third_party/android_sdk/public/extras',
              'version': Var('android_sdk_extras_version'),
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
              'package': 'chromium/third_party/android_sdk/public/platforms/android-30',
              'version': Var('android_sdk_platforms_version'),
          },
          {
              'package': 'chromium/third_party/android_sdk/public/sources/android-29',
              'version': Var('android_sdk_sources_version'),
          },
          {
              'package': 'chromium/third_party/android_sdk/public/cmdline-tools',
              'version': 'V__2Ycej-H2-6AcXX5A3gi7sIk74SuN44PBm2uC_N1sC',
          },
      ],
      'condition': 'checkout_android_native_support',
      'dep_type': 'cipd',
  },

  'src/third_party/angle':
    Var('chromium_git') + '/angle/angle.git' + '@' +  Var('angle_revision'),

  'src/third_party/dav1d/libdav1d':
    Var('chromium_git') + '/external/github.com/videolan/dav1d.git' + '@' + '441e643915bea71a0234d85121109d2816f18984',

  'src/third_party/dawn':
    Var('dawn_git') + '/dawn.git' + '@' +  Var('dawn_revision'),

  'src/third_party/tint/src':
    Var('dawn_git') + '/tint.git' + '@' +  Var('tint_revision'),

  'src/third_party/glfw/src':
    Var('chromium_git') + '/external/github.com/glfw/glfw.git@' +  '2de2589f910b1a85905f425be4d32f33cec092df',

  'src/third_party/apache-portable-runtime/src': {
      'url': Var('chromium_git') + '/external/apache-portable-runtime.git' + '@' + 'c3f11fcd86b42922834cae91103cf068246c6bb6',
      'condition': 'checkout_android',
  },

  'src/third_party/barhopper': {
      'url': 'https://chrome-internal.googlesource.com/chrome/deps/barhopper.git' + '@' + 'ad3c4382875afdd0340f1549f8b9c93cbbc16e37',
      'condition': 'checkout_src_internal and checkout_chromeos',
  },

  'src/third_party/bazel': {
      'packages': [
          {
              'package': 'chromium/third_party/bazel',
              'version': 'VjMsf48QUWw8n7XtJP2AuSjIGmbQeYdWdwyxVvIRLmAC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/google_benchmark/src': {
    'url': Var('chromium_git') + '/external/github.com/google/benchmark.git' + '@' + 'ffe1342eb2faa7d2e7c35b4db2ccf99fab81ec20',
    'condition': 'checkout_google_benchmark',
  },

  'src/third_party/boringssl/src':
    Var('boringssl_git') + '/boringssl.git' + '@' +  Var('boringssl_revision'),

  'src/third_party/bouncycastle': {
      'packages': [
          {
              'package': 'chromium/third_party/bouncycastle',
              'version': 'c078e87552ba26e776566fdaf0f22cd8712743d0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/breakpad/breakpad':
    Var('chromium_git') + '/breakpad/breakpad.git' + '@' + 'e3d485f73f5836fdd6fb287ab96973c4f63175e1',

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

  # Tools used when building Chrome for Chrome OS. This affects both the Simple
  # Chrome workflow, as well as the chromeos-chrome ebuild.
  'src/third_party/chromite': {
      'url': Var('chromium_git') + '/chromiumos/chromite.git' + '@' + '628b7f61f813da6b7e44a31e34a6c51f041bcf68',
      'condition': 'checkout_chromeos',
  },

  'src/third_party/cld_3/src':
    Var('chromium_git') + '/external/github.com/google/cld_3.git' + '@' + '06f695f1c8ee530104416aab5dcf2d6a1414a56a',

  'src/third_party/colorama/src':
    Var('chromium_git') + '/external/colorama.git' + '@' + '799604a1041e9b3bc5d2789ecbd7e8db2e18e6b8',

  'src/third_party/crc32c/src':
    Var('chromium_git') + '/external/github.com/google/crc32c.git' + '@' + '5998f8451548244de8cde7fab387a550e7c4497d',

  # For Linux and Chromium OS.
  'src/third_party/cros_system_api': {
      'url': Var('chromium_git') + '/chromiumos/platform2/system_api.git' + '@' + 'adce67ac57ed882a52d04d4838677834b337ee56',
      'condition': 'checkout_linux',
  },

  'src/third_party/depot_tools':
    Var('chromium_git') + '/chromium/tools/depot_tools.git' + '@' + '428143ee24c5f084c8dfb38cd17f07b4f7ba9bf7',

  'src/third_party/devtools-frontend/src':
    Var('chromium_git') + '/devtools/devtools-frontend' + '@' + Var('devtools_frontend_revision'),

  'src/third_party/dom_distiller_js/dist':
    Var('chromium_git') + '/chromium/dom-distiller/dist.git' + '@' + 'f339eb9463714c3d31657c8ee1bd53d1c7e5c555',

  'src/third_party/eigen3/src':
    Var('chromium_git') + '/external/gitlab.com/libeigen/eigen.git' + '@' + '2ce2f5198929caab4b41a6ad1b9c93f67d8b9a69',

  'src/third_party/emoji-metadata/src': {
    'url': Var('chromium_git') + '/external/github.com/googlefonts/emoji-metadata' + '@' + '9d8e5cfe52ee763b7dd91e33e5a50250efbf7135',
    'condition': 'checkout_chromeos',
  },

  'src/third_party/espresso': {
      'packages': [
          {
              'package': 'chromium/third_party/espresso',
              'version': 'y8fIfH8Leo2cPm7iGCYnBxZpwOlgLv8rm2mlcmJlvGsC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/farmhash/src':
    Var('chromium_git') + '/external/github.com/google/farmhash.git' + '@' + '816a4ae622e964763ca0862d9dbd19324a1eaf45',

  'src/third_party/ffmpeg':
    Var('chromium_git') + '/chromium/third_party/ffmpeg.git' + '@' + '841aa72c9e153ae5f952e31e4b6406870555922d',

  'src/third_party/flac':
    Var('chromium_git') + '/chromium/deps/flac.git' + '@' + 'af862024c8c8fa0ae07ced05e89013d881b00596',

  'src/third_party/flatbuffers/src':
    Var('chromium_git') + '/external/github.com/google/flatbuffers.git' + '@' + '6df40a2471737b27271bdd9b900ab5f3aec746c7',

  # Used for embedded builds. CrOS & Linux use the system version.
  'src/third_party/fontconfig/src': {
      'url': Var('chromium_git') + '/external/fontconfig.git' + '@' + '452be8125f0e2a18a7dfef469e05d19374d36307',
      'condition': 'checkout_linux',
  },

  'src/third_party/fp16/src':
    Var('chromium_git') + '/external/github.com/Maratyszcza/FP16.git' + '@' + 'febbb1c163726b5db24bed55cc9dc42529068997',

  'src/third_party/gemmlowp/src':
    Var('chromium_git') + '/external/github.com/google/gemmlowp.git' + '@' + 'fda83bdc38b118cc6b56753bd540caa49e570745',

  'src/third_party/grpc/src': {
      'url': Var('chromium_git') + '/external/github.com/grpc/grpc.git' + '@' + '4ac9c6f755463a2321f84b0cb2d631e1828faedb',
  },

  'src/third_party/freetype/src':
    Var('chromium_git') + '/chromium/src/third_party/freetype2.git' + '@' + Var('freetype_revision'),

  'src/third_party/freetype-testing/src':
    Var('chromium_git') + '/external/github.com/freetype/freetype2-testing.git' + '@' + Var('freetype_testing_revision'),

  'src/third_party/harfbuzz-ng/src':
    Var('chromium_git') + '/external/github.com/harfbuzz/harfbuzz.git' + '@' + Var('harfbuzz_revision'),

  'src/third_party/emoji-segmenter/src':
    Var('chromium_git') + '/external/github.com/google/emoji-segmenter.git' + '@' + Var('emoji_segmenter_revision'),

  'src/third_party/libgav1/src':
    Var('chromium_git') + '/codecs/libgav1.git' + '@' + 'a5ee0e00923c355ef3aad2b2829365a9fde84430',

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

  'src/third_party/guava': {
      'packages': [
          {
              'package': 'chromium/third_party/guava',
              'version': 'y8Zx7cKTiOunLhOrfC4hOt5kDQrLJ_Rq7ISDmXkPdYsC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/gvr-android-sdk/src': {
      'url': Var('chromium_git') + '/external/github.com/googlevr/gvr-android-sdk.git' + '@' + '233e7fe922a543e0bc55382d64cacd047307d0e7',
      'condition': 'checkout_android',
  },

  'src/third_party/arcore-android-sdk/src': {
      'url': Var('chromium_git') + '/external/github.com/google-ar/arcore-android-sdk.git' + '@' + '54861e38cceb406021fc8ece495f82387376af62',
      'condition': 'checkout_android',
  },

  'src/third_party/arcore-android-sdk-client': {
      'packages': [
        {
          'package': 'chromium/third_party/arcore-android-sdk-client',
          'version': 'KFu2mHSzZr0ZNOlM_LG5AA5ZIi2_rICieWgkME9IUFYC',
        },
      ],

      'condition': 'checkout_android',
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
    Var('chromium_git') + '/chromium/deps/hunspell_dictionaries.git' + '@' + '18e09b9197a3b1d771c077c530d1a4ebad04c167',

  'src/third_party/icu':
    Var('chromium_git') + '/chromium/deps/icu.git' + '@' + '39e57b58107d5dbe083ea73a1d5433f08f00fb1d',

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
      'url': Var('chromium_git') + '/external/github.com/c2nes/javalang.git' + '@' + 'f98ffcb31d1daa57fbe5bd6def8ad7c3126d8242',
      'condition': 'checkout_android',
  },

  'src/third_party/jdk': {
      'packages': [
          {
              'package': 'chromium/third_party/jdk',
              'version': 'PfRSnxe8Od6WU4zBXomq-zsgcJgWmm3z4gMQNB-r2QcC',
          },
          {
              'package': 'chromium/third_party/jdk/extras',
              'version': 'fkhuOQ3r-zKtWEdKplpo6k0vKkjl-LY_rJTmtzFCQN4C',
          },
      ],
      # Needed on Linux for use on chromium_presubmit (for checkstyle).
      'condition': 'checkout_android or checkout_linux',
      'dep_type': 'cipd',
  },

  'src/third_party/jsoncpp/source':
    Var('chromium_git') + '/external/github.com/open-source-parsers/jsoncpp.git'
      + '@' + '9059f5cad030ba11d37818847443a53918c327b1', # release 1.9.4

  'src/third_party/junit/src': {
      'url': Var('chromium_git') + '/external/junit.git' + '@' + '64155f8a9babcfcf4263cf4d08253a1556e75481',
      'condition': 'checkout_android',
  },

  'src/third_party/leveldatabase/src':
    Var('chromium_git') + '/external/leveldb.git' + '@' + 'c46e79c760125f9adb87a575d94034032637f8d0',

  'src/third_party/libFuzzer/src':
    Var('chromium_git') + '/chromium/llvm-project/compiler-rt/lib/fuzzer.git' + '@' +  Var('libfuzzer_revision'),

  'src/third_party/libaddressinput/src':
    Var('chromium_git') + '/external/libaddressinput.git' + '@' + '390dfc08e3806a2125d08d4d8d034a24d587d77a',

  'src/third_party/libaom/source/libaom':
    Var('aomedia_git') + '/aom.git' + '@' +  '43927e4611e7c3062a67ebaca38a625faa9a39d6',

  'src/third_party/libavif/src':
    Var('chromium_git') + '/external/github.com/AOMediaCodec/libavif.git' + '@' + Var('libavif_revision'),

  'src/third_party/nearby/src':
    Var('chromium_git') + '/external/github.com/google/nearby-connections.git' + '@' + Var('nearby_revision'),

  'src/third_party/securemessage/src':
    Var('chromium_git') + '/external/github.com/google/securemessage.git' + '@' + Var('securemessage_revision'),

  'src/third_party/ukey2/src':
    Var('chromium_git') + '/external/github.com/google/ukey2.git' + '@' + Var('ukey2_revision'),

  # Userspace interface to kernel DRM services.
  'src/third_party/libdrm/src': {
      'url': Var('chromium_git') + '/chromiumos/third_party/libdrm.git' + '@' + '0190f49a139e7069d7cad6a6890832831da1aa8b',
      'condition': 'checkout_linux',
  },

  'src/third_party/expat/src':
    Var('chromium_git') + '/external/github.com/libexpat/libexpat.git' + '@' + Var('libexpat_revision'),

  # The library for IPP protocol (Chrome OS).
  'src/third_party/libipp/libipp': {
      'url': Var('chromium_git') + '/chromiumos/platform2/libipp.git' + '@' + '6c45a4f3a05cb5dd700414fe4d94cf685159d3ce',
      'condition': 'checkout_linux',
  },

  'src/third_party/libjpeg_turbo':
    Var('chromium_git') + '/chromium/deps/libjpeg_turbo.git' + '@' + '518d81558c797486e125e37cb529d65b560a6ea0',

  'src/third_party/liblouis/src': {
      'url': Var('chromium_git') + '/external/liblouis-github.git' + '@' + '03d7d72d983d47c17f7de95bbeae89fe33e41943',
      'condition': 'checkout_linux',
  },

  'src/third_party/libphonenumber/dist':
    Var('chromium_git') + '/external/libphonenumber.git' + '@' + '68eba9d6ee8b11fb58ece36b6c46d07965d7f7ff',

  'src/third_party/libprotobuf-mutator/src':
    Var('chromium_git') + '/external/github.com/google/libprotobuf-mutator.git' + '@' +  Var('libprotobuf-mutator'),

  'src/third_party/libsrtp':
    Var('chromium_git') + '/chromium/deps/libsrtp.git' + '@' + '7990ca64c616b150a9cb4714601c4a3b0c84fe91',

  # Android Explicit Synchronization.
  'src/third_party/libsync/src': {
      'url': Var('chromium_git') + '/aosp/platform/system/core/libsync.git' + '@' + 'f4f4387b6bf2387efbcfd1453af4892e8982faf6',
      'condition': 'checkout_linux',
  },

  'src/third_party/libunwindstack': {
      'url': Var('chromium_git') + '/chromium/src/third_party/libunwindstack.git' + '@' + '11659d420a71e7323b379ea8781f07c6f384bc7e',
      'condition': 'checkout_android',
  },

  'src/third_party/libvpx/source/libvpx':
    Var('chromium_git') + '/webm/libvpx.git' + '@' +  'b5d77a48d740e211a130c8e45d9353ef8c154a47',

  'src/third_party/libwebm/source':
    Var('chromium_git') + '/webm/libwebm.git' + '@' + '51ca718c3adf0ddedacd7df25fe45f67dc5a9ce1',

  'src/third_party/libyuv':
    Var('chromium_git') + '/libyuv/libyuv.git' + '@' + '93b1b332cd60b56ab90aea14182755e379c28a80',  # from r1770

  'src/third_party/lighttpd': {
      'url': Var('chromium_git') + '/chromium/deps/lighttpd.git' + '@' + Var('lighttpd_revision'),
      'condition': 'checkout_mac or checkout_win',
  },

  'src/third_party/lss': {
      'url': Var('chromium_git') + '/linux-syscall-support.git' + '@' + Var('lss_revision'),
      'condition': 'checkout_android or checkout_linux',
  },

  'src/third_party/material_design_icons/src': {
      'url': Var('chromium_git') + '/external/github.com/google/material-design-icons.git' + '@' +
          '5ab428852e35dc177a8c37a2df9dc9ccf768c65a',
      'condition': 'checkout_ios',
  },

  # GNU binutils assembler for x86-64.
  'src/third_party/mingw-w64/mingw/bin': {
      'url': Var('chromium_git') + '/native_client/deps/third_party/mingw-w64/mingw/bin.git' + '@' + '3cc8b140b883a9fe4986d12cfd46c16a093d3527',
      'condition': 'checkout_nacl and checkout_win',
  },

  # Graphics buffer allocator for Chrome OS.
  'src/third_party/minigbm/src': {
      'url': Var('chromium_git') + '/chromiumos/platform/minigbm.git' + '@' + '150b4968810447d72714a7c88afb97d4e59fe23a',
      'condition': 'checkout_linux',
  },

  # Minizip library. Used on Chrome OS.
  'src/third_party/minizip/src': {
      'url': Var('chromium_git') + '/external/github.com/nmoinvaz/minizip' + '@' + '1ff40343b55e738d941abb51c70eddb803db16e2',
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
      '19f3fad68da99277b2882939d3b2fa4c4b8d51d9'
  },

  'src/third_party/neon_2_sse/src':
    Var('chromium_git') + '/external/github.com/intel/ARM_NEON_2_x86_SSE.git' + '@' + '42b2bebacee25452e150095ef4480b3fa26e30f5',

  'src/third_party/netty-tcnative/src': {
      'url': Var('chromium_git') + '/external/netty-tcnative.git' + '@' + '5b46a8ef4a39c39c576fcdaaf718b585d75df463',
      'condition': 'checkout_android',
  },

  'src/third_party/netty4/src': {
      'url': Var('chromium_git') + '/external/netty4.git' + '@' + 'cc4420b13bb4eeea5b1cf4f93b2755644cd3b120',
      'condition': 'checkout_android',
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
    Var('chromium_git') + '/external/github.com/cisco/openh264' + '@' + '3dd5b80bc4f172dd82925bb259cb7c82348409c5',

  'src/third_party/openscreen/src':
    Var('chromium_git') + '/openscreen' + '@' + '6051838253185d8478f5fa4d70c96ef6c0241f94',

  'src/third_party/openxr/src': {
    'url': Var('chromium_git') + '/external/github.com/KhronosGroup/OpenXR-SDK' + '@' + '97cfe495bb7a3853266b646d1c79e169387f9c7a',
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
    Var('android_git') + '/platform/external/perfetto.git' + '@' + '2d79f95e908dcbae188e1fedd12e8424cbd14392',

  'src/third_party/perl': {
      'url': Var('chromium_git') + '/chromium/deps/perl.git' + '@' + '6f3e5028eb65d0b4c5fdd792106ac4c84eee1eb3',
      'condition': 'checkout_win',
  },

  'src/third_party/proguard': {
      'packages': [
          {
              'package': 'chromium/third_party/proguard',
              'version': 'Fd91BJFVlmiO6c46YMTsdy7n2f5Sk2hVVGlzPLvqZPsC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/protoc_javalite': {
      'packages': [
          {
              'package': 'chromium/third_party/protoc_javalite',
              'version': 'version:3.0.0-cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  # Dependency of skia.
  'src/third_party/pyelftools': {
      'url': Var('chromium_git') + '/chromiumos/third_party/pyelftools.git' + '@' + '19b3e610c86fcadb837d252c794cb5e8008826ae',
      'condition': 'checkout_linux',
  },

  'src/third_party/quic_trace/src':
    Var('chromium_git') + '/external/github.com/google/quic-trace.git' + '@' + '2dccbe8e996d43c6d93b69e4f6d024f015e8c488',

  'src/third_party/pywebsocket3/src':
    Var('chromium_git') + '/external/github.com/GoogleChromeLabs/pywebsocket3.git' + '@' + 'faf478a0453ceae78f2a3bc11c6c21e13362628f',

  'src/third_party/qemu-linux-arm64': {
      'packages': [
          {
              'package': 'fuchsia/qemu/linux-arm64',
              'version': 'b1b61a39e3ab0935cd030f27e01740578b04b967'
          },
      ],
      'condition': 'host_os == "linux" and checkout_fuchsia and checkout_fuchsia_for_arm64_host',
      'dep_type': 'cipd',
  },

  'src/third_party/qemu-linux-x64': {
      'packages': [
          {
              'package': 'fuchsia/qemu/linux-amd64',
              'version': '9cc486c5b18a0be515c39a280ca9a309c54cf994'
          },
      ],
      'condition': 'host_os == "linux" and checkout_fuchsia',
      'dep_type': 'cipd',
  },

  'src/third_party/qemu-mac-x64': {
      'packages': [
          {
              'package': 'fuchsia/qemu/mac-amd64',
              'version': '2d3358ae9a569b2d4a474f498b32b202a152134f'
          },
      ],
      'condition': 'host_os == "mac" and checkout_fuchsia',
      'dep_type': 'cipd',
  },

  'src/third_party/aemu-linux-x64': {
      'packages': [
          {
              'package': 'fuchsia/third_party/aemu/linux-amd64',
              'version': 'xAHa1IXmKteChkPvba9ezjSnKL7IyDePQRzWVUEAx9UC'
          },
      ],
      'condition': 'host_os == "linux" and checkout_fuchsia',
      'dep_type': 'cipd',
  },

  'src/third_party/aemu-mac-x64': {
      'packages': [
          {
              'package': 'fuchsia/third_party/aemu/mac-amd64',
              'version': 'guAtTBQ1SKIjr06srbTOjNWiYxrM4nK1AbfeQo1gBmQC'
          },
      ],
      'condition': 'host_os == "mac" and checkout_fuchsia',
      'dep_type': 'cipd',
  },

  'src/third_party/zstd-linux-arm64': {
      'packages': [
          {
              'package': 'fuchsia/third_party/zstd/linux-arm64',
              'version': '3_wH3L3VWelJ8aIDSabZ0P9oMqHrKnuVUyXVBvjaE9UC'
          },
      ],
      'condition': 'host_os == "linux" and checkout_fuchsia and checkout_fuchsia_for_arm64_host',
      'dep_type': 'cipd',
  },

  'src/third_party/zstd-linux-x64': {
      'packages': [
          {
              'package': 'fuchsia/third_party/zstd/linux-amd64',
              'version': 'nVy-mclpAVYv0eyAe8SDxiE1g2UuafhnjOx2wr6vd_IC'
          },
      ],
      'condition': 'host_os == "linux" and checkout_fuchsia',
      'dep_type': 'cipd',
  },

  'src/third_party/re2/src':
    Var('chromium_git') + '/external/github.com/google/re2.git' + '@' + 'c33d1680c7e9ab7edea02d7465a8db13e80b558d',

  'src/third_party/r8': {
      'packages': [
          {
              'package': 'chromium/third_party/r8',
              'version': 'vI1-bFGf5I1oPyW89wWgoQpREvQy7GGtrjrqUhXTnBIC',
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
              'version': 'iC6RDM5EH3GEAzR-1shW_Mg0FeeNE5shq1okkFfuuNQC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/ruy/src':
    Var('chromium_git') + '/external/github.com/google/ruy.git' + '@' + '34ea9f4993955fa1ff4eb58e504421806b7f2e8f',

  'src/third_party/skia':
    Var('skia_git') + '/skia.git' + '@' +  Var('skia_revision'),

  'src/third_party/smhasher/src':
    Var('chromium_git') + '/external/smhasher.git' + '@' + 'e87738e57558e0ec472b2fc3a643b838e5b6e88f',

  'src/third_party/snappy/src':
    Var('chromium_git') + '/external/github.com/google/snappy.git' + '@' + 'ea368c2f07de5f31146a10214f27d15091b09771',

  'src/third_party/sqlite/src':
    Var('chromium_git') + '/chromium/deps/sqlite.git' + '@' + '702a79c7d77331cbe2838c4b3c5fab529f75f7c2',

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

  'src/third_party/swiftshader':
    Var('swiftshader_git') + '/SwiftShader.git' + '@' +  Var('swiftshader_revision'),

  'src/third_party/text-fragments-polyfill/src': {
    'url': Var('chromium_git') + '/external/github.com/GoogleChromeLabs/text-fragments-polyfill.git' + '@' + '20b44319352e4245ace896c96dda24e001297dfd',
    'condition': 'checkout_ios',
  },

  'src/third_party/tflite/src':
    Var('chromium_git') + '/external/github.com/tensorflow/tensorflow.git' + '@' + 'fcc4b966f1265f466e82617020af93670141b009',

  'src/third_party/turbine': {
      'packages': [
          {
              'package': 'chromium/third_party/turbine',
              'version': '_iPtB_ThhxlMOt2TsYqVppwriEEn0mp-NUNRwDwYLUAC',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/turbine/src': {
      'url': Var('chromium_git') + '/external/github.com/google/turbine.git' + '@' + '3c31e67ae25b5e43713fd868e3a9b535ff6298af',
      'condition': 'checkout_android',
  },

  'src/third_party/ub-uiautomator/lib': {
      'url': Var('chromium_git') + '/chromium/third_party/ub-uiautomator.git' + '@' + '00270549ce3161ae72ceb24712618ea28b4f9434',
      'condition': 'checkout_android',
  },

  'src/third_party/usrsctp/usrsctplib':
    Var('chromium_git') + '/external/github.com/sctplab/usrsctp' + '@' + '4191ca1784d8774dbf62d48ab9426c7311a91bc5',

  'src/third_party/vulkan-deps': '{chromium_git}/vulkan-deps@4c6cbd7a1be7d099b4c7f0060f7a133b8f6a2aa4',

  'src/third_party/vulkan_memory_allocator':
    Var('chromium_git') + '/external/github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git' + '@' + '6c656df63da5995a932aafd45b32af1974e497d9',

  # Display server protocol for Linux.
  'src/third_party/wayland/src': {
      'url': Var('chromium_git') + '/external/anongit.freedesktop.org/git/wayland/wayland.git' + '@' + 'eb1339edd398b9f5328816931e585db4229aa132',
      'condition': 'checkout_linux',
  },

  # Wayland protocols that add functionality not available in the core protocol.
  'src/third_party/wayland-protocols/src': {
      'url': Var('chromium_git') + '/external/anongit.freedesktop.org/git/wayland/wayland-protocols.git' + '@' + '684cd23ae31fafb9eda2a0cd15edb4e96de92259',
      'condition': 'checkout_linux',
  },

  # Wireless Display Software. Used on Chrome OS.
  'src/third_party/wds/src': {
      'url': Var('chromium_git') + '/external/github.com/01org/wds' + '@' + 'a488b169d724d6b4cfcd7037dbab1f1c2bf04f3b',
      'condition': 'checkout_linux',
  },

  'src/third_party/webdriver/pylib':
    Var('chromium_git') + '/external/github.com/SeleniumHQ/selenium/py.git' + '@' + 'd0045ec570c1a77612db35d1e92f05e1d27b4d53',

  'src/third_party/webgl/src':
    Var('chromium_git') + '/external/khronosgroup/webgl.git' + '@' + '3e3f152617996e4a175fd0b597a1936824b371f3',

  'src/third_party/webgpu-cts/src':
    Var('chromium_git') + '/external/github.com/gpuweb/cts.git' + '@' + '3c2fe3888658d82b47ca831d59a2e07579619c2d',

  'src/third_party/webrtc':
    Var('webrtc_git') + '/src.git' + '@' + '1f8862eff854305fe6ee27a375d297f1a3b5b1ce',

  'src/third_party/libgifcodec':
     Var('skia_git') + '/libgifcodec' + '@'+  Var('libgifcodec_revision'),

  # Wuffs' canonical repository is at github.com/google/wuffs, but we use
  # Skia's mirror of Wuffs, the same as in upstream Skia's DEPS file.
  'src/third_party/wuffs/src':
    Var('skia_git') + '/external/github.com/google/wuffs.git' + '@' +  Var('wuffs_revision'),

  'src/third_party/weston/src': {
      'url': Var('chromium_git') + '/external/anongit.freedesktop.org/git/wayland/weston.git' + '@' + 'db905f467169f9f131c5ddc6038296f866d5daeb',
      'condition': 'checkout_linux',
  },

  'src/third_party/xdg-utils': {
      'url': Var('chromium_git') + '/chromium/deps/xdg-utils.git' + '@' + 'd80274d5869b17b8c9067a1022e4416ee7ed5e0d',
      'condition': 'checkout_linux',
  },

  'src/third_party/xstream': {
      'packages': [
          {
              'package': 'chromium/third_party/xstream',
              'version': '4278b1b78b86ab7a1a29e64d5aec9a47a9aab0fe',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/tools/page_cycler/acid3':
    Var('chromium_git') + '/chromium/deps/acid3.git' + '@' + '6be0a66a1ebd7ebc5abc1b2f405a945f6d871521',

  'src/tools/skia_goldctl/linux': {
      'packages': [
        {
          'package': 'skia/tools/goldctl/linux-amd64',
          'version': 'UBQI89SqFeAIiGdjUUikXKgyMRFxDbvtqVUceCn1kcwC',
        },
      ],
      'dep_type': 'cipd',
      'condition': 'checkout_linux',
  },
  'src/tools/skia_goldctl/win': {
      'packages': [
        {
          'package': 'skia/tools/goldctl/windows-amd64',
          'version': 'kIBp6ZuMA5QTpwxtMM3NtSMezpaetuByhNmVTMg8TtMC',
        },
      ],
      'dep_type': 'cipd',
      'condition': 'checkout_win',
  },
  'src/tools/skia_goldctl/mac': {
      'packages': [
        {
          'package': 'skia/tools/goldctl/mac-amd64',
          'version': 'Cb3ZdanPJ4Sjcf0QVUa5PMI5nkXnfRX3fQltIjHI7ZoC',
        },
      ],
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
  },

  'src/tools/swarming_client':
    Var('chromium_git') + '/infra/luci/client-py.git' + '@' +  Var('swarming_revision'),

  'src/v8':
    Var('nwjs_git') + '/v8.git' + '@' +  Var('nw_v8_revision'),

  'src/third_party/node-nw':
    Var('nwjs_git') + '/node.git' + '@' +  Var('nw_node_revision'),

  #'src/v8':
  #  Var('chromium_git') + '/v8/v8.git' + '@' +  Var('v8_revision'),

  'src-internal': {
    'url': 'https://chrome-internal.googlesource.com/chrome/src-internal.git@7fe61c5b7676d898a134cdb6dc13602456009f94',
    'condition': 'checkout_src_internal',
  },

  'src/chromeos/components/help_app_ui/resources/prod': {
    'packages': [
      {
        'package': 'chromeos_internal/apps/help_app/app',
        'version': 'Z8SPFOK8xXNLHkZQpPeKA5AZN3O-ectwfCwceIdJ0oYC',
      },
    ],
    'condition': 'checkout_chromeos and checkout_src_internal',
    'dep_type': 'cipd',
  },

  'src/chromeos/components/media_app_ui/resources/prod': {
    'packages': [
      {
        'package': 'chromeos_internal/apps/media_app/app',
        'version': '6-PmZaslIqMHBCSf-A2dk6fFJPSzh90s5WKnGNkNPJ4C',
      },
    ],
    'condition': 'checkout_chromeos and checkout_src_internal',
    'dep_type': 'cipd',
  },

  'src/third_party/google_android_play_core': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_play_core_verification',
              'version': 'ojrkXUE6tjG8FYmoLfCD3YdOxTyl2BXMdmk7Fb6cS5MC',
          },
      ],
      'condition': 'checkout_android',
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
      'url': Var('android_git') + '/platform/prebuilts/build-tools.git' + '@' + '5794e96eb8bae47bb48feee915d99583573b3887',
      'condition': 'checkout_android_prebuilts_build_tools',
  },

  # === ANDROID_DEPS Generated Code Start ===
  # Generated by //third_party/android_deps/fetch_all.py
  'src/third_party/android_deps/libs/android_arch_core_common': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/android_arch_core_common',
              'version': 'version:1.1.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/android_arch_core_runtime': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/android_arch_core_runtime',
              'version': 'version:1.1.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/android_arch_lifecycle_common': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/android_arch_lifecycle_common',
              'version': 'version:1.1.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/android_arch_lifecycle_common_java8': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/android_arch_lifecycle_common_java8',
              'version': 'version:1.1.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/android_arch_lifecycle_livedata': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/android_arch_lifecycle_livedata',
              'version': 'version:1.1.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/android_arch_lifecycle_livedata_core': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/android_arch_lifecycle_livedata_core',
              'version': 'version:1.1.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/android_arch_lifecycle_runtime': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/android_arch_lifecycle_runtime',
              'version': 'version:1.1.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/android_arch_lifecycle_viewmodel': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/android_arch_lifecycle_viewmodel',
              'version': 'version:1.1.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_activity_activity': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_activity_activity',
              'version': 'version:1.1.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_annotation_annotation': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_annotation_annotation',
              'version': 'version:1.2.0-alpha01-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_annotation_annotation_experimental': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_annotation_annotation_experimental',
              'version': 'version:1.1.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_appcompat_appcompat': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_appcompat_appcompat',
              'version': 'version:1.2.0-beta01-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_appcompat_appcompat_resources': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_appcompat_appcompat_resources',
              'version': 'version:1.2.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_arch_core_core_common': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_arch_core_core_common',
              'version': 'version:2.2.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_arch_core_core_runtime': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_arch_core_core_runtime',
              'version': 'version:2.1.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_asynclayoutinflater_asynclayoutinflater': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_asynclayoutinflater_asynclayoutinflater',
              'version': 'version:1.1.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_cardview_cardview': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_cardview_cardview',
              'version': 'version:1.1.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_collection_collection': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_collection_collection',
              'version': 'version:1.2.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_concurrent_concurrent_futures': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_concurrent_concurrent_futures',
              'version': 'version:1.2.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_coordinatorlayout_coordinatorlayout': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_coordinatorlayout_coordinatorlayout',
              'version': 'version:1.2.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_core_core': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_core_core',
              'version': 'version:1.5.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_core_core_animation': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_core_core_animation',
              'version': 'version:1.0.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_cursoradapter_cursoradapter': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_cursoradapter_cursoradapter',
              'version': 'version:1.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_customview_customview': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_customview_customview',
              'version': 'version:1.2.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_documentfile_documentfile': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_documentfile_documentfile',
              'version': 'version:1.1.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_drawerlayout_drawerlayout': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_drawerlayout_drawerlayout',
              'version': 'version:1.2.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_exifinterface_exifinterface': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_exifinterface_exifinterface',
              'version': 'version:1.4.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_fragment_fragment': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_fragment_fragment',
              'version': 'version:1.2.5-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_gridlayout_gridlayout': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_gridlayout_gridlayout',
              'version': 'version:1.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_interpolator_interpolator': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_interpolator_interpolator',
              'version': 'version:1.1.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_leanback_leanback': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_leanback_leanback',
              'version': 'version:1.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_leanback_leanback_preference': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_leanback_leanback_preference',
              'version': 'version:1.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_legacy_legacy_preference_v14': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_legacy_legacy_preference_v14',
              'version': 'version:1.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_legacy_legacy_support_core_ui': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_legacy_legacy_support_core_ui',
              'version': 'version:1.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_legacy_legacy_support_core_utils': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_legacy_legacy_support_core_utils',
              'version': 'version:1.1.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_legacy_legacy_support_v4': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_legacy_legacy_support_v4',
              'version': 'version:1.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_lifecycle_lifecycle_common': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_lifecycle_lifecycle_common',
              'version': 'version:2.2.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_lifecycle_lifecycle_common_java8': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_lifecycle_lifecycle_common_java8',
              'version': 'version:2.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_lifecycle_lifecycle_livedata': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_lifecycle_lifecycle_livedata',
              'version': 'version:2.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_lifecycle_lifecycle_livedata_core': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_lifecycle_lifecycle_livedata_core',
              'version': 'version:2.2.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_lifecycle_lifecycle_runtime': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_lifecycle_lifecycle_runtime',
              'version': 'version:2.2.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_lifecycle_lifecycle_viewmodel': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_lifecycle_lifecycle_viewmodel',
              'version': 'version:2.2.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_lifecycle_lifecycle_viewmodel_savedstate': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_lifecycle_lifecycle_viewmodel_savedstate',
              'version': 'version:2.2.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_loader_loader': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_loader_loader',
              'version': 'version:1.2.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_localbroadcastmanager_localbroadcastmanager': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_localbroadcastmanager_localbroadcastmanager',
              'version': 'version:1.1.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_media_media': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_media_media',
              'version': 'version:1.3.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_mediarouter_mediarouter': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_mediarouter_mediarouter',
              'version': 'version:1.3.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_multidex_multidex': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_multidex_multidex',
              'version': 'version:2.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_palette_palette': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_palette_palette',
              'version': 'version:1.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_preference_preference': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_preference_preference',
              'version': 'version:1.1.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_print_print': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_print_print',
              'version': 'version:1.1.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_recyclerview_recyclerview': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_recyclerview_recyclerview',
              'version': 'version:1.2.0-alpha06-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_savedstate_savedstate': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_savedstate_savedstate',
              'version': 'version:1.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_slice_slice_builders': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_slice_slice_builders',
              'version': 'version:1.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_slice_slice_core': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_slice_slice_core',
              'version': 'version:1.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_slidingpanelayout_slidingpanelayout': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_slidingpanelayout_slidingpanelayout',
              'version': 'version:1.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_swiperefreshlayout_swiperefreshlayout': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_swiperefreshlayout_swiperefreshlayout',
              'version': 'version:1.2.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_test_core': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_test_core',
              'version': 'version:1.2.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_test_espresso_espresso_contrib': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_test_espresso_espresso_contrib',
              'version': 'version:3.2.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_test_espresso_espresso_core': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_test_espresso_espresso_core',
              'version': 'version:3.2.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_test_espresso_espresso_idling_resource': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_test_espresso_espresso_idling_resource',
              'version': 'version:3.2.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_test_espresso_espresso_intents': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_test_espresso_espresso_intents',
              'version': 'version:3.2.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_test_espresso_espresso_web': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_test_espresso_espresso_web',
              'version': 'version:3.2.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_test_ext_junit': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_test_ext_junit',
              'version': 'version:1.1.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_test_monitor': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_test_monitor',
              'version': 'version:1.2.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_test_rules': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_test_rules',
              'version': 'version:1.2.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_test_runner': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_test_runner',
              'version': 'version:1.2.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_test_uiautomator_uiautomator': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_test_uiautomator_uiautomator',
              'version': 'version:2.2.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_transition_transition': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_transition_transition',
              'version': 'version:1.4.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_tvprovider_tvprovider': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_tvprovider_tvprovider',
              'version': 'version:1.1.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_vectordrawable_vectordrawable': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_vectordrawable_vectordrawable',
              'version': 'version:1.2.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_vectordrawable_vectordrawable_animated': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_vectordrawable_vectordrawable_animated',
              'version': 'version:1.2.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_versionedparcelable_versionedparcelable': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_versionedparcelable_versionedparcelable',
              'version': 'version:1.1.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_viewpager2_viewpager2': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_viewpager2_viewpager2',
              'version': 'version:1.1.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_viewpager_viewpager': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_viewpager_viewpager',
              'version': 'version:1.1.0-SNAPSHOT-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/androidx_webkit_webkit': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/androidx_webkit_webkit',
              'version': 'version:1.3.0-rc01-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/backport_util_concurrent_backport_util_concurrent': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/backport_util_concurrent_backport_util_concurrent',
              'version': 'version:3.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/classworlds_classworlds': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/classworlds_classworlds',
              'version': 'version:1.1-alpha-2-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_animated_vector_drawable': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_animated_vector_drawable',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_appcompat_v7': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_appcompat_v7',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_asynclayoutinflater': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_asynclayoutinflater',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_cardview_v7': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_cardview_v7',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_collections': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_collections',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_coordinatorlayout': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_coordinatorlayout',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_cursoradapter': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_cursoradapter',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_customview': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_customview',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_design': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_design',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_documentfile': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_documentfile',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_drawerlayout': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_drawerlayout',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_interpolator': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_interpolator',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_loader': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_loader',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_localbroadcastmanager': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_localbroadcastmanager',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_multidex': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_multidex',
              'version': 'version:1.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_print': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_print',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_recyclerview_v7': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_recyclerview_v7',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_slidingpanelayout': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_slidingpanelayout',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_support_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_support_annotations',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_support_compat': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_support_compat',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_support_core_ui': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_support_core_ui',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_support_core_utils': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_support_core_utils',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_support_fragment': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_support_fragment',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_support_media_compat': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_support_media_compat',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_support_v4': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_support_v4',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_support_vector_drawable': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_support_vector_drawable',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_swiperefreshlayout': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_swiperefreshlayout',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_transition': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_transition',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_versionedparcelable': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_versionedparcelable',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_support_viewpager': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_viewpager',
              'version': 'version:28.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_tools_build_jetifier_jetifier_core': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_tools_build_jetifier_jetifier_core',
              'version': 'version:1.0.0-beta08-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_tools_build_jetifier_jetifier_processor': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_tools_build_jetifier_jetifier_processor',
              'version': 'version:1.0.0-beta08-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_tools_desugar_jdk_libs': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_tools_desugar_jdk_libs',
              'version': 'version:1.0.10-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_android_tools_desugar_jdk_libs_configuration': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_tools_desugar_jdk_libs_configuration',
              'version': 'version:1.0.10-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_github_ben_manes_caffeine_caffeine': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_github_ben_manes_caffeine_caffeine',
              'version': 'version:2.8.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_github_kevinstern_software_and_algorithms': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_github_kevinstern_software_and_algorithms',
              'version': 'version:1.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_auth': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_auth',
              'version': 'version:17.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_auth_api_phone': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_auth_api_phone',
              'version': 'version:17.5.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_auth_base': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_auth_base',
              'version': 'version:17.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_base': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_base',
              'version': 'version:17.1.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_basement': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_basement',
              'version': 'version:17.1.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_cast': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_cast',
              'version': 'version:17.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_cast_framework': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_cast_framework',
              'version': 'version:17.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_clearcut': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_clearcut',
              'version': 'version:17.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_fido': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_fido',
              'version': 'version:18.1.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_flags': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_flags',
              'version': 'version:17.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_gcm': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_gcm',
              'version': 'version:17.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_iid': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_iid',
              'version': 'version:17.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_instantapps': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_instantapps',
              'version': 'version:17.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_location': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_location',
              'version': 'version:17.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_phenotype': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_phenotype',
              'version': 'version:17.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_places_placereport': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_places_placereport',
              'version': 'version:17.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_stats': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_stats',
              'version': 'version:17.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_tasks': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_tasks',
              'version': 'version:17.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_vision': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_vision',
              'version': 'version:18.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_gms_play_services_vision_common': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_vision_common',
              'version': 'version:18.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_android_material_material': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_material_material',
              'version': 'version:1.2.0-alpha06-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_auto_auto_common': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_auto_auto_common',
              'version': 'version:0.10-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_auto_service_auto_service': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_auto_service_auto_service',
              'version': 'version:1.0-rc6-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_auto_service_auto_service_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_auto_service_auto_service_annotations',
              'version': 'version:1.0-rc6-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_auto_value_auto_value_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_auto_value_auto_value_annotations',
              'version': 'version:1.7-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_code_findbugs_jFormatString': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_code_findbugs_jformatstring',
              'version': 'version:3.0.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_code_findbugs_jsr305': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_code_findbugs_jsr305',
              'version': 'version:3.0.2-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_code_gson_gson': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_code_gson_gson',
              'version': 'version:2.8.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_dagger_dagger': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_dagger_dagger',
              'version': 'version:2.30-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_dagger_dagger_compiler': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_dagger_dagger_compiler',
              'version': 'version:2.30-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_dagger_dagger_producers': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_dagger_dagger_producers',
              'version': 'version:2.30-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_dagger_dagger_spi': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_dagger_dagger_spi',
              'version': 'version:2.30-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_errorprone_error_prone_annotation': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_errorprone_error_prone_annotation',
              'version': 'version:2.4.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_errorprone_error_prone_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_errorprone_error_prone_annotations',
              'version': 'version:2.4.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_errorprone_error_prone_check_api': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_errorprone_error_prone_check_api',
              'version': 'version:2.4.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_errorprone_error_prone_core': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_errorprone_error_prone_core',
              'version': 'version:2.4.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_errorprone_error_prone_type_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_errorprone_error_prone_type_annotations',
              'version': 'version:2.4.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_errorprone_javac': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_errorprone_javac',
              'version': 'version:9+181-r4173-1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_errorprone_javac_shaded': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_errorprone_javac_shaded',
              'version': 'version:9-dev-r4023-3-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_googlejavaformat_google_java_format': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_googlejavaformat_google_java_format',
              'version': 'version:1.5-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_guava_failureaccess': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_guava_failureaccess',
              'version': 'version:1.0.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_guava_guava': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_guava_guava',
              'version': 'version:30.1-jre-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_guava_guava_android': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_guava_guava_android',
              'version': 'version:30.1-android-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_guava_listenablefuture': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_guava_listenablefuture',
              'version': 'version:1.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_j2objc_j2objc_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_j2objc_j2objc_annotations',
              'version': 'version:1.3-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_protobuf_protobuf_java': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_protobuf_protobuf_java',
              'version': 'version:3.4.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_google_protobuf_protobuf_javalite': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_protobuf_protobuf_javalite',
              'version': 'version:3.13.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_googlecode_java_diff_utils_diffutils': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_googlecode_java_diff_utils_diffutils',
              'version': 'version:1.3.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_squareup_javapoet': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_squareup_javapoet',
              'version': 'version:1.13.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/com_squareup_javawriter': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_squareup_javawriter',
              'version': 'version:2.1.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/commons_cli_commons_cli': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/commons_cli_commons_cli',
              'version': 'version:1.3.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/javax_annotation_javax_annotation_api': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/javax_annotation_javax_annotation_api',
              'version': 'version:1.3.2-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/javax_annotation_jsr250_api': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/javax_annotation_jsr250_api',
              'version': 'version:1.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/javax_inject_javax_inject': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/javax_inject_javax_inject',
              'version': 'version:1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/nekohtml_nekohtml': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/nekohtml_nekohtml',
              'version': 'version:1.9.6.2-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/nekohtml_xercesMinimal': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/nekohtml_xercesminimal',
              'version': 'version:1.9.6.2-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/net_ltgt_gradle_incap_incap': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/net_ltgt_gradle_incap_incap',
              'version': 'version:0.2-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/net_sf_kxml_kxml2': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/net_sf_kxml_kxml2',
              'version': 'version:2.3.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_apache_ant_ant': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_apache_ant_ant',
              'version': 'version:1.8.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_apache_ant_ant_launcher': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_apache_ant_ant_launcher',
              'version': 'version:1.8.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_apache_maven_maven_ant_tasks': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_apache_maven_maven_ant_tasks',
              'version': 'version:2.1.3-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_apache_maven_maven_artifact': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_apache_maven_maven_artifact',
              'version': 'version:2.2.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_apache_maven_maven_artifact_manager': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_apache_maven_maven_artifact_manager',
              'version': 'version:2.2.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_apache_maven_maven_error_diagnostics': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_apache_maven_maven_error_diagnostics',
              'version': 'version:2.2.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_apache_maven_maven_model': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_apache_maven_maven_model',
              'version': 'version:2.2.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_apache_maven_maven_plugin_registry': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_apache_maven_maven_plugin_registry',
              'version': 'version:2.2.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_apache_maven_maven_profile': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_apache_maven_maven_profile',
              'version': 'version:2.2.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_apache_maven_maven_project': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_apache_maven_maven_project',
              'version': 'version:2.2.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_apache_maven_maven_repository_metadata': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_apache_maven_maven_repository_metadata',
              'version': 'version:2.2.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_apache_maven_maven_settings': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_apache_maven_maven_settings',
              'version': 'version:2.2.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_apache_maven_wagon_wagon_file': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_apache_maven_wagon_wagon_file',
              'version': 'version:1.0-beta-6-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_apache_maven_wagon_wagon_http_lightweight': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_apache_maven_wagon_wagon_http_lightweight',
              'version': 'version:1.0-beta-6-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_apache_maven_wagon_wagon_http_shared': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_apache_maven_wagon_wagon_http_shared',
              'version': 'version:1.0-beta-6-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_apache_maven_wagon_wagon_provider_api': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_apache_maven_wagon_wagon_provider_api',
              'version': 'version:1.0-beta-6-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_ccil_cowan_tagsoup_tagsoup': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_ccil_cowan_tagsoup_tagsoup',
              'version': 'version:1.2.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_checkerframework_checker_compat_qual': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_checkerframework_checker_compat_qual',
              'version': 'version:2.5.5-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_checkerframework_checker_qual': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_checkerframework_checker_qual',
              'version': 'version:3.5.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_checkerframework_dataflow_shaded': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_checkerframework_dataflow_shaded',
              'version': 'version:3.1.2-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_codehaus_mojo_animal_sniffer_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_codehaus_mojo_animal_sniffer_annotations',
              'version': 'version:1.17-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_codehaus_plexus_plexus_container_default': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_codehaus_plexus_plexus_container_default',
              'version': 'version:1.0-alpha-9-stable-1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_codehaus_plexus_plexus_interpolation': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_codehaus_plexus_plexus_interpolation',
              'version': 'version:1.11-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_codehaus_plexus_plexus_utils': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_codehaus_plexus_plexus_utils',
              'version': 'version:1.5.15-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_jdom_jdom2': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_jdom_jdom2',
              'version': 'version:2.0.6-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_jetbrains_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_jetbrains_annotations',
              'version': 'version:13.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_jetbrains_kotlin_kotlin_stdlib': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_jetbrains_kotlin_kotlin_stdlib',
              'version': 'version:1.3.72-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_jetbrains_kotlin_kotlin_stdlib_common': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_jetbrains_kotlin_kotlin_stdlib_common',
              'version': 'version:1.3.72-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_jetbrains_kotlinx_kotlinx_metadata_jvm': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_jetbrains_kotlinx_kotlinx_metadata_jvm',
              'version': 'version:0.1.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_ow2_asm_asm': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_ow2_asm_asm',
              'version': 'version:7.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_ow2_asm_asm_analysis': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_ow2_asm_asm_analysis',
              'version': 'version:7.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_ow2_asm_asm_commons': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_ow2_asm_asm_commons',
              'version': 'version:7.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_ow2_asm_asm_tree': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_ow2_asm_asm_tree',
              'version': 'version:7.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_ow2_asm_asm_util': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_ow2_asm_asm_util',
              'version': 'version:7.0-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_pcollections_pcollections': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_pcollections_pcollections',
              'version': 'version:2.1.2-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_robolectric_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_annotations',
              'version': 'version:4.3.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_robolectric_junit': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_junit',
              'version': 'version:4.3.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_robolectric_pluginapi': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_pluginapi',
              'version': 'version:4.3.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_robolectric_plugins_maven_dependency_resolver': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_plugins_maven_dependency_resolver',
              'version': 'version:4.3.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_robolectric_resources': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_resources',
              'version': 'version:4.3.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_robolectric_robolectric': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_robolectric',
              'version': 'version:4.3.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_robolectric_sandbox': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_sandbox',
              'version': 'version:4.3.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_robolectric_shadowapi': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_shadowapi',
              'version': 'version:4.3.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_robolectric_shadows_framework': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_shadows_framework',
              'version': 'version:4.3.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_robolectric_shadows_multidex': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_shadows_multidex',
              'version': 'version:4.3.1-cr1',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_robolectric_shadows_playservices': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_shadows_playservices',
              'version': 'version:4.3.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_robolectric_utils': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_utils',
              'version': 'version:4.3.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_robolectric_utils_reflector': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_utils_reflector',
              'version': 'version:4.3.1-cr0',
          },
      ],
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/libs/org_threeten_threeten_extra': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_threeten_threeten_extra',
              'version': 'version:1.5.0-cr0',
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
}


include_rules = [
  # Everybody can use some things.
  # NOTE: THIS HAS TO STAY IN SYNC WITH third_party/DEPS which disallows these.
  '+base',
  '+build',
  '+ipc',

  # Everybody can use headers generated by tools/generate_library_loader.
  '+library_loaders',

  '+testing',
  '+third_party/icu/source/common/unicode',
  '+third_party/icu/source/i18n/unicode',
  '+url',

  # Abseil features must be allowlisted explicitly for now. See
  # //styleguide/c++/c++11.html. Allowed features' headers will be listed
  # explicitly here.
  '-absl',
  '-third_party/abseil-cpp',
  '+third_party/abseil-cpp/absl/types/variant.h',
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
  {
    # This clobbers when necessary (based on get_landmines.py). It must be the
    # first hook so that other things that get/generate into the output
    # directory will not subsequently be clobbered.
    'name': 'landmines',
    'pattern': '.',
    'action': [
        'python',
        'src/build/landmines.py',
    ],
  },
  {
    # This clobbers when necessary (based on get_landmines.py). It must be the
    # first hook so that other things that get/generate into the output
    # directory will not subsequently be clobbered. This script is only run
    # for iOS build with src_internal.
    'name': 'landmines_ios_internal',
    'pattern': '.',
    'condition': 'checkout_ios and checkout_src_internal',
    'action': [
        'python',
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
        'python',
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
        'python',
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
        'src/tools',
    ],
  },
  {
    # Verify that we have the right GN binary and force-install it if we
    # don't, in order to work around crbug.com/944367.
    # TODO(crbug.com/944667) Get rid of this when cipd is ensuring we
    # have the right binary more carefully and we no longer need this.
    'name': 'ensure_gn_version',
    'pattern': '.',
    'action': [
      'python',
      'src/buildtools/ensure_gn_version.py',
      Var('gn_version')
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
        'python',
        'src/build/download_nacl_toolchains.py',
        '--mode', 'nacl_core_sdk',
        'sync', '--extract',
    ],
  },
  {
    'name': 'sysroot_arm',
    'pattern': '.',
    'condition': 'checkout_linux and checkout_arm',
    'action': ['python', 'src/build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=arm'],
  },
  {
    'name': 'sysroot_arm64',
    'pattern': '.',
    'condition': 'checkout_linux and checkout_arm64',
    'action': ['python', 'src/build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=arm64'],
  },
  {
    'name': 'sysroot_x86',
    'pattern': '.',
    'condition': 'checkout_linux and (checkout_x86 or checkout_x64)',
    'action': ['python', 'src/build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=x86'],
  },
  {
    'name': 'sysroot_mips',
    'pattern': '.',
    'condition': 'checkout_linux and checkout_mips',
    'action': ['python', 'src/build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=mips'],
  },
  {
    'name': 'sysroot_mips64',
    'pattern': '.',
    'condition': 'checkout_linux and checkout_mips64',
    'action': ['python', 'src/build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=mips64el'],
  },

  {
    'name': 'sysroot_x64',
    'pattern': '.',
    'condition': 'checkout_linux and checkout_x64',
    'action': ['python', 'src/build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=x64'],
  },
  {
    # Case-insensitivity for the Win SDK. Must run before win_toolchain below.
    'name': 'ciopfs_linux',
    'pattern': '.',
    'condition': 'checkout_win and host_os == "linux"',
    'action': [ 'python',
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
    'action': ['python', 'src/build/vs_toolchain.py', 'update', '--force'],
  },
  {
    # Update the Mac toolchain if necessary.
    'name': 'mac_toolchain',
    'pattern': '.',
    'condition': 'checkout_mac',
    'action': ['python', 'src/build/mac_toolchain.py'],
  },
  {
    # Update the prebuilt clang toolchain.
    # Note: On Win, this should run after win_toolchain, as it may use it.
    'name': 'clang',
    'pattern': '.',
    'condition': 'not llvm_force_head_revision',
    'action': ['python', 'src/tools/clang/scripts/update.py'],
  },
  {
    # Build the clang toolchain from tip-of-tree.
    # Note: On Win, this should run after win_toolchain, as it may use it.
    'name': 'clang_tot',
    'pattern': '.',
    'condition': 'llvm_force_head_revision',
    'action': ['python', 'src/tools/clang/scripts/build.py',
               '--llvm-force-head-revision',
               '--with-android={checkout_android}'],
  },
  {
    # This is supposed to support the same set of platforms as 'clang' above.
    'name': 'clang_coverage',
    'pattern': '.',
    'condition': 'checkout_clang_coverage_tools',
    'action': ['python', 'src/tools/clang/scripts/update.py',
               '--package=coverage_tools'],
  },
  {
    # This is also supposed to support the same set of platforms as 'clang'
    # above. LLVM ToT support isn't provided at the moment.
    'name': 'clang_tidy',
    'pattern': '.',
    'condition': 'checkout_clang_tidy',
    'action': ['python', 'src/tools/clang/scripts/update.py',
               '--package=clang-tidy'],
  },
  {
    # Mac doesn't use lld so it's not included in the default clang bundle
    # there.  lld is however needed in win and Fuchsia cross builds, so
    # download it there. Should run after the clang hook.
    'name': 'lld/mac',
    'pattern': '.',
    'condition': 'host_os == "mac" and (checkout_win or checkout_fuchsia)',
    'action': ['python', 'src/tools/clang/scripts/update.py',
               '--package=lld_mac'],
  },
  {
    # Should run after the clang hook.
    'name': 'objdump/mac',
    'pattern': '.',
    'condition': 'checkout_mac and host_os != "mac"',
    'action': ['python', 'src/tools/clang/scripts/update.py',
               '--package=objdump'],
  },
  {
    # Update LASTCHANGE.
    'name': 'lastchange',
    'pattern': '.',
    'action': ['python', 'src/build/util/lastchange.py',
               '-o', 'src/build/util/LASTCHANGE'],
  },
  {
    # Update GPU lists version string (for gpu/config).
    'name': 'gpu_lists_version',
    'pattern': '.',
    'action': ['python', 'src/build/util/lastchange.py',
               '-m', 'GPU_LISTS_VERSION',
               '--revision-id-only',
               '--header', 'src/gpu/config/gpu_lists_version.h'],
  },
  {
    # Update skia_commit_hash.h.
    'name': 'lastchange_skia',
    'pattern': '.',
    'action': ['python', 'src/build/util/lastchange.py',
               '-m', 'SKIA_COMMIT_HASH',
               '-s', 'src/third_party/skia',
               '--header', 'src/skia/ext/skia_commit_hash.h'],
  },
  # Pull clang-format binaries using checked-in hashes.
  {
    'name': 'clang_format_win',
    'pattern': '.',
    'condition': 'host_os == "win"',
    'action': [ 'python',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-clang-format',
                '-s', 'src/buildtools/win/clang-format.exe.sha1',
    ],
  },
  {
    'name': 'clang_format_mac',
    'pattern': '.',
    'condition': 'host_os == "mac"',
    'action': [ 'python',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-clang-format',
                '-s', 'src/buildtools/mac/clang-format.sha1',
    ],
  },
  {
    'name': 'clang_format_linux',
    'pattern': '.',
    'condition': 'host_os == "linux"',
    'action': [ 'python',
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
    'action': [ 'python',
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
    'action': [ 'python',
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
    'action': [ 'python',
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
    'action': [ 'download_from_google_storage',
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
    'action': ['download_from_google_storage',
               '--no_auth',
               '--quiet',
               '--bucket', 'chromium-webrtc-resources',
               '-d', 'src/third_party/opus/tests/resources'],
  },
  {
    'name': 'apache_mac',
    'pattern': '\\.sha1',
    'condition': 'checkout_mac',
    'action': [ 'python',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--directory',
                '--recursive',
                '--no_auth',
                '--num_threads=16',
                '--bucket', 'chromium-apache-mac',
                'src/third_party/apache-mac',
    ],
  },
  {
    'name': 'apache_win32',
    'pattern': '\\.sha1',
    'condition': 'checkout_win',
    'action': [ 'python',
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
    'name': 'msan_chained_origins',
    'pattern': '.',
    'condition': 'checkout_instrumented_libraries',
    'action': [ 'python',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-instrumented-libraries',
                '-s', 'src/third_party/instrumented_libraries/binaries/msan-chained-origins-trusty.tgz.sha1',
              ],
  },
  {
    'name': 'msan_no_origins',
    'pattern': '.',
    'condition': 'checkout_instrumented_libraries',
    'action': [ 'python',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-instrumented-libraries',
                '-s', 'src/third_party/instrumented_libraries/binaries/msan-no-origins-trusty.tgz.sha1',
              ],
  },
  {
    'name': 'wasm_fuzzer',
    'pattern': '.',
    'action': [ 'python',
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
    'action': [ 'python',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--extract',
                '--no_auth',
                '--bucket', 'chromium-nodejs/14.15.4',
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
  {
    'name': 'node_mac',
    'pattern': '.',
    'condition': 'host_os == "mac"',
    'action': [ 'python',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--extract',
                '--no_auth',
                '--bucket', 'chromium-nodejs/14.15.4',
                '-s', 'src/third_party/node/mac/node-darwin-x64.tar.gz.sha1',
    ],
  },
  {
    'name': 'node_win',
    'pattern': '.',
    'condition': 'host_os == "win"',
    'action': [ 'python',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-nodejs/14.15.4',
                '-s', 'src/third_party/node/win/node.exe.sha1',
    ],
  },

  # Pull down NPM dependencies for WebUI toolchain.
  {
    'name': 'webui_node_modules',
    'pattern': '.',
    'action': [ 'python',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--extract',
                '--no_auth',
                '--bucket', 'chromium-nodejs',
                '-s', 'src/third_party/node/node_modules.tar.gz.sha1',
    ],
  },

  # Download Telemetry's binary dependencies via conditionals
  {
    'name': 'checkout_telemetry_binary_dependencies',
    'condition': 'checkout_telemetry_dependencies',
    'pattern': '.',
    'action': [ 'vpython',
                'src/third_party/catapult/telemetry/bin/fetch_telemetry_binary_dependencies',
    ],
  },

  # Download Telemetry's benchmark binary dependencies via conditionals
  {
    'name': 'checkout_telemetry_benchmark_deps',
    'condition': 'checkout_telemetry_dependencies and checkout_linux and not checkout_android and not skip_wpr_archives_download',
    'pattern': '.',
    'action': [ 'vpython',
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
    'action': [ 'vpython',
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
    'action': [ 'vpython',
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
    'action': [ 'vpython',
                'src/tools/perf/fetch_benchmark_deps.py',
                '-f',
                '-p',
                'android'
    ],
  },


  # This is used to ensure that all network operations are properly
  # annotated so we can document what they're for.
  {
    'name': 'tools_traffic_annotation_linux',
    'pattern': '.',
    'condition': 'host_os == "linux" and checkout_traffic_annotation_tools',
    'action': [ 'python',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--num_threads=4',
                '--bucket', 'chromium-tools-traffic_annotation',
                '-d', 'src/tools/traffic_annotation/bin/linux64',
    ],
  },

# This is used to ensure that all network operations are properly
# annotated so we can document what they're for.
  {
    'name': 'tools_traffic_annotation_windows',
    'pattern': '.',
    'condition': 'host_os == "win" and checkout_traffic_annotation_tools',
    'action': [ 'python',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--num_threads=4',
                '--bucket', 'chromium-tools-traffic_annotation',
                '-d', 'src/tools/traffic_annotation/bin/win32',
    ],
  },

  # Pull down Zucchini test data.
  {
    'name': 'zucchini_testdata',
    'pattern': '.',
    'action': [ 'python',
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
    'action': [ 'python',
                'src/chrome/test/data/android/manage_wpr_archives.py',
                'download',
    ],
  },
  {
    'name': 'Fetch Android AFDO profile',
    'pattern': '.',
    'condition': 'checkout_android or checkout_linux',
    'action': [ 'vpython',
                'src/tools/download_optimization_profile.py',
                '--newest_state=src/chrome/android/profiles/newest.txt',
                '--local_state=src/chrome/android/profiles/local.txt',
                '--output_name=src/chrome/android/profiles/afdo.prof',
                '--gs_url_base=chromeos-prebuilt/afdo-job/llvm',
    ],
  },
  {
    'name': 'gvr_static_shim_android_arm_1',
    'pattern': '\\.sha1',
    'condition': 'checkout_android',
    'action': [ 'python',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-gvr-static-shim',
                '-s', 'src/third_party/gvr-android-sdk/libgvr_shim_static_arm_1.a.sha1',
    ],
  },
  {
    'name': 'gvr_static_shim_android_arm_Cr',
    'pattern': '\\.sha1',
    'condition': 'checkout_android',
    'action': [ 'python',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-gvr-static-shim',
                '-s', 'src/third_party/gvr-android-sdk/libgvr_shim_static_arm_Cr.a.sha1',
    ],
  },
  {
    'name': 'gvr_static_shim_android_arm64_1',
    'pattern': '\\.sha1',
    'condition': 'checkout_android',
    'action': [ 'python',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-gvr-static-shim',
                '-s', 'src/third_party/gvr-android-sdk/libgvr_shim_static_arm64_1.a.sha1',
    ],
  },
  {
    'name': 'gvr_static_shim_android_arm64_Cr',
    'pattern': '\\.sha1',
    'condition': 'checkout_android',
    'action': [ 'python',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-gvr-static-shim',
                '-s', 'src/third_party/gvr-android-sdk/libgvr_shim_static_arm64_Cr.a.sha1',
    ],
  },
  {
    'name': 'vr_controller_test_api',
    'pattern': '\\.sha1',
    'condition': 'checkout_android',
    'action': [ 'python',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-gvr-static-shim/controller_test_api',
                '-s', 'src/third_party/gvr-android-sdk/test-libraries/controller_test_api.aar.sha1',
    ],
  },
  # Download VR test APKs only if the environment variable is set
  {
    'name': 'vr_test_apks',
    'pattern': '.',
    'condition': 'checkout_android',
    'action': [ 'python',
                'src/third_party/gvr-android-sdk/test-apks/update.py',
    ],
  },
  # DOWNLOAD AR test APKs only if the environment variable is set
  {
    'name': 'ar_test_apks',
    'pattern': '.',
    'condition': 'checkout_android',
    'action': [ 'python',
                'src/third_party/arcore-android-sdk/test-apks/update.py',
    ],
  },
  # Download common ink resources for chromeos.
  {
    'name': 'ink-build',
    'pattern': '.',
    'condition': 'checkout_chromeos',
    'action': [ 'python',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--num_threads=4',
                '--bucket', 'chromium-ink',
                '-d', 'src/third_party/ink/build',
    ],
  },
  {
    'name': 'subresource-filter-ruleset',
    'pattern': '.',
    'action': [ 'python',
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
    'action': [ 'vpython',
                'src/tools/download_optimization_profile.py',
                '--newest_state=src/chromeos/profiles/orderfile.newest.txt',
                '--local_state=src/chromeos/profiles/orderfile.local.txt',
                '--output_name=src/chromeos/profiles/chromeos.orderfile.txt',
                '--gs_url_base=chromeos-prebuilt/afdo-job/orderfiles/vetted',
    ],
  },
  # Download AFDO profiles for Chrome OS for each architecture.
  {
    'name': 'Fetch Chrome OS AFDO profiles (from Intel Atom cores)',
    'pattern': '.',
    'condition': 'checkout_chromeos or checkout_simplechrome',
    'action': [ 'vpython',
                'src/tools/download_optimization_profile.py',
                '--newest_state=src/chromeos/profiles/atom.afdo.newest.txt',
                '--local_state=src/chromeos/profiles/atom.afdo.local.txt',
                '--output_name=src/chromeos/profiles/atom.afdo.prof',
                '--gs_url_base=chromeos-prebuilt/afdo-job/vetted/release',
    ],
  },
  {
    'name': 'Fetch Chrome OS AFDO profiles (from Intel big cores)',
    'pattern': '.',
    'condition': 'checkout_chromeos or checkout_simplechrome',
    'action': [ 'vpython',
                'src/tools/download_optimization_profile.py',
                '--newest_state=src/chromeos/profiles/bigcore.afdo.newest.txt',
                '--local_state=src/chromeos/profiles/bigcore.afdo.local.txt',
                '--output_name=src/chromeos/profiles/bigcore.afdo.prof',
                '--gs_url_base=chromeos-prebuilt/afdo-job/vetted/release',
    ],
  },
  {
    # Pull doclava binaries if building for Android.
    'name': 'doclava',
    'pattern': '.',
    'condition': 'checkout_android',
    'action': [ 'python',
                'src/build/android/download_doclava.py',
    ],
  },

  {
    'name': 'Download Fuchsia SDK',
    'pattern': '.',
    'condition': 'checkout_fuchsia',
    'action': [
      'python',
      'src/build/fuchsia/update_sdk.py',
      '--default-bucket={fuchsia_sdk_bucket}',
    ],
  },

  {
    'name': 'Download Fuchsia system images',
    'pattern': '.',
    'condition': 'checkout_fuchsia',
    'action': [
      'python',
      'src/build/fuchsia/update_images.py',
      '--boot-images={checkout_fuchsia_boot_images}',
      '--default-bucket={fuchsia_images_bucket}',
    ],
  },

  # Download public CrOS simplechrome artifacts. The first hooks is for boards
  # that support VM images, the second hook for all other boards.
  {
    'name': 'cros_simplechrome_artifacts_with_vm',
    'pattern': '.',
    'condition': 'checkout_simplechrome_with_vms and not checkout_src_internal',
    'action': [
      'src/third_party/chromite/bin/cros',
      'chrome-sdk',
      '--fallback-versions=10',
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
      'src/third_party/chromite/bin/cros',
      'chrome-sdk',
      '--fallback-versions=10',
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
    'name': 'cros_simplechrome_artifacts_with_vm_internal',
    'pattern': '.',
    'condition': 'checkout_simplechrome_with_vms and checkout_src_internal',
    'action': [
      'src/third_party/chromite/bin/cros',
      'chrome-sdk',
      '--fallback-versions=10',
      '--nogoma',
      '--nogn-gen',
      '--no-shell',
      '--log-level=warning',
      '--cache-dir=src/build/cros_cache/',
      '--boards={cros_boards_with_qemu_images}',
      '--download-vm',
    ],
  },
  {
    'name': 'cros_simplechrome_artifacts_with_no_vm_internal',
    'pattern': '.',
    'condition': 'checkout_simplechrome and checkout_src_internal',
    'action': [
      'src/third_party/chromite/bin/cros',
      'chrome-sdk',
      '--fallback-versions=10',
      '--nogoma',
      '--nogn-gen',
      '--no-shell',
      '--log-level=warning',
      '--cache-dir=src/build/cros_cache/',
      '--boards={cros_boards}',
    ],
  },

  # Download PGO profiles.
  {
    'name': 'Fetch PGO profiles for win32',
    'pattern': '.',
    'condition': 'checkout_pgo_profiles and checkout_win',
    'action': [ 'vpython',
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
    'action': [ 'vpython',
                'src/tools/update_pgo_profiles.py',
                '--target=win64',
                'update',
                '--gs-url-base=chromium-optimization-profiles/pgo_profiles',
    ],
  },
  {
    'name': 'Fetch PGO profiles for mac',
    'pattern': '.',
    'condition': 'checkout_pgo_profiles and checkout_mac',
    'action': [ 'vpython',
                'src/tools/update_pgo_profiles.py',
                '--target=mac',
                'update',
                '--gs-url-base=chromium-optimization-profiles/pgo_profiles',
    ],
  },
  {
    'name': 'Fetch PGO profiles for linux',
    'pattern': '.',
    'condition': 'checkout_pgo_profiles and checkout_linux',
    'action': [ 'vpython',
                'src/tools/update_pgo_profiles.py',
                '--target=linux',
                'update',
                '--gs-url-base=chromium-optimization-profiles/pgo_profiles',
    ],
  },

  # Download and initialize "vpython" VirtualEnv environment packages.
  {
    'name': 'vpython_common',
    'pattern': '.',
    'action': [ 'vpython',
                '-vpython-spec', 'src/.vpython',
                '-vpython-tool', 'install',
    ],
  },
]

recursedeps = [
  # ANGLE manages DEPS that it also owns the build files for, such as dEQP.
  'src/third_party/angle',
  'src/third_party/openscreen/src',
  'src/third_party/vulkan-deps',
  # src-internal has its own DEPS file to pull additional internal repos
  'src-internal',
]
