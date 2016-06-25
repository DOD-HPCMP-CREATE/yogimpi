#!/usr/bin/env python

import os
import sys
import time
import subprocess
import tempfile
import re
from AVUtility import AVUtility as AU
import xml.etree.ElementTree as ET

class YogiMPIWrapper(object):

    notLinkingOpts = [ '-c', '-S', '-E', '-M', '-MM' ]
    extensions = { 'C': ['.c'],
                   'C++': ['.c', '.cpp', '.hpp', '.cxx', '.cc', '.i'],
                   'Fortran': [ '.f', '.f90', '.for', '.f77', '.ftn', '.f95',
                                '.f03' ]
                 }

    def __init__(self, prefixDir, compilerName, compilerLang):
        self.prefixDir = prefixDir
        self.compilerName = compilerName
        self.compilerLang = compilerLang
        self.supportFile = self.prefixDir + '/bin/YogiSupported.xml'
        self.argArray = list(sys.argv)
        self.sourceDir = ''
        self.sourceFile = None
        self.sourceLocation = None
        self.isLinking = True
        self.newFile = None
        self.newPath = None
        self.namesOutput = False
        self.preprocessOnly = False
        self.mpi_constants = []
        self.mpi_objects = []
        self.mpi_functions = []
        self.rc = 0

        diagOptions = [ '-h', '-help', '-show', '--help', '--version',
                        '-compile_info', '-link_info' ]
        fortranOptions = [ '--yogi-preprocess-only' ]

        diagMode = False
        for anArg in self.argArray:
            if anArg in diagOptions:
                diagMode = True
            elif self.compilerLang == 'Fortran' and anArg in fortranOptions:
                self.preprocessOnly = True
        if diagMode:
            self._showCompilerString()
        else:
            self.loadSupported()
            self.doWrap()

    def getRC(self):
        return self.rc

    def _isSourceType(self, inputFile, language):
        for anExt in YogiMPIWrapper.extensions[language]:
            if inputFile.lower().endswith(anExt):
                return True
        return False

    def _showCompilerString(self):
        self.compString = self.compilerName 
        self.compString += ' -I' + self.prefixDir + '/include -DYOGIMPI_ENABLED'
        self.compString += ' -L' + self.prefixDir + '/lib -lyogimpi'
        print self.compString

    def _outputMsg(self, message):
        if 'YMPI_COMPILER_DEBUG' in os.environ:
            if int(os.environ['YMPI_COMPILER_DEBUG']) == 1:
                print message

    def loadSupported(self):
        fileTree = ET.parse(self.supportFile).getroot()
        if self.compilerLang == 'Fortran':
            thisLang = 'Fortran'
        else:
            thisLang = 'C'
        thisLangElement = None
        for langElement in fileTree.findall('Language'):
            if langElement.attrib['name'] == thisLang:
                thisLangElement = langElement

        for entry in thisLangElement.findall('Constant'):
            self.mpi_constants.append(AU._getValidText(entry, True))
        if not self.compilerLang == 'Fortran':
            # Only search for constants and functions if this is not Fortran.
            # MPI objects such as MPI_Datatype and MPI_Comm are integers in
            # Fortran bindings.
            for entry in thisLangElement.findall('Object'):
                self.mpi_objects.append(AU._getValidText(entry, True))
        for entry in thisLangElement.findall('Function'):
            self.mpi_functions.append(AU._getValidText(entry, True))
        
    def setFile(self, inputFile, argLocation):
        self.sourceDir = os.path.dirname(inputFile)
        self.sourceFile = os.path.basename(inputFile)
        self.sourceLocation = argLocation

    def _getFullSourcePath(self):
        if self.sourceDir:
            return self.sourceDir + '/' + self.sourceFile
        else:
            return self.sourceFile

    def _stripExtension(self, filename):
        for ext in YogiMPIWrapper.extensions[self.compilerLang]:
            if filename.lower().endswith(ext):
                return filename[:-(len(ext))] 
        return filename 

    def _getExtension(self, filename):
        for ext in YogiMPIWrapper.extensions[self.compilerLang]:
            if filename.lower().endswith(ext):
                return filename[-(len(ext)):]
        return filename

    def _changeArgs(self):
        self.argArray[0] = self.compilerName 
        if self.sourceFile:
            if self.compilerLang == 'Fortran':
                # Fortran files require preprocssing.
                changedFile = self.preprocessFortran()
                if changedFile:
                    self.argArray[self.sourceLocation] = self.newPath
            self.argArray.insert(self.sourceLocation, '-I' + self.prefixDir +\
                                 '/include -DYOGIMPI_ENABLED')
            if not self.namesOutput and not self.isLinking:
                objectName = self._stripExtension(self.sourceFile) + '.o'
                self.argArray.append('-o ' + objectName)
        if self.isLinking:
             self.argArray.append('-L' + self.prefixDir + '/lib -lyogimpi')

    def _getCallString(self):
        callString = ''
        for anArg in self.argArray:
            callString += anArg + ' '
        return callString
        
    def _printCommand(self):
        sys.stdout.write(self._getCallString() + '\n')

    ## Preprocesses a Fortran file, changing MPI_ to YogiMPI_ wherever
    #  used.  A temporary file is created with the new contents.
    #  Returns True on False as to whether the file was ever changed.
    def preprocessFortran(self, inPlace=False):
        changedFile = False
        self._outputMsg("Preprocessing Fortran source " +\
                        self._getFullSourcePath())
        fileHandle = open(self._getFullSourcePath(), 'r')
        rawFile = fileHandle.readlines()
        fileHandle.close()

        searchTerms = self.mpi_constants + self.mpi_functions
        for aPattern in searchTerms: 
            for i in range(len(rawFile)):
                mpiString = re.compile(r"(^|_|=|\s|\(|\)|,|\*|\+)(" +\
                                       aPattern +\
                                       r')(\s|,|\*|\)|\()', re.IGNORECASE)
                oldLine = rawFile[i]
                rawFile[i] = mpiString.sub(r"\g<1>Yogi\g<2>\g<3>", rawFile[i])
                useString = re.compile(r"([\s]*)use[\s]+mpi", re.IGNORECASE)
                rawFile[i] = useString.sub(r"\g<1>use yogimpi", rawFile[i])
                if oldLine != rawFile[i]:
                    changedFile = True

        if not changedFile:
            self._outputMsg("File " + self._getFullSourcePath() +\
                            " was unchanged.")
        else:
            fileExtension = self._getExtension(self.sourceFile)
            if inPlace: 
                oldFile = self._getFullSourcePath()
                timestr = time.strftime("%Y%m%d-%H%M%S")
                backupPath = oldFile + '.yogi_orig_' + timestr
                os.rename(oldFile, backupPath)
                self._outputMsg("File " + oldFile + " was changed in place," +\
                                " backup is " + backupPath)
                newFile = open(oldFile, 'w')
                newFile.writelines(rawFile)
                newFile.close()
            else:
                self.newFile, self.newPath = tempfile.mkstemp(prefix='yogiF_',
                                                          suffix=fileExtension)
                os.close(self.newFile)
                self.newFile = open(self.newPath, 'w')
                self._outputMsg("File " + self._getFullSourcePath() +\
                                " was changed, new file is " + self.newPath)                                                         
                self.newFile.writelines(rawFile)
                self.newFile.close()

        return changedFile
        
    def doWrap(self):
        for i in range(len(self.argArray)):
            anOpt = self.argArray[i]
            if anOpt in YogiMPIWrapper.notLinkingOpts:
                # See if this command is linking, which means we have to add 
                # library flags as well as extra include flags.
                self.isLinking = False
            elif self._isSourceType(anOpt, self.compilerLang):
                # Find the source file to preprocess, if any.
                self.setFile(anOpt, i)
            elif anOpt == '-o':
                # If there was an output file name, use what was specified.
                self.namesOutput = True

        if self.preprocessOnly:
            self.preprocessFortran(inPlace=True)
        else:
            self._changeArgs()
            self._outputMsg("Final compile string: " + self._getCallString())
            self.rc = subprocess.call(self._getCallString(), shell=True)
            if self.newPath:
                os.remove(self.newPath)
