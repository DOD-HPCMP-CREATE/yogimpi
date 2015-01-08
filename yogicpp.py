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
        self.logFile = None
        if not self.actionType == 'preprocess':
            self.logFile = open('yogicpp.out', 'w')
        self.definitions = [] 
        self.loadSupported()
        if self.actionType == 'preprocess' and self.inputMode == 'directory':
            self.outputPath = os.path.abspath(self.outputPath)
            if not os.path.exists(self.outputPath):
                os.mkdir(self.outputPath)
        if self.actionType == 'preprocess':
            if self.inputMode == 'file':
                self.preprocessFile(self.inputPath, self.outputPath,
                                    makeChanges=True)
            else:
                self.preprocessDirectory(self.inputPath, self.outputPath,
                                         makeChanges=True)
        elif self.actionType == 'simulate':
            self.outputPath = os.path.abspath(self.outputPath)
            if self.inputMode == 'file':
                self.preprocessFile(self.inputPath, self.outputPath,
                                    makeChanges=False)
            else:
                self.preprocessDirectory(self.inputPath, self.outputPath,
                                         makeChanges=False)
        elif self.actionType == 'checkwrap':
            if self.inputMode == 'file':
                self.checkFileWrap(self.inputPath)
            else:
                self.checkDirectoryWrap(self.inputPath) 
        if self.logFile:
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

    def _findMPI(self, searchLine, mpiPattern=None, caseSensitive=False,
                 underScoreAllowed=False):
        if mpiPattern is None:
            mpiPattern = 'MPI_[a-zA-Z0-9_]+'
        if caseSensitive:
            flags = 0
        else:
            flags = re.IGNORECASE 

        if not underScoreAllowed:
            group1 = r"(^|=|\s|\(|\)|,|\*|\+)("
        else:
            group1 = r"(^|_|=|\s|\(|\)|,|\*|\+)("
        mpiString = re.compile(group1 + mpiPattern + r')(\s|,|\)|\()', flags)
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
        underScoreAllowed=False
        if not self._isFortranSource(inputFile):
            caseSensitive=True
        else:
            underScoreAllowed=True
        for line in rawFile:
            matchMPI = self._findMPI(line, caseSensitive=caseSensitive,
                                     underScoreAllowed=underScoreAllowed)
            for item in matchMPI: 
                if caseSensitive:
                    if not item in self.definitions: 
                        noMPI.add(item)
                else:
                    if not item.upper() in (md.upper() for md in self.definitions):
                        noMPI.add(item) 
        if noMPI:
            self.writeLog(os.path.abspath(inputFile))
            self.writeLog(*noMPI)
          

    def checkDirectoryWrap(self, inputDir):
        for dirpath, dnames, fnames in os.walk(inputDir):
            for f in fnames:
                if self._isSourceFile(f):
                    self.checkFileWrap(os.path.join(dirpath, f))

    def preprocessDirectory(self, inputDir, outputDir, makeChanges=False):
        for dirpath, dnames, fnames in os.walk(inputDir):
            for f in fnames:
                if self._isSourceFile(f):
                    self.preprocessFile(os.path.join(dirpath, f), 
                                        outputDir + '/' + f)
 
    def _showChanges(self, fileName, originalFile, modFile):
        isChanged = False
        changeList = []
        for i in range(len(originalFile)):
            if originalFile[i] != modFile[i]:
                changeList.append(i)
                isChanged = True
        if isChanged:
            self.logFile.write("Changing " + fileName + "\n")
            for lineNum in changeList:
                self.logFile.write("Line " + str(lineNum + 1) + "\n")
                self.logFile.write("- " + originalFile[lineNum])
                self.logFile.write("+ " + modFile[lineNum])
                self.logFile.flush()         
 
    def preprocessFile(self, inputFile, outputFile, makeChanges=False):
        ihandle = open(inputFile, 'r')
        rawFile = ihandle.readlines()
        originalFile = list(rawFile)
        ihandle.close()

        if self._isFortranSource(inputFile):
            for aPattern in self.definitions:
                for i in range(len(rawFile)):
                    mpiString = re.compile(r"(^|_|=|\s|\(|\)|,|\*|\+)(" +\
                                           aPattern +\
                                           r')(\s|,|\)|\()', re.IGNORECASE)
                    rawFile[i] = mpiString.sub(r"\g<1>Yogi\g<2>\g<3>", 
                                               rawFile[i])
                    rawFile[i] = re.sub(r"(\"|')mpif.h(\"|')", "'yogimpif.h'", 
                                        rawFile[i])
        elif self._isCSource(inputFile) or self._isCXXSource(inputFile):
            for i in range(len(rawFile)):
                rawFile[i] = re.sub("(\"|<)mpi.h(\"|>)", "\"mpitoyogi.h\"", 
                                    rawFile[i])
        
        if makeChanges:
            ohandle = open(outputFile, 'w')
            ohandle.writelines(rawFile)
        else:
            self._showChanges(inputFile, originalFile, rawFile)
 
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
    parser.add_argument('--action', '-a',
                        choices=['checkwrap', 'simulate', 'preprocess'],
                        default='preprocess',
                        help="Action to take") 
    parser.add_argument('--output', '-o',
                        action="store",
                        default=None,
                        help="Output path")

    configArguments = parser.parse_args()

    preproc = yogicpp(configArguments.mode, configArguments.input,
                      configArguments.output, configArguments.action)
