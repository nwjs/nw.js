{
  "targets": [
    {
        "target_name": "buffer_example",
        "sources": [ "buffer_example.cpp" ], 
        "include_dirs" : ["<!(node -e \"require('nan')\")"]
    }
  ]
}