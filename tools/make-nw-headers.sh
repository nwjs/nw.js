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

for h in env.h env-inl.h node.h; do
cat $tmpdir/node/src/$h | sed -e 's|third_party/node/deps/uv/include/uv.h|uv.h|' > tmp_$h && mv tmp_$h $tmpdir/node/src/$h
done

pushd tmp
tar czf ../nw-headers-v0.11.0-rc1.tar.gz node
popd
