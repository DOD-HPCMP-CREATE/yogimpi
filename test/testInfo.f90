program testInfo
    include 'mpif.h'

    integer :: errs, info1, infodup, ierr
    integer :: nkeys, nkeysdup, i, vallen
    logical :: flag, flagdup
    character(len=MPI_MAX_INFO_KEY) :: key, keydup
    character(len=MPI_MAX_INFO_VAL) :: value, valdup

    call MPI_Init(ierr)

    call MPI_Info_create(info1, ierr)
    call MPI_Info_set(info1, 'host', 'myhost.myorg.org', ierr)
    call MPI_Info_set(info1, 'file', 'runfile.txt', ierr)
    call MPI_Info_set(info1, 'soft', '2:1000:4,3:1000:7', ierr)

    call MPI_Info_dup(info1, infodup, ierr)

    call MPI_Info_get_nkeys(infodup, nkeysdup, ierr)
    call MPI_Info_get_nkeys(info1, nkeys, ierr)
    if (nkeys /= nkeysdup) then
        errs = errs + 1
        print *, 'Duped info has a different number of keys; is ', nkeysdup, ' should be ', nkeys
    end if
    vallen = MPI_MAX_INFO_VAL
    do i=0,nkeys-1
        call MPI_Info_get_nthkey(info1, i, key, ierr)
        call MPI_Info_get_nthkey(infodup, i, keydup, ierr)
        if (key /= keydup) then
            errs = errs + 1
            print *, 'Keys do not match: ', keydup, ' should be ', key
        end if

        vallen = MPI_MAX_INFO_VAL
        call MPI_Info_get(info1, key, vallen, value, flag, ierr)
        call MPI_Info_get(infodup, keydup, vallen, valdup, flagdup, ierr)
        if (.not. flag .or. .not. flagdup) then
            errs = errs + 1
            print *, 'Info get failed for key ', key
        else if (value /= valdup) then
            errs = errs + 1
            print *, 'Info values for key ', key, ' not the same after dup'
        end if
    end do

    call MPI_Info_set(info1, 'path', '/a:/b:/c/d', ierr)
    call MPI_Info_get(infodup, 'path', vallen, value, flag, ierr)

    if (flag) then
        errs = errs + 1
        print *, "Inserting path into info changed infodup"
    end if

    call MPI_Info_free(info1, ierr)
    call MPI_Info_free(infodup, ierr)

    call MPI_Finalize(ierr)
    call exit(errs)
end program
