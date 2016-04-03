# Small script to determine exactly which MPI functions aren't supported at
# the C level of YogiMPI

import xml.etree.ElementTree as ET
from pprint import pprint

allNames = []
supported = []

tree = ET.parse("mpi22.xml")
xmlRoot = tree.getroot()
for aTag in xmlRoot.iterfind("Function"):
    funcName = aTag.attrib['name']
    if not funcName.startswith("MPI_T_"):
        allNames.append(funcName)

tree2 = ET.parse("preprocessor.xml")
xmlRoot = tree2.getroot()
cStuff = xmlRoot.find("Language")
for aTag in cStuff.iterfind("Function"):
    supported.append(aTag.text)

for blah in supported:
    try:
        allNames.remove(blah)
    except ValueError:
        print "Omitting " + blah + " removal"

print "Total of " + str(len(allNames)) + " missing functions."
pprint(allNames)
