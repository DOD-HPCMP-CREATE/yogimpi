# Simple Fortran preprocessor for YogiMPI
import sys
import os

if len(sys.argv) != 3:
    print("Usage: yogifpp <inputFile> <outputFile>") 

inputFile = sys.argv[1]
outputFile = sys.argv[2]

if os.path.exists(sys.argv[1]):
    raise ValueError("Cannot locate input file.")

if os.path.exists(sys.argv[2]):
    raise ValueError("Output file already exists!")

definitionFile = 'mpiftoyogi.h'
defHandle = open(definitionFile, 'r')

