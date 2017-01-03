int YogiMPI_Get_count(YogiMPI_Status *status, YogiMPI_Datatype datatype, 
		              int *count) {
    MPI_Datatype mpi_datatype = datatype_to_mpi(datatype);
    MPI_Status *mpi_status = yogi_status_to_mpi(status);
    int mpi_err = MPI_Get_count(mpi_status, mpi_datatype, count);
    return error_to_yogi(mpi_err);
}

int YogiMPI_Wait(YogiMPI_Request* request, YogiMPI_Status *status) {
	
    MPI_Request* mpi_request = request_to_mpi(*request);
    int mpi_err;
    if (YogiMPI_STATUS_IGNORE != status) {
        MPI_Status *mpi_status = yogi_status_to_mpi(status);
        mpi_err = MPI_Wait(mpi_request, mpi_status);
        mpi_status_to_yogi(mpi_status, status);
    }
    else {
        mpi_err = MPI_Wait(mpi_request, MPI_STATUS_IGNORE);
    }
    if (MPI_REQUEST_NULL == *mpi_request) { 
        assert(MPI_REQUEST_NULL == request_pool[*request]);
        *request = YogiMPI_REQUEST_NULL;
    }

    return error_to_yogi(mpi_err);
}

int YogiMPI_Waitall(int count, YogiMPI_Request *array_of_requests, 
		            YogiMPI_Status *array_of_statuses) {
    int i;
    MPI_Request mpi_requests[count];
    for(i = 0; i < count; ++i) {
    	mpi_requests[i] = * request_to_mpi(array_of_requests[i]);
    }
    int mpi_err;

    if (YogiMPI_STATUSES_IGNORE != array_of_statuses) {
        MPI_Status mpi_statuses[count];
        for (i = 0; i < count; i++) {
            mpi_statuses[i] = *(yogi_status_to_mpi(&array_of_statuses[i]));
        }
        mpi_err = MPI_Waitall(count, mpi_requests, mpi_statuses);
        for(i = 0; i < count; i++) {
            mpi_status_to_yogi(&mpi_statuses[i], &array_of_statuses[i]);
        }
    }
    else {
        mpi_err = MPI_Waitall(count, mpi_requests, MPI_STATUSES_IGNORE);
    }
 
    /* reset requests */
    for(i = 0; i < count; ++i) {
        if(mpi_requests[i] == MPI_REQUEST_NULL) {
            *request_to_mpi(array_of_requests[i]) = MPI_REQUEST_NULL; 
            array_of_requests[i] = YogiMPI_REQUEST_NULL;
        }
    }

    return error_to_yogi(mpi_err);
}

int YogiMPI_Waitsome(int incount, YogiMPI_Request *array_of_requests, 
		             int *outcount, int *array_of_indices, 
					 YogiMPI_Status *array_of_statuses) {
    int i;
    int mpi_err;
    MPI_Request* mpi_requests = (MPI_Request*)malloc(incount*sizeof(MPI_Request));
    for(i = 0; i < incount; ++i) {
        mpi_requests[i] = * request_to_mpi(array_of_requests[i]);
    }
    
    if (YogiMPI_STATUSES_IGNORE != array_of_statuses ) {
        MPI_Status* mpi_statuses = (MPI_Status*)malloc(incount*sizeof(MPI_Status));
        for (i = 0; i < incount; i++) {
            mpi_statuses[i] = *(yogi_status_to_mpi(&array_of_statuses[i]));
        }
	    mpi_err = MPI_Waitsome(incount, mpi_requests, outcount, 
	                           array_of_indices, mpi_statuses);
        for(i = 0; i < *outcount; i++) {
            mpi_status_to_yogi(&mpi_statuses[i], &array_of_statuses[i]);
        }
        free(mpi_statuses);
    }
    else {
        mpi_err = MPI_Waitsome(incount, mpi_requests, outcount, 
                               array_of_indices, MPI_STATUSES_IGNORE);
    }
    
    /* reset requests */
    if (*outcount == MPI_UNDEFINED) {
        *outcount = YogiMPI_UNDEFINED;
    }
    else {
        for(i = 0; i < *outcount; ++i) {
            int requestIndex = array_of_indices[i];
            if(mpi_requests[requestIndex] == MPI_REQUEST_NULL) {
                *request_to_mpi(array_of_requests[requestIndex]) = MPI_REQUEST_NULL; 
                array_of_requests[requestIndex] = YogiMPI_REQUEST_NULL;
            }
        }
    }
    free(mpi_requests);
    return error_to_yogi(mpi_err);
}

int YogiMPI_Waitany(int count, YogiMPI_Request *array_of_requests, int *index, 
		            YogiMPI_Status *status) {
    int i;
    int mpi_err;
    MPI_Request* mpi_requests = (MPI_Request*)malloc(count*sizeof(MPI_Request));
    for(i = 0; i < count; ++i) {
        mpi_requests[i] = * request_to_mpi(array_of_requests[i]);
    }
    
    if (YogiMPI_STATUS_IGNORE != status ) {
        MPI_Status *mpi_status = yogi_status_to_mpi(status);
        mpi_err = MPI_Waitany(count, mpi_requests, index, mpi_status);
        mpi_status_to_yogi(mpi_status, status);
    }
    else {
        mpi_err = MPI_Waitany(count, mpi_requests, index, MPI_STATUS_IGNORE);
    }
    
    /* reset requests */
    if (*index == MPI_UNDEFINED) {
        *index = YogiMPI_UNDEFINED;
    }
    else {
        if(mpi_requests[*index] == MPI_REQUEST_NULL) {
            *request_to_mpi(array_of_requests[*index]) = MPI_REQUEST_NULL; 
            array_of_requests[*index] = YogiMPI_REQUEST_NULL;
        }
    }
    free(mpi_requests);
    return error_to_yogi(mpi_err);
}

int YogiMPI_Group_translate_ranks(YogiMPI_Group group1, int n, int *ranks1, 
		                          YogiMPI_Group group2, int *ranks2) {
    MPI_Group mpi_group1 = group_to_mpi(group1);
    MPI_Group mpi_group2 = group_to_mpi(group2);
    int mpi_err = MPI_Group_translate_ranks(mpi_group1, n, ranks1, mpi_group2,
    		                                ranks2);

    /* convert MPI_UNDEFINED ranks into YogiMPI_UNDEFINED */
    int i = 0;
    for(i = 0; i < n ; ++i) {
        if (MPI_UNDEFINED == ranks2[i]) ranks2[i] = YogiMPI_UNDEFINED;
	}

	return error_to_yogi(mpi_err);
}

int YogiMPI_Get_processor_name(char *name, int *resultlen) {
    int mpi_err;
    if (MPI_MAX_PROCESSOR_NAME > YogiMPI_MAX_PROCESSOR_NAME) {
        char *tmp_name = (char*)malloc(MPI_MAX_PROCESSOR_NAME);
        mpi_err = MPI_Get_processor_name(tmp_name,resultlen);
        memcpy(name,tmp_name,YogiMPI_MAX_PROCESSOR_NAME-1);
        name[YogiMPI_MAX_PROCESSOR_NAME-1] = '\0';
        free(tmp_name);
        if (*resultlen > YogiMPI_MAX_PROCESSOR_NAME-1) {
            *resultlen = YogiMPI_MAX_PROCESSOR_NAME-1;
        }
    }
    else {
        mpi_err = MPI_Get_processor_name(name, resultlen);
    }
    
    return error_to_yogi(mpi_err);
}

int YogiMPI_File_close(YogiMPI_File *fh) {
    MPI_File mpi_file = file_to_mpi(*fh); 
    int mpi_error = MPI_File_close(&mpi_file);
    file_pool[*fh] = MPI_FILE_NULL;
    *fh = YogiMPI_FILE_NULL;
    return error_to_yogi(mpi_error);
}

int YogiMPI_Test(YogiMPI_Request *request, int *flag, YogiMPI_Status *status) {
    int mpi_error;
    MPI_Request* mpi_request = request_to_mpi(*request);
    if (YogiMPI_STATUS_IGNORE != status) {
        MPI_Status *mpi_status = yogi_status_to_mpi(status);
        mpi_error = MPI_Test(mpi_request, flag, mpi_status);
        mpi_status_to_yogi(mpi_status, status);
    }
    else {
        mpi_error = MPI_Test(mpi_request, flag, MPI_STATUS_IGNORE);
    }

    if (MPI_REQUEST_NULL == *mpi_request) {
        assert(MPI_REQUEST_NULL == request_pool[*request]);
        *request = YogiMPI_REQUEST_NULL;
    }

    return error_to_yogi(mpi_error); 
}

int YogiMPI_Attr_get(YogiMPI_Comm comm, int keyval, void *attribute_val, 
                     int *flag) {
    if (keyval == YogiMPI_TAG_UB) {
        keyval = MPI_TAG_UB;
    }
    MPI_Comm mpi_comm = comm_to_mpi(comm);
    int mpi_error = MPI_Attr_get(mpi_comm, keyval, attribute_val, flag);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Get_elements(YogiMPI_Status* status, YogiMPI_Datatype datatype, int* elements) {

    MPI_Datatype conv_datatype = datatype_to_mpi(datatype);
    int mpi_error;
    MPI_Status * in_status = yogi_status_to_mpi(status);
    mpi_error = MPI_Get_elements(in_status, conv_datatype, elements);
    if (*elements == MPI_UNDEFINED) {
        *elements = YogiMPI_UNDEFINED;
    }
    return error_to_yogi(mpi_error);
}

int YogiMPI_Error_string(int errorcode, char* string, int* resultlen) {

    int mpi_error;
    mpi_error = MPI_Error_string(errorcode, string, resultlen);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Group_range_excl(YogiMPI_Group group, int n, int ranges[][3], 
		                     YogiMPI_Group* newgroup) {

    MPI_Group conv_group = group_to_mpi(group);
    MPI_Group conv_newgroup;
    int mpi_error;
    mpi_error = MPI_Group_range_excl(conv_group, n, ranges,
    		                         &conv_newgroup);
    *newgroup = add_new_group(conv_newgroup);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Error_class(int errorcode, int* errorclass) {

    *errorclass = errorcode;
    return error_to_yogi(MPI_SUCCESS);
}

int YogiMPI_Comm_get_errhandler(YogiMPI_Comm comm,
		                        YogiMPI_Errhandler* errhandler) {

    MPI_Comm conv_comm = comm_to_mpi(comm);
    MPI_Errhandler conv_errhandler;
    int mpi_error;
    mpi_error = MPI_Comm_get_errhandler(conv_comm, &conv_errhandler);
    
	if (conv_errhandler == MPI_ERRHANDLER_NULL) {
		*errhandler = YogiMPI_ERRHANDLER_NULL;
	}
	else if (conv_errhandler == MPI_ERRORS_ARE_FATAL) {
		*errhandler = YogiMPI_ERRORS_ARE_FATAL;
	}
	else if (conv_errhandler == MPI_ERRORS_RETURN) {
		*errhandler = YogiMPI_ERRORS_RETURN;
	}
	else {
	    *errhandler = add_new_errhandler(conv_errhandler);		
	}
    return error_to_yogi(mpi_error);
}

int YogiMPI_Type_get_envelope(YogiMPI_Datatype datatype, int* num_integers,
		                      int* num_addresses, int* num_datatypes,
							  int* combiner) {

    MPI_Datatype conv_datatype = datatype_to_mpi(datatype);
    int mpi_error;
    mpi_error = MPI_Type_get_envelope(conv_datatype, num_integers, 
    		                          num_addresses, num_datatypes, combiner);
    
    switch (*combiner) {
        case MPI_COMBINER_NAMED:
        	*combiner = YogiMPI_COMBINER_NAMED;
        	break;
        case MPI_COMBINER_DUP:
    	    *combiner = YogiMPI_COMBINER_DUP;        
            break;
        case MPI_COMBINER_CONTIGUOUS:
    	    *combiner = YogiMPI_COMBINER_CONTIGUOUS;        
        	break;
        case MPI_COMBINER_VECTOR:
    	    *combiner = YogiMPI_COMBINER_VECTOR;
        	break;
        case MPI_COMBINER_HVECTOR_INTEGER:
    	    *combiner = YogiMPI_COMBINER_HVECTOR_INTEGER;
        	break;
        case MPI_COMBINER_HVECTOR:
    	    *combiner = YogiMPI_COMBINER_HVECTOR;
        	break;
        case MPI_COMBINER_INDEXED:
        	*combiner = YogiMPI_COMBINER_INDEXED;
        	break;
        case MPI_COMBINER_HINDEXED_INTEGER:
        	*combiner = YogiMPI_COMBINER_HINDEXED_INTEGER;
        	break;
        case MPI_COMBINER_HINDEXED:
        	*combiner = YogiMPI_COMBINER_HINDEXED;
        	break;
        case MPI_COMBINER_INDEXED_BLOCK:
        	*combiner = YogiMPI_COMBINER_INDEXED_BLOCK;
        	break;
        case MPI_COMBINER_STRUCT_INTEGER:
        	*combiner = YogiMPI_COMBINER_STRUCT_INTEGER;
        	break;
        case MPI_COMBINER_STRUCT:
		    *combiner = YogiMPI_COMBINER_STRUCT;
         	break;
        case MPI_COMBINER_SUBARRAY:
        	*combiner - YogiMPI_COMBINER_SUBARRAY;
        	break;
        case MPI_COMBINER_DARRAY:
        	*combiner = YogiMPI_COMBINER_DARRAY;
        	break;
        case MPI_COMBINER_F90_REAL:
        	*combiner = YogiMPI_COMBINER_F90_REAL;
        	break;
        case MPI_COMBINER_F90_COMPLEX:
        	*combiner = YogiMPI_COMBINER_F90_COMPLEX;
        	break;
        case MPI_COMBINER_F90_INTEGER:
        	*combiner = YogiMPI_COMBINER_F90_INTEGER;
        	break;
        case MPI_COMBINER_RESIZED:
        	*combiner = YogiMPI_COMBINER_RESIZED;
        	break;
    }
    return error_to_yogi(mpi_error);
}

int YogiMPI_Type_get_contents(YogiMPI_Datatype datatype, int max_integers,
		                      int max_addresses, int max_datatypes,
							  int array_of_integers[],
							  YogiMPI_Aint array_of_addresses[],
							  YogiMPI_Datatype array_of_datatypes[]) {

    MPI_Datatype conv_datatype = datatype_to_mpi(datatype);

    /* In order to appropriately handle this function, we must call 
     * MPI_Type_get_envelope to figure out what the user will get. This helps
     * determine if the type is derived and if new datatype handles are added
     * to our translation pool.
     */
    int num_integers, num_addresses, num_datatypes, combiner;
    MPI_Type_get_envelope(conv_datatype, &num_integers, &num_addresses, 
    		              &num_datatypes, &combiner);
    /* This should not ever be a named, predefined data type. Bad call. */
    assert(combiner != MPI_COMBINER_NAMED);
    
    MPI_Aint * conv_array_of_addresses = 
    		           (MPI_Aint*)malloc(max_addresses*sizeof(MPI_Aint));
    MPI_Datatype * conv_array_of_datatypes =
    		          (MPI_Datatype*)malloc(max_datatypes*sizeof(MPI_Datatype));
    int mpi_error;
    mpi_error = MPI_Type_get_contents(conv_datatype, max_integers,
    		                          max_addresses, max_datatypes,
									  array_of_integers,
									  conv_array_of_addresses,
									  conv_array_of_datatypes);
    aint_array_to_yogi(conv_array_of_addresses, array_of_addresses,
    		           max_addresses);
    if (combiner != MPI_COMBINER_F90_COMPLEX &&
        combiner != MPI_COMBINER_F90_REAL && 
		combiner != MPI_COMBINER_F90_INTEGER) {
    	/* The array won't be empty, so convert them to Yogi versions. */
        datatype_array_to_yogi(conv_array_of_datatypes, array_of_datatypes,
    		                   max_datatypes);
    }
    free_aint_array(conv_array_of_addresses);
    free_datatype_array(conv_array_of_datatypes);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Type_create_darray(int size, int rank, int ndims,
		                       int array_of_gsizes[], int array_of_distribs[],
							   int array_of_dargs[], int array_of_psizes[],
							   int order, YogiMPI_Datatype oldtype,
							   YogiMPI_Datatype* newtype) {

    MPI_Datatype conv_oldtype = datatype_to_mpi(oldtype);
    MPI_Datatype conv_newtype;
    /* Constant conversion requires some temporary arrays to hold state
     * and distribution values.
     */
    int distribs_conv[ndims];
    int dargs_conv[ndims];
    int i;
    for (i = 0; i < ndims; i++) {
    	switch(array_of_distribs[i]) {
    	    case YogiMPI_DISTRIBUTE_BLOCK:
    	    	distribs_conv[i] = MPI_DISTRIBUTE_BLOCK;
                break;
    	    case YogiMPI_DISTRIBUTE_CYCLIC:
    	    	distribs_conv[i] = MPI_DISTRIBUTE_CYCLIC;
    		    break;
    	    case YogiMPI_DISTRIBUTE_NONE:
    	    	distribs_conv[i] = MPI_DISTRIBUTE_NONE;
    		    break;
    	    default:
    		    distribs_conv[i] = array_of_distribs[i];
    	}
    	if (array_of_dargs[i] == YogiMPI_DISTRIBUTE_DFLT_DARG) {
    	    dargs_conv[i] = MPI_DISTRIBUTE_DFLT_DARG;
    	}
    	else {
    		dargs_conv[i] = array_of_dargs[i];
    	}
    }
    int mpi_error;
    mpi_error = MPI_Type_create_darray(size, rank, ndims, array_of_gsizes,
    		                           distribs_conv, dargs_conv,
									   array_of_psizes, order, conv_oldtype,
									   &conv_newtype);
    *newtype = add_new_datatype(conv_newtype);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Keyval_create(YogiMPI_Copy_function* copy_fn,
		                  YogiMPI_Delete_function* delete_fn, int* keyval,
						  void* extra_state) {

	MPI_Copy_function* mpi_copy_fn;
	MPI_Delete_function* mpi_delete_fn;
    if (copy_fn == YogiMPI_NULL_COPY_FN) {
        mpi_copy_fn = MPI_NULL_COPY_FN;
    }
    else if (copy_fn == YogiMPI_DUP_FN) {
        mpi_copy_fn = MPI_DUP_FN;
    }
    else {
    	mpi_copy_fn = (MPI_Copy_function*)copy_fn;
    }
    if (delete_fn == YogiMPI_NULL_DELETE_FN) {
        mpi_delete_fn = MPI_NULL_DELETE_FN;
    }
    else {
    	mpi_delete_fn = (MPI_Delete_function*)delete_fn;
    }
    int mpi_error;
    mpi_error = MPI_Keyval_create(mpi_copy_fn, mpi_delete_fn, keyval,
    		                      extra_state);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Comm_create_errhandler(YogiMPI_Comm_errhandler_function* function,
		                           YogiMPI_Errhandler* errhandler) {

    MPI_Errhandler conv_errhandler;
    MPI_Comm_errhandler_function * mpi_function = 
    		                            (MPI_Comm_errhandler_function*)function;
    int mpi_error;
    mpi_error = MPI_Comm_create_errhandler(mpi_function, &conv_errhandler);
    *errhandler = add_new_errhandler(conv_errhandler);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Op_create(YogiMPI_User_function* function, int commute,
		              YogiMPI_Op* op) {

    MPI_Op conv_op;
    MPI_User_function * mpi_function = (MPI_User_function*)function;
    int mpi_error;
    mpi_error = MPI_Op_create(mpi_function, commute, &conv_op);
    *op = add_new_op(conv_op);
    return error_to_yogi(mpi_error);
}

int YogiMPI_Comm_call_errhandler(YogiMPI_Comm comm, int errorcode) {
	MPI_Comm comm_conv = comm_to_mpi(comm);
	int mpi_error = MPI_Comm_call_errhandler(comm_conv, yogi_error_to_mpi(errorcode));
	return error_to_yogi(mpi_error);
}

int YogiMPI_Testall(int count, YogiMPI_Request array_of_requests[],
                    int *flag, YogiMPI_Status array_of_statuses[]) {

    int i, mpi_error;
    MPI_Request mpi_requests[count];
    for(i = 0; i < count; ++i) {
        mpi_requests[i] = *request_to_mpi(array_of_requests[i]);
    }    
    if (YogiMPI_STATUSES_IGNORE != array_of_statuses) {
        MPI_Status mpi_statuses[count];
        for (i = 0; i < count; i++) {
            mpi_statuses[i] = *(yogi_status_to_mpi(&array_of_statuses[i]));
        }
        mpi_error = MPI_Testall(count, mpi_requests, flag, mpi_statuses);
        for(i = 0; i < count; i++) {
            mpi_status_to_yogi(&mpi_statuses[i], &array_of_statuses[i]);
        }
    }
    else {
        mpi_error = MPI_Testall(count, mpi_requests, flag, MPI_STATUSES_IGNORE);
    }
    /* reset requests */
    for(i = 0; i < count; ++i) {
        if(mpi_requests[i] == MPI_REQUEST_NULL) {
            *request_to_mpi(array_of_requests[i]) = MPI_REQUEST_NULL;
            array_of_requests[i] = YogiMPI_REQUEST_NULL;
        }
    }

    return error_to_yogi(mpi_error);
}

int YogiMPI_Test_cancelled(YogiMPI_Status *status, int *flag) {
    MPI_Status *mpi_status = yogi_status_to_mpi(status);
    int mpi_err = MPI_Test_cancelled(mpi_status, flag);
    return error_to_yogi(mpi_err);
}

int YogiMPI_Testany(int count, YogiMPI_Request *array_of_requests,
                    int *index, int *flag, YogiMPI_Status *status) {

    int i;
    int mpi_err;
    MPI_Request* mpi_requests = (MPI_Request*)malloc(count*sizeof(MPI_Request));
    for(i = 0; i < count; ++i) {
        mpi_requests[i] = * request_to_mpi(array_of_requests[i]);
    }

    if (YogiMPI_STATUS_IGNORE != status ) {
        MPI_Status *mpi_status = yogi_status_to_mpi(status);
        mpi_err = MPI_Testany(count, mpi_requests, index, flag, mpi_status);
        mpi_status_to_yogi(mpi_status, status);
    }
    else {
        mpi_err = MPI_Testany(count, mpi_requests, index, flag,
                              MPI_STATUS_IGNORE);
    }

    /* reset requests */
    if (*index == MPI_UNDEFINED) {
        *index = YogiMPI_UNDEFINED;
    }
    else {
        if(mpi_requests[*index] == MPI_REQUEST_NULL) {
            *request_to_mpi(array_of_requests[*index]) = MPI_REQUEST_NULL;
            array_of_requests[*index] = YogiMPI_REQUEST_NULL;
        }
    }
    free(mpi_requests);
    return error_to_yogi(mpi_err);

}

int YogiMPI_Testsome(int incount, YogiMPI_Request *array_of_requests,
                     int *outcount, int *array_of_indices, 
                     YogiMPI_Status *array_of_statuses) {

    int i;
    int mpi_err;
    MPI_Request* mpi_requests = (MPI_Request*)malloc(incount*sizeof(MPI_Request));
    for(i = 0; i < incount; ++i) {
        mpi_requests[i] = * request_to_mpi(array_of_requests[i]);
    }

    if (YogiMPI_STATUSES_IGNORE != array_of_statuses ) {
        MPI_Status* mpi_statuses = (MPI_Status*)malloc(incount*sizeof(MPI_Status));
        for (i = 0; i < incount; i++) {
            mpi_statuses[i] = *(yogi_status_to_mpi(&array_of_statuses[i]));
        }
            mpi_err = MPI_Testsome(incount, mpi_requests, outcount,
                                   array_of_indices, mpi_statuses);
        for(i = 0; i < *outcount; i++) {
            mpi_status_to_yogi(&mpi_statuses[i], &array_of_statuses[i]);
        }
        free(mpi_statuses);
    }
    else {
        mpi_err = MPI_Testsome(incount, mpi_requests, outcount,
                               array_of_indices, MPI_STATUSES_IGNORE);
    }

    /* reset requests */
    if (*outcount == MPI_UNDEFINED) {
        *outcount = YogiMPI_UNDEFINED;
    }
    else {
        for(i = 0; i < *outcount; ++i) {
            int requestIndex = array_of_indices[i];
            if(mpi_requests[requestIndex] == MPI_REQUEST_NULL) {
                *request_to_mpi(array_of_requests[requestIndex]) = MPI_REQUEST_NULL;
                array_of_requests[requestIndex] = YogiMPI_REQUEST_NULL;
            }
        }
    }
    free(mpi_requests);
    return error_to_yogi(mpi_err);

}

int YogiMPI_Comm_create_keyval(YogiMPI_Comm_copy_attr_function
                               *comm_copy_attr_fn,
                               YogiMPI_Comm_delete_attr_function
                               *comm_delete_attr_fn, int *comm_keyval,
                               void *extra_state) {
    MPI_Comm_copy_attr_function * copy_function;
    MPI_Comm_delete_attr_function * delete_function;
    if (comm_copy_attr_fn == YogiMPI_COMM_NULL_COPY_FN) {
        copy_function = MPI_COMM_NULL_COPY_FN;
    }
    else if (comm_copy_attr_fn == YogiMPI_COMM_DUP_FN) {
        copy_function = MPI_COMM_DUP_FN;
    }
    else {
        copy_function = (MPI_Comm_copy_attr_function*)comm_copy_attr_fn;
    }
    if (comm_delete_attr_fn == YogiMPI_COMM_NULL_DELETE_FN) {
        delete_function = MPI_COMM_NULL_DELETE_FN;
    }
    else {
        delete_function = (MPI_Comm_delete_attr_function*)comm_delete_attr_fn;
    }
    int mpi_err = MPI_Comm_create_keyval(copy_function, delete_function,
                                         comm_keyval, extra_state);
    return error_to_yogi(mpi_err);
}
