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
./comp -e <file_path>/<file_name>
```
* Compressed file is generated at ```<file_path>``` with extension .compressed
* Decoding :
```sh
./comp -d <file_path>/<file_name>
```
* Decompressed file is generated at ```<file_path>``` with extension .out (change it according to original file).

## Compressed file layout
Compressed files layout is divided into 4 parts : Header, Table, String, Encoded data

1. First 16 bytes makes header with 4 magic bytes ```0x86 0x54 0x57 0x70``` (or ```V69F```), 4 bytes for offset of Table, 4 bytes for offset of String, 4 bytes for offset of Encoded data.
2. Next 256 bytes make the table with ith byte (from table) denoting length of huffman code for ```i``` (can be proved that maximum length is always less than 256, so this fits in a byte).
3. Next part makes concatenated codes for each integer in range ```[0,255]```.
4. Last part is the encoded data.

While decoding, decoder checks that magic bytes are ```V69F``` (very nice file) and proceds to decode if it matches.

## License
[MIT](https://choosealicense.com/licenses/mit/)
