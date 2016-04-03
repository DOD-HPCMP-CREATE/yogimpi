# Small script to determine exactly which MPI functions aren't supported.
# These are placed into an XML file that Yogi search and preprocessor
# utilities can understand. 

from lxml import etree as ET
from pprint import pprint
import sys
import copy

thisModule = sys.modules[__name__]

cNames = []
fNames = []
supportedC = []
supportedF = []

tree = ET.parse("mpi22.xml")
xmlRoot = tree.getroot()
for aTag in xmlRoot.iterfind("Function"):
    funcName = aTag.attrib['name']
    if not funcName.startswith("MPI_T_"):
        cNames.append(funcName)
        fNames.append(funcName)

tree2 = ET.parse("YogiSupported.xml")
xmlRoot = tree2.getroot()
for aLang in xmlRoot.iterfind("Language"):
    if aLang.get('name') == 'C':
        targetList = supportedC
    elif aLang.get('name') == 'Fortran':
        targetList = supportedF
    for aTag in aLang.iterfind("Function"):
        targetList.append(aTag.text)

langMap = { 'cNames':'supportedC',
            'fNames':'supportedF' }

for ll in langMap:
    langNames = getattr(thisModule, ll)
    langSupported = getattr(thisModule, langMap[ll])
    for aFunc in langSupported:
        try:
            langNames.remove(aFunc)
        except ValueError:
            print "Omitting " + aFunc + " removal"

topMissing = ET.Element('MPI2API')
cLang = ET.SubElement(topMissing, 'Language')
cLang.set('name', 'C')
fLang = ET.SubElement(topMissing, 'Language')
fLang.set('name', 'Fortran')

writeMap = { 'cNames':cLang,
             'fNames':fLang }

for wl in writeMap:
    langNames = getattr(thisModule, wl)
    langTag = writeMap[wl]
    for aFunc in langNames:
        funky = ET.Element('Function')    
        funky.text = aFunc
        langTag.append(funky)

oPut = ET.ElementTree(element=topMissing)

oPut.write("UnsupportedMPI.xml", pretty_print=True)
