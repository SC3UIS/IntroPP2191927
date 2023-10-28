/* I/O related functions for heat equation solver */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <mpi.h>

#include "heat.h"
#include "pngwriter.h"

/* Output routine that prints out a picture of the temperature
 * distribution. */
void write_field(field *temperature, int iter, parallel_data *parallel)
{
    char filename[64];

    /* The actual write routine takes only the actual data
     * (without ghost layers) so we need array for that. */
    int height, width;
    double *full_data;

    int coords[2];
    int ix, jy;

    int i, p;

    height = temperature->nx_full;
    width = temperature->ny_full;

    if (parallel->rank == 0) {
        /* Copy the inner data */
        full_data = malloc_2d(height, width);
        for (i = 0; i < temperature->nx; i++)
            memcpy(&full_data[idx(i, 0, width)], 
                     &temperature->data[idx(i+1, 1, temperature->ny + 2)],
                   temperature->ny * sizeof(double));
        /* Receive data from other ranks */
        for (p = 1; p < parallel->size; p++) {
            MPI_Cart_coords(parallel->comm, p, 2, coords);
            ix = coords[0] * temperature->nx;
            jy = coords[1] * temperature->ny;
            MPI_Recv(&full_data[idx(ix, jy, width)], 1, 
                     parallel->subarraytype, p, 22, 
                     parallel->comm, MPI_STATUS_IGNORE);
        }
        /* Write out the data to a png file */
        sprintf(filename, "%s_%04d.png", "heat", iter);
        save_png(full_data, height, width, filename, 'c');
        free_2d(full_data);
    } else {
        /* Send data */
        MPI_Ssend(temperature->data, 1, 
                  parallel->subarraytype, 0,
                  22, parallel->comm);
    }
}

/* Read the initial temperature distribution from a file and
 * initialize the temperature fields temperature1 and
 * temperature2 to the same initial state. */
void read_field(field *temperature1, field *temperature2, char *filename,
                parallel_data *parallel)
{
    FILE *fp;
    int nx, ny, i, j;
    double *full_data;

    int coords[2];
    int ix, jy, p;

    int count;

    fp = fopen(filename, "r");
    /* Read the header */
    count = fscanf(fp, "# %d %d \n", &nx, &ny);
    if (count < 2) {
        fprintf(stderr, "Error while reading the input file!\n");
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    parallel_setup(parallel, nx, ny);
    set_field_dimensions(temperature1, nx, ny, parallel);
    set_field_dimensions(temperature2, nx, ny, parallel);


    /* Allocate arrays (including ghost layers) */
    temperature1->data =
        malloc_2d(temperature1->nx + 2, temperature1->ny + 2);
    temperature2->data =
        malloc_2d(temperature2->nx + 2, temperature2->ny + 2);

    if (parallel->rank == 0) {
        /* Full array */
        full_data = malloc_2d(nx, ny);

        /* Read the actual data */
        for (i = 0; i < nx; i++) {
            for (j = 0; j < ny; j++) {
                count = fscanf(fp, "%lf", &full_data[idx(i, j, ny)]);
            }
        }
        /* Copy to own local array */
        for (i = 0; i < temperature1->nx; i++) {
            memcpy(&temperature1->data[idx(i+1, 1, temperature1->ny + 2)],
                   &full_data[idx(i, 0, ny)], temperature1->ny * sizeof(double));
        }
        /* Send to other processes */
        for (p = 1; p < parallel->size; p++) {
            MPI_Cart_coords(parallel->comm, p, 2, coords);
            ix = coords[0] * temperature1->nx;
            jy = coords[1] * temperature1->ny;
            MPI_Send(&full_data[idx(ix, jy, ny)], 1, parallel->subarraytype, 
                     p, 44, parallel->comm);
        }
    } else {
        /* Receive data */
        MPI_Recv(temperature1->data, 1, 
                 parallel->subarraytype, 0,
                 44, parallel->comm, MPI_STATUS_IGNORE);
    }

    /* Set the boundary values */
    for (i = 0; i < temperature1->nx + 1; i++) {
        temperature1->data[idx(i, 0, temperature1->ny + 2)] = 
            temperature1->data[idx(i, 1, temperature1->ny + 2)];
        temperature1->data[idx(i, temperature1->ny + 1, temperature1->ny + 2)] =
            temperature1->data[idx(i, temperature1->ny, temperature1->ny + 2)];
    }
    for (j = 0; j < temperature1->ny + 2; j++) {
        temperature1->data[idx(0, j, temperature1->ny + 2)] = 
                        temperature1->data[idx(1, j, temperature1->ny + 2)];
        temperature1->data[idx(temperature1->nx + 1, j, temperature1->ny + 2)] =
            temperature1->data[idx(temperature1->nx, j, temperature1->ny + 2)];
    }

    copy_field(temperature1, temperature2);

    if (parallel->rank == 0) {
        free_2d(full_data);
    }

    fclose(fp);
}

/* Write a restart checkpoint that contains field dimensions, current
 * iteration number and temperature field. */
void write_restart(field *temperature, parallel_data *parallel, int iter)
{
    MPI_File fp;
    MPI_Offset disp;

    // open the file and write the dimensions
    MPI_File_open(MPI_COMM_WORLD, CHECKPOINT,
                  MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fp);
    if (parallel->rank == 0) {
        MPI_File_write(fp, &temperature->nx_full, 1, MPI_INT,
                       MPI_STATUS_IGNORE);
        MPI_File_write(fp, &temperature->ny_full, 1, MPI_INT,
                       MPI_STATUS_IGNORE);
        MPI_File_write(fp, &iter, 1, MPI_INT, MPI_STATUS_IGNORE);
    }

    disp = 3 * sizeof(int);
    MPI_File_set_view(fp, 0, MPI_DOUBLE, parallel->filetype, "native", 
                      MPI_INFO_NULL);
    MPI_File_write_at_all(fp, disp, temperature->data,
                          1, parallel->restarttype, MPI_STATUS_IGNORE);
    MPI_File_close(&fp);
}

/* Read a restart checkpoint that contains field dimensions, current
 * iteration number and temperature field. */
void read_restart(field *temperature, parallel_data *parallel, int *iter)
{
    MPI_File fp;
    MPI_Offset disp;

    int nx, ny;

    // open the file and write the dimensions
    MPI_File_open(MPI_COMM_WORLD, CHECKPOINT, MPI_MODE_RDONLY,
                  MPI_INFO_NULL, &fp);

    // read grid size and current iteration
    MPI_File_read_all(fp, &nx, 1, MPI_INT, MPI_STATUS_IGNORE);
    MPI_File_read_all(fp, &ny, 1, MPI_INT, MPI_STATUS_IGNORE);
    MPI_File_read_all(fp, iter, 1, MPI_INT, MPI_STATUS_IGNORE);
    // set correct dimensions to MPI metadata
    parallel_setup(parallel, nx, ny);
    // set local dimensions and allocate memory for the data
    set_field_dimensions(temperature, nx, ny, parallel);
    allocate_field(temperature);


    disp = 3 * sizeof(int);
    MPI_File_set_view(fp, 0, MPI_DOUBLE, parallel->filetype, "native", 
                      MPI_INFO_NULL);
    MPI_File_read_at_all(fp, disp, temperature->data,
                          1, parallel->restarttype, MPI_STATUS_IGNORE);
    MPI_File_close(&fp);
}
