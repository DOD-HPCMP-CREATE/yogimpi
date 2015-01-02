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

    # Initialize preprocessor object with appropriate input and output modes.
    # @arg inputMode The input mode to be used (file or directory)
    # @arg inputPath The input path to be used, dependent on the mode.
    # @arg outputPath Where to place the preprocessed file(s). 
    # @arg actionType What to do.  Options are to check source to see if any
    #                 unsupported MPI constants or functions will be an issue,
    #                 to report what will be changed in the source, or to
    #                 actually alter the source.  Default is to alter.
    def __init__(self, inputMode, inputPath, outputPath, actionType):
        self.supportFile = 'preprocessor.xml'
        if not os.path.isfile(self.supportFile):
            self.supportFile = os.path.dirname(__file__) + '/' +\
                               self.supportFile
            if not os.path.isfile(self.supportFile):
                raise ValueError("Cannot locate preproceessor XML file.") 
            
        self.inputMode = inputMode
        self.inputPath = inputPath
        self.outputPath = outputPath 
        self.actionType = actionType
        self.logFile = open('yogicpp.out', 'w')
        self.definitions = [] 
        self.loadSupported()
        self.outputPath = os.path.abspath(self.outputPath)
        if self.actionType == 'preprocess':
            if self.inputMode == 'file':
                self.preprocessFile(self.inputPath, self.outputPath)
            else:
                self.preprocessDirectory(self.inputPath, self.outputPath)
        elif self.actionType == 'checkwrap':
            if self.inputMode == 'file':
                self.checkFileWrap(self.inputPath)
            else:
                self.checkDirectoryWrap(self.inputPath) 
        self.logFile.close()

    # Loads from XML MPI functions and constants supported by YogiMPI.
    def loadSupported(self):
        fileTree = ET.parse(self.supportFile).getroot()
        for entry in fileTree.iterfind('Constant'):
            self.definitions.append(AU._getValidText(entry, True))
        for entry in fileTree.iterfind('Object'):
            self.definitions.append(AU._getValidText(entry, True))
        for entry in fileTree.iterfind('Function'):
            self.definitions.append(AU._getValidText(entry, True))

    def _findMPI(self, searchLine, mpiPattern=None, caseSensitive=False):
        if mpiPattern is None:
            mpiPattern = 'MPI_[a-zA-Z0-9_]+'
        if caseSensitive:
            flags = 0
        else:
            flags = re.IGNORECASE 

        mpiString = re.compile(r"(^|_|=|\s|\(|\)|,|\*|\+)(" + mpiPattern +\
                               r')(\s|,|\)|\()', flags)
        matchInfo = mpiString.finditer(searchLine)
        mpiMatches = []
        if matchInfo: 
            for match in matchInfo:
                mpiMatches.append(match.group(2))
        return mpiMatches

    def writeLog(self, *args):
        for a in args:
            self.logFile.write(a + '\n')

    def checkFileWrap(self, inputFile):
        noMPI = set() 
        ihandle = open(inputFile, 'r')
        rawFile = ihandle.readlines()
        ihandle.close()
        caseSensitive=False
        if not self._isFortranSource(inputFile):
            caseSensitive=True
        for line in rawFile:
            matchMPI = self._findMPI(line, caseSensitive=caseSensitive)
            for item in matchMPI: 
                if not item in self.definitions: 
                    noMPI.add(item)
        if noMPI:
            self.writeLog(os.path.abspath(inputFile))
            self.writeLog(*noMPI)
          

    def checkDirectoryWrap(self, inputDir):
        for dirpath, dnames, fnames in os.walk(inputDir):
            for f in fnames:
                if self._isSourceFile(f):
                    self.checkFileWrap(os.path.join(dirpath, f))

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
   
    def _isSourceFile(self, fileName):
        if self._isFortranSource(fileName):
            return True
        elif self._isCSource(fileName):
            return True
        elif self._isCXXSource(fileName):
            return True
        return False
 
    def _isFortranSource(self, fileName):
        fExtensions = ['f90', 'for', 'f', 'f77']
        for fe in fExtensions:
            if fileName.endswith(('.' + fe.lower(), '.' + fe.upper())):
                return True
        return False

    def _isCSource(self, fileName):
        cExtensions = ['c', 'h']
        for ce in cExtensions:
            if fileName.endswith('.' + ce):
                return True
        return False

    def _isCXXSource(self, fileName):
        cxxExtensions = ['C', 'cpp', 'cxx', 'hpp', 'H', 'I']
        for cxxe in cxxExtensions:
            if fileName.endswith('.' + cxxe):
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
    parser.add_argument('--action',
                        choices=['checkwrap', 'logchanges', 'preprocess'],
                        default='preprocess',
                        help="Action to take") 
    parser.add_argument('--output', '-o',
                        action="store",
                        required=True,
                        help="Output path")

    configArguments = parser.parse_args()

    preproc = yogicpp(configArguments.mode, configArguments.input,
                      configArguments.output, configArguments.action)
