/*
YogiMPI Library - Conversion Utility Header 
Copyright (C) 2006, 2007 Toon Knapen Free Field Technologies S.A.
Additions made by Stephen Adamec, University of Alabama at Birmingham

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "mpi.h"

#ifndef YOGIMPI_CONVUTIL_H 
#define YOGIMPI_CONVUTIL_H
#ifdef __cplusplus
extern "C" {
#endif

YogiMPI_Comm fortran_comm_to_yogi(int fortran_comm);

YogiMPI_Group fortran_group_to_yogi(int fortran_group);

#ifdef __cplusplus
}
#endif
#endif /* YOGIMPI_CONVUTIL_H */
