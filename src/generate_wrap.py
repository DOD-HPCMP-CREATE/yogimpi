"""
                                  COPYRIGHT
 
 The following is a notice of limited availability of the code, and disclaimer
 which must be included in the prologue of the code and in all source listings
 of the code.
 
 Copyright Notice
  + 2002 University of Chicago
  + 2016 Stephen Adamec

 Permission is hereby granted to use, reproduce, prepare derivative works, and
 to redistribute to others.  This software was authored by:
 
 Mathematics and Computer Science Division
 Argonne National Laboratory, Argonne IL 60439
 
 (and)
 
 Department of Computer Science
 University of Illinois at Urbana-Champaign

 (and)

 Stephen Adamec
 
                              GOVERNMENT LICENSE
 
 Portions of this material resulted from work developed under a U.S.
 Government Contract and are subject to the following license: the Government
 is granted for itself and others acting on its behalf a paid-up, nonexclusive,
 irrevocable worldwide license in this computer software to reproduce, prepare
 derivative works, and perform publicly and display publicly.
  
                                    DISCLAIMER
  
 This computer code material was prepared, in part, as an account of work
 sponsored by an agency of the United States Government.  Neither the United
 States, nor the University of Chicago, nor any of their employees, makes any
 warranty express or implied, or assumes any legal liability or responsibility
 for the accuracy, completeness, or usefulness of any information, apparatus,
 product, or process disclosed, or represents that its use would not infringe
 privately owned rights.
"""

import xml.etree.ElementTree as ET
import sys
import wrap_objects
import source_writers

class GenerateWrap(object):

    # Define a few things up front as class variables.
    mpiHandles = [ 'MPI_Comm', 'MPI_Datatype', 'MPI_Info', 'MPI_File',
                   'MPI_Request', 'MPI_Group', 'MPI_Op', 'MPI_Errhandler',
                   'MPI_Win' ]

    mpiObjects = [ 'MPI_Status' ]

    mpiTypeDefs = [ 'MPI_Offset', 'MPI_Aint' ]

    mpiTypes = mpiHandles + mpiTypeDefs + mpiObjects

    manPrefix = "YogiManager::getInstance()->"

    #mpiFunctionMap = { 'MPI_User_function': 'UserFunction' }
    mpiFunctionMap = { }

#                 'MPI_Comm_errhandler_function': 'CommErrhandlerFunction',
#                 'MPI_Copy_function':'CopyFunction',
#                 'MPI_Delete_function':'DeleteFunction',
#                 'MPI_Type_copy_attr_function': 'TypeCopyFunction',
#                 'MPI_Type_delete_attr_function': 'TypeDeleteFunction',
#                 'MPI_Win_copy_attr_function': 'WinCopyFunction',
#                 'MPI_Win_delete_attr_function': 'WinDeleteFunction',
#                 'MPI_File_errhandler_function': 'FileErrhandlerFunction',
#                 'MPI_Win_errhandler_function': 'WinErrhandlerFunction',
#                 'MPI_Grequest_cancel_function': 'GrequestCancelFunction',
#                'MPI_Datarep_conversion_function': 'DatarepConversionFunction',
#                 'MPI_Datarep_extent_function': 'DatarepExtentFunction',
#                 'MPI_Comm_copy_attr_function': 'CommCopyFunction',
#                 'MPI_Comm_delete_attr_function': 'CommDeleteFunction'

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
            fortranSupport = funcElement.find('FortranSupport')
            if fortranSupport is not None:
                if fortranSupport.text == 'no':
                    thisFunction.fortran_support = False 
            for codeElement in funcElement.findall('Code'):
                order = codeElement.attrib.get('order', None)
                if order is None:
                    raise ValueError("Code block in " + thisFunction.name +\
                                     " missing order.")
                thisFunction.addBlock(codeElement.text, order)

            for argElement in funcElement.findall('Arg'):
                thisArg = wrap_objects.MPIArgument()
                thisArg.name = argElement.attrib['name']
                # Start out simple.
                thisArg.call_name = thisArg.name
                if thisArg.name.strip().endswith(']'):
                    # An array is considered a pointer.
                    thisArg.is_pointer = True
                    thisArg.is_plural = True
                    # Brackets aren't used to reference an array variable.
                    thisArg.call_name = thisArg.name.split('[')[0]
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

                if argElement.attrib.get('dims', None):
                    thisArg.is_plural = True
                    thisArg.is_pointer = True
                    thisArg.dims = argElement.attrib['dims']

                if rawType.startswith('MPI_'):
                    if not rawType.endswith('function*'):
                        thisArg.is_mpi_type = True
                        thisArg.mpi_name = 'conv_' + thisArg.call_name
                        for mpiType in GenerateWrap.mpiTypes:
                            if thisArg.type.startswith(mpiType):
                                thisArg.mpi_type = mpiType
                        isStatusInput = (thisArg.mpi_type == 'MPI_Status') and \
                                        thisArg.is_input
                        if isStatusInput:
                            # An input MPI_Status always gets a pointer. 
                            thisArg.mpi_is_ptr = True
                        elif thisArg.is_plural:
                            # A pointer will be allocated with new on the heap.
                            thisArg.mpi_is_ptr = True
                        else:
                            # The object will just be created on the stack, and
                            # the & symbol is used to reference the address.
                            thisArg.mpi_is_ptr = False
                    else:
                        thisArg.is_mpi_type = True
                        thisArg.is_mpi_ptr = True
                        thisArg.mpi_func_ptr = True
                        thisArg.mpi_type = thisArg.type
                        thisArg.mpi_name = 'conv_' + thisArg.call_name

                freeVal = argElement.attrib.get('free', None)
                thisArg.free_handle = self._checkTrue(freeVal)
                thisArg.convert_class = argElement.attrib.get('class', None)
                for conv in argElement.findall('Convert'):
                    if thisArg.convert_class:
                        loc = "Function " + thisFunction.name + ", argument " +\
                              thisArg.name + ": " 
                        errMsg = "Convert tag ignored when class specified."
                        raise ValueError(loc + errMsg)
                    val = wrap_objects.MPIConvertValue()
                    val.name = conv.text
                    isPtr = conv.attrib.get('pointer', None)
                    val.is_pointer = self._checkTrue(isPtr)
                    if conv.attrib.get('trigger', None) == 'post':
                        thisArg.post_convert_values.append(val)
                    else:
                        thisArg.pre_convert_values.append(val)
                thisFunction.args.append(thisArg)
            self.functions.append(thisFunction)
               
    ## Generates the line that invokes the actual underlying MPI function. 
    #  @param self Pointer to object.
    #  @param aFunc The MPI function to be invoked.
    #  @param doIgnore Whether or not to create an MPI_STATUS_IGNORE branch.
    def _mpiCallString(self, aFunc, doIgnore):
        callRealMPI = 'mpi_error = '
        callRealMPI += aFunc.name + '('
        for i, anArg in enumerate(aFunc.args):
            if i > 0:
                callRealMPI += ", "
            if anArg.mpi_func_ptr:
                # For now, pass function pointers as they are.
                printName = anArg.mpi_name
            elif anArg.is_mpi_type and not anArg.type.startswith('MPI_Status'):
                printName = anArg.mpi_name
                if anArg.is_pointer:
                    if not anArg.mpi_is_ptr:
                        printName = '&' + printName
                else:
                    if anArg.mpi_is_ptr:
                        printName = '*' + printName
            elif anArg.type.startswith('MPI_Status'):
                if anArg.is_input and anArg.is_pointer:
                    # MPI_Status input arguments are always pointers.
                    printName = anArg.mpi_name 
                elif anArg.is_pointer and not anArg.is_plural:
                    # An MPI_Status argument that is scalar and only 
                    # output is almost always a pointer, but a conversion
                    # variable is normally passed.  Therefore we need the
                    # & to pass the address.
                    printName = '&' + anArg.mpi_name
                elif anArg.is_plural:
                    printName = anArg.mpi_name
            else:
                printName = anArg.call_name
            if doIgnore:
                if aFunc.status_ignore_arg:
                    if i == aFunc.status_ignore_arg:
                        printName = aFunc.status_ignore_type
            callRealMPI += printName            
        callRealMPI += ');'
        return callRealMPI
   
    def _statusOutputLines(self, sourceFile, aFunc, phase):
        convPrefix = GenerateWrap.manPrefix + 'statusTo'
        if phase == 'input':
            if aFunc.status_ignore:
                theArg = aFunc.args[aFunc.status_ignore_arg]
                tempVarDecl = 'MPI_Status '
                if theArg.is_plural:
                    tempVarDecl += '* '
                tempVarDecl += theArg.mpi_name
                if theArg.is_plural:
                    tempVarDecl += ' = NULL'
                sourceFile.addLines(tempVarDecl + ';')
                if theArg.is_plural:
                    if theArg.dims is None:
                        msg = 'Func ' + aFunc.name + ', arg ' + theArg.name +\
                              ' has no dimensions.'
                        raise ValueError(msg)
                    convLine = GenerateWrap.manPrefix + 'createStatus(' +\
                               theArg.mpi_name + ', ' + theArg.dims + ');'
                    sourceFile.addLines(convLine)
        elif phase == 'output':
            if aFunc.status_ignore:
                outToYogi = ''
                theArg = aFunc.args[aFunc.status_ignore_arg]
                if theArg.is_pointer and not theArg.is_plural:
                    outToYogi += '*'
                if not theArg.is_plural:
                    outToYogi += theArg.call_name  + ' = ' + convPrefix +\
                                 'Yogi(' + theArg.mpi_name + ');'
                if theArg.is_plural:
                    arrayConv = GenerateWrap.manPrefix + 'statusToYogi'
                    outToYogi = arrayConv + '(' + theArg.mpi_name + ', ' +\
                                theArg.call_name + ', ' + theArg.dims +\
                                ', true);'
                sourceFile.addLines(outToYogi) 

    def _makeConstantCall(self, sourceFile, anArg, before=True):
        if anArg.is_mpi_type:
            errMsg = "Constant functions unsupported for MPI arguments."
            raise ValueError(errMsg)
        
        convPrefix = GenerateWrap.manPrefix + anArg.convert_class 
        if before:
            classFunc = convPrefix + "ToMPI"
        else:
            classFunc = convPrefix + "ToYogi"
        callString = ''
        if anArg.is_pointer:
            callString += '*'
        callString += anArg.call_name + ' = ' + classFunc + '('
        if anArg.is_pointer:
            callString += '*'
        callString += anArg.call_name + ');'
        sourceFile.addLines(callString);

    def _makeConstantCheck(self, sourceFile, anArg, before=True):
        if before:
            values = anArg.pre_convert_values
        else:
            values = anArg.post_convert_values

        if values is None:
            return 

        if not anArg.is_mpi_type:
            compareVar = anArg.call_name
            assignVar = anArg.call_name
        else:
            if before:
                compareVar = anArg.call_name
                assignVar = anArg.mpi_name
            else:
                compareVar = anArg.mpi_name
                assignVar = anArg.call_name
        
        for i, convVal in enumerate(values):
            loopAssignVar = assignVar
            loopCompareVar = compareVar
            if before:
                compareValue = self.prefix + convVal.name
                changeValue = convVal.name
            else:
                compareValue = convVal.name
                changeValue = self.prefix + convVal.name
            if anArg.is_pointer:
                if anArg.is_mpi_type:
                    if not convVal.is_pointer:
                        loopAssignVar = '*' + assignVar
                        if anArg.mpi_is_ptr:
                            loopCompareVar = '*' + assignVar
                elif not convVal.is_pointer:
                    loopAssignVar = '*' + assignVar
                    loopCompareVar = '*' + compareVar
            elif convVal.is_pointer:
                errMsg = "Assigning a pointer to a non-pointer."
                raise ValueError(errMsg)

            compareStr = loopCompareVar + ' == ' + compareValue
            if i == 0:
                sourceFile.addIf(compareStr)
            else:
                sourceFile.addElseIf(compareStr)
            sourceFile.addLines(loopAssignVar + ' = ' + changeValue + ';')
            if i == 0:
                sourceFile.endIf()
            else:
                sourceFile.endElseIf()

    ## Appends the generated source code lines to convert an MPI item back and
    #  forth between Yogi and MPI types. 
    def _makeMPIConversion(self, sourceFile, anArg, before=True):

        # For now, just pass function pointers as they are.
        if anArg.mpi_func_ptr:
            return
        
        if anArg.is_plural and not anArg.dims:
            msg = "arg " + anArg.name + " does not have dimensions."
            raise ValueError(msg)

        stripType = anArg.mpi_type.replace('MPI_', '')
        convPrefix = GenerateWrap.manPrefix + stripType.lower() 

        if before:
            convFunc = convPrefix + "ToMPI"
            returnVal = anArg.mpi_name
            inputArg = anArg.call_name
        else:
            convFunc = convPrefix + "ToYogi"
            returnVal = anArg.call_name
            inputArg = anArg.mpi_name
 
        callString = ''
        if not before and anArg.free_handle:
            # If this is the "after" phase and this should be freed,
            # don't do a conversion, just empty the handle.
            if anArg.is_pointer:
                callString += '*'          
            callString += returnVal + ' = ' + GenerateWrap.manPrefix +\
                          'unmap' + stripType + '('
            if anArg.is_pointer:
                callString += '*'
            callString += anArg.call_name + ');'
        elif anArg.mpi_type == 'MPI_Status' and anArg.is_input:
            # We get a pointer back, always, and we supply the pointer.
            # No handling pluralism here; as of this writing, there is no
            # array input argument for MPI_Status.
            if before:
                callString += returnVal + ' = ' + convFunc + '(' + inputArg +\
                              ');' 
            else:
                callString += '*' + returnVal + ' = ' + convFunc + '(*' +\
                              inputArg + ');'
        elif anArg.is_plural and anArg.dims:
            # Array conversion functions don't produce a return value.  This
            # becomes the second argument, passed by reference, which is
            # modified in-place.
            callString += convFunc + '(' + inputArg + ', ' + returnVal + ', ' +\
                          anArg.dims
            if not before:
                callString += ', true' 
            callString += ');'
        else:
            if (not before and anArg.is_pointer):
                # If this is a conversion after the MPI call, dereference the
                # argument pointer and assign a new value.
                callString += '*'
            callString += returnVal + ' = ' + convFunc + '('
            if before and anArg.is_pointer:
                # If this is a conversion before the MPI call, dereference the
                # argument pointer so that it may be converted to MPI.
                callString += '*'
            callString += inputArg + ');' 

        sourceFile.addLines(callString);

    def writeFiles(self):        
        self.writeCHeader()
        self.writeCUserHeader()
        self.writeCXXSource()
        self.writeFortranBridge()
        self.writeFortranSource()

    # Writes the C++ code that binds YogiMPI to the Fortran layer.
    def writeFortranBridge(self):
        cxxInput = 'yogimpi_f90bridge.cxx.in'
        bridge_file = source_writers.CSource(inputFile=cxxInput)
        bridge_funcs = source_writers.CSource()
        bridge_defs = source_writers.CSource()  
        toCStrFunc = 'copy_and_add_null_terminator'
        allocCStrFunc = 'allocate_and_add_null_terminator'
        strBackToFortran = 'copy_without_null_terminator'
        for aFunc in self.functions:
            if not aFunc.fortran_support:
                continue
            # Figure out which arguments are strings.
            stringArgs = []
            for anArg in aFunc.args:
                if anArg.type.startswith('char') and anArg.is_pointer:
                    stringArgs.append(anArg) 
            fUpper = 'YOGIBRIDGE_' + aFunc.name.upper()
            fLower = fUpper.lower() + '_'
            bridge_defs.addLines('#define ' + fUpper + ' ' + fLower) 
            funcArgs = self._getBridgeArgsString(aFunc)
            bridge_funcs.addFunction(fUpper, 'void', funcArgs) 
            for anArg in stringArgs:
                copyLine = 'char * conv_' + anArg.name + ' = '
                if anArg.is_input:
                    copyLine += toCStrFunc 
                else:
                    copyLine += allocCStrFunc
                copyLine += '(' + anArg.name + ', ' + anArg.name + '_len);'
                bridge_funcs.addLines(copyLine)
            callLine = '*ierr = Yogi' + aFunc.name + '(' +\
                       self._getBridgeCallString(aFunc) + ');' 
            bridge_funcs.addLines(callLine)
            for anArg in stringArgs:
                if anArg.is_output:
                    copyLine = strBackToFortran + '(' + anArg.name +\
                               ', conv_' + anArg.name + ', ' + anArg.name +\
                               '_len);'
                    bridge_funcs.addLines(copyLine)
            bridge_funcs.endFunction(fUpper)
            bridge_funcs.newLine()

        bridge_file.merge(bridge_defs, 'FUNCTION_DEFINES')
        bridge_file.merge(bridge_funcs, 'BRIDGE_FUNCTIONS')
        bridge_file.writeFile('yogimpi_f90bridge.cxx')

    # Writes the Fortran module that binds YogiMPI to Fortran.
    def writeFortranSource(self):
        fInput = 'yogimpi_functions.f90.in'
        fort_file = source_writers.FortranSource(inputFile=fInput)
        fort_funcs = source_writers.FortranSource()
        for aFunc in self.functions:
            if not aFunc.fortran_support:
                continue
            fortName = 'YogiFortran_' + aFunc.name
            fortArgs = self._getFortArgsString(aFunc)
            fort_funcs.addSubroutine(fortName, fortArgs, implicit=False)
            fort_funcs.addLines('use yogimpi',
                                'implicit none')
            for anArg in aFunc.args:
                fort_funcs.addLines(self._getFortArgDecl(anArg))
                if self._isFortranLogical(anArg):
                    fort_funcs.addLines('integer :: ' + 'conv_' + anArg.name)
            fort_funcs.addLines('integer, intent(out) :: ierr')
            fort_funcs.newLine()
            for anArg in aFunc.args:
                if self._isFortranLogical(anArg):
                    if anArg.is_input:
                        fort_funcs.addLines('conv_' + anArg.name + ' = ' +\
                                            'Yogi_LogicalToInteger(' +\
                                            anArg.name + ')') 
            argString = self._getFortArgsString(aFunc, noConvert=False)
            fort_funcs.addLines('call YogiBridge_' + aFunc.name + '(' +\
                                argString + ')') 
            for anArg in aFunc.args:
                if self._isFortranLogical(anArg):
                    if anArg.is_output:
                        fort_funcs.addLines(anArg.name + ' = ' +\
                                            'Yogi_IntegerToLogical(' +\
                                            'conv_' + anArg.name + ')')
            fort_funcs.endSubroutine(fortName)
            fort_funcs.newLine()
        fort_file.merge(fort_funcs, 'YOGI_FUNCTIONS')
        fort_file.writeFile('yogimpi_functions.f90')

    # Returns a string with argument names suitable for the C++ Fortran
    # bridge declaration.
    def _getBridgeArgsString(self, func, ierr=True):
        argString = ''
        stringArgs = []
        for i, anArg in enumerate(func.args):
            if i > 0:
                argString += ', '
            # Strip all pointer and array argument information from the type.
            # Using the old-fashioned Fortran-to-C method, everything shows
            # up as a pointer.
            if anArg.is_mpi_type:
                argType = 'Yogi' + anArg.mpi_type + ' *'
            else:
                if anArg.type.startswith('char') and anArg.is_pointer:
                    stringArgs.append(anArg)
                argType = anArg.type.split(' ')[0].split('[')[0].strip('*')
                argType += ' *'
            argString += argType + anArg.call_name
        if ierr:
            # Add an ierror integer.
            argString += ', int *ierr'
        for anArg in stringArgs:
            argString += ', size_t ' + anArg.name + '_len'
        return argString

    # Returns a string with an argument call string suitable for C++ Fortran
    # bridge declaration.
    def _getBridgeCallString(self, func):
        callString = ''
        for i, anArg in enumerate(func.args):
            if i > 0:
                callString += ', '
            # Pointer to pointer goes as-is, anything else is dereferenced. 
            if anArg.type.startswith('MPI_Status'):
                if anArg.is_output and not anArg.is_input:
                    callString += 'check_status_ignore(' + anArg.call_name + ')'
                else:
                    # MPI_Status input arguments are always pointers.
                    callString += anArg.call_name
            elif anArg.type.startswith('char') and anArg.is_pointer:
                callString += 'conv_' + anArg.call_name
            elif anArg.is_pointer or anArg.is_plural:
                callString += anArg.call_name
            else:
                callString += '*' + anArg.call_name
        return callString

    # Returns a string with argument names suitable for Fortran subroutine
    # declaration.
    def _getFortArgsString(self, func, ierr=True, noConvert=True):
        argString = ''
        for i, anArg in enumerate(func.args):
            if i > 0:
                argString += ', '
            if self._isFortranLogical(anArg) and (not noConvert):
                argString += 'conv_' + anArg.call_name
            else:
                argString += anArg.call_name
        if ierr:
            # Add an ierror integer.
            argString += ', ierr'
        return argString

    # Returns a string that can be used to declare an argument's type and
    # intent within an ISO C-binding interface. 
    def _getISOCArgDecl(self, anArg):
        argDecl = ''
        intent = 'in'
        if anArg.is_mpi_type:
            if anArg.mpi_type == 'MPI_Address':
                isoType = 'integer(kind=C_LONG_LONG)'
            elif anArg.mpi_type == 'MPI_Offset':
                isoType = 'integer(kind=C_LONG_LONG)'
            else:
                isoType = 'integer(kind=C_INT)'
        else:
            if anArg.type.startswith('int'):
                isoType = 'integer(kind=C_INT)'
            elif anArg.type.startswith('void'):
                # If an argument is void, it can take any type since a pointer
                # will be passed to C.  Just call it an integer.
                isoType = 'integer(kind=C_INT)'
            elif anArg.type.startswith('char *') or \
                 anArg.type.startswith('char*'):
                isoType = 'character(kind=C_CHAR)'
            else:
                raise ValueError('no Fortran type assigned for ' + anArg.name +\
                                 ' with type ' + anArg.type)
        argDecl += isoType + ', '
        if anArg.is_input and anArg.is_output:
            intent = 'inout'
        elif anArg.is_output:
            intent = 'out'
        argDecl += 'intent(' + intent + ')'
        if not anArg.is_pointer:
            argDecl += ', value'
        argDecl += ' :: ' + anArg.call_name
        if anArg.type.startswith('char *') or anArg.type.startswith('char*'):
            argDecl += '(*)'
        if anArg.is_plural:
            argDecl += '(:)'

        return argDecl

    # Returns a string that can be used to declare an argument's type and
    # intent within a Fortran subroutine.
    def _getFortArgDecl(self, anArg):
        argDecl = ''
        intent = 'in'
        fType = 'integer'
        if anArg.is_mpi_type:
            if anArg.mpi_type == 'MPI_Address':
                fType = 'integer(YogiMPI_ADDRESS_KIND)'
            elif anArg.mpi_type == 'MPI_Offset':
                fType = 'integer(YogiMPI_OFFSET_KIND)'
            else:
                fType = 'integer(YogiMPI_INTEGER_KIND)' 
        else:
            if self._isFortranLogical(anArg):
                fType = 'logical'
            elif anArg.type.startswith('int'):
                fType = 'integer'
            elif anArg.type.startswith('void'):
                # If an argument is void, it can take any type since a pointer
                # will be passed to C.  Just call it an integer.
                fType = 'integer'
            elif anArg.type.startswith('char *') or \
                 anArg.type.startswith('char*'):
                fType = 'character(len=*)'
            else:    
                raise ValueError('no Fortran type assigned for ' + anArg.name +\
                                 ' with type ' + anArg.type)
        argDecl += fType + ', '
        if anArg.is_input and anArg.is_output:
            intent = 'inout'
        elif anArg.is_output:
            intent = 'out'
        argDecl += 'intent(' + intent + ') :: ' + anArg.call_name 
        if anArg.is_plural:
            argDecl += '(*)'
        return argDecl

    # Writes the header file that users will include in their program.
    def writeCUserHeader(self):
        user_header = source_writers.CSource(inputFile='mpitoyogi.h.in')
        func_defines = source_writers.CSource()
        for aFunc in self.functions:
            newName = 'Yogi' + aFunc.name
            func_defines.addLines('#define ' + aFunc.name + ' ' + newName)
        user_header.merge(func_defines, 'YOGI_DEFINES')
        user_header.writeFile('mpitoyogi.h')

    # Writes the actual header file for YogiMPI.
    def writeCHeader(self):
        c_header = source_writers.CSource(inputFile='yogimpi.h.in')
        func_protos = source_writers.CSource()
        for aFunc in self.functions:
            name = self.prefix + aFunc.name
            arg_string = aFunc.cArgString()
            func_protos.addPrototype(name, aFunc.return_type, arg_string) 
            func_protos.newLine()
        c_header.merge(func_protos, 'YOGI_PROTOTYPES') 
        c_header.writeFile('yogimpi.h')

    ## Writes a chunk of code to handle conversions and conditionals before
    #  calling the actual MPI function, for a single argument.
    def _createBeforeCXXCode(self, sourceFile, anArg, aFunc):
        funcException = "Function " + aFunc.name + ": "
        if not anArg.is_mpi_type:
            if not anArg.is_input:
                return 
            if anArg.convert_class is not None:
                self._makeConstantCall(sourceFile, anArg, before=True)
            else:
                self._makeConstantCheck(sourceFile, anArg, before=True)
        else:
            stripType = anArg.mpi_type.replace('MPI_', '')
            if anArg.type.startswith('MPI_Status'):
                if anArg.is_output and not anArg.is_input:
                    return 
                else:
                    # MPI_Status input arguments are always pointers. 
                    varDecl = anArg.mpi_type + ' *' + anArg.mpi_name
                    sourceFile.addLines(varDecl + ' = NULL;')
            # Create a blank MPI version of that type.
            elif not anArg.is_plural:
                varDecl = anArg.mpi_type + ' ' + anArg.mpi_name
                # Declare it now, to be put on the stack.
                sourceFile.addLines(varDecl + ';')
            else:
                # The conversion array must be allocated because the size will
                # not be known until runtime. Plus you don't want the stack
                # overflowing from vararg-sized arrays, so use the heap.
                varDecl = anArg.mpi_type + ' * ' + anArg.mpi_name
                sourceFile.addLines(varDecl + ' = NULL;')
            if anArg.is_input:
                if anArg.mpi_func_ptr:
                    funcType = anArg.type.strip('*')
                    if anArg.pre_convert_values:
                        self._makeConstantCheck(sourceFile, anArg, before=True)
                        sourceFile.addElse()
                    if funcType in GenerateWrap.mpiFunctionMap:
                        convFunc = GenerateWrap.manPrefix + 'convert' +\
                                   GenerateWrap.mpiFunctionMap[funcType]
                        sourceFile.addLines(anArg.mpi_name + ' = ' +\
                                            convFunc + '(' + anArg.call_name +
                                            ');')
                    else:
                        sourceFile.addLines(anArg.mpi_name + ' = (' +\
                                            anArg.type + ')' +\
                                            anArg.call_name + ';') 
                    if anArg.pre_convert_values:
                        sourceFile.endElse()

                elif anArg.pre_convert_values:
                    # Add conditional for conversion.
                    # Add else to create an item and convert the item normally.
                    self._makeConstantCheck(sourceFile, anArg, before=True)
                    sourceFile.addElse()
                    try:
                        self._makeMPIConversion(sourceFile, anArg, before=True)
                    except ValueError as v:
                        print funcException + str(v)
                    sourceFile.endElse()
                else:
                    # Convert the item normally.
                    try:
                        self._makeMPIConversion(sourceFile, anArg, before=True)
                    except ValueError as v:
                        print funcException + str(v)

    ## Writes a chunk of code to handle conversions and conditionals after 
    #  calling the actual MPI function, for a single argument.
    def _createAfterCXXCode(self, sourceFile, anArg, aFunc):
        funcException = "Function " + aFunc.name + ": "
        if not anArg.is_mpi_type:
            if not anArg.is_output:
                return
            if anArg.convert_class is not None:
                self._makeConstantCall(sourceFile, anArg, before=False)
            else:
                self._makeConstantCheck(sourceFile, anArg, before=False)
        else:
            if anArg.type.startswith('MPI_Status'):
                if not (anArg.is_input and anArg.is_output):
                    return
            if anArg.is_output or anArg.free_handle:
                if anArg.post_convert_values:
                    # Add conditional for conversion.
                    # Add else to create an item and convert the item normally.
                    self._makeConstantCheck(sourceFile, anArg, before=False)
                    sourceFile.addElse()
                    try:
                        self._makeMPIConversion(sourceFile, anArg, before=False)
                    except ValueError as v:
                        print funcException + str(v)
                    sourceFile.endElse()
                else:
                    # Convert the item normally.
                    try:
                        self._makeMPIConversion(sourceFile, anArg, before=False)
                    except ValueError as v:
                        print funcException + str(v)
            elif anArg.is_input and anArg.is_plural:
                stripType = anArg.mpi_type.replace('MPI_', '')
                freeFunc = GenerateWrap.manPrefix + 'free' + stripType
                sourceFile.addLines(freeFunc + '(' + anArg.mpi_name + ');')

    ## Writes the internal C++ source file for YogiMPI.
    def writeCXXSource(self):
        cxx_source = source_writers.CSource(inputFile='yogimpi.cxx.in')
        yogi_functions = source_writers.CSource()

        for aFunc in self.functions:
            aFunc.validate()
            for i, anArg in enumerate(aFunc.args):
                # Check if an MPI_Status argument is output. This means
                # that the user can optionally specify MPI_STATUS_IGNORE, which
                # means Yogi skips any conversions for it.
                if anArg.type.startswith('MPI_Status'):
                    if anArg.is_output and not anArg.is_input:
                        aFunc.status_ignore = True
                        aFunc.status_ignore_arg = i
                        if anArg.is_plural:
                            aFunc.status_ignore_type = 'MPI_STATUSES_IGNORE'
                        else:
                            aFunc.status_ignore_type = 'MPI_STATUS_IGNORE'

            name = self.prefix + aFunc.name                                
            arg_string = aFunc.cArgString()
            yogi_functions.addFunction(name, aFunc.return_type, arg_string)
            yogi_functions.addLines('int mpi_error;')
 
            # Write a code block marked as "first"
            firstCode = aFunc.getBlock('first')
            if firstCode is not None:
                for aLine in firstCode:
                    aLine = aLine.replace('{manPrefix}', GenerateWrap.manPrefix)
                    yogi_functions.addLines(aLine)

            for i, anArg in enumerate(aFunc.args):
                self._createBeforeCXXCode(yogi_functions, anArg, aFunc) 

            bcCode = aFunc.getBlock('beforecall')
            if bcCode is not None:
                for aLine in bcCode:
                    aLine = aLine.replace('{manPrefix}', GenerateWrap.manPrefix)
                    yogi_functions.addLines(aLine)

            withoutIgnore = self._mpiCallString(aFunc, False)
            if aFunc.status_ignore:
                # Optional STATUS_IGNORE must be recognized.
                withIgnore = self._mpiCallString(aFunc, True)
                ignoreArgNum = aFunc.status_ignore_arg
                ignoreType = aFunc.status_ignore_type
                ignoreArg = aFunc.args[ignoreArgNum]
                ignoreCallName = ignoreArg.name.strip('[]')
                yogi_functions.addIf(ignoreCallName + ' == ' + self.prefix +\
                                     ignoreType) 
                yogi_functions.addLines(withIgnore)
                yogi_functions.endIf()
                yogi_functions.addElse()
                self._statusOutputLines(yogi_functions, aFunc, 'input')
                yogi_functions.addLines(withoutIgnore)
                self._statusOutputLines(yogi_functions, aFunc, 'output')
                yogi_functions.endElse()
            else:
                yogi_functions.addLines(withoutIgnore)

            # Write a code block marked as "aftercall"
            afterCode = aFunc.getBlock('aftercall')
            if afterCode is not None:
                for aLine in afterCode:
                    aLine = aLine.replace('{manPrefix}', GenerateWrap.manPrefix)
                    yogi_functions.addLines(aLine)

            for i, anArg in enumerate(aFunc.args):
                self._createAfterCXXCode(yogi_functions, anArg, aFunc)
               
            # Write a code block marked as "beforereturn"
            lastCode = aFunc.getBlock('beforereturn')
            if lastCode is not None:
                for aLine in lastCode:
                    aLine = aLine.replace('{manPrefix}', GenerateWrap.manPrefix)
                    yogi_functions.addLines(aLine)

            errorConv = GenerateWrap.manPrefix + 'errorToYogi' 
            yogi_functions.addLines('return ' + errorConv + '(mpi_error);')
            yogi_functions.endFunction(name)
            yogi_functions.newLine()
        cxx_source.merge(yogi_functions, 'YOGI_FUNCTIONS')
        cxx_source.writeFile('yogimpi.cxx')

    def _isFortranLogical(self, anArg):
        if anArg.name == 'flag':
            if anArg.type.startswith('int'):
                if not anArg.is_plural:
                    return True
        return False


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
