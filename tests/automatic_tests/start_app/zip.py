import zipfile
import os

zip = zipfile.ZipFile(os.path.join('tmp-nw', 'app.nw'), 'w',
                      compression=zipfile.ZIP_DEFLATED)

source_file = ['index.html', 'package.json']

for file in source_file:    
  path = os.path.join('tmp-nw', file)
  zip.write(path, file)

zip.close();
 