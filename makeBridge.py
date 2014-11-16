file1 = open('mpitoyogi.h', 'r')
file2 = open('mpitoyogi.h.new', 'w')
for line in file1:
    mpiCall = line.strip()
    if not(mpiCall.startswith('#define') or
           mpiCall.startswith('#ifndef')):
        if 'MPI_' in mpiCall:
            origLine = mpiCall.replace('Yogi', '')
            file2.write('#define ' + origLine + " " + mpiCall + "\n")
        else:
            file2.write(line)
    else:
        file2.write(line)
        
file1.close()
file2.close()
