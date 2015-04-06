#!/usr/bin/env python
import sys
import os
import argparse
import re
import shutil
from pprint import pprint
from AVUtility import AVUtility as AU
import xml.etree.ElementTree as ET

class yogisearch(object):

    def __init__(self):
        self.supportFile = 'preprocessor.xml'
        if not os.path.isfile(self.supportFile):
            self.supportFile = os.path.dirname(__file__) + '/' +\
                               self.supportFile
            if not os.path.isfile(self.supportFile):
                raise ValueError("Cannot locate preprocessor XML file.") 
        self.inputPath = None 
        self.outputPath = None 
        self.ccxxOnly = False
        self.fortranOnly = False

    def run(self):
        if self.outputPath is None:
            self.outputPath = 'yogisearch.out'
        self.logFile = open(self.outputPath, 'w')
        self.cDefinitions = [] 
        self.fDefinitions = []
        self.loadSupported()
        if os.path.isdir(self.inputPath):
            self.checkDirectoryWrap(self.inputPath)
        elif os.path.isfile(self.inputPath):
            self.checkFileWrap(self.inputPath,
                               self._getSearchDefs(self.inputPath))
        else:
            raise ValueError("Error: " + self.inputPath + " is not a file" +\
                             " or directory.")
        self.logFile.close()

    def _getSearchDefs(self, filename):
        if self.ccxxOnly:
            if self._isCSource(filename) or self._isCXXSource(filename):
                return self.cDefinitions
        elif self.fortranOnly:
            if self._isFortranSource(filename):
                return self.fDefinitions
        else:
            if self._isCSource(filename) or self._isCXXSource(filename):
                return self.cDefinitions 
            elif self._isFortranSource(filename):
                return self.fDefinitions
        # If no conditions matched, return None to indicate file should not
        # be searched.
        return None 

    # Loads from XML MPI functions and constants supported by YogiMPI.
    def loadSupported(self):
        fileTree = ET.parse(self.supportFile).getroot()
        cLangElement = None
        fLangElement = None
        for langElement in fileTree.iterfind('Language'):
            if langElement.attrib['name'] == 'C':
                cLangElement = langElement
            elif langElement.attrib['name'] == 'Fortran':
                fLangElement = langElement

        for entry in cLangElement.iterfind('Constant'):
            self.cDefinitions.append(AU._getValidText(entry, True))
        for entry in cLangElement.iterfind('Object'):
            self.cDefinitions.append(AU._getValidText(entry, True))
        for entry in cLangElement.iterfind('Function'):
            self.cDefinitions.append(AU._getValidText(entry, True))

        for entry in fLangElement.iterfind('Constant'):
            self.fDefinitions.append(AU._getValidText(entry, True))
        for entry in fLangElement.iterfind('Object'):
            self.fDefinitions.append(AU._getValidText(entry, True))
        for entry in fLangElement.iterfind('Function'):
            self.fDefinitions.append(AU._getValidText(entry, True))

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

    def checkFileWrap(self, inputFile, definitions):
        # If definitions file is None, this isn't a source file to search.
        # Skip the file.
        if definitions is None:
            return
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
                    if not item in definitions: 
                        noMPI.add(item)
                else:
                    if not item.upper() in (md.upper() for md in definitions):
                        noMPI.add(item) 
        if noMPI:
            self.writeLog(os.path.abspath(inputFile))
            self.writeLog(*noMPI)
          

    def checkDirectoryWrap(self, inputDir):
        for dirpath, dnames, fnames in os.walk(inputDir):
            for f in fnames:
                self.checkFileWrap(os.path.join(dirpath, f),
                                   self._getSearchDefs(f))

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
    parser = argparse.ArgumentParser(description='YogiMPI Search Tool')

    parser.add_argument('--input', '-i',
                        action="store",
                        required=True,
                        help="Input directory or file to examine")
    parser.add_argument('--fortranonly',
                        action='store_true',
                        help="Consider only Fortran source files")
    parser.add_argument('--ccxxonly',
                        action='store_true',
                        help="Consider only C/C++ source files")
    parser.add_argument('--output', '-o',
                        action="store",
                        default=None,
                        help="Output path")

    configArguments = parser.parse_args()

    preproc = yogisearch()
    preproc.inputPath = configArguments.input
    preproc.outputPath = configArguments.output
    preproc.fortranOnly = configArguments.fortranonly
    preproc.ccxxOnly = configArguments.ccxxonly
    preproc.run()
