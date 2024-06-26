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

import sys

class MPIVersion(object):
    def __init__(self, version):
        splitVer = version.split('.')
        self.major = int(splitVer[0].strip())
        self.minor = int(splitVer[1].strip())

    def __eq__(self, other):
        if isinstance(other, MPIVersion):
            return (self.major == other.major) and (self.minor == other.minor)
        else:
            return NotImplemented

    def __ne__(self, other):
        result = self.__eq__(other)
        if result is NotImplemented:
            return result
        return not result

    def __lt__(self, other):
        if isinstance(other, MPIVersion):
            if self.major < other.major:
                return True
            if self.major == other.major:
                if self.minor < other.minor:
                    return True
            return False
        else:
            return NotImplemented

    def __gt__(self, other):
        if isinstance(other, MPIVersion):
            if self.major > other.major:
                return True
            if self.major == other.major:
                if self.minor > other.minor:
                    return True
            return False
        else:
            return NotImplemented

    def __le__(self, other):
        if isinstance(other, MPIVersion):
            return self.__eq__(other) or self.__lt__(other)
        else:
            return NotImplemented

    def __ge__(self, other):
        if isinstance(other, MPIVersion):
            return self.__eq__(other) or self.__gt__(other)
        else:
            return NotImplemented

    def __repr__(self):
        return 'MPIVersion({!s})'.format(self)

    def __str__(self):
        return '{}.{}'.format(self.major, self.minor)

class MPICode(object):
    def __init__(self, orders=None):
        self.codeBlockOrders = []
        self.codeBlocks = {}
        if orders is not None:
            self.codeBlockOrders += orders

    def _checkOrder(self, order):
        if order not in self.codeBlockOrders:
            raise ValueError("Error setting block in function " + self.name +\
                             ": order " + order + " not recognized.")

    def addBlock(self, block, order):
        self._checkOrder(order)
        # Add line by line and don't add blank lines.
        self.codeBlocks[order] = []
        for line in block.split('\n'):
            if line.rstrip():
                self.codeBlocks[order].append(line.rstrip())

    def getBlock(self, order):
        self._checkOrder(order)
        return self.codeBlocks.get(order, None)

class MPIArgument(MPICode):
    def __init__(self):
        super().__init__(orders=['convertelse'])

        # Original name of the argument.
        self.name = None
        # The way the argument is called in a function.
        self.call_name = None
        # C API type of the argument.  This is normally the same as the
        # original type, but it may include const qualifiers that make C++
        # happy.
        self.c_api_type = None
        # Original type of the argument.
        self.type = None
        # Whether the argument is const.
        self.is_const = False
        # Whether the argument is a pointer.
        self.is_pointer = False
        # Whether the argument is an array.  Also makes is_pointer True.
        self.is_plural = False
        # Whether the argument is considered output.
        self.is_output = False
        # Whether the argument is considered input.
        self.is_input = False
        # Values that, if matching Yogi equivalent of argument, cause a
        # conversion to that value.  (Example: YogiMPI_UNDEFINED to
        # MPI_UNDEFINED).
        self.pre_convert_values = []
        # Values that, if matching Yogi equivalent of argument, cause a
        # conversion to the value, except it happens AFTER the MPI call.
        self.post_convert_values = []
        # Set if the argument is automatically converted by a special "class"
        # of known constants. Useful for avoiding long lists of constants in
        # the XML file.
        self.convert_class = None
        # Whether this argument is an MPI typedef or MPI structure.
        self.is_mpi_type = False
        # If the argument is plural, the dimensions. This is not necessarily
        # set for all plural arguments, but it is required for MPI type args.
        self.dims = None

        # The following variables are only used if is_mpi_type is True.
        # Otherwise the values all remain set to False and None.

        # The name of the MPI variable.
        self.mpi_name = None
        # The type of the MPI variable.
        self.mpi_type = None
        # Whether the MPI variable is a pointer.
        self.mpi_is_ptr = False
        # Whether the MPI variable is a function pointer.
        self.mpi_func_ptr = False
        # Whether a handle will be freed by the operation.
        self.free_handle = False

    def validate(self):
        if self.is_output:
            if not self.is_pointer:
                errMsg = self.name + " is marked output but isn't a pointer."
                raise ValueError(errMsg)

    def preConvertNames(self):
        nameList = []
        for val in self.pre_convert_values:
            nameList.append(val.name)
        return nameList

    def __str__(self):
        return str(self.__dict__)

class MPIConvertValue(object):
    def __init__(self):
        self.name = None
        # Whether the output of the conversion is a pointer value or not.
        self.is_pointer = False
        # Whether the object requires a type casting.
        self.cast_type = None
        # What version of MPI this conversion value appears.
        self.version = 2.0

    @property
    def version(self):
        return self._version

    @version.setter
    def version(self, value):
        self._version = MPIVersion(str(value))

class MPIFunction(MPICode):

    def __init__(self):
        super().__init__(orders=['first', 'beforecall', 'aftercall', 'beforereturn'])
        self.name = None
        self.status_ignore = False
        self.status_ignore_type = None
        self.status_ignore_arg = 0
        self.return_type = 'int'
        self.args = []
        self.fortran_support = True
        self.mpi_version = None

    def validate(self):
        for anArg in self.args:
            try:
                anArg.validate()
            except ValueError as v:
                print("Error validating function " + self.name + ": " + str(v))
                sys.exit(1)

    def cArgString(self):
        arg_string = ''
        for i, anArg in enumerate(self.args):
            if i > 0:
                arg_string += ', '
            argType = anArg.c_api_type
            if anArg.is_mpi_type:
                argType = 'Yogi' + anArg.type
                if anArg.is_const:
                    argType = 'const ' + argType
            arg_string += argType + ' ' + anArg.name
        return arg_string

    def getArg(self, name):
        for anArg in self.args:
            if anArg.name == name:
                return anArg
        return None
