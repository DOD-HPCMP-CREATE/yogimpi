// Author: Wes Kendall
// Copyright 2011 www.mpitutorial.com
// This code is provided freely with the tutorials on mpitutorial.com. Feel
// free to modify it for your own use. Any distribution of the code must
// either provide a link to www.mpitutorial.com or keep this header in tact.
//
// Example of using MPI_Probe to dynamically allocated received messages
//
#include "mpi.h" 
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char** argv) {
  MPI_Init(NULL, NULL);

  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  if (world_size != 2) {
    fprintf(stderr, "Must use two processes for this example\n");
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  const int number_amount = 100;
  int numbers[number_amount];
  int i;
  for (i = 0; i < number_amount; i++) {
      numbers[i] = (i * 11) + 42; 
  }

  if (world_rank == 0) {
    // Send the amount of integers to process one
    MPI_Send(numbers, number_amount, MPI_INT, 1, 0, MPI_COMM_WORLD);
  } else if (world_rank == 1) {
    int number_to_receive;
    MPI_Status status;
    MPI_Message message;
    int flag = 0;
    // Probe for an incoming message from process zero
    while (!flag) {
        MPI_Improbe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &message, &status);
    }
    // When probe returns, the status object has the size and other
    // attributes of the incoming message. Get the size of the message.
    MPI_Get_count(&status, MPI_INT, &number_to_receive);
    assert(number_to_receive == number_amount);
    // Allocate a buffer just big enough to hold the incoming numbers
    int* number_buf = (int*)malloc(sizeof(int) * number_to_receive);
    // Now receive the message with the allocated buffer
    MPI_Mrecv(number_buf, number_amount, MPI_INT, &message, MPI_STATUS_IGNORE);
    for (i = 0; i < number_to_receive; i++) {
        assert(number_buf[i] == numbers[i]);
    }
    free(number_buf);
  }
  MPI_Finalize();
}

