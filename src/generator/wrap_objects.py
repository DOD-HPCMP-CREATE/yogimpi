import xml.etree.ElementTree as ET
import sys

class MPIArgument(object):
    def __init__(self):
        # Original name of the argument.
        self.name = None
        # The way the argument is called in a function.
        self.call_name = None
        # Original type of the argument.
        self.type = None
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
        # Whether this argument is an MPI typedef or MPI structure.
        self.is_mpi_type = False
        # If the argument is an MPI type and is plural, the dimensions. Note
        # that this is not set for primitive type arrays.
        self.dims = None
        
        # The following variables are only used if is_mpi_type is True.
        # Otherwise the values all remain set to False and None.

        # The name of the temporary variable.
        self.temp_name = None
        # The type of the temporary variable.
        self.temp_type = None
        # Whether the temporary variable is a pointer.
        self.temp_is_ptr = False
        # The function used to convert the argument to the input variable.
        # If is_mpi_type is True and this is None, then it instantiates a 
        # variable without a function call.
        self.temp_pre_converter = None
        # In the case of an output variable, the function used to convert the
        # input variable back to the argument.
        self.temp_post_converter = None
        # Whether a handle will be freed by the operation.
        self.free_handle = False
        # If free_handle is True, what to set the freed value to for user.
        self.freed_value = None

class MPIConvertValue(object):
    def __init__(self):
        self.name = None
        # Whether the output of the conversion is a pointer value or not.
        self.is_pointer = False
        # Whether this conversion is a function or not.
        self.is_function = False

class MPIFunction(object):
    def __init__(self):
        self.name = None
        self.status_ignore = False
        self.status_ignore_type = None
        self.status_ignore_arg = 0
        self.return_type = 'int'
        self.args = []
    
    def getArg(self, name):
        for anArg in self.args:
            if anArg.name == name:
                return anArg
        return None
