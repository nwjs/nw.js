mkdir -p tmp
tmpdir=$PWD/tmp
pushd third_party
find node -iname '*.h' -or -iname '*.gypi' -or -iname gyp -or -iname gyp_addon | xargs tar cf - | (cd $tmpdir; tar xf - )
popd
rm -rf $tmpdir/node/deps/v8
find v8 -iname '*.h' -or -iname '*.gypi' -or -iname gyp -or -iname gyp_addon | xargs tar cf - | (cd $tmpdir/node/deps; tar xf - )
rm -rf $tmpdir/node/deps/v8/build
rm -rf $tmpdir/node/deps/v8/test
rm -rf $tmpdir/node/deps/v8/out
rm -rf $tmpdir/node/deps/npm/node_modules

cat $tmpdir/node/src/node.h | sed -e 's|third_party/node/deps/uv/include/uv.h|uv.h|' > tmp_node.h && mv tmp_node.h $tmpdir/node/src/node.h

pushd tmp
tar czf ../nw-headers-v0.6.1.tar.gz node
popd
