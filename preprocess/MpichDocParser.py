import xml.etree.ElementTree as ET
import sys
import os
import re

class MpichDocParser:
   """Generates an xml representation of the mpich api from the html mpich reference.
      The necessary html can be found in an mpich source directory at: <mpich-x.y.z/www/www3>.

      The resulting xml looks like:
      <MpichAPI>
         <Function name="MPI_Function">
            <Arg type="int" name="argOne" input="true" />
            <Arg type="char*" name="argTwo" input="true" />
            <Arg type="float" name="argThree" output="true" />
            <ReturnType>int</Returns>
         </Function>
      </MpichAPI>"""


   def __init__(self, docRoot, outputFilename):
      #init the ElementTree
      xmlRoot = ET.Element('MpichAPI')

      #parse all the files in docRoot
      for (dirpath, dirnames, filenames) in os.walk(docRoot):
         for f in filenames:
            self.htmlToXml(os.path.join(docRoot,f), xmlRoot)
         break #because we don't care about subdirectories

      #indent & write the XML
      self.indentXML(xmlRoot)
      tree = ET.ElementTree(xmlRoot)
      tree.write(outputFilename)

   def htmlToXml(self, filename, xmlRoot):
      """Parse some mpich html documentation from filename and add the relevant bits to xmlRoot."""

      #read the file
      if not (filename.endswith(".html") or filename.endswith(".htm")):
         print "Skipping non-html file:", filename
         return
      docFileContents = open(filename).read()

      #here comes the squishy regexy bit
      #first, scrape the function synopsis block from the html
      searchResult = re.search('<H2>Synopsis</H2>\n<PRE>(.+?)</PRE>',docFileContents,re.MULTILINE | re.DOTALL)
      if searchResult is None or len(searchResult.groups()) < 1:
         print("Error parsing html: couldn't isolate function synopsis")
         return
      synopsis = searchResult.group(1)

      #remove any lines that start with '#' from the synopsis
      #this is necessary because some synopses annoyingly contain C preprocessor directives
      synopsis = re.sub('#.+\n','',synopsis)

      #remove newlines from the synopsis
      synopsis = synopsis.replace('\n','')

      #remove 'const' because we don't care about const correctness and it greatly simplifies parsing
      synopsis = synopsis.replace('const','')

      #For consistency, transform 'type * varName' into 'type *varName'
      synopsis = synopsis.replace(' * ',' *')

      #split the synopsis into a list of alphanumeric (plus *, [, and ]) substrings
      splitSynopsis = re.split('[^a-zA-Z0-9_\*\[\]]+',synopsis)

      #remove any empty strings
      while '' in splitSynopsis:
         splitSynopsis.remove('')

      #add a new xml element for the function
      returnType = splitSynopsis[0]
      functionName = splitSynopsis[1]

      functionElement = ET.SubElement(xmlRoot, 'Function')
      functionElement.set('name',functionName)
      returnTypeElement = ET.SubElement(functionElement,'ReturnType')
      returnTypeElement.text = returnType

      inputArgs, outputArgs = self.findIOArgs(docFileContents)

      #add xml sub-elements for each argument to the function
      i = 2
      while i + 1 < len(splitSynopsis):
         argType = splitSynopsis[i]
         argName = splitSynopsis[i+1]

         #if the argument name starts with a *, it's a pointer type, so move the * to the type
         while argName.startswith('*'):
            argName = argName[1:]
            argType = argType + '*'

         argElement = ET.SubElement(functionElement,'Arg')
         argElement.set('type',argType)
         argElement.set('name',argName)

         #this comparison doesn't work on args with [], so we also compare against a truncated name
         shortArgName = argName[:argName.find('[')] if argName.find('[') > 0 else argName
         if shortArgName in inputArgs:
            argElement.set('input','true')
         if shortArgName in outputArgs:
            argElement.set('output','true')

         i += 2

   def findIOArgs(self, html):
      """Given an html string, return a list of arguments flagged as inputs, and a list of
         arguments flagged as outputs."""

      inputs = []
      outputs = []

      #another squishy regexy bit incoming...
      #first, find the Input Parameters block of the html
      searchResult = re.search('<H2>Input Parameters</H2>\n<DL>(.+?)</DL>', html, re.MULTILINE | re.DOTALL)
      if searchResult and len(searchResult.groups()) >= 1:
         inputArgs = searchResult.group(1)
         #the bolded words within the Input Parameters block are our input args
         inputs = self.findBoldWords(inputArgs)

      #now do the same thing for the Output Parameters
      searchResult = re.search('<H2>Output Parameters</H2>\n<DL>(.+?)</DL>', html, re.MULTILINE | re.DOTALL)
      if searchResult and len(searchResult.groups()) >= 1:
         outputArgs = searchResult.group(1)
         #the bolded words within the Input Parameters block are our output args
         outputs = self.findBoldWords(outputArgs)

      return inputs, outputs

   def findBoldWords(self, html):
      """Given an html string, return a list of bolded words within that string"""

      #Yet Another Squishy Regex Bit (YASRB)
      boldWords = []
      for match in re.finditer('<B>(.+?)</B>', html, re.MULTILINE | re.DOTALL):
         if len(match.groups()) >= 1:
            boldWords.append(str(match.group(1)).strip())
      return boldWords

   def indentXML(self, elem, level=0):
      """Add some indentation to an ElementTree Element and its children for readability."""
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

