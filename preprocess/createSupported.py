# Small script to determine exactly which MPI functions aren't supported.
# These are placed into an XML file that Yogi search and preprocessor
# utilities can understand. 

from lxml import etree as ET
from pprint import pprint
import sys
import copy

thisModule = sys.modules[__name__]

presentNames = []
cElements = {}

tree = ET.parse("mpi22.xml")
xmlRoot = tree.getroot()
for funcTag in xmlRoot.iterfind("Function"):
    funcName = funcTag.attrib['name']
    if not funcName.startswith('MPI_T_') and not funcName.endswith('_x'):
        cElements[funcName] = funcTag

tree2 = ET.parse("YogiSupported.xml")
xmlRoot = tree2.getroot()
langTag = xmlRoot.find('Language')
for aTag in langTag.iterfind("Function"):
    presentNames.append(aTag.text)
presentNames.sort()

apiMap = ET.Element('MpichAPI')
for aFunc in presentNames:
    if not (aFunc.endswith('_c2f') or aFunc.endswith('_f2c')):
        apiMap.append(cElements[aFunc])

oPut = ET.ElementTree(element=apiMap)

oPut.write("PresentMPI.xml", pretty_print=True)
