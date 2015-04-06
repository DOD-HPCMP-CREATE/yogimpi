%module pyogimpi 

%{
#define SWIG_FILE_WITH_INIT
%}

%inline %{
void startYogiMPI(void) {
    extern void allocate_yogimpi_storage(void);
    allocate_yogimpi_storage();
}

void stopYogiMPI(void) {
    extern void deallocate_yogimpi_storage(void);
    deallocate_yogimpi_storage();
}
%}
