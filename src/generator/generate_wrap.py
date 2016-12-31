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

            for argElement in funcElement.findall('Arg'):
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
                shouldFree = argElement.attrib.get('free', None)
                thisArg.free_handle = self._checkTrue(shouldFree)
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
                    isFunc = conv.attrib.get('function', None)
                    val.is_function = self._checkTrue(isFunc)
                    iterate = conv.attrib.get('iterate', None)
                    val.iteration = self._checkTrue(iterate)
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

        for i, anArg in enumerate(aFunc.args):
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

    ## Creates the lines to convert MPI handles and typedefs in both the
    #  input (passing from Yogi to MPI) and output (getting from MPI to Yogi) 
    #  phases. 
    def _mpiConvLines(self, sourceFile, aFunc, phase):
        if phase == 'input':
            for i, anArg in enumerate(aFunc.args):
                # This is only for MPI handles and typedefs, nothing else.
                if not anArg.is_mpi_type:
                    continue
                # MPI_Status is a special case and is handled elsewhere.
                if anArg.type.startswith('MPI_Status'):
                    continue
                if anArg.temp_pre_converter:
                    convLine = anArg.temp_type + ' ' + anArg.temp_name
                    fullCall = GenerateWrap.manPrefix + anArg.temp_pre_converter
                    convName = anArg.call_name
                    if not anArg.is_plural:
                        if anArg.is_pointer:
                            convName = "*" + convName
                    convLine += ' = ' + fullCall + '(' + convName
                    if anArg.is_plural:
                        convLine += ', '
                        dimArg = aFunc.getArg(anArg.dims)
                        if not dimArg:
                            print "Warning: function " + aFunc.name + ", " +\
                                  "argument " + anArg.name + " needed dims, " +\
                                  "didn't have 'em."
                        else:
                            if dimArg.is_pointer:
                                convLine += '*'
                            convLine += anArg.dims
                    convLine += ');'
                    sourceFile.addLines(convLine)                   
        elif phase == 'output':
            for i, anArg in enumerate(aFunc.args):
                # This is only for MPI handles and typedefs, nothing else.
                if not anArg.is_mpi_type:
                    continue
                # MPI_Status is a special case and is handled elsewhere.
                if anArg.type.startswith('MPI_Status'):
                    continue
                if anArg.temp_post_converter:
                    convLine = ''
                    if not anArg.temp_is_ptr:
                        if anArg.is_pointer:
                            convLine += '*'
                    convLine += anArg.name + ' = ' +\
                                anArg.temp_post_converter + '('
                    if anArg.temp_is_ptr:
                        if not anArg.is_pointer:
                            convLine += '*'
                    convLine += anArg.temp_name + ');'
                    sourceFile.addLines(convLine) 
    
    def _freeHandleLines(self, sourceFile, aFunc):
        for i, anArg in enumerate(aFunc.args):
            if anArg.type.startswith('MPI_Status'):
                continue
            if anArg.free_handle:
                if not anArg.is_mpi_type:
                    errMsg = "Cannot free an argument that isn't an MPI handle."
                    raise ValueError(errMsg)
                if not anArg.is_pointer:
                    errMsg = "Freeing a non-pointer won't save the changes."
                    raise ValueError(errMsg)
                freeFunc = GenerateWrap.manPrefix + 'FreeYogi' + anArg.temp_type
                sourceFile.addLines(freeFunc + '(' + anArg.name + ');') 

    def _constCond(self, sourceFile, anArg, values):
        convLines = '' 
        for i, convVal in enumerate(values):
            compareValue = self.prefix + convVal.name
            changeValue = convVal.name
            refAssign = anArg.name
            if anArg.is_pointer:
                if not convVal.is_pointer:
                    refAssign = '*' + refAssign
                elif not anArg.is_pointer:
                    if convVal.is_pointer:
                        errMsg = "Assigning a pointer to a non-pointer."
                        raise ValueError(errMsg)
            if convVal.is_function:
                sourceFile.addLines(anArg.call_name + ' = ' + changeValue +\
                                    '(' + anArg.call_name + ');')
            else:
                sourceFile.addIf(refAssign + ' == ' + compareValue)
                sourceFile.addLines(refAssign + ' = ' + changeValue + ';')
                sourceFile.endIf()

    def _constantConversions(self, sourceFile, aFunc, phase):
        convLines = '' 
        for anArg in aFunc.args:
            if phase == 'input':
                if not anArg.is_input:
                    continue
                if not anArg.pre_convert_values:
                    continue
                try:
                    self._constCond(sourceFile, anArg,
                                    anArg.pre_convert_values)
                except ValueError as v:
                    errMsg = "Error with constant conversion in " +\
                             aFunc.name + ", arg " + anArg.name + ": " + str(v)
                    raise ValueError(errMsg)

            elif phase == 'output':
                if not anArg.is_output:
                    continue
                if not anArg.post_convert_values:
                    continue
                try:
                    self._constCond(sourceFile, anArg,
                                    anArg.post_convert_values)
                except ValueError as v:
                    errMsg = "Error with constant conversion in " +\
                             aFunc.name + ", arg " + anArg.name + ": " + str(v)
                    raise ValueError(errMsg)

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

    ## Writes the internal C++ source file for YogiMPI.
    def writeCXXSource(self):
        cxx_source = source_writers.CSource(inputFile='yogimpi.cxx.in')
        yogi_functions = source_writers.CSource()

        for aFunc in self.functions:
            self._mpiConversions(aFunc)
            aFunc.validate()
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
            self._mpiConvLines(yogi_functions, aFunc, 'input')
            self._constantConversions(yogi_functions, aFunc, 'input')

            withoutIgnore = self._mpiCallString(aFunc, False)
            if aFunc.status_ignore:
                # Optional STATUS_IGNORE must be recognized.
                withIgnore = self._mpiCallString(aFunc, True)
                ignoreArg = aFunc.status_ignore_arg
                ignoreType = aFunc.status_ignore_type
                yogi_functions.addIf(aFunc.args[ignoreArg].name + ' == ' +\
                                     self.prefix + ignoreType) 
                yogi_functions.addLines(withIgnore)
                yogi_functions.addElse()
                yogi_functions.addLines(self._statusConvLine(aFunc, 'input'),
                                        withoutIgnore,
                                        self._statusConvLine(aFunc, 'output'))
                yogi_functions.endElse()
                yogi_functions.endIf(cleanFormatting=False)
            else:
                yogi_functions.addLines(withoutIgnore)
                
            self._constantConversions(yogi_functions, aFunc, 'output')
            self._mpiConvLines(yogi_functions, aFunc, 'output')
            self._freeHandleLines(yogi_functions, aFunc)
            yogi_functions.addLines('return error_to_yogi(mpi_error);')
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
