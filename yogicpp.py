#!/usr/bin/env python
import sys
import os

class yogicpp(object):

    def __init__(self, walkPath='.', mode='diff', outputPath='.'):
        pass

    def createFileManifest(self):
        pass
  
    def preprocessFile(self, inputFile):
        pass

    def makePatchFile(self):
        pass

    def generateReport(self): 
        pass

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: yogicpp <inputDirectory> <outputFile>") 
