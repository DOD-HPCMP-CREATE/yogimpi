import xml.etree.ElementTree as ET
import sys

class MPIArgument(object):
    def __init__(self):
        self.name = None
        self.callName = None
        self.type = None
        self.isPointer = False
        self.isPlural = False
        self.isOutput = False
        self.isInput = False
        self.convertValues = []
        self.isMPIType = False
        self.hasTempOutputVar = False
        self.tempOutputIsPtr = False
        self.tempOutputType = None
        self.tempOutputConverter = None
        self.freeObject = False

class MPIFunction(object):
    def __init__(self):
        self.name = None
        self.statusIgnore = False
        self.statusIgnoreType = None
        self.statusIgnoreArg = 0
        self.returnType = 'int'
        self.args = []
        self.preprocess = {}
        self.postprocess = {}
    
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
                for convs in argElement.iterfind('Convert'):
                    thisArg.convertValues.append(convs.text)
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
            printName = anArg.callName
            if anArg.hasTempOutputVar:
                if anArg.isPointer:
                    printName = '&' + printName
            if doIgnore:
                if aFunc.statusIgnore:
                    if i == aFunc.statusIgnoreArg:
                        printName = aFunc.statusIgnoreType
                callRealMPI += printName
            else:
                callRealMPI += printName            
        callRealMPI += ');'
        return callRealMPI
    
    def _typeConversions(self, aFunc):
        preFunc = {'MPI_Comm':'comm_to_mpi',
                   'MPI_Info':'info_to_mpi',
                   'MPI_File':'file_to_mpi',
                   'MPI_Datatype':'datatype_to_mpi',
                   'MPI_Request':'request_to_mpi',
                   'MPI_Group':'group_to_mpi'}
        postFunc = {'MPI_Comm':'add_new_mpi',
                    'MPI_Info':'add_new_info',
                    'MPI_File':'add_new_file',
                    'MPI_Datatype':'add_new_datatype',
                    'MPI_Request':'add_new_request',
                    'MPI_Group':'add_new_group'}
        for i in range(len(aFunc.args)):
            anArg = aFunc.args[i]
            if anArg.isInput:
                if anArg.isMPIType:
                    for key in preFunc.keys():
                        if anArg.type.startswith(key):
                            aFunc.preprocess[i] = {}
                            anArg.callName = 'in_' + anArg.name
                            aFunc.preprocess[i]['newtype'] = key
                            aFunc.preprocess[i]['converter'] = preFunc[key]
            if anArg.isOutput:
                if anArg.isMPIType:
                    for key in preFunc.keys():
                        if anArg.type.startswith(key):
                            anArg.callName = 'in_' + anArg.name
                            anArg.hasTempOutputVar = True
                            anArg.tempOutputIsPtr = False
                            anArg.tempOutputType = key
                    for key in postFunc.keys():
                        if anArg.type.startswith(key):
                            anArg.tempOutputConverter = postFunc[key]
                                
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
                            
    def _convLines(self, aFunc, phase):
        convLine = ''
        if phase == 'input':
            for i in range(len(aFunc.args)):
                anArg = aFunc.args[i]
                if anArg.hasTempOutputVar:
                    convLine += '    ' + anArg.tempOutputType + ' ' +\
                               anArg.callName + ';\n'
                elif i in aFunc.preprocess:
                    argMap = aFunc.preprocess[i]
                    if argMap['converter'] is not None:
                        convLine += '    ' + argMap['newtype'] + ' ' +\
                                   anArg.callName + ' = ' +\
                                   argMap['converter'] +\
                                   '(' + anArg.name + ');\n'
                                   
        elif phase == 'output':
            for i in range(len(aFunc.args)):
                anArg = aFunc.args[i]
                if anArg.hasTempOutputVar:
                    convLine += '    '
                    if anArg.isPointer:
                        convLine += '*'
                    convLine += anArg.name + ' = ' +\
                                anArg.tempOutputConverter + '(' +\
                                anArg.callName + ');\n'           
                
        return convLine
            
    def writeFiles(self):
        headerLines = []
        sourceLines = []
        headerFile = open(self.filePrefix + '.h', 'w')
        sourceFile = open(self.filePrefix + '.c', 'w')
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
        headerFile.writelines(headerLines)
        headerFile.close()

        for aFunc in self.functions:
            self._typeConversions(aFunc)
            # There are a few things that must be checked.
            # - If there is an output MPI_Status (or MPI_Status plural), the 
            #   check for MPI_STATUSES_IGNORE or MPI_STATUS_IGNORE must offer
            #   a branch call (done).
            # - Check for any manual comparison/converts on arguments.
            # - Convert scalar input arguments to proper format (done).
            # - Convert array input arguments to proper format.
            # - Convert scalar output arguments to proper format.
            # - Convert array output arguments to proper format.
            # - Free objects that are being freed.
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
            sourceLines.append(self._convLines(aFunc, 'input'))

            for anArg in aFunc.args:
                if anArg.convertValues and anArg.isInput:
                    for val in anArg.convertValues:
                        refAssign = anArg.name
                        convLine = '    if ('
                        convLine += refAssign + ' == ' + self.prefix + val +\
                                    ') {\n' +\
                                    '        ' + refAssign + ' = ' + val +\
                                    ';\n    }\n'
                        sourceLines.append(convLine)
            
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
            sourceLines.append(self._convLines(aFunc, 'output'))
            sourceLines.append('    return error_to_yogi(mpi_error);\n')
            sourceLines.append('}\n\n')        
        sourceFile.writelines(sourceLines)
        sourceFile.close()
            
if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: python GenerateWrap.py input.xml prefix")
    else:
        GenerateWrap(sys.argv[1], sys.argv[2])

