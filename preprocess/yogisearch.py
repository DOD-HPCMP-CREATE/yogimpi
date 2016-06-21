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
        self.unsupportedFile = 'UnsupportedMPI.xml'
        checkFiles = [ self.unsupportedFile ]
        for aFile in checkFiles:
            if not os.path.isfile(aFile):
                raise ValueError("Cannot locate needed file " + filePath)

        self.inputPath = None 
        self.outputPath = None 
        self.ccxxOnly = False
        self.fortranOnly = False

    def run(self):
        if self.outputPath is None:
            self.outputPath = 'yogisearch.out'
        self.logFile = open(self.outputPath, 'a')
        self.cMissing = [] 
        self.fMissing = []
        missingLangMap = { 'C': self.cMissing, 'Fortran': self.fMissing }
        self.loadLanguageLists(self.unsupportedFile, missingLangMap)
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
                return self.cMissing
        elif self.fortranOnly:
            if self._isFortranSource(filename):
                return self.fMissing
        else:
            if self._isCSource(filename) or self._isCXXSource(filename):
                return self.cMissing
            elif self._isFortranSource(filename):
                return self.fMissing
        # If no conditions matched, return None to indicate file should not
        # be searched.
        return None 

    # Appends, for user-specified languages, to corresponding lists the 
    # strings of MPI function names, constants, and objects from the input file.
    # inputFile - Name of XML input file to use.
    # langMap - A dictionary specifying the language and list to use.
    #            Example: { 'C' : myList, 
    #                       'Fortran' : myList2 } 
    #           All information listed for C in the input file is placed into
    #           myList, and all information listed for Fortran in the input
    #           file is placed into myList2.
    def loadLanguageLists(self, inputFile, langMap):
        fileTree = ET.parse(inputFile).getroot()
        for langElement in fileTree.iterfind('Language'):
            targetList = langMap[langElement.get('name')]
            entryTypeList = [ 'Constant', 'Object', 'Function' ]
            for entryType in entryTypeList:
                for entry in langElement.iterfind(entryType):
                    targetList.append(AU._getValidText(entry, True))

    def writeLog(self, *args):
        for a in args:
            self.logFile.write(a + '\n')

    def _hasMPIInLine(self, aLine, term, caseSensitive):
        if caseSensitive:
            checkLine = aLine
            checkTerm = term
        else:
            checkLine = aLine.lower()
            checkTerm = term.lower()
        
        startIndex = 0
        if checkTerm in checkLine:
            while startIndex < len(checkLine):
                searchResult = checkLine.find(checkTerm, startIndex)
                if not searchResult == -1:
                    endIndex = searchResult + len(term)
                    if endIndex >= len(checkLine):
                        return True
                    elif not checkLine[endIndex] == '_' \
                         and not checkLine[endIndex].isalnum():
                        return True
                    startIndex = endIndex
                else:
                    return False
        return False

    def _hasMPIInFile(self, aFile, term, caseSensitive):
        with open(aFile, 'r') as f:
            theFileString = f.read()

        if not term.lower() in theFileString.lower():
            return False

        with open(aFile, 'r') as f:
            theFile = f.readlines()
        
        for aLine in theFile:
            if self._hasMPIInLine(aLine, term, caseSensitive):
                return True 
        return False 

    def checkFileWrap(self, inputFile, definitions):
        # If definitions file is None, this isn't a source file to search.
        # Skip the file.
        if definitions is None:
            return
        noMPI = set() 
        caseSensitive = True 
        if self._isFortranSource(inputFile):
            caseSensitive = False

        for aDef in definitions:
            if self._hasMPIInFile(inputFile, aDef, caseSensitive):
                noMPI.add(aDef)    

        if noMPI:
            self.writeLog(os.path.abspath(inputFile))
            self.writeLog(*noMPI)

    def checkDirectoryWrap(self, inputDir):
        totalChecked = 0
        for dirpath, dnames, fnames in os.walk(inputDir):
            for f in fnames:
               defs = self._getSearchDefs(f)
               if defs:
                   self.checkFileWrap(os.path.join(dirpath, f), defs)
                   totalChecked += 1 
                   totalStr = str(totalChecked) + ' source files scanned'
                   sys.stdout.write('%s\r' % totalStr) 
                   sys.stdout.flush()

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
        cxxExtensions = ['C', 'cpp', 'cxx', 'hpp', 'H', 'I', 'cc']
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
