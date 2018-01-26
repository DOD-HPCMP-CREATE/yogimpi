#!/usr/bin/env python

import os
import sys
import time
import subprocess
import tempfile
import filecmp
import re

# MPI constants currently supported in Fortran.
mpiConstants = [ 'MPI_VERSION', 'MPI_SUBVERSION', 'MPI_INTEGER_KIND',
                 'MPI_OFFSET_KIND', 'MPI_ADDRESS_KIND', 'MPI_SUCCESS',
                 'MPI_ERR_BUFFER', 'MPI_ERR_COUNT', 'MPI_ERR_TYPE',
                 'MPI_ERR_TAG', 'MPI_ERR_COMM', 'MPI_ERR_RANK',
                 'MPI_ERR_REQUEST', 'MPI_ERR_ROOT', 'MPI_ERR_GROUP',
                 'MPI_ERR_OP', 'MPI_ERR_TOPOLOGY', 'MPI_ERR_DIMS',
                 'MPI_ERR_ARG', 'MPI_ERR_UNKNOWN', 'MPI_ERR_TRUNCATE',
                 'MPI_ERR_OTHER', 'MPI_ERR_INTERN', 'MPI_ERR_PENDING',
                 'MPI_ERR_IN_STATUS', 'MPI_ERR_FILE', 'MPI_ERR_NOT_SAME',
                 'MPI_ERR_AMODE', 'MPI_ERR_UNSUPPORTED_DATAREP',
                 'MPI_ERR_UNSUPPORTED_OPERATION', 'MPI_ERR_NO_SUCH_FILE',
                 'MPI_ERR_FILE_EXISTS', 'MPI_ERR_BAD_FILE', 'MPI_ERR_ACCESS',
                 'MPI_ERR_NO_SPACE', 'MPI_ERR_QUOTA', 'MPI_ERR_READ_ONLY',
                 'MPI_ERR_FILE_IN_USE', 'MPI_ERR_DUP_DATAREP',
                 'MPI_ERR_CONVERSION', 'MPI_ERR_IO', 'MPI_ERR_INFO',
                 'MPI_ERR_INFO_KEY', 'MPI_ERR_INFO_VALUE', 'MPI_ERR_INFO_NOKEY',
                 'MPI_ERR_NAME', 'MPI_ERR_NO_MEM', 'MPI_ERR_PORT',
                 'MPI_ERR_SERVICE', 'MPI_ERR_SPAWN', 'MPI_ERR_WIN',
                 'MPI_ERR_KEYVAL', 'MPI_ERR_BASE', 'MPI_ERR_LOCKTYPE',
                 'MPI_ERR_RMA_CONFLICT', 'MPI_ERR_RMA_SYNC', 'MPI_ERR_SIZE',
                 'MPI_ERR_DISP', 'MPI_ERR_ASSERT', 'MPI_ERR_LASTCODE',
                 'MPI_BOTTOM', 'MPI_PROC_NULL', 'MPI_ROOT', 'MPI_ANY_SOURCE',
                 'MPI_ANY_TAG', 'MPI_UNDEFINED', 'MPI_BSEND_OVERHEAD',
                 'MPI_KEYVAL_INVALID', 'MPI_WEIGHTS_EMPTY', 'MPI_UNWEIGHTED',
                 'MPI_MODE_NOCHECK', 'MPI_MODE_NOSTORE', 'MPI_MODE_NOPRECEDE',
                 'MPI_MODE_NOSUCCEED', 'MPI_MODE_NOPUT', 'MPI_SEEK_SET',
                 'MPI_SEEK_CUR',
                 'MPI_SEEK_END', 'MPI_TYPECLASS_REAL', 'MPI_TYPECLASS_INTEGER',
                 'MPI_TYPECLASS_COMPLEX', 'MPI_MODE_RDONLY', 'MPI_MODE_RDWR',
                 'MPI_MODE_WRONLY', 'MPI_MODE_CREATE', 'MPI_MODE_EXCL',
                 'MPI_MODE_DELETE_ON_CLOSE', 'MPI_MODE_UNIQUE_OPEN',
                 'MPI_MODE_SEQUENTIAL', 'MPI_MODE_APPEND', 'MPI_MAX_INFO_KEY',
                 'MPI_MAX_INFO_VAL', 'MPI_ORDER_C', 'MPI_ORDER_FORTRAN',
                 'MPI_DISTRIBUTE_BLOCK', 'MPI_DISTRIBUTE_CYCLIC',
                 'MPI_DISTRIBUTE_NONE',
                 'MPI_DISTRIBUTE_DFLT_DARG', 'MPI_MAX_PROCESSOR_NAME',
                 'MPI_MAX_ERROR_STRING', 'MPI_MAX_PORT_NAME',
                 'MPI_MAX_OBJECT_NAME', 'MPI_MAX_DATAREP_STRING',
                 'MPI_CHARACTER', 'MPI_INTEGER',
                 'MPI_REAL', 'MPI_DOUBLE_PRECISION', 'MPI_BYTE', 'MPI_PACKED',
                 'MPI_COMPLEX', 'MPI_DOUBLE_COMPLEX', 'MPI_LOGICAL',
                 'MPI_2REAL', 'MPI_2DOUBLE_PRECISION', 'MPI_2INTEGER',
                 'MPI_INTEGER1', 'MPI_INTEGER2', 'MPI_INTEGER4', 'MPI_INTEGER8',
                 'MPI_REAL4', 'MPI_REAL8', 'MPI_LB', 'MPI_UB', 'MPI_COMM_WORLD',
                 'MPI_COMM_SELF', 'MPI_IDENT', 'MPI_CONGRUENT', 'MPI_SIMILAR',
                 'MPI_UNEQUAL', 'MPI_WIN_BASE', 'MPI_WIN_SIZE',
                 'MPI_WIN_DISP_UNIT', 'MPI_TAG_UB', 'MPI_IO', 'MPI_HOST',
                 'MPI_WTIME_IS_GLOBAL', 'MPI_MAX', 'MPI_MIN', 'MPI_SUM',
                 'MPI_PROD', 'MPI_MAXLOC', 'MPI_MINLOC', 'MPI_BAND', 'MPI_BOR',
                 'MPI_BXOR', 'MPI_LAND', 'MPI_LOR', 'MPI_LXOR', 'MPI_REPLACE',
                 'MPI_GROUP_NULL', 'MPI_COMM_NULL', 'MPI_DATATYPE_NULL',
                 'MPI_REQUEST_NULL', 'MPI_OP_NULL', 'MPI_INFO_NULL',
                 'MPI_FILE_NULL', 'MPI_ERRHANDLER_NULL', 'MPI_WIN_NULL',
                 'MPI_ERRORS_ARE_FATAL', 'MPI_ERRORS_RETURN', 'MPI_GROUP_EMPTY',
                 'MPI_THREAD_SINGLE', 'MPI_THREAD_FUNNELED',
                 'MPI_THREAD_SERIALIZED', 'MPI_THREAD_MULTIPLE',
                 'MPI_LOCK_EXCLUSIVE', 'MPI_LOCK_SHARED', 'MPI_GRAPH',
                 'MPI_CART', 'MPI_DIST_GRAPH', 'MPI_COMBINER_NAMED',
                 'MPI_COMBINER_DUP', 'MPI_COMBINER_CONTIGUOUS',
                 'MPI_COMBINER_VECTOR', 'MPI_COMBINER_HVECTOR_INTEGER',
                 'MPI_COMBINER_HVECTOR', 'MPI_COMBINER_INDEXED',
                 'MPI_COMBINER_HINDEXED_INTEGER', 'MPI_COMBINER_HINDEXED',
                 'MPI_COMBINER_INDEXED_BLOCK', 'MPI_COMBINER_STRUCT_INTEGER',
                 'MPI_COMBINER_STRUCT', 'MPI_COMBINER_SUBARRAY',
                 'MPI_COMBINER_DARRAY', 'MPI_COMBINER_F90_REAL',
                 'MPI_COMBINER_F90_COMPLEX', 'MPI_COMBINER_F90_INTEGER',
                 'MPI_COMBINER_RESIZED', 'MPI_COMM_TYPE_SHARED',
                 'MPI_STATUS_SIZE', 'MPI_SOURCE', 'MPI_TAG', 'MPI_ERROR',
                 'MPI_STATUS_IGNORE', 'MPI_STATUSES_IGNORE', 'MPI_IN_PLACE' ]

# MPI objects currently supported in Fortran.
mpiObjects = [ 'MPI_Comm', 'MPI_Request', 'MPI_Op', 'MPI_Info', 'MPI_Datatype',
               'MPI_Group', 'MPI_Status', 'MPI_File', 'MPI_Win',
               'MPI_Errhandler' ]

# MPI time functions, which are the only functions to return a non-integer
# value.
mpiTimeFunctions = [ 'MPI_Wtime', 'MPI_Wtick' ]

# MPI functions currently supported in Fortran.
mpiFunctions = [ 'MPI_Init', 'MPI_Finalize', 'MPI_Get_processor_name',
                 'MPI_Abort', 'MPI_Address', 'MPI_Allgather',
                 'MPI_Allgatherv', 'MPI_Allreduce', 'MPI_Alltoall',
                 'MPI_Alltoallv', 'MPI_Attr_delete', 'MPI_Attr_get',
                 'MPI_Attr_put', 'MPI_Barrier', 'MPI_Bcast', 'MPI_Bsend',
                 'MPI_Buffer_attach', 'MPI_Buffer_detach', 'MPI_Cancel',
                 'MPI_Cart_coords', 'MPI_Cart_create',
                 'MPI_Comm_call_errhandler', 'MPI_Comm_compare',
                 'MPI_Comm_create', 'MPI_Comm_create_errhandler',
                 'MPI_Comm_create_keyval', 'MPI_Comm_delete_attr',
'MPI_Comm_dup', 'MPI_Comm_free', 'MPI_Comm_free_keyval', 'MPI_Comm_get_attr',
'MPI_Comm_get_errhandler', 'MPI_Comm_group', 'MPI_Comm_rank',
'MPI_Comm_remote_size', 'MPI_Comm_set_attr', 'MPI_Comm_set_errhandler',
'MPI_Comm_size', 'MPI_Comm_split', 'MPI_Dims_create',
'MPI_Errhandler_free', 'MPI_Errhandler_set', 'MPI_Error_class',
'MPI_Error_string', 'MPI_File_close', 'MPI_File_delete',
'MPI_File_get_atomicity', 'MPI_File_get_info', 'MPI_File_get_size',
'MPI_File_open', 'MPI_File_read', 'MPI_File_read_all', 'MPI_File_read_at',
'MPI_File_read_at_all', 'MPI_File_seek', 'MPI_File_set_atomicity',
'MPI_File_set_size', 'MPI_File_set_view', 'MPI_File_sync', 'MPI_File_write',
'MPI_File_write_all', 'MPI_File_write_at', 'MPI_File_write_at_all',
'MPI_Finalized', 'MPI_Gather', 'MPI_Gatherv', 'MPI_Get_address',
'MPI_Get_count', 'MPI_Get_elements', 'MPI_Group_excl', 'MPI_Group_free',
'MPI_Group_incl', 'MPI_Group_range_excl', 'MPI_Group_range_incl',
'MPI_Group_rank', 'MPI_Group_size', 'MPI_Group_translate_ranks',
'MPI_Info_create', 'MPI_Info_dup', 'MPI_Info_free', 'MPI_Info_get',
'MPI_Info_get_nkeys', 'MPI_Info_get_nthkey', 'MPI_Info_get_valuelen',
'MPI_Info_set', 'MPI_Initialized', 'MPI_Intercomm_create',
'MPI_Iprobe', 'MPI_Irecv', 'MPI_Irsend', 'MPI_Isend', 'MPI_Issend',
'MPI_Keyval_create', 'MPI_Keyval_free', 'MPI_Op_create', 'MPI_Op_free',
'MPI_Pack', 'MPI_Pack_size', 'MPI_Pcontrol', 'MPI_Probe', 'MPI_Recv',
'MPI_Recv_init', 'MPI_Reduce', 'MPI_Reduce_scatter', 'MPI_Request_free',
'MPI_Rsend', 'MPI_Scan', 'MPI_Scatter', 'MPI_Scatterv', 'MPI_Send',
'MPI_Send_init', 'MPI_Sendrecv', 'MPI_Ssend', 'MPI_Startall', 'MPI_Test',
'MPI_Test_cancelled', 'MPI_Testall', 'MPI_Testany', 'MPI_Testsome',
'MPI_Type_commit', 'MPI_Type_contiguous', 'MPI_Type_create_darray',
'MPI_Type_create_hindexed', 'MPI_Type_create_hvector',
'MPI_Type_create_indexed_block', 'MPI_Type_create_resized',
'MPI_Type_create_struct', 'MPI_Type_create_subarray', 'MPI_Type_dup',
'MPI_Type_extent', 'MPI_Type_free', 'MPI_Type_get_contents',
'MPI_Type_get_envelope', 'MPI_Type_get_extent', 'MPI_Type_get_true_extent',
'MPI_Type_hindexed', 'MPI_Type_hvector', 'MPI_Type_indexed', 'MPI_Type_size',
'MPI_Type_struct', 'MPI_Type_vector', 'MPI_Unpack', 'MPI_Wait', 'MPI_Waitall',
'MPI_Waitany', 'MPI_Waitsome', 'MPI_Accumulate', 'MPI_Add_error_class',
'MPI_Add_error_code', 'MPI_Add_error_string', 'MPI_Alloc_mem', 'MPI_Alltoallw',
'MPI_Bsend_init', 'MPI_Cart_get', 'MPI_Cart_map', 'MPI_Cart_rank',
'MPI_Cart_shift', 'MPI_Cart_sub', 'MPI_Cartdim_get', 'MPI_Close_port',
'MPI_Comm_accept', 'MPI_Comm_connect', 'MPI_Comm_create_group',
'MPI_Comm_disconnect', 'MPI_Comm_dup_with_info', 'MPI_Comm_get_info',
'MPI_Comm_get_name', 'MPI_Comm_get_parent', 'MPI_Comm_idup', 'MPI_Comm_join',
'MPI_Comm_remote_group', 'MPI_Comm_set_info', 'MPI_Comm_set_name',
'MPI_Comm_split_type', 'MPI_Comm_test_inter', 'MPI_Compare_and_swap',
'MPI_Dist_graph_create', 'MPI_Dist_graph_create_adjacent',
'MPI_Dist_graph_neighbors', 'MPI_Dist_graph_neighbors_count',
'MPI_Errhandler_create', 'MPI_Errhandler_get', 'MPI_Exscan', 'MPI_Fetch_and_op',
'MPI_File_call_errhandler', 'MPI_File_create_errhandler', 'MPI_File_get_amode',
'MPI_File_get_byte_offset', 'MPI_File_get_errhandler', 'MPI_File_get_group',
'MPI_File_get_position', 'MPI_File_get_position_shared',
'MPI_File_get_type_extent', 'MPI_File_get_view', 'MPI_File_iread',
'MPI_File_iread_at', 'MPI_File_iread_shared', 'MPI_File_iwrite',
'MPI_File_iwrite_at', 'MPI_File_iwrite_shared', 'MPI_File_preallocate',
'MPI_File_read_all_begin', 'MPI_File_read_all_end',
'MPI_File_read_at_all_begin', 'MPI_File_read_at_all_end',
'MPI_File_read_ordered', 'MPI_File_read_ordered_begin',
'MPI_File_read_ordered_end', 'MPI_File_read_shared', 'MPI_File_seek_shared',
'MPI_File_set_errhandler', 'MPI_File_set_info', 'MPI_File_write_all_begin',
'MPI_File_write_all_end', 'MPI_File_write_at_all_begin',
'MPI_File_write_at_all_end', 'MPI_File_write_ordered',
'MPI_File_write_ordered_begin', 'MPI_File_write_ordered_end',
'MPI_File_write_shared', 'MPI_Free_mem', 'MPI_Get', 'MPI_Get_accumulate',
'MPI_Get_library_version', 'MPI_Get_version', 'MPI_Graph_create',
'MPI_Graph_get', 'MPI_Graph_map', 'MPI_Graph_neighbors',
'MPI_Graph_neighbors_count', 'MPI_Graphdims_get', 'MPI_Grequest_complete',
'MPI_Grequest_start', 'MPI_Group_compare', 'MPI_Group_difference',
'MPI_Group_intersection', 'MPI_Group_union', 'MPI_Iallgather',
'MPI_Iallgatherv', 'MPI_Iallreduce', 'MPI_Ialltoall', 'MPI_Ialltoallv',
'MPI_Ialltoallw', 'MPI_Ibarrier', 'MPI_Ibcast', 'MPI_Ibsend', 'MPI_Iexscan',
'MPI_Igather', 'MPI_Igatherv', 'MPI_Ineighbor_allgather',
'MPI_Ineighbor_allgatherv', 'MPI_Ineighbor_alltoall',
'MPI_Ineighbor_alltoallv', 'MPI_Ineighbor_alltoallw',
'MPI_Info_delete', 'MPI_Init_thread', 'MPI_Intercomm_merge',
'MPI_Ireduce', 'MPI_Ireduce_scatter', 'MPI_Ireduce_scatter_block',
'MPI_Is_thread_main', 'MPI_Iscan', 'MPI_Iscatter', 'MPI_Iscatterv',
'MPI_Lookup_name', 'MPI_Neighbor_allgather', 'MPI_Neighbor_allgatherv',
'MPI_Neighbor_alltoall', 'MPI_Neighbor_alltoallv', 'MPI_Neighbor_alltoallw',
'MPI_Op_commutative', 'MPI_Open_port', 'MPI_Pack_external',
'MPI_Pack_external_size', 'MPI_Publish_name', 'MPI_Put', 'MPI_Query_thread',
'MPI_Raccumulate', 'MPI_Reduce_local', 'MPI_Reduce_scatter_block',
'MPI_Register_datarep', 'MPI_Request_get_status', 'MPI_Rget',
'MPI_Rget_accumulate', 'MPI_Rput', 'MPI_Rsend_init', 'MPI_Sendrecv_replace',
'MPI_Ssend_init', 'MPI_Start', 'MPI_Status_set_cancelled',
'MPI_Status_set_elements', 'MPI_Topo_test', 'MPI_Type_create_hindexed_block',
'MPI_Type_create_keyval', 'MPI_Type_delete_attr', 'MPI_Type_free_keyval',
'MPI_Type_get_attr', 'MPI_Type_get_name', 'MPI_Type_lb', 'MPI_Type_match_size',
'MPI_Type_set_attr', 'MPI_Type_set_name', 'MPI_Type_ub', 'MPI_Unpack_external',
'MPI_Unpublish_name', 'MPI_Win_allocate', 'MPI_Win_allocate_shared',
'MPI_Win_attach', 'MPI_Win_call_errhandler', 'MPI_Win_complete',
'MPI_Win_create', 'MPI_Win_create_dynamic', 'MPI_Win_create_errhandler',
'MPI_Win_create_keyval', 'MPI_Win_delete_attr', 'MPI_Win_detach',
'MPI_Win_fence', 'MPI_Win_flush', 'MPI_Win_flush_all', 'MPI_Win_flush_local',
'MPI_Win_flush_local_all', 'MPI_Win_free', 'MPI_Win_free_keyval',
'MPI_Win_get_attr', 'MPI_Win_get_errhandler', 'MPI_Win_get_group',
'MPI_Win_get_info', 'MPI_Win_get_name', 'MPI_Win_lock', 'MPI_Win_lock_all',
'MPI_Win_post', 'MPI_Win_set_attr', 'MPI_Win_set_errhandler',
'MPI_Win_set_info', 'MPI_Win_set_name', 'MPI_Win_shared_query', 'MPI_Win_start',
'MPI_Win_sync', 'MPI_Win_test', 'MPI_Win_unlock', 'MPI_Win_unlock_all',
'MPI_Win_wait'

]

class YogiMPIWrapper(object):

    notLinkingOpts = [ '-c', '-S', '-E', '-M', '-MM' ]
    extensions = { 'C': ['.c'],
                   'C++': ['.c', '.cpp', '.hpp', '.cxx', '.cc', '.i'],
                   'Fortran': [ '.f', '.f90', '.for', '.f77', '.ftn', '.f95',
                                '.f03' ]
                 }

    fixedExts = [ '.f77', '.f', '.for' ]
    f77Comments = [ 'c', 'C', '*' ]
    useMPIRegEx = re.compile(r"([\s]*)use[\s]+mpi([\s]+)", re.IGNORECASE)
    defQuoteEx = re.compile(r'(-D[a-zA-Z0-9_]+)="([a-zA-Z0-9_/]+)"')

    def __init__(self, prefixDir, compilerName, compilerLang):
        global mpiConstants
        global mpiFunctions
        global mpiTimeFunctions
        global mpiObjects
        self.prefixDir = prefixDir
        self.compilerName = compilerName
        self.compilerLang = compilerLang
        self.argArray = list(sys.argv)
        self.cppArgs = []
        self.sourceDir = ''
        self.sourceFile = None
        self.sourceLocation = None
        self.isLinking = True
        self.newFile = None
        self.newPath = None
        self.namesOutput = False
        # Whether or not Yogi does only preprocessing and stops sending
        # the file to the compiler.
        self.preprocessOnly = False
        self.mpi_constants = mpiConstants 
        self.mpi_objects = mpiObjects 
        self.mpi_functions = mpiFunctions
        self.mpi_time_functions = mpiTimeFunctions
        self.const_regexes = []
        self.func_regexes = []
        self.time_regexes = []
        self.rc = 0

        diagOptions = [ '-h', '-help', '-show', '--help', '--version',
                        '-compile_info', '-link_info' ]
        fortranOptions = [ '--yogi-preprocess-only' ]
        
        # Obtain the debug level set by the user.
        self.debug = int(os.environ.get('YMPI_COMPILER_DEBUG', 0))

        # Check if the user wants fixed-length Fortran lines
        if self.compilerLang == 'Fortran' and self.compilerName == 'gfortran':
            self.lineLength = int(os.environ.get('YMPI_OVERRIDE_LINE_LENGTH', 1))
        else:
            self.lineLength = False

        diagMode = False
        if "--no-yogi" in self.argArray:
            self.passThrough()
            return

        for anArg in self.argArray:
            if anArg in diagOptions:
                diagMode = True
            elif self.compilerLang == 'Fortran' and anArg in fortranOptions:
                self.preprocessOnly = True
            if anArg.startswith(('-D', '-I')):
                self.cppArgs.append(anArg)
        if self.lineLength:
            self.argArray.append('-ffixed-line-length-none')
            self.argArray.append('-ffree-line-length-none')
        if diagMode:
            self._showCompilerString()
        else:
            self.createRegExes()
            self.doWrap()

    def getRC(self):
        return self.rc

    def _checkAssignQuotes(self, anArg):
        aRegex = YogiMPIWrapper.defQuoteEx
        retVal = aRegex.sub(r'\g<1>=' + '\\"' + r'\g<2>' + '\\"', anArg)
        return retVal 

    def _isSourceType(self, inputFile, language):
        for anExt in YogiMPIWrapper.extensions[language]:
            if inputFile.lower().endswith(anExt):
                return True
        return False

    def _showCompilerString(self):
        self.compString = self.compilerName 
        self.compString += ' -I' + self.prefixDir + '/include -DYOGIMPI_ENABLED'
        self.compString += ' -L' + self.prefixDir + '/lib -lyogimpi'
        print self.compString

    def _outputMsg(self, message):
        if self.debug:
            print message

    def _buildRegEx(self, aPattern):
        # Functions require an open parenthesis to follow, or
        # if there is a line continuation just trust that someone
        # put one on the next line.
        # ToDo: Actually do lookahead and find out for that last case.
        regexString = r"(^|=|\s|\(|\)|,|\*|\+)(" + aPattern +\
                      r')([\s]*\(|[\s]*&$)'
        return re.compile(regexString, re.IGNORECASE)

    ## Creates the regular expressions needed to search and replace MPI
    #  definitions with Fortran files. This is only needed within Fortran as
    #  C and C++ have mpitoyogi.h transformations.
    def createRegExes(self):
        if self.compilerLang == 'Fortran':
            for aPattern in self.mpi_constants:
                regexString = r"(^|_|=|\s|\(|\)|,|\*|\+)(" + aPattern +\
                              r')(\s|,|\*|\)|\()'
                self.const_regexes.append(re.compile(regexString, re.IGNORECASE))
            for aPattern in self.mpi_functions:
                self.func_regexes.append(self._buildRegEx(aPattern))
            for aPattern in self.mpi_time_functions:
                self.time_regexes.append(self._buildRegEx(aPattern))
        
    def setFile(self, inputFile, argLocation):
        self.sourceDir = os.path.dirname(inputFile)
        self.sourceFile = os.path.basename(inputFile)
        self.sourceLocation = argLocation

    def _getFullSourcePath(self):
        if self.sourceDir:
            return self.sourceDir + '/' + self.sourceFile
        else:
            return self.sourceFile

    def _stripExtension(self, filename):
        for ext in YogiMPIWrapper.extensions[self.compilerLang]:
            if filename.lower().endswith(ext):
                return filename[:-(len(ext))] 
        return filename 

    def _getExtension(self, filename):
        for ext in YogiMPIWrapper.extensions[self.compilerLang]:
            if filename.lower().endswith(ext):
                return filename[-(len(ext)):]
        return filename

    def _changeArgs(self):
        for i, anArg in enumerate(self.argArray):
            if anArg.startswith('-D'):
                self.argArray[i] = self._checkAssignQuotes(anArg)

        self.argArray[0] = self.compilerName 
        if self.sourceFile:
            if self.compilerLang == 'Fortran':
                # Fortran files require preprocssing.
                changedFile = self.preprocessFortran()
                if changedFile:
                    self.argArray[self.sourceLocation] = self.newPath
                if not self.namesOutput and not self.isLinking:
                    objectName = self._stripExtension(self.sourceFile) + '.o'
                    self.argArray.append('-o ' + objectName)
            self.argArray.insert(self.sourceLocation, '-I' + self.prefixDir +\
                                 '/include -DYOGIMPI_ENABLED')
        if self.isLinking:
             self.argArray.append('-L' + self.prefixDir + '/lib -lyogimpi')

    def _getCallString(self):
        callString = ''
        for anArg in self.argArray:
            callString += anArg + ' '
        return callString
        
    def _printCommand(self):
        sys.stdout.write(self._getCallString() + '\n')

    def _isFixedForm(self, sourceFile):
        # Automatically assume a .f or .f77 file is fixed-form.
        if self._getExtension(sourceFile).lower() in YogiMPIWrapper.fixedExts:
            return True
        return False 

    ## Return whether this is a line to ignore (blank or comment). 
    def _isFortranIgnoreLine(self, sourceLine, fixed=False):
	if not sourceLine.strip():
            # Ignore empty lines.
	    return True 
        if sourceLine.lstrip().startswith('!'):
            # Ignore comment lines for free-form. Unfortunately some pseudo
            # fixed-form codes also have lines that start with this, and it
            # is expected they'll also be treated as comments.
            return True 
        elif fixed and sourceLine[0] in YogiMPIWrapper.f77Comments:
            # Ignore comments for fixed-form.
            return True 
        if sourceLine.startswith('#'):
            # Ignore preprocessor directives.
            return True           
        return False

    ## Handle line-continuation in Fortran, which may require breaking a line
    #  into pieces if we have made it too long.
    #  @arg fileArray The Python list containing lines of the files.
    #  @arg lineNumber The number of the line to check.
    def _checkFixedLine(self, fileArray, lineNumber):
        line = fileArray[lineNumber]
        if len(line.rstrip()) <= 72:
            # Nothing to do here.
            return (lineNumber + 1)

        # Strip off comments coming after valid expression.
        commentStart = line.find('!') 
        if commentStart > -1 and line[commentStart - 1] != "\\":
            line = line[:commentStart]
            if len(line.rstrip()) <= 72:
                # If removing the comment fixed length problems, done.
                fileArray[lineNumber] = line.rstrip() + '\n'
                return (lineNumber + 1) 

        # Otherwise split the line into two pieces, the second with
        # a continuation character.
        firstPart = line[:72].strip('\n')
        secondPart = '     &' + line[72:].strip('\n')
        fileArray[lineNumber] = firstPart + '\n'
        if (lineNumber + 1) < len(fileArray):
            fileArray.insert(lineNumber + 1, secondPart + '\n')
        else:
            # This was the last line of the file.  Add one more line.
            fileArray.append(secondPart + '\n')
        # Return the bumped up line count.  In any case it's two lines.
        return (lineNumber + 2)

    ## Calls the system preprocessor (cpp) to yield a fully expanded Fortran
    #  source file.  Sometimes this is needed in tricky situations where 
    #  Fortran source "includes" another source file that has MPI definitions.
    def _callSystemPreprocessor(self):
        cppString = ''
        for i in range(len(self.cppArgs)):
            anArg = self.cppArgs[i]
            if i > 0:
                cppString += ' ' 
            cppString += anArg 
        cppString += " " + self._getFullSourcePath()
        try:   
            fileExtension = self._getExtension(self.sourceFile)
            newFile, newPath = tempfile.mkstemp(prefix='yogiF_',
                                                suffix=fileExtension)
            os.close(newFile)
            cppString += " " + newPath
            subprocess.check_call("cpp -P -traditional-cpp " + cppString,
                                  shell=True)
            if filecmp.cmp(self._getFullSourcePath(), newPath):
                # The files are the same, no need to do anything.
                os.remove(newPath) 
                return False
            else:
                self._outputMsg("File " + self._getFullSourcePath() +\
                                " changed when running system preprocessor," +\
                                " new file is " + newPath) 
                return newPath
        except:
            print "YogiMPI encountered an error preprocessing Fortran source."
            raise

    ## Preprocesses a Fortran file, changing MPI_ to YogiFortran_ wherever
    #  used.  A temporary file is created with the new contents.
    #  Returns True on False as to whether the file was ever changed.
    def preprocessFortran(self, inPlace=False):
        changedFile = False
        self._outputMsg("Preprocessing Fortran source " +\
                        self._getFullSourcePath())
        with open(self._getFullSourcePath(), 'r') as f:
           firstRead = f.read()
           f.seek(0)
           origLines = f.readlines()
           if '#include' in firstRead:
               neededCPP = self._callSystemPreprocessor()
           else:
               neededCPP = False

        if neededCPP:
            startFile = neededCPP
            fileHandle = open(startFile, 'r')
            rawFile = fileHandle.readlines()
            fileHandle.close()
        else:
            rawFile = origLines 
            startFile = self._getFullSourcePath() 

        fixedForm = self._isFixedForm(startFile) 
        if neededCPP:
            os.remove(neededCPP)

        i = 0
        while i < len(rawFile):
            line = rawFile[i]
            if self._isFortranIgnoreLine(line, fixedForm):
                i += 1
                continue
            oldLine = line

            for aRegex in self.const_regexes:
                # Run through all the loaded MPI regular expressions.
                line = aRegex.sub(r"\g<1>Yogi\g<2>\g<3>", line)

            for aRegex in self.func_regexes:
                # Run through all the loaded MPI regular expressions.
                line = aRegex.sub(r"\g<1>YogiFortran_\g<2>\g<3>", line)

            for aRegex in self.time_regexes:
                # Run through all the loaded MPI regular expressions.
                line = aRegex.sub(r"\g<1>YogiBridge_\g<2>\g<3>", line)

            # Substitute "use yogimpi" for "use mpi" where applicable.
            line = YogiMPIWrapper.useMPIRegEx.sub(r"\g<1>use yogimpi\g<2>",
                                                  line)

            # Detect if the line changed, and if so, handle fixed-format 
            # problems and/or bumping the line counter. 
            if oldLine != line:
                rawFile[i] = line
                if fixedForm:
                    # i can be modified if added lines should be skipped.
                    i = self._checkFixedLine(rawFile, i)
                else:
                    i += 1
                changedFile = True
            else:
                i += 1

        if not changedFile:
            self._outputMsg("File " + self._getFullSourcePath() +\
                            " was unchanged.")
        else:
            fileExtension = self._getExtension(self.sourceFile)
            if inPlace: 
                oldFile = self._getFullSourcePath()
                timestr = time.strftime("%Y%m%d-%H%M%S")
                backupPath = oldFile + '.yogi_orig_' + timestr
                os.rename(oldFile, backupPath)
                self._outputMsg("File " + oldFile + " was changed in place," +\
                                " backup is " + backupPath)
                newFile = open(oldFile, 'w')
                newFile.writelines(rawFile)
                newFile.close()
            else:
                self.newFile, self.newPath = tempfile.mkstemp(prefix='yogiF_',
                                                          suffix=fileExtension)
                os.close(self.newFile)
                self.newFile = open(self.newPath, 'w')
                self._outputMsg("File " + self._getFullSourcePath() +\
                                " was changed, new file is " + self.newPath)                                                         
                self.newFile.writelines(rawFile)
                self.newFile.close()

        return changedFile

    ## Passes through doing any YogiMPI flags and acts only as the underlying
    #  serial compiler.       
    def passThrough(self):
        self.argArray[0] = self.compilerName
        self.argArray.remove('--no-yogi')
        self._outputMsg("Final compile string: " + self._getCallString())
        self.rc = subprocess.call(self._getCallString(), shell=True)
 
    def doWrap(self):
        for i in range(len(self.argArray)):
            anOpt = self.argArray[i]
            if anOpt in YogiMPIWrapper.notLinkingOpts:
                # See if this command is linking, which means we have to add 
                # library flags as well as extra include flags.
                self.isLinking = False
            elif self._isSourceType(anOpt, self.compilerLang):
                # Find the source file to preprocess, if any.
                self.setFile(anOpt, i)
            elif anOpt == '-o':
                # If there was an output file name, use what was specified.
                self.namesOutput = True

        if self.preprocessOnly:
            self.preprocessFortran(inPlace=True)
        else:
            self._changeArgs()
            self._outputMsg("Final compile string: " + self._getCallString())
            self.rc = subprocess.call(self._getCallString(), shell=True)
            if self.newPath and (not self.debug):
                os.remove(self.newPath)
