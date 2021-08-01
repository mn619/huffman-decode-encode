# Huffman Encode/Decode

Lossless compression with huffman coding.   

## Installation

* Create folder build
```sh
mkdir build
```  
```sh
cd build
```

* From build/ use cmake to install 
```sh
cmake ../
```
```sh
make
```


## Usage
* The executable file comp under build/src/ is used for encoding/decoding
* Encoding :
```sh
comp -e <file_path>/<file_name>
```
* Compressed file is generated at ```<file_path>``` with extension .compressed
* Decoding :
```sh
comp -d <file_path>/<file_name>
```
* Decompressed file is generated at ```<file_path>``` with extension .out (change it according to original file).

## License
[MIT](https://choosealicense.com/licenses/mit/)