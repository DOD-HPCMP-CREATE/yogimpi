import xml.etree.ElementTree as ET
import sys

class GenerateWrap:
   """Generates a YogiMPI wrapper (.c and .h file) based on an input MpichAPI file (from MpichDocParser.py)."""

   def __init__(self, xmlInput, hFilename, cFilename):
      tree = ET.parse(xmlInput)
      xmlRoot = tree.getroot()
      prefix = 'Yogi'

      hFile = open(hFilename,'w')
      cFile = open(cFilename,'w')

      #each child of xmlRoot should be a <Function> element in the xml
      for child in xmlRoot:
         if child.tag != 'Function':
            continue
         functionName = child.attrib['name']

         typedArgList = '' #takes the form 'int* arg1Name, float arg2Name'
         argList = ''      #takes the form 'arg1Name, arg2Name'

         nativeCallArgList = '' #same as the argList, but with some arg names replaced
                                #with their converted version.
                                #for example, if we want to pass a 'YogiMPI_Comm comm'
                                #to the native MPI routine, first we need to convert it
                                #to a native MPI_comm by doing something like:
                                #     MPI_Comm mpi_comm = comm_to_mpi(comm);
                                #so the nativeCallList would list mpi_comm instead of comm

         setupLines = '' #lines of code that precede the native MPI call

         #each child of the current <Function> element is either an <Arg> or a <ReturnType>
         for grandchild in child:
            if grandchild.tag == 'ReturnType':
               returnType = grandchild.text
            if grandchild.tag == 'Arg':
               #update all the various forms of the argument list
               if typedArgList:
                  typedArgList += ', '
                  argList += ', '
                  nativeCallArgList += ', '
               argName = grandchild.attrib['name']
               argType = grandchild.attrib['type']
               argInput = True if 'input' in grandchild.attrib and grandchild.attrib['input'] == "true" else False
               typedArgList += argType + ' ' + argName
               argList += argName 

               #convert MPI_Datatypes to native format before native MPI call
               if argType == 'MPI_Datatype' and argInput:
                  setupLines += '   MPI_Datatype mpi_'+argName+' = datatype_to_mpi('+argName+');\n'
                  nativeCallArgList += 'mpi_'+argName 

               #convert MPI_Comms to native format before native MPI call
               elif argType == 'MPI_Comm' and argInput:
                  setupLines += '   MPI_Comm mpi_'+argName+' = comm_to_mpi('+argName+');\n'
                  nativeCallArgList += 'mpi_'+argName 

               #FIXME: support for group_to_mpi, comm_to_mpi, request_to_mpi, op_to_mpi, file_to_mpi, info_to_mpi,
               #       and comparison_to_yogi. Also needs to support output parameters, and references/pointers are
               #       probably not handled correctly everywhere yet. In short, this section needs a lot of work.
               else:
                  nativeCallArgList += argName 

                  
         #wrappedFunc is the function definition with Yogi prefixes
         wrappedFunc = returnType + ' ' + functionName + '(' + typedArgList + ')'
         wrappedFunc = wrappedFunc.replace('MPI',prefix+'MPI')
         #whereas nativeCall is the native MPI call with converted arg names
         nativeCall = functionName + '(' + nativeCallArgList + ')'

         if setupLines:
            setupLines += '\n'

         #write header line
         hFile.write(wrappedFunc+';\n\n')

         #write implementation
         cFile.write(wrappedFunc+'\n{\n')

         cFile.write(setupLines)
         cFile.write('   int mpi_error = '+nativeCall+';\n\n')
         cFile.write('   return error_to_yogi(mpi_error);\n')
         cFile.write('}\n\n')

      hFile.close()
      cFile.close()

if __name__ == '__main__':
   if len(sys.argv) < 4:
      print("Usage: python GenerateWrap.py input.xml output.h output.c")
   else:
      GenerateWrap(sys.argv[1], sys.argv[2], sys.argv[3])

