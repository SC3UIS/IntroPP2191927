#ifndef __HEAT_H__
#define __HEAT_H__


/* Datatype for temperature field */
typedef struct {
    /* nx and ny are the true dimensions of the field. The array data
     * contains also ghost layers, so it will have dimensions nx+2 x ny+2 */
    int nx;                     /* Local dimensions of the field */
    int ny;
    int nx_full;                /* Global dimensions of the field */
    int ny_full;                /* Global dimensions of the field */
    double dx;
    double dy;
    double *data;
} field;

/* Datatype for basic parallelization information */
typedef struct {
    int size;                   /* Number of MPI tasks */
    int rank;
    int nup, ndown, nleft, nright; /* Ranks of neighbouring MPI tasks */
    MPI_Comm comm;             /* Cartesian communicator */
    MPI_Request requests[8];   /* Requests for non-blocking communication */
    MPI_Datatype rowtype;      /* MPI Datatype for communication of rows */
    MPI_Datatype columntype;   /* MPI Datatype for communication of columns */
    MPI_Datatype subarraytype; /* MPI Datatype for communication in text I/O */
    MPI_Datatype restarttype;  /* MPI Datatype for communication in restart I/O */
    MPI_Datatype filetype;     /* MPI Datatype for file view in restart I/O */
} parallel_data;


/* We use here fixed grid spacing */
#define DX 0.01
#define DY 0.01

/* file name for restart checkpoints*/
#define CHECKPOINT "HEAT_RESTART.dat"

/* Inline function for indexing the 2D arrays */
static inline int idx(int i, int j, int width)
{
    return i * width + j;
}

/* Function prototypes */
double *malloc_2d(int nx, int ny);

void free_2d(double *array);

void set_field_dimensions(field *temperature, int nx, int ny,
                          parallel_data *parallel);

void parallel_setup(parallel_data *parallel, int nx, int ny);

void initialize(int argc, char *argv[], field *temperature1,
                field *temperature2, int *nsteps, parallel_data *parallel,
                int *iter0);

void generate_field(field *temperature, parallel_data *parallel);

void exchange_init(field *temperature, parallel_data *parallel);

void exchange_finalize(parallel_data *parallel);

void evolve_interior(field *curr, field *prev, double a, double dt);

void evolve_edges(field *curr, field *prev, double a, double dt);

void write_field(field *temperature, int iter, parallel_data *parallel);

void read_field(field *temperature1, field *temperature2,
                char *filename, parallel_data *parallel);

void write_restart(field *temperature, parallel_data *parallel, int iter);

void read_restart(field *temperature, parallel_data *parallel, int *iter);

void copy_field(field *temperature1, field *temperature2);

void swap_fields(field *temperature1, field *temperature2);

void allocate_field(field *temperature);

void deallocate_field(field *temperature);

void finalize(field *temperature1, field *temperature2,
              parallel_data *parallel);

#endif  /* __HEAT_H__ */

