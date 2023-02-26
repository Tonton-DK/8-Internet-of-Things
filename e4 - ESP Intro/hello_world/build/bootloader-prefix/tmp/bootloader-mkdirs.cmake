# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Espressif/frameworks/esp-idf-v5.0.1/components/bootloader/subproject"
  "D:/Git/8-Internet-of-Things/e4 - ESP Intro/hello_world/build/bootloader"
  "D:/Git/8-Internet-of-Things/e4 - ESP Intro/hello_world/build/bootloader-prefix"
  "D:/Git/8-Internet-of-Things/e4 - ESP Intro/hello_world/build/bootloader-prefix/tmp"
  "D:/Git/8-Internet-of-Things/e4 - ESP Intro/hello_world/build/bootloader-prefix/src/bootloader-stamp"
  "D:/Git/8-Internet-of-Things/e4 - ESP Intro/hello_world/build/bootloader-prefix/src"
  "D:/Git/8-Internet-of-Things/e4 - ESP Intro/hello_world/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Git/8-Internet-of-Things/e4 - ESP Intro/hello_world/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Git/8-Internet-of-Things/e4 - ESP Intro/hello_world/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
