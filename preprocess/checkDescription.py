# Small script to determine exactly which MPI functions aren't supported.
# These are placed into an XML file that Yogi search and preprocessor
# utilities can understand. 

from lxml import etree as ET
from pprint import pprint
import sys
import copy

thisModule = sys.modules[__name__]

cElements = {}
toAdd = []

tree = ET.parse("mpi22.xml")
xmlRoot = tree.getroot()
for aTag in xmlRoot.iterfind("Function"):
    funcName = aTag.attrib['name']
    if not funcName.startswith("MPI_T_"):
        cElements[funcName] = aTag

tree2 = ET.parse("ToImplement.xml")
xmlRoot = tree2.getroot()
for aFunc in xmlRoot.iterfind("Function"):
    toAdd.append(aFunc.text)

apiMap = ET.Element('MpichAPI')
xmlRoot = tree.getroot()
for aFunc in toAdd:
    apiMap.append(cElements[aFunc])

oPut = ET.ElementTree(element=apiMap)

oPut.write("MissingMPI.xml", pretty_print=True)
