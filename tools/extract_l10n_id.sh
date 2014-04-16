#!/bin/bash

for i in chrome/app/resources/generated_resources_*.xtb; do
  fn=`basename $i`
  lc=${fn#generated_resources_}
  lc=${lc%.xtb}
  outf=content/nw/src/resources/locale/nw_strings_$lc.xtb
  echo output locale: $lc
  cat > $outf <<EOF
<?xml version="1.0" ?>
<!DOCTYPE translationbundle>
<translationbundle lang="$lc">
EOF

  while read id; do
     echo id: $id
     grep $id $i >> $outf
  done < content/nw/src/resources/locale/id_list

  cat >> $outf <<"EOF2"
</translationbundle>
EOF2

done
