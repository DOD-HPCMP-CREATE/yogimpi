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

    mpiTypeDefs = [ 'MPI_Offset', 'MPI_Aint' ]

    mpiTypes = mpiHandles + mpiTypeDefs

    manPrefix = "YogiManager::getInstance()::"

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

                if rawType.startswith('MPI_') and \
                   not rawType.endswith('function*'):
                    thisArg.is_mpi_type = True
                    thisArg.mpi_name = 'conv_' + thisArg.call_name
                    for mpiType in GenerateWrap.mpiTypes:
                        if thisArg.type.startswith(mpiType):
                            thisArg.mpi_type = mpiType
                    if thisArg.is_plural:
                        # A pointer will be allocated with new on the heap.
                        thisArg.mpi_is_ptr = True
                    else:
                        # The object will just be created on the stack, and
                        # the & symbol will be used to reference the address.
                        thisArg.mpi_is_ptr = False

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
            if anArg.is_mpi_type and not anArg.type.startswith('MPI_Status'):
                printName = anArg.mpi_name
                if anArg.is_pointer:
                    if not anArg.mpi_is_ptr:
                        printName = '&' + printName
                else:
                    if anArg.mpi_is_ptr:
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
   
    def _statusConvLine(self, aFunc, phase):
        convLine = ''
        if phase == 'input':
            if aFunc.status_ignore:
                theArg = aFunc.args[aFunc.status_ignore_arg]
                convLine += 'MPI_Status * ' + theArg.call_name +\
                        ' = yogi_status_to_mpi(' + theArg.name + ');'
        elif phase == 'output':
            if aFunc.status_ignore:
                theArg = aFunc.args[aFunc.status_ignore_arg]
                convLine += 'mpi_status_to_yogi(' + theArg.call_name +\
                            ', ' + theArg.name + ');'

        return convLine

    def _makeConstantCall(self, sourceFile, anArg, before=True):
        if anArg.is_mpi_type:
            errMsg = "Constant functions unsupported for MPI arguments."
            raise ValueError(errMsg)

        if before:
            classFunc = GenerateWrap.manPrefix + "toMPI"
        else:
            classFunc = GenerateWrap.manPrefix + "toYogi"

        sourceFile.addLines(anArg.call_name + ' = ' + classFunc +\
                                    '(' + anArg.call_name + ');')

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
            if before:
                compareValue = self.prefix + convVal.name
                changeValue = convVal.name
            else:
                compareValue = convVal.name
                changeValue = self.prefix + convVal.name
            if anArg.is_pointer:
                if anArg.is_mpi_type:
                    if not convVal.is_pointer:
                        assignVar = '*' + assignVar
                        if anArg.mpi_is_ptr:
                            compareVar = '*' + assignVar
                elif not convVal.is_pointer:
                    assignVar = '*' + assignVar
                    compareVar = '*' + compareVar
            elif convVal.is_pointer:
                errMsg = "Assigning a pointer to a non-pointer."
                raise ValueError(errMsg)

            compareStr = compareVar + ' == ' + compareValue
            if i == 0:
                sourceFile.addIf(compareStr)
            else:
                sourceFile.addElseIf(compareStr)
            sourceFile.addLines(assignVar + ' = ' + changeValue + ';')
            if i == 0:
                sourceFile.endIf()
            else:
                sourceFile.endElseIf()

    ## Appends the generated source code lines to convert an MPI item back and
    #  forth between Yogi and MPI types. 
    def _makeMPIConversion(self, sourceFile, anArg, before=True):
        stripType = anArg.mpi_type.replace('MPI_', '')
        if before:
            convFunc = GenerateWrap.manPrefix + "toMPI"
            firstArg = anArg.mpi_name
            secondArg = anArg.call_name
        else:
            convFunc = GenerateWrap.manPrefix + "toYogi"
            firstArg = anArg.call_name
            secondArg = anArg.mpi_name

        if not before and anArg.free_handle:
            # If this is the "after" phase and this should be freed,
            # don't do a conversion, just empty the handle.
            freeHandle = GenerateWrap.manPrefix + 'unmap' + stripType
            fhCall = freeHandle + '(' + anArg.call_name + ');'
            sourceFile.addLines(fhCall)
        elif anArg.is_plural and anArg.dims:
            sourceFile.addLines(convFunc + '(' + firstArg + ', ' + secondArg +\
                                ', ' + anArg.dims + ');')
        else:
            sourceFile.addLines(convFunc + '(' + firstArg + ', ' + secondArg +\
                                ');')

    def writeFiles(self):        
        self.writeCHeader()
        self.writeCUserHeader()
        self.writeCXXSource()

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
    def _createBeforeCXXCode(self, sourceFile, anArg):
        if not anArg.is_mpi_type:
            if not anArg.is_input:
                return 
            if anArg.convert_class is not None:
                self._makeConstantCall(sourceFile, anArg, before=True)
            else:
                self._makeConstantCheck(sourceFile, anArg, before=True)
        else:
            if anArg.type.startswith('MPI_Status'):
                return 
            stripType = anArg.mpi_type.replace('MPI_', '')
            # Create a blank MPI version of that type.
            varDecl = anArg.mpi_type + ' ' + anArg.mpi_name
            if not anArg.is_plural:
                # Declare it now, to be put on the stack.
                sourceFile.addLines(varDecl + ';')
            else:
                # The conversion array must be allocated because the size will
                # not be known until runtime.
                sourceFile.addLines(varDecl + '* = NULL;')
                createArrayFunc = GenerateWrap.manPrefix + 'create' +\
                                  stripType + 'Array'
                if not anArg.dims:
                    print "Argument " + anArg.name + " needed dims, " +\
                          "didn't have 'em."
                else:
                    createCall = createArrayFunc + '(' + anArg.mpi_name +\
                                 ', ' + anArg.dims + ');'
                    sourceFile.addLines(createCall)
            if anArg.is_input:
                if anArg.pre_convert_values:
                    # Add conditional for conversion.
                    # Add else to create an item and convert the item normally.
                    self._makeConstantCheck(sourceFile, anArg, before=True)
                    sourceFile.addElse()
                    self._makeMPIConversion(sourceFile, anArg, before=True)
                    sourceFile.endElse()
                else:
                    # Convert the item normally.
                    self._makeMPIConversion(sourceFile, anArg, before=True)

    ## Writes a chunk of code to handle conversions and conditionals after 
    #  calling the actual MPI function, for a single argument.
    def _createAfterCXXCode(self, sourceFile, anArg):
        if not anArg.is_mpi_type:
            if not anArg.is_output:
                return
            if anArg.convert_class is not None:
                self._makeConstantCall(sourceFile, anArg, before=False)
            else:
                self._makeConstantCheck(sourceFile, anArg, before=False)
        else:
            if anArg.type.startswith('MPI_Status'):
                return
            if anArg.is_output or anArg.free_handle:
                if anArg.post_convert_values:
                    # Add conditional for conversion.
                    # Add else to create an item and convert the item normally.
                    self._makeConstantCheck(sourceFile, anArg, before=False)
                    sourceFile.addElse()
                    self._makeMPIConversion(sourceFile, anArg, before=False)
                    sourceFile.endElse()
                else:
                    # Convert the item normally.
                    self._makeMPIConversion(sourceFile, anArg, before=False)

            # Free any temporary arrays created. 
            stripType = anArg.mpi_type.replace('MPI_', '')

            if anArg.is_plural:
                freeArrayFunc = GenerateWrap.manPrefix + 'free' + stripType +\
                                'Array'
                freeCall = freeArrayFunc + '(' + anArg.mpi_name + ');'
                sourceFile.addLines(freeCall)

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
                    if anArg.is_output:
                        aFunc.status_ignore = True
                        aFunc.status_ignore_arg = i
                        aFunc.args[i].call_name = 'in_' + aFunc.args[i].name
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
                    yogi_functions.addLines(aLine)

            for i, anArg in enumerate(aFunc.args):
                self._createBeforeCXXCode(yogi_functions, anArg) 

            withoutIgnore = self._mpiCallString(aFunc, False)
            if aFunc.status_ignore:
                # Optional STATUS_IGNORE must be recognized.
                withIgnore = self._mpiCallString(aFunc, True)
                ignoreArg = aFunc.status_ignore_arg
                ignoreType = aFunc.status_ignore_type
                yogi_functions.addIf(aFunc.args[ignoreArg].name + ' == ' +\
                                     self.prefix + ignoreType) 
                yogi_functions.addLines(withIgnore)
                yogi_functions.endIf()
                yogi_functions.addElse()
                yogi_functions.addLines(self._statusConvLine(aFunc, 'input'),
                                        withoutIgnore,
                                        self._statusConvLine(aFunc, 'output'))
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
                self._createAfterCXXCode(yogi_functions, anArg)
               
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
