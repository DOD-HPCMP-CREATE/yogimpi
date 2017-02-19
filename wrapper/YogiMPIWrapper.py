#!/usr/bin/env python

import os
import sys
import time
import subprocess
import tempfile
import filecmp
import re

# MPI constants currently supported in Fortran.
mpiConstants = [ 'MPI_SUCCESS', 'MPI_ERR_BUFFER', 'MPI_ERR_COUNT',
    'MPI_ERR_TYPE', 'MPI_ERR_TAG', 'MPI_ERR_COMM', 'MPI_ERR_RANK', 
    'MPI_ERR_REQUEST', 'MPI_ERR_ROOT',
    'MPI_ERR_GROUP', 'MPI_ERR_OP', 'MPI_ERR_TOPOLOGY', 'MPI_ERR_DIMS', 
    'MPI_ERR_ARG', 'MPI_ERR_UNKNOWN', 'MPI_ERR_TRUNCATE', 'MPI_ERR_OTHER', 
    'MPI_ERR_INTERN', 'MPI_ERR_PENDING', 'MPI_ERR_IN_STATUS', 
    'MPI_ERR_LASTCODE', 'MPI_BOTTOM', 'MPI_PROC_NULL', 'MPI_ANY_SOURCE',
    'MPI_ANY_TAG', 'MPI_UNDEFINED', 'MPI_BSEND_OVERHEAD', 'MPI_KEYVAL_INVALID',
    'MPI_STATUS_SIZE', 'MPI_SOURCE', 'MPI_TAG', 'MPI_ERROR', 
    'MPI_MAX_PROCESSOR_NAME', 'MPI_CHARACTER', 'MPI_INTEGER', 'MPI_REAL',
    'MPI_DOUBLE_PRECISION', 'MPI_BYTE', 'MPI_PACKED', 'MPI_COMPLEX',
    'MPI_DOUBLE_COMPLEX', 'MPI_LOGICAL', 'MPI_2REAL', 'MPI_2DOUBLE_PRECISION',
    'MPI_2INTEGER', 'MPI_INTEGER1', 'MPI_INTEGER2', 'MPI_INTEGER4',
    'MPI_INTEGER8', 'MPI_REAL4', 'MPI_REAL8',
    'MPI_COMM_WORLD', 'MPI_COMM_SELF', 'MPI_IDENT', 'MPI_CONGRUENT',
    'MPI_SIMILAR', 'MPI_UNEQUAL', 'MPI_TAG_UB', 'MPI_IO', 'MPI_HOST',
    'MPI_WTIME_IS_GLOBAL', 'MPI_MAX', 'MPI_MIN', 'MPI_SUM', 'MPI_PROD',
    'MPI_MAXLOC', 'MPI_MINLOC', 'MPI_BAND', 'MPI_BOR', 'MPI_BXOR', 'MPI_LAND',
    'MPI_LOR', 'MPI_LXOR', 'MPI_GROUP_NULL', 'MPI_COMM_NULL',
    'MPI_REQUEST_NULL', 'MPI_OP_NULL', 'MPI_DATATYPE_NULL', 'MPI_INFO_NULL',
    'MPI_FILE_NULL', 'MPI_GROUP_EMPTY', 'MPI_SEEK_SET', 'MPI_SEEK_CUR',
    'MPI_SEEK_END', 'MPI_MODE_RDONLY', 'MPI_MODE_RDWR', 'MPI_MODE_WRONLY',
    'MPI_MODE_CREATE', 'MPI_MODE_EXCL', 'MPI_MODE_DELETE_ON_CLOSE',
    'MPI_MODE_UNIQUE_OPEN', 'MPI_MODE_SEQUENTIAL', 'MPI_MODE_APPEND',
    'MPI_OFFSET_KIND', 'MPI_STATUS_IGNORE', 'MPI_STATUSES_IGNORE' ]

# MPI objects currently supported in Fortran.
mpiObjects = [ 'MPI_Comm', 'MPI_Request', 'MPI_Op', 'MPI_Info', 'MPI_Datatype',
               'MPI_Group', 'MPI_Status', 'MPI_File', 'MPI_Offset' ]

# MPI functions currently supported in Fortran.
mpiFunctions = [ 'MPI_Send', 'MPI_Recv', 'MPI_Get_count', 'MPI_Ssend', 
    'MPI_Isend',
    'MPI_Issend', 'MPI_Irecv', 'MPI_Sendrecv', 'MPI_Wait', 'MPI_Request_free',
    'MPI_Waitsome', 'MPI_Waitany', 'MPI_Waitall', 'MPI_Send_init',
    'MPI_Type_size', 'MPI_Type_contiguous', 'MPI_Type_vector',
    'MPI_Type_indexed', 'MPI_Type_commit', 'MPI_Type_free', 'MPI_Barrier',
    'MPI_Bcast', 'MPI_Gather', 'MPI_Gatherv', 'MPI_Scatter', 'MPI_Scatterv',
    'MPI_Allgather', 'MPI_Allgatherv', 'MPI_Reduce', 'MPI_Allreduce',
    'MPI_Comm_create', 'MPI_Comm_group', 'MPI_Comm_size', 'MPI_Comm_rank',
    'MPI_Comm_dup', 'MPI_Comm_split', 'MPI_Comm_free', 'MPI_Group_free',
    'MPI_Group_incl', 'MPI_Group_rank', 'MPI_Group_size',
    'MPI_Group_translate_ranks', 'MPI_Get_processor_name', 'MPI_Wtime',
    'MPI_Init', 'MPI_Finalize', 'MPI_Type_create_indexed_block',
    'MPI_Comm_f2c', 'MPI_Recv_init', 'MPI_Scan', 'MPI_Startall', 'MPI_Alltoall',
    'MPI_Alltoallv', 'MPI_File_close', 'MPI_File_get_info', 'MPI_File_open',
    'MPI_File_set_view', 'MPI_File_write_all', 'MPI_File_write_at',
    'MPI_File_read_at', 'MPI_File_read_all', 'MPI_File_read',
    'MPI_Info_create', 'MPI_Info_set', 'MPI_Info_free', 'MPI_Abort',
    'MPI_Probe', 'MPI_Iprobe', 'MPI_Test', 'MPI_Attr_get', 'MPI_Finalized',
    'MPI_Initialized', 'MPI_Bsend', 'MPI_Buffer_attach', 'MPI_File_delete',
    'MPI_File_write', 'MPI_File_seek' ]


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
        global mpiConstants
        global mpiFunctions
        global mpiObjects
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
        self.mpi_constants = mpiConstants 
        self.mpi_objects = mpiObjects 
        self.mpi_functions = mpiFunctions
        self.mpi_regexes = []
        self.rc = 0

        diagOptions = [ '-h', '-help', '-show', '--help', '--version',
                        '-compile_info', '-link_info' ]
        fortranOptions = [ '--yogi-preprocess-only' ]
        
        # Obtain the debug level set by the user.
        self.debug = int(os.environ.get('YMPI_COMPILER_DEBUG', 0))

        diagMode = False
        if "--no-yogi" in self.argArray:
            self.passThrough()
            return

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
        if self.compilerLang == 'Fortran':
            for aPattern in self.mpi_constants:
                regexString = r"(^|_|=|\s|\(|\)|,|\*|\+)(" + aPattern +\
                              r')(\s|,|\*|\)|\()'
                self.mpi_regexes.append(re.compile(regexString, re.IGNORECASE))
            for aPattern in self.mpi_functions:
                # Functions require an open parenthesis to follow, or
                # if there is a line continuation just trust that someone
                # put one on the next line.
                # ToDo: Actually do lookahead and find out for that last case.
                regexString = r"(^|=|\s|\(|\)|,|\*|\+)(" + aPattern +\
                              r')([\s]*\(|[\s]*&$)'
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

    ## Passes through doing any YogiMPI flags and acts only as the underlying
    #  serial compiler.       
    def passThrough(self):
        self.argArray[0] = self.compilerName
        self.argArray.remove('--no-yogi')
        self._outputMsg("Final compile string: " + self._getCallString())
        self.rc = subprocess.call(self._getCallString(), shell=True)
 
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
