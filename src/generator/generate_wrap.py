import xml.etree.ElementTree as ET
import sys
import wrap_objects
from pprint import pprint

class GenerateWrap(object):

    # Define a few things up front as class variables.
    mpiHandles = [ 'MPI_Comm', 'MPI_Datatype', 'MPI_Info', 'MPI_File',
                   'MPI_Request', 'MPI_Group', 'MPI_Op', 'MPI_Errhandler',
                   'MPI_Win' ]

    mpiTypeDefs = [ 'MPI_Offset', 'MPI_Aint' ]

    mpiTypes = mpiHandles + mpiTypeDefs

    def __init__(self, xmlInput):
        # Prefix for all the functions to wrap.
        self.prefix = 'Yogi'
        # Parsed functions to wrap.
        self.functions = []

        self.parseInput(xmlInput)
        self.writeFiles()
       
    ## Parses an XML input and creates MPI functions and their arguments.
    #  @param self Pointer to object.
    #  @param fileName XML filename. 
    def parseInput(self, fileName):
        tree = ET.parse(fileName)
        xmlRoot = tree.getroot()
        for funcElement in xmlRoot.findall('Function'):
            thisFunction = wrap_objects.MPIFunction()
            thisFunction.name = funcElement.attrib['name']
            thisFunction.return_type = funcElement.find('ReturnType').text

            for argElement in funcElement.iterfind('Arg'):
                thisArg = wrap_objects.MPIArgument()
                thisArg.name = argElement.attrib['name']
                # Start out simple.
                thisArg.call_name = thisArg.name
                if thisArg.name.strip().endswith('[]'):
                    # An array is considered a pointer.
                    thisArg.is_pointer = True
                    thisArg.is_plural = True
                    # Brackets aren't used to reference an array variable.
                    thisArg.call_name = thisArg.name.replace('[]', '')
                elif thisArg.name.strip().startswith('array_of'):
                    thisArg.is_pointer = True
                    thisArg.is_plural = True
                if self._checkTrue(argElement.attrib.get('output', None)):
                    thisArg.is_output = True
                    # Only make it input after specifying output if both are
                    # explicitly listed.
                    if self._checkTrue(argElement.attrib.get('input', None)):
                        thisArg.is_input = True
                else:
                    # Assume that if nothing else is specified, this is an
                    # input argument.
                    thisArg.is_input = True
                rawType = argElement.attrib['type'].strip()
                thisArg.type = rawType
                if rawType.endswith('*'):
                    thisArg.is_pointer = True
                if rawType.startswith('MPI_') and \
                   not rawType.endswith('function*'):
                    thisArg.is_mpi_type = True
                    if argElement.attrib.get('dims', None):
                        thisArg.is_plural = True
                        thisArg.is_pointer = True
                        thisArg.dims = argElement.attrib['dims']
                for conv in argElement.iterfind('Convert'):
                    val = wrap_objects.MPIConvertValue()
                    val.name = conv.text
                    isPtr = conv.attrib.get('pointer', None)
                    val.is_pointer = self._checkTrue(isPtr)
                    isFunc = conv.attrib.get('function', None)
                    val.is_function = self._checkTrue(isFunc)
                    isPre = True
                    if conv.attrib.get('trigger', None) == 'post':
                        thisArg.pre_convert_values.append(val)
                    else:
                        thisArg.post_convert_values.append(val)
                freeHandle = argElement.find('PostFreeHandle')
                if freeHandle is not None:
                    thisArg.free_handle = True
                    thisArg.freed_value = freeHandle.text
                thisFunction.args.append(thisArg)
            self.functions.append(thisFunction)
               
    ## Generates the line that invokes the actual underlying MPI function. 
    #  @param self Pointer to object.
    #  @param aFunc The MPI function to be invoked.
    #  @param doIgnore Whether or not to create an MPI_STATUS_IGNORE branch.
    def _mpiCallString(self, aFunc, doIgnore):
        callRealMPI = 'mpi_error = '
        callRealMPI += aFunc.name + '('
        for i in range(len(aFunc.args)):
            anArg = aFunc.args[i]
            if i > 0:
                callRealMPI += ", "
            if anArg.is_mpi_type and not anArg.type.startswith('MPI_Status'):
                printName = anArg.temp_name
                if anArg.is_pointer:
                    if not anArg.temp_is_ptr:
                        printName = '&' + printName
                else:
                    if anArg.temp_is_ptr:
                        printName = '*' + printName
            else:
                printName = anArg.call_name
            if doIgnore:
                if aFunc.status_ignore_arg:
                    if i == aFunc.status_ignore_arg:
                        printName = aFunc.status_ignore_type
            callRealMPI += printName            
        callRealMPI += ');'
        return callRealMPI
   
    ## Handles setting up conversion code for MPI types. These always have to
    #  be converted before (for input) and after (for output) MPI function
    #  calls. 
    def _mpiConversions(self, aFunc):

        for i in range(len(aFunc.args)):
            anArg = aFunc.args[i]
            # MPI_Status is handled specially; don't do it here.
            if anArg.type.startswith('MPI_Status'):
                continue
            if anArg.is_mpi_type:
                anArg.temp_name = 'conv_' + anArg.call_name
                for mpiType in GenerateWrap.mpiTypes:
                    if anArg.type.startswith(mpiType):
                        anArg.temp_type = mpiType
                if anArg.is_input:
                    if not anArg.is_plural:
                        anArg.temp_pre_converter = anArg.temp_type + "ToMPI"
                        anArg.temp_is_ptr = False
                    elif anArg.is_plural:
                        anArg.temp_is_ptr = True
                        anArg.temp_pre_converter = anArg.temp_type +\
                                                   "ArrayToMPI"
                if anArg.is_output:
                    if not anArg.is_plural:
                        anArg.temp_post_converter = anArg.temp_type + "ToYogi"
                    elif anArg.is_plural:
                        anArg.temp_post_converter = anArg.temp_type +\
                                                    "ArrayToYogi" 
                                
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
                     'MPI_Op':'op_pool',
                     'MPI_Errhandler':'errhandler_pool',
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

    def _constantConversions(self, aFunc, phase):
        convLines = '' 
        for anArg in aFunc.args:
            if not anArg.convertValues:
                continue
            if phase == 'input':
                if not anArg.isInput:
                    continue
            elif phase == 'output':
                if not anArg.isOutput:
                    continue
            for i in range(len(anArg.convertValues)):
                val = anArg.convertValues[i]
                if anArg.isInput:
                    compareValue = self.prefix + val
                    changeValue = val
                elif anArg.isOutput:
                    compareValue = val
                    changeValue = self.prefix + val
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

    def writeFiles(self):        
        self.writeHeader()
        self.writeUserHeader()
        self.writeSource()

    # Writes the header file that users will include in their program.
    def writeUserHeader(self):
        convFile = open('mpitoyogi.h', 'w')
        convLines = []
        for aFunc in self.functions:
            convLines.append('#define ' + aFunc.name + ' Yogi' + aFunc.name +\
                             '\n')
        convFile.writelines(convLines)
        convFile.close()

    # Writes the actual header file for YogiMPI.
    def writeHeader(self):
        headerLines = []
        headerFile = open('yogimpi.h', 'w')
        for aFunc in self.functions:
            fProto = aFunc.return_type + ' ' + self.prefix + aFunc.name + '('
            for i in range(len(aFunc.args)):
                anArg = aFunc.args[i]
                if i > 0:
                    fProto += ", "
                if anArg.is_mpi_type:
                    argType = self.prefix + anArg.type
                else:
                    argType = anArg.type
                fProto += argType + " " + anArg.name
            fProto += ');\n'
            fProto += '\n'
            headerLines.append(fProto)
        headerFile.writelines(headerLines)
        headerFile.close()

    ## Writes the internal source file for YogiMPI.
    def writeSource(self):
        sourceLines = []
        sourceFile = open('yogimpi.cxx', 'w')

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
                #if anArg.type.startswith('MPI_Status'):
                #    if anArg.is_output:
                #        aFunc.status_ignore = True
                #        aFunc.status_ignore_arg = i
                #        aFunc.args[i].call_name = 'in_' + aFunc.args[i].name
                #        if anArg.is_plural:
                #            aFunc.status_ignore_type = 'MPI_STATUSES_IGNORE'
                #        else:
                #            aFunc.status_ignore_type = 'MPI_STATUS_IGNORE'
                                            
            openLine = aFunc.return_type + ' ' + self.prefix + aFunc.name + '('
            for i in range(len(aFunc.args)):
                anArg = aFunc.args[i]
                if i > 0:
                    openLine += ", "
                argType = anArg.type
                if anArg.is_mpi_type:
                    argType = self.prefix + argType
                openLine += argType + " " + anArg.name
            openLine += ') {\n\n'
            sourceLines.append(openLine)
            callLine = '    int mpi_error;\n'
            #sourceLines.append(self._mpiConvLines(aFunc, 'input'))
            #sourceLines.append(self._constantConversions(aFunc, 'input'))

            withoutIgnore = self._mpiCallString(aFunc, False)
            #if aFunc.statusIgnore:
                # Optional STATUS_IGNORE must be recognized.
            #    withIgnore = self._mpiCallString(aFunc, True)
            #    callLine += '    if (' + aFunc.args[aFunc.statusIgnoreArg].name +\
            #                ' == ' + self.prefix + aFunc.statusIgnoreType +\
            #                ') {\n' +\
            #                '        ' + withIgnore +\
            #                '\n    }\n' +\
            #                '    else {\n' + self._statusConvLine(aFunc, 
            #                                                      'input') +\
            #                '        ' + withoutIgnore +\
            #                '\n' + self._statusConvLine(aFunc, 'output') +\
            #                '\n    }\n'
            #else:
            callLine += '    ' + withoutIgnore + '\n'
                
            sourceLines.append(callLine)
            #sourceLines.append(self._constantConversions(aFunc, 'output'))
            #sourceLines.append(self._mpiConvLines(aFunc, 'output'))
            #sourceLines.append(self._freeHandleLines(aFunc))
            #sourceLines.append(self._cleanUpTmpArrays(aFunc))
            sourceLines.append('    return error_to_yogi(mpi_error);\n')
            sourceLines.append('}\n\n')        
        sourceFile.writelines(sourceLines)
        sourceFile.close()

    def _checkTrue(self, value):
        if not value:
            return False
        if value.lower() == 'true':
            return True
        return False            

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: python generate_wrap.py input.xml")
    else:
        GenerateWrap(sys.argv[1])

