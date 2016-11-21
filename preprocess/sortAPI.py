# Small script to determine exactly which MPI functions aren't supported.
# These are placed into an XML file that Yogi search and preprocessor
# utilities can understand. 

from lxml import etree as ET
from pprint import pprint
import sys
import copy

thisModule = sys.modules[__name__]

allNames = []
presentNames = []
missingNames = []
rawData = open("completeAPI", 'r')
allOfThem = rawData.readlines()
rawData.close()

for line in allOfThem:
    mew = line.split()
    for name in mew:
        if not name.startswith('MPI_T_') and not name.endswith('_x'):
            allNames.append(name)
allNames.sort()

tree = ET.parse("MissingMPI.xml")
xmlRoot = tree.getroot()
for aTag in xmlRoot.iterfind("Function"):
    funcName = aTag.attrib['name']
    missingNames.append(funcName)


tree2 = ET.parse("YogiSupported.xml")
xmlRoot = tree2.getroot()
langTag = xmlRoot.find('Language')
for aTag in langTag.iterfind("Function"):
    presentNames.append(aTag.text)

whatIHave = missingNames + presentNames
for name in allNames:
    if name not in whatIHave:
        print name
