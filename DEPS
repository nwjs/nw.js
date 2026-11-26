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
#  git commit -a
#  git cl upload
#
# For more on the syntax and semantics of this file, see:
#   https://bit.ly/chromium-gclient-conditionals
#
# which is a bit incomplete but the best documentation we have at the
# moment.

# We expect all git dependencies specified in this file to be in sync with git
# submodules (gitlinks).
git_dependencies = 'SYNC'

gclient_gn_args_file = 'src/build/config/gclient_args.gni'
gclient_gn_args = [
  'android_ndk_version',
  'build_with_chromium',
  'checkout_android',
  'checkout_android_prebuilts_build_tools',
  'checkout_clang_coverage_tools',
  'checkout_copybara',
  'checkout_fuchsia',
  'checkout_glic_e2e_tests',
  'checkout_ios_webkit',
  'checkout_mutter',
  'checkout_openxr',
  'checkout_src_internal',
  'checkout_src_internal_infra',
  'checkout_clusterfuzz_data',
  'cros_boards',
  'cros_boards_with_qemu_images',
  'generate_location_tags',
]


vars = {
  'nw_src_revision': '666dfda972ab7af97e9d86ae45d8883908b37c0a',
  'nw_v8_revision': '59f3d506e4ef4095b4d58ef2efb2cda1f8fffafb',
  'nw_node_revision': 'c4521b945781cc625bde96274a65b4325e281c34',
  # The version of the NDK. Set here, to allow the autoroller to update this
  # value when updating the CIPD hash.
  'android_ndk_version': Str('2@30.0.16248370'),

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
  # TODO(crbug.com/875037): Remove this once the problem in gclient is fixed.
  'checkout_android': False,

  # By default, don't check out Fuchsia. Will be overridden by gclient
  # variables.
  # TODO(crbug.com/875037): Remove this once the problem in gclient is fixed.
  'checkout_fuchsia': False,

  # For code related to internal Fuchsia images.
  'checkout_fuchsia_internal': False,

  # Fetches the internal Fuchsia SDK boot images, with the images in a
  # comma-separated list.
  'checkout_fuchsia_internal_images': '',

  # Used for downloading the Fuchsia SDK without running hooks.
  'checkout_fuchsia_no_hooks': False,

  # Pull in Android prebuilts build tools so we can create Java xrefs
  'checkout_android_prebuilts_build_tools': False,

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

  # Checkout out mutter and its dependencies to be able to run tests like
  # interactive_ui_tests on the linux/wayland compositor.
  'checkout_mutter': False,

  # By default, do not check out src-internal. This can be overridden e.g. with
  # custom_vars.
  'checkout_src_internal': False,

  # By default, do not check out //src/internal. This can be overridden e.g. with
  # custom_vars. This acts the same way as checkout_src_internal, but only affects
  # the internal infra folder, instead of all internal repos. It is used by
  # Cronet internal gn2bp to make sure no internal source code is uploaded.
  # See https://crbug.com/404202679: do not modify the set of directories this
  # acts upon.
  'checkout_src_internal_infra' : False,

  # Checkout legacy src_internal. This variable is ignored if
  # checkout_src_internal is set as false.
  'checkout_legacy_src_internal': True,


  # Checkout test code and archives for glic E2E tests.
  'checkout_glic_e2e_tests': False,

  # By default, check out Meet GPU test asset bundles (~278MB) unless
  # checkout_configuration is set to "small".
  'checkout_gpu_meet_effects': 'checkout_configuration != "small"',

  # For super-internal deps. Set by the official builders.
  'checkout_google_internal': False,

  # Checkout SODA (Speech On-Device API go/chrome-live-caption)
  'checkout_soda': False,

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

  # Fetch clangd into the same bin/ directory as our clang binary.
  'checkout_clangd': False,

  # By default checkout the OpenXR loader library on Windows, Linux and Android.
  # The OpenXR backend for VR in Chromium is supported on these platforms;
  # support for other platforms may be added in the future.
  'checkout_openxr' : 'checkout_win or checkout_linux or checkout_android',

  # By default, do not check out instrumented libraries. These prebuilt
  # binaries are only consumed by MSan builds (`is_msan = true` in GN).
  #
  # They should only be checked out on Linux environments with a full checkout
  # configuration (`checkout_linux and checkout_configuration != "small"`)
  # when specifically compiling MSan targets.
  'checkout_instrumented_libraries': False,

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
  #   - terminal.x64
  #   - terminal.qemu-arm64
  #   - workstation.qemu-x64
  #   Hardware:
  #   - workstation_eng.chromebook-x64
  #   - workstation_eng.chromebook-x64-dfv2
  #
  # Since the images are hundreds of MB, default to only downloading the image
  # most commonly useful for developers. Bots and developers that need to use
  # other images can override this with additional images.
  'checkout_fuchsia_boot_images': "terminal.x64",
  'checkout_fuchsia_product_bundles': '"{checkout_fuchsia_boot_images}" != ""',

  # By default, do not check out files required to run fuchsia tests in
  # qemu on linux-arm64 machines.
  'checkout_fuchsia_for_arm64_host': False,

  # By default, download the fuchsia sdk from the public sdk directory.
  'fuchsia_sdk_cipd_prefix': 'fuchsia/sdk/core/',

  # By default, download the fuchsia images from the fuchsia GCS bucket.
  'fuchsia_images_bucket': 'fuchsia',

  # Default to the empty board. Desktop Chrome OS builds don't need cros SDK
  # dependencies. Other Chrome OS builds should always define this explicitly.
  'cros_boards': Str(''),
  'cros_boards_with_qemu_images': Str(''),
  # Building for CrOS is only supported on linux currently.
  'checkout_simplechrome': '"{cros_boards}" != ""',
  'checkout_simplechrome_with_vms': '"{cros_boards_with_qemu_images}" != ""',

  # Generate location tag metadata to include in tests result data uploaded
  # to ResultDB. This isn't needed on some configs and the tool that generates
  # the data may not run on them, so we make it possible for this to be
  # turned off. Note that you also generate the metadata but not include it
  # via a GN build arg (tests_have_location_tags).
  'generate_location_tags': True,

  # By default, do not check out Copybara 3pp dependency that is specifically
  # needed by Cronet gn2bp CI builder.
  'checkout_copybara': False,

  # By default, check out the latest press benchmark versions for local
  # testing and debugging. Benchmarks are also hosted on publicly accessible
  # sites as backup solution.
  'checkout_press_benchmarks': 'checkout_configuration != "small"',

  # luci-go CIPD package version.
  # Make sure the revision is uploaded by infra-packagers builder.
  # https://ci.chromium.org/p/infra-internal/g/infra-packagers/console
  'luci_go': 'git_revision:24dbdf6f322fca1b840de9950bfce2d10ce1df73',

  # This can be overridden, e.g. with custom_vars, to build clang from HEAD
  # instead of downloading the prebuilt pinned revision.
  'llvm_force_head_revision': False,

  # This can be overridden, e.g. with custom_vars, to build rust from HEAD
  # against ToT LLVM, instead of downloading the prebuilt pinned revision.
  'rust_force_head_revision': False,

  # Make Dawn skip its standalone dependencies
  'dawn_standalone': False,

  # Fetch configuration files required for the 'use_remoteexec' gn arg
  'download_remoteexec_cfg': False,
  # RBE instance to use for running remote builds
  # Ignored if reapi_instance is configured for non-RBE address.
  'rbe_instance': Str('projects/rbe-chrome-untrusted/instances/default_instance'),
  # REAPI instance for non-RBE backends.
  # need to set reapi_address too.
  'reapi_instance': Str(''),
  # REAPI address for REAPI backends.
  'reapi_address': Str(''),
  # REAPI backend config path for Siso.
  # pathname relative to build/config/siso/backend_config, or absolute path.
  'reapi_backend_config_path': Str(''),
  # REAPI credential helper to use for Siso.
  # binary available on `PATH`, or absolute path.
  'reapi_credential_helper': Str(''),
  # siso CIPD package version.
  'siso_version': 'git_revision:dfc32e4edca158cff2b64a1658ee3940f0116b3a',

  # CPython 3 CIPD package version for Siso hermetic toolchain.
  'cpython3_version': 'version:3@3.11.9.chromium.38',

  # reclient options.
  # download reclient binaries, required for 'use_reclient` gn arg.
  # TODO(crbug.com/448517720): make it false by default.
  'download_reclient': 'checkout_chromeos',
  # RBE project to download rewrapper config files for. Only needed if
  # different from the project used in 'rbe_instance'
  'rewrapper_cfg_project': Str(''),
  # reclient CIPD package
  'reclient_package': 'infra/rbe/client/',
  # reclient CIPD package version
  'reclient_version': 're_client_version:0.185.0.db415f21-gomaip',

  # screen-ai CIPD packages
  'screen_ai_linux': 'version:153.05',
  'screen_ai_macos_amd64': 'version:153.05',
  'screen_ai_macos_arm64': 'version:153.05',
  'screen_ai_windows_amd64': 'version:153.05',
  'screen_ai_windows_386': 'version:153.05',

  # download libaom test data
  'download_libaom_testdata': False,

  # download libvpx test data
  'download_libvpx_testdata': False,

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
  # the commit queue can handle CLs rolling V8
  # and whatever else without interference from each other.
  'src_internal_revision': '882ccf18650dffce7bbee7522dcdde77e4e1969a',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling Skia
  # and whatever else without interference from each other.
  'skia_revision': '3279352fdf1fbbd01a6dd024a52d3f178a6d5f07',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling V8
  # and whatever else without interference from each other.
  'v8_revision': 'b601a2347f005e8bfadd32f285df97ffba5cf1bd',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling ANGLE
  # and whatever else without interference from each other.
  'angle_revision': '1f6526a566c27c7c33aa73a78517450a4a2a1714',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling SwiftShader
  # and whatever else without interference from each other.
  'swiftshader_revision': 'cddb5c91b23ff70a1f607ea31610aa0d7f4b5ad5',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling PDFium
  # and whatever else without interference from each other.
  'pdfium_revision': 'b23fee3ac22360878efaa8aa533dac892fe777ab',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling BoringSSL
  # and whatever else without interference from each other.
  'boringssl_revision': 'ea9f3a34c3977144c45d272be81842fd891a0bdb',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling Fuchsia sdk
  # and whatever else without interference from each other.
  'fuchsia_version': 'version:33.20261022.4.1',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling googletest
  # and whatever else without interference from each other.
  'googletest_revision': '0b656495436f57212be8700794a3d5fb14000c5f',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling lss
  # and whatever else without interference from each other.
  'lss_revision': '29164a80da4d41134950d76d55199ea33fbb9613',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling breakpad
  # and whatever else without interference from each other.
  'breakpad_revision': '94c818ea27482d8a8d32b0c8de50cbca0270db9a',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling freetype
  # and whatever else without interference from each other.
  'freetype_revision': 'a5877f48fff8dd6cf8d53c505ce055499376aee2',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling HarfBuzz
  # and whatever else without interference from each other.
  'harfbuzz_revision': '12ee77ee00514c442bcd28f9aa9d56b708c37382',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling Emoji Segmenter
  # and whatever else without interference from each other.
  'emoji_segmenter_revision': '955936be8b391e00835257059607d7c5b72ce744',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling OTS
  # and whatever else without interference from each other.
  'ots_revision': '46bea9879127d0ff1c6601b078e2ce98e83fcd33',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling catapult
  # and whatever else without interference from each other.
  'catapult_revision': '4a6f0659c26fc478756e83f0d007b91ca5f6d36f',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling CrossBench
  # and whatever else without interference from each other.
  'crossbench_revision': '4e3359f5fdee4d0abaf5425096812e8be613e505',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling CrossBench
  # and whatever else without interference from each other.
  'crossbench_web_tests_revision': 'e0202531507cbca798946c6f0daa869fb3e2c9cc',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling libFuzzer
  # and whatever else without interference from each other.
  'libfuzzer_revision': 'c7ea90fb61105752fa78e257e77a7c5d62fff7d3',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling fuzztest
  # and whatever else without interference from each other.
  'fuzztest_revision': 'a2d3cb097f07a91dd26768db7d9f0898cd42b0f0',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling domato
  # and whatever else without interference from each other.
  'domato_revision': 'fadff396cc45d521cc594d3e2396e27e887b1963',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling devtools-frontend
  # and whatever else without interference from each other.
  'devtools_frontend_revision': '047c90eca37c7170d826d72245d5df9281989cf3',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling libprotobuf-mutator
  # and whatever else without interference from each other.
  'libprotobuf-mutator': 'a0448e5055e4740774e00d496f016d9fcd572200',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling android_sdk_build-tools_version
  # and whatever else without interference from each other.
  'android_sdk_build-tools_version': 'version_37.0.0',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling aapt2_version
  # and whatever else without interference from each other.
  'aapt2_version': 'version:2@9.6.0-alpha01-16666295',
  # Used by the //third_party/android_deps/cipd entry. Rolled by the
  # android_deps autoroller.
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling android_deps_autorolled_version
  # and whatever else without interference from each other.
  'android_deps_autorolled_version': '2hih_vvXKUPFfw6rXNVGefeqVSndktCfh2cMrR4qiPsC',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling android_sdk_emulator_version
  # and whatever else without interference from each other.
  'android_sdk_emulator_version': '9lGp8nTUCRRWGMnI_96HcKfzjnxEJKUcfvfwmA3wXNkC',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling android_sdk_platform-tools_version
  # and whatever else without interference from each other.
  'android_sdk_platform-tools_version': 'qTD9QdBlBf3dyHsN1lJ0RH6AhHxR42Hmg2Ih-Vj4zIEC',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling android_sdk_platforms_version
  # and whatever else without interference from each other.
  'android_sdk_platforms_version': 'AAswZ5rZBFDhMaH4QbKPmVtP4vnjiNUW_zPhdWvNe6wC',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling jdk_version
  # and whatever else without interference from each other.
  'jdk_version': 'version:2@jdk-25.0.5+8.a4d27635bd.cr0',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'dawn_revision': '6e24542436a3ef58d4a1110f01712da9afbff3e3',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'quiche_revision': '132bfd90ea370faeacee616c70d10210bb12ba66',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling ink
  # and whatever else without interference from each other.
  'ink_revision': 'b6def591a73a6aa0ffce1e58dc69b39de31484ba',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling ios_webkit
  # and whatever else without interference from each other.
  'ios_webkit_revision': 'f8c0fe750d94b7db23d193c0b1f31858c2537620',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling libexpat
  # and whatever else without interference from each other.
  'libexpat_revision': 'a851869e111c06fbcdc89edf639abf5fc2b6deba',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling jetstream-main
  # and whatever else without interference from each other.
  'jetstream_main_revision': 'c603c04db8505477867974a69789309ded2cc948',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling jetstream-v2.2
  # and whatever else without interference from each other.
  'jetstream_2.2_revision': '2145cedef4ca2777b792cb0059d3400ee2a6153c',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling jetstream-v3.0
  # and whatever else without interference from each other.
  'jetstream_3.0_revision': '06785cf861ac44855f168cbbe829278c2802e6de',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling wuffs
  # and whatever else without interference from each other.
  'wuffs_revision': '3bbda65fd6e7266f6cd80bc48f0c7ebce4e8f24b',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling crabbyavif
  # and whatever else without interference from each other.
  'crabbyavif_revision': '40732a9090353e3dae550b77d799a0ce5c7a3c29',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling openh264rs
  # and whatever else without interference from each other.
  'openh264rs_revision': '8893d61aeb20b7e1784fe995a70d4b52f691b87f',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling Speedometer main
  # and whatever else without interference from each other.
  'speedometer_main_revision': 'f866d2b077038bc85bea5edb7bff8e05cadef0b2',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling Speedometer v3.1
  # and whatever else without interference from each other.
  'speedometer_3.1_revision': '1386415be8fef2f6b6bbdbe1828872471c5d802a',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling Speedometer v3.0
  # and whatever else without interference from each other.
  'speedometer_3.0_revision': '8d67f28d0281ac4330f283495b7f48286654ad7d',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling Speedometer v2.1
  # and whatever else without interference from each other.
  'speedometer_2.1_revision': '8bf7946e39e47c875c00767177197aea5727e84a',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling Speedometer v2.0
  # and whatever else without interference from each other.
  'speedometer_2.0_revision': '732af0dfe867f8815e662ac637357e55f285dbbb',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling nearby
  # and whatever else without interference from each other.
  'nearby_revision': '0bad8b0c9877f92eeeb550654f1ea51a71a085e4',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling securemessage
  # and whatever else without interference from each other.
  'securemessage_revision': '69a02984d4563c6c21132ee882d1d3df1981f5a1',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling ukey2
  # and whatever else without interference from each other.
  'ukey2_revision': '0275885d8e6038c39b8a8ca55e75d1d4d1727f47',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'cros_components_revision': '8d175bbffcaed7d388701535cc8c95457c179cff',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'result_adapter_revision': 'git_revision:5fb3ca203842fd691cab615453f8e5a14302a1d8',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'libcxxabi_revision':    '440f88852530ac197479edb1463dda1916692cce',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'libunwind_revision':    '9e44ea2ff4f53a60c2a3d2b6f511d7bab9ab12f0',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'clang_format_revision':    '9f796802e5d633c96dbe97e0b6683d5e62594f9f',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'highway_revision': '2607d3b5b0113992fe84d3848859eae13b3b52c1',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling ffmpeg
  # and whatever else without interference from each other.
  'ffmpeg_revision': '42a2a7a2b2ceb0b1554af1e99a8cdaae189404bb',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling webpagereplay
  # and whatever else without interference from each other.
  'webpagereplay_revision': '7ed888744ce019687980074a909b6588db7da971',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling llvm-libc
  # and whatever else without interference from each other.
  'llvm_libc_revision':    '567c143bea85f11d15702a120561a8e51f46f02c',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling llvm-libc
  # and whatever else without interference from each other.
  'compiler_rt_revision': 'b8403c8cb61be59a519c3cf63217190c414bdf07',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling clusterfuzz-data
  # and whatever else without interference from each other.
  'clusterfuzz_data_revision':'db1fa22b3f283ddc2cbe9263651ba48fa5625d8e',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling agents-internal
  # and whatever else without interference from each other.
  'agents_internal_revision': '3eb5fb7503fbcde16249bce04b60c02d42602a74',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling agents-public
  # and whatever else without interference from each other.
  'agents_public_revision': 'e2af388a840c95959617a9d3e6bda280e71fada6',

  # If you change this, also update the libc++ revision in
  # //buildtools/deps_revisions.gni.
  'libcxx_revision':       '9ba7713d3a0f1e179904c984bb626748b15e57d6',

  # GN CIPD package version.
  'gn_version': 'git_revision:938087829fcbcc8fd87835ee301dd522c7637685',

  # ninja CIPD package.
  'ninja_package': 'infra/3pp/tools/ninja/',

  # ninja CIPD package version.
  # https://chrome-infra-packages.appspot.com/p/infra/3pp/tools/ninja
  'ninja_version': 'version:3@1.12.1.chromium.4',

  # 'magic' variable to tell depot_tools that git submodules should be accepted
  # but parity with DEPS file is expected.
  'SUBMODULE_MIGRATION': 'True',

  # condition to allowlist deps to be synced in Cider. Allowlisting is needed
  # because not all deps are compatible with Cider. Once we migrate everything
  # to be compatible we can get rid of this allowlisting mecahnism and remove
  # this condition. Tracking bug for removing this condition: b/349365433
  'non_git_source': 'True',
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

   # TODO(337061377): Move into a separate allowed gcs bucket list.
  'chromium-ads-detection',
  'chromium-browser-clang',
  'chromium-clang-format',
  'chromium-doclava',
  'chromium-nodejs',
  'chrome-linux-sysroot',
  'chromium-fonts',
  'chromium-style-perftest',
  'chromium-telemetry',
  'chromium-webrtc-resources',
  'meet-bundles',
  'perfetto',
]

deps = {
  'src/tools/gyp':
    Var('chromium_git') + '/external/gyp.git' + '@' + 'd61a9397e668fa9843c4aa7da9e79460fe590bfb',

  # NPM dependencies for JavaScript code coverage.
  'src/third_party/js_code_coverage/node_modules': {
    'dep_type': 'gcs',
    'bucket': 'chromium-nodejs',
    'condition': 'non_git_source',
    'objects': [
      {
        'object_name': 'js_code_coverage/e932c86d2d4f250416970dc270002a9cb6acecbec034998cdadf9a394d0f1abc',
        'sha256sum': 'e932c86d2d4f250416970dc270002a9cb6acecbec034998cdadf9a394d0f1abc',
        'size_bytes': 1557200,
        'generation': 1742338539536352,
      }
    ]
  },
  'src/build/linux/debian_bullseye_amd64-sysroot': {
    'bucket': 'chrome-linux-sysroot',
    'condition': 'checkout_linux and checkout_x64 and non_git_source',
    'dep_type': 'gcs',
    'objects': [
      {
        'generation': 1792192152972543,
        'object_name': 'f851add76bdae472fb6c94b4f920bc4a132d85ac607951391297f636af7e934e',
        'sha256sum': 'f851add76bdae472fb6c94b4f920bc4a132d85ac607951391297f636af7e934e',
        'size_bytes': 19314716,
      },
    ],
  },
  'src/build/linux/debian_bullseye_arm64-sysroot': {
    'bucket': 'chrome-linux-sysroot',
    'condition': 'checkout_linux and checkout_arm64 and non_git_source',
    'dep_type': 'gcs',
    'objects': [
      {
        'generation': 1792192630237696,
        'object_name': '278c36fe3a785d7f89fbe3f46956899a3bd49fe665f68dc667d6f626a951fcd6',
        'sha256sum': '278c36fe3a785d7f89fbe3f46956899a3bd49fe665f68dc667d6f626a951fcd6',
        'size_bytes': 18015920,
      },
    ],
  },
  'src/build/linux/debian_bullseye_armhf-sysroot': {
    'bucket': 'chrome-linux-sysroot',
    'condition': 'checkout_linux and checkout_arm and non_git_source',
    'dep_type': 'gcs',
    'objects': [
      {
        'generation': 1792192750740303,
        'object_name': 'd78a9e2739dd4cce413dc8629ad0be840591c6456f9a4c6ed4b29ecf2a4257f3',
        'sha256sum': 'd78a9e2739dd4cce413dc8629ad0be840591c6456f9a4c6ed4b29ecf2a4257f3',
        'size_bytes': 17981276,
      },
    ],
  },
  'src/build/linux/debian_bullseye_i386-sysroot': {
    'bucket': 'chrome-linux-sysroot',
    'condition': 'checkout_linux and (checkout_x86 or checkout_x64) and non_git_source',
    'dep_type': 'gcs',
    'objects': [
      {
        'generation': 1792192792939606,
        'object_name': '854e63992a633c8d54b69959d7b789453239d7f4bc30216594f0e3eb1b520402',
        'sha256sum': '854e63992a633c8d54b69959d7b789453239d7f4bc30216594f0e3eb1b520402',
        'size_bytes': 19375240,
      },
    ],
  },
  'src/build/linux/debian_bullseye_mips64el-sysroot': {
    'bucket': 'chrome-linux-sysroot',
    'condition': 'checkout_linux and checkout_mips64 and non_git_source',
    'dep_type': 'gcs',
    'objects': [
      {
        'generation': 1792192782030052,
        'object_name': '932ee790d10a9a138da669ddff036de8f59111db077a12597f9a0bc062d14b94',
        'sha256sum': '932ee790d10a9a138da669ddff036de8f59111db077a12597f9a0bc062d14b94',
        'size_bytes': 18607664,
      },
    ],
  },
  'src/build/linux/debian_bullseye_mipsel-sysroot': {
    'bucket': 'chrome-linux-sysroot',
    'condition': 'checkout_linux and checkout_mips and non_git_source',
    'dep_type': 'gcs',
    'objects': [
      {
        'generation': 1792192740132829,
        'object_name': 'c52c047102757452c2d740c9d322d56256edd942033a566d0fd242751e4c6fd4',
        'sha256sum': 'c52c047102757452c2d740c9d322d56256edd942033a566d0fd242751e4c6fd4',
        'size_bytes': 18186196,
      },
    ],
  },
  'src/build/linux/debian_bullseye_ppc64el-sysroot': {
    'bucket': 'chrome-linux-sysroot',
    'condition': 'checkout_linux and checkout_ppc and non_git_source',
    'dep_type': 'gcs',
    'objects': [
      {
        'generation': 1792192808061626,
        'object_name': '8f32b4b819bad950eccfed230d8079769324033293e74cea1af36c96a64acce0',
        'sha256sum': '8f32b4b819bad950eccfed230d8079769324033293e74cea1af36c96a64acce0',
        'size_bytes': 18184840,
      },
    ],
  },
  'src/build/linux/debian_bullseye_s390x-sysroot': {
    'bucket': 'chrome-linux-sysroot',
    'condition': 'checkout_linux and checkout_s390 and non_git_source',
    'dep_type': 'gcs',
    'objects': [
      {
        'generation': 1792192150947727,
        'object_name': '544702b33e8d7361272cdb2982c82af3e8227694a3d136d370f27487e6f754f8',
        'sha256sum': '544702b33e8d7361272cdb2982c82af3e8227694a3d136d370f27487e6f754f8',
        'size_bytes': 17930224,
      },
    ],
  },
  'src/build/linux/debian_trixie_riscv64-sysroot': {
    'bucket': 'chrome-linux-sysroot',
    'condition': 'checkout_linux and checkout_riscv64 and non_git_source',
    'dep_type': 'gcs',
    'objects': [
      {
        'generation': 1792192872416094,
        'object_name': 'e43668340baeedccaee12de4f818c8718171ab8c9164df0344939a5dca8650ee',
        'sha256sum': 'e43668340baeedccaee12de4f818c8718171ab8c9164df0344939a5dca8650ee',
        'size_bytes': 19653752,
      },
    ],
  },
  'src/buildtools/win-format': {
    'bucket': 'chromium-clang-format',
    'condition': 'host_os == "win" and non_git_source',
    'dep_type': 'gcs',
    'objects': [
      {
        'object_name': 'fb0bcaf406ad6f0bd6b4a6347d2abe78a94fb13e',
        'sha256sum': 'ee7a1f89733de23daf8ee035f8aebafa52410c9f4dd1f05ee12a95ce517a0953',
        'size_bytes': 3799552,
        'generation': 1779297371701340,
        'output_file': 'clang-format.exe',
      },
    ],
  },
  'src/buildtools/mac-format': {
    'bucket': 'chromium-clang-format',
    'condition': 'host_os == "mac" and host_cpu == "x64" and non_git_source',
    'dep_type': 'gcs',
    'objects': [
      {
        'object_name': '35ddc571ab654a4f58d7b35f394b04835d0b98f9',
        'sha256sum': '39a9c4e138a8c5e6ab5cd7b21321c2ca03f3f518a6ed07de82162d9f4f37318f',
        'size_bytes': 3567528,
        'generation': 1779297382681217,
        'output_file': 'clang-format',
      },
    ],
  },
  'src/buildtools/mac_arm64-format': {
    'bucket': 'chromium-clang-format',
    'condition': 'host_os == "mac" and host_cpu == "arm64" and non_git_source',
    'dep_type': 'gcs',
    'objects': [
      {
        'object_name': '945e45cbcb5443c3e569d5e0783fd012154fc4a7',
        'sha256sum': 'a1fb17260eda2971f52ed341501b93703b0fce69ffb43a179b891b66f6f63e5a',
        'size_bytes': 3245456,
        'generation': 1779297393605542,
        'output_file': 'clang-format',
      },
    ],
  },
  'src/buildtools/linux64-format': {
    'bucket': 'chromium-clang-format',
    'condition': 'host_os == "linux" and non_git_source',
    'dep_type': 'gcs',
    'objects': [
      {
        'object_name': 'f5485c39451137ee943021a0fb63738b306c5026',
        'sha256sum': '5d384f2a3e72f01c5c61dd07e3a2577723cfa20c8c019131467f00fb6da8a75c',
        'size_bytes': 3764032,
        'generation': 1779297360709713,
        'output_file': 'clang-format',
      },
    ],
  },
  'src/buildtools/third_party/mold/cipd': {
      'packages': [
          {
              'package': 'chromium/buildtools/third_party/mold/mold',
              'version': 'w9QqZ-oP3GovA-zjOawYzb5yca-t47GbT-ejPa0BQmYC',
          },
      ],
      'condition': 'host_os == "linux" and non_git_source',
      'dep_type': 'cipd',
  },
  'src/third_party/data_sharing_sdk/cipd': {
      'packages': [
          {
              'package': 'chrome_internal/third_party/google3/data_sharing_sdk',
              'version': 'NQq9pR3VrvsqPtQSfrezFe8hL-z_SfZsONkwC-MqX6wC',
          },
      ],
      'condition': 'checkout_src_internal and non_git_source',
      'dep_type': 'cipd',
  },
  # Pull down Node binaries for WebUI toolchain.
  # The Linux binary is always downloaded regardless of host os and architecture
  # since remote node actions run on Linux worker.
  # See also //third_party/node/node.gni
  'src/third_party/node/linux': {
      'dep_type': 'gcs',
      'condition': 'non_git_source',
      'bucket': 'chromium-nodejs',
      'objects': [
          {
              'object_name': '744e6926ffdd4a4fb2080ae2b9ce4575490261e7',
              'sha256sum': 'bdea62a6857735d62165ec97242d79a693564c72d551890c4d009d2355abf14c',
              'size_bytes': 53566970,
              'generation': 1767604834866657,
              'output_file': 'node-linux-x64.tar.gz',
          },
      ],
  },
  # The Mac x64/arm64 binaries are downloaded regardless of host architecture
  # since it's possible to cross-compile for the other architecture. This can
  # cause problems for tests that use node if the test device architecture does
  # not match the architecture of the compile machine.
  'src/third_party/node/mac': {
      'dep_type': 'gcs',
      'condition': 'host_os == "mac" and non_git_source',
      'bucket': 'chromium-nodejs',
      'objects': [
          {
              'object_name': '9202c05a8e7c67cb2bb2fec1e50fb6188d26f281',
              'sha256sum': 'a25cd3ef35d8b4b5a59498a5a62b5b12cc271dc420ee809abaa76110d12c156e',
              'size_bytes': 49563652,
              'generation': 1767604826991919,
              'output_file': 'node-darwin-x64.tar.gz',
          },
      ],
  },
  'src/third_party/node/mac_arm64': {
      'dep_type': 'gcs',
      'condition': 'host_os == "mac" and non_git_source',
      'bucket': 'chromium-nodejs',
      'objects': [
          {
              'object_name': '6661e9b9bd7df6b45daf506c82d06d303597cb27',
              'sha256sum': 'b1be502d1635330ebf51d85f8d32a0d3dd92b35c6700def56ae6f903906ea825',
              'size_bytes': 48369082,
              'generation': 1767604830942392,
              'output_file': 'node-darwin-arm64.tar.gz',
          },
      ],
  },
  'src/third_party/node/win': {
      'dep_type': 'gcs',
      'condition': 'host_os == "win" and non_git_source',
      'bucket': 'chromium-nodejs',
      'objects': [
          {
              'object_name': '2f710ced2db2beb7c3debf6097196c35ee5adb74',
              'sha256sum': '2ffe3acc0458fdde999f50d11809bbe7c9b7ef204dcf17094e325d26ace101d8',
              'size_bytes': 89935872,
              'generation': 1767604839180114,
              'output_file': 'node.exe',
          },
      ],
  },
  # Pull down NPM dependencies for WebUI toolchain.
  'src/third_party/node/node_modules': {
    'bucket': 'chromium-nodejs',
    'dep_type': 'gcs',
    'condition': 'non_git_source',
    'objects': [
      {
        'object_name': '8b170cc9b6a9725b4082b7b2255ca797bc80ea5f',
        'sha256sum': '5b5e38c58094ee898b1eceb29df7d3a2a2e6b07add6a05b4c554eef0dfb56465',
        'size_bytes': 11436046,
        'generation': 1792004641957491,
        'output_file': 'node_modules.tar.gz',
      },
    ],
  },
  # Pull NPM dependencies for chromium-bidi.
  'src/third_party/chromium-bidi/node_modules': {
    'dep_type': 'gcs',
    'bucket': 'chromium-nodejs',
    'condition': 'non_git_source',
    'objects': [
      {
        'object_name': 'chromium-bidi/7da310bf46956828c164237a912a201d21f51220dfbe60df454188dbad75889c',
        'sha256sum': '7da310bf46956828c164237a912a201d21f51220dfbe60df454188dbad75889c',
        'size_bytes': 20868481,
        'generation': 1790681952304657,
        'output_file': 'node_modules.tar.gz',
      },
    ],
  },
  'src/third_party/llvm-build/Release+Asserts': {
    'dep_type': 'gcs',
    'bucket': 'chromium-browser-clang',
    'condition': 'not llvm_force_head_revision',
    'objects': [
      {
        'object_name': 'Linux_x64/clang-android-runtime-library-llvmorg-24-init-14453-ge063be79-1.tar.xz',
        'sha256sum': 'dd03492cad72f223d7743c2f4af39c405f5bac7f7f342de09697a45b7108ce4e',
        'size_bytes': 6188284,
        'generation': 1792086693754706,
        'condition': 'checkout_android and non_git_source',
      },
      {
        'object_name': 'Linux_x64/clang-linux-runtime-library-llvmorg-24-init-14453-ge063be79-1.tar.xz',
        'sha256sum': '38338f434311fcd12a070757c23830d6076529f4dcbefebc9d094804dc4eca63',
        'size_bytes': 3145380,
        'generation': 1792086698521278,
        'condition': 'checkout_linux and not host_os == "linux"',
      },
      {
        'object_name': 'Linux_x64/clang-llvmorg-24-init-14453-ge063be79-1.tar.xz',
        'sha256sum': '0b76afc1a29b0583f7723e966b9b65a22b15d8fdcd38e7c567ae32343c68944d',
        'size_bytes': 58394648,
        'generation': 1792086691304470,
        'condition': 'host_os == "linux" and non_git_source',
      },
      {
        'object_name': 'Linux_x64/clang-tidy-llvmorg-24-init-14453-ge063be79-1.tar.xz',
        'sha256sum': 'cf2a7610086b45fb878f606709babd3cbd8e26ea97a087ea47929e4ee871b562',
        'size_bytes': 15213064,
        'generation': 1792086691338093,
        'condition': 'host_os == "linux" and checkout_clang_tidy and non_git_source',
      },
      {
        'object_name': 'Linux_x64/clangd-llvmorg-24-init-14453-ge063be79-1.tar.xz',
        'sha256sum': '5ef64ed6fdeab8dfd91f11db07c601ce5107fc9e61e9d7a18dcee97af42f03c6',
        'size_bytes': 15595776,
        'generation': 1792086691391942,
        'condition': 'host_os == "linux" and checkout_clangd and non_git_source',
      },
      {
        'object_name': 'Linux_x64/llvm-code-coverage-llvmorg-24-init-14453-ge063be79-1.tar.xz',
        'sha256sum': '756e08684179166571dd155471e0a9c4525223d30b45d776b3d75a4d53b820d3',
        'size_bytes': 2432768,
        'generation': 1792086691721951,
        'condition': 'host_os == "linux" and checkout_clang_coverage_tools and non_git_source',
      },
      {
        'object_name': 'Linux_x64/llvmobjdump-llvmorg-24-init-14453-ge063be79-1.tar.xz',
        'sha256sum': '2282c8a4bdc6e12242989525039a36e7eeb4335b22d39d3b5d624c38083881c5',
        'size_bytes': 6011380,
        'generation': 1792086691439732,
        'condition': '((checkout_linux or checkout_mac or checkout_ios or checkout_android) and host_os == "linux") and non_git_source',
      },
      {
        'object_name': 'Mac/clang-llvmorg-24-init-14453-ge063be79-1.tar.xz',
        'sha256sum': 'ae29ec360eda6e80e96f8638d368ecdeea93d19c18c527c525dc0932bd5243a4',
        'size_bytes': 54834700,
        'generation': 1792086700293656,
        'condition': 'host_os == "mac" and host_cpu == "x64"',
      },
      {
        'object_name': 'Mac/clang-mac-runtime-library-llvmorg-24-init-14453-ge063be79-1.tar.xz',
        'sha256sum': 'c18444eef64cf27a8913ce73ffa8fd92e6f41306a02bbc82d805aa0c521863e0',
        'size_bytes': 997164,
        'generation': 1792086707123823,
        'condition': '(checkout_mac or checkout_ios) and not host_os == "mac"',
      },
      {
        'object_name': 'Mac/clang-tidy-llvmorg-24-init-14453-ge063be79-1.tar.xz',
        'sha256sum': '3997f34e72dc28eb30af1b8af903ceb6b0f516db7821996297bf296793addc72',
        'size_bytes': 14437076,
        'generation': 1792086700400707,
        'condition': 'host_os == "mac" and host_cpu == "x64" and checkout_clang_tidy',
      },
      {
        'object_name': 'Mac/clangd-llvmorg-24-init-14453-ge063be79-1.tar.xz',
        'sha256sum': '0fc9c351ad3bc6c357cc53a57dad6f8ebf45ba9e38150012cb457812f2a1314f',
        'size_bytes': 15359244,
        'generation': 1792086700318835,
        'condition': 'host_os == "mac" and host_cpu == "x64" and checkout_clangd',
      },
      {
        'object_name': 'Mac/llvm-code-coverage-llvmorg-24-init-14453-ge063be79-1.tar.xz',
        'sha256sum': 'b8b2f099cb029ff36cce33c9983984f547d7da0d9c34c1b427bb1080a31b8047',
        'size_bytes': 2307268,
        'generation': 1792086700464636,
        'condition': 'host_os == "mac" and host_cpu == "x64" and checkout_clang_coverage_tools',
      },
      {
        'object_name': 'Mac/llvmobjdump-llvmorg-24-init-14453-ge063be79-1.tar.xz',
        'sha256sum': 'eae7800773f3955fd1dcc334d8779b9a232ca6ebd3a79a55be028bd8797b1197',
        'size_bytes': 5586688,
        'generation': 1792086700364844,
        'condition': 'host_os == "mac" and host_cpu == "x64"',
      },
      {
        'object_name': 'Mac_arm64/clang-llvmorg-24-init-14453-ge063be79-1.tar.xz',
        'sha256sum': '0a8d887534b3153c7f6860265dc7e075d420174521c60ccd52c2c243949ef468',
        'size_bytes': 44917408,
        'generation': 1792086708845653,
        'condition': 'host_os == "mac" and host_cpu == "arm64"',
      },
      {
        'object_name': 'Mac_arm64/clang-tidy-llvmorg-24-init-14453-ge063be79-1.tar.xz',
        'sha256sum': '1aee3a0078bbe3b840125f521d3e6d985de61531ee6686a0bef80abc6e6d6a78',
        'size_bytes': 12291060,
        'generation': 1792086708806950,
        'condition': 'host_os == "mac" and host_cpu == "arm64" and checkout_clang_tidy',
      },
      {
        'object_name': 'Mac_arm64/clangd-llvmorg-24-init-14453-ge063be79-1.tar.xz',
        'sha256sum': '799e034e1fdff7eb7d899329ab20bbeb96642234844c9e070797059bd6ce5ee9',
        'size_bytes': 12292508,
        'generation': 1792086709015693,
        'condition': 'host_os == "mac" and host_cpu == "arm64" and checkout_clangd',
      },
      {
        'object_name': 'Mac_arm64/llvm-code-coverage-llvmorg-24-init-14453-ge063be79-1.tar.xz',
        'sha256sum': 'a77bacfb3e6c08af7d51981d88fb4aa07e4fb797f37937071dd8657b7c2dea8e',
        'size_bytes': 1897140,
        'generation': 1792086709049271,
        'condition': 'host_os == "mac" and host_cpu == "arm64" and checkout_clang_coverage_tools',
      },
      {
        'object_name': 'Mac_arm64/llvmobjdump-llvmorg-24-init-14453-ge063be79-1.tar.xz',
        'sha256sum': '8d15119ffd7e9d52e86676bfdcfd8159506f0ba6c4b292ae4112cbf15e629d7d',
        'size_bytes': 5269524,
        'generation': 1792086709009144,
        'condition': 'host_os == "mac" and host_cpu == "arm64"',
      },
      {
        'object_name': 'Win/clang-llvmorg-24-init-14453-ge063be79-1.tar.xz',
        'sha256sum': '08abce6e1d01c2fff9b25a09e36d9976296f27842196f96411b8220364481818',
        'size_bytes': 52783904,
        'generation': 1792086718487613,
        'condition': 'host_os == "win"',
      },
      {
        'object_name': 'Win/clang-tidy-llvmorg-24-init-14453-ge063be79-1.tar.xz',
        'sha256sum': 'ee25e107e6da91254780b0e9b6fb34198dc03636330c5564a7786e6450916d45',
        'size_bytes': 15436080,
        'generation': 1792086718412245,
        'condition': 'host_os == "win" and checkout_clang_tidy',
      },
      {
        'object_name': 'Win/clang-win-runtime-library-llvmorg-24-init-14453-ge063be79-1.tar.xz',
        'sha256sum': 'a156f6fa1f149e45faa683f2b2d1cc010df1e29912b3cb0a0dd3e3d9186c513b',
        'size_bytes': 2707764,
        'generation': 1792086725161419,
        'condition': 'checkout_win and not host_os == "win"',
      },
      {
        'object_name': 'Win/clangd-llvmorg-24-init-14453-ge063be79-1.tar.xz',
        'sha256sum': '0db39ead1a1f49137570fd5e99ab7520262c2e6444900b551862ae2cccdf6bb8',
        'size_bytes': 15910376,
        'generation': 1792086718402867,
       'condition': 'host_os == "win" and checkout_clangd',
      },
      {
        'object_name': 'Win/llvm-code-coverage-llvmorg-24-init-14453-ge063be79-1.tar.xz',
        'sha256sum': '3cfd8304148bd6bdd86a99f4e5f01ea2b4fbcbf4f35e29a2ca3cb235cb88c393',
        'size_bytes': 2604364,
        'generation': 1792086718581883,
        'condition': 'host_os == "win" and checkout_clang_coverage_tools',
      },
      {
        'object_name': 'Win/llvmobjdump-llvmorg-24-init-14453-ge063be79-1.tar.xz',
        'sha256sum': '3b58242027a9e86114d05444e9e70a9ed497a6a9e9c14d1b86c355e140503a53',
        'size_bytes': 6123208,
        'generation': 1792086718463583,
        'condition': '(checkout_linux or checkout_mac or checkout_android) and host_os == "win"',
      },
    ]
  },
  # Update prebuilt Rust toolchain.
  'src/third_party/rust-toolchain': {
    'dep_type': 'gcs',
    'bucket': 'chromium-browser-clang',
    'condition': 'not rust_force_head_revision',
    'objects': [
      {
        'object_name': 'Linux_x64/rust-toolchain-8a9dd7abdf013ec10cfeaaa3685feba18ef1c90c-1-llvmorg-24-init-14453-ge063be79.tar.xz',
        'sha256sum': '3f62ea86b027923b1fce17ac2ef002d1788b608245003440b46558a8e735b83d',
        'size_bytes': 287328068,
        'generation': 1792086676135089,
        'condition': 'host_os == "linux" and non_git_source',
      },
      {
        'object_name': 'Mac/rust-toolchain-8a9dd7abdf013ec10cfeaaa3685feba18ef1c90c-1-llvmorg-24-init-14453-ge063be79.tar.xz',
        'sha256sum': '54d1db81fbbc7a97387b3cc9192a3497eea096c258ccae8f94fa46b5dac463c8',
        'size_bytes': 274448924,
        'generation': 1792086679706753,
        'condition': 'host_os == "mac" and host_cpu == "x64"',
      },
      {
        'object_name': 'Mac_arm64/rust-toolchain-8a9dd7abdf013ec10cfeaaa3685feba18ef1c90c-1-llvmorg-24-init-14453-ge063be79.tar.xz',
        'sha256sum': '40b95cd2e8ccfa77724058832cc9809bb3aa0c5cf990d8c7b10cf854315ccce5',
        'size_bytes': 258029728,
        'generation': 1792086684183512,
        'condition': 'host_os == "mac" and host_cpu == "arm64"',
      },
      {
        'object_name': 'Win/rust-toolchain-8a9dd7abdf013ec10cfeaaa3685feba18ef1c90c-1-llvmorg-24-init-14453-ge063be79.tar.xz',
        'sha256sum': '8d501e922bc40c4e3bb255e8e31783c027054bff27b17970150c399c80eaf647',
        'size_bytes': 426164384,
        'generation': 1792086687600249,
        'condition': 'host_os == "win"',
      },
    ],
  },

  # Update prebuilt libclang.
  'src/third_party/llvm-libclang': {
    'dep_type': 'gcs',
    'bucket': 'chromium-browser-clang',
    'objects': [
      {
        'object_name': 'Linux_x64/rust-libclang-8a9dd7abdf013ec10cfeaaa3685feba18ef1c90c-1-llvmorg-24-init-14453-ge063be79.tar.xz',
        'sha256sum': '505dfa0b8c6c7abb332437c83e1ba169595e53aa8eef8f0aa1ea7adce4576835',
        'size_bytes': 23017532,
        'generation': 1792086677820729,
        'condition': 'host_os == "linux" and non_git_source',
      },
      {
        'object_name': 'Mac/rust-libclang-8a9dd7abdf013ec10cfeaaa3685feba18ef1c90c-1-llvmorg-24-init-14453-ge063be79.tar.xz',
        'sha256sum': 'add54dcb54194a627e6c0b7171aa06114064c231410e33f616a83948e4626882',
        'size_bytes': 23163236,
        'generation': 1792086682384306,
        'condition': 'host_os == "mac" and host_cpu == "x64"',
      },
      {
        'object_name': 'Mac_arm64/rust-libclang-8a9dd7abdf013ec10cfeaaa3685feba18ef1c90c-1-llvmorg-24-init-14453-ge063be79.tar.xz',
        'sha256sum': '3abad0aab3e8f49e2bb596a85cc88c38ee872bfe1dd55c6dc76fcfab9c6f0ba7',
        'size_bytes': 20838968,
        'generation': 1792086685890604,
        'condition': 'host_os == "mac" and host_cpu == "arm64"',
      },
      {
        'object_name': 'Win/rust-libclang-8a9dd7abdf013ec10cfeaaa3685feba18ef1c90c-1-llvmorg-24-init-14453-ge063be79.tar.xz',
        'sha256sum': 'b0716ed45f80b964b058414a80eb417272cb2e1df684c86efa31edf6ede31211',
        'size_bytes': 21797328,
        'generation': 1792086689442006,
        'condition': 'host_os == "win"',
      },
    ],
  },

  'src/third_party/clang-format/script':
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
    'condition': 'host_os == "linux" and non_git_source',
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
    'condition': 'download_reclient and non_git_source',
    'dep_type': 'cipd',
  },

  'src/content/test/data/gpu/meet_effects_videos': {
    'packages': [
      {
        'package': 'chromium/testing/meet-effects-videos',
        'version': 'I5ME8r-BP_A2lRn-c5E-9POZkrJwGS8BNnRdREhYbs8C',
      }
    ],
    'condition': 'checkout_gpu_meet_effects and non_git_source',
    'dep_type': 'cipd',
  },
  'src/content/test/data/gpu/meet_effects': {
    'dep_type': 'gcs',
    'bucket': 'meet-bundles',
    'condition': 'checkout_gpu_meet_effects and non_git_source',
    'objects': [
      {
        'object_name': 'meet-gpu-tests/1007017915.tar.gz',
        'sha256sum': 'e4f2196599241df7ea647ce853ec55c6558c3e9f1531f65200290608a32f5cd2',
        'size_bytes': 92206178,
        'generation': 1793004435706713,
      },
    ],
  },

  # We don't know target_cpu at deps time. At least until there's a universal
  # binary of httpd-php, pull both intel and arm versions in DEPS and then pick
  # the right one at runtime.
  'src/third_party/apache-mac': {
    'packages': [
      {
        'package': 'infra/3pp/tools/httpd-php/mac-amd64',
        'version': 'version:3@httpd2.4.38.php8.4.27.chromium.4.chromium.3',
      },
    ],
    'dep_type': 'cipd',
    'condition': 'checkout_mac or checkout_ios',
  },
  'src/third_party/apache-mac-arm64': {
    'packages': [
      {
        'package': 'infra/3pp/tools/httpd-php/mac-arm64',
        'version': 'version:3@httpd2.4.38.php8.4.27.chromium.4.chromium.3',
      },
    ],
    'dep_type': 'cipd',
    'condition': 'checkout_mac or checkout_ios',
  },

  'src/third_party/apache-linux': {
    'packages': [
      {
        'package': 'infra/3pp/tools/httpd-php/linux-amd64',
        'version': 'version:3@httpd2.4.38.php8.4.27.chromium.4.chromium.3',
      },
    ],
    'dep_type': 'cipd',
    'condition': '(host_os == "linux") and non_git_source',
  },

  'src/third_party/apache-win32': {
    'packages': [
      {
        'package': 'infra/3pp/tools/httpd-php/windows-amd64',
        'version': 'version:3@httpd2.4.55-php8.4.27.chromium.8',
      },
    ],
    'dep_type': 'cipd',
    'condition': 'checkout_win',
  },

  'src/third_party/apache-windows-arm64': {
    'packages': [
        {
            'package': 'infra/3pp/tools/httpd-php/windows-arm64',
            'version': 'version:3@httpd2.4.55-php8.4.27.chromium.7',
        }
    ],
    'dep_type': 'cipd',
    'condition': 'checkout_win'
  },

  'src/third_party/aosp_dalvik/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/aosp_dalvik/linux-amd64',
              'version': 'version:2@13.0.0_r24.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/android_webview/tools/cts_archive/cipd': {
      'packages': [
          {
              'package': 'chromium/android_webview/tools/cts_archive',
              'version': 'oW6-jyOPGwPJeLlaldYwrxZoYqzXpjO1OZUdyF3Qq7sC',
          },
      ],
      # TODO: crbug.com/487671154 - add back non_git_source condtion once we can
      # find a way to make individual files in the package smaller.
      'condition': 'checkout_android',
      'dep_type': 'cipd',
  },

  'src/chrome/android/orderfiles/arm': {
      'packages': [
          {
              'package': 'chromium/chrome/android/orderfiles/arm',
              'version_file': 'chrome/build/android-arm.orderfile.txt',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/chrome/android/orderfiles/arm64': {
      'packages': [
          {
              'package': 'chromium/chrome/android/orderfiles/arm64',
              'version_file': 'chrome/build/android-arm64.orderfile.txt',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/android_webview/tools/orderfiles/arm': {
      'packages': [
          {
              'package': 'chromium/android_webview/tools/orderfiles/arm',
              'version_file': 'android_webview/tools/android-webview-arm.orderfile.txt',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/android_webview/tools/orderfiles/arm64': {
      'packages': [
          {
              'package': 'chromium/android_webview/tools/orderfiles/arm64_webview_pgo',
              'version_file': 'android_webview/tools/android-webview-arm64.orderfile.txt',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/chrome/browser/resources/preinstalled_web_apps/internal': {
    'url': Var('chrome_git') + '/chrome/components/default_apps.git' + '@' + 'fbc0f36186884f936537d5f74758550dd772b980',
    'condition': 'checkout_src_internal',
  },

  'src/third_party/compiler-rt/src':
    Var('chromium_git') +
    '/external/github.com/llvm/llvm-project/compiler-rt.git' + '@' +
    Var('compiler_rt_revision'),
  'src/third_party/libc++/src':
    Var('chromium_git') +
    '/external/github.com/llvm/llvm-project/libcxx.git' + '@' +
    Var('libcxx_revision'),
  'src/third_party/libc++abi/src':
    Var('chromium_git') +
    '/external/github.com/llvm/llvm-project/libcxxabi.git' + '@' +
    Var('libcxxabi_revision'),
  'src/third_party/libunwind/src':
    Var('chromium_git') +
    '/external/github.com/llvm/llvm-project/libunwind.git' + '@' +
    Var('libunwind_revision'),
  'src/third_party/llvm-libc/src':
    Var('chromium_git') +
    '/external/github.com/llvm/llvm-project/libc.git' + '@' +
    Var('llvm_libc_revision'),

  'src/third_party/updater/chrome_linux64/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_linux and non_git_source',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chrome_linux64',
          'version': 'version:2@157.0.8089.0',
        },
      ],
  },

  'src/third_party/updater/chrome_mac_universal/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chrome_mac_universal',
          'version': 'version:2@157.0.8089.0',
        },
      ],
  },

  'src/third_party/updater/chrome_mac_universal_prod/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chrome_mac_universal_prod',
          'version': 'version:2@157.0.8089.0',
        },
      ],
  },

  'src/third_party/updater/chrome_win_arm64/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chrome_win_arm64',
          'version': 'version:2@157.0.8089.0',
        },
      ],
  },

  'src/third_party/updater/chrome_win_x86/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chrome_win_x86',
          'version': 'version:2@157.0.8089.0',
        },
      ],
  },

  'src/third_party/updater/chrome_win_x86_64/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chrome_win_x86_64',
          'version': 'version:2@157.0.8089.0',
        },
      ],
  },

  # Branded builds for testing previous interface without `install_id` support.
  'src/third_party/updater/chrome_linux64_sans_iid/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_linux and non_git_source',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chrome_linux64',
          'version': 'ytJ0UbU9gMLUMLRQlmqQpGpOy1dYswI3rOJ0ILnIFbUC',
        },
      ],
  },

  'src/third_party/updater/chrome_mac_universal_sans_iid/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chrome_mac_universal',
          'version': 'gzutuY-G7u8n5746jgmishm8uWjUR070TXdFc23Ea7YC',
        },
      ],
  },

  'src/third_party/updater/chrome_mac_universal_prod_sans_iid/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chrome_mac_universal_prod',
          'version': 'IrAigaqukp1GbaksroZcR3Jo0oOYKg9kzatjzNNbXKQC',
        },
      ],
  },

  'src/third_party/updater/chrome_win_arm64_sans_iid/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chrome_win_arm64',
          'version': 'OoGMyCUHah7D_AOVm4RpVoPOdkGqjFHuSMcoKTZ0rlsC',
        },
      ],
  },

  'src/third_party/updater/chrome_win_x86_sans_iid/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chrome_win_x86',
          'version': 'gqhcrVQFtXAKUu7yEISTlbrBpPaBUnQZ6FdOo5E4HLQC',
        },
      ],
  },

  'src/third_party/updater/chrome_win_x86_64_sans_iid/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chrome_win_x86_64',
          'version': 'koo1i4cAVdOK2-re2VGbx-crCcbTz9h0qumUk1hyEt0C',
        },
      ],
  },

  'src/third_party/updater/chromium_linux64/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_linux and non_git_source',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chromium_linux64',
          'version': 'version:2@1722005',
        },
      ],
  },

  'src/third_party/updater/chromium_mac_amd64/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chromium_mac_amd64',
          'version': 'version:2@1722014',
        },
      ],
  },

  'src/third_party/updater/chromium_mac_arm64/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chromium_mac_arm64',
          'version': 'version:2@1722002',
        },
      ],
  },

  'src/third_party/updater/chromium_win_arm64/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chromium_win_arm64',
          'version': 'version:2@1721031',
        },
      ],
  },

  'src/third_party/updater/chromium_win_x86/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chromium_win_x86',
          'version': 'version:2@1721040',
        },
      ],
  },

  'src/third_party/updater/chromium_win_x86_64/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chromium_win_x86_64',
          'version': 'version:2@1721041',
        },
      ],
  },

  # Chromium builds for testing previous interface without `install_id` support.
  'src/third_party/updater/chromium_linux64_sans_iid/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_linux and non_git_source',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chromium_linux64',
          'version': 'OLbfeePAbqPnFqcPmkFtR-GK8dN3T3NeH2AChZwBHjgC',
        },
      ],
  },

  'src/third_party/updater/chromium_mac_amd64_sans_iid/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chromium_mac_amd64',
          'version': 'zVv93X5XSClxTR1YejkQuBdSpye7JfPS_h6GcH1N4i4C',
        },
      ],
  },

  'src/third_party/updater/chromium_mac_arm64_sans_iid/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chromium_mac_arm64',
          'version': 'Va20qxSst3lq4WfEZlWiwzXCpSo5XbhhuqJXyqzvhF8C',
        },
      ],
  },

  'src/third_party/updater/chromium_win_arm64_sans_iid/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chromium_win_arm64',
          'version': 'ksRzLnqewvz7P-YMX2e8mxZuDI1hgPtLNCuAXIisXhoC',
        },
      ],
  },

  'src/third_party/updater/chromium_win_x86_sans_iid/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chromium_win_x86',
          'version': 'tcFWGGE3wmwKDhT_s-9mzPiQVYxMOI1U9sRAhm1_5QcC',
        },
      ],
  },

  'src/third_party/updater/chromium_win_x86_64_sans_iid/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chromium_win_x86_64',
          'version': 'iXamEEmbStVBV_c-vEG4Xtrda8tTqSRCZ-tmE7VRHb0C',
        },
      ],
  },

  'src/third_party/webview2': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/webview2',
          'version': 'version:2@1.0.3856.49.r1',
        },
      ],
  },

  'src/third_party/wtl': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/wtl',
          'version': 'version:2@10.1.0',
        },
      ],
  },

  'src/third_party/enterprise_companion/chromium_linux64/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_linux and non_git_source',
      'packages': [
        {
          'package': 'chromium/third_party/enterprise_companion/chromium_linux64',
          'version': '_-I1ZzAleWRuBzPGt7zAEuxqLoFLIA6IX2A4Fql0ocYC',
        },
      ],
  },

  'src/third_party/enterprise_companion/chromium_mac_amd64/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
      'packages': [
        {
          'package': 'chromium/third_party/enterprise_companion/chromium_mac_amd64',
          'version': 'slXOxXpT6bgoy2LiDKEicN7Arw4JPaKMmEfHBuTpyncC',
        },
      ],
  },

  'src/third_party/enterprise_companion/chromium_mac_arm64/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
      'packages': [
        {
          'package': 'chromium/third_party/enterprise_companion/chromium_mac_arm64',
          'version': 'w7jGpX4PHzGuwa8KSaBGmL5QSBNA-SuL57tOC6hHiacC',
        },
      ],
  },

  'src/third_party/enterprise_companion/chromium_win_x86/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/enterprise_companion/chromium_win_x86',
          'version': 'Z6Q1-HfRO99AoCldCcjZxit9sjpjTf0hdXpNPP-ec_0C',
        },
      ],
  },

  'src/third_party/enterprise_companion/chromium_win_x86_64/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/enterprise_companion/chromium_win_x86_64',
          'version': 'ECRhc9berP0mj1_eYZ_H5EvTgbyD9xXXCD_ZSkALbngC',
        },
      ],
  },

  'src/chrome/test/data/autofill/captured_sites/artifacts': {
    'url': Var('chrome_git') + '/chrome/test/captured_sites/autofill.git' + '@' + '2a9fb59b5ff65eb3ce98946af93cf96b838bf04b',
    'condition': 'checkout_chromium_autofill_test_dependencies',
  },

  'src/chrome/test/data/password/captured_sites/artifacts': {
    'url': Var('chrome_git') + '/chrome/test/captured_sites/password.git' + '@' + 'a252d7722f577b62c6244648b4031baffa876f17',
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

  'src/components/variations/test_data/cipd': {
    'packages': [
      {
        'package': 'chromium/chrome/test/data/variations/cipd',
        'version': 'NBdqmq9I_YhOVLwVfWx13sT8V0zNA1DZB9VvDN6bS64C',
      },
    ],
    'condition': 'non_git_source',
    'dep_type': 'cipd',
  },

  'src/chrome/test/data/xr/webvr_info':
    Var('chromium_git') + '/external/github.com/toji/webvr.info.git' + '@' + 'c58ae99b9ff9e2aa4c524633519570bf33536248',

  'src/clank': {
    'url': Var('chrome_git') + '/clank/internal/apps.git' + '@' +
    '9f946f78b0a3b1444005340220c9b6f750f61cd7',
    'condition': 'checkout_android and checkout_src_internal',
  },

  'src/docs/website': {
    'url': Var('chromium_git') + '/website.git' + '@' + '47ee94275c3b0ad9a38c6724bd11eefba4b616ba',
  },

  'src/ios/third_party/earl_grey2/src': {
      'url': Var('chromium_git') + '/external/github.com/google/EarlGrey.git' + '@' + 'b2e2d7f42e75297c1414d41c3cacbc556a78f51c',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/edo/src': {
      'url': Var('chromium_git') + '/external/github.com/google/eDistantObject.git' + '@' + '2bbd17fe3acacc199efd2c758b1b8044acd9ee05',
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

  'src/ios/third_party/webkit/src': {
      'url': Var('chromium_git') + '/external/github.com/WebKit/webkit.git' +
             '@' + Var('ios_webkit_revision'),
      'condition': 'checkout_ios and checkout_ios_webkit'
  },

  'src/media/cdm/api':
    Var('chromium_git') + '/chromium/cdm.git' + '@' + 'd6c4e1ea4c8fc3dcd98ac3ab5a981f63067223a4',

  'src/net/third_party/quiche/src':
    Var('quiche_git') + '/quiche.git' + '@' +  Var('quiche_revision'),

  'src/testing/libfuzzer/fuzzers/wasm_corpus':
    Var('chromium_git') + '/v8/fuzzer_wasm_corpus.git' + '@' +  '1df5e50a45db9518a56ebb42cb020a94a090258b',

  'src/tools/copybara': {
      'packages' : [
          {
              'package': 'infra/3pp/tools/copybara',
              'version': '2qCa8KkjfMwFChKfX1jLq21xIbsVexQa1jIzjibwg7EC',
          },
      ],
      'condition': 'host_os == "linux" and checkout_copybara',
      'dep_type': 'cipd',
  },

  'src/tools/luci-go': {
      'packages': [
        {
          'package': 'infra/tools/luci/cas/${{platform}}',
          'version': Var('luci_go'),
        },
        # TODO(crbug.com/382506663): Remove after investigation/deprecation
        {
          'package': 'infra/tools/luci/isolate/${{platform}}',
          'version': Var('luci_go'),
        },
        {
          'package': 'infra/tools/luci/swarming/${{platform}}',
          'version': Var('luci_go'),
        },
      ],
      'condition': 'non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_build_tools/protoc/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/android_build_tools/protoc/${{platform}}',
              'version': 'version:2@v35.1',
          },
      ],
      'condition': 'checkout_android and non_git_source and '
                   '((host_os == "linux" and host_cpu == "x64") or '
                   '(host_os == "mac" and host_cpu == "arm64"))',
      'dep_type': 'cipd',
  },

  'src/third_party/android_toolchain/ndk': {
      'packages': [
            {
                'package': 'chromium/third_party/android_toolchain/android_toolchain/${{platform}}',
                'version': 'version:' + Var('android_ndk_version'),
            },
      ],
      'condition': 'checkout_android and non_git_source and '
                   '((host_os == "linux" and host_cpu == "x64") or '
                   '(host_os == "mac" and host_cpu == "arm64"))',
      'dep_type': 'cipd',
  },

  'src/third_party/androidx/cipd': {
    'packages': [
      {
          'package': 'chromium/third_party/androidx',
          'version': 'rRFJv-wIHBcmJDI0MSLZWuPeFNdlHF1i5nIwEn_cUXoC',
      },
    ],
    'condition': 'checkout_android and non_git_source',
    'dep_type': 'cipd',
  },

  'src/third_party/androidx/third_party/ink/src': {
    'url': Var('chromium_git') + '/external/github.com/google/ink.git@4413fe7bd41f09f37d7c8fe87df59b6e0f54300a', # 1.1.0-alpha03
    'condition': 'checkout_android and non_git_source',
  },

  'src/third_party/androidx_javascriptengine/src': {
      'url': Var('chromium_git') + '/aosp/platform/frameworks/support/javascriptengine/javascriptengine/src.git' + '@' + '7539442db1dc790c64d0d9bade922329292d834b',
      'condition': 'checkout_android',
  },

  'src/third_party/android_system_sdk/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/android_system_sdk/public',
              'version': 'QAcOcO9AlzRvhHOaFCxeQAcdO3qiFPwxUHXUIlSbw3kC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_build_tools/aapt2/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/android_build_tools/aapt2/${{platform}}',
              'version': Var('aapt2_version'),
          },
      ],
      'condition': 'checkout_android and non_git_source and '
                   '((host_os == "linux" and host_cpu == "x64") or '
                   '(host_os == "mac" and host_cpu == "arm64"))',
      'dep_type': 'cipd',
  },

  # Resources are compiled on Linux workers under remote execution, so
  # non-Linux hosts also need the Linux aapt2.
  'src/third_party/android_build_tools/aapt2/linux-amd64': {
      'packages': [
          {
              'package': 'chromium/third_party/android_build_tools/aapt2/linux-amd64',
              'version': Var('aapt2_version'),
          },
      ],
      'condition': 'checkout_android and non_git_source and host_os != "linux"',
      'dep_type': 'cipd',
  },

  'src/third_party/android_build_tools/bazel_tools/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/android_build_tools/bazel_tools/${{platform}}',
              'version': 'version:2@7.4.1',
          },
      ],
      'condition': 'checkout_android and non_git_source and '
                   '((host_os == "linux" and host_cpu == "x64") or '
                   '(host_os == "mac" and host_cpu == "arm64"))',
      'dep_type': 'cipd',
  },

  'src/third_party/android_build_tools/bundletool/cipd': {
      'packages': [
          {
               'package': 'chromium/third_party/android_build_tools/bundletool',
               'version': '7Vo6ZzIxIaC51ATTBlo_KUkgxJCmmGmXAijlVkXUTpAC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_build_tools/error_prone/cipd': {
      'packages': [
          {
               'package': 'chromium/third_party/android_build_tools/error_prone',
               'version': 'MyopHMvD7ppHVhTyCks9LuHSI7Ak0as21iMAE5sdznkC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_build_tools/error_prone_javac/cipd': {
      'packages': [
          {
               'package': 'chromium/third_party/android_build_tools/error_prone_javac',
               'version': '7EcHxlEXEaLRWEyHIAxf0ouPjkmN1Od6jkutuo0sfBIC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_build_tools/lint/cipd': {
      'packages': [
          {
               'package': 'chromium/third_party/android_build_tools/lint',
               'version': 'Zof5xPeiL6icsW3VUhp-vdD4Y4NzVol7qORgkuWtUIkC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_build_tools/manifest_merger/cipd': {
      'packages': [
          {
               'package': 'chromium/third_party/android_build_tools/manifest_merger',
               'version': 'AOTkL10zrs8MLBJB06RShLmwy8iaJBBwGyLG_TraiRsC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_build_tools/nullaway/cipd': {
      'packages': [
          {
               'package': 'chromium/third_party/android_build_tools/nullaway',
               'version': 'MDrb2sXrlSVpSh9bFN8HuMsydpJEeRcNUSbU0wj5xBsC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_build_tools/signflinger/cipd': {
      'packages': [
          {
               'package': 'chromium/third_party/android_build_tools/signflinger',
               'version': 'sR382zqUwar3urYhP_Zf4n9TGzyuJQRGJznAtGCSGOUC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_sdk/public': {
      'packages': [
          {
              'package': 'chromium/third_party/android_sdk/public/build-tools/37.0.0/${{os}}',
              'version': Var('android_sdk_build-tools_version'),
          },
          {
              'package': 'chromium/third_party/android_sdk/public/emulator',
              'version': Var('android_sdk_emulator_version'),
          },
          {
              'package': 'chromium/third_party/android_sdk/public/platform-tools',
              'version': Var('android_sdk_platform-tools_version'),
          },
          {
              'package': 'chromium/third_party/android_sdk/public/platforms/android-37.2',
              'version': Var('android_sdk_platforms_version'),
          },
          {
              'package': 'chromium/third_party/android_sdk/public/cmdline-tools/${{os}}',
              'version': 'version:23.0.0',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/angle':
    Var('chromium_git') + '/angle/angle.git' + '@' +  Var('angle_revision'),

  'src/third_party/anonymous_tokens/src':
    Var('chromium_git') + '/external/github.com/google/anonymous-tokens.git' + '@' + '1ae8df5e40bab28959cc182c2c0d88923059e278',


    'src/third_party/blink/renderer/core/css/perftest_data': {
      'dep_type': 'gcs',
      'condition': 'non_git_source',
      'bucket': 'chromium-style-perftest',
      'objects': [
          {
              'object_name': 'e9ce994346c62f8c9fd6d0cecb2b2b0b93b4c2d8',
              'sha256sum': '519019df16c628c6c0893df18928faeaa3150a9d8f26a787a16ce7c6b2cec2ad',
              'size_bytes': 601672,
              'generation': 1664794185950162,
              'output_file': 'ecommerce.json',
          },
          {
              'object_name': '756068da5e551516b23b0ba133e55c144f623d38',
              'sha256sum': '84ef87a8163335a95111d9709306596f96742539da0b34fbe7397f799946a168',
              'size_bytes': 2156935,
              'generation': 1664794188995509,
              'output_file': 'encyclopedia.json',
          },
          {
              'object_name': '314e4e0d5e89ea9e9e9a234c617b4413adf48aa9',
              'sha256sum': 'a721ada40011a286631baae6d76878f2023ff000151792228c83b1958ea8a197',
              'size_bytes': 608840,
              'generation': 1664794191929032,
              'output_file': 'extension.json',
          },
          {
              'object_name': '3a19b42a7c46257b716d55d6733f070c87180b1e',
              'sha256sum': 'f203ff9e8c8a6a3b714f0a26db38cc940544a907435c62c79b21f4bd3f8bee8e',
              'size_bytes': 1750837,
              'generation': 1664794194891567,
              'output_file': 'news.json',
          },
          {
              'object_name': 'fdc43ee18cbd65487249441849f58aa13484aaef',
              'sha256sum': '0e92de92f49abc9a521f7175106c80744196f8cefc0263bc0f4a6b4f724a7d10',
              'size_bytes': 1310798,
              'generation': 1664794197855470,
              'output_file': 'search.json',
          },
          {
              'object_name': '7fc9338af75b7d9d185c91ddf262a356def5623d',
              'sha256sum': '34e92acae8aade2a186abe79ed1f379c266f04f72f1eb54bd3a912e889bc5cc0',
              'size_bytes': 2280846,
              'generation': 1664794200867034,
              'output_file': 'social1.json',
          },
          {
              'object_name': 'c2d7e9ce67522dad138c7feb0a6911b828bfb130',
              'sha256sum': '95c6b148577b891310c024b2daa5d68faf644a37707ac0cb21501eefe8a399a3',
              'size_bytes': 411708,
              'generation': 1664794203829582,
              'output_file': 'social2.json',
          },
          {
              'object_name': '031d5599c8a21118754e30dbea141be66104f556',
              'sha256sum': '8e7b765d72bb8e7742f5bf955f4bf64d5469f61197dad8b632304095a52322d7',
              'size_bytes': 3203922,
              'generation': 1664794206824773,
              'output_file': 'sports.json',
          },
          {
              'object_name': '8aac3db2a8c9e44babec81e539a3d60aeab4985c',
              'sha256sum': '6aeb0036dfafaf5e905abdb0ffe515a3952ffe35a7c59afb0fc8b233b27c6ce4',
              'size_bytes': 5902660,
              'generation': 1664794209886788,
              'output_file': 'video.json',
          },
      ],
  },

  'src/third_party/readability/src':
    Var('chromium_git') + '/external/github.com/mozilla/readability.git' + '@' + 'd7949dc47dd9ed9ee1d3b34ffdcf3bce28cde435',

  'src/third_party/content_analysis_sdk/src':
    Var('chromium_git') + '/external/github.com/chromium/content_analysis_sdk.git' + '@' + '0a0e7ea516249e786d5a8884552bbd6e163d89cb',

  'src/third_party/dav1d/libdav1d':
    Var('chromium_git') + '/external/github.com/videolan/dav1d.git' + '@' + '9433da2daa61bf7029a52b84bafa5daf59cdcc9d',

  'src/third_party/dawn':
    Var('dawn_git') + '/dawn.git' + '@' +  Var('dawn_revision'),

  'src/third_party/disarm/src':
     Var('chromium_git') + '/external/github.com/aengelke/disarm.git' + '@' + '2d960dae4b4fb29dba05cfc48210fbed423f3d2c',

  'src/third_party/fadec/src':
     Var('chromium_git') + '/external/github.com/aengelke/fadec.git' + '@' + 'c9f78f532b9004de278489019ab2c6c28ae9746e',

  'src/third_party/highway/src':
    Var('chromium_git') + '/external/github.com/google/highway.git' + '@' + Var('highway_revision'),

  'src/third_party/apache-portable-runtime/src': {
      'url': Var('chromium_git') + '/external/apache-portable-runtime.git' + '@' + 'c3f11fcd86b42922834cae91103cf068246c6bb6',
      'condition': 'checkout_android',
  },

  'src/third_party/google_benchmark/src':
    Var('chromium_git') + '/external/github.com/google/benchmark.git' + '@' + 'a6b4a9f00b36c19af9997d68a1cf6e0b5b3d032c',

  'src/third_party/libpfm4/src':
    Var('chromium_git') + '/external/git.code.sf.net/p/perfmon2/libpfm4.git' + '@' + '186a658c3e1e49b0ee3da414a7db8ba971a00cb5',

  # Download test data for Maps telemetry_gpu_integration_test.
  'src/tools/perf/page_sets/maps_perf_test/dataset': {
      'dep_type': 'gcs',
      'condition': 'non_git_source',
      'bucket': 'chromium-telemetry',
      'objects': [
          {
              'object_name': 'e6bf26977c2fd80c18789d1f279d474096a7b0d1',
              'sha256sum': 'f5f7fe360ad2b9c3d9dda2612f17336c0541bac15b4e4992f2c167e059a190fa',
              'size_bytes': 3285237,
              'generation': 1513305740113238,
              'output_file': 'load_dataset',
          },
      ],
  },

  'src/third_party/boringssl/src':
    Var('boringssl_git') + '/boringssl.git' + '@' +  Var('boringssl_revision'),

  'src/third_party/breakpad/breakpad':
    Var('chromium_git') + '/breakpad/breakpad.git' + '@' + Var('breakpad_revision'),

  'src/third_party/cast_core/public/src':
    Var('chromium_git') + '/cast_core/public' + '@' + '1629985cfc0b065310738512a000b6e68716aa03',

  'src/third_party/catapult':
    Var('chromium_git') + '/catapult.git' + '@' + Var('catapult_revision'),

  'src/third_party/ced/src':
    Var('chromium_git') + '/external/github.com/google/compact_enc_det.git' + '@' + 'd127078cedef9c6642cbe592dacdd2292b50bb19',

  'src/third_party/checkstyle/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/checkstyle',
              'version': 'udEqw9ygTuIy4x8w97hBz9uRD-oh6wnXR4O0B5sXmPMC',
          },
      ],
      # Needed on Linux for use on chromium_presubmit.
      'condition': '(checkout_android or checkout_linux) and non_git_source',
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
      'url': Var('chromium_git') + '/chromiumos/chromite.git' + '@' + '0a07e67301004edc7a84050684dd9b4bffabe713',
      'condition': 'checkout_chromeos',
  },

  'src/third_party/cld_3/src':
    Var('chromium_git') + '/external/github.com/google/cld_3.git' + '@' + 'b48dc46512566f5a2d41118c8c1116c4f96dc661',

  'src/third_party/colorama/src':
    Var('chromium_git') + '/external/colorama.git' + '@' + '3de9f013df4b470069d03d250224062e8cf15c49',

  'src/third_party/cpu_features/src':
    Var('chromium_git') + '/external/github.com/google/cpu_features.git' + '@' + '81d13c49649f0714dd41fb56bb246398b6584085',

  'src/third_party/cpuinfo/src':
    Var('chromium_git') + '/external/github.com/pytorch/cpuinfo.git' + '@' + '66ee79c038d70dad9f08705b2c9b3e58f6d8f512',

  'src/third_party/crc32c/src':
    Var('chromium_git') + '/external/github.com/google/crc32c.git' + '@' + '2bbb3be42e20a0e6c0f7b39dc07dc863d9ffbc07',

  # For Linux and Chromium OS.
  'src/third_party/cros_system_api': {
      'url': Var('chromium_git') + '/chromiumos/platform2/system_api.git' + '@' + '003766e04c2518357425077e1e1d0ef8e12bf306',
      'condition': 'checkout_linux or checkout_chromeos',
  },

  'src/third_party/crossbench':
    Var('chromium_git') + '/crossbench.git' + '@' + Var('crossbench_revision'),

  'src/third_party/crossbench-web-tests':
    Var('chromium_git') + '/chromium/web-tests.git' + '@' + Var('crossbench_web_tests_revision'),

  'src/third_party/depot_tools':
    Var('chromium_git') + '/chromium/tools/depot_tools.git' + '@' + 'b86fc981b72ab966f8260f73b4eca600b3b12677',

  'src/third_party/devtools-frontend/src':
    Var('chromium_git') + '/devtools/devtools-frontend' + '@' + Var('devtools_frontend_revision'),

  'src/third_party/dom_distiller_js/dist':
    Var('chromium_git') + '/chromium/dom-distiller/dist.git' + '@' + '015cda307cf81e293889c0887cffff4f0aa090ce',

  'src/third_party/ds_store/src':
    Var('chromium_git') + '/external/github.com/dmgbuild/ds_store.git' + '@' + '3a85d45dbb9dc08a6d74770010e5053e0561e538',

  'src/third_party/dragonbox/src':
    Var('chromium_git') + '/external/github.com/jk-jeon/dragonbox.git' + '@' + 'beeeef91cf6fef89a4d4ba5e95d47ca64ccb3a44',

  'src/third_party/eigen3/src':
    Var('chromium_git') + '/external/gitlab.com/libeigen/eigen.git' + '@' + '7177d997a36148ac1cecb26e3cda8fa0ef35711e',

  'src/third_party/emoji-metadata/src': {
    'url': Var('chromium_git') + '/external/github.com/googlefonts/emoji-metadata' + '@' + '173b9b26e8fcbcbe64e1ecbf073a21a96b95c6b1',
    'condition': 'checkout_chromeos',
  },

  'src/third_party/espresso': {
      'packages': [
          {
              'package': 'chromium/third_party/espresso',
              'version': '5LoBT0j383h_4dXbnap7gnNQMtMjpbMJD1JaGIYNj-IC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/farmhash/src':
    Var('chromium_git') + '/external/github.com/google/farmhash.git' + '@' + '9d99331eb762e9ee22fd97d15a594525ad98310a',

  'src/third_party/fast_float/src':
    Var('chromium_git') + '/external/github.com/fastfloat/fast_float.git' + '@' + '42f254836ee416ffcf2f4e6576260003ac33d48c',

  'src/third_party/federated_compute/src':
    Var('chromium_git') + '/external/github.com/google-parfait/federated-compute.git' + '@' + 'a9d5262aa05a4b1ff3c23697d644023c5d913840',

  'src/third_party/ffmpeg':
    Var('chromium_git') + '/chromium/third_party/ffmpeg.git' + '@' + Var('ffmpeg_revision'),

  'src/third_party/flac':
    Var('chromium_git') + '/chromium/deps/flac.git' + '@' + 'e7108e2ed031547c3759217819a032065c820d73',

  'src/third_party/flatbuffers/src':
    Var('chromium_git') + '/external/github.com/google/flatbuffers.git' + '@' + 'a86afae9399bbe631d1ea0783f8816e780e236cc',

  'src/third_party/fontconfig/src': {
      'url': Var('chromium_git') + '/external/fontconfig.git' + '@' + 'd416ada7b20a1cdd5050b60a7a1bbdb3d9ddd2dc',
      'condition': 'checkout_linux',
  },

  'src/third_party/fp16/src':
    Var('chromium_git') + '/external/github.com/Maratyszcza/FP16.git' + '@' + '782eea126dc5c755827be751a099eb01826175cf',

  'src/third_party/gemmlowp/src':
    Var('chromium_git') + '/external/github.com/google/gemmlowp.git' + '@' + '16e8662c34917be0065110bfcd9cc27d30f52fdf',

  'src/third_party/glib/src': {
      'url': Var('chromium_git') + '/external/gitlab.gnome.org/GNOME/glib.git' + '@' + 'e05063ccc6c8f222465a1080927f4c14349f3de6',
      'condition': 'checkout_linux and checkout_mutter',
  },

  'src/third_party/gsettings-desktop-schemas/src': {
      'url': Var('chromium_git') + '/external/gitlab.gnome.org/GNOME/gsettings-desktop-schemas.git' + '@' + '1db238b6a349ea7fae6f1c0713afe04d1bb7ea5c',
      'condition': 'checkout_linux and checkout_mutter',
  },

  'src/third_party/gvdb/src': {
      'url': Var('chromium_git') + '/external/gitlab.gnome.org/GNOME/gvdb.git' + '@' + 'c6f2359cc1d00f16e0a0e2527fa0bc1882b8b5ab',
      'condition': 'checkout_linux and checkout_mutter',
  },

  'src/third_party/freetype/src':
    Var('chromium_git') + '/chromium/src/third_party/freetype2.git' + '@' + Var('freetype_revision'),

  'src/third_party/fxdiv/src':
    Var('chromium_git') + '/external/github.com/Maratyszcza/FXdiv.git' + '@' + '63058eff77e11aa15bf531df5dd34395ec3017c8',

  'src/third_party/harfbuzz/src':
    Var('chromium_git') + '/external/github.com/harfbuzz/harfbuzz.git' + '@' + Var('harfbuzz_revision'),

  'src/third_party/ink/src':
    Var('chromium_git') + '/external/github.com/google/ink.git' + '@' + Var('ink_revision'),

  'src/third_party/instrumented_libs': {
    'url': Var('chromium_git') + '/chromium/third_party/instrumented_libraries.git' + '@' + '83128b60913a82a39d01055312687a27bb959b59',
    'condition': 'checkout_instrumented_libraries',
  },

  'src/third_party/emoji-segmenter/src':
    Var('chromium_git') + '/external/github.com/google/emoji-segmenter.git' + '@' + Var('emoji_segmenter_revision'),

  'src/third_party/oak/src':
    Var('chromium_git') + '/external/github.com/project-oak/oak.git' + '@' + '8288b8006066499d95c370498da9bb04348914f3',

  'src/third_party/ots/src':
    Var('chromium_git') + '/external/github.com/khaledhosny/ots.git' + '@' + Var('ots_revision'),

  'src/third_party/libgav1/src':
    Var('chromium_git') + '/codecs/libgav1.git' + '@' + 'c3c824dcc1b907eed484f88ca7af21c743d27494',

  'src/third_party/google-truth/src': {
      'url': Var('chromium_git') + '/external/github.com/google/truth.git' + '@' + '67bc67430c5eabedf6134e364dcb04d1efc57ae9',
      'condition': 'checkout_android',
  },

  'src/third_party/googletest/src':
    Var('chromium_git') + '/external/github.com/google/googletest.git' + '@' + Var('googletest_revision'),

  # TODO: crbug.com/431264806 - Switch to CIPD package for Windows builds.
  'src/third_party/gperf': {
      'url': Var('chromium_git') + '/chromium/deps/gperf.git' + '@' + 'e9eeea862a18e77b945d98eff7e1bf065d3daf8e',
      'condition': 'checkout_win',
  },

  'src/third_party/gperf/cipd': {
      'packages': [
        {
          'package': 'infra/3pp/tools/gperf/${{platform}}',
          'version': 'version:3@3.2.chromium.1',
        },
      ],
      'condition': 'host_os == "linux" and non_git_source',
      'dep_type': 'cipd',
  },

  # Always download Linux x64 package regardless of host OS for RBE workers.
  'src/third_party/cpython3/linux-amd64': {
      'packages': [
        {
          'package': 'infra/3pp/tools/cpython3/linux-amd64',
          'version': Var('cpython3_version'),
        },
      ],
      'condition': 'non_git_source',
      'dep_type': 'cipd',
  },

  # Host platform package. ${platform} folder is not used as in .gn the variable
  # is not initialized yet by the time Python is required.
  'src/third_party/cpython3/host': {
      'packages': [
        {
          'package': 'infra/3pp/tools/cpython3/${{platform}}',
          'version': Var('cpython3_version'),
        },
      ],
      'condition': 'non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/cardboard/src' : {
      'url': Var('chromium_git') + '/external/github.com/googlevr/cardboard/' + '@' + '8bd424da7f1e399255a1b6e7c02be8e281ecc974',
      'condition': 'checkout_android',
  },

  'src/third_party/arcore-android-sdk/src': {
      'url': Var('chromium_git') + '/external/github.com/google-ar/arcore-android-sdk.git' + '@' + '80036a515b38deca1ad080b7c436856b454358f5',
      'condition': 'checkout_android',
  },

  'src/third_party/arcore-android-sdk-client/cipd': {
      'packages': [
        {
          'package': 'chromium/third_party/arcore-android-sdk-client',
          'version': 'gHDxvBRNpM868XTWU9SdfMqtVYTFSvK2tLRAKq4V37wC',
        },
      ],

      'condition': 'checkout_android and non_git_source',
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

  'src/third_party/google-java-format/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/google-java-format',
              'version': 'URUSmFrNzqsVV2mQVMaVMdyBwYBBOYqHcwjw7XNvZL0C',
          },
      ],
      # Needed on Linux for use on chromium_presubmit.
      'condition': '(checkout_android or checkout_linux) and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/hamcrest/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/hamcrest',
              'version': 'dBioOAmFJjqAr_DY7dipbXdVfAxUQwjOBNibMPtX8lQC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/hunspell_dictionaries':
    Var('chromium_git') + '/chromium/deps/hunspell_dictionaries.git' + '@' + 'eb106bd524eea61487644fffeec0d229fa41791a',

  'src/third_party/icu':
    Var('chromium_git') + '/chromium/deps/icu.git' + '@' + 'c670d124c9b6aa92c0b492cf543808300e0e6103',

  'src/third_party/icu4j/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/icu4j',
              'version': '8dV7WRVX0tTaNNqkLEnCA_dMofr2MJXFK400E7gOFygC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/jacoco/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/jacoco',
              'version': '6qArMm1GKYy92Ubn7rJ-8TGgwpr90jl792Roc1OCMs0C',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/javalang/src': {
      'url': Var('chromium_git') + '/external/github.com/c2nes/javalang.git' + '@' + '566963547575e93d305871d9cb26ce47ff1a036e',
      'condition': 'checkout_android',
  },

  'src/third_party/jdk/current': {
      'packages': [
          {
              'package': 'chromium/third_party/jdk/${{platform}}',
              'version': Var('jdk_version'),
          },
      ],
      # Needed on Linux for use on chromium_presubmit (for checkstyle).
      'condition': '(checkout_android or checkout_linux) and non_git_source and '
                   '((host_os == "linux" and host_cpu == "x64") or '
                   '(host_os == "mac" and host_cpu == "arm64"))',
      'dep_type': 'cipd',
  },

  # Remote Java actions run on Linux workers, so non-Linux hosts also need the
  # Linux JDK.
  'src/third_party/jdk/linux-amd64': {
      'packages': [
          {
              'package': 'chromium/third_party/jdk/linux-amd64',
              'version': Var('jdk_version'),
          },
      ],
      'condition': 'checkout_android and non_git_source and host_os != "linux"',
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

  'src/third_party/nlohmann_json/src':
    Var('chromium_git') + '/external/github.com/nlohmann/json.git'+ '@' + '55f93686c01528224f448c19128836e7df245f72',

  'src/third_party/jsoncpp/source':
    Var('chromium_git') + '/external/github.com/open-source-parsers/jsoncpp.git'
      + '@' + '93acc7b155048f1a66c43235cc205519f1e7a6ea',

  'src/third_party/junit/src': {
      'url': Var('chromium_git') + '/external/junit.git' + '@' + '890f3c972647de378f25e7271d8fbbd9d3456b79',
      'condition': 'checkout_android',
  },

  'src/third_party/kotlinc/current': {
      'packages': [
          {
              'package': 'chromium/third_party/kotlinc',
              'version': 'Xy4qNWZP3NoO07sqwJWALGJi4bZ9G0Gx0TFES96r7cUC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/ktfmt/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/ktfmt',
              'version': '92DTkkrV319mkh0f9oIJcZ-fsvUo8YMXaAY5BQmD-M8C',
          },
      ],
      'condition': '(checkout_android or checkout_linux) and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/leveldatabase/src':
    Var('chromium_git') + '/external/leveldb.git' + '@' + 'bb74ef739973a70ca9f0d90788a1da89b788c0ad',

  'src/third_party/libFuzzer/src':
    Var('chromium_git') + '/external/github.com/llvm/llvm-project/compiler-rt/lib/fuzzer.git' + '@' +  Var('libfuzzer_revision'),

  'src/third_party/fuzztest/src':
    Var('chromium_git') + '/external/github.com/google/fuzztest.git' + '@' + Var('fuzztest_revision'),

  'src/third_party/domato/src':
    Var('chromium_git') + '/external/github.com/googleprojectzero/domato.git' + '@' + Var('domato_revision'),

  'src/third_party/libaddressinput/src':
    Var('chromium_git') + '/external/libaddressinput.git' + '@' + '81eb9628382b07d371d8ea0b11badf7de3857fd5',

  'src/third_party/libaom/source/libaom':
    Var('aomedia_git') + '/aom.git' + '@' +  '8f126544519961bb3d5afe83455f4caf5d312d05',

  'src/third_party/crabbyavif/src':
    Var('chromium_git') + '/external/github.com/webmproject/CrabbyAvif.git' + '@' + Var('crabbyavif_revision'),

  'src/third_party/nearby/src':
    Var('chromium_git') + '/external/github.com/google/nearby-connections.git' + '@' + Var('nearby_revision'),

  'src/third_party/securemessage/src':
    Var('chromium_git') + '/external/github.com/google/securemessage.git' + '@' + Var('securemessage_revision'),

  'src/third_party/jetstream/main': {
    'url': Var('chromium_git') + '/external/github.com/WebKit/JetStream.git' + '@' + Var('jetstream_main_revision'),
    'condition': 'checkout_press_benchmarks',
  },

  'src/third_party/jetstream/v2.2': {
    'url': Var('chromium_git') + '/external/github.com/WebKit/JetStream.git' + '@' + Var('jetstream_2.2_revision'),
    'condition': 'checkout_press_benchmarks',
  },

  'src/third_party/jetstream/v3.0': {
    'url': Var('chromium_git') + '/external/github.com/WebKit/JetStream.git' + '@' + Var('jetstream_3.0_revision'),
    'condition': 'checkout_press_benchmarks',
  },

  'src/third_party/speedometer/main': {
    'url': Var('chromium_git') + '/external/github.com/WebKit/Speedometer.git' + '@' + Var('speedometer_main_revision'),
    'condition': 'checkout_press_benchmarks',
  },

  'src/third_party/speedometer/v3.1': {
    'url': Var('chromium_git') + '/external/github.com/WebKit/Speedometer.git' + '@' + Var('speedometer_3.1_revision'),
    'condition': 'checkout_press_benchmarks',
  },

  'src/third_party/speedometer/v3.0': {
    'url': Var('chromium_git') + '/external/github.com/WebKit/Speedometer.git' + '@' + Var('speedometer_3.0_revision'),
    'condition': 'checkout_press_benchmarks',
  },

  'src/third_party/speedometer/v2.1': {
    'url': Var('chromium_git') + '/external/github.com/WebKit/Speedometer.git' + '@' + Var('speedometer_2.1_revision'),
    'condition': 'checkout_press_benchmarks',
  },

  'src/third_party/speedometer/v2.0': {
    'url': Var('chromium_git') + '/external/github.com/WebKit/Speedometer.git' + '@' + Var('speedometer_2.0_revision'),
    'condition': 'checkout_press_benchmarks',
  },

  'src/third_party/ukey2/src':
    Var('chromium_git') + '/external/github.com/google/ukey2.git' + '@' + Var('ukey2_revision'),

  'src/third_party/cros-components/src':
    Var('chromium_git') + '/external/google3/cros_components.git' + '@' + Var('cros_components_revision'),

  'src/third_party/libdisplay-info/src': {
      'url': Var('chromium_git') + '/external/gitlab.freedesktop.org/emersion/libdisplay-info.git' + '@' + 'c67a3e9bedb05ab61c7443704a1a107e76254595',
      'condition': 'checkout_linux and checkout_mutter',
  },

  # Userspace interface to kernel DRM services.
  'src/third_party/libdrm/src': {
      'url': Var('chromium_git') + '/chromiumos/third_party/libdrm.git' + '@' + 'e984d448b8b17aab853369e6c203e53719f46de1',
      'condition': 'checkout_linux',
  },

  'src/third_party/libgudev/src': {
      'url': Var('chromium_git') + '/external/gitlab.gnome.org/GNOME/libgudev.git' + '@' + 'df7c9c9940160307aaeb31347f4776a46f8736a9',
      'condition': 'checkout_linux and checkout_mutter',
  },

  'src/third_party/libinput/src': {
      'url': Var('chromium_git') + '/external/gitlab.freedesktop.org/libinput/libinput.git' + '@' + '5cf833b572f5a33663888da321dce0ba1abd82a2',
      'condition': 'checkout_linux and checkout_mutter',
  },

  'src/third_party/expat/src':
    Var('chromium_git') + '/external/github.com/libexpat/libexpat.git' + '@' + Var('libexpat_revision'),

  # The library for IPP protocol (Chrome OS).
  'src/third_party/libipp/libipp': {
      'url': Var('chromium_git') + '/chromiumos/platform2/libipp.git' + '@' + '4be5f77f672a3a9f1bbf3c935fb0ea8b3f86ce61',
      'condition': 'checkout_linux',
  },

  'src/third_party/libjpeg_turbo':
    Var('chromium_git') + '/chromium/deps/libjpeg_turbo.git' + '@' + '640f254ad0fa03f6b1f29f89b7dd9366f2f6e533',

  'src/third_party/liblouis/src': {
      'url': Var('chromium_git') + '/external/liblouis-github.git' + '@' + '9700847afb92cb35969bdfcbbfbbb74b9c7b3376',
      'condition': 'checkout_linux',
  },

  'src/third_party/libphonenumber/src':
    Var('chromium_git') + '/external/libphonenumber.git' + '@' + '1ae0d623301b302f828628dfc368c983e8d27141',

  'src/third_party/libprotobuf-mutator/src':
    Var('chromium_git') + '/external/github.com/google/libprotobuf-mutator.git' + '@' +  Var('libprotobuf-mutator'),

  'src/third_party/libsrtp':
    Var('chromium_git') + '/chromium/deps/libsrtp.git' + '@' + 'cd5d177bf1fde755ddb4c7f0d9ff7693f8b49e5e',

  'src/third_party/libunwindstack': {
      'url': Var('chromium_git') + '/chromium/src/third_party/libunwindstack.git' + '@' + '333fcafb91bd3830c5ef814c071ff73df9cdc976',
      'condition': 'checkout_android',
  },

  'src/third_party/libva-fake-driver/src':
    Var('chromium_git') + '/chromiumos/platform/libva-fake-driver.git' + '@' + '04f902b9ba9f8083b19d22e7b55591bbcaae31c0',

  'src/third_party/libvpx/source/libvpx':
    Var('chromium_git') + '/webm/libvpx.git' + '@' +  'fb1279dcc6db0cbfb0b6015d0ba7587759c5625a',

  'src/third_party/libwebm/source':
    Var('chromium_git') + '/webm/libwebm.git' + '@' + 'a7c6662c68a90af18dd53dbb1f4262a346e9a547',

  'src/third_party/libwebp/src':
    Var('chromium_git') + '/webm/libwebp.git' + '@' +  '68dd1c3040d500f56fe7ffb754b1813fb64b5887',

  'src/third_party/libyuv':
    Var('chromium_git') + '/libyuv/libyuv.git' + '@' + '1e2e3a35993609da1d89ac170b907719d555f4b3',

  'src/third_party/lss': {
      'url': Var('chromium_git') + '/linux-syscall-support.git' + '@' + Var('lss_revision'),
      'condition': 'checkout_android or checkout_linux',
  },

  'src/third_party/lzma_sdk/bin/host_platform': {
      'packages': [
          {
              'package': 'infra/3pp/tools/7z/${{platform}}',
              'version': 'version:3@26.03',
          },
      ],
      'condition': 'checkout_win',
      'dep_type': 'cipd',
  },

  'src/third_party/lzma_sdk/bin/win64': {
      'packages': [
          {
              'package': 'infra/3pp/tools/7z/windows-amd64',
              'version': 'version:3@26.03',
          },
      ],
      'condition': 'checkout_win',
      'dep_type': 'cipd',
  },

  'src/third_party/mac_alias/src': {
      'url': Var('chromium_git') + '/external/github.com/dmgbuild/mac_alias.git' + '@' + 'ecc3ad82459ec92894de5994d1b5c4adacbbc7a7',
  },

  'src/third_party/material_color_utilities/src': {
      'url': Var('chromium_git') + '/external/github.com/material-foundation/material-color-utilities.git' + '@' + 'dd62982dfdefdfc9232ce8d76e122fb1c6d188ae',
  },

  'src/third_party/microsoft_dxheaders/src': {
    'url': Var('chromium_git') + '/external/github.com/microsoft/DirectX-Headers.git' + '@' + '07b94cba474be6cdfd934febdbd0b21ca0524187',
    'condition': 'checkout_win',
  },

  'src/third_party/microsoft_multimedia_redirection/src': {
    'url': Var('chromium_git') + '/external/github.com/microsoft/multimedia-redirection.git' + '@' + '8b1d8893d126547309f1bab8d8232418aa7c7109',
    'condition': 'checkout_win',
  },

  'src/third_party/microsoft_webauthn/src': {
    'url': Var('chromium_git') + '/external/github.com/microsoft/webauthn.git' + '@' + 'ef82c157125a0490e05f6ea82a7adb1b8e1bad08',
    'condition': 'checkout_win',
  },

  'src/third_party/mig/bin': {
      'packages': [
          {
              'package': 'chromium/third_party/mig/${{platform}}',
              'version': '4wxov_ILjFdgBumBqgUgOgIcr4kcMh7i4b4oJi_cLjcC',
          },
      ],
      'condition': 'host_os == "linux" and (checkout_mac or checkout_ios)',
      'dep_type': 'cipd',
  },

  # Graphics buffer allocator for Chrome OS.
  'src/third_party/minigbm/src': {
      'url': Var('chromium_git') + '/chromiumos/platform/minigbm.git' + '@' + 'a2d42f09d696b04e6ded5ad38596d8554ffd8988',
      'condition': 'checkout_linux',
  },

  'src/third_party/mutter/src': {
      'url': Var('chromium_git') + '/external/gitlab.gnome.org/GNOME/mutter.git' + '@' + '9134e3a5869ce232bf95b77519005865747ba603',
      'condition': 'checkout_linux and checkout_mutter',
  },

  'src/third_party/pipewire/src': {
      'url': Var('chromium_git') + '/external/gitlab.freedesktop.org/pipewire/pipewire.git'
             + '@' + 'b741e0c74f5436f0c925f7741140db0efd32cf4e',
      'condition': 'checkout_linux',
  },

  'src/third_party/nasm': {
      'url': Var('chromium_git') + '/chromium/deps/nasm.git' + '@' +
      '525a09a813be0f75b646ee93fc2a31c27b87d722'
  },

  'src/third_party/neon_2_sse/src':
    Var('chromium_git') + '/external/github.com/intel/ARM_NEON_2_x86_SSE.git' + '@' + '78784fefa872994ea9acbb7371d3003884815116',

  'src/third_party/netty-tcnative/src': {
      'url': Var('chromium_git') + '/external/netty-tcnative.git' + '@' + 'b21c6eee658866b83122c674e3c0da054e09a3d7',
      'condition': 'checkout_android',
  },

  'src/third_party/netty4/src': {
      'url': Var('chromium_git') + '/external/netty4.git' + '@' + 'cc4420b13bb4eeea5b1cf4f93b2755644cd3b120',
      'condition': 'checkout_android',
  },

  'src/third_party/ninja': {
    'packages': [
      {
        'package': Var('ninja_package') + '${{platform}}',
        'version': Var('ninja_version'),
      }
    ],
    'condition': 'non_git_source',
    'dep_type': 'cipd',
  },
  'src/third_party/siso/cipd': {
    'packages': [
      {
        'package': 'build/siso/${{platform}}',
        'version': Var('siso_version'),
      }
    ],
    'condition': 'non_git_source',
    'dep_type': 'cipd',
  },

  'src/third_party/openh264/src':
    Var('chromium_git') + '/external/github.com/cisco/openh264' + '@' + '1bca62311e447fd9c39fc93de2c4b96cd5ce971c',

  'src/third_party/openh264rs/src':
    Var('chromium_git') + '/chromium/third_party/openh264rs.git' + '@' + Var('openh264rs_revision'),

  'src/third_party/openscreen/src':
    Var('chromium_git') + '/openscreen' + '@' + 'e090df71da01b6b5afff9ef2d9d2c4d28a950b9e',

  'src/third_party/openxr/src': {
    'url': Var('chromium_git') + '/external/github.com/KhronosGroup/OpenXR-SDK' + '@' + '75c53b6e853dc12c7b3c771edc9c9c841b15faaa',
    'condition': 'checkout_openxr',
  },

  'src/third_party/opus/tests/resources': {
      'dep_type': 'gcs',
      'condition': 'non_git_source',
      'bucket': 'chromium-webrtc-resources',
      'objects': [
          {
              'object_name': '009a3ee778767c2402b1d2c920bc2449265f5a2c',
              'sha256sum': '34de3161f242895a682d9cdcbbf4ad50246742b6db46873386104cfde8a24332',
              'size_bytes': 26889600,
              'generation': 1392811661954000,
              'output_file': 'speech_mono_32_48kHz.pcm',
          },
      ],
  },

  'src/third_party/pdfium':
    Var('pdfium_git') + '/pdfium.git' + '@' +  Var('pdfium_revision'),

  'src/third_party/perfetto':
    Var('chromium_git') + '/external/github.com/google/perfetto.git' + '@' + '214c983f4c72c442d71415d9c81a7e1ed11566a7',

  'src/base/tracing/test/data': {
    'bucket': 'perfetto',
    'objects': [
      {
        'object_name': 'test_data/chrome_fcp_lcp_navigations.pftrace-ae01d849fbd75a98be1b7ddd5a8873217c377b393a1d5bbd788ed3364f7fefc3',
        'sha256sum': 'ae01d849fbd75a98be1b7ddd5a8873217c377b393a1d5bbd788ed3364f7fefc3',
        'size_bytes': 2398645,
        'generation': 1697714434866488,
        'output_file': 'chrome_fcp_lcp_navigations.pftrace'
      },
      {
        'object_name': 'test_data/chrome_input_with_frame_view.pftrace-a93548822e481508c728ccc5da3ad34afcd0aec02ca7a7a4dad84ff340ee5975',
        'sha256sum': 'a93548822e481508c728ccc5da3ad34afcd0aec02ca7a7a4dad84ff340ee5975',
        'size_bytes': 6392331,
        'generation': 1711402389089075,
        'output_file': 'chrome_input_with_frame_view.pftrace'
      },
      {
        'object_name': 'test_data/scroll_offsets_trace_2.pftrace-2ddd9f78d91d51e39c72c520bb54fdc9dbf1333ae722e87633fc345159296289',
        'sha256sum': '2ddd9f78d91d51e39c72c520bb54fdc9dbf1333ae722e87633fc345159296289',
        'size_bytes': 1496388,
        'generation': 1712592637141461,
        'output_file': 'scroll_offsets_trace_2.pftrace'
      },
      {
        'object_name': 'test_data/top_level_java_choreographer_slices-8001e73b2458e94f65a606bb558a645ba5bca553b57fe416001f6c2175675a8a',
        'sha256sum': '8001e73b2458e94f65a606bb558a645ba5bca553b57fe416001f6c2175675a8a',
        'size_bytes': 5323017,
        'generation': 1671708979893186,
        'output_file': 'top_level_java_choreographer_slices'
      },
      {
        'object_name': 'test_data/chrome_page_load_all_categories_not_extended.pftrace.gz-6586e9e2bbc0c996caddb321a0374328654983733e6ffd7f4635ac07db32a493',
        'sha256sum': '6586e9e2bbc0c996caddb321a0374328654983733e6ffd7f4635ac07db32a493',
        'size_bytes': 1277750,
        'generation': 1652442088902445,
        'output_file': 'chrome_page_load_all_categories_not_extended.pftrace.gz'
      },
      {
        'object_name': 'test_data/jetstream_3.pftrace.gz-8b5915cdf7f4182a74345cc080c34ce190835e5b46c30e921ce81433119acfda',
        'sha256sum': '8b5915cdf7f4182a74345cc080c34ce190835e5b46c30e921ce81433119acfda',
        'size_bytes': 78162,
        'generation': 1771505932279144,
        'output_file': 'jetstream_3.pftrace.gz'
      },
      {
        'object_name': 'test_data/speedometer_21.perfetto_trace.gz-8a159b354d74a3ca0d38ce9cd071ef47de322db4261ee266bfafe04d70310529',
        'sha256sum': '8a159b354d74a3ca0d38ce9cd071ef47de322db4261ee266bfafe04d70310529',
        'size_bytes': 891088,
        'generation': 1716566741068306,
        'output_file': 'speedometer_21.perfetto_trace.gz'
      },
      {
       'object_name': 'test_data/speedometer_3.perfetto_trace.gz-b2c77fbe2c17363432a1ad0c05c1c1c20d3ebc62bda92c041d39918011af6f02',
        'sha256sum': 'b2c77fbe2c17363432a1ad0c05c1c1c20d3ebc62bda92c041d39918011af6f02',
        'size_bytes': 1301036,
        'generation': 1716566914245446,
        'output_file': 'speedometer_3.perfetto_trace.gz'
      },
      {
        'object_name': 'test_data/scroll_jank_with_pinch.pftrace-8587d2016fdb5d39b5f852704b6e3925e9e6527af01696396be767bed04d4a45',
        'sha256sum': '8587d2016fdb5d39b5f852704b6e3925e9e6527af01696396be767bed04d4a45',
        'size_bytes': 3914720,
        'generation': 1717497788778335,
        'output_file': 'scroll_jank_with_pinch.pftrace'
      },
      {
        'object_name': 'test_data/cpu_powerups_1.pb-70f5511ba0cd6ce1359e3cadb4d1d9301fb6e26be85158e3384b06f41418d386',
        'sha256sum': '70f5511ba0cd6ce1359e3cadb4d1d9301fb6e26be85158e3384b06f41418d386',
        'size_bytes': 2033064,
        'generation': 1669652389509708,
        'output_file': 'cpu_powerups_1.pb'
      },
      {
        'object_name': 'test_data/chrome_5672_histograms.pftrace.gz-a09bd44078ac71bcfbc901b0544750e8344d0d0f6f96e220f700a5a53fa932ee',
        'sha256sum': 'a09bd44078ac71bcfbc901b0544750e8344d0d0f6f96e220f700a5a53fa932ee',
        'size_bytes': 1127472,
        'generation': 1684946598804577,
        'output_file': 'chrome_5672_histograms.pftrace.gz'
      },
      {
        'object_name': 'test_data/chrome_custom_navigation_trace.gz-ff68279e3cec94076b69259d756eed181a63eaf834d8b956a7f4ba665fabf939',
        'sha256sum': 'ff68279e3cec94076b69259d756eed181a63eaf834d8b956a7f4ba665fabf939',
        'size_bytes': 7572484,
        'generation': 1666713705258900,
        'output_file': 'chrome_custom_navigation_trace.gz'
      },
      {
        'object_name': 'test_data/scroll_offsets.pftrace-62101edb5204fec8bea30124f65d4e49bda0808d7b036e95f89445aaad6d8d98',
        'sha256sum': '62101edb5204fec8bea30124f65d4e49bda0808d7b036e95f89445aaad6d8d98',
        'size_bytes': 769741,
        'generation': 1693402148909129,
        'output_file': 'scroll_offsets.pftrace'
      },
      {
        'object_name': 'test_data/chrome_input_with_frame_view_new.pftrace-e901ad9577088e62c921dd8bfcb43d652ecf49fa69b5b57f81bb3d27dbe94e12',
        'sha256sum': 'e901ad9577088e62c921dd8bfcb43d652ecf49fa69b5b57f81bb3d27dbe94e12',
        'size_bytes': 1967821,
        'generation': 1719520814352733,
        'output_file': 'chrome_input_with_frame_view_new.pftrace'
      },
      {
        'object_name': 'test_data/scroll_m131.pftrace-14171c9e502a65a454f39fe14fce8b313c7012a2c14394bed496fc93b1644b0d',
        'sha256sum': '14171c9e502a65a454f39fe14fce8b313c7012a2c14394bed496fc93b1644b0d',
        'size_bytes': 1247290,
        'generation': 1729619115836875,
        'output_file': 'scroll_m131.pftrace'
      },
      {
        'object_name': 'test_data/scroll_m132.pftrace-779e1fa6cab1abc0ab2dcafa4bbbf826ea8fc052878886ab168c783ca32f3bbe',
        'sha256sum': '779e1fa6cab1abc0ab2dcafa4bbbf826ea8fc052878886ab168c783ca32f3bbe',
        'size_bytes': 1854184,
        'generation': 1732117381568969,
        'output_file': 'scroll_m132.pftrace'
      },
      {
        'object_name': 'test_data/scroll_m132_with_atrace.pftrace-b1fe189738a4a714cef7dd13b12318c89d6c4345fbe65096b4680ffa79f2872a',
        'sha256sum': 'b1fe189738a4a714cef7dd13b12318c89d6c4345fbe65096b4680ffa79f2872a',
        'size_bytes': 18887098,
        'generation': 1730148514067154,
        'output_file': 'scroll_m132_with_atrace.pftrace'
      },
      {
        'object_name': 'test_data/scroll_m133beta.pftrace-2acc70545be1caeb3fb43218ab52872c2c749b49577b973cb8400057c680fc19',
        'sha256sum': '2acc70545be1caeb3fb43218ab52872c2c749b49577b973cb8400057c680fc19',
        'size_bytes': 1448694,
        'generation': 1738338471679054,
        'output_file': 'scroll_m133beta.pftrace'
      },
      {
        'object_name': 'test_data/scroll_with_input_not_forwarded_to_renderer.pftrace-ceb2ff3cb64c99ddb28a29867bc82f1491c4b413bfacd8638551b80936240faf',
        'sha256sum': 'ceb2ff3cb64c99ddb28a29867bc82f1491c4b413bfacd8638551b80936240faf',
        'size_bytes': 729606,
        'generation': 1732902685002882,
        'output_file': 'scroll_with_input_not_forwarded_to_renderer.pftrace'
      },
      {
        'object_name': 'test_data/scroll_m144.pftrace-6a3426ee6a441cf71ec8b634b662fe800700b8c3d2d1e9f247feaba890315843',
        'sha256sum': '6a3426ee6a441cf71ec8b634b662fe800700b8c3d2d1e9f247feaba890315843',
        'size_bytes': 4384532,
        'generation': 1769534965677794,
        'output_file': 'scroll_m144.pftrace'
      },
      {
        'object_name': 'test_data/loadline2_phone_perfetto_trace.pb.gz-446e12518282e078360cb1ab55ebb3e3788c3863b269601d2392c9e5d058a92c',
        'sha256sum': '446e12518282e078360cb1ab55ebb3e3788c3863b269601d2392c9e5d058a92c',
        'size_bytes': 1214995,
        'generation': 1763134487216405,
        'output_file': 'loadline2_phone_perfetto_trace.pb.gz'
      },
    ],
    'dep_type': 'gcs'
  },

  'src/third_party/perl': {
      'url': Var('chromium_git') + '/chromium/deps/perl.git' + '@' + 'a2bd4470d3485e6eda532eee416a71d16db2d7ba',
      'condition': 'checkout_win',
  },

  'src/third_party/protobuf-javascript/src':
    Var('chromium_git') + '/external/github.com/protocolbuffers/protobuf-javascript' + '@' + 'e6d763860001ba1a76a63adcff5efb12b1c96024',

  'src/third_party/pthreadpool/src':
    Var('chromium_git') + '/external/github.com/google/pthreadpool.git' + '@' + '15a6644ba1c45f1acc16ac1e883efc3e56c6bed2',

  # Dependency of skia.
  'src/third_party/pyelftools': {
      'url': Var('chromium_git') + '/chromiumos/third_party/pyelftools.git' + '@' + '2909369467745064ab218de04fb3d7199ed44e4c',
      'condition': 'checkout_linux',
  },

  'src/third_party/quic_trace/src':
    Var('chromium_git') + '/external/github.com/google/quic-trace.git' + '@' + '6ecf6aa8e604655f1dfa72c4a1ae733c4c7fc883',

  'src/third_party/pywebsocket3/src':
    Var('chromium_git') + '/external/github.com/GoogleChromeLabs/pywebsocket3.git' + '@' + '139224e0d71db84fe1922601cb92ec8c8f3885ba',

  'src/third_party/qemu-linux-arm64': {
      'packages': [
          {
              'package': 'fuchsia/third_party/qemu/linux-arm64',
              'version': 'go4qfwROMthoOfAh-G1240ysyEx07kIuWBqqkXENA7AC'
          },
      ],
      # TODO(b/351926334): Do not add `non_git_source` to this condition until the bug is fixed.
      'condition': 'host_os == "linux" and checkout_fuchsia and checkout_fuchsia_for_arm64_host',
      'dep_type': 'cipd',
  },

  'src/third_party/re2/src':
    Var('chromium_git') + '/external/github.com/google/re2.git' + '@' + '2da0056814cf180480a19f5cf811e7e1c054bf6d',

  'src/third_party/r8/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/r8',
              'version': 'XXyYTl5yoVa0ipEckErzaApx4jB7EmePZ_qSZ6skqx0C',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/requests/src': {
      'url': Var('chromium_git') + '/external/github.com/kennethreitz/requests.git' + '@' + 'c2b307dbefe21177af03f9feb37181a89a799fcc',
      'condition': 'checkout_android',
  },

  'src/third_party/robolectric/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/robolectric/${{platform}}',
              'version': 'version:2@android-all-17-robolectric-15733970-2921868143',
          },
      ],
      'condition': 'checkout_android and non_git_source and '
                   '((host_os == "linux" and host_cpu == "x64") or '
                   '(host_os == "mac" and host_cpu == "arm64"))',
      'dep_type': 'cipd',
  },

  'src/third_party/ruy/src':
    Var('chromium_git') + '/external/github.com/google/ruy.git' + '@' + '2264753777198e4393fb83c44c693462d57a2be1',

  'src/third_party/search_engines_data/resources':
    Var('chromium_git') + '/external/search_engines_data.git' + '@' + '5dc9f1afd38a8ac2b26b1972eb4fc1bbd5754e8c',

  'src/third_party/search_engines_data/resources_internal': {
    'url': Var('chrome_git') + '/external/search_engines_data_internal.git' + '@' + '7c90875a405251ca4b5407fff7759f08bc8069d1',
    'condition': 'checkout_src_internal',
  },

  'src/third_party/sframe/src':
    Var('chromium_git') + '/external/github.com/cisco/sframe' + '@' + '6a37f061f4eda0c6594c91ecf99b5ff498c8c124',

  'src/third_party/skia':
    Var('skia_git') + '/skia.git' + '@' +  Var('skia_revision'),

  'src/third_party/smhasher/src':
    Var('chromium_git') + '/external/smhasher.git' + '@' + '07bb4de10a63e8cc2e1724865454eba635742383',

  'src/third_party/snappy/src':
    Var('chromium_git') + '/external/github.com/google/snappy.git' + '@' + '32ded457c0b1fe78ceb8397632c416568d6714a0',

  'src/third_party/sqlite/src':
    Var('chromium_git') + '/chromium/deps/sqlite.git' + '@' + 'c1172ac3c91edb02a86ea9ce479ef5b5fe5c6f34',

  'src/third_party/sqlite4java/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/sqlite4java',
              'version': 'LofjKH9dgXIAJhRYCPQlMFywSwxYimrfDeBmaHc-Z5EC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/subresource-filter-ruleset/data': {
      'dep_type': 'gcs',
      'condition': 'non_git_source',
      'bucket': 'chromium-ads-detection',
      'objects': [
          {
              'object_name': '22a0a5fc9e38d0de4e68eaf7e961dbbd2bebf234bebffbcd21bd496c0813fcde',
              'sha256sum': '22a0a5fc9e38d0de4e68eaf7e961dbbd2bebf234bebffbcd21bd496c0813fcde',
              'size_bytes': 158519,
              'generation': 1789431671005402,
              'output_file': 'UnindexedRules',
          },
      ],
  },

  'src/third_party/swift-format': {
      'packages': [
          {
              'package': 'infra/3pp/tools/swift-format/mac-${{arch}}',
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
              'version': 'version:2@5.8-release',
          },
      ],
      'condition': 'host_os == mac',
      'dep_type': 'cipd',
  },

  'src/third_party/test_fonts/test_fonts': {
      'dep_type': 'gcs',
      'condition': 'non_git_source',
      'bucket': 'chromium-fonts',
      'objects': [
          {
              'object_name': '9c07d19d9c5ee1ff94f717e6fb17e0c8c354e6f9',
              'sha256sum': 'f0e9628f9e43e3f3476cde06a1849058de460e0e037b7449ce0d42b9a73c37d5',
              'size_bytes': 33413117,
              'generation': 1775663926405386
          },
      ],
  },

  'src/third_party/text-fragments-polyfill/src':
    Var('chromium_git') + '/external/github.com/GoogleChromeLabs/text-fragments-polyfill.git' + '@' + 'abc6ed408b3f20e91d9cbda9977748459f5e3877',

  'src/third_party/tflite/src':
    Var('chromium_git') + '/external/github.com/tensorflow/tensorflow.git' + '@' + '201e0b2f5e7469145e4c37efb7459eea0b7a79d5',
  'src/third_party/litert/src':
    Var('chromium_git') + '/external/github.com/google-ai-edge/LiteRT.git' + '@' + '86ae38de889076209e2eb71d9ac43f243226cb3a',
  'src/third_party/ml_drift/src':
    Var('chromium_git') + '/external/github.com/google-ai-edge/ml-drift.git' + '@' + '7483ad8a5181b220a19bcc20ec02dbe9c040c100',
  'src/third_party/turbine/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/turbine',
              'version': 'cbhWS80l6vxMsButpJwV9RdtCh3F5ds7bcRvcwUUuiEC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  # The Linux binary is always downloaded regardless of host os and architecture
  # since remote node actions run on Linux worker.
  # See also //third_party/typescript/tsgo.gni
  'src/third_party/typescript/linux-amd64/src': {
      'dep_type': 'cipd',
      'condition': 'non_git_source',
      'packages': [
          {
              'package': 'chromium/third_party/typescript/linux-amd64',
              'version': 'version:2@7.0.2',
          },
      ],
  },

  'src/third_party/typescript/mac-amd64/src': {
      'dep_type': 'cipd',
      'condition': 'host_os == "mac" and host_cpu == "x64" and non_git_source',
      'packages': [
          {
              'package': 'chromium/third_party/typescript/mac-amd64',
              'version': 'version:2@7.0.2',
          },
      ],
  },

  'src/third_party/typescript/mac-arm64/src': {
      'dep_type': 'cipd',
      'condition': 'host_os == "mac" and host_cpu == "arm64" and non_git_source',
      'packages': [
          {
              'package': 'chromium/third_party/typescript/mac-arm64',
              'version': 'version:2@7.0.2',
          },
      ],
  },

  'src/third_party/typescript/windows-amd64/src': {
      'dep_type': 'cipd',
      'condition': 'checkout_win and non_git_source',
      'packages': [
          {
              'package': 'chromium/third_party/typescript/windows-amd64',
              'version': 'version:2@7.0.2',
          },
      ],
  },

  'src/third_party/vulkan-deps': '{chromium_git}/vulkan-deps@40a2f4ac1c8988ea00961bfd0cc49f6396e65fa0',
  'src/third_party/glslang/src': '{chromium_git}/external/github.com/KhronosGroup/glslang@a37cfbe30cd45ec64b3b74fc128eaa9595789192',
  'src/third_party/spirv-cross/src': '{chromium_git}/external/github.com/KhronosGroup/SPIRV-Cross@b8fcf307f1f347089e3c46eb4451d27f32ebc8d3',
  'src/third_party/spirv-headers/src': '{chromium_git}/external/github.com/KhronosGroup/SPIRV-Headers@fbf86836a9b1966586faf7be7bb8a6440e156d62',
  'src/third_party/spirv-tools/src': '{chromium_git}/external/github.com/KhronosGroup/SPIRV-Tools@066042ec3a9af3094c794bbbbd3600689fe7ff29',
  'src/third_party/vulkan-headers/src': '{chromium_git}/external/github.com/KhronosGroup/Vulkan-Headers@73814d006638aa00bd553215b054f0f02e457e22',
  'src/third_party/vulkan-loader/src': '{chromium_git}/external/github.com/KhronosGroup/Vulkan-Loader@3172c4859f613576c1811b9e7a25d61230ecaeaa',
  'src/third_party/vulkan-tools/src': '{chromium_git}/external/github.com/KhronosGroup/Vulkan-Tools@c38e7c390892e06fe4c84f0c19fb891c79f885d9',
  'src/third_party/vulkan-utility-libraries/src': '{chromium_git}/external/github.com/KhronosGroup/Vulkan-Utility-Libraries@205badc9cf9ca28000fcea4b0addd50d75655c8c',
  'src/third_party/vulkan-validation-layers/src': '{chromium_git}/external/github.com/KhronosGroup/Vulkan-ValidationLayers@14d48a8357631a6494d68a635e85a6fa5d408eac',

  'src/third_party/vulkan_memory_allocator':
    Var('chromium_git') + '/external/github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git' + '@' + '82a9d47e4f9d91f0e32d2b6acd9714fcee1933a0',

  # Display server protocol for Linux.
  'src/third_party/wayland/src': {
      'url': Var('chromium_git') + '/external/anongit.freedesktop.org/git/wayland/wayland.git' + '@' + '87cc8a8728a923fc57938faa81ba0e74f34ecdc7',
      'condition': 'checkout_linux',
  },

  # Wayland protocols that add functionality not available in the core protocol.
  'src/third_party/wayland-protocols/src': {
      'url': Var('chromium_git') + '/external/anongit.freedesktop.org/git/wayland/wayland-protocols.git' + '@' + 'ee78491a237eaff9389a0ccf8680521d074407d3',
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

  # Keep this version (currently 4.33.0) synchronized with the selenium wheel
  # in //vpython.toml.
  'src/third_party/webdriver/pylib':
    Var('chromium_git') + '/external/github.com/SeleniumHQ/selenium/py.git' + '@' + 'd4ee125066352f260f5cc9a495b2fefce9a9f095',

  'src/third_party/webgl/src':
    Var('chromium_git') + '/external/khronosgroup/webgl.git' + '@' + '714857a28445e8f5d8d6ae1c78498578009534d8',

  'src/third_party/webgpu-cts/src':
    Var('chromium_git') + '/external/github.com/gpuweb/cts.git' + '@' + 'd116bcccd1833369fa3715a6df503c0dd9b0d7fa',

  'src/third_party/webpagereplay':
    Var('chromium_git') + '/webpagereplay.git' + '@' + Var('webpagereplay_revision'),

  'src/third_party/webrtc':
    Var('webrtc_git') + '/src.git' + '@' + 'fb1c41a93d92d4188f068f437d80ef2c4b50c3a9',

  # Wuffs' canonical repository is at github.com/google/wuffs, but we use
  # Skia's mirror of Wuffs, the same as in upstream Skia's DEPS file.
  #
  # The local directory is called "third_party/wuffs" (matching upstream Skia
  # and other non-Chromium, Skia-using projects) even though the git repo we
  # clone is called "wuffs-mirror-release-c". The reasons for using w-m-r-c are
  # listed in the https://crrev.com/c/3086053 commit message. One reason is
  # that the w-m-r-c subset is much smaller and changes much less frequently.
  'src/third_party/wuffs/src':
    Var('skia_git') + '/external/github.com/google/wuffs-mirror-release-c.git' + '@' +  Var('wuffs_revision'),

  'src/third_party/weston/src': {
      'url': Var('chromium_git') + '/external/anongit.freedesktop.org/git/wayland/weston.git' + '@' + 'b65be9e699847c975440108a42f05412cc7fddac',
      'condition': 'checkout_linux',
  },

  'src/third_party/xnnpack/src':
    Var('chromium_git') + '/external/github.com/google/XNNPACK.git' + '@' + '49ac6bffa2343b7917163d88e6aacedb7d56d088',

  'src/third_party/libei/src': {
      'url': Var('chromium_git') + '/external/gitlab.freedesktop.org/libinput/libei.git' + '@' + 'a9bf31da06f06bfce73702191c3db93aae289459',
      'condition': 'checkout_linux',
  },

  'src/third_party/zstd/src':
    Var('chromium_git') + '/external/github.com/facebook/zstd.git' + '@' + '10da6ba6de05e29169261fa4b68eb99239f770dd',

  'src/tools/skia_goldctl/linux': {
      'packages': [
        {
          'package': 'skia/tools/goldctl/linux-amd64',
          'version': 'fc0rosm6jptTOG1_LCI0Nl_jFzMxMphDRYRQFET5I9wC',
        },
      ],
      'dep_type': 'cipd',
      'condition': 'checkout_linux and non_git_source',
  },
  'src/tools/skia_goldctl/win': {
      'packages': [
        {
          'package': 'skia/tools/goldctl/windows-amd64',
          'version': 'vSCkdxPlzerJ9Fa-VoP2YtjVKNidSinpsBHzDdf8BP4C',
        },
      ],
      'dep_type': 'cipd',
      'condition': 'checkout_win',
  },

  'src/tools/skia_goldctl/mac_amd64': {
      'packages': [
        {
          'package': 'skia/tools/goldctl/mac-amd64',
          'version': '7oC8mkcwErxSuuzv2_jl5iPGbqGzhlqj5CdrpIEJTZcC',
        },
      ],
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
  },

  'src/tools/skia_goldctl/mac_arm64': {
      'packages': [
        {
          'package': 'skia/tools/goldctl/mac-arm64',
          'version': 'y0Az2Icm0hFvopXhoXU_ORu8Aok93K-hisu0GrWTYoEC',
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

# See checkout_src_internal_infra declaration.
# LINT.IfChange
  'src/internal': {
    'url': Var('chrome_git') + '/chrome/src-internal.git' + '@' + Var('src_internal_revision'),
    'condition': 'checkout_src_internal or checkout_src_internal_infra',
  },
# LINT.ThenChange(/components/cronet/gn2bp/copy.bara.sky)

  'src/agents/internal': {
    'url': Var('chrome_git') + '/chrome/agents-internal.git' + '@' + Var('agents_internal_revision'),
    'condition': 'checkout_src_internal or checkout_src_internal_infra',
  },

  'src/agents/shared': {
    'url': Var('chromium_git') + '/chromium/agents.git' + '@' + Var('agents_public_revision'),
  },

  'src/internal/tools/edit_monitor/cipd': {
      'packages': [
          {
              'package': 'infra_internal/edit_monitor/linux-amd64',
              'version': 'GnKYrWWnxMRz9PuawzZLQd6jXfLy-yYYs03lINw5Oh4C',
          },
      ],
      'condition': 'host_os == "linux" and (checkout_src_internal or checkout_src_internal_infra)',
      'dep_type': 'cipd',
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
        'version': 'l3lxv3u6Ygb9O_8_rfz7yuJwBZ64trjGFMzyXIuNl6sC',
      },
    ],
    'condition': 'checkout_chromeos and checkout_src_internal',
    'dep_type': 'cipd',
  },

  'src/ash/webui/boca_receiver_app_ui/resources/prod': {
    'packages': [
      {
        'package': 'chromeos_internal/apps/boca_receiver_app/app',
        'version': 'GlPfBgTMjN_pj9cb0Hs6rlS6KjQSDZp9UcMPKUZ0wtUC',
      },
    ],
    'condition': 'checkout_chromeos and checkout_src_internal',
    'dep_type': 'cipd',
  },

  'src/ash/webui/boca_ui/resources/prod': {
    'packages': [
      {
        'package': 'chromeos_internal/apps/boca_app/app',
        'version': 'SfBsBs1LRIcDBuh3UEu_5tqko304A2GyVsjb-tski8UC',
      },
    ],
    'condition': 'checkout_chromeos and checkout_src_internal',
    'dep_type': 'cipd',
  },

  'src/ash/webui/help_app_ui/resources/prod': {
    'packages': [
      {
        'package': 'chromeos_internal/apps/help_app/app',
        'version': 'kTin2KNUOwSJ-hhqr_UVxEqJ30zV1LiLtLt_9RRn-iEC',
      },
    ],
    'condition': 'checkout_chromeos and checkout_src_internal',
    'dep_type': 'cipd',
  },

  'src/ash/webui/media_app_ui/resources/prod': {
    'packages': [
      {
        'package': 'chromeos_internal/apps/media_app/app',
        'version': 'iZtQs3crHBAxFVWNbE-ulAnsitNO724eh5hA83Zf3j8C',
      },
    ],
    'condition': 'checkout_chromeos and checkout_src_internal',
    'dep_type': 'cipd',
  },

  'src/ash/webui/personalization_app/resources': {
    'packages': [
      {
        'package': 'chromeos_internal/assistant/time_of_day',
        'version': 'VLbxe5avqbXTSa8HRjm9NNtD9H2wYovk8GPPxO0kpvQC',
      },
    ],
    'condition': 'checkout_chromeos and checkout_src_internal',
    'dep_type': 'cipd',
  },

  'src/ash/webui/shimless_rma/resources': {
    'packages': [
      {
        'package': 'chromeos_internal/ash/peripherals-and-serviceability/shimless-rma-project-simon-strings',
        'version': '-uRXiZeA4Yl-Nv-6jP69DyDs5cGroZgGsa1NHnVySQwC',
      },
    ],
    'condition': 'checkout_chromeos and checkout_src_internal',
    'dep_type': 'cipd',
  },

  'src/ash/webui/projector_app/resources/prod': {
    'packages': [
      {
        'package': 'chromeos_internal/apps/projector_app/app',
        'version': 'HGx-D8Yt-B5aF25dWg3h0elvxY3TUHOyiGAaYZ3eyB0C',
      },
    ],
    'condition': 'checkout_chromeos and checkout_src_internal',
    'dep_type': 'cipd',
  },

  'src/third_party/android_prebuilts/build_tools': {
      'url': Var('android_git') + '/platform/prebuilts/build-tools.git' + '@' + '3fbcdf38727f967badc13f9537bda0b4392c1f74',
      'condition': 'checkout_android_prebuilts_build_tools',
  },

  'src/third_party/android_deps/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/autorolled',
              'version': Var('android_deps_autorolled_version'),
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  # === ANDROID_DEPS Generated Code Start ===
  # Generated by //third_party/android_deps/fetch_all.py
  # === ANDROID_DEPS Generated Code End ===

  'src/tools/resultdb': {
      'packages': [
        {
          'package': 'infra/tools/result_adapter/${{platform}}',
          'version': Var('result_adapter_revision'),
        },
      ],
      'condition': 'non_git_source',
      'dep_type': 'cipd',
  },

  # Dependencies from src_internal
  'src/chromeos/ash/resources/internal': {
      'url': Var('chrome_git') + '/chrome/chromeos/ash/resources/internal.git' + '@' +
        'd4509cba32a0981c86a9fd63db3186f32c1042e2',
      'condition': 'checkout_src_internal and checkout_chromeos',
  },

  'src/chromeos/assistant/internal': {
      'url': Var('chrome_git') + '/chrome/assistant.git' + '@' +
        '1929b1906442b7b8d3fd5fffafe2ffbb9e395f13',
      'condition': 'checkout_src_internal and checkout_chromeos',
    },

  'src/ui/gl/resources/angle-metal': {
    'packages': [{
       'package': 'chromium/gpu/angle-metal-shader-libraries',
       'version': 'S0FPOVKrgaiqyuR20SSHiPorLgYez29bfwEdKBobUMMC',
    }],
    'dep_type': 'cipd',
    'condition': 'checkout_mac or checkout_ios',
  },
  'src/buildtools/android/doclava': {
    'dep_type': 'gcs',
    'bucket': 'chromium-doclava',
    'condition': 'checkout_android and non_git_source',
    'objects': [
      {
      'object_name': '1931becb8a8e21685f39c62854e9e814d64ccf1a',
      'sha256sum': '935a602c0dda7f769fc48fc777697ea23ccead170fbc70d2d41af5713ef50bbf',
      'size_bytes': 3517760,
      'generation': 1516199210583319,
      },
    ],
  },

  'src/chrome/app/theme/default_100_percent/google_chrome': {
      'url': Var('chrome_git') + '/chrome/theme/default_100_percent/google_chrome.git' + '@' +
        '7ee202378c4a9f7f27ee414d9279aed17dc0405d',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/app/theme/default_200_percent/google_chrome': {
      'url': Var('chrome_git') + '/chrome/theme/default_200_percent/google_chrome.git' + '@' +
        'ef57d258616e413be4280d12e946a6251867765a',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/app/theme/google_chrome': {
      'url': Var('chrome_git') + '/chrome/theme/google_chrome.git' + '@' +
        'a3f8a0d7fbeb56e64acf5d02b54d0e7c480bde4f',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/actor/resources/internal': {
      'url': Var('chrome_git') + '/chrome/browser/actor/resources/internal.git' + '@' +
        'a93269b6f5eaf301ee21a44c7d286cf9d029237c',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/cpu_performance_internal': {
      'url': Var('chrome_git') + '/chrome/browser/cpu_performance_internal.git' + '@' +
        'dfe1ccf3c2a93128082bb1d955517b4a6489ba62',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/enterprise/connectors/internal': {
      'url': Var('chrome_git') + '/chrome/browser/enterprise/connectors/internal.git' + '@' +
        '7fd7c8dd496740724d0024408ae7a96298e34aa2',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/glic/resources/internal': {
      'url': Var('chrome_git') + '/chrome/browser/glic/resources/internal.git' + '@' +
         '0edfdbec50d4175aa5319a12d953fda3993f342b',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/glic/e2e_test/internal': {
      'url': Var('chrome_git') + '/chrome/browser/glic/test/internal.git' + '@' +
        '7f8f7ff793b413e91da85dcf7d08cc23202fc053',
      'condition': 'checkout_glic_e2e_tests',
  },

  'src/chrome/browser/indigo/resources/internal': {
      'url': Var('chrome_git') + '/chrome/browser/indigo/resources/internal.git' + '@' +
        '981f85da72be35b841062295fffeac05bbddff44',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/google/linkdoctor_internal': {
      'url': Var('chrome_git') + '/chrome/linkdoctor.git' + '@' +
        'fe28a8f90c5471f20f8fee9ff7f6c6f8b8d02bed', # from svn revision 32577
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/internal': {
      'url': Var('chrome_git') + '/chrome/browser_internal.git' + '@' +
        'fca0d68798dd07690d4a52846a612b631b5d2c72',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/media/engagement_internal': {
      'url': Var('chrome_git') + '/chrome/browser/media/engagement_internal.git' + '@' +
        '14b00ddbb904612ec8805f00718ae3f95c02a076',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/nearby_sharing/internal': {
      'url': Var('chrome_git') + '/chrome/browser/nearby_sharing/internal.git' + '@' +
        '042372f4d9c1f5022eeb0b969cc56dd8ef3e461f',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/payments/internal': {
      'url': Var('chrome_git') + '/chrome/payments.git' + '@' +
        '2ee0bfd5990740fccfe223fd6c9fe21bc4846b9a',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/platform_experience/win': {
      'url': Var('chrome_git') + '/chrome/browser/platform_experience/win.git' + '@' +
        '1b84d873410cfc497c7987d2cd9084c75354993c',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/resources/downloads/internal': {
      'url': Var('chrome_git') + '/chrome/browser/resources/downloads_internal.git' + '@' +
        '4fefb8a24ae6c421f640b9ef028e4b4ca75df729',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/resources/history/internal': {
      'url': Var('chrome_git') + '/chrome/browser/resources/history/internal.git' + '@' +
        '38176286288ea406972da7b2fd2a391c3ca54050',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/resources/settings/internal': {
      'url': Var('chrome_git') + '/chrome/browser/resources/settings_internal.git' + '@' +
        '9fe49c27bc46e72b437d5b5e24b977ba5893fcaa',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/resources/webui_browser/internal': {
      'url': Var('chrome_git') + '/chrome/browser/resources/webui_browser/internal.git' + '@' +
        '54ba496f51ce3792b7ef8161558985b192ac078b',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/resources/webui_toolbar/internal': {
      'url': Var('chrome_git') + '/chrome/browser/resources/webui_toolbar/internal.git' + '@' +
        'a306270e77c186a116ba17cb40fcf23660ff432d',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/ttc/resources/internal': {
      'url': Var('chrome_git') + '/chrome/browser/ttc/resources/internal.git' + '@' +
        '82613cc46d9c4c58d00897ace17951abc0ed8565',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/spellchecker/internal': {
      'url': Var('chrome_git') + '/chrome/spellchecker/internal.git' + '@' +
        'a22002a5b3cf7c6b872b25712af97a5664a350e2', # from svn revision 24388
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/resources/chromeos/mako/resources': {
    'packages' : [
      {
        'package': 'chromeos_internal/inputs/orca',
        'version': 'CLLNJzExghWGCRzz-vpG8vW2k5B467uuTQhYvBH8pAcC'
      }
    ],
    'condition': 'checkout_chromeos and checkout_src_internal',
    'dep_type': 'cipd',
  },

  'src/chrome/common/request_header_integrity/internal': {
      'url': Var('chrome_git') + '/chrome/browser/request_header_integrity/internal.git' + '@' +
        '1cb2c5fc30aebc581f3ee53cd5cfcf39fa4d895f',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/elevation_service/internal': {
    'url': Var('chrome_git') + '/chrome/elevation_service/internal.git' + '@' +
        '9ac9f6861cfc85fd46748dad79b7aa2d6c0e47f6',
    'condition': 'checkout_src_internal and checkout_win',
  },

  # Installer bits used only by Mac, but mapped for all OSes to ease source
  # grepping.
  'src/chrome/installer/mac/internal': {
      'url': Var('chrome_git') + '/chrome/installer/mac/internal.git' + '@' +
        '18a92d1fc1bb3809a6edfa483a5aad9e9a550f6c',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/test/data/firefox3_profile/searchplugins': {
      'url': Var('chrome_git') + '/chrome/data/osdd/firefox3_profile_searchplugins.git' + '@' +
        '6cf09b86fb9d058453e7d05978ff8e91b5e8e749',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/test/data/firefox3_searchplugins': {
      'url': Var('chrome_git') + '/chrome/data/osdd/firefox3_searchplugins.git' + '@' +
        '490580801915834d72dd8a1e943924c35df45673',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/test/data/gpu/vt': {
      'url': Var('chrome_git') + '/chrome/data/vectortown_endurance/vectortownstatic-20121022.git' + '@' +
        'c34f30f909a414d378a1678eba921e58940708c4',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/test/data/perf/frame_rate/private': {
      'url': Var('chrome_git') + '/chrome/data/frame_rate_tests.git' + '@' +
        '6394c925a272b92a4e9e16d929af049b7aa6e4f8',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/test/data/perf/private': {
      'url': Var('chrome_git') + '/chrome/data/perf_tests.git' + '@' +
        '6f3e320b1fa10910eb1dcbad36afdd1ad00b2c5a',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/test/data/pdf_private': {
      'url': Var('chrome_git') + '/chrome/data/pdf_private.git' + '@' +
        '23b64c03647779d193ee8ccb3f2a1a5560da9c94',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/test/media_router/internal': {
      'url': Var('chrome_git') + '/chrome/test/media_router/internal.git' + '@' +
        '99937b3180700d9fb63eace8c762c7a5977a301f',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/tools/memory': {
      'url': Var('chrome_git') + '/chrome/tools/memory.git' + '@' +
        '3c9359382236f6d57c91505234a2bc7fd635ba6c',
      'condition': 'checkout_win and checkout_src_internal',
  },

  'src/chrome/services/speech/internal': {
      'url': Var('chrome_git') + '/chromeos/speech.git' + '@' + '5517dd8896bf2d66fc8e98986dbc314913fa6574',
      'condition': 'checkout_chromeos and checkout_src_internal',
   },

  'src/chrome/updater/internal': {
    'url': Var('chrome_git') + '/chrome/updater/internal.git' + '@' +
        '5f1216c3c95eb081c1a24a9e7bf65d0096f1e645',
    'condition': 'checkout_src_internal',
  },

  'src/components/accessibility/internal': {
      'url': Var('chrome_git') + '/chrome-accessibility.git' + '@' +
        'a16011cd66bd0a7c80a0034cfa6da5fce54327f7',
      'condition': 'checkout_src_internal',
  },

  'src/components/activity_reporter/internal': {
    'url': Var('chrome_git') + '/activity_reporter_internal.git' + '@' +
        '0540703a9db51619739c3de547961dd2227d2f26',
    'condition': 'checkout_src_internal',
  },

  'src/components/autofill/core/browser/geo/chrome_alternative_state_name_maps': {
      'url': Var('chrome_git') + '/chrome/components/autofill_alternative_state_name_maps.git' + '@' +
        'a2c20aaa74def75d6485ba287340ffb05bbabc09',
      'condition': 'checkout_src_internal',
  },

  'src/components/autofill/core/browser/form_parsing/internal_resources': {
      'url': Var('chrome_git') + '/chrome/components/autofill_regex_patterns.git' + '@' +
        '976c96d021af4588b1138e1028b514f8b39cef23',
      'condition': 'checkout_src_internal',
  },

  'src/components/crash/core/app/internal': {
    'url': Var('chrome_git') + '/chrome/components/crash.git' + '@' + '977755983e64eb91813875407050afcc7c1b6683',
    'condition': 'checkout_src_internal',
  },

  'src/components/internal': {
      'url': Var('chrome_git') + '/chrome/components.git' + '@' +
        'a44c4b3def49aa6eefab3a64eec3dec1e48ce944',
      'condition': 'checkout_src_internal',
  },

  'src/components/metrics/internal': {
      'url': Var('chrome_git') + '/chrome/components/metrics/internal.git' + '@' +
        '6c5b0a009ec24624e40e58045cc107d1c47672c6',
      'condition': 'checkout_src_internal',
  },

  'src/components/ntp_tiles/resources/internal': {
      'url': Var('chrome_git') + '/chrome/components/ntp_tiles/resources.git' + '@' +
        '48c257ae331a9c642af38b8f62cb2c789e2a7da6',
      'condition': 'checkout_src_internal',
  },

  'src/components/optimization_guide/internal': {
      'url': Var('chrome_git') + '/chrome/components/optimization_guide.git' + '@' +
        '6ee7f8edfffae6573f0723efcddb0f38ef1fcdaf',
      'condition': 'checkout_src_internal',
  },

  'src/components/personal_context/core/resources/internal': {
      'url': Var('chrome_git') + '/chrome/components/personal_context/core/resources/internal.git' + '@' +
        'c901bb10e5eead4fb1754cdafd9627c3393f7384',
      'condition': 'checkout_src_internal',
  },

  'src/components/record_replay/internal': {
      'url': Var('chrome_git') + '/chrome/components/record_replay.git' + '@' +
        '0b41acf3313cd01d274d997287e0fec258bd5016',
      'condition': 'checkout_src_internal',
  },

  'src/components/resources/default_100_percent/google_chrome': {
      'url': Var('chrome_git') + '/chrome/components/default_100_percent/google_chrome.git' + '@' +
        '0a1ee5c152fed45a3327765a02f97f93dc57ac9a',
      'condition': 'checkout_src_internal',
  },

  'src/components/resources/default_200_percent/google_chrome': {
      'url': Var('chrome_git') + '/chrome/components/default_200_percent/google_chrome.git' + '@' +
        '9c3d5e1eaf940652d6eb93ec4aa846d5c7f527c0',
      'condition': 'checkout_src_internal',
  },

  'src/components/resources/default_300_percent/google_chrome': {
      'url': Var('chrome_git') + '/chrome/components/default_300_percent/google_chrome.git' + '@' +
        '48ffc6fb9142c31e7a51cb3361a0771034aa50bc',
      'condition': 'checkout_src_internal',
  },

  'src/components/safe_browsing/internal': {
      'url': Var('chrome_git') + '/chrome/components/safe_browsing/internal.git' + '@' +
        'a4e5252de8a9fb866cd79a3616ed638887b943bd',
      'condition': 'checkout_src_internal',
  },

  'src/components/site_isolation/internal': {
      'url': Var('chrome_git') + '/chrome/components/site_isolation.git' + '@' +
        'c0820b79078dbee4b737614f83f5942ea4c0b916',
      'condition': 'checkout_src_internal',
  },

  'src/components/test/data/autofill/heuristics-json/internal': {
      'url': Var('chrome_git') + '/chrome/test/autofill/structured_forms.git' + '@' +
        '9f2940e86cf8cf5fb0e5ca77c448d0c57c8421c2',
      'condition': 'checkout_chromium_autofill_test_dependencies',
  },

  'src/components/test/data/autofill/label-doms/internal': {
      'url': Var('chrome_git') + '/chrome/test/autofill/field_labels.git' + '@' +
        'c67083dbb39d4bd87d86c083fecbe35aa562c777',
      'condition': 'checkout_chromium_autofill_test_dependencies',
  },

  'src/components/vector_icons/google_chrome': {
      'url': Var('chrome_git') + '/chrome/vector_icons/google_chrome.git' + '@' +
        'cda3a7d9464a2325a24a3d62b8448dcd612bffeb',
      'condition': 'checkout_src_internal',
  },

  'src/content/test/data/plugin': {
      'url': Var('chrome_git') + '/chrome/data/chrome_plugin_tests.git' + '@' +
        '3e80d4d08f5421d6bc9340964834ebc903a318aa',
      'condition': 'checkout_src_internal',
  },

  'src/google_apis/internal': {
      'url': Var('chrome_git') + '/chrome/google_apis/internal.git' + '@' +
        '7b404d2cef42e71f92977364e5249511e46f3571',
      'condition': 'checkout_src_internal',
  },

  'src/ios_internal':  {
      'url': Var('chrome_git') + '/chrome/ios_internal.git' + '@' +
        '18a22aff27d4a62ad171eb9cfdfd5a4adddb3223',
      'condition': 'checkout_ios and checkout_src_internal',
  },

  'src/remoting/host/installer/linux/internal': {
      'url': Var('chrome_git') + '/chrome/remoting/host/installer/linux/internal.git' + '@' +
        'e190816de75b14897f1af785eb37d237750460e2',
      'condition': 'checkout_linux and checkout_src_internal',
  },

  'src/remoting/internal': {
      'url': Var('chrome_git') + '/chrome/remoting/internal.git' + '@' +
        'a338c959172c18f64ce54efcc6b92606830ffcb3',
      'condition': 'checkout_src_internal',
  },

  'src/remoting/test/internal': {
      'url': Var('chrome_git') + '/chrome/remoting/test/internal.git' + '@' +
        '34ff3657e2176fc48a57fad555b076a50a409de6',
      'condition': 'checkout_src_internal',
  },

  'src/remoting/tools/internal': {
      'url': Var('chrome_git') + '/chrome/remoting/tools/internal.git' + '@' +
        'acfed9c3a363694f37aadfb5cda4c31109661eb8',
      'condition': 'checkout_src_internal',
  },

  'src/services/shape_detection/internal': {
      'url': Var('chrome_git') + '/chrome/services/shape_detection.git' + '@' +
        'c56fb8783131945365a2ec6a52f3fb8c57a8e8d3',
      'condition': 'checkout_src_internal',
  },

  'src/signing_keys': {
      'url': Var('chrome_git') + '/clank/apptestkey.git' + '@' +
        '5138e684915721cbccbb487ec0764ed05650fcd0',
      'condition': 'checkout_android and checkout_google_internal and checkout_src_internal',
  },

  'src/clusterfuzz-data':{
      'url': Var('chrome_git') + '/chrome/tools/clusterfuzz-data.git' + '@' +
        Var('clusterfuzz_data_revision'),
      'condition': 'checkout_clusterfuzz_data and checkout_src_internal',
  },

  'src/third_party/screen-ai/linux': {
      'packages': [
          {
              'package': 'chromium/third_party/screen-ai/linux',
              'version': Var('screen_ai_linux'),
          },
      ],
      'condition': 'checkout_linux and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/screen-ai/macos_amd64': {
      'packages': [
          {
              'package': 'chromium/third_party/screen-ai/mac-amd64',
              'version': Var('screen_ai_macos_amd64'),
          },
      ],
      'condition': 'checkout_mac',
      'dep_type': 'cipd',
  },

  'src/third_party/screen-ai/macos_arm64': {
      'packages': [
          {
              'package': 'chromium/third_party/screen-ai/mac-arm64',
              'version': Var('screen_ai_macos_arm64'),
          },
      ],
      'condition': 'checkout_mac',
      'dep_type': 'cipd',
  },

  'src/third_party/screen-ai/windows_amd64': {
      'packages': [
          {
              'package': 'chromium/third_party/screen-ai/windows-amd64',
              'version': Var('screen_ai_windows_amd64'),
          },
      ],
      'condition': 'checkout_win',
      'dep_type': 'cipd',
  },

  'src/third_party/screen-ai/windows_386': {
      'packages': [
          {
              'package': 'chromium/third_party/screen-ai/windows-386',
              'version': Var('screen_ai_windows_386'),
          },
      ],
      'condition': 'checkout_win',
      'dep_type': 'cipd',
  },

  'src/third_party/soda': {
      'packages': [
          {
              'package': 'chrome_internal/third_party/soda',
              'version': 'StdK8khsivYZXVo2wZuVMnDN_xrVO2a8HV8kvfJ3emwC',
          },
      ],
      'condition': 'checkout_linux and checkout_soda and checkout_src_internal and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/soda-mac64': {
      'packages': [
          {
              'package': 'chrome_internal/third_party/soda-mac64',
              'version': 'bJ-qwdYVguWT0V24YjNZ7Nw_toipv0YnVuadeX5xozEC',
          },
      ],
      'condition': 'checkout_mac and checkout_soda and checkout_src_internal',
      'dep_type': 'cipd',
  },

  'src/third_party/soda-win32': {
      'packages': [
          {
              'package': 'chrome_internal/third_party/soda-win32',
              'version': '977qxBGTKbe2kY9aQch9OkNJ3YE8Nt2mVjetdpWeM0IC',
          },
      ],
      'condition': 'checkout_win and checkout_soda and checkout_src_internal',
      'dep_type': 'cipd',
  },

  'src/third_party/soda-win64': {
      'packages': [
          {
              'package': 'chrome_internal/third_party/soda-win64',
              'version': '1elz1jfCAzy5tZUNBr8FsovjgFxmtu8jdyA8ay9Ta8UC',
          },
      ],
      'condition': 'checkout_win and checkout_soda and checkout_src_internal',
      'dep_type': 'cipd',
    },

  'src/third_party/widevine/cdm/chromeos': {
      'url': Var('chrome_git') + '/chrome/deps/widevine/cdm/chromeos.git' + '@' +
        'c0c105991677d8ef446cf23989939f2dedece36a',
      'condition': '(checkout_chromeos or checkout_linux) and checkout_src_internal',
  },

  'src/third_party/widevine/cdm/linux': {
      'url': Var('chrome_git') + '/chrome/deps/widevine/cdm/linux.git' + '@' +
        '6327c8373bcf4198d37a94c10d8495b7ffe281ae',
      'condition': 'checkout_linux and checkout_src_internal',
  },

  'src/third_party/widevine/cdm/mac': {
      'url': Var('chrome_git') + '/chrome/deps/widevine/cdm/mac.git' + '@' +
        '75510ce15dcf46354b6cfc6d66caaa41da855423',
      'condition': 'checkout_mac and checkout_src_internal',
    },

  'src/third_party/widevine/cdm/win': {
      'url': Var('chrome_git') + '/chrome/deps/widevine/cdm/win.git' + '@' +
        '96de637e98fcefc5d1cbd2037605c2b452522dea',
      'condition': 'checkout_win and checkout_src_internal',
  },

  'src/third_party/widevine/scripts': {
      'url': Var('chrome_git') + '/chrome/deps/widevine/scripts.git' + '@' +
        '6a0f9d00e79c6453d33dcccaf7c4e701e0eba4e8',
      'condition': 'checkout_src_internal',
  },

  'src/third_party/wix': {
      'url': Var('chrome_git') + '/chrome/deps/wix/v3_5_2519.git' + '@' +
        'd0745e3bc7a7a83b8848d9384ac0178036c1fd04',
      'condition': 'checkout_win and checkout_src_internal',
  },

  'src/tools/perf/data': {
      'url': Var('chrome_git') + '/chrome/tools/perf/data.git' + '@' +
        'c7eaf497f690ee69e832b1530e19877602e65b18',
      'condition': 'checkout_src_internal',
  },

  'src/ui/file_manager/internal': {
      'url': Var('chrome_git') + '/chrome/file_manager.git' + '@' +
        'a84801be1d5ef906cc03db7eeadd25ce0245ce44',
      'condition': '(checkout_chromeos or checkout_linux) and checkout_src_internal',
  },

  'src/ui/webui/internal': {
      'url': Var('chrome_git') + '/chrome/ui-webui-internal.git' + '@' +
        '4afc450a9363ab44f45c3639d0661daa7dbe5bda',
      'condition': 'checkout_chromeos and checkout_src_internal',
  },

  'src/webkit/data/bmp_decoder': {
      'url': Var('chrome_git') + '/chrome/data/bmp_decoder.git' + '@' +
        '595d7702a51c161b9d0e0239c1a44dbfed818162',
      'condition': 'checkout_src_internal',
  },

  'src/webkit/data/ico_decoder': {
      'url': Var('chrome_git') + '/chrome/data/ico_decoder.git' + '@' +
        'aba38604e037bdbeedca9c2780c94502a8a6034d',
      'condition': 'checkout_src_internal',
  },

  'src/webkit/data/test_shell/plugins': {
      'url': Var('chrome_git') + '/chrome/data/webkit_plugin_tests.git' + '@' +
        'e4bd19f95afa6483a54906c2a3e5d329d2d81690',
      'condition': 'checkout_src_internal',
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

  # PartitionAlloc is located at `base/allocator/partition_allocator` but
  # prefers its own include path:
  # `#include "partition_alloc/..."` is prefered to
  # `#include "base/allocator/partition_allocator/src/partition_alloc/..."`.
  "+partition_alloc",
  "-base/allocator/partition_allocator",

  # Everybody can use headers generated by tools/generate_library_loader.
  '+library_loaders',

  '+testing',
  '+third_party/jni_zero',
  '+third_party/google_benchmark/src/include/benchmark/benchmark.h',
  '+third_party/icu/source/common/unicode',
  '+third_party/icu/source/i18n/unicode',
  '+url',

  # Everybody can use support libraries for C++/Rust interop based on Crubit.
  #
  # Actual headers location (under `third_party/rust-toolchain/lib`) should be
  # treated as an implementation detail - code should instead use
  # `third_party/crubit/support/...` as the canonical `#include` path (this is
  # unified across other major Crubit clients).
  '+third_party/crubit/support',

  # Abseil is allowed by default, but some features are banned. See
  # //styleguide/c++/c++-features.md.
  '+third_party/abseil-cpp',
  '-third_party/abseil-cpp/absl/algorithm/container.h',
  '-third_party/abseil-cpp/absl/base/attributes.h',
  '-third_party/abseil-cpp/absl/base/no_destructor.h',
  '-third_party/abseil-cpp/absl/base/nullability.h',
  '-third_party/abseil-cpp/absl/base/throw_delegate.h',
  '-third_party/abseil-cpp/absl/container/btree_map.h',
  '-third_party/abseil-cpp/absl/container/btree_set.h',
  '-third_party/abseil-cpp/absl/container/linked_hash_map.h',
  '-third_party/abseil-cpp/absl/container/linked_hash_set.h',
  '-third_party/abseil-cpp/absl/flags',
  '-third_party/abseil-cpp/absl/functional/any_invocable.h',
  '-third_party/abseil-cpp/absl/functional/bind_back.h',
  '-third_party/abseil-cpp/absl/functional/bind_front.h',
  '-third_party/abseil-cpp/absl/functional/function_ref.h',
  '-third_party/abseil-cpp/absl/hash',
  '+third_party/abseil-cpp/absl/hash/hash_testing.h',
  '-third_party/abseil-cpp/absl/log',
  '-third_party/abseil-cpp/absl/random',
  '-third_party/abseil-cpp/absl/status/statusor.h',
  '-third_party/abseil-cpp/absl/strings',
  '+third_party/abseil-cpp/absl/strings/ascii.h',
  '+third_party/abseil-cpp/absl/strings/cord.h',
  '+third_party/abseil-cpp/absl/strings/str_format.h',
  '-third_party/abseil-cpp/absl/synchronization',
  '-third_party/abseil-cpp/absl/time',
  '-third_party/abseil-cpp/absl/types/any.h',
  '-third_party/abseil-cpp/absl/types/any_span.h',
  '-third_party/abseil-cpp/absl/types/optional.h',
  '-third_party/abseil-cpp/absl/types/optional_ref.h',
  '-third_party/abseil-cpp/absl/types/source_location.h',
  '-third_party/abseil-cpp/absl/types/span.h',
  '-third_party/abseil-cpp/absl/types/variant.h',
  '-third_party/abseil-cpp/absl/utility/utility.h',
]


# checkdeps.py shouldn't check include paths for files in these dirs:
skip_child_includes = [
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
                '-vpython-spec', 'src/vpython.toml',
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
    # Ensure we remove any file from disk that is no longer needed (e.g. after
    # hooks to native GCS deps migration).
    'name': 'remove_stale_files',
    'pattern': '.',
    'action': [
        'python3',
        'src/tools/remove_stale_files.py',
        'src/third_party/test_fonts/test_fonts.tar.gz', # Remove after 20240901
        'src/third_party/node/node_modules.tar.gz', # TODO: Remove after 20241201, see https://crbug.com/351092787
        'src/third_party/tfhub_models', # TODO: Remove after 20241211
        'src/tools/clang/crashreports', # TODO: Remove after 20260401
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
    'name': 'edit_monitor_rollout',
    'pattern': '.',
    'condition': 'host_os == "linux" and (checkout_src_internal or checkout_src_internal_infra)',
    'action': [
        'vpython3',
        'src/internal/tools/edit_monitor/daemon_manager.py',
        'start',
    ],
  },
  {
    # Case-insensitivity for the Win SDK. Must run before win_toolchain below.
    'name': 'ciopfs_linux',
    'pattern': '.',
    'condition': 'checkout_win and host_os == "linux"',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
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
    # Build the clang toolchain from tip-of-tree.
    # Note: On Win, this should run after win_toolchain, as it may use it.
    'name': 'clang_tot',
    'pattern': '.',
    'condition': 'llvm_force_head_revision',
    'action': ['vpython3', 'src/tools/clang/scripts/build.py',
               '--llvm-force-head-revision',
               '--with-android={checkout_android}',
               '--with-fuchsia={checkout_fuchsia}'],
  },
  {
    # Build the rust toolchain from tip-of-tree, against tip-of-tree LLVM.
    # Note: On Win, this should run after win_toolchain, as it may use it.
    'name': 'rust_tot',
    'pattern': '.',
    'condition': 'rust_force_head_revision',
    'action': ['vpython3', 'src/tools/rust/build_rust.py',
               '--rust-force-head-revision',
               '--llvm-force-head-revision',
               '--entire-toolchain',
               '--skip-test'],
  },
  {
    # Update LASTCHANGE.
    'name': 'lastchange',
    'pattern': '.',
    'action': ['python3', 'src/build/util/lastchange.py',
               '-o', 'src/build/util/LASTCHANGE'],
  },
  {
    # Update MINIBRANCH.
    'name': 'minibranch',
    'pattern': '.',
    'action': ['python3', 'src/build/util/lastchange.py',
               '--minibranch-out', 'src/build/util/MINIBRANCH'],
  },
  {
    # Update lastchange_commit_position.h (only for CrOS).
    'name': 'lastchange_commit_position_cros',
    'pattern': '.',
    'condition': 'checkout_chromeos',
    'action': ['python3', 'src/build/util/lastchange.py',
               '-m', 'CHROMIUM',
               '--commit-position-header',
               'src/build/util/LASTCHANGE_commit_position.h'],
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
               '-m', 'DAWN_COMMIT_HASH',
               '-s', 'src/third_party/dawn',
               '--revision', 'src/gpu/webgpu/DAWN_VERSION',
               '--header', 'src/gpu/webgpu/dawn_commit_hash.h'],
  },
  # Pull dsymutil binaries using checked-in hashes.
  {
    'name': 'dsymutil_mac_arm64',
    'pattern': '.',
    'condition': 'host_os == "mac" and host_cpu == "arm64"',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
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
                '--bucket', 'chromium-browser-clang',
                '-s', 'src/tools/clang/dsymutil/bin/dsymutil.x64.sha1',
                '-o', 'src/tools/clang/dsymutil/bin/dsymutil',
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
                '--bucket', 'chromium-browser-clang/rc',
                '-s', 'src/build/toolchain/win/rc/linux64/rc.sha1',
    ]
  },
  {
    'name': 'nw_patch',
    'pattern': '.',
    'action': [
      'python',
      'src/content/nw/tools/patcher.py'
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
                '--all',
                '--platform=linux'
    ],
  },
  {
    'name': 'checkout_telemetry_benchmark_deps',
    'condition': 'checkout_telemetry_dependencies and checkout_win and not skip_wpr_archives_download',
    'pattern': '.',
    'action': [ 'vpython3',
                'src/tools/perf/fetch_benchmark_deps.py',
                '--all',
                '--platform=win'
    ],
  },
  {
    'name': 'checkout_telemetry_benchmark_deps',
    'condition': 'checkout_telemetry_dependencies and checkout_mac and not skip_wpr_archives_download',
    'pattern': '.',
    'action': [ 'vpython3',
                'src/tools/perf/fetch_benchmark_deps.py',
                '--all',
                '--platform=mac'
    ],
  },
  {
    'name': 'checkout_telemetry_benchmark_deps',
    'condition': 'checkout_telemetry_dependencies and checkout_android and not skip_wpr_archives_download',
    'pattern': '.',
    'action': [ 'vpython3',
                'src/tools/perf/fetch_benchmark_deps.py',
                '--all',
                '--jobs=auto',
                '--platform=android'
    ],
  },

  {
    'name': 'Fetch Android AFDO profile',
    'pattern': '.',
    'condition': 'checkout_android and checkout_pgo_profiles',
    'action': [ 'vpython3',
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
    'condition': 'checkout_android and checkout_pgo_profiles',
    'action': [ 'vpython3',
                'src/tools/download_optimization_profile.py',
                '--newest_state=src/chrome/android/profiles/arm.newest.txt',
                '--local_state=src/chrome/android/profiles/arm.local.txt',
                '--output_name=src/chrome/android/profiles/arm.afdo.prof',
                '--gs_url_base=chromeos-prebuilt/afdo-job/llvm',
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
  # Download AFDO profiles for ChromeOS for each architecture.
  {
    'name': 'Fetch ChromeOS AFDO profiles (from Intel Atom cores)',
    'pattern': '.',
    'condition': 'checkout_chromeos or checkout_simplechrome',
    'action': [ 'vpython3',
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
    'action': [ 'vpython3',
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
    'action': [ 'vpython3',
                'src/tools/download_optimization_profile.py',
                '--newest_state=src/chromeos/profiles/arm.afdo.newest.txt',
                '--local_state=src/chromeos/profiles/arm.afdo.local.txt',
                '--output_name=src/chromeos/profiles/arm.afdo.prof',
                '--gs_url_base=chromeos-prebuilt/afdo-job/vetted/release',
    ],
  },
  {
    'name': 'Download Fuchsia SDK',
    'pattern': '.',
    'condition': 'checkout_fuchsia and not checkout_fuchsia_no_hooks',
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
    'name': 'Download Fuchsia internal system images',
    'pattern': '.',
    'condition': 'checkout_fuchsia_internal and checkout_src_internal',
    'action': ['python3',
               'src/build/fuchsia/update_product_bundles.py',
               '{checkout_fuchsia_internal_images}',
               '--internal'],
  },

  {
    'name': 'Create Fuchsia GN sdk build rules when not running update_sdk.py',
    'pattern': '.',
    'condition': 'checkout_fuchsia_no_hooks',
    'action': [
      'python3',
      'src/build/fuchsia/gen_build_defs.py',
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
      '--no-use-remoteexec',
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
      '--no-use-remoteexec',
      '--nogn-gen',
      '--no-shell',
      '--log-level=warning',
      '--cache-dir=src/build/cros_cache/',
      '--use-external-config',
      '--boards={cros_boards}',
      '--download-test-metadata',
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
      '--no-use-remoteexec',
      '--nogn-gen',
      '--no-shell',
      '--log-level=warning',
      '--cache-dir=src/build/cros_cache/',
      '--boards={cros_boards}',
      '--download-test-metadata',
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
      '--no-use-remoteexec',
      '--nogn-gen',
      '--no-shell',
      '--log-level=warning',
      '--cache-dir=src/build/cros_cache/',
      '--boards={cros_boards_with_qemu_images}',
      '--download-vm',
    ],
  },

  # Download PGO profiles.
  {
    'name': 'Fetch PGO profiles for win-arm64',
    'pattern': '.',
    'condition': 'checkout_pgo_profiles and checkout_win',
    'action': [ 'python3',
                'src/tools/update_pgo_profiles.py',
                '--target=win-arm64',
                'update',
                '--gs-url-base=chromium-optimization-profiles/pgo_profiles',
    ],
  },
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
    'condition': 'checkout_pgo_profiles and (checkout_mac or checkout_fuchsia or checkout_ios)',
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
    'name': 'Fetch PGO profiles for android arm32',
    'pattern': '.',
    'condition': 'checkout_pgo_profiles and checkout_android',
    'action': [ 'python3',
                'src/tools/update_pgo_profiles.py',
                '--target=android-arm32',
                'update',
                '--gs-url-base=chromium-optimization-profiles/pgo_profiles',
    ],
  },
  {
    'name': 'Fetch PGO profiles for android-desktop-x64',
    'pattern': '.',
    'condition': 'checkout_pgo_profiles and checkout_android',
    'action': [ 'python3',
                'src/tools/update_pgo_profiles.py',
                '--target=android-desktop-x64',
                'update',
                '--gs-url-base=chromium-optimization-profiles/pgo_profiles',
    ],
  },
  {
    'name': 'Fetch PGO profiles for V8 builtins',
    'pattern': '.',
    # Always download profiles on Android builds. The GN arg `is_official_build`
    # is required to consider the profiles during build time.
    'condition': 'checkout_pgo_profiles',
    'action': [ 'python3',
                'src/v8/tools/builtins-pgo/download_profiles.py',
                'download',
                '--depot-tools',
                'src/third_party/depot_tools',
                '--check-v8-revision',
                '--quiet',
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

  # Configure remote exec cfg files.
  {
    # Use luci_auth if on windows and using chrome-untrusted project
    'name': 'download_and_configure_reclient_cfgs',
    'pattern': '.',
    'condition': 'download_remoteexec_cfg and host_os == "win"',
    'action': ['python3',
               'src/buildtools/reclient_cfgs/configure_reclient_cfgs.py',
               '--rbe_instance',
               Var('rbe_instance'),
               '--reproxy_cfg_template',
               'reproxy.cfg.template',
               '--rewrapper_cfg_project',
               Var('rewrapper_cfg_project'),
               '--use_luci_auth_credshelper',
               '--quiet',
               ],
  },  {
    'name': 'download_and_configure_reclient_cfgs',
    'pattern': '.',
    'condition': 'download_remoteexec_cfg and not host_os == "win"',
    'action': ['python3',
               'src/buildtools/reclient_cfgs/configure_reclient_cfgs.py',
               '--rbe_instance',
               Var('rbe_instance'),
               '--reproxy_cfg_template',
               'reproxy.cfg.template',
               '--rewrapper_cfg_project',
               Var('rewrapper_cfg_project'),
               '--quiet',
               ],
  },
  {
    'name': 'configure_reclient_cfgs',
    'pattern': '.',
    'condition': 'not download_remoteexec_cfg',
    'action': ['python3',
               'src/buildtools/reclient_cfgs/configure_reclient_cfgs.py',
               '--rbe_instance',
               Var('rbe_instance'),
               '--reproxy_cfg_template',
               'reproxy.cfg.template',
               '--rewrapper_cfg_project',
               Var('rewrapper_cfg_project'),
               '--skip_remoteexec_cfg_fetch',
               '--quiet',
               ],
  },
  # Configure Siso for developer builds.
  {
    'name': 'configure_siso',
    'pattern': '.',
    'action': ['python3',
               'src/build/config/siso/configure_siso.py',
               '--rbe_instance',
               Var('rbe_instance'),
               '--reapi_instance',
               Var('reapi_instance'),
               '--reapi_address',
               Var('reapi_address'),
               '--reapi_backend_config_path',
               Var('reapi_backend_config_path'),
               '--credential-helper',
               Var('reapi_credential_helper'),
               ],
  },
  {
    'name': 'libaom_testdata',
    'pattern': '.',
    'condition': 'download_libaom_testdata',
    'action': ['python3',
               'src/third_party/depot_tools/gsutil.py',
               '-q',
               '-m',
               'rsync',
               'gs://aom-test-data',
               'src/third_party/libaom/testdata']
  },
  {
    'name': 'libvpx_testdata',
    'pattern': '.',
    'condition': 'download_libvpx_testdata',
    'action': ['python3',
               'src/third_party/depot_tools/gsutil.py',
               '-q',
               '-m',
               'rsync',
               'gs://downloads.webmproject.org/test_data/libvpx',
               'src/third_party/libvpx/testdata'],
  },
  {
    # Update chromeos/tast_control.gni
    'name': 'tast_control',
    'pattern': '.',
    'action': ['python3', 'src/build/util/tast_control.py',
               '-o', 'src/chromeos/tast_control.gni',
               '-t', 'src/chromeos/tast_control.gni.template',
               '-i', 'src/chromeos/tast_control_disabled_tests.txt',
               '--input-public',
               'src/chromeos/tast_control_disabled_tests_public_builders.txt',
               '-f', 'src/chromeos/tast_control_flaky_tests.txt'],
  },
  {
    # Update chromeos/tast_control_cq_tests.txt
    'name': 'tast_control_cq_tests',
    'pattern': '.',
    'condition': 'checkout_simplechrome',
    'action': ['python3', 'src/build/util/generate_tast_control_cq_tests.py',
               '-o', 'src/chromeos/tast_control_cq_tests.txt',
               '-a', 'src/chromeos/tast_control_additional_cq_tests.txt',
               '-m', 'src/build/cros_cache/chrome-sdk/misc/test_metadata.jsonpb',
               '-b', '{cros_boards}'],
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
  'src/third_party/instrumented_libs',
  'src/third_party/openscreen/src',
  'src/third_party/devtools-frontend/src',
  # clank has its own DEPS file, does not need to be in trybot_analyze_config
  # since the roller does not run tests.
  'src/clank',
  'src/components/optimization_guide/internal',
  'src/ios_internal',

  # WebPageReplay ships with a go toolchain in its DEPS that's necessary for
  # some bots to run it.
  'src/third_party/webpagereplay'
]
