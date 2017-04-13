"""
                                  COPYRIGHT
 
 The following is a notice of limited availability of the code, and disclaimer
 which must be included in the prologue of the code and in all source listings
 of the code.
 
 Copyright Notice
  + 2002 University of Chicago
  + 2016 Stephen Adamec

 Permission is hereby granted to use, reproduce, prepare derivative works, and
 to redistribute to others.  This software was authored by:
 
 Mathematics and Computer Science Division
 Argonne National Laboratory, Argonne IL 60439
 
 (and)
 
 Department of Computer Science
 University of Illinois at Urbana-Champaign

 (and)

 Stephen Adamec
 
                              GOVERNMENT LICENSE
 
 Portions of this material resulted from work developed under a U.S.
 Government Contract and are subject to the following license: the Government
 is granted for itself and others acting on its behalf a paid-up, nonexclusive,
 irrevocable worldwide license in this computer software to reproduce, prepare
 derivative works, and perform publicly and display publicly.
  
                                    DISCLAIMER
  
 This computer code material was prepared, in part, as an account of work
 sponsored by an agency of the United States Government.  Neither the United
 States, nor the University of Chicago, nor any of their employees, makes any
 warranty express or implied, or assumes any legal liability or responsibility
 for the accuracy, completeness, or usefulness of any information, apparatus,
 product, or process disclosed, or represents that its use would not infringe
 privately owned rights.
"""

import re

class SourceFile(object):

    def __init__(self, startFile=None):
        if startFile:
            with open(startFile, 'r') as startHandle:
                self.sourceString = startHandle.read()
        else:
            self.sourceString = ''
        self.currentIndent = 0

    def replace(self, oldTerm, newTerm):
        self.sourceString = self.sourceString.replace(oldTerm, newTerm)

    def replaceWithTemplate(self, oldTerm, template, strip=False):
        sFile = SourceFile()
        sFile.addTemplate(template)
        if strip:
            self.replace(oldTerm, sFile.sourceString.strip())
        else:
            self.replace(oldTerm, sFile.sourceString) 

    def _asList(self):
        return self.sourceString.split('\n')

    def _fromList(self, lineList):
        self.sourceString = '\n'.join(lineList)

    def getIndent(self):
        return self.currentIndent

    def addIndent(self):
        self.currentIndent += 4

    def removeIndent(self, numIndents=1):
        self.currentIndent -= (4 * numIndents)
        if self.currentIndent < 0:
            self.currentIndent = 0

    def addSegments(self, *rawSegments):
        for segment in rawSegments:
            self.sourceString += (' ' * self.currentIndent) + segment
    
    def addUnindentedLines(self, *rawLines):
        for rawLine in rawLines:
            self.sourceString += rawLine + '\n'

    def addLines(self, *rawLines):
        for rawLine in rawLines:
            self.sourceString += (' ' * self.currentIndent) + rawLine + '\n'
    
    def addTemplate(self, aTemplate):
        for aLine in aTemplate:
            self.sourceString += (' ' * self.currentIndent) + aLine        

    def newLine(self):
        self.sourceString += '\n'

    ## Finds a merge tag within source lines.
    #  mergeTag - Tag to locate within source lines.
    #  Returns a tuple (tagLocation, tagIndent) with the line location of the
    #  tag and the tag indentation.
    def _findMergeTag(self, mergeTag):
        sourceLines = self._asList()
        tagLocation = -1
        tagIndent = 0 
        for i in range(len(sourceLines)):
            result = re.match("(\s*)@" + mergeTag + "@", sourceLines[i])
            if result:
                tagLocation = i
                tagIndent = len(result.group(1))
                break
        if tagLocation < 0:
            raise ValueError("Specified merge tag " + mergeTag + " not found.")
        return (tagLocation, tagIndent)

    ## Performs a simple merge of another SourceFile into this one.
    #  mergeFile - SourceFile object to be merged into this one.
    #  mergeTag - Location at which to merge object.  Merge tags have @
    #             symbols surrounding them in the target file.
    #             Example is @PUTSTUFFHERE@, and the mergeTag is PUTSTUFFHERE.
    #             The mergeTag is replaced with the file's contents, and the
    #             indentation level of the mergeTag determines how the paste
    #             is placed.
    def merge(self, mergeFile, mergeTag):
        sourceLines = self._asList() 
        # First, find the mergeTag.  Determine the indent.
        tagLocation, tagIndent = self._findMergeTag(mergeTag)

        otherLines = mergeFile._asList() 
        if tagIndent > 0:
            for i in range(len(otherLines)):
                otherLines[i] = ' ' * tagIndent + otherLines[i]
        sourceLines[tagLocation:tagLocation + 1] = otherLines
        self._fromList(sourceLines)

    ## Performs a simple merge of a SourceTemplate entry into this one. 
    #  mergeEntry - SourceTemplate entry to be merged into this one.
    #  mergeTag - Location at which to merge object.  Merge tags have @
    #             symbols surrounding them in the target file.
    #             Example is @PUTSTUFFHERE@, and the mergeTag is PUTSTUFFHERE.
    #             The mergeTag is replaced with the entry's contents, and the
    #             indentation level of the mergeTag determines how the paste
    #             is placed.
    def mergeTemplate(self, mergeEntry, mergeTag):
        sourceLines = self._asList()
        # First, find the mergeTag.  Determine the indent.
        tagLocation, tagIndent = self._findMergeTag(mergeTag)

        stripEntries = []
        if tagIndent > 0:
            for i in range(len(mergeEntry)):
                stripEntries.append(' ' * tagIndent + mergeEntry[i].rstrip())
        sourceLines[tagLocation:tagLocation + 1] = stripEntries 
        self._fromList(sourceLines)
 
    def writeFile(self, location, append=False):        
        if append:
            handle = open(location, 'a')
        else:
            handle = open(location, 'w')
        handle.write(self.sourceString)
        handle.close()
        
class CSource(SourceFile):
    
    def __init__(self, inputFile=None):
        self.functions = []
        self.switches = [] 
        self.cases = 0
        self.forLoops = 0
        self.insideStruct = False
        super(CSource,self).__init__(inputFile)

    def addStructDecl(self, name):
        if self.insideStruct:
            raise ValueError("Error: already inside struct declaration.")
        else:
            self.addLines("typedef struct " + name + " {")
            self.addIndent()
            self.insideStruct = True
            
    def endStructDecl(self, name):
        if not self.insideStruct:
            raise ValueError("Error: not inside struct declaration.")
        else:
            self.removeIndent()
            self.addLines("} " + name + "_t;")
            self.insideStruct = False

    # Add a for loop with conditional
    def addFor(self, conditional):
        self.addLines("for (" + conditional + ") {")
        self.addIndent()
        self.forLoops += 1

    ## End a previous for loop
    def endFor(self):
        if self.forLoops == 0:
            raise ValueError("Error: no for statements exist.")
        else:
            self.removeIndent()
            self.addLines("}")
            self.forLoops -= 1
            
    # Add an if statement with conditional
    def addIf(self, conditional):
        self.addLines("if (" + conditional + ") {")
        self.addIndent()
    
    ## End a previous if statement 
    def endIf(self):
        self.removeIndent()
        self.addLines("}")
            
    ## Add an else if statement to an existing if statement
    def addElseIf(self, conditional):
        self.addLines("else if(" + conditional + ") {")
        self.addIndent()

    ## End a previous if statement 
    def endElseIf(self):
        self.removeIndent()
        self.addLines("}")
            
    ## Add an else statement to an existing if statement
    def addElse(self):
        self.addLines("else {")
        self.addIndent()

    ## Add an else statement to an existing if statement
    def endElse(self):
        self.removeIndent()
        self.addLines("}")
            
    def addComments(self, *commentLines):
        numLines = len(commentLines)
        if numLines > 1:
            self.addLines('/*')
            for commentLine in commentLines: 
                self.addLines('* ' + commentLine)
            self.addLines('*/')
        else:
            for commentLine in commentLines:
                self.addLines('/* ' + commentLine + ' */')
                
    def addFunction(self, name, returnType, args='void'):
        self.addLines(returnType + ' ' + name + '(' + args + ') {')
        self.addIndent()
        self.functions.append(name)

    def addSwitch(self, expression):
        self.addLines("switch(" + expression + ") {")
        self.addIndent()
        self.switches.append(expression)
 
    def addPrototype(self, name, returnType, args='void'):
        self.addLines(returnType + ' ' + name + '(' + args + ');')
    
    def endFunction(self, name):
        if not self.functions.pop() == name:
            raise ValueError("Error: " + name + " not most nested function.")
        self.removeIndent()
        self.addLines('}')

    def addCase(self, expression):
        if self.switches == 0:
            raise ValueError("Error: no switch statements exist.")
        else:
            self.addLines("case " + expression + ":")
            self.addIndent()
            self.cases += 1

    def endCase(self):
        if self.cases == 0:
            raise ValueError("Error: no cases exist.")
        else:
            self.addLines("break;")
            self.removeIndent()
            self.cases -= 1

    def endSwitch(self, expression):
        if not self.switches.pop() == expression:
            raise ValueError("Error: " + expression + " not most nested switch.")
        self.removeIndent()
        self.addLines('}')

class FortranSource(SourceFile):
    
    def __init__(self, inputFile=None):
        self.modules = []
        self.subroutines = []
        self.functions = []
        self.selects = []
        self.doLoops = 0
        self.interfaces = 0 
        self.ifBranches = 0
        self.insideType = False
        super(FortranSource,self).__init__(inputFile)
    
    def addModule(self, name, parent="object"):
        self.addLines('module ' + name)
        self.addIndent()
        self.modules.append(name)

    def endModule(self, name):
        if not self.modules.pop() == name:
            raise ValueError("Error: " + name + " not most nested module.")
        self.removeIndent()
        self.addLines('end module ' + name)

    def addTypeDecl(self, name):
        if self.insideType:
            raise ValueError("Error: already inside type declaration.")
        else:
            self.addLines('type ' + name)
            self.addIndent()
            self.insideType = True
            
    def endTypeDecl(self, name):
        if not self.insideType:
            raise ValueError("Error: not inside type declaration.")
        else:
            self.removeIndent()
            self.addLines('end type')
            self.insideType = False

    def addInterface(self, name=None):
        iface = "interface"
        if name:
            iface += " " + name
        self.addLines(iface)
        self.addIndent()
        self.interfaces += 1

    ## End a previous if statement 
    def endInterface(self):
        if self.interfaces == 0:
            raise ValueError("Error: no interfaces are open.")
        else:
            self.removeIndent()
            self.addLines("end interface")
            self.interfaces -= 1

    # Add an if statement with conditional
    def addIf(self, conditional):
        self.addLines("if (" + conditional + ") then")
        self.addIndent()
        self.ifBranches += 1
    
    ## End a previous if statement 
    def endIf(self):
        if self.ifBranches == 0:
            raise ValueError("Error: no if statements exist.")
        else:
            self.removeIndent()
            self.addLines("end if")
            self.ifBranches -= 1
                      
    ## Add an else if statement to an existing if statement
    def addElseIf(self, conditional):
        if self.ifBranches == 0:
            raise ValueError("Error: no if statements exist.")
        else:
            self.removeIndent()
            self.addLines("else if (" + conditional + ") then")
            self.addIndent()
    
    ## Add an else statement to an existing if statement
    def addElse(self):
        if self.ifBranches == 0:
            raise ValueError("Error: no if statements exist.")
        else:
            self.removeIndent()
            self.addLines("else")
            self.addIndent()
            
    # Add an do loop with iterator
    def addDo(self, iterator):
        self.addLines("do " + iterator)
        self.addIndent()
        self.doLoops += 1
    
    ## End a previous do loop
    def endDo(self):
        if self.doLoops == 0:
            raise ValueError("Error: no do loops exist.")
        else:
            self.removeIndent()
            self.addLines("end do")
            self.doLoops -= 1
            
    def addComments(self, *commentLines):
        for commentLine in commentLines:
            self.addLines('! ' + commentLine)

    def addSubroutine(self, name, args="", bind=False, implicit=True):
        if bind:
            addBind = " bind(C)"
        else:
            addBind = "" 
        self.addLines('subroutine ' + name + '(' + args + ')' + addBind)
        self.addIndent()
        if implicit:
            self.addLines('implicit none')
        self.subroutines.append(name)

    def endSubroutine(self, name):
        if not self.subroutines.pop() == name:
            raise ValueError("Error: " + name + " not most nested subroutine.")
        self.removeIndent()
        self.addLines('end subroutine ' + name)
                
    def addFunction(self, name, returnType, args="", result=None):
        if not result:
            self.addLines(returnType + ' function ' + name + '(' + args + ')')
        else:
            self.addLines(returnType + ' function ' + name + '(' + args +\
                          ') result(' + result + ')')
        self.addIndent()
        self.addLines('implicit none')
        self.functions.append(name)
    
    def endFunction(self, name):
        if not self.functions.pop() == name:
            raise ValueError("Error: " + name + " not most nested function.")
        self.removeIndent()
        self.addLines('end function ' + name)

    def addSelect(self, name):
        self.addLines('select case (' + name + ')')
        self.addIndent()
        self.selects.append(name)

    def addCase(self, label):
        if len(self.selects) == 0:
            raise ValueError("Error: no select statements exist.")
        self.addLines('case (' + label + ')')
        self.addIndent()

    def endCase(self, label):
        if len(self.selects) == 0:
            raise ValueError("Error: no select statements exist.")
        self.removeIndent()

    def endSelect(self, name):
        if not self.selects.pop() == name:
            raise ValueError("Error: " + name + " not most nested select.")
        self.removeIndent()
        self.addLines('end select')

class PythonSource(SourceFile):
    
    def __init__(self, inputFile=None):
        self.classes = []
        self.functions = []
        self.ifBranches = 0
        self.forLoops = 0
        super(PythonSource,self).__init__(inputFile)

    def addClass(self, name, parent="object"):
        self.addLines('class ' + name + '(' + parent + '):')
        self.addIndent()
        self.classes.append(name)
        
    def addFunction(self, name, args=""):
        self.addLines('def ' + name + '(' + args + '):')
        self.addIndent()
        self.functions.append(name)

    def endClass(self, name):
        if not self.classes.pop() == name:
            raise ValueError("Error: " + name + " not most nested class.")
        self.removeIndent()

    def endFunction(self, name):
        if not self.functions.pop() == name:
            raise ValueError("Error: " + name + " not most nested function.")
        self.removeIndent()
        
    # Add an if statement with conditional
    def addIf(self, conditional):
        self.addLines("if " + conditional + ":")
        self.addIndent()
        self.ifBranches += 1
    
    ## End a previous if statement 
    def endIf(self):
        if self.ifBranches == 0:
            raise ValueError("Error: no if statements exist.")
        else:
            self.removeIndent()
            self.ifBranches -= 1
            
    ## Add an else if statement to an existing if statement
    def addElseIf(self, conditional):
        if self.ifBranches == 0:
            raise ValueError("Error: no if statements exist.")
        else:
            self.removeIndent()
            self.addLines("elif " + conditional + ":")
            self.addIndent()
    
    ## Add an else statement to an existing if statement
    def addElse(self):
        if self.ifBranches == 0:
            raise ValueError("Error: no if statements exist.")
        else:
            self.removeIndent()
            self.addLines("else:")
            self.addIndent()

    # Add a for loop
    def addFor(self, conditional):
        self.addLines("for " + conditional + ":")
        self.addIndent()
        self.forLoops += 1
    
    ## End a previous for loop 
    def endFor(self):
        if self.forLoops == 0:
            raise ValueError("Error: no for loops exist.")
        else:
            self.removeIndent()
            self.forLoops -= 1

    def addComments(self, *commentLines):
        for commentLine in commentLines:
            self.addLines('# ' + commentLine)

class SourceTemplate(object):
    def __init__(self, templateFile):
        self.entries = {}
        activeTemplate = None
        with open(templateFile, 'r') as th:
            for line in th:
                if line.startswith('#!'):
                    thisTemplate = line.lstrip('#!').strip()
                    if not activeTemplate:
                        self.entries[thisTemplate] = []
                        activeTemplate = thisTemplate
                    elif activeTemplate == thisTemplate:
                        activeTemplate = None
                    else:
                        raise ValueError("Template " + thisTemplate +\
                                         " doesn't match " + activeTemplate)
                elif not activeTemplate:
                    if not line.strip():
                        continue
                    raise ValueError("Line read without template section!")
                else:
                    self.entries[activeTemplate].append(line) 

    def getEntry(self, name):
        if name in self.entries:
            return list(self.entries[name])
        return None
