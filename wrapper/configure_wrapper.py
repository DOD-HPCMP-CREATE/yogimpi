import xml.etree.ElementTree as ET

tree = ET.parse('preprocess.xml')
root = tree.getroot()

data = {}
tags = [ 'Functions', 'Objects', 'Constants' ]

inHandle = open('YogiMPIWrapper.py.in', 'r')
wrapperContents = inHandle.read()
inHandle.close()

for tag in tags:
    data[tag] = []
    tagElem = root.find(tag)
    subTag = tag[:-1]
    for aTag in tagElem.findall(subTag):
       data[tag].append(aTag.attrib.get('name'))
    varName = 'mpi' + tag
    subString = '@MPI_' + tag.upper() + '@'
    outString = varName + ' = [ '
    for i in range(len(data[tag])):
        item = data[tag][i]
        if i > 0: 
            outString += ', ' 
        outString += "'" + item + "'"
    outString += ' ]'
    wrapperContents = wrapperContents.replace(subString, outString)

outHandle = open('YogiMPIWrapper.py', 'w')
outHandle.write(wrapperContents)
outHandle.close()
