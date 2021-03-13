### INSTALLATION

Logiciel compilé sous Visual Studio 2019 Community.

Triplet de compilation : **x64-windows-static** avec le flag **/MT**

------

Liste des librairies utilisés et compilé avec l'outil [VCPKG (gestionnaire de package C++)](https://github.com/Microsoft/vcpkg)

- SDL 2
- Wintoast
- PoDoFo Version 0.96
- GNU LibICONV / GNU LibCharset
- LybCrypto et LibSSL {OpenSSL}
- LibIDN 2
- XXHash
- Google Brotli
- BZip 2
- LZMA
- Zlib
- Zstd
- Freetype
- LibJPEG
- TurboJPEG
- LibPNG16
- LibTIFF

A compiler avec le triplet x64-windows-static et placer les librairies compilés dans le dossier /libs .