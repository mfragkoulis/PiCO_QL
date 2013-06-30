/************************************************************

  This example shows how to read and write compound
  datatypes to a dataset.  The program first writes
  compound structures to a dataset with a dataspace of DIM0,
  then closes the file.  Next, it reopens the file, reads
  back the data, and outputs it to the screen.

  This file is intended for use with HDF5 Library version 1.8

 ************************************************************/

#include "hdf5.h"
#include <stdio.h>
#include <stdlib.h>
#include "sensor_t.h"
#include "pico_ql_search.h"

#define FILENAME            "h5ex_t_cmpd.h5"
#define DATASETNAME         "DS1"
#define DIM0            4
#define DIM1            3
#define DIM2            2
#define RANK            3

int
main (void)
{
    /* For writing dataset to HDF file */
    hid_t       file, memtype, strtype, space, dset;
                                            /* Handles */
    herr_t      status;
    hsize_t     dims[RANK] = {DIM0, DIM1, DIM2};
    sensor_t    wdata[DIM0][DIM1][DIM2];                /* Write buffer */

    /* For reading dataset from HDF file */
    sensor_t      rdata[DIM0][DIM1][DIM2];                     /* Read buffer */
    int          i, j, k;

    /*
     * Initialize data.
     */
    wdata[0][0][0].serial_no = 1153;
    wdata[0][0][0].location = "Exterior (static)";
    wdata[0][0][0].temperature = 53.23;
    wdata[0][0][0].pressure = 24.57;
    wdata[1][0][0].serial_no = 1184;
    wdata[1][0][0].location = "Intake";
    wdata[1][0][0].temperature = 55.12;
    wdata[1][0][0].pressure = 22.95;
    wdata[2][0][0].serial_no = 1027;
    wdata[2][0][0].location = "Intake manifold";
    wdata[2][0][0].temperature = 103.55;
    wdata[2][0][0].pressure = 31.23;
    wdata[3][0][0].serial_no = 1313;
    wdata[3][0][0].location = "Exhaust manifold";
    wdata[3][0][0].temperature = 1252.89;
    wdata[3][0][0].pressure = 84.11;

    wdata[0][1][0].serial_no = 1153;
    wdata[0][1][0].location = "Exterior (static)";
    wdata[0][1][0].temperature = 53.23;
    wdata[0][1][0].pressure = 24.57;
    wdata[1][1][0].serial_no = 1184;
    wdata[1][1][0].location = "Intake";
    wdata[1][1][0].temperature = 55.12;
    wdata[1][1][0].pressure = 22.95;
    wdata[2][1][0].serial_no = 1027;
    wdata[2][1][0].location = "Intake manifold";
    wdata[2][1][0].temperature = 103.55;
    wdata[2][1][0].pressure = 31.23;
    wdata[3][1][0].serial_no = 1313;
    wdata[3][1][0].location = "Exhaust manifold";
    wdata[3][1][0].temperature = 1252.89;
    wdata[3][1][0].pressure = 84.11;

    wdata[0][2][0].serial_no = 1153;
    wdata[0][2][0].location = "Exterior (static)";
    wdata[0][2][0].temperature = 53.23;
    wdata[0][2][0].pressure = 24.57;
    wdata[1][2][0].serial_no = 1184;
    wdata[1][2][0].location = "Intake";
    wdata[1][2][0].temperature = 55.12;
    wdata[1][2][0].pressure = 22.95;
    wdata[2][2][0].serial_no = 1027;
    wdata[2][2][0].location = "Intake manifold";
    wdata[2][2][0].temperature = 103.55;
    wdata[2][2][0].pressure = 31.23;
    wdata[3][2][0].serial_no = 1313;
    wdata[3][2][0].location = "Exhaust manifold";
    wdata[3][2][0].temperature = 1252.89;
    wdata[3][2][0].pressure = 84.11;

    wdata[0][0][1].serial_no = 1153;
    wdata[0][0][1].location = "Exterior (static)";
    wdata[0][0][1].temperature = 53.23;
    wdata[0][0][1].pressure = 24.57;
    wdata[1][0][1].serial_no = 1184;
    wdata[1][0][1].location = "Intake";
    wdata[1][0][1].temperature = 55.12;
    wdata[1][0][1].pressure = 22.95;
    wdata[2][0][1].serial_no = 1027;
    wdata[2][0][1].location = "Intake manifold";
    wdata[2][0][1].temperature = 103.55;
    wdata[2][0][1].pressure = 31.23;
    wdata[3][0][1].serial_no = 1313;
    wdata[3][0][1].location = "Exhaust manifold";
    wdata[3][0][1].temperature = 1252.89;
    wdata[3][0][1].pressure = 84.11;

    wdata[0][1][1].serial_no = 1153;
    wdata[0][1][1].location = "Exterior (static)";
    wdata[0][1][1].temperature = 53.23;
    wdata[0][1][1].pressure = 24.57;
    wdata[1][1][1].serial_no = 1184;
    wdata[1][1][1].location = "Intake";
    wdata[1][1][1].temperature = 55.12;
    wdata[1][1][1].pressure = 22.95;
    wdata[2][1][1].serial_no = 1027;
    wdata[2][1][1].location = "Intake manifold";
    wdata[2][1][1].temperature = 103.55;
    wdata[2][1][1].pressure = 31.23;
    wdata[3][1][1].serial_no = 1313;
    wdata[3][1][1].location = "Exhaust manifold";
    wdata[3][1][1].temperature = 1252.89;
    wdata[3][1][1].pressure = 84.11;

    wdata[0][2][1].serial_no = 1153;
    wdata[0][2][1].location = "Exterior (static)";
    wdata[0][2][1].temperature = 53.23;
    wdata[0][2][1].pressure = 24.57;
    wdata[1][2][1].serial_no = 1184;
    wdata[1][2][1].location = "Intake";
    wdata[1][2][1].temperature = 55.12;
    wdata[1][2][1].pressure = 22.95;
    wdata[2][2][1].serial_no = 1027;
    wdata[2][2][1].location = "Intake manifold";
    wdata[2][2][1].temperature = 103.55;
    wdata[2][2][1].pressure = 31.23;
    wdata[3][2][1].serial_no = 1313;
    wdata[3][2][1].location = "Exhaust manifold";
    wdata[3][2][1].temperature = 1252.89;
    wdata[3][2][1].pressure = 84.11;


    pico_ql_register(&wdata, "hd5_sensors");
    pico_ql_serve(8080);

    /*
     * Create dataspace.  Setting maximum size to NULL sets the maximum
     * size to be the current size.
     */
    space = H5Screate_simple (RANK, dims, NULL);

    /*
     * Create a new file using the default properties.
     */
    file = H5Fcreate (FILENAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    /*
     * Create variable-length string datatype.
     */
    strtype = H5Tcopy (H5T_C_S1);
    status = H5Tset_size (strtype, H5T_VARIABLE);

    /*
     * Create the compound datatype for memory.
     */
    memtype = H5Tcreate (H5T_COMPOUND, sizeof (sensor_t));
    status = H5Tinsert (memtype, "Serial number",
                HOFFSET (sensor_t, serial_no), H5T_NATIVE_INT);
    status = H5Tinsert (memtype, "Location", HOFFSET (sensor_t, location),
                strtype);
    status = H5Tinsert (memtype, "Temperature (F)",
                HOFFSET (sensor_t, temperature), H5T_NATIVE_DOUBLE);
    status = H5Tinsert (memtype, "Pressure (inHg)",
                HOFFSET (sensor_t, pressure), H5T_NATIVE_DOUBLE);

    /*
     * Create the dataset and write the compound data to it.
     */
    dset = H5Dcreate2 (file, DATASETNAME, memtype, space, H5P_DEFAULT, H5P_DEFAULT,
                H5P_DEFAULT);
    status = H5Dwrite (dset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, wdata);

    /*
     * Close and release resources.
     */
    status = H5Tclose (memtype);
    status = H5Sclose (space);
    status = H5Dclose (dset);
    status = H5Fclose (file);



    /*
     * Now we begin the read section of this example.  Here we assume
     * the dataset has the same name and rank, but can have any size.
     * Therefore we must allocate a new array to read in data using
     * malloc().  For simplicity, we do not rebuild memtype.
     */

    /*
     * Open file and dataset.
     */
    file = H5Fopen (FILENAME, H5F_ACC_RDONLY, H5P_DEFAULT);
    dset = H5Dopen2 (file, DATASETNAME, H5P_DEFAULT);

    /*
     * Get dataspace and allocate memory for read buffer.
     */
    space = H5Dget_space (dset);
    //ndims = H5Sget_simple_extent_ndims (space);
    //status_n = H5Sget_simple_extent_dims (space, dimsr, NULL);
    
    /*
     * Define the memory space to read dataset.
     */
    //space = H5Screate_simple(ndims, dimsr, NULL);

    /*
     * Create the compound datatype for memory.
     */
    memtype = H5Tcreate (H5T_COMPOUND, sizeof (sensor_t));
    status = H5Tinsert (memtype, "Serial number",
                HOFFSET (sensor_t, serial_no), H5T_NATIVE_INT);
    status = H5Tinsert (memtype, "Location", HOFFSET (sensor_t, location),
                strtype);
    status = H5Tinsert (memtype, "Temperature (F)",
                HOFFSET (sensor_t, temperature), H5T_NATIVE_DOUBLE);
    status = H5Tinsert (memtype, "Pressure (inHg)",
                HOFFSET (sensor_t, pressure), H5T_NATIVE_DOUBLE);


    /*
     * Read the data.
     */
    status = H5Dread (dset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);


    /*
     * Output the data to the screen.
     */
    printf("rdata %lx\n", (long unsigned int)rdata);
    for (i=0; i<dims[0]; i++) {
      printf("rdata[%d] %lx\n", i, (long unsigned int)rdata[i]);
      for (j=0; j<dims[1]; j++) {
        printf("rdata[%d][%d] %lx\n", i, j, (long unsigned int)rdata[i][j]);
        for (k=0; k<dims[2]; k++) {
          printf("&rdata[%d][%d][%d] %lx\n", i, j, k, (long unsigned int)&rdata[i][j][k]);
          printf ("%s[%d][%d][%d]:\n", DATASETNAME, i, j, k);
          printf ("Serial number   : %d\n", rdata[i][j][k].serial_no);
          printf ("Location        : %s\n", rdata[i][j][k].location);
          printf ("Temperature (F) : %f\n", rdata[i][j][k].temperature);
          printf ("Pressure (inHg) : %f\n\n", rdata[i][j][k].pressure);
/*          printf ("Serial number   : %d\n", rdata[i*dims[0]+j*dims[1]+k].serial_no);
          printf ("Location        : %s\n", rdata[i*dims[0]+j*dims[1]+k].location);
          printf ("Temperature (F) : %f\n", rdata[i*dims[0]+j*dims[1]+k].temperature);
          printf ("Pressure (inHg) : %f\n\n", rdata[i*dims[0]+j*dims[1]+k].pressure);
*/
        }
      }
    }


    /*
     * Close and release resources.  H5Dvlen_reclaim will automatically
     * traverse the structure and free any vlen data (strings in this
     * case).
     */
    status = H5Dvlen_reclaim (memtype, space, H5P_DEFAULT, rdata);

    status = H5Dclose (dset);
    status = H5Sclose (space);
    status = H5Tclose (memtype);
    status = H5Tclose (strtype);
    status = H5Fclose (file);

    return 0;
}
