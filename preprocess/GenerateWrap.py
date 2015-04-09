import xml.etree.ElementTree as ET
import sys
from __builtin__ import False

class MPIArgument(object):
    def __init__(self):
        # Original name of the argument.
        self.name = None
        # The way the argument is called in a function.
        self.callName = None
        # Original type of the argument.
        self.type = None
        # Whether the argument is a pointer.
        self.isPointer = False
        # Whether the argument is an array.  Also makes isPointer True.
        self.isPlural = False
        # Whether the argument is considered output.
        self.isOutput = False
        # Whether the argument is considered input.
        self.isInput = False
        # Values that, if matching Yogi equivalent of argument, cause a
        # conversion to that value.  (Example: YogiMPI_UNDEFINED to 
        # MPI_UNDEFINED).
        self.convertValues = []
        # Values that, if matching Yogi equivalent of argument, cause a
        # conversion to the value, except it happens AFTER the MPI call.
        self.postconvertValues = []
        # Whether this argument is an MPI typedef or MPI structure.
        self.isMPIType = False
        # If the argument is an MPI type and is plural, the dimensions. Note
        # that this is not set for primitive type arrays.
        self.dims = None
        
        # The following variables are only used if isMPIType is True. Otherwise
        # the values all remain set to False and None.
        # The name of the temporary variable.
        self.tempName = None
        # The type of the temporary variable.
        self.tempType = None
        # Whether the temporary variable is a pointer.
        self.tempIsPtr = False
        # The function used to convert the argument to the input variable.
        # If isMPIType is True and this is None, then it instantiates a 
        # variable without a function call.
        self.tempPreConverter = None
        # In the case of an output variable, the function used to convert the
        # input variable back to the argument.
        self.tempPostConverter = None
        # Whether a handle will be freed by the operation.
        self.freeHandle = False
        # If freeObject is True, what to set the freed value to for user.
        self.freedValue = None

class MPIFunction(object):
    def __init__(self):
        self.name = None
        self.statusIgnore = False
        self.statusIgnoreType = None
        self.statusIgnoreArg = 0
        self.returnType = 'int'
        self.args = []
    
    def getArg(self, name):
        for anArg in self.args:
            if anArg.name == name:
                return anArg
        return None
    
class GenerateWrap(object):

    def __init__(self, xmlInput, filePrefix):
        self.filePrefix = filePrefix
        self.prefix = 'Yogi'
        self.functions = []
        self.parseInput(xmlInput)
        #self.debugValues()
        self.writeFiles()
        
    def parseInput(self, fileName):
        tree = ET.parse(fileName)
        xmlRoot = tree.getroot()

        for funcElement in xmlRoot:
            if funcElement.tag != 'Function':
                continue
            thisFunction = MPIFunction()
            thisFunction.name = funcElement.attrib['name']
            thisFunction.returnType = funcElement.find('ReturnType').text

            for argElement in funcElement.iterfind('Arg'):
                thisArg = MPIArgument()
                thisArg.name = argElement.attrib['name']
                thisArg.callName = thisArg.name
                if thisArg.name.strip().endswith('[]'):
                    thisArg.isPointer = True
                    thisArg.isPlural = True
                    thisArg.callName = thisArg.name.replace('[]', '')
                elif thisArg.name.strip().startswith('array_of'):
                    thisArg.isPointer = True
                    thisArg.isPlural = True
                if 'output' in argElement.attrib:
                    thisArg.isOutput = True
                    if 'input' in argElement.attrib:
                        thisArg.isInput = True
                else:
                    thisArg.isInput = True
                rawType = argElement.attrib['type'].strip()
                thisArg.type = rawType
                if rawType.endswith('*'):
                    thisArg.isPointer = True
                if rawType.startswith('MPI_'):
                    thisArg.isMPIType = True
                    if 'dims' in argElement.attrib:
                        thisArg.isPlural = True
                        thisArg.isPointer = True
                        thisArg.dims = argElement.attrib['dims']
                for convs in argElement.iterfind('Convert'):
                    isPre = True
                    if 'trigger' in convs.attrib:
                        if convs.attrib['trigger'] == 'post':
                            isPre = False
                    if isPre:
                        thisArg.convertValues.append(convs.text)
                    else:
                        thisArg.postconvertValues.append(convs.text)
                freeHandle = argElement.find('PostFreeHandle')
                if freeHandle is not None:
                    thisArg.freeHandle = True
                    thisArg.freedValue = freeHandle.text
                thisFunction.args.append(thisArg)
            self.functions.append(thisFunction)
                
    def debugValues(self):
        for aFunc in self.functions:
            print "Name: " + aFunc.name
            print "Returns: " + aFunc.returnType
            print "Arguments:"
            print "=========="
            for anArg in aFunc.args:
                print "    Name: " + anArg.name
                print "    Type: " + anArg.type
                if anArg.convertValues:
                    print "    Convert Values:"
                    for val in anArg.convertValues:
                        print "        " + val
                print
            print
    
    def _mpiCallString(self, aFunc, doIgnore):
        callRealMPI = 'mpi_error = '
        callRealMPI += aFunc.name + '('
        for i in range(len(aFunc.args)):
            anArg = aFunc.args[i]                
            if i > 0:
                callRealMPI += ", "
            if anArg.isMPIType and not anArg.type.startswith('MPI_Status'):
                printName = anArg.tempName
                if anArg.isPointer:
                    if not anArg.tempIsPtr:
                        printName = '&' + printName
                else:
                    if anArg.tempIsPtr:
                        printName = '*' + printName
            else:
                printName = anArg.callName
            if doIgnore:
                if aFunc.statusIgnore:
                    if i == aFunc.statusIgnoreArg:
                        printName = aFunc.statusIgnoreType
            callRealMPI += printName            
        callRealMPI += ');'
        return callRealMPI
    
    def _mpiConversions(self, aFunc):
        mpiTypes = ['MPI_Comm', 'MPI_Datatype', 'MPI_Info', 'MPI_File',
                    'MPI_Request', 'MPI_Group', 'MPI_Offset', 'MPI_Aint',
                    'MPI_Op']
        scalarFunc = {'MPI_Comm':'comm_to_mpi',
                      'MPI_Info':'info_to_mpi',
                      'MPI_File':'file_to_mpi',
                      'MPI_Datatype':'datatype_to_mpi',
                      'MPI_Request':'request_to_mpi',
                      'MPI_Group':'group_to_mpi',
                      'MPI_Offset':'offset_to_mpi',
                      'MPI_Aint':'aint_to_mpi',
                      'MPI_Op':'op_to_mpi'}
        arrayFunc = { 'MPI_Comm':'comm_array_to_mpi',
                      'MPI_Datatype':'datatype_array_to_mpi',
                      'MPI_Offset':'offset_array_to_mpi',
                      'MPI_Aint':'aint_array_to_mpi'}
        pscalarFunc = {'MPI_Comm':'add_new_comm',
                       'MPI_Info':'add_new_info',
                       'MPI_File':'add_new_file',
                       'MPI_Datatype':'add_new_datatype',
                       'MPI_Request':'add_new_request',
                       'MPI_Group':'add_new_group',
                       'MPI_Aint':'aint_to_yogi',
                       'MPI_Offset':'offset_to_yogi'}
        parrayFunc = {'MPI_Offset':'offset_array_to_yogi',
                      'MPI_Datatype':'datatype_array_to_yogi',
                      'MPI_Aint':'aint_array_to_yogi'}
        for i in range(len(aFunc.args)):
            anArg = aFunc.args[i]
            # MPI_Status is handled specially; don't do it here.
            if anArg.type.startswith('MPI_Status'):
                continue
            if anArg.isMPIType:
                anArg.tempName = 'conv_' + anArg.callName
                for mpiType in mpiTypes:
                    if anArg.type.startswith(mpiType):
                        anArg.tempType = mpiType
                if anArg.isInput:
                    if not anArg.isPlural:
                        anArg.tempPreConverter = scalarFunc[anArg.tempType]
                        anArg.tempIsPtr = False
                    elif anArg.isPlural:
                        anArg.tempIsPtr = True
                        anArg.tempPreConverter = arrayFunc[anArg.tempType]
                if anArg.isOutput:
                    if not anArg.isPlural:
                        anArg.tempPostConverter = pscalarFunc[anArg.tempType]
                    elif anArg.isPlural:
                        anArg.tempPostConverter = parrayFunc[anArg.tempType]
                                
    def _statusConvLine(self, aFunc, phase):
        convLine = ''
        if phase == 'input':
            if aFunc.statusIgnore:
                theArg = aFunc.args[aFunc.statusIgnoreArg]
                convLine += '        MPI_Status * ' + theArg.callName +\
                        ' = yogi_status_to_mpi(' + theArg.name + ');\n'
        elif phase == 'output':
            if aFunc.statusIgnore:
                theArg = aFunc.args[aFunc.statusIgnoreArg]
                convLine += '        mpi_status_to_yogi(' + theArg.callName +\
                            ', ' + theArg.name + ');\n'

        return convLine

    def _cleanUpTmpArrays(self, aFunc):
        cleanMap= { 'MPI_Comm':'free_comm_array',
                    'MPI_Datatype':'free_datatype_array',
                    'MPI_Offset':'free_offset_array',
                    'MPI_Aint':'free_aint_array'}
        cleanUpLines = ''
        for i in range(len(aFunc.args)):
            anArg = aFunc.args[i]
            if anArg.type.startswith('MPI_Status'):
                continue
            if anArg.isMPIType:
                if anArg.isPlural:
                    cleanUpLines += '    ' + cleanMap[anArg.tempType] + '(' +\
                                    anArg.tempName + ');\n'
        return cleanUpLines
    
    def _mpiConvLines(self, aFunc, phase):
        convLine = ''
        if phase == 'input':
            for i in range(len(aFunc.args)):
                anArg = aFunc.args[i]
                if anArg.type.startswith('MPI_Status'):
                    continue
                if anArg.isMPIType:
                    convLine += '    ' + anArg.tempType + ' ' +\
                                anArg.tempName
                    if anArg.tempPreConverter:
                        convName = anArg.callName
                        if not anArg.isPlural:
                            if anArg.isPointer:
                                convName = "*" + convName
                        convLine += ' = ' + anArg.tempPreConverter + '(' +\
                                    convName
                        if anArg.isPlural:
                            convLine += ', '
                            dimArg = aFunc.getArg(anArg.dims)
                            if dimArg.isPointer:
                                convLine += '*'
                            convLine += anArg.dims
                        convLine += ');\n'
                    else:
                        convLine += ';\n'
                                   
        elif phase == 'output':
            for i in range(len(aFunc.args)):
                anArg = aFunc.args[i]
                if anArg.type.startswith('MPI_Status'):
                    continue
                if anArg.tempPostConverter:
                    convLine += '    '
                    if not anArg.tempIsPtr:
                        if anArg.isPointer:
                            convLine += '*'
                    convLine += anArg.name + ' = ' +\
                                anArg.tempPostConverter + '('
                    if anArg.tempIsPtr:
                        if not anArg.isPointer:
                            convLine += '*'
                    convLine += anArg.tempName + ');\n'
                
        return convLine
    
    def _freeHandleLines(self, aFunc):
        handleMap = {'MPI_Comm':'comm_pool',
                     'MPI_Group':'group_pool',
                     'MPI_Info':'info_pool',
                     'MPI_File':'file_pool',
                     'MPI_Datatype':'datatype_pool'}
        handleLines = ''
        for i in range(len(aFunc.args)):
            anArg = aFunc.args[i]
            if anArg.type.startswith('MPI_Status'):
                continue
            if anArg.freeHandle:
                poolName = handleMap[anArg.tempType]
                handleLines += '    ' + poolName + '['
                if anArg.isPointer:
                    handleLines += '*'
                handleLines += anArg.name + "] = " + anArg.freedValue + ";\n"
                handleLines += '    '
                if anArg.isPointer:
                    handleLines += '*'
                handleLines += anArg.name + ' = Yogi' +\
                               anArg.freedValue + ';\n'
        return handleLines

    def _makeConversionLines(self, anArg, convValues, isInput):
        convLines = ''
        for i in range(len(convValues)):
                if anArg.isInput:
                    compareValue = self.prefix + val
                    changeValue = val
                elif anArg.isOutput:
                    compareValue = val
                    changeValue = self.prefix + val
            val = convValues[i]
            refAssign = anArg.name
            if anArg.isPointer:
                refAssign = '*' + refAssign
                if i == 0:
                    convLines += '    if ('
                else:
                    convLines += '    else if ('
                convLines += refAssign + ' == ' + compareValue +\
                            ') {\n' +\
                            '        ' + refAssign + ' = ' + changeValue +\
                            ';\n    }\n'
        return convLines
    
    def _constantConversions(self, aFunc, phase):
        convLines = '' 
        for anArg in aFunc.args:
            if not anArg.convertValues:
                continue
            if phase == 'input':
                if not anArg.isInput:
                    continue
                else:
                    self._makeConversionLines(self, anArg, anArg.convertValues)                    
            elif phase == 'output':
                if not anArg.isOutput:
                    continue
            self._makeConversionLines(self, anArg, )                    
            for i in range(len(anArg.convertValues)):


        return convLines
                                        
    def writeFiles(self):
        headerLines = []
        convHeaderLines = []
        sourceLines = []
        ppLines = []
        headerFile = open(self.filePrefix + '.h', 'w')
        sourceFile = open(self.filePrefix + '.c', 'w')
        convFile = open(self.filePrefix + 'toyogi.h', 'w')
        ppFile = open(self.filePrefix + '_cpp.xml', 'w')
        for aFunc in self.functions:
            fProto = aFunc.returnType + ' Yogi' + aFunc.name + '('
            for i in range(len(aFunc.args)):
                anArg = aFunc.args[i]
                if i > 0:
                    fProto += ", "
                if anArg.isMPIType:
                    argType = self.prefix + anArg.type
                else:
                    argType = anArg.type
                fProto += argType + " " + anArg.name
            fProto += ');\n'
            fProto += '\n'
            headerLines.append(fProto)
            convHeaderLines.append('#define ' + aFunc.name + ' Yogi' +\
                                   aFunc.name + '\n')
            ppLines.append('    <Function>' + aFunc.name + '</Function>\n')
        headerFile.writelines(headerLines)
        headerFile.close()
        convFile.writelines(convHeaderLines)
        convFile.close()
        ppFile.writelines(ppLines)
        ppFile.close()

        for aFunc in self.functions:
            self._mpiConversions(aFunc)
            # There are a few things that must be checked.
            # - If there is an output MPI_Status (or MPI_Status plural), the 
            #   check for MPI_STATUSES_IGNORE or MPI_STATUS_IGNORE must offer
            #   a branch call (done).
            # - Check for any manual comparison/converts on arguments (done).
            # - Convert scalar input arguments to proper format (done).
            # - Convert array input arguments to proper format (done).
            # - Convert scalar output arguments to proper format (done).
            # - Convert array output arguments to proper format (N/A).
            # - Free objects that are being freed (done).
            for i in range(len(aFunc.args)):
                anArg = aFunc.args[i]
                if anArg.type.startswith('MPI_Status'):
                    if anArg.isOutput:
                        aFunc.statusIgnore = True
                        aFunc.statusIgnoreArg = i
                        aFunc.args[i].callName = 'in_' + aFunc.args[i].name
                        if anArg.isPlural:
                            aFunc.statusIgnoreType = 'MPI_STATUSES_IGNORE'
                        else:
                            aFunc.statusIgnoreType = 'MPI_STATUS_IGNORE'
                                            
            openLine = aFunc.returnType + ' Yogi' + aFunc.name + '('
            for i in range(len(aFunc.args)):
                anArg = aFunc.args[i]
                if i > 0:
                    openLine += ", "
                if anArg.isMPIType:
                    argType = self.prefix + anArg.type
                else:
                    argType = anArg.type
                openLine += argType + " " + anArg.name
            openLine += ') {\n\n'
            sourceLines.append(openLine)
            callLine = '    int mpi_error;\n'
            sourceLines.append(self._mpiConvLines(aFunc, 'input'))
            sourceLines.append(self._constantConversions(aFunc, 'input'))

            withoutIgnore = self._mpiCallString(aFunc, False)
            if aFunc.statusIgnore:
                # Optional STATUS_IGNORE must be recognized.
                withIgnore = self._mpiCallString(aFunc, True)
                callLine += '    if (' + aFunc.args[aFunc.statusIgnoreArg].name +\
                            ' == ' + self.prefix + aFunc.statusIgnoreType +\
                            ') {\n' +\
                            '        ' + withIgnore +\
                            '\n    }\n' +\
                            '    else {\n' + self._statusConvLine(aFunc, 
                                                                  'input') +\
                            '        ' + withoutIgnore +\
                            '\n' + self._statusConvLine(aFunc, 'output') +\
                            '\n    }\n'
            else:
                callLine += '    ' + withoutIgnore + '\n'
                
            sourceLines.append(callLine)
            sourceLines.append(self._constantConversions(aFunc, 'output'))
            sourceLines.append(self._mpiConvLines(aFunc, 'output'))
            sourceLines.append(self._freeHandleLines(aFunc))
            sourceLines.append(self._cleanUpTmpArrays(aFunc))
            sourceLines.append('    return error_to_yogi(mpi_error);\n')
            sourceLines.append('}\n\n')        
        sourceFile.writelines(sourceLines)
        sourceFile.close()
            
if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: python GenerateWrap.py input.xml prefix")
    else:
        GenerateWrap(sys.argv[1], sys.argv[2])

