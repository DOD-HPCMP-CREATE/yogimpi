#!/usr/bin/env python

import os
import sys
import time
import subprocess
import tempfile
import filecmp
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

    fixedExts = [ '.f77', '.f', '.for' ]
    f77Comments = [ 'c', 'C', '*' ]
    useMPIRegEx = re.compile(r"([\s]*)use[\s]+mpi([\s]+)", re.IGNORECASE)

    def __init__(self, prefixDir, compilerName, compilerLang):
        self.prefixDir = prefixDir
        self.compilerName = compilerName
        self.compilerLang = compilerLang
        self.supportFile = self.prefixDir + '/bin/YogiSupported.xml'
        self.argArray = list(sys.argv)
        self.cppArgs = []
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
        self.mpi_regexes = []
        self.rc = 0

        diagOptions = [ '-h', '-help', '-show', '--help', '--version',
                        '-compile_info', '-link_info' ]
        fortranOptions = [ '--yogi-preprocess-only' ]
        
        # Obtain the debug level set by the user.
        self.debug = int(os.environ.get('YMPI_COMPILER_DEBUG', 0))

        diagMode = False
        for anArg in self.argArray:
            if anArg in diagOptions:
                diagMode = True
            elif self.compilerLang == 'Fortran' and anArg in fortranOptions:
                self.preprocessOnly = True
            if anArg.startswith(('-D', '-I')):
                self.cppArgs.append(anArg)
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
        if self.debug:
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
        # Now create regular expressions from everything loaded.
        # This is currently only needed if using Fortran.
        if self.compilerLang == 'Fortran':
            searchTerms = self.mpi_constants + self.mpi_functions
            for aPattern in searchTerms:
                regexString = r"(^|_|=|\s|\(|\)|,|\*|\+)(" + aPattern +\
                              r')(\s|,|\*|\)|\()'
                self.mpi_regexes.append(re.compile(regexString, re.IGNORECASE))

        
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
                if not self.namesOutput and not self.isLinking:
                    objectName = self._stripExtension(self.sourceFile) + '.o'
                    self.argArray.append('-o ' + objectName)
            self.argArray.insert(self.sourceLocation, '-I' + self.prefixDir +\
                                 '/include -DYOGIMPI_ENABLED')
        if self.isLinking:
             self.argArray.append('-L' + self.prefixDir + '/lib -lyogimpi')

    def _getCallString(self):
        callString = ''
        for anArg in self.argArray:
            callString += anArg + ' '
        return callString
        
    def _printCommand(self):
        sys.stdout.write(self._getCallString() + '\n')

    def _isFixedForm(self, sourceFile):
        # Automatically assume a .f or .f77 file is fixed-form.
        if self._getExtension(sourceFile).lower() in YogiMPIWrapper.fixedExts:
            return True
        return False 

    ## Return whether this is a line to ignore (blank or comment). 
    def _isFortranIgnoreLine(self, sourceLine, fixed=False):
	if not sourceLine.strip():
            # Ignore empty lines.
	    return True 
        if sourceLine.lstrip().startswith('!'):
            # Ignore comment lines for free-form. Unfortunately some pseudo
            # fixed-form codes also have lines that start with this, and it
            # is expected they'll also be treated as comments.
            return True 
        elif fixed and sourceLine[0] in YogiMPIWrapper.f77Comments:
            # Ignore comments for fixed-form.
            return True 
        if sourceLine.startswith('#'):
            # Ignore preprocessor directives.
            return True           
        return False

    ## Handle line-continuation in Fortran, which may require breaking a line
    #  into pieces if we have made it too long.
    #  @arg fileArray The Python list containing lines of the files.
    #  @arg lineNumber The number of the line to check.
    def _checkFixedLine(self, fileArray, lineNumber):
        line = fileArray[lineNumber]
        if len(line.rstrip()) <= 72:
            # Nothing to do here.
            return (lineNumber + 1)

        # Strip off comments coming after valid expression.
        commentStart = line.find('!') 
        if commentStart > -1 and line[commentStart - 1] != "\\":
            line = line[:commentStart]
            if len(line.rstrip()) <= 72:
                # If removing the comment fixed length problems, done.
                fileArray[lineNumber] = line.rstrip() + '\n'
                return (lineNumber + 1) 

        # Otherwise split the line into two pieces, the second with
        # a continuation character.
        firstPart = line[:72].strip('\n')
        secondPart = '     &' + line[72:].strip('\n')
        fileArray[lineNumber] = firstPart + '\n'
        if (lineNumber + 1) < len(fileArray):
            fileArray.insert(lineNumber + 1, secondPart + '\n')
        else:
            # This was the last line of the file.  Add one more line.
            fileArray.append(secondPart + '\n')
        # Return the bumped up line count.  In any case it's two lines.
        return (lineNumber + 2)

    ## Calls the system preprocessor (cpp) to yield a fully expanded Fortran
    #  source file.  Sometimes this is needed in tricky situations where 
    #  Fortran source "includes" another source file that has MPI definitions.
    def _callSystemPreprocessor(self):
        cppString = ''
        for i in range(len(self.cppArgs)):
            anArg = self.cppArgs[i]
            if i > 0:
                cppString += ' ' 
            cppString += anArg 
        cppString += " " + self._getFullSourcePath()
        try:   
            fileExtension = self._getExtension(self.sourceFile)
            newFile, newPath = tempfile.mkstemp(prefix='yogiF_',
                                                suffix=fileExtension)
            os.close(newFile)
            cppString += " " + newPath
            subprocess.check_call("cpp -P -traditional-cpp " + cppString,
                                  shell=True)
            if filecmp.cmp(self._getFullSourcePath(), newPath):
                # The files are the same, no need to do anything.
                os.remove(newPath) 
                return False
            else:
                self._outputMsg("File " + self._getFullSourcePath() +\
                                " changed when running system preprocessor," +\
                                " new file is " + newPath) 
                return newPath
        except:
            print "YogiMPI encountered an error preprocessing Fortran source."
            raise

    ## Preprocesses a Fortran file, changing MPI_ to YogiMPI_ wherever
    #  used.  A temporary file is created with the new contents.
    #  Returns True on False as to whether the file was ever changed.
    def preprocessFortran(self, inPlace=False):
        changedFile = False
        self._outputMsg("Preprocessing Fortran source " +\
                        self._getFullSourcePath())
        with open(self._getFullSourcePath(), 'r') as f:
           firstRead = f.read()
           f.seek(0)
           origLines = f.readlines()
           if '#include' in firstRead:
               neededCPP = self._callSystemPreprocessor()
           else:
               neededCPP = False

        if neededCPP:
            startFile = neededCPP
            fileHandle = open(startFile, 'r')
            rawFile = fileHandle.readlines()
            fileHandle.close()
        else:
            rawFile = origLines 
            startFile = self._getFullSourcePath() 

        fixedForm = self._isFixedForm(startFile) 
        if neededCPP:
            os.remove(neededCPP)

        i = 0
        while i < len(rawFile):
            line = rawFile[i]
            if self._isFortranIgnoreLine(line, fixedForm):
                i += 1
                continue
            oldLine = line

            for aRegex in self.mpi_regexes:
                # Run through all the loaded MPI regular expressions.
                line = aRegex.sub(r"\g<1>Yogi\g<2>\g<3>", line)

            # Substitute "use yogimpi" for "use mpi" where applicable.
            line = YogiMPIWrapper.useMPIRegEx.sub(r"\g<1>use yogimpi\g<2>",
                                                  line)

            # Detect if the line changed, and if so, handle fixed-format 
            # problems and/or bumping the line counter. 
            if oldLine != line:
                rawFile[i] = line
                if fixedForm:
                    # i can be modified if added lines should be skipped.
                    i = self._checkFixedLine(rawFile, i)
                else:
                    i += 1
                changedFile = True
            else:
                i += 1

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
            if self.newPath and (not self.debug):
                os.remove(self.newPath)
