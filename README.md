# Cryptographic File System

A FUSE-based file system that encrypts files transparently using strong cryptographic algorithms (AES-256 CBC).

## Overview

This project implements a Cryptographic File System using FUSE (Filesystem in Userspace). It allows users to mount a virtual encrypted filesystem where all data written to disk is automatically encrypted using AES-256 CBC encryption.

## Features

- Transparent encryption and decryption of files
- Support for basic file system operations (read, write, create, delete, etc.)
- Strong cryptographic algorithms (AES-256 CBC)
- Easily extendable and customizable

## Prerequisites

Before running the Cryptographic File System, ensure you have the following installed:

- FUSE (Filesystem in Userspace)
- OpenSSL development libraries
- GCC compiler
- pkg-config

## Installation

1. Clone the repository:

   ```bash
   git clone https://github.com/your-username/Cryptographic-File-System.git
   cd Cryptographic-File-System

make

#### Usage

Environment Setup: Before running the cryptofs executable, ensure to set CRYPTFS_KEY and CRYPTFS_IV environment variables with appropriate encryption keys

export CRYPTFS_KEY="01234567890123456789012345678901"
export CRYPTFS_IV="0123456789012345"
./cryptofs -f /path/to/mount_point



Once the Cryptographic File System is mounted, you can use it like any other file system:

- Create files: `echo "Hello, world!" > /path/to/mount_point/testfile.txt`
- Read files: `cat /path/to/mount_point/testfile.txt`
- List files: `ls /path/to/mount_point`
- Unmount the file system: `fusermount -u /path/to/mount_point`

## Key Management

By default, the project uses hardcoded keys (`key` and `iv`) in the source code. For production use, implement a secure key management strategy such as reading keys from environment variables or integrating with a Key Management Service (KMS).


