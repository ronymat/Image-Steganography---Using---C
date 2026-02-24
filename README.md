# Image Steganography – Using C

## Project Overview
The Image Steganography project is a C-based implementation that hides and extracts secret text data inside a 24-bit BMP image using the Least Significant Bit (LSB) technique.
This project demonstrates bit-level manipulation, binary file handling, dynamic memory management, and modular programming principles in C while implementing a practical data hiding mechanism.

## Objectives

* Hide secret text files inside BMP images
* Extract hidden data from stego images
* Implement LSB encoding and decoding
* Work with low-level binary file operations
* Strengthen understanding of structured C programming
  
## Technologies Used

* C Programming
* GCC Compiler
* Makefile
* 24-bit BMP Image Format
* Linux / Ubuntu Environment

## Core Concepts

* Bitwise Operations (`&`, `|`, `<<`, `>>`)
* Least Significant Bit (LSB) Encoding
* File Handling (`fopen`, `fread`, `fwrite`, `fseek`)
* Dynamic Memory Allocation (`malloc`, `free`)
* Pointer Manipulation
* Structures and Modular Programming

## Features

* Encode secret text into BMP image
* Decode hidden text from stego image
* Magic string validation for data integrity
* Error handling and input validation
* Clean modular architecture
* Makefile for easy build process

## Project Structure

* main.c → Entry point and argument validation
* encode.c → Encoding logic implementation
* decode.c → Decoding logic implementation
* common.c → Shared utility functions
* common.h → Function declarations
* types.h → Structure definitions
* Makefile → Compilation automation

## How It Works

### Encoding Process

* Validate input arguments
* Copy BMP header (first 54 bytes)
* Encode magic string
* Encode secret file extension
* Encode secret file size
* Encode secret file data
* Generate stego image

### Decoding Process

* Validate stego image
* Skip BMP header
* Verify magic string
* Extract file extension
* Extract file size
* Extract secret data
* Recreate original file

## How to Compile

Using Makefile:
make

## How to Run

Encoding:
./stego -e input.bmp secret.txt output.bmp

Decoding:
./stego -d output.bmp extracted.txt


If you want, I can now give you a **strong 3–4 line resume description** for this project to help in placements.
