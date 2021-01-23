
#include "read_test.h"

#include "dataset.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_test
(
 configuration* pconfig,
 int size,
 int rank,
 int my_proc_row,
 int my_proc_col,
 unsigned long my_rows,
 unsigned long my_cols,
 hid_t fapl,
 hid_t mspace,
 hid_t dxpl,
 double* read_time
 )
{
  unsigned int step_first_flg;
  unsigned int istep, iarray;
  double *rbuf;
  size_t i, j;

  char path[255];

  hid_t file, dset, fspace;

#ifdef VERIFY_DATA
  /* Extent of the logical 4D array and partition origin/offset */
  size_t d[4], o[4];

  /*
   * The C-order of an index [i0, i1, i2, i3] in a 4D array of extent
   * [D0, D1, D2, D3] is ((i0*D1 + i1)*D2 + i2)*D3 + i3.
   *
   * The extent depends on the kind of scaling and, in parallel, we need
   * to adjust the indices by the partition origin/offset.
   */
#endif

  step_first_flg = (strncmp(pconfig->slowest_dimension, "step", 16) == 0);

  rbuf = (double*) calloc(my_rows*my_cols, sizeof(double));

  assert((file = H5Fopen(pconfig->hdf5_file, H5F_ACC_RDONLY, fapl)) >= 0);

  switch (pconfig->rank)
    {
    case 4:
      {
        assert((dset = H5Dopen(file, "dataset", H5P_DEFAULT)) >= 0);

        for (istep = 0; istep < pconfig->steps; ++istep)
          {
            for (iarray = 0; iarray < pconfig->arrays; ++iarray)
              {
                assert((fspace = H5Dget_space(dset)) >= 0);
                create_selection(pconfig, fspace, my_proc_row, my_proc_col,
                                 istep, iarray);
                *read_time -= MPI_Wtime();
                assert(H5Dread(dset, H5T_NATIVE_DOUBLE, mspace, fspace, dxpl,
                               rbuf) >= 0);
                *read_time += MPI_Wtime();
                assert(H5Sclose(fspace) >= 0);

#ifdef VERIFY_DATA
                d[0] = step_first_flg ? pconfig->steps : pconfig->arrays;
                d[1] = step_first_flg ? pconfig->arrays : pconfig->steps;
                o[0] = step_first_flg ? istep : iarray;
                o[1] = step_first_flg ? iarray : istep;
                verify_read_buffer(rbuf, &my_rows, &my_cols, d, o);
#endif
              }
          }

        assert(H5Dclose(dset) >= 0);
      }
      break;
    case 3:
      {
        if (step_first_flg) /* dataset per step */
          {
            for (istep = 0; istep < pconfig->steps; ++istep)
              {
                sprintf(path, "step=%d", istep);
                assert((dset = H5Dopen(file, path, H5P_DEFAULT)) >= 0);
                assert((fspace = H5Dget_space(dset)) >= 0);

                for (iarray = 0; iarray < pconfig->arrays; ++iarray)
                  {
                    create_selection(pconfig, fspace, my_proc_row,
                                     my_proc_col, istep, iarray);

                    *read_time -= MPI_Wtime();
                    assert(H5Dread(dset, H5T_NATIVE_DOUBLE, mspace, fspace,
                                   dxpl, rbuf) >= 0);
                    *read_time += MPI_Wtime();

#ifdef VERIFY_DATA
                    d[0] = pconfig->steps; d[1] = pconfig->arrays;
                    o[0] = istep; o[1] = iarray;
                    verify_read_buffer(rbuf, &my_rows, &my_cols, d, o);
#endif
                  }

                assert(H5Sclose(fspace) >= 0);
                assert(H5Dclose(dset) >= 0);

              }
          }
        else /* dataset per array */
          {
            for (istep = 0; istep < pconfig->steps; ++istep)
              {
                for (iarray = 0; iarray < pconfig->arrays; ++iarray)
                  {
                    sprintf(path, "array=%d", iarray);
                    assert((dset = H5Dopen(file, path, H5P_DEFAULT)) >= 0);
                    assert((fspace = H5Dget_space(dset)) >= 0);
                    create_selection(pconfig, fspace, my_proc_row,
                                     my_proc_col, istep, iarray);

                    *read_time -= MPI_Wtime();
                    assert(H5Dread(dset, H5T_NATIVE_DOUBLE, mspace, fspace,
                                   dxpl, rbuf) >= 0);
                    *read_time += MPI_Wtime();

                    assert(H5Sclose(fspace) >= 0);
                    assert(H5Dclose(dset) >= 0);

#ifdef VERIFY_DATA
                    d[0] = pconfig->arrays; d[1] = pconfig->steps;
                    o[0] = iarray; o[1] = istep;
                    verify_read_buffer(rbuf, &my_rows, &my_cols, d, o);
#endif
                  }
              }
          }
      }
      break;
    case 2:
      {
        for (istep = 0; istep < pconfig->steps; ++istep)
          {
            for (iarray = 0; iarray < pconfig->arrays; ++iarray)
              {
                /* group per step or array */
                sprintf(path, (step_first_flg ?
                               "step=%d/array=%d" : "array=%d/step=%d"),
                        (step_first_flg ? istep : iarray),
                        (step_first_flg ? iarray : istep));

                assert((dset = H5Dopen(file, path, H5P_DEFAULT)) >= 0);

                assert((fspace = H5Dget_space(dset)) >= 0);
                create_selection(pconfig, fspace, my_proc_row, my_proc_col,
                                 istep, iarray);

                *read_time -= MPI_Wtime();
                assert(H5Dread(dset, H5T_NATIVE_DOUBLE, mspace, fspace, dxpl,
                               rbuf) >= 0);
                *read_time += MPI_Wtime();

                assert(H5Sclose(fspace) >= 0);
                assert(H5Dclose(dset) >= 0);

#ifdef VERIFY_DATA
                d[0] = step_first_flg ? pconfig->steps : pconfig->arrays;
                d[1] = step_first_flg ? pconfig->arrays : pconfig->steps;
                o[0] = step_first_flg ? istep : iarray;
                o[1] = step_first_flg ? iarray : istep;
                verify_read_buffer(rbuf, &my_rows, &my_cols, d, o);
#endif
              }
          }
      }
      break;
    default:
      break;
    }

  assert(H5Fclose(file) >= 0);

  free(rbuf);
}
