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

# We expect all git dependencies specified in this file to be in sync with git
# submodules (gitlinks).
git_dependencies = 'SYNC'

gclient_gn_args_file = 'src/build/config/gclient_args.gni'
gclient_gn_args = [
  'build_with_chromium',
  'checkout_android',
  'checkout_android_prebuilts_build_tools',
  'checkout_clang_coverage_tools',
  'checkout_copybara',
  'checkout_glic_e2e_tests',
  'checkout_ios_webkit',
  'checkout_mutter',
  'checkout_openxr',
  'checkout_src_internal',
  'checkout_src_internal_infra',
  'cros_boards',
  'cros_boards_with_qemu_images',
  'generate_location_tags',
]


vars = {
  'nw_src_revision': 'd223c4d907a5b34672969ee00b9eee6700d7dd91',
  'nw_v8_revision': '3c5900bf5b0a5a55c2fdc9084b96f44fde09bbde',
  'nw_node_revision': '6076c1be47112f994431fa1b074e61547cc578b5',
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

  # By default checkout the OpenXR loader library only on Windows and Android.
  # The OpenXR backend for VR in Chromium is currently only supported for these
  # platforms, but support for other platforms may be added in the future.
  'checkout_openxr' : 'checkout_win or checkout_android',

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
  'luci_go': 'git_revision:b0488766ec2a119f97c0189ffa5fff13e4da0131',

  # This can be overridden, e.g. with custom_vars, to build clang from HEAD
  # instead of downloading the prebuilt pinned revision.
  'llvm_force_head_revision': False,

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
  'reclient_version': 're_client_version:0.179.0.28341fc7-gomaip',

  # screen-ai CIPD packages
  'screen_ai_linux': 'version:138.06',
  'screen_ai_macos_amd64': 'version:138.06',
  'screen_ai_macos_arm64': 'version:138.06',
  'screen_ai_windows_amd64': 'version:138.06',
  'screen_ai_windows_386': 'version:138.06',

  # siso CIPD package version.
  'siso_version': 'git_revision:c23de742b8483ddc09d8b68ef845efc7f5c6192f',

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
  'src_internal_revision': '87e9a6c12bf53e47654fad919dc90f734e1b08a4',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling Skia
  # and whatever else without interference from each other.
  'skia_revision': 'cbc694239b06ecf694676aba22d5263dbc23ee5e',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling V8
  # and whatever else without interference from each other.
  'v8_revision': 'b07b4e9376489c7f7c0ff2af5eceb4261b3bb784',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling ANGLE
  # and whatever else without interference from each other.
  'angle_revision': '0145c376fadde16390298681252785f98ae90185',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling SwiftShader
  # and whatever else without interference from each other.
  'swiftshader_revision': 'ed01d9931de34d3a6fb4d615050db5080d9cea16',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling PDFium
  # and whatever else without interference from each other.
  'pdfium_revision': '849572b5c41e5bf59dc88bf54c41067faa9b5b00',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling BoringSSL
  # and whatever else without interference from each other.
  'boringssl_revision': '81be8eb2ca225281bb263ac09ece5370d6462a7d',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling Fuchsia sdk
  # and whatever else without interference from each other.
  'fuchsia_version': 'version:28.20250609.4.1',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling google-toolbox-for-mac
  # and whatever else without interference from each other.
  'google_toolbox_for_mac_revision': '42b12f10cd8342f5cb41a1e3e3a2f13fd9943b0d',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling googletest
  # and whatever else without interference from each other.
  'googletest_revision': '35b75a2cba6ef72b7ce2b6b94b05c54ca07df866',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling lighttpd
  # and whatever else without interference from each other.
  'lighttpd_revision': '9dfa55d15937a688a92cbf2b7a8621b0927d06eb',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling lss
  # and whatever else without interference from each other.
  'lss_revision': 'ed31caa60f20a4f6569883b2d752ef7522de51e0',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling breakpad
  # and whatever else without interference from each other.
  'breakpad_revision': '9d1f417714a6883f8d4e345c07802eb79edd2e90',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling freetype
  # and whatever else without interference from each other.
  'freetype_revision': '43940e4cb8fa6fec96cd52669544629c5eef58fa',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling HarfBuzz
  # and whatever else without interference from each other.
  'harfbuzz_revision': '9f83bbbe64654b45ba5bb06927ff36c2e7588495',
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
  'catapult_revision': 'c4f7831fe85d210ed50572e54d0cb1a26ccc401a',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling CrossBench
  # and whatever else without interference from each other.
  'crossbench_revision': '77308ff3c8445656fd104cd80e3bd933b23cb05d',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling libFuzzer
  # and whatever else without interference from each other.
  'libfuzzer_revision': 'e31b99917861f891308269c36a32363b120126bb',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling fuzztest
  # and whatever else without interference from each other.
  'fuzztest_revision': '45a1c3ad5ac3de58c8e9a3f89036e3f954820d4c',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling domato
  # and whatever else without interference from each other.
  'domato_revision': '053714bccbda79cf76dac3fee48ab2b27f21925e',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling devtools-frontend
  # and whatever else without interference from each other.
  'devtools_frontend_revision': 'bc417052ebef6175721d690a4910e717d92181be',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling libprotobuf-mutator
  # and whatever else without interference from each other.
  'libprotobuf-mutator': '7bf98f78a30b067e22420ff699348f084f802e12',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling android_sdk_build-tools_version
  # and whatever else without interference from each other.
  'android_sdk_build-tools_version': 'y3EsZLg4bxPmpW0oYsAHylywNyMnIwPS3kh1VbQLAFAC',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling android_sdk_emulator_version
  # and whatever else without interference from each other.
  'android_sdk_emulator_version': '9lGp8nTUCRRWGMnI_96HcKfzjnxEJKUcfvfwmA3wXNkC',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling android_sdk_platform-tools_version
  # and whatever else without interference from each other.
  'android_sdk_platform-tools_version': 'mjFmRj7k_XR9yj60pYbr9mG38FyEbU5oWdU56bZQ5cwC',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling android_sdk_platforms_version
  # and whatever else without interference from each other.
  'android_sdk_platforms_version': '_YHemUrK49JrE7Mctdf5DDNOHu1VKBx_PTcWnZ-cbOAC',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'dawn_revision': '46b4670bc67cb4f6d34f6ce6a46ba7e1d6059abf',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'quiche_revision': '823662119bac4eb4a2771a1d45a8c00b5c6737ce',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling ink
  # and whatever else without interference from each other.
  'ink_revision': '4e6081ad7052f97df7d77e1d87cea2d70c18a47b',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling ink_stroke_modeler
  # and whatever else without interference from each other.
  'ink_stroke_modeler_revision': 'fe79520c9ad7d2d445d26d3c59fda6fc54eb4d5c',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling ios_webkit
  # and whatever else without interference from each other.
  'ios_webkit_revision': 'f8c0fe750d94b7db23d193c0b1f31858c2537620',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling libexpat
  # and whatever else without interference from each other.
  'libexpat_revision': '69d6c054c1bd5258c2a13405a7f5628c72c177c2',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling jetstream-main
  # and whatever else without interference from each other.
  'jetstream_main_revision': '6947a460f6b55ef5613c36263049ecf74c5ec1cd',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling jetstream-v2.2
  # and whatever else without interference from each other.
  'jetstream_2.2_revision': '2145cedef4ca2777b792cb0059d3400ee2a6153c',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling wuffs
  # and whatever else without interference from each other.
  'wuffs_revision': 'e3f919ccfe3ef542cfc983a82146070258fb57f8',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling crabbyavif
  # and whatever else without interference from each other.
  'crabbyavif_revision': 'eb883022a5886739f07f0241f918e2be97d65ff0',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling Speedometer main
  # and whatever else without interference from each other.
  'speedometer_main_revision': 'ba41f91e480cfd79c97a9d1d70a4c3d42d16c72b',
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
  'nearby_revision': 'fff5c22e3178a633f57e4ad1fb5ad96a6116240a',
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
  'cros_components_revision': '97dc8c7a1df880206cc54d9913a7e9d73677072a',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'result_adapter_revision': 'git_revision:5fb3ca203842fd691cab615453f8e5a14302a1d8',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'libcxxabi_revision':    'e44c3c4560f1742744ef3f9fb4217a5f26ebca1b',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'libunwind_revision':    '5bbf35ae6801f579c523893176789774c0726e22',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'clang_format_revision':    '37f6e68a107df43b7d7e044fd36a13cbae3413f2',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling feed
  # and whatever else without interference from each other.
  'highway_revision': '00fe003dac355b979f36157f9407c7c46448958e',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling ffmpeg
  # and whatever else without interference from each other.
  'ffmpeg_revision': 'dcdd0fa51b65a0b1688ff6b8f0cc81908f09ded2',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling webpagereplay
  # and whatever else without interference from each other.
  'webpagereplay_revision': 'f3397454e39a7c0b35af192e6d8a1896af7bd9ec',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling llvm-libc
  # and whatever else without interference from each other.
  'llvm_libc_revision':    '79a5aa1b7fcbdf3397bc2a08cbd6ef5c302dfb5a',
  # Three lines of non-changing comments so that
  # the commit queue can handle CLs rolling llvm-libc
  # and whatever else without interference from each other.
  'compiler_rt_revision': '2a4f69a118bdc5d03c415de1b9b166b2f1d4084f',

  # If you change this, also update the libc++ revision in
  # //buildtools/deps_revisions.gni.
  'libcxx_revision':       '2c359c239b138a20a03f798e47889448ef131c22',

  # GN CIPD package version.
  'gn_version': 'git_revision:97b68a0bb62b7528bc3491c7949d6804223c2b82',

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
  'perfetto',
]

deps = {
  'src/tools/gyp':
    Var('chromium_git') + '/external/gyp.git' + '@' + 'd61a9397e668fa9843c4aa7da9e79460fe590bfb',

  # NPM dependencies for JavaScript code coverage.
  'src/third_party/js_code_coverage/node_modules': {
    'dep_type': 'gcs',
    'bucket': 'chromium-nodejs',
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
        'generation': 1741221486381719,
        'object_name': '36a164623d03f525e3dfb783a5e9b8a00e98e1ddd2b5cff4e449bd016dd27e50',
        'sha256sum': '36a164623d03f525e3dfb783a5e9b8a00e98e1ddd2b5cff4e449bd016dd27e50',
        'size_bytes': 20781612,
      },
    ],
  },
  'src/build/linux/debian_bullseye_arm64-sysroot': {
    'bucket': 'chrome-linux-sysroot',
    'condition': 'checkout_linux and checkout_arm64 and non_git_source',
    'dep_type': 'gcs',
    'objects': [
      {
        'generation': 1741221484487736,
        'object_name': '2f915d821eec27515c0c6d21b69898e23762908d8d7ccc1aa2a8f5f25e8b7e18',
        'sha256sum': '2f915d821eec27515c0c6d21b69898e23762908d8d7ccc1aa2a8f5f25e8b7e18',
        'size_bytes': 19204088,
      },
    ],
  },
  'src/build/linux/debian_bullseye_armhf-sysroot': {
    'bucket': 'chrome-linux-sysroot',
    'condition': 'checkout_linux and checkout_arm and non_git_source',
    'dep_type': 'gcs',
    'objects': [
      {
        'generation': 1741221481689337,
        'object_name': '47b3a0b161ca011b2b33d4fc1ef6ef269b8208a0b7e4c900700c345acdfd1814',
        'sha256sum': '47b3a0b161ca011b2b33d4fc1ef6ef269b8208a0b7e4c900700c345acdfd1814',
        'size_bytes': 19054416,
      },
    ],
  },
  'src/build/linux/debian_bullseye_i386-sysroot': {
    'bucket': 'chrome-linux-sysroot',
    'condition': 'checkout_linux and (checkout_x86 or checkout_x64) and non_git_source',
    'dep_type': 'gcs',
    'objects': [
      {
        'generation': 1741221485445080,
        'object_name': '63f0e5128b84f7b0421956a4a40affa472be8da0e58caf27e9acbc84072daee7',
        'sha256sum': '63f0e5128b84f7b0421956a4a40affa472be8da0e58caf27e9acbc84072daee7',
        'size_bytes': 20786772,
      },
    ],
  },
  'src/build/linux/debian_bullseye_mips64el-sysroot': {
    'bucket': 'chrome-linux-sysroot',
    'condition': 'checkout_linux and checkout_mips64 and non_git_source',
    'dep_type': 'gcs',
    'objects': [
      {
        'generation': 1741221481819702,
        'object_name': '58f8594905bfe0fa0b7c7a7e882f01725455d07b7161e6539de5169867009b9f',
        'sha256sum': '58f8594905bfe0fa0b7c7a7e882f01725455d07b7161e6539de5169867009b9f',
        'size_bytes': 19896004,
      },
    ],
  },
  'src/build/linux/debian_bullseye_mipsel-sysroot': {
    'bucket': 'chrome-linux-sysroot',
    'condition': 'checkout_linux and checkout_mips and non_git_source',
    'dep_type': 'gcs',
    'objects': [
      {
        'generation': 1741221481662026,
        'object_name': '2098b42d9698f5c8a15683abbf6d424b7f56200bd2488198e15f31554acb391f',
        'sha256sum': '2098b42d9698f5c8a15683abbf6d424b7f56200bd2488198e15f31554acb391f',
        'size_bytes': 19690120,
      },
    ],
  },
  'src/build/linux/debian_bullseye_ppc64el-sysroot': {
    'bucket': 'chrome-linux-sysroot',
    'condition': 'checkout_linux and checkout_ppc and non_git_source',
    'dep_type': 'gcs',
    'objects': [
      {
        'generation': 1741221484110989,
        'object_name': '485f85dde52830594f7b58ad53b9ca8ff6088b397cacb52aff682be5ffd6f198',
        'sha256sum': '485f85dde52830594f7b58ad53b9ca8ff6088b397cacb52aff682be5ffd6f198',
        'size_bytes': 19637392,
      },
    ],
  },
  'src/build/linux/debian_trixie_riscv64-sysroot': {
    'bucket': 'chrome-linux-sysroot',
    'condition': 'checkout_linux and checkout_riscv64 and non_git_source',
    'dep_type': 'gcs',
    'objects': [
      {
        'generation': 1749572829637587,
        'object_name': '5c8ef4067f41a625d81113a6292180acf4ef49a2ffe015c2779123c133b8e250',
        'sha256sum': '5c8ef4067f41a625d81113a6292180acf4ef49a2ffe015c2779123c133b8e250',
        'size_bytes': 20178952,
      },
    ],
  },
  'src/buildtools/win-format': {
    'bucket': 'chromium-clang-format',
    'condition': 'host_os == "win" and non_git_source',
    'dep_type': 'gcs',
    'objects': [
      {
        'object_name': '565cab9c66d61360c27c7d4df5defe1a78ab56d3',
        'sha256sum': '5557943a174e3b67cdc389c10b0ceea2195f318c5c665dd77a427ed01a094557',
        'size_bytes': 3784704,
        'generation': 1738622386314064,
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
        'object_name': '7d46d237f9664f41ef46b10c1392dcb559250f25',
        'sha256sum': '0c3c13febeb0495ef0086509c24605ecae9e3d968ff9669d12514b8a55c7824e',
        'size_bytes': 3204008,
        'generation': 1738622388489334,
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
        'object_name': '8503422f469ae56cc74f0ea2c03f2d872f4a2303',
        'sha256sum': 'dabf93691361e8bd1d07466d67584072ece5c24e2b812c16458b8ff801c33e29',
        'size_bytes': 3212560,
        'generation': 1738622390717009,
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
        'object_name': '79a7b4e5336339c17b828de10d80611ff0f85961',
        'sha256sum': '889266a51681d55bd4b9e02c9a104fa6ee22ecdfa7e8253532e5ea47e2e4cb4a',
        'size_bytes': 3899440,
        'generation': 1738622384130717,
        'output_file': 'clang-format',
      },
    ],
  },
  'src/third_party/data_sharing_sdk': {
      'packages': [
          {
              'package': 'chrome_internal/third_party/google3/data_sharing_sdk',
              'version': 'UIfm6JqoDwxBPA4coc_W6eoc3FQ7wzG6f66DOgsuU3MC',
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
              'object_name': 'fa98c6432de572206bc5519f85e9c96bd518b039',
              'sha256sum': 'fb563633b5bfe2d4307075c54c6bb54664a3b5ec6bc811f5b15742720549007a',
              'size_bytes': 50288755,
              'generation': 1730835522207929,
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
              'object_name': '4c8952a65a1ce7a2e4cff6db68f9b7454c46349f',
              'sha256sum': 'fadb4530fbe6e35ed298848c66102a0aa7d92974789e6222c4eadee26a381e7e',
              'size_bytes': 45672893,
              'generation': 1730835514382259,
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
              'object_name': '0886aa6a146cb5c213cb09b59ed1075982e4cb57',
              'sha256sum': 'd39e2d44d58bb89740b9aca1073959fc92edbdbbe810a5e48448e331cf72c196',
              'size_bytes': 44929037,
              'generation': 1730835518292126,
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
              'object_name': '907d7e104e7389dc74cec7d32527c1db704b7f96',
              'sha256sum': '7447c4ece014aa41fb2ff866c993c708e5a8213a00913cc2ac5049ea3ffc230d',
              'size_bytes': 80511640,
              'generation': 1730835526374028,
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
        'object_name': 'c2085eeb07e1db47d75633a23311b7d2b7b5d1ee',
        'sha256sum': '24f08d6ea505a468362b862591bdebdb8ba5f6b3da73b435fcc85bd1e38cfbca',
        'size_bytes': 9528349,
        'generation': 1748543466441365,
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
        # The Android libclang_rt.builtins libraries are currently only included in the Linux clang package.
        'object_name': 'Linux_x64/clang-llvmorg-21-init-11777-gfd3fecfc-1.tar.xz',
        'sha256sum': '5f69279b3697166facfc354634157e0a8a32fa6e36864200ad8a8f85add3f3f6',
        'size_bytes': 55654416,
        'generation': 1747138963230938,
        'condition': '(host_os == "linux" or checkout_android) and non_git_source',
      },
      {
        'object_name': 'Linux_x64/clang-tidy-llvmorg-21-init-11777-gfd3fecfc-1.tar.xz',
        'sha256sum': '9c9cd089b46f36232d7553d03a0b30cf509f5e42b2113fe8172ba14f905b91df',
        'size_bytes': 13597708,
        'generation': 1747138963497696,
        'condition': 'host_os == "linux" and checkout_clang_tidy and non_git_source',
      },
      {
        'object_name': 'Linux_x64/clangd-llvmorg-21-init-11777-gfd3fecfc-1.tar.xz',
        'sha256sum': 'cf580450a46e262d899c2d48fff2b6ea8f6691a6bcfc0c280f87d8b6da088e5f',
        'size_bytes': 13854788,
        'generation': 1747138963598582,
        'condition': 'host_os == "linux" and checkout_clangd and non_git_source',
      },
      {
        'object_name': 'Linux_x64/llvm-code-coverage-llvmorg-21-init-11777-gfd3fecfc-1.tar.xz',
        'sha256sum': '1a3488e55d62bda48d09b69fb4e5267ac64247258dc51fd9dabdb7fe5d66da4c',
        'size_bytes': 2297996,
        'generation': 1747138964068040,
        'condition': 'host_os == "linux" and checkout_clang_coverage_tools and non_git_source',
      },
      {
        'object_name': 'Linux_x64/llvmobjdump-llvmorg-21-init-11777-gfd3fecfc-1.tar.xz',
        'sha256sum': 'efb127c69ac198284ff3622cd173e9dc4839d0bed33dbcaa96501db169607a2e',
        'size_bytes': 5701128,
        'generation': 1747138963789285,
        'condition': '((checkout_linux or checkout_mac or checkout_android) and host_os == "linux") and non_git_source',
      },
      {
        'object_name': 'Mac/clang-llvmorg-21-init-11777-gfd3fecfc-1.tar.xz',
        'sha256sum': '3d83bd33ae0a0331ba8e23340023ae05174128503d94116cd8a855913fca88c7',
        'size_bytes': 52212572,
        'generation': 1747138966013176,
        'condition': 'host_os == "mac" and host_cpu == "x64"',
      },
      {
        'object_name': 'Mac/clang-mac-runtime-library-llvmorg-21-init-11777-gfd3fecfc-1.tar.xz',
        'sha256sum': '809b6f72718d9bcbf25ad6ab80517f215ed2526435a095ee9bceb5ed40f989a2',
        'size_bytes': 993800,
        'generation': 1747138984414942,
        'condition': 'checkout_mac and not host_os == "mac"',
      },
      {
        'object_name': 'Mac/clang-tidy-llvmorg-21-init-11777-gfd3fecfc-1.tar.xz',
        'sha256sum': 'c1fd7930eadede03470cbc8d38697669c6254069da2f6bd58328a8654d68f70d',
        'size_bytes': 13699584,
        'generation': 1747138966324770,
        'condition': 'host_os == "mac" and host_cpu == "x64" and checkout_clang_tidy',
      },
      {
        'object_name': 'Mac/clangd-llvmorg-21-init-11777-gfd3fecfc-1.tar.xz',
        'sha256sum': '3aa82177f6e03da693d93aa9f8052fa669af1bd897359a20eaf01379db0924b8',
        'size_bytes': 15136432,
        'generation': 1747138966446697,
        'condition': 'host_os == "mac" and host_cpu == "x64" and checkout_clangd',
      },
      {
        'object_name': 'Mac/llvm-code-coverage-llvmorg-21-init-11777-gfd3fecfc-1.tar.xz',
        'sha256sum': '1f8d0d65f9d027707890c28433b0bfe29e32551008e9f3c1b9803318ede2bfc6',
        'size_bytes': 2272500,
        'generation': 1747138966854415,
        'condition': 'host_os == "mac" and host_cpu == "x64" and checkout_clang_coverage_tools',
      },
      {
        'object_name': 'Mac_arm64/clang-llvmorg-21-init-11777-gfd3fecfc-1.tar.xz',
        'sha256sum': '2d5660c50637a7ee6e5501525e7588bb255cdfd48e792b12b86ae7113c31b8ae',
        'size_bytes': 44214476,
        'generation': 1747138986010150,
        'condition': 'host_os == "mac" and host_cpu == "arm64"',
      },
      {
        'object_name': 'Mac_arm64/clang-tidy-llvmorg-21-init-11777-gfd3fecfc-1.tar.xz',
        'sha256sum': 'c17b6d7d112ff54699477d5fc7f5009e7b650d263d127cad4f8406f8df914996',
        'size_bytes': 11838956,
        'generation': 1747138986259606,
        'condition': 'host_os == "mac" and host_cpu == "arm64" and checkout_clang_tidy',
      },
      {
        'object_name': 'Mac_arm64/clangd-llvmorg-21-init-11777-gfd3fecfc-1.tar.xz',
        'sha256sum': 'd6c134df8273fe9c4c2b5ff5786f2ceeb9ed5f251e223e55edbc0cd7aa772e83',
        'size_bytes': 12115024,
        'generation': 1747138986359326,
        'condition': 'host_os == "mac" and host_cpu == "arm64" and checkout_clangd',
      },
      {
        'object_name': 'Mac_arm64/llvm-code-coverage-llvmorg-21-init-11777-gfd3fecfc-1.tar.xz',
        'sha256sum': '82ca9187d1fd5ed14266612339b921d560b1008f92e1719255b755ff882d23e3',
        'size_bytes': 1982036,
        'generation': 1747138986831545,
        'condition': 'host_os == "mac" and host_cpu == "arm64" and checkout_clang_coverage_tools',
      },
      {
        'object_name': 'Win/clang-llvmorg-21-init-11777-gfd3fecfc-1.tar.xz',
        'sha256sum': '39e17b8282b9f1e3cbc6b22d3144696728c867f3ae66694b0125c3ed60755401',
        'size_bytes': 47250032,
        'generation': 1747139012194774,
        'condition': 'host_os == "win"',
      },
      {
        'object_name': 'Win/clang-tidy-llvmorg-21-init-11777-gfd3fecfc-1.tar.xz',
        'sha256sum': '9dec82a917bd55947e39891137ba5c13663ca94d2c102d56eb52b9b176365910',
        'size_bytes': 13492960,
        'generation': 1747139012510054,
        'condition': 'host_os == "win" and checkout_clang_tidy',
      },
      {
        'object_name': 'Win/clang-win-runtime-library-llvmorg-21-init-11777-gfd3fecfc-1.tar.xz',
        'sha256sum': '220abd9ce9a85446a2e7879aab3c1f2f5393665f6b13b067f8cec565ae7d36eb',
        'size_bytes': 2486856,
        'generation': 1747139033547001,
        'condition': 'checkout_win and not host_os == "win"',
      },
      {
        'object_name': 'Win/clangd-llvmorg-21-init-11777-gfd3fecfc-1.tar.xz',
        'sha256sum': '81fa230b6311e3e50147cae3acd6d2c83c1bef13ff46df3df3d580ca911e9d15',
        'size_bytes': 13918432,
        'generation': 1747139013351355,
       'condition': 'host_os == "win" and checkout_clangd',
      },
      {
        'object_name': 'Win/llvm-code-coverage-llvmorg-21-init-11777-gfd3fecfc-1.tar.xz',
        'sha256sum': '611b9689c3d3e80ab7485e698f6b67e0df328b5407f30f177c29fe394b81a13e',
        'size_bytes': 2378584,
        'generation': 1747139013764369,
        'condition': 'host_os == "win" and checkout_clang_coverage_tools',
      },
      {
        'object_name': 'Win/llvmobjdump-llvmorg-21-init-11777-gfd3fecfc-1.tar.xz',
        'sha256sum': '67a05210ee38b2f575df3cc1feb04ed36b4186748c2d769e2201572b413305fe',
        'size_bytes': 5698204,
        'generation': 1747139013159526,
        'condition': '(checkout_linux or checkout_mac or checkout_android) and host_os == "win"',
      },
    ]
  },
  # Update prebuilt Rust toolchain.
  'src/third_party/rust-toolchain': {
    'dep_type': 'gcs',
    'bucket': 'chromium-browser-clang',
    'objects': [
      {
        'object_name': 'Linux_x64/rust-toolchain-4a0969e06dbeaaa43914d2d00b2e843d49aa3886-1-llvmorg-21-init-11777-gfd3fecfc.tar.xz',
        'sha256sum': 'df1573701599cb7d00c1050b71636a436320b8ad47bb09cb46d6e96b3f8ac585',
        'size_bytes': 118529212,
        'generation': 1747160498430964,
        'condition': 'host_os == "linux" and non_git_source',
      },
      {
        'object_name': 'Mac/rust-toolchain-4a0969e06dbeaaa43914d2d00b2e843d49aa3886-1-llvmorg-21-init-11777-gfd3fecfc.tar.xz',
        'sha256sum': '83836493c8a81b212c20e16666c6b918bff28748f4d6685c5107eb7e9d16f6fe',
        'size_bytes': 111691772,
        'generation': 1747160500088595,
        'condition': 'host_os == "mac" and host_cpu == "x64"',
      },
      {
        'object_name': 'Mac_arm64/rust-toolchain-4a0969e06dbeaaa43914d2d00b2e843d49aa3886-1-llvmorg-21-init-11777-gfd3fecfc.tar.xz',
        'sha256sum': 'd975e322a2e5c680b54f27a6545c63c0de2c5367ca6ffefda04de491d6b76553',
        'size_bytes': 102206348,
        'generation': 1747160501743827,
        'condition': 'host_os == "mac" and host_cpu == "arm64"',
      },
      {
        'object_name': 'Win/rust-toolchain-4a0969e06dbeaaa43914d2d00b2e843d49aa3886-1-llvmorg-21-init-11777-gfd3fecfc.tar.xz',
        'sha256sum': '25c51b4c2e0e8b4e974973f0357b12ecb645a0a7a136f9e0a6604e87c21acc7a',
        'size_bytes': 193582440,
        'generation': 1747160503376785,
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
    'condition': '(host_cpu != "arm64" or host_os == "mac") and non_git_source',
    'dep_type': 'cipd',
  },

  'src/content/test/data/gpu/meet_effects_videos': {
    'packages': [
      {
        'package': 'chromium/testing/meet-effects-videos',
        'version': 'version:1.0',
      }
    ],
    'dep_type': 'cipd',
  },
  'src/content/test/data/gpu/meet_effects': {
    'dep_type': 'gcs',
    'bucket': 'chromium-telemetry',
    'objects': [
      {
        'object_name': 'meet-gpu-tests/750908933.tar.gz',
        'sha256sum': '687f07f3963ca30339aae16bd053357a1d632db67dc744539f43bf9c83b137e5',
        'size_bytes': 192267405,
        'generation': 1747334995545331,
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
        'version': 'version:2@httpd2.4.38.php7.3.31.chromium.3',
      },
    ],
    'dep_type': 'cipd',
    'condition': 'checkout_mac or checkout_ios',
  },
  'src/third_party/apache-mac-arm64': {
    'packages': [
      {
        'package': 'infra/3pp/tools/httpd-php/mac-arm64',
        'version': 'version:2@httpd2.4.38.php7.3.31.chromium.3',
      },
    ],
    'dep_type': 'cipd',
    'condition': 'checkout_mac or checkout_ios',
  },

  'src/third_party/apache-linux': {
    'packages': [
      {
        'package': 'infra/3pp/tools/httpd-php/linux-amd64',
        'version': 'version:2@httpd2.4.38.php7.3.31.chromium.3',
      },
    ],
    'dep_type': 'cipd',
    'condition': '(host_os == "linux") and non_git_source',
  },

  'src/third_party/apache-windows-arm64': {
    'packages': [
        {
            'package': 'infra/3pp/tools/httpd-php/windows-arm64',
            'version': 'version:3@httpd2.4.55-php8.2.5.chromium.6',
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

  'src/third_party/cronet_android_mainline_clang/linux-amd64': {
      'packages': [
          {
              'package': 'chromium/third_party/cronet_android_mainline_clang/linux-amd64',
              'version': 'V0dPF9Cc6TYEaE6vsPyJoFO6xD2_NglmR7Gbqrp7d-sC',
          },
      ],
      'condition': 'checkout_android and host_os == "linux"',
      'dep_type': 'cipd',
  },

  'src/android_webview/tools/cts_archive/cipd': {
      'packages': [
          {
              'package': 'chromium/android_webview/tools/cts_archive',
              'version': '8BpUBTnmt5bH3GiqPKpmTWTP-Ie2X1TuUgf4F0IsgVgC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/chrome/browser/resources/preinstalled_web_apps/internal': {
    'url': Var('chrome_git') + '/chrome/components/default_apps.git' + '@' + '656ac90405cdbb7d7ab05c54a1628c6d7418832e',
    'condition': 'checkout_src_internal',
  },

  'src/chrome/installer/mac/third_party/xz/xz': {
      'url': Var('chromium_git') + '/chromium/deps/xz.git' + '@' + '10d236393a338a55830db628356f022a91978b61',
      'condition': 'checkout_mac',
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
          'version': 'ytJ0UbU9gMLUMLRQlmqQpGpOy1dYswI3rOJ0ILnIFbUC',
        },
      ],
  },

  'src/third_party/updater/chrome_mac_universal/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chrome_mac_universal',
          'version': 'gzutuY-G7u8n5746jgmishm8uWjUR070TXdFc23Ea7YC',
        },
      ],
  },

  'src/third_party/updater/chrome_mac_universal_prod/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chrome_mac_universal_prod',
          # 138.0.7194.0
          'version': 'RUzQ5K8MJXrR4gyHEiCfYR2AMjD-zb6b6c242SR6IOkC',
        },
      ],
  },

  # version:2@132.0.6833.0
  'src/third_party/updater/chrome_win_arm64/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chrome_win_arm64',
          'version': 'OoGMyCUHah7D_AOVm4RpVoPOdkGqjFHuSMcoKTZ0rlsC',
        },
      ],
  },

  # version:2@132.0.6833.0
  'src/third_party/updater/chrome_win_x86/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chrome_win_x86',
          'version': 'gqhcrVQFtXAKUu7yEISTlbrBpPaBUnQZ6FdOo5E4HLQC',
        },
      ],
  },

  # version:2@132.0.6833.0
  'src/third_party/updater/chrome_win_x86_64/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chrome_win_x86_64',
          'version': 'koo1i4cAVdOK2-re2VGbx-crCcbTz9h0qumUk1hyEt0C',
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
          # 129.0.6651.0
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
          'version': 'OLbfeePAbqPnFqcPmkFtR-GK8dN3T3NeH2AChZwBHjgC',
        },
      ],
  },

  # A somewhat recent Chromium-branded updater build. (x86_64)
  'src/third_party/updater/chromium_mac_amd64/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chromium_mac_amd64',
          'version': 'zVv93X5XSClxTR1YejkQuBdSpye7JfPS_h6GcH1N4i4C',
        },
      ],
  },

  # A somewhat recent Chromium-branded updater build. (ARM64)
  'src/third_party/updater/chromium_mac_arm64/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chromium_mac_arm64',
          'version': 'Va20qxSst3lq4WfEZlWiwzXCpSo5XbhhuqJXyqzvhF8C',
        },
      ],
  },

  # r1372006
  'src/third_party/updater/chromium_win_arm64/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chromium_win_arm64',
          'version': 'ksRzLnqewvz7P-YMX2e8mxZuDI1hgPtLNCuAXIisXhoC',
        },
      ],
  },

  # r1372046
  'src/third_party/updater/chromium_win_x86/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chromium_win_x86',
          'version': 'tcFWGGE3wmwKDhT_s-9mzPiQVYxMOI1U9sRAhm1_5QcC',
        },
      ],
  },

  # r1372030
  'src/third_party/updater/chromium_win_x86_64/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chromium_win_x86_64',
          'version': 'iXamEEmbStVBV_c-vEG4Xtrda8tTqSRCZ-tmE7VRHb0C',
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
          'version': 'version:2@1477001',
        },
      ],
  },

  'src/third_party/updater/chromium_mac_amd64_sans_iid/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chromium_mac_amd64',
          'version': 'version:2@1477001',
        },
      ],
  },

  'src/third_party/updater/chromium_mac_arm64_sans_iid/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
      'packages': [
        {
          'package': 'chromium/third_party/updater/chromium_mac_arm64',
          'version': 'version:2@1477001',
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

  'src/third_party/enterprise_companion/chromium_linux64/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_linux and non_git_source',
      'packages': [
        {
          'package': 'chromium/third_party/enterprise_companion/chromium_linux64',
          'version': 'iwCwxMXVY0_oAVNP7XtqnJKKL9jfI9WK_Jwv_DL7qjQC',
        },
      ],
  },

  'src/third_party/enterprise_companion/chromium_mac_amd64/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
      'packages': [
        {
          'package': 'chromium/third_party/enterprise_companion/chromium_mac_amd64',
          'version': '2LSXZdphQPwKLxjSeMUhkHedkGFEAK55ymGWiOcnfLQC',
        },
      ],
  },

  'src/third_party/enterprise_companion/chromium_mac_arm64/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
      'packages': [
        {
          'package': 'chromium/third_party/enterprise_companion/chromium_mac_arm64',
          'version': 'vYq1wh1oMhbtZjMiMsX2BtyHqHAyT1EmfrW1kXGNj3YC',
        },
      ],
  },

  'src/third_party/enterprise_companion/chromium_win_x86/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/enterprise_companion/chromium_win_x86',
          'version': 'dnOfSfX6zBCjpn1hA43h0D1uBls4BnJoAkHMDQo85ScC',
        },
      ],
  },

  'src/third_party/enterprise_companion/chromium_win_x86_64/cipd': {
      'dep_type': 'cipd',
      'condition': 'checkout_win',
      'packages': [
        {
          'package': 'chromium/third_party/enterprise_companion/chromium_win_x86_64',
          'version': 'q7xjPQfd7rTuo4OjmjLq2F-wpDXnPsPZKlmzrlFQkYYC',
        },
      ],
  },

  'src/chrome/test/data/autofill/captured_sites/artifacts': {
    'url': Var('chrome_git') + '/chrome/test/captured_sites/autofill.git' + '@' + 'e5aa37b4b290c94d642cbc85e7777de838db86f7',
    'condition': 'checkout_chromium_autofill_test_dependencies',
  },

  'src/chrome/test/data/password/captured_sites/artifacts': {
    'url': Var('chrome_git') + '/chrome/test/captured_sites/password.git' + '@' + '749475b8448a6d9397fae332da7c9f989b386ac4',
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
        'version': 'ReWAdAhFsmTN1Z6g09iwNFqpNxVIeGzINfzDf4EumCUC',
      },
    ],
    'dep_type': 'cipd',
  },

  'src/chrome/test/data/xr/webvr_info':
    Var('chromium_git') + '/external/github.com/toji/webvr.info.git' + '@' + 'c58ae99b9ff9e2aa4c524633519570bf33536248',

  'src/clank': {
    'url': Var('chrome_git') + '/clank/internal/apps.git' + '@' +
    '55ad8668428d4af7ba27030669a0eeaff98fee13',
    'condition': 'checkout_android and checkout_src_internal',
  },

  'src/docs/website': {
    'url': Var('chromium_git') + '/website.git' + '@' + 'a812d22617824ad2cd291e110378ccec5ae7735f',
  },

  'src/ios/third_party/earl_grey2/src': {
      'url': Var('chromium_git') + '/external/github.com/google/EarlGrey.git' + '@' + '3894e319725773683ded248ee6dedea5b29a3189',
      'condition': 'checkout_ios',
  },

  'src/ios/third_party/edo/src': {
      'url': Var('chromium_git') + '/external/github.com/google/eDistantObject.git' + '@' + '38e71ff183d76f702db6966fa7236c98831acd80',
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
      'url': Var('chromium_git') + '/external/github.com/material-components/material-components-ios.git' + '@' + 'b29adefbd8be6d39542d679b8f9189fc466283b7',
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
    Var('chromium_git') + '/chromium/cdm.git' + '@' + 'a4cbc4325e6de42ead733f2af43c08292d0e65a8',

  'src/net/third_party/quiche/src':
    Var('quiche_git') + '/quiche.git' + '@' +  Var('quiche_revision'),

  'src/testing/libfuzzer/fuzzers/wasm_corpus':
    Var('chromium_git') + '/v8/fuzzer_wasm_corpus.git' + '@' +  '1df5e50a45db9518a56ebb42cb020a94a090258b',

  'src/tools/copybara': {
      'packages' : [
          {
              'package': 'infra/3pp/tools/copybara',
              'version': 'dgqNEgpF5OVIfAUisIefJgYrPj2E1KlPd2hC9cN9LfcC',
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

  'src/third_party/accessibility_test_framework/src': {
      'url': Var('chromium_git') + '/external/github.com/google/Accessibility-Test-Framework-for-Android.git' + '@' + '4a764c690353ea136c82f1a696a70bf38d1ef5fe',
  },

  'src/third_party/android_build_tools/protoc/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/android_build_tools/protoc',
              'version': 'ivH_8voaWaRDbk7bDHj8n5YR2IH7sFuenkqy0bVOb2cC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_toolchain/ndk': {
      'packages': [
            {
                'package': 'chromium/third_party/android_toolchain/android_toolchain',
                'version': 'KXOia11cm9lVdUdPlbGLu8sCz6Y4ey_HV2s8_8qeqhgC',
            },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/androidx/cipd': {
    'packages': [
      {
          'package': 'chromium/third_party/androidx',
          'version': 'QYckO8Ulw3I5dZsuBNrg-nCGi9Xh6rVDTCewemHdCgsC',
      },
    ],
    'condition': 'checkout_android and non_git_source',
    'dep_type': 'cipd',
  },

  'src/third_party/androidx_javascriptengine/src': {
      'url': Var('chromium_git') + '/aosp/platform/frameworks/support/javascriptengine/javascriptengine/src.git' + '@' + 'e3b6fc680728040f97442dc29d978ce5df6e76f4',
      'condition': 'checkout_android',
  },

  'src/third_party/android_system_sdk/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/android_system_sdk/public',
              'version': 'Pfb3HDUW_uRir_VVTCYkGhf6bnPPF55NUJO2WXOxIe0C',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_build_tools/aapt2/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/android_build_tools/aapt2',
              'version': '_lNsOL_GGlXLOIMGtrbMOqNd7TQHabaP1q8SlvUpFbMC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_build_tools/apkanalyzer/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/android_build_tools/apkanalyzer',
              'version': 'zx9JRwc9Dxfnzac4GqNIUa7rWsoABWZjl5ePnEIoSWYC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_build_tools/bundletool/cipd': {
      'packages': [
          {
               'package': 'chromium/third_party/android_build_tools/bundletool',
               'version': 'zV93G9_1s5h6x7c2qdcibr0uuQ_5Q2QgcxhkUs9-tOsC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_build_tools/dagger_compiler/cipd': {
      'packages': [
          {
               'package': 'chromium/third_party/android_build_tools/dagger_compiler',
               'version': 'AC0DoTEXQf40KFt7hyCNSEJPrT9Rprw9zsZxNKdw7BQC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_build_tools/error_prone/cipd': {
      'packages': [
          {
               'package': 'chromium/third_party/android_build_tools/error_prone',
               'version': 'KOECi08Fw4fRTVXkKocxg94coB-e_dlcHRokC21yxyoC',
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
               'version': 'RevkTwhOQRk-oOzI4BBk3FYEeVAlBijeTqjNPrGsi_8C',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_build_tools/manifest_merger/cipd': {
      'packages': [
          {
               'package': 'chromium/third_party/android_build_tools/manifest_merger',
               'version': 'v0s5-TwZ9OxfzbqU5ULhQ7vufSuKIIld6Z7TK8artzoC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_build_tools/nullaway/cipd': {
      'packages': [
          {
               'package': 'chromium/third_party/android_build_tools/nullaway',
               'version': 'mFkUqdlIqmMQ3rAaTHFmOBYjNFgFUQOmMlPG6PGgmHYC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_sdk/public': {
      'packages': [
          {
              'package': 'chromium/third_party/android_sdk/public/build-tools/36.0.0',
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
              'package': 'chromium/third_party/android_sdk/public/platforms/android-36',
              'version': Var('android_sdk_platforms_version'),
          },
          {
              'package': 'chromium/third_party/android_sdk/public/cmdline-tools',
              'version': 'gekOVsZjseS1w9BXAT3FsoW__ByGDJYS9DgqesiwKYoC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/angle':
    Var('chromium_git') + '/angle/angle.git' + '@' +  Var('angle_revision'),

  'src/third_party/anonymous_tokens/src':
    Var('chromium_git') + '/external/github.com/google/anonymous-tokens.git' + '@' + 'd708a2602a5947ee068f784daa1594a673d47c4a',

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
    Var('chromium_git') + '/external/github.com/mozilla/readability.git' + '@' + '04fd32f72b448c12b02ba6c40928b67e510bac49',

  'src/third_party/content_analysis_sdk/src':
    Var('chromium_git') + '/external/github.com/chromium/content_analysis_sdk.git' + '@' + '9a408736204513e0e95dd2ab3c08de0d95963efc',

  'src/third_party/dav1d/libdav1d':
    Var('chromium_git') + '/external/github.com/videolan/dav1d.git' + '@' + '63bf075aada99afa112f84c61ddc9cead8ce04d3',

  'src/third_party/dawn':
    Var('dawn_git') + '/dawn.git' + '@' +  Var('dawn_revision'),

  'src/third_party/highway/src':
    Var('chromium_git') + '/external/github.com/google/highway.git' + '@' + Var('highway_revision'),

  'src/third_party/apache-portable-runtime/src': {
      'url': Var('chromium_git') + '/external/apache-portable-runtime.git' + '@' + 'c3f11fcd86b42922834cae91103cf068246c6bb6',
      'condition': 'checkout_android',
  },

  'src/third_party/barhopper': {
      'url': Var('chrome_git') + '/chrome/deps/barhopper.git' + '@' + '9230af4dc38c6d2cc9c0841692267762ebfca991',
      'condition': 'checkout_src_internal and checkout_chromeos',
  },

  'src/third_party/google_benchmark/src':
    Var('chromium_git') + '/external/github.com/google/benchmark.git' + '@' + '761305ec3b33abf30e08d50eb829e19a802581cc',

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
    Var('chromium_git') + '/cast_core/public' + '@' + 'f5ee589bdaea60418f670fa176be15ccb9a34942',

  'src/third_party/catapult':
    Var('chromium_git') + '/catapult.git' + '@' + Var('catapult_revision'),

  'src/third_party/ced/src':
    Var('chromium_git') + '/external/github.com/google/compact_enc_det.git' + '@' + 'ba412eaaacd3186085babcd901679a48863c7dd5',

  'src/third_party/checkstyle/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/checkstyle',
              'version': 'vnbLn0H_kr5nVeziAzIlGqjH1LhxEslL7O0w-UKTHh4C',
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
      'url': Var('chromium_git') + '/chromiumos/chromite.git' + '@' + '341c29acc0a4e66ced602961622ad06a434de62d',
      'condition': 'checkout_chromeos',
  },

  'src/third_party/cld_3/src':
    Var('chromium_git') + '/external/github.com/google/cld_3.git' + '@' + 'b48dc46512566f5a2d41118c8c1116c4f96dc661',

  'src/third_party/colorama/src':
    Var('chromium_git') + '/external/colorama.git' + '@' + '3de9f013df4b470069d03d250224062e8cf15c49',

  'src/third_party/cpu_features/src':
    Var('chromium_git') + '/external/github.com/google/cpu_features.git' + '@' + '936b9ab5515dead115606559502e3864958f7f6e',

  'src/third_party/cpuinfo/src':
    Var('chromium_git') + '/external/github.com/pytorch/cpuinfo.git' + '@' + 'd7427551d6531037da216d20cd36feb19ed4905f',

  'src/third_party/crc32c/src':
    Var('chromium_git') + '/external/github.com/google/crc32c.git' + '@' + 'd3d60ac6e0f16780bcfcc825385e1d338801a558',

  # For Linux and Chromium OS.
  'src/third_party/cros_system_api': {
      'url': Var('chromium_git') + '/chromiumos/platform2/system_api.git' + '@' + '349c5cb547162b967df40a336fc08bb18819a5e1',
      'condition': 'checkout_linux or checkout_chromeos',
  },

  'src/third_party/crossbench':
    Var('chromium_git') + '/crossbench.git' + '@' + Var('crossbench_revision'),


  'src/third_party/depot_tools':
    Var('chromium_git') + '/chromium/tools/depot_tools.git' + '@' + 'ea7a0baff0d8554cf6d38f525b4e7882c2b4ec18',

  'src/third_party/devtools-frontend/src':
    Var('chromium_git') + '/devtools/devtools-frontend' + '@' + Var('devtools_frontend_revision'),

  'src/third_party/dom_distiller_js/dist':
    Var('chromium_git') + '/chromium/dom-distiller/dist.git' + '@' + '199de96b345ada7c6e7e6ba3d2fa7a6911b8767d',

  'src/third_party/dragonbox/src':
    Var('chromium_git') + '/external/github.com/jk-jeon/dragonbox.git' + '@' + '6c7c925b571d54486b9ffae8d9d18a822801cbda',

  'src/third_party/eigen3/src':
    Var('chromium_git') + '/external/gitlab.com/libeigen/eigen.git' + '@' + 'd0b490ee091629068e0c11953419eb089f9e6bb2',

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
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/farmhash/src':
    Var('chromium_git') + '/external/github.com/google/farmhash.git' + '@' + '816a4ae622e964763ca0862d9dbd19324a1eaf45',

  'src/third_party/fast_float/src':
    Var('chromium_git') + '/external/github.com/fastfloat/fast_float.git' + '@' + 'cb1d42aaa1e14b09e1452cfdef373d051b8c02a4',

  'src/third_party/ffmpeg':
    Var('chromium_git') + '/chromium/third_party/ffmpeg.git' + '@' + Var('ffmpeg_revision'),

  'src/third_party/flac':
    Var('chromium_git') + '/chromium/deps/flac.git' + '@' + '689da3a7ed50af7448c3f1961d1791c7c1d9c85c',

  'src/third_party/flatbuffers/src':
    Var('chromium_git') + '/external/github.com/google/flatbuffers.git' + '@' + '8db59321d9f02cdffa30126654059c7d02f70c32',

  # Used for embedded builds. CrOS & Linux use the system version.
  'src/third_party/fontconfig/src': {
      'url': Var('chromium_git') + '/external/fontconfig.git' + '@' + 'c527fe1452d469e5fa1a211180dd40bcdb79fb2a',
      'condition': 'checkout_linux',
  },

  'src/third_party/fp16/src':
    Var('chromium_git') + '/external/github.com/Maratyszcza/FP16.git' + '@' + '0a92994d729ff76a58f692d3028ca1b64b145d91',

  'src/third_party/gemmlowp/src':
    Var('chromium_git') + '/external/github.com/google/gemmlowp.git' + '@' + '13d57703abca3005d97b19df1f2db731607a7dc2',

  'src/third_party/glib/src': {
      'url': Var('chromium_git') + '/external/gitlab.gnome.org/GNOME/glib.git' + '@' + 'bd40fbba132fb3d6e5d785ae9d2b05a9e0da8409',
      'condition': 'checkout_linux and checkout_mutter',
  },

  'src/third_party/gvdb/src': {
      'url': Var('chromium_git') + '/external/gitlab.gnome.org/GNOME/gvdb.git' + '@' + 'b54bc5da25127ef416858a3ad92e57159ff565b3',
      'condition': 'checkout_linux and checkout_mutter',
  },

  'src/third_party/freetype/src':
    Var('chromium_git') + '/chromium/src/third_party/freetype2.git' + '@' + Var('freetype_revision'),

  'src/third_party/fxdiv/src':
    Var('chromium_git') + '/external/github.com/Maratyszcza/FXdiv.git' + '@' + '63058eff77e11aa15bf531df5dd34395ec3017c8',

  'src/third_party/harfbuzz-ng/src':
    Var('chromium_git') + '/external/github.com/harfbuzz/harfbuzz.git' + '@' + Var('harfbuzz_revision'),

  'src/third_party/ink/src':
    Var('chromium_git') + '/external/github.com/google/ink.git' + '@' + Var('ink_revision'),

  'src/third_party/ink_stroke_modeler/src':
    Var('chromium_git') + '/external/github.com/google/ink-stroke-modeler.git' + '@' + Var('ink_stroke_modeler_revision'),

  'src/third_party/instrumented_libs': {
    'url': Var('chromium_git') + '/chromium/third_party/instrumented_libraries.git' + '@' + '69015643b3f68dbd438c010439c59adc52cac808',
    'condition': 'checkout_instrumented_libraries',
  },

  'src/third_party/emoji-segmenter/src':
    Var('chromium_git') + '/external/github.com/google/emoji-segmenter.git' + '@' + Var('emoji_segmenter_revision'),

  'src/third_party/ots/src':
    Var('chromium_git') + '/external/github.com/khaledhosny/ots.git' + '@' + Var('ots_revision'),

  'src/third_party/libgav1/src':
    Var('chromium_git') + '/codecs/libgav1.git' + '@' + 'c05bf9be660cf170d7c26bd06bb42b3322180e58',

  'src/third_party/google_toolbox_for_mac/src': {
      'url': Var('chromium_git') + '/external/github.com/google/google-toolbox-for-mac.git' + '@' + Var('google_toolbox_for_mac_revision'),
      'condition': 'checkout_ios or checkout_mac',
  },

  'src/third_party/google-truth/src': {
      'url': Var('chromium_git') + '/external/github.com/google/truth.git' + '@' + '33387149b465f82712a817e6744847fe136949b3',
      'condition': 'checkout_android',
  },

  'src/third_party/googletest/src':
    Var('chromium_git') + '/external/github.com/google/googletest.git' + '@' + Var('googletest_revision'),

  'src/third_party/gperf': {
      'url': Var('chromium_git') + '/chromium/deps/gperf.git' + '@' + 'd892d79f64f9449770443fb06da49b5a1e5d33c1',
      'condition': 'checkout_win',
  },

  'src/third_party/cardboard/src' : {
      'url': Var('chromium_git') + '/external/github.com/googlevr/cardboard/' + '@' + '8cc8ab8153e82367e32e168a93dd7ec9d6b4509a',
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
              'version': 'AQn4F5NfPAs_GKX-z3OW_Q7-yJ9N6tPrDnmnDScjkTEC',
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
    Var('chromium_git') + '/chromium/deps/hunspell_dictionaries.git' + '@' + '41cdffd71c9948f63c7ad36e1fb0ff519aa7a37e',

  'src/third_party/icu':
    Var('chromium_git') + '/chromium/deps/icu.git' + '@' + 'b929596baebf0ab4ac7ec07f38365db4c50a559d',

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
              'version': 'DWx1sUw2_F3SN9e21bI3W5vGT08eR3HQpXLZy6f-AnwC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/javalang/src': {
      'url': Var('chromium_git') + '/external/github.com/c2nes/javalang.git' + '@' + '0664afb7f4d40254312693f2e833c1ed4ac551c7',
      'condition': 'checkout_android',
  },

  'src/third_party/jdk/current': {
      'packages': [
          {
              'package': 'chromium/third_party/jdk/linux-amd64',
              'version': '2iiuF-nKDH3moTImx2op4WTRetbfhzKoZhH7Xo44zGsC',
          },
      ],
      # Needed on Linux for use on chromium_presubmit (for checkstyle).
      'condition': '(checkout_android or checkout_linux) and non_git_source',
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
      'url': Var('chromium_git') + '/external/junit.git' + '@' + '0eb5ce72848d730da5bd6d42902fdd6a8a42055d',
      'condition': 'checkout_android',
  },

  'src/third_party/kotlin_stdlib/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/kotlin_stdlib',
              'version': 'Ly_nOcmB3VyAmA3kTKonDTj_hmJ8y1ij5aTcLYpERbYC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/kotlinc/current': {
      'packages': [
          {
              'package': 'chromium/third_party/kotlinc',
              'version': 'Z5tCYfpRej0QQvZLXjrGdS4s0SY8h10GnAqpVv05eWsC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/leveldatabase/src':
    Var('chromium_git') + '/external/leveldb.git' + '@' + '4ee78d7ea98330f7d7599c42576ca99e3c6ff9c5',

  'src/third_party/libFuzzer/src':
    Var('chromium_git') + '/external/github.com/llvm/llvm-project/compiler-rt/lib/fuzzer.git' + '@' +  Var('libfuzzer_revision'),

  'src/third_party/fuzztest/src':
    Var('chromium_git') + '/external/github.com/google/fuzztest.git' + '@' + Var('fuzztest_revision'),

  'src/third_party/domato/src':
    Var('chromium_git') + '/external/github.com/googleprojectzero/domato.git' + '@' + Var('domato_revision'),

  'src/third_party/libaddressinput/src':
    Var('chromium_git') + '/external/libaddressinput.git' + '@' + '2610f7b1043d6784ada41392fc9392d1ea09ea07',

  'src/third_party/libaom/source/libaom':
    Var('aomedia_git') + '/aom.git' + '@' +  '0ddc6630b3723b14b164752d46c27752f078ddd3',

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
      'url': Var('chromium_git') + '/external/gitlab.freedesktop.org/emersion/libdisplay-info.git' + '@' + '66b802d05b374cd8f388dc6ad1e7ae4f08cb3300',
      'condition': 'checkout_linux and checkout_mutter',
  },

  # Userspace interface to kernel DRM services.
  'src/third_party/libdrm/src': {
      'url': Var('chromium_git') + '/chromiumos/third_party/libdrm.git' + '@' + 'ad78bb591d02162d3b90890aa4d0a238b2a37cde',
      'condition': 'checkout_linux',
  },

  'src/third_party/libgudev/src': {
      'url': Var('chromium_git') + '/external/gitlab.gnome.org/GNOME/libgudev.git' + '@' + 'df7c9c9940160307aaeb31347f4776a46f8736a9',
      'condition': 'checkout_linux and checkout_mutter',
  },

  'src/third_party/libinput/src': {
      'url': Var('chromium_git') + '/external/gitlab.freedesktop.org/libinput/libinput.git' + '@' + '3aa004b96488bf0b2446c11b97a7f8a75ff37c23',
      'condition': 'checkout_linux and checkout_mutter',
  },

  'src/third_party/expat/src':
    Var('chromium_git') + '/external/github.com/libexpat/libexpat.git' + '@' + Var('libexpat_revision'),

  # The library for IPP protocol (Chrome OS).
  'src/third_party/libipp/libipp': {
      'url': Var('chromium_git') + '/chromiumos/platform2/libipp.git' + '@' + '2209bb84a8e122dab7c02fe66cc61a7b42873d7f',
      'condition': 'checkout_linux',
  },

  'src/third_party/libjpeg_turbo':
    Var('chromium_git') + '/chromium/deps/libjpeg_turbo.git' + '@' + 'e14cbfaa85529d47f9f55b0f104a579c1061f9ad',

  'src/third_party/liblouis/src': {
      'url': Var('chromium_git') + '/external/liblouis-github.git' + '@' + '9700847afb92cb35969bdfcbbfbbb74b9c7b3376',
      'condition': 'checkout_linux',
  },

  'src/third_party/libphonenumber/dist':
    Var('chromium_git') + '/external/libphonenumber.git' + '@' + '9d46308f313f2bf8dbce1dfd4f364633ca869ca7',

  'src/third_party/libprotobuf-mutator/src':
    Var('chromium_git') + '/external/github.com/google/libprotobuf-mutator.git' + '@' +  Var('libprotobuf-mutator'),

  'src/third_party/libsrtp':
    Var('chromium_git') + '/chromium/deps/libsrtp.git' + '@' + 'a52756acb1c5e133089c798736dd171567df11f5',

  # Android Explicit Synchronization.
  'src/third_party/libsync/src': {
      'url': Var('chromium_git') + '/aosp/platform/system/core/libsync.git' + '@' + 'f4f4387b6bf2387efbcfd1453af4892e8982faf6',
      'condition': 'checkout_linux or checkout_android',
  },

  'src/third_party/libunwindstack': {
      'url': Var('chromium_git') + '/chromium/src/third_party/libunwindstack.git' + '@' + '0d758dd57f42564acecdd7a1e7ac5c8521c1b01a',
      'condition': 'checkout_android',
  },

  'src/third_party/libva-fake-driver/src':
    Var('chromium_git') + '/chromiumos/platform/libva-fake-driver.git' + '@' + 'a9bcab9cd6b15d4e3634ca44d5e5f7652c612194',

  'src/third_party/libvpx/source/libvpx':
    Var('chromium_git') + '/webm/libvpx.git' + '@' +  '686bf6f1cde888898498f89ba9aefa66b683566a',

  'src/third_party/libwebm/source':
    Var('chromium_git') + '/webm/libwebm.git' + '@' + 'c4522d6cd68582d66f1adfd24debfa9bee202afa',

  'src/third_party/libwebp/src':
    Var('chromium_git') + '/webm/libwebp.git' + '@' +  '2af6c034ac871c967e04c8c9f8bf2dbc2e271b18',

  'src/third_party/libyuv':
    Var('chromium_git') + '/libyuv/libyuv.git' + '@' + '88798bcd636a93e92d69242da914deb4cec1dfb6',

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
              'version': 'version:3@24.09',
          },
      ],
      'condition': 'checkout_win',
      'dep_type': 'cipd',
  },

  'src/third_party/lzma_sdk/bin/win64': {
      'packages': [
          {
              'package': 'infra/3pp/tools/7z/windows-amd64',
              'version': 'version:3@24.09',
          },
      ],
      'condition': 'checkout_win',
      'dep_type': 'cipd',
  },

  'src/third_party/material_color_utilities/src': {
      'url': Var('chromium_git') + '/external/github.com/material-foundation/material-color-utilities.git' + '@' + '13434b50dcb64a482cc91191f8cf6151d90f5465',
  },

  'src/third_party/material_design_icons/src': {
      'url': Var('chromium_git') + '/external/github.com/google/material-design-icons.git' + '@' +
          '5ab428852e35dc177a8c37a2df9dc9ccf768c65a',
      'condition': 'checkout_ios',
  },

  'src/third_party/microsoft_dxheaders/src': {
    'url': Var('chromium_git') + '/external/github.com/microsoft/DirectX-Headers.git' + '@' + 'd9020edfc69b789653e04fa2d8d10ee3317ffa56',
    'condition': 'checkout_win',
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

  # Graphics buffer allocator for Chrome OS.
  'src/third_party/minigbm/src': {
      'url': Var('chromium_git') + '/chromiumos/platform/minigbm.git' + '@' + '3018207f4d89395cc271278fb9a6558b660885f5',
      'condition': 'checkout_linux',
  },

  'src/third_party/mutter/src': {
      'url': Var('chromium_git') + '/external/gitlab.gnome.org/GNOME/mutter.git' + '@' + 'e99ff5359c4134093efe3cb2aa208e4addf565c6',
      'condition': 'checkout_linux and checkout_mutter',
  },

  'src/third_party/nasm': {
      'url': Var('chromium_git') + '/chromium/deps/nasm.git' + '@' +
      'e2c93c34982b286b27ce8b56dd7159e0b90869a2'
  },

  'src/third_party/neon_2_sse/src':
    Var('chromium_git') + '/external/github.com/intel/ARM_NEON_2_x86_SSE.git' + '@' + 'eb8b80b28f956275e291ea04a7beb5ed8289e872',

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
        'package': 'infra/build/siso/${{platform}}',
        'version': Var('siso_version'),
      }
    ],
    'condition': 'non_git_source',
    'dep_type': 'cipd',
  },

  'src/third_party/openh264/src':
    Var('chromium_git') + '/external/github.com/cisco/openh264' + '@' + '652bdb7719f30b52b08e506645a7322ff1b2cc6f',

  'src/third_party/openscreen/src':
    Var('chromium_git') + '/openscreen' + '@' + 'dd421dc540e75bd4e52388dcb0656d4d96137a73',

  'src/third_party/openxr/src': {
    'url': Var('chromium_git') + '/external/github.com/KhronosGroup/OpenXR-SDK' + '@' + '781f2eab3698d653c804ecbd11e0aed47eaad1c6',
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
    Var('chromium_git') + '/external/github.com/google/perfetto.git' + '@' + '18d4fdc15d027a989db705592585b924f93f1d42',

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
    ],
    'dep_type': 'gcs'
  },

  'src/third_party/perl': {
      'url': Var('chromium_git') + '/chromium/deps/perl.git' + '@' + '8ef97ff3b7332e38e61b347a2fbed425a4617151',
      'condition': 'checkout_win',
  },

  'src/third_party/protobuf-javascript/src':
    Var('chromium_git') + '/external/github.com/protocolbuffers/protobuf-javascript' + '@' + '28bf5df73ef2f345a936d9cc95d64ba8ed426a53',

  'src/third_party/pthreadpool/src':
    Var('chromium_git') + '/external/github.com/google/pthreadpool.git' + '@' + 'dcc9f28589066af0dbd4555579281230abbf74dd',

  # Dependency of skia.
  'src/third_party/pyelftools': {
      'url': Var('chromium_git') + '/chromiumos/third_party/pyelftools.git' + '@' + '19b3e610c86fcadb837d252c794cb5e8008826ae',
      'condition': 'checkout_linux',
  },

  'src/third_party/quic_trace/src':
    Var('chromium_git') + '/external/github.com/google/quic-trace.git' + '@' + 'ed3deb8a056b260c59f2fd42af6dfa3db48a8cad',

  'src/third_party/pywebsocket3/src':
    Var('chromium_git') + '/external/github.com/GoogleChromeLabs/pywebsocket3.git' + '@' + '50602a14f1b6da17e0b619833a13addc6ea78bc2',

  'src/third_party/qemu-linux-arm64': {
      'packages': [
          {
              'package': 'fuchsia/third_party/qemu/linux-arm64',
              'version': 'MDf3sCxn9kct3Tg1oVRHch1hkzO6-9qZZFBRPT6jDuoC'
          },
      ],
      # TODO(b/351926334): Do not add `non_git_source` to this condition until the bug is fixed.
      'condition': 'host_os == "linux" and checkout_fuchsia and checkout_fuchsia_for_arm64_host',
      'dep_type': 'cipd',
  },

  'src/third_party/re2/src':
    Var('chromium_git') + '/external/github.com/google/re2.git' + '@' + 'c84a140c93352cdabbfb547c531be34515b12228',

  'src/third_party/r8/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/r8',
              'version': '9vZCRnASPSjh3_T-2AsumCO_8C5PuBJ5-e84K9b7-80C',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  # This duplication is intentional, so we avoid updating the r8.jar used by
  # dexing unless necessary, since each update invalidates all incremental
  # dexing and unnecessarily slows down all bots.
  'src/third_party/r8/d8/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/r8',
              'version': 'F0pTWc6uG70kZPngypnQxCgD3CgY5D8O-pDqj_qeoQMC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/requests/src': {
      'url': Var('chromium_git') + '/external/github.com/kennethreitz/requests.git' + '@' + 'c7e0fc087ceeadb8b4c84a0953a422c474093d6d',
      'condition': 'checkout_android',
  },

  'src/third_party/robolectric/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/robolectric',
              'version': 'G3VkWqTv1YWDvC6zCrL34iQREzrzdBmSL4GMboAIiAEC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/ruy/src':
    Var('chromium_git') + '/external/github.com/google/ruy.git' + '@' + '83fd40d730feb0804fafbc2d8814bcc19a17b2e5',

  'src/third_party/search_engines_data/resources':
    Var('chromium_git') + '/external/search_engines_data.git' + '@' + '273082bef7b1bc05eddb5079b83702938e40c677',

  'src/third_party/search_engines_data/resources_internal': {
    'url': Var('chrome_git') + '/external/search_engines_data_internal.git' + '@' + 'cc57055d268edc933691dccb2139d0ced064a6c2',
    'condition': 'checkout_src_internal',
  },

  'src/third_party/skia':
    Var('skia_git') + '/skia.git' + '@' +  Var('skia_revision'),

  'src/third_party/smhasher/src':
    Var('chromium_git') + '/external/smhasher.git' + '@' + '0ff96f7835817a27d0487325b6c16033e2992eb5',

  'src/third_party/snappy/src':
    Var('chromium_git') + '/external/github.com/google/snappy.git' + '@' + '32ded457c0b1fe78ceb8397632c416568d6714a0',

  'src/third_party/sqlite/src':
    Var('chromium_git') + '/chromium/deps/sqlite.git' + '@' + 'cc08c79629643fdd5b592f1391e738815f5577b6',

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
              'object_name': '2e9b747b519d133c2cb0ef7f10119b7b023533363db30ef45c93bdeaf6dbc57e',
              'sha256sum': '2e9b747b519d133c2cb0ef7f10119b7b023533363db30ef45c93bdeaf6dbc57e',
              'size_bytes': 79185,
              'generation': 1744292578626605,
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
              'object_name': 'a28b222b79851716f8358d2800157d9ffe117b3545031ae51f69b7e1e1b9a969',
              'sha256sum': 'a28b222b79851716f8358d2800157d9ffe117b3545031ae51f69b7e1e1b9a969',
              'size_bytes': 33233004,
              'generation': 1746454415712520
          },
      ],
  },

  'src/third_party/text-fragments-polyfill/src':
    Var('chromium_git') + '/external/github.com/GoogleChromeLabs/text-fragments-polyfill.git' + '@' + 'c036420683f672d685e27415de0a5f5e85bdc23f',

  'src/third_party/tflite/src':
    Var('chromium_git') + '/external/github.com/tensorflow/tensorflow.git' + '@' + 'e6c5574b82d7950f978447704a70971c478f0f50',

  'src/third_party/turbine/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/turbine',
              'version': 'oD9ecDiZFUZfazjjD0N5yl6G4aJsgODDdNjqIp5m1GwC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/vulkan-deps': '{chromium_git}/vulkan-deps@f227ce323fb5a2fee1a98b6feea54b0e42b2f30d',
  'src/third_party/glslang/src': '{chromium_git}/external/github.com/KhronosGroup/glslang@d176fb41992d5c091fb1c401e4e70306382e67fc',
  'src/third_party/spirv-cross/src': '{chromium_git}/external/github.com/KhronosGroup/SPIRV-Cross@b8fcf307f1f347089e3c46eb4451d27f32ebc8d3',
  'src/third_party/spirv-headers/src': '{chromium_git}/external/github.com/KhronosGroup/SPIRV-Headers@2a611a970fdbc41ac2e3e328802aed9985352dca',
  'src/third_party/spirv-tools/src': '{chromium_git}/external/github.com/KhronosGroup/SPIRV-Tools@33e02568181e3312f49a3cf33df470bf96ef293a',
  'src/third_party/vulkan-headers/src': '{chromium_git}/external/github.com/KhronosGroup/Vulkan-Headers@10739e8e00a7b6f74d22dd0a547f1406ff1f5eb9',
  'src/third_party/vulkan-loader/src': '{chromium_git}/external/github.com/KhronosGroup/Vulkan-Loader@342da33fdec78d269657194c9082835d647d2e68',
  'src/third_party/vulkan-tools/src': '{chromium_git}/external/github.com/KhronosGroup/Vulkan-Tools@e3fc64396755191b3c51e5c57d0454872e7fa487',
  'src/third_party/vulkan-utility-libraries/src': '{chromium_git}/external/github.com/KhronosGroup/Vulkan-Utility-Libraries@72665ee1e50db3d949080df8d727dffa8067f5f8',
  'src/third_party/vulkan-validation-layers/src': '{chromium_git}/external/github.com/KhronosGroup/Vulkan-ValidationLayers@e086a717059f54c94d090998628250ae8f238fd6',

  'src/third_party/vulkan_memory_allocator':
    Var('chromium_git') + '/external/github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git' + '@' + '56300b29fbfcc693ee6609ddad3fdd5b7a449a21',

  'src/third_party/wasm_tts_engine/src': Var('chromium_git') + '/chromium/wasm-tts-engine' + '@' + '352880bb49e2410707543c252ef6b94a21b0f47f',

  # Display server protocol for Linux.
  'src/third_party/wayland/src': {
      'url': Var('chromium_git') + '/external/anongit.freedesktop.org/git/wayland/wayland.git' + '@' + 'a156431ea66fe67d69c9fbba8a8ad34dabbab81c',
      'condition': 'checkout_linux',
  },

  # Wayland protocols that add functionality not available in the core protocol.
  'src/third_party/wayland-protocols/src': {
      'url': Var('chromium_git') + '/external/anongit.freedesktop.org/git/wayland/wayland-protocols.git' + '@' + '7d5a3a8b494ae44cd9651f9505e88a250082765e',
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
    Var('chromium_git') + '/external/github.com/SeleniumHQ/selenium/py.git' + '@' + '1e954903022e9386b9acf452c24f4458dd4c4fc1',

  'src/third_party/webgl/src':
    Var('chromium_git') + '/external/khronosgroup/webgl.git' + '@' + 'c01b768bce4a143e152c1870b6ba99ea6267d2b0',

  'src/third_party/webgpu-cts/src':
    Var('chromium_git') + '/external/github.com/gpuweb/cts.git' + '@' + '2a8d4a83f751286302ce34573409ad75cc318508',

  'src/third_party/webpagereplay':
    Var('chromium_git') + '/webpagereplay.git' + '@' + Var('webpagereplay_revision'),

  'src/third_party/webrtc':
    Var('webrtc_git') + '/src.git' + '@' + '23d8e44f84822170bee4425760b44237959423e5',

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
      'url': Var('chromium_git') + '/external/anongit.freedesktop.org/git/wayland/weston.git' + '@' + 'bdba2f9adaca673fd58339d8140bc04727ee279d',
      'condition': 'checkout_linux',
  },

  'src/third_party/xdg-utils': {
      'url': Var('chromium_git') + '/chromium/deps/xdg-utils.git' + '@' + 'cb54d9db2e535ee4ef13cc91b65a1e2741a94a44',
      'condition': 'checkout_linux',
  },

  'src/third_party/xnnpack/src':
    Var('chromium_git') + '/external/github.com/google/XNNPACK.git' + '@' + '3c99186b3276aa891f94ebba35f6b16e627d57de',

  'src/third_party/libei/cipd': {

    'packages': [
      {
        'package': 'chromium/third_party/libei/linux-amd64',
        'version': 'bPkImucD7usjluBHHjsxfAC9pIK_Z8v8MjmDTgHYV28C',
      },
    ],

    'condition': 'checkout_linux and non_git_source',
    'dep_type': 'cipd',
  },

  'src/third_party/zstd/src':
    Var('chromium_git') + '/external/github.com/facebook/zstd.git' + '@' + 'f9938c217da17ec3e9dcd2a2d99c5cf39536aeb9',

  'src/tools/skia_goldctl/linux': {
      'packages': [
        {
          'package': 'skia/tools/goldctl/linux-amd64',
          'version': 'DuaWI_iN1aKYqw68BNRC2gsLbwarydFdh-0pBp8HS6UC',
        },
      ],
      'dep_type': 'cipd',
      'condition': 'checkout_linux and non_git_source',
  },
  'src/tools/skia_goldctl/win': {
      'packages': [
        {
          'package': 'skia/tools/goldctl/windows-amd64',
          'version': '2KZkcCD9mC9MsaKQTOIECvyDm_gtn0YaOjpfdhXw9wcC',
        },
      ],
      'dep_type': 'cipd',
      'condition': 'checkout_win',
  },

  'src/tools/skia_goldctl/mac_amd64': {
      'packages': [
        {
          'package': 'skia/tools/goldctl/mac-amd64',
          'version': 'kfSAMIADvMhxx5OoeY0EajZqqvBTlkVsrDEiWApdnsMC',
        },
      ],
      'dep_type': 'cipd',
      'condition': 'checkout_mac',
  },

  'src/tools/skia_goldctl/mac_arm64': {
      'packages': [
        {
          'package': 'skia/tools/goldctl/mac-arm64',
          'version': 'TMdajugFqfAjHrtZbG5sFxtFgsHk5GeyYgO7Exhlx20C',
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
        'version': 'UgKA0HNsj2ADAOb8F95SERt5rcJdyYXePvIV5ycV5_cC',
      },
    ],
    'condition': 'checkout_chromeos and checkout_src_internal',
    'dep_type': 'cipd',
  },

  'src/ash/webui/boca_ui/resources/prod': {
    'packages': [
      {
        'package': 'chromeos_internal/apps/boca_app/app',
        'version': '2C2_QHqh5RgEqN162CQfR0FmIGF0MANKrcR9W4CWxv4C',
      },
    ],
    'condition': 'checkout_chromeos and checkout_src_internal',
    'dep_type': 'cipd',
  },

  'src/ash/webui/help_app_ui/resources/prod': {
    'packages': [
      {
        'package': 'chromeos_internal/apps/help_app/app',
        'version': '6RE1LMc8lBy8XK6vNbGHCsuib6l1_5o_R3lh-NyO3s0C',
      },
    ],
    'condition': 'checkout_chromeos and checkout_src_internal',
    'dep_type': 'cipd',
  },

  'src/ash/webui/media_app_ui/resources/prod': {
    'packages': [
      {
        'package': 'chromeos_internal/apps/media_app/app',
        'version': 'jgUYZwoBLwTTXABtFfNVSniwUlmnn4ue-oKLXzkmxFwC',
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
        'version': 'AbrE_oILb_zjITzM-b71AVUd4sYJZCam8HpaxgVNMj8C',
      },
    ],
    'condition': 'checkout_chromeos and checkout_src_internal',
    'dep_type': 'cipd',
  },

  'src/third_party/webpagereplay/cipd': {
      'packages' : [
          {
              'package': 'infra/tools/wpr/linux_x86_64',
              'version': 'y28SfbEF6nHSkZ1eHysM1t711zpOCmtk7jUdxZB-QSMC',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_prebuilts/build_tools': {
      'url': Var('android_git') + '/platform/prebuilts/build-tools.git' + '@' + '3fbcdf38727f967badc13f9537bda0b4392c1f74',
      'condition': 'checkout_android_prebuilts_build_tools',
  },

  'src/third_party/android_deps/autorolled/cipd': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/autorolled',
              'version': 'A3dJOufp8dbBYcfXrWzJNEzSO4qpqACgdZsVLdeWym8C',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  # === ANDROID_DEPS Generated Code Start ===
  # Generated by //third_party/android_deps/fetch_all.py
  'src/third_party/android_deps/cipd/libs/com_android_support_support_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_support_support_annotations',
              'version': 'version:2@28.0.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_android_tools_common': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_tools_common',
              'version': 'version:2@30.2.0-beta01.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_android_tools_layoutlib_layoutlib_api': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_tools_layoutlib_layoutlib_api',
              'version': 'version:2@30.2.0-beta01.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_android_tools_sdk_common': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_android_tools_sdk_common',
              'version': 'version:2@30.2.0-beta01.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_apps_common_testing_accessibility_framework_accessibility_test_framework': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_apps_common_testing_accessibility_framework_accessibility_test_framework',
              'version': 'version:2@4.0.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_datatransport_transport_api': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_datatransport_transport_api',
              'version': 'version:2@4.0.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_datatransport_transport_backend_cct': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_datatransport_transport_backend_cct',
              'version': 'version:2@4.0.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_datatransport_transport_runtime': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_datatransport_transport_runtime',
              'version': 'version:2@4.0.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_gms_play_services_auth': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_auth',
              'version': 'version:2@21.3.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_gms_play_services_auth_api_phone': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_auth_api_phone',
              'version': 'version:2@18.1.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_gms_play_services_auth_base': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_auth_base',
              'version': 'version:2@18.1.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_gms_play_services_auth_blockstore': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_auth_blockstore',
              'version': 'version:2@16.4.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_gms_play_services_base': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_base',
              'version': 'version:2@18.5.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_gms_play_services_basement': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_basement',
              'version': 'version:2@18.5.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_gms_play_services_cast': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_cast',
              'version': 'version:2@22.0.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_gms_play_services_cast_framework': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_cast_framework',
              'version': 'version:2@22.0.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_gms_play_services_clearcut': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_clearcut',
              'version': 'version:2@17.0.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_gms_play_services_cloud_messaging': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_cloud_messaging',
              'version': 'version:2@17.2.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_gms_play_services_fido': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_fido',
              'version': 'version:2@21.1.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_gms_play_services_flags': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_flags',
              'version': 'version:2@18.1.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_gms_play_services_gcm': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_gcm',
              'version': 'version:2@17.0.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_gms_play_services_identity_credentials': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_identity_credentials',
              'version': 'version:2@16.0.0-alpha05.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_gms_play_services_iid': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_iid',
              'version': 'version:2@17.0.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_gms_play_services_instantapps': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_instantapps',
              'version': 'version:2@18.1.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_gms_play_services_location': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_location',
              'version': 'version:2@21.3.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_gms_play_services_phenotype': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_phenotype',
              'version': 'version:2@17.0.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_gms_play_services_stats': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_stats',
              'version': 'version:2@17.1.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_gms_play_services_tasks': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_tasks',
              'version': 'version:2@18.2.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_gms_play_services_vision': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_vision',
              'version': 'version:2@20.1.3.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_gms_play_services_vision_common': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_gms_play_services_vision_common',
              'version': 'version:2@19.1.3.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_libraries_identity_googleid_googleid': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_libraries_identity_googleid_googleid',
              'version': 'version:2@1.1.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_material_material': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_material_material',
              'version': 'version:2@1.13.0-alpha05.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_play_core_common': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_play_core_common',
              'version': 'version:2@2.0.3.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_android_play_feature_delivery': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_android_play_feature_delivery',
              'version': 'version:2@2.1.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_ar_impress': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_ar_impress',
              'version': 'version:2@0.0.2.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_auto_service_auto_service_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_auto_service_auto_service_annotations',
              'version': 'version:2@1.0-rc6.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_auto_value_auto_value_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_auto_value_auto_value_annotations',
              'version': 'version:2@1.11.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_code_findbugs_jsr305': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_code_findbugs_jsr305',
              'version': 'version:2@3.0.2.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_code_gson_gson': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_code_gson_gson',
              'version': 'version:2@2.8.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_dagger_dagger': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_dagger_dagger',
              'version': 'version:2@2.52.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_errorprone_error_prone_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_errorprone_error_prone_annotations',
              'version': 'version:2@2.30.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_firebase_firebase_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_firebase_firebase_annotations',
              'version': 'version:2@16.2.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_firebase_firebase_common': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_firebase_firebase_common',
              'version': 'version:2@21.0.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_firebase_firebase_common_ktx': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_firebase_firebase_common_ktx',
              'version': 'version:2@21.0.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_firebase_firebase_components': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_firebase_firebase_components',
              'version': 'version:2@18.0.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_firebase_firebase_datatransport': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_firebase_firebase_datatransport',
              'version': 'version:2@19.0.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_firebase_firebase_encoders': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_firebase_firebase_encoders',
              'version': 'version:2@17.0.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_firebase_firebase_encoders_json': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_firebase_firebase_encoders_json',
              'version': 'version:2@18.0.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_firebase_firebase_encoders_proto': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_firebase_firebase_encoders_proto',
              'version': 'version:2@16.0.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_firebase_firebase_iid': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_firebase_firebase_iid',
              'version': 'version:2@21.1.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_firebase_firebase_iid_interop': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_firebase_firebase_iid_interop',
              'version': 'version:2@17.1.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_firebase_firebase_installations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_firebase_firebase_installations',
              'version': 'version:2@17.2.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_firebase_firebase_installations_interop': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_firebase_firebase_installations_interop',
              'version': 'version:2@17.1.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_firebase_firebase_measurement_connector': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_firebase_firebase_measurement_connector',
              'version': 'version:2@20.0.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_firebase_firebase_messaging': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_firebase_firebase_messaging',
              'version': 'version:2@24.1.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_guava_failureaccess': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_guava_failureaccess',
              'version': 'version:2@1.0.2.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_guava_guava': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_guava_guava',
              'version': 'version:2@33.3.1-jre.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_guava_guava_android': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_guava_guava_android',
              'version': 'version:2@33.3.1-android.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_j2objc_j2objc_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_j2objc_j2objc_annotations',
              'version': 'version:2@3.0.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_protobuf_protobuf_javalite': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_protobuf_protobuf_javalite',
              'version': 'version:2@4.28.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_google_testparameterinjector_test_parameter_injector': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_google_testparameterinjector_test_parameter_injector',
              'version': 'version:2@1.18.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_googlecode_java_diff_utils_diffutils': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_googlecode_java_diff_utils_diffutils',
              'version': 'version:2@1.3.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_squareup_javapoet': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_squareup_javapoet',
              'version': 'version:2@1.13.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_squareup_moshi_moshi': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_squareup_moshi_moshi',
              'version': 'version:2@1.15.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_squareup_moshi_moshi_adapters': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_squareup_moshi_moshi_adapters',
              'version': 'version:2@1.15.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_squareup_okio_okio_jvm': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_squareup_okio_okio_jvm',
              'version': 'version:2@3.9.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/com_squareup_wire_wire_runtime_jvm': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/com_squareup_wire_wire_runtime_jvm',
              'version': 'version:2@5.2.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/jakarta_inject_jakarta_inject_api': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/jakarta_inject_jakarta_inject_api',
              'version': 'version:2@2.0.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/javax_annotation_javax_annotation_api': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/javax_annotation_javax_annotation_api',
              'version': 'version:2@1.3.2.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/javax_inject_javax_inject': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/javax_inject_javax_inject',
              'version': 'version:2@1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/net_bytebuddy_byte_buddy': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/net_bytebuddy_byte_buddy',
              'version': 'version:2@1.14.12.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/net_bytebuddy_byte_buddy_agent': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/net_bytebuddy_byte_buddy_agent',
              'version': 'version:2@1.14.12.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_bouncycastle_bcprov_jdk18on': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_bouncycastle_bcprov_jdk18on',
              'version': 'version:2@1.78.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_ccil_cowan_tagsoup_tagsoup': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_ccil_cowan_tagsoup_tagsoup',
              'version': 'version:2@1.2.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_checkerframework_checker_compat_qual': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_checkerframework_checker_compat_qual',
              'version': 'version:2@2.5.5.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_checkerframework_checker_qual': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_checkerframework_checker_qual',
              'version': 'version:2@3.43.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_checkerframework_checker_util': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_checkerframework_checker_util',
              'version': 'version:2@3.25.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_codehaus_mojo_animal_sniffer_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_codehaus_mojo_animal_sniffer_annotations',
              'version': 'version:2@1.17.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_conscrypt_conscrypt_openjdk_uber': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_conscrypt_conscrypt_openjdk_uber',
              'version': 'version:2@2.5.2.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_jetbrains_kotlin_kotlin_android_extensions_runtime': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_jetbrains_kotlin_kotlin_android_extensions_runtime',
              'version': 'version:2@1.9.22.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_jetbrains_kotlin_kotlin_parcelize_runtime': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_jetbrains_kotlin_kotlin_parcelize_runtime',
              'version': 'version:2@1.9.22.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_jetbrains_kotlinx_atomicfu_jvm': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_jetbrains_kotlinx_atomicfu_jvm',
              'version': 'version:2@0.23.2.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_jetbrains_kotlinx_kotlinx_coroutines_android': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_jetbrains_kotlinx_kotlinx_coroutines_android',
              'version': 'version:2@1.8.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_jetbrains_kotlinx_kotlinx_coroutines_core_jvm': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_jetbrains_kotlinx_kotlinx_coroutines_core_jvm',
              'version': 'version:2@1.10.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_jetbrains_kotlinx_kotlinx_coroutines_guava': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_jetbrains_kotlinx_kotlinx_coroutines_guava',
              'version': 'version:2@1.8.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_jetbrains_kotlinx_kotlinx_coroutines_play_services': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_jetbrains_kotlinx_kotlinx_coroutines_play_services',
              'version': 'version:2@1.10.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_jetbrains_kotlinx_kotlinx_coroutines_test_jvm': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_jetbrains_kotlinx_kotlinx_coroutines_test_jvm',
              'version': 'version:2@1.7.3.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_jetbrains_kotlinx_kotlinx_serialization_core_jvm': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_jetbrains_kotlinx_kotlinx_serialization_core_jvm',
              'version': 'version:2@1.7.2.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_jsoup_jsoup': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_jsoup_jsoup',
              'version': 'version:2@1.15.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_jspecify_jspecify': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_jspecify_jspecify',
              'version': 'version:2@1.0.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_mockito_mockito_android': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_mockito_mockito_android',
              'version': 'version:2@5.11.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_mockito_mockito_core': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_mockito_mockito_core',
              'version': 'version:2@5.11.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_mockito_mockito_subclass': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_mockito_mockito_subclass',
              'version': 'version:2@5.11.0.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_objenesis_objenesis': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_objenesis_objenesis',
              'version': 'version:2@3.3.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_ow2_asm_asm': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_ow2_asm_asm',
              'version': 'version:2@9.7.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_ow2_asm_asm_analysis': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_ow2_asm_asm_analysis',
              'version': 'version:2@9.7.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_ow2_asm_asm_commons': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_ow2_asm_asm_commons',
              'version': 'version:2@9.7.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_ow2_asm_asm_tree': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_ow2_asm_asm_tree',
              'version': 'version:2@9.7.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_ow2_asm_asm_util': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_ow2_asm_asm_util',
              'version': 'version:2@9.7.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_robolectric_annotations': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_annotations',
              'version': 'version:2@4.14.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_robolectric_junit': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_junit',
              'version': 'version:2@4.14.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_robolectric_nativeruntime': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_nativeruntime',
              'version': 'version:2@4.14.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_robolectric_nativeruntime_dist_compat': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_nativeruntime_dist_compat',
              'version': 'version:2@1.0.16.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_robolectric_pluginapi': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_pluginapi',
              'version': 'version:2@4.14.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_robolectric_plugins_maven_dependency_resolver': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_plugins_maven_dependency_resolver',
              'version': 'version:2@4.14.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_robolectric_resources': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_resources',
              'version': 'version:2@4.14.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_robolectric_robolectric': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_robolectric',
              'version': 'version:2@4.14.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_robolectric_sandbox': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_sandbox',
              'version': 'version:2@4.14.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_robolectric_shadowapi': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_shadowapi',
              'version': 'version:2@4.14.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_robolectric_shadows_framework': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_shadows_framework',
              'version': 'version:2@4.14.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_robolectric_utils': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_utils',
              'version': 'version:2@4.14.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_robolectric_utils_reflector': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_robolectric_utils_reflector',
              'version': 'version:2@4.14.1.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

  'src/third_party/android_deps/cipd/libs/org_yaml_snakeyaml': {
      'packages': [
          {
              'package': 'chromium/third_party/android_deps/libs/org_yaml_snakeyaml',
              'version': 'version:2@2.3.cr1',
          },
      ],
      'condition': 'checkout_android and non_git_source',
      'dep_type': 'cipd',
  },

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
        '79861294509d9038645cd39ca828edd93abaf37b',
      'condition': 'checkout_src_internal and checkout_chromeos',
  },

  'src/chromeos/assistant/internal': {
      'url': Var('chrome_git') + '/chrome/assistant.git' + '@' +
        '7333fc7271a9c673862fcc3b5fd5d054d2e2ab30',
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
        'ad19f22e72e15777d7b631683fd0e1241ac18f15',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/app/theme/default_200_percent/google_chrome': {
      'url': Var('chrome_git') + '/chrome/theme/default_200_percent/google_chrome.git' + '@' +
        'a5391ccb5ac297551c1a7cd82ec66bb4d8887502',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/app/theme/google_chrome': {
      'url': Var('chrome_git') + '/chrome/theme/google_chrome.git' + '@' +
        '88a6504d9c1073afc1919ad1a929e63f8715e186',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/enterprise/connectors/internal': {
      'url': Var('chrome_git') + '/chrome/browser/enterprise/connectors/internal.git' + '@' +
        '7fd7c8dd496740724d0024408ae7a96298e34aa2',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/glic/resources/internal': {
      'url': Var('chrome_git') + '/chrome/browser/glic/resources/internal.git' + '@' +
        '304dea378929869bb5d29dccc19febf41542a9f5',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/glic/e2e_test/internal': {
      'url': Var('chrome_git') + '/chrome/browser/glic/test/internal.git' + '@' +
        '851d0f7cbbe4e97a8edc5b139756b305f509e43f',
      'condition': 'checkout_glic_e2e_tests',
  },

  'src/chrome/browser/google/linkdoctor_internal': {
      'url': Var('chrome_git') + '/chrome/linkdoctor.git' + '@' +
        'fe28a8f90c5471f20f8fee9ff7f6c6f8b8d02bed', # from svn revision 32577
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/internal': {
      'url': Var('chrome_git') + '/chrome/browser_internal.git' + '@' +
        '8cb067a9ea39c3b7f38bad55e4e5e8818319d0b7',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/media/engagement_internal': {
      'url': Var('chrome_git') + '/chrome/browser/media/engagement_internal.git' + '@' +
        '14b00ddbb904612ec8805f00718ae3f95c02a076',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/nearby_sharing/internal': {
      'url': Var('chrome_git') + '/chrome/browser/nearby_sharing/internal.git' + '@' +
        'd46f4456f93aa8d7cd84161ce9fa4663b8068c49',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/platform_experience/win': {
      'url': Var('chrome_git') + '/chrome/browser/platform_experience/win.git' + '@' +
        '1d09642924d28d2596cbe9a48ae590fc45ae624f',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/resources/downloads/internal': {
      'url': Var('chrome_git') + '/chrome/browser/resources/downloads_internal.git' + '@' +
        '4fefb8a24ae6c421f640b9ef028e4b4ca75df729',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/browser/resources/settings/internal': {
      'url': Var('chrome_git') + '/chrome/browser/resources/settings_internal.git' + '@' +
        '3626fa13752d21303b9b1a4546251bf0d187d0c9', # from svn revision 41419
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

  'src/chrome/browser/resources/chromeos/seal/resources': {
    'packages' : [
      {
        'package': 'chromeos_internal/inputs/seal',
        'version': '11AdGL1RBEo2LflLT5Vc8Q3vBfjsHQAuH5jAhUBxL9QC'
      }
    ],
    'condition': 'checkout_chromeos and checkout_src_internal',
    'dep_type': 'cipd',
  },

  'src/chrome/common/request_header_integrity/internal': {
      'url': Var('chrome_git') + '/chrome/browser/request_header_integrity/internal.git' + '@' +
        '1592aa7ba598a048f918f54735bcf056556532c3',
      'condition': 'checkout_src_internal',
  },

  'src/chrome/elevation_service/internal': {
    'url': Var('chrome_git') + '/chrome/elevation_service/internal.git' + '@' +
        'dbffd67b918c1d5b5657e8a4dfd10ee66b57bf48',
    'condition': 'checkout_src_internal and checkout_win',
  },

  # Installer bits used only by Mac, but mapped for all OSes to ease source
  # grepping.
  'src/chrome/installer/mac/internal': {
      'url': Var('chrome_git') + '/chrome/installer/mac/internal.git' + '@' +
        '30e560343a03842393c5913997ba1de268bf83ef',
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
      'url': Var('chrome_git') + '/chromeos/speech.git' + '@' + '990f8c2f45589075ff14084e45bda575d882d78d',
      'condition': 'checkout_chromeos and checkout_src_internal',
   },

  'src/chrome/updater/internal': {
    'url': Var('chrome_git') + '/chrome/updater/internal.git' + '@' +
        '9ae52b67200db02d7d35d2712602c820cee7dea6',
    'condition': 'checkout_src_internal',
  },

  'src/components/accessibility/internal': {
      'url': Var('chrome_git') + '/chrome-accessibility.git' + '@' +
        '2e6c405fd5f09ad9d8933bc531af8c5dd24f348c',
      'condition': 'checkout_src_internal',
  },

  'src/components/autofill/core/browser/form_parsing/internal_resources': {
      'url': Var('chrome_git') + '/chrome/components/autofill_regex_patterns.git' + '@' +
        '103ef5ee6ebd7611908d3685faf5dc42d79b569d',
      'condition': 'checkout_src_internal',
  },

  'src/components/crash/core/app/internal': {
    'url': Var('chrome_git') + '/chrome/components/crash.git' + '@' + '977755983e64eb91813875407050afcc7c1b6683',
    'condition': 'checkout_src_internal',
  },

  'src/components/metrics/internal': {
      'url': Var('chrome_git') + '/chrome/components/metrics/internal.git' + '@' +
        'd74edd43ad58d28a67278699f6756e2482aa6cc1',
      'condition': 'checkout_src_internal',
  },

  'src/components/ntp_tiles/resources/internal': {
      'url': Var('chrome_git') + '/chrome/components/ntp_tiles/resources.git' + '@' +
        '48c257ae331a9c642af38b8f62cb2c789e2a7da6',
      'condition': 'checkout_src_internal',
  },

  'src/components/optimization_guide/internal': {
      'url': Var('chrome_git') + '/chrome/components/optimization_guide.git' + '@' +
        '4b39877398214042ef0e0bf31426ba3bcc51162c',
      'condition': 'checkout_src_internal',
  },

  'src/components/plus_addresses/resources/internal': {
      'url': Var('chrome_git') + '/chrome/components/plus_addresses/resources.git' + '@' +
        '32ee8b65a346a72e8692d112ba9304d6344e3dd6',
      'condition': 'checkout_src_internal',
  },

  'src/components/resources/default_100_percent/google_chrome': {
      'url': Var('chrome_git') + '/chrome/components/default_100_percent/google_chrome.git' + '@' +
        '4d5ab89719e23b96a1468e87c27e98f07daeb478',
      'condition': 'checkout_src_internal',
  },

  'src/components/resources/default_200_percent/google_chrome': {
      'url': Var('chrome_git') + '/chrome/components/default_200_percent/google_chrome.git' + '@' +
        '389fe308d5b5ee7a306726f258169b963797af7e',
      'condition': 'checkout_src_internal',
  },

  'src/components/resources/default_300_percent/google_chrome': {
      'url': Var('chrome_git') + '/chrome/components/default_300_percent/google_chrome.git' + '@' +
        'a047858983ced9b274ccf3a85597774c5b7e041e',
      'condition': 'checkout_src_internal',
  },

  'src/components/site_isolation/internal': {
      'url': Var('chrome_git') + '/chrome/components/site_isolation.git' + '@' +
        'c0820b79078dbee4b737614f83f5942ea4c0b916',
      'condition': 'checkout_src_internal',
  },

  'src/components/test/data/autofill/heuristics-json/internal': {
      'url': Var('chrome_git') + '/chrome/test/autofill/structured_forms.git' + '@' +
        'adc0282015eb4a9fa58b09adb2e1dcd75a522b7f',
      'condition': 'checkout_chromium_autofill_test_dependencies',
  },

  'src/components/test/data/autofill/label-doms/internal': {
      'url': Var('chrome_git') + '/chrome/test/autofill/field_labels.git' + '@' +
        'c67083dbb39d4bd87d86c083fecbe35aa562c777',
      'condition': 'checkout_chromium_autofill_test_dependencies',
  },

  'src/components/vector_icons/google_chrome': {
      'url': Var('chrome_git') + '/chrome/vector_icons/google_chrome.git' + '@' +
        '3d3b47ae1125a3e622a7b33ea7810c2bb83d39f1',
      'condition': 'checkout_src_internal',
  },

  'src/content/test/data/plugin': {
      'url': Var('chrome_git') + '/chrome/data/chrome_plugin_tests.git' + '@' +
        '3e80d4d08f5421d6bc9340964834ebc903a318aa',
      'condition': 'checkout_src_internal',
  },

  'src/google_apis/internal': {
      'url': Var('chrome_git') + '/chrome/google_apis/internal.git' + '@' +
        '02538e2044536d99127760477f6d787d3c4f6ae2',
      'condition': 'checkout_src_internal',
  },

  'src/ios_internal':  {
      'url': Var('chrome_git') + '/chrome/ios_internal.git' + '@' +
        'b99b1a6e42f75e6e1adef8e9e5f74bd59852f258',
      'condition': 'checkout_ios and checkout_src_internal',
  },

  'src/remoting/host/installer/linux/internal': {
      'url': Var('chrome_git') + '/chrome/remoting/host/installer/linux/internal.git' + '@' +
        'e190816de75b14897f1af785eb37d237750460e2',
      'condition': 'checkout_linux and checkout_src_internal',
  },

  'src/remoting/internal': {
      'url': Var('chrome_git') + '/chrome/remoting/internal.git' + '@' +
        'e2527eef826745f968d1e26247d7ba5d5b289012',
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

  'src/signing_keys': {
      'url': Var('chrome_git') + '/clank/apptestkey.git' + '@' +
        '5138e684915721cbccbb487ec0764ed05650fcd0',
      'condition': 'checkout_android and checkout_google_internal and checkout_src_internal',
  },

  'src/skia/tools/clusterfuzz-data':{
      'url': Var('chrome_git') + '/chrome/tools/clusterfuzz-data.git' + '@' +
        'fa1fc4acacddd8d655cfca0bcadef5f7e2259bed',
      'condition': 'checkout_clusterfuzz_data and checkout_src_internal',
  },

  'src/third_party/android_tools_internal': {
      'url': Var('chrome_git') + '/clank/third_party/android_tools.git' + '@' +
        'ab59dfd133386420a319a194c9ac6f5cae802471',
      'condition': 'checkout_android and checkout_src_internal',
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
        'acdae03333e2a0da65745b30f6764f06eec655e4',
      'condition': 'checkout_linux and checkout_src_internal',
  },

  'src/third_party/widevine/cdm/mac': {
      'url': Var('chrome_git') + '/chrome/deps/widevine/cdm/mac.git' + '@' +
        '6d4742c1d3197bd3e8d5884f8620286bd3ac89be',
      'condition': 'checkout_mac and checkout_src_internal',
    },

  'src/third_party/widevine/cdm/win': {
      'url': Var('chrome_git') + '/chrome/deps/widevine/cdm/win.git' + '@' +
        '5a5f896c36cbf8708705f5fbfcc6427a73e7551a',
      'condition': 'checkout_win and checkout_src_internal',
  },

  'src/third_party/widevine/scripts': {
      'url': Var('chrome_git') + '/chrome/deps/widevine/scripts.git' + '@' +
        '6ae793a606aeed0d0f1c6f688117653710137744',
      'condition': 'checkout_src_internal',
  },

  # Only Linux test license server is available.
  'src/third_party/widevine/test/license_server': {
      'url': Var('chrome_git') + '/chrome/deps/widevine/test/license_server.git' + '@' +
        '8b195ed15e73e2ecc9861afb05e6af0b4bdf7413',
      'condition': 'checkout_linux and checkout_src_internal',
  },

  'src/third_party/wix': {
      'url': Var('chrome_git') + '/chrome/deps/wix/v3_5_2519.git' + '@' +
        '1cda03778b09bee24389da73daef3de862da37fc',
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
        '5a3232a478b8afd0e8403fb8c668baf8c9e25ea3',
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
  '+third_party/google_benchmark/src/include/benchmark/benchmark.h',
  '+third_party/icu/source/common/unicode',
  '+third_party/icu/source/i18n/unicode',
  '+url',

  # Abseil is allowed by default, but some features are banned. See
  # //styleguide/c++/c++-features.md.
  '+third_party/abseil-cpp',
  '-third_party/abseil-cpp/absl/algorithm/container.h',
  '-third_party/abseil-cpp/absl/base/attributes.h',
  '-third_party/abseil-cpp/absl/base/no_destructor.h',
  '-third_party/abseil-cpp/absl/base/nullability.h',
  '-third_party/abseil-cpp/absl/container/btree_map.h',
  '-third_party/abseil-cpp/absl/container/btree_set.h',
  '-third_party/abseil-cpp/absl/flags',
  '-third_party/abseil-cpp/absl/functional/any_invocable.h',
  '-third_party/abseil-cpp/absl/functional/bind_front.h',
  '-third_party/abseil-cpp/absl/functional/function_ref.h',
  '-third_party/abseil-cpp/absl/hash',
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
  '-third_party/abseil-cpp/absl/types/optional.h',
  '-third_party/abseil-cpp/absl/types/span.h',
  '-third_party/abseil-cpp/absl/types/variant.h',
  '-third_party/abseil-cpp/absl/utility/utility.h',
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
    # Update LASTCHANGE.
    'name': 'lastchange',
    'pattern': '.',
    'action': ['python3', 'src/build/util/lastchange.py',
               '-o', 'src/build/util/LASTCHANGE'],
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
    'name': 'apache_win32',
    'pattern': '\\.sha1',
    'condition': 'checkout_win',
    'action': [ 'python3',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--directory',
                '--recursive',
                '--num_threads=16',
                '--bucket', 'chromium-apache-win32',
                'src/third_party/apache-win32',
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

  # Pull down WPR Archive files
  {
    'name': 'Fetch WPR archive files',
    'pattern': '.',
    'condition': 'checkout_android and checkout_wpr_archives',
    'action': [ 'python3',
                'src/chrome/test/data/android/manage_wpr_archives.py',
                'download',
    ],
  },
  # Download only WPR binary dependencies from Telemetry via conditionals
  {
    'name': 'checkout_wpr_binary_dependencies',
    'condition': 'checkout_chromium_autofill_test_dependencies or checkout_chromium_password_manager_test_dependencies',
    'pattern': '.',
    'action': [ 'vpython3',
                'src/third_party/catapult/telemetry/bin/fetch_wpr_binary_dependencies',
    ],
  },
  {
    'name': 'Fetch Android AFDO profile',
    'pattern': '.',
    'condition': 'checkout_android and checkout_pgo_profiles',
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
    'condition': 'checkout_android and checkout_pgo_profiles',
    'action': [ 'python3',
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
    'name': 'Fetch PGO profiles for android arm64',
    'pattern': '.',
    'condition': 'checkout_pgo_profiles and checkout_android',
    'action': [ 'python3',
                'src/tools/update_pgo_profiles.py',
                '--target=android-arm64',
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

  # Configure remote exec cfg files
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
               'src/chromeos/tast_control_disabled_tests_public_builders.txt'],
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
]
