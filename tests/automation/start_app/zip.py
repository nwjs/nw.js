import zipfile
import os
curDir = os.path.dirname(os.path.abspath(__file__))
zip = zipfile.ZipFile(os.path.join(curDir, 'tmp-nw', 'app.nw'), 'w',
                      compression=zipfile.ZIP_DEFLATED)

source_file = ['index.html', 'package.json']

for file in source_file:    
  path = os.path.join(curDir, 'tmp-nw', file)
  zip.write(path, file)

zip.close();
 
