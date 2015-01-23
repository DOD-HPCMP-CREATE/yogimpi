import xml.etree.ElementTree as ET
import sys
from __builtin__ import True
from Carbon.Aliases import true
from matplotlib.mathtext import TruetypeFonts

class MPIArgument(object):
    def __init__(self):
        self.name = None
        self.type = None
        self.isPointer = False
        self.isPlural = False
        self.isOutput = False
        self.isInput = False
        self.isMPIType = False

class MPIFunction(object):
    def __init__(self):
        self.name = None
        self.statusIgnore = False
        self.statusIgnoreType = None
        self.statusIgnoreArg = 0
        self.returnType = 'int'
        self.args = []
    
class GenerateWrap(object):

    def __init__(self, xmlInput, filePrefix):
        self.filePrefix = filePrefix
        self.prefix = 'Yogi'
        self.functions = []
        self.parseInput(xmlInput)
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
                if thisArg.name.strip().endswith('[]'):
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
                print
            print
    
    def _mpiCallString(self, aFunc, doIgnore, inListIndex, preConversions):
        callRealMPI = 'mpi_error = '
        callRealMPI += aFunc.name + '('
        for i in range(len(aFunc.args)):
            anArg = aFunc.args[i]                
            argName = anArg.name
            if i in inListIndex:
                argName = 'in_' + anArg.name
            if doIgnore:
                if aFunc.statusIgnore:
                    if i == aFunc.statusIgnoreArg:
                        argName = aFunc.statusIgnoreType
            if i > 0:
                callRealMPI += ", "
            callRealMPI += argName            
        callRealMPI += ');'
        return callRealMPI
            
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
        dataMappings = {'MPI_Comm':'comm_to_mpi',
                        'MPI_Info':'info_to_mpi',
                        'MPI_File':'file_to_mpi',
                        'MPI_Datatype':'datatype_to_mpi',
                        'MPI_Status*':'yogi_status_to_mpi'}
        for aFunc in self.functions:
            inListIndex = []
            preConversions = {}
            postConversions = {}
            newMPIObjects = {}
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
                if anArg.isMPIType:
                    for key in dataMappings.keys():
                        if anArg.type.startswith(key):
                            if anArg.isInput:
                                preConversions[anArg.name] = {}
                                argMap = preConversions[anArg.name]
                                inListIndex.append(i)
                                argMap['newtype'] = key
                                argMap['converter'] = dataMappings[key]                    
                    if anArg.type.startswith('MPI_Status'):
                        if anArg.isOutput:
                            aFunc.statusIgnore = True
                            aFunc.statusIgnoreArg = i
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
            # Now handle pre-conversions.
            for convert in preConversions.keys():
                argMap = preConversions[convert]
                convLine = '    ' + argMap['newtype'] + ' in_' + convert +\
                           ' = ' + argMap['converter'] + '(' + convert + ');\n'
                callLine += convLine
            
            withoutIgnore = self._mpiCallString(aFunc, False, inListIndex,
                                                preConversions)
            if aFunc.statusIgnore:
                # Optional STATUS_IGNORE must be recognized.
                withIgnore = self._mpiCallString(aFunc, True, inListIndex,
                                                 preConversions)
                callLine += '    if (' + aFunc.args[aFunc.statusIgnoreArg].name +\
                            ' == ' + self.prefix + aFunc.statusIgnoreType +\
                            ') {\n' +\
                            '        ' + withIgnore +\
                            '\n    }\n' +\
                            '    else {\n' +\
                            '        ' + withoutIgnore +\
                            '\n    }\n'
            else:
                callLine += '    ' + withoutIgnore + '\n'
                
            sourceLines.append(callLine)
            sourceLines.append('    return error_to_yogi(mpi_error);\n')
            sourceLines.append('}\n\n')        
        sourceFile.writelines(sourceLines)
        sourceFile.close()
            
if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: python GenerateWrap.py input.xml prefix")
    else:
        GenerateWrap(sys.argv[1], sys.argv[2])

