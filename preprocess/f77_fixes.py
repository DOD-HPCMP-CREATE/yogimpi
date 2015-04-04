#!/usr/bin/env python

    def _ffshouldIgnore(self, line):
        # Ignore comments
        if (len(line.lstrip()) > 0):
            if line.lstrip()[0] == '!':
                return True 
        lineBeginning = line[:6]
        if len(lineBeginning.lstrip()) > 0:
            if lineBeginning.lstrip()[0] == 'c' \
            or lineBeginning.lstrip()[0] == 'C':
                return True 
        # Ignore preprocessor directives 
        if line.startswith('#'):
            return True           
        return False
    
            # Handle the possibility of fixed-form Fortran, which means line
            # length cannot exceed 72 characters.  Yuck.
            if self._checkFixedForm(inputFile):
                lineIndex = range(len(rawFile))
                for i in lineIndex:
                    # If this was not edited, don't bother.
                    if not 'Yogi' in rawFile[i]:
                        continue
                    if self._ffshouldIgnore(rawFile[i]):
                        continue 
                    # Strip off comments coming after valid expression.
                    commentStart = rawFile[i].find('!') 
                    if commentStart > 0 \
                    and rawFile[i][commentStart - 1] != "\\":
                        rawFile[i] = rawFile[i][:commentStart] + '\n' 
                    if len(rawFile[i].rstrip()) <= 72:
                        continue 

                    firstPart = rawFile[i][:72].rstrip('\n')
                    continueLine = '     &' + rawFile[i][72:].rstrip('\n')
                    rawFile[i] = firstPart + '\n'
                    addLine = True
                    if (i+1 < len(rawFile)):
                        nextLine = rawFile[i+1]
                        if len(nextLine) > 5:
                            # This is a continuation line.  Merge the two
                            if nextLine[5] != ' ' \
                            and not self._ffshouldIgnore(nextLine):
                                continueLine += nextLine[6:].rstrip('\n')
                                rawFile[i+1] = continueLine + '\n'
                                addLine = False
                    # Next line unrelated, add extra continuation line.
                    if addLine:
                        rawFile.insert(i+1, continueLine + '\n')
                        lineIndex.append(len(rawFile) - 1)

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

    def _checkFixedForm(self, fileName):
        if self.fixedform:
            if self._isFortranSource(fileName):
                fExtensions = ['f', 'f77']
                for fe in fExtensions:
                    if fileName.endswith(('.' + fe.lower(), '.' + fe.upper())):
                        return True 
        return False 
