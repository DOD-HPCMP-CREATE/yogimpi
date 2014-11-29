#!/usr/bin/env python
import sys
import os
import argparse
import re
import shutil
from pprint import pprint
from AVUtility import AVUtility as AU
import xml.etree.ElementTree as ET

class yogicpp(object):

    supportFile = os.path.dirname(__file__) + '/preprocessor.xml'

    # Initialize preprocessor object with appropriate input and output modes.
    # @arg inputMode The input mode to be used (file, list, directory)
    # @arg inputPath The input path to be used, dependent on the mode.
    # @arg outputPath Where to place the preprocessed file. 
    def __init__(self, inputMode, inputPath, outputPath):
        self.inputMode = inputMode
        self.inputPath = inputPath
        self.outputPath = outputPath 
        if (self.inputPath == self.outputPath):
            raise ValueError("Input path cannot be the same as output path.")
        self.report = ''
        self.definitions = [] 
        self.findSupported()
        self.outputPath = os.path.abspath(self.outputPath)
        if self.inputMode == 'file':
            self.preprocessFile(self.inputPath, self.outputPath)
        else:
            self.preprocessDirectory(self.inputPath, self.outputPath)
    
    def findSupported(self):
        fileTree = ET.parse(yogicpp.supportFile).getroot()
        for entry in fileTree.iterfind('Constant'):
            self.definitions.append(AU._getValidText(entry, True))
        for entry in fileTree.iterfind('Function'):
            self.definitions.append(AU._getValidText(entry, True))

    def preprocessDirectory(self, inputDir, outputDir):
        pass
  
    def preprocessFile(self, inputFile, outputFile):
        if self._isFortranSource(inputFile):
            ihandle = open(inputFile, 'r')
            rawFile = ihandle.readlines()
            ihandle.close()
            for aPattern in self.definitions:
                for i in range(len(rawFile)):
                    mpiString = re.compile(r"(^|_|=|\s|\(|\)|,|\*|\+)(" +\
                                           aPattern +\
                                           r')(\s|,|\)|\()', re.IGNORECASE)
                    rawFile[i] = mpiString.sub(r"\g<1>Yogi\g<2>\g<3>", 
                                               rawFile[i])
                    rawFile[i] = re.sub(r"(\"|')mpif.h(\"|')", "'yogimpif.h'", 
                                        rawFile[i])
        
            ohandle = open(outputFile, 'w')
            ohandle.writelines(rawFile)
        elif self._isCSource(inputFile):
            ihandle = open(inputFile, 'r')
            rawFile = ihandle.readlines()
            ihandle.close()
            for i in range(len(rawFile)):
                rawFile[i] = re.sub("(\"|<)mpi.h(\"|>)", "\"mpitoyogi.h\"", 
                                    rawFile[i])
        
            ohandle = open(outputFile, 'w')
            ohandle.writelines(rawFile)            
    
    def _isFortranSource(self, fileName):
        fExtensions = ['f90', 'for', 'f', 'f77']
        for fe in fExtensions:
            if fileName.endswith('.' + fe.lower()):
                return True
        return False

    def _isCSource(self, fileName):
        fExtensions = ['c', 'h']
        for fe in fExtensions:
            if fileName.endswith('.' + fe.lower()):
                return True
        return False

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='YogiMPI preprocessor')

    parser.add_argument('-m', '--mode',
                        choices=['file', 'directory'],
                        default="file", 
                        help="Input mode (process file, or files in directory)")
    parser.add_argument('--input', '-i',
                        action="store",
                        required=True,
                        help="Input path for preprocessing")
    parser.add_argument('--output', '-o',
                        action="store",
                        required=True,
                        help="Output path")

    configArguments = parser.parse_args()

    preproc = yogicpp(configArguments.mode, configArguments.input,
                      configArguments.output)
