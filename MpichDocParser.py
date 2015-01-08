import xml.etree.ElementTree as ET
import sys
import os
import re

class MpichDocParser:
   """Generates an xml representation of the mpich api from the html mpich reference.
      The necessary html can be found in an mpich source directory at: mpich-x.y.z/www/www3

      The resulting xml looks like:
      <MpichAPI>
         <Function name="MPI_Function">
            <Arg type="int" name="argOne"/>
            <Arg type="char*" name="argTwo"/>
            <Arg type="float" name="argThree"/>
            <ReturnType>int</Returns>
         </Function>
      </MpichAPI>

      TODO: flag each arg as output="true" and/or input="true" based on 'Output Parameters'/'Input Parameters"""

   def __init__(self, docRoot, outputFilename):
      #init the ElementTree stuff
      self.xmlRoot = ET.Element('MpichAPI')
      tree = ET.ElementTree(self.xmlRoot)

      #parse all the files in docRoot
      for (dirpath, dirnames, filenames) in os.walk(docRoot):
         for f in filenames:
            self.parseHTML(os.path.join(docRoot,f))
         break #because we don't care about subdirectories

      #indent & write the XML
      self.indentXML(self.xmlRoot)
      tree.write(outputFilename)

   def parseHTML(self, filename):
      """Parse an mpich html documentation file and add the relevant bits as a child element of self.xmlRoot"""
      #print filename
      if not (filename.endswith(".html") or filename.endswith(".htm")):
         print "Skipping non-html file:", filename
         return

      docFileContents = open(filename).read()

      #Here comes the squishy regexy bit.
      #First, scrape the first <PRE> block from the html. We assume this is the function synopsis.
      #FIXME: maybe also look for 'Synopsis' before the <PRE> tag
      searchResult = re.search('<PRE>(.+?)</PRE>',docFileContents,re.MULTILINE | re.DOTALL)
      if searchResult is None or len(searchResult.groups()) < 1:
         print("Error parsing html: couldn't isolate function description")
         return
      apiString = searchResult.group(1)

      #remove any lines that start with '#' from the search result
      #this is necessary because some synopses annoyingly contain C preprocessor directives
      apiString = re.sub('#.+\n','',apiString)

      #remove newlines from the search result
      apiString = apiString.replace('\n','')

      #remove 'const' because we don't care about const correctness and it greatly simplifies parsing
      apiString = apiString.replace('const','')

      #For consistency, transform 'type * varName' into 'type *varName'
      apiString = apiString.replace(' * ',' *')

      #split the function description into a list of alphanumeric (plus *, [, and ]) substrings
      apiSubstrings = re.split('[^a-zA-Z0-9_\*\[\]]+',apiString)

      #remove any empty strings
      while '' in apiSubstrings: apiSubstrings.remove('')

      #add a new xml element for the function
      returnType = apiSubstrings[0]
      functionName = apiSubstrings[1]

      functionElement = ET.SubElement(self.xmlRoot, 'Function')
      functionElement.set('name',functionName)
      returnTypeElement = ET.SubElement(functionElement,'ReturnType')
      returnTypeElement.text = returnType

      #add xml sub-elements for each argument to the function
      i = 2
      while i + 1 < len(apiSubstrings):
         argType = apiSubstrings[i]
         argName = apiSubstrings[i+1]

         #if the argument name starts with a *, it's a pointer type, so move the * appropriately
         while argName.startswith('*'):
            argName = argName[1:]
            argType = argType + '*'

         argElement = ET.SubElement(functionElement,'Arg')
         argElement.set('type',argType)
         argElement.set('name',argName)
         i += 2

   def indentXML(self, elem, level=0):
      """Add some indentation to an ElementTree Element and its children for readability"""
      i = "\n" + level*"  "
      if len(elem):
         if not elem.text or not elem.text.strip():
            elem.text = i + "  "
         if not elem.tail or not elem.tail.strip():
            elem.tail = i
         for elem in elem:
            self.indentXML(elem, level+1)
         if not elem.tail or not elem.tail.strip():
            elem.tail = i
      else:
         if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i

if __name__ == '__main__':
   if len(sys.argv) < 3:
      print("Usage: python MpichDocParser.py /path/to/mpich-x.y.z/www/www3 output.xml")
   else:
      MpichDocParser(sys.argv[1],sys.argv[2])

