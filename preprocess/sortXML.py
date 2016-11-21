# Small script to determine exactly which MPI functions aren't supported.
# These are placed into an XML file that Yogi search and preprocessor
# utilities can understand. 

from lxml import etree as ET
from pprint import pprint
import sys
import copy

thisModule = sys.modules[__name__]

cElements = {}
cNames = []
toAdd = []

tree = ET.parse("MissingMPI.xml")
xmlRoot = tree.getroot()
for aTag in xmlRoot.iterfind("Function"):
    funcName = aTag.attrib['name']
    cElements[funcName] = aTag
    cNames.append(funcName)
cNames.sort()


apiMap = ET.Element('MpichAPI')
xmlRoot = tree.getroot()
for aFunc in cNames:
    apiMap.append(cElements[aFunc])

oPut = ET.ElementTree(element=apiMap)

oPut.write("SortedMissing.xml", pretty_print=True)
