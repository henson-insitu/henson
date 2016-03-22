
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef _EXTERN_C_
#ifdef __cplusplus
#define _EXTERN_C_ extern "C"
#else /* __cplusplus */
#define _EXTERN_C_
#endif /* __cplusplus */
#endif /* _EXTERN_C_ */

#ifdef MPICH_HAS_C2F
_EXTERN_C_ void *MPIR_ToPointer(int);
#endif // MPICH_HAS_C2F

#ifdef PIC
/* For shared libraries, declare these weak and figure out which one was linked
   based on which init wrapper was called.  See mpi_init wrappers.  */
#pragma weak pmpi_init
#pragma weak PMPI_INIT
#pragma weak pmpi_init_
#pragma weak pmpi_init__
#endif /* PIC */

_EXTERN_C_ void pmpi_init(MPI_Fint *ierr);
_EXTERN_C_ void PMPI_INIT(MPI_Fint *ierr);
_EXTERN_C_ void pmpi_init_(MPI_Fint *ierr);
_EXTERN_C_ void pmpi_init__(MPI_Fint *ierr);

#include <henson/context.h>

#define HENSON_REPLACE_COMM_WORLD(comm) \
    if (comm == MPI_COMM_WORLD) \
        comm = henson_get_world();

/* ================== C Wrappers for MPI_Init ================== */
_EXTERN_C_ int PMPI_Init(int *argc, char ***argv);
_EXTERN_C_ int MPI_Init(int *argc, char ***argv) { 
    int _wrap_py_return_val = 0;

    if (henson_active())
        return MPI_SUCCESS;
    _wrap_py_return_val = PMPI_Init(argc, argv);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Finalize ================== */
_EXTERN_C_ int PMPI_Finalize();
_EXTERN_C_ int MPI_Finalize() { 
    int _wrap_py_return_val = 0;

    if (henson_active())
        return MPI_SUCCESS;
    _wrap_py_return_val = PMPI_Finalize();
    return _wrap_py_return_val;
}



/* ================== C Wrappers for MPI_Init_thread ================== */
_EXTERN_C_ int PMPI_Init_thread(int *argc, char ***argv, int required, int *provided);
_EXTERN_C_ int MPI_Init_thread(int *argc, char ***argv, int required, int *provided) { 
    int _wrap_py_return_val = 0;

    if (henson_active())
    {
        *provided = MPI_THREAD_SINGLE;
        return MPI_SUCCESS;
    }
    _wrap_py_return_val = PMPI_Init_thread(argc, argv, required, provided);
    return _wrap_py_return_val;
}



/* ================== C Wrappers for MPI_Type_delete_attr ================== */
_EXTERN_C_ int PMPI_Type_delete_attr(MPI_Datatype datatype, int type_keyval);
_EXTERN_C_ int MPI_Type_delete_attr(MPI_Datatype datatype, int type_keyval) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_delete_attr(datatype, type_keyval);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Sendrecv_replace ================== */
_EXTERN_C_ int PMPI_Sendrecv_replace(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status);
_EXTERN_C_ int MPI_Sendrecv_replace(void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Sendrecv_replace(buf, count, datatype, dest, sendtag, source, recvtag, comm, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_create ================== */
_EXTERN_C_ int PMPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win);
_EXTERN_C_ int MPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Win_create(base, size, disp_unit, info, comm, win);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Error_class ================== */
_EXTERN_C_ int PMPI_Error_class(int errorcode, int *errorclass);
_EXTERN_C_ int MPI_Error_class(int errorcode, int *errorclass) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Error_class(errorcode, errorclass);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Free_mem ================== */
_EXTERN_C_ int PMPI_Free_mem(void *base);
_EXTERN_C_ int MPI_Free_mem(void *base) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Free_mem(base);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_get_info ================== */
_EXTERN_C_ int PMPI_Win_get_info(MPI_Win win, MPI_Info *info_used);
_EXTERN_C_ int MPI_Win_get_info(MPI_Win win, MPI_Info *info_used) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_get_info(win, info_used);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Buffer_detach ================== */
_EXTERN_C_ int PMPI_Buffer_detach(void *buffer_addr, int *size);
_EXTERN_C_ int MPI_Buffer_detach(void *buffer_addr, int *size) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Buffer_detach(buffer_addr, size);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Aint_add ================== */
_EXTERN_C_ MPI_Aint PMPI_Aint_add(MPI_Aint base, MPI_Aint disp);
_EXTERN_C_ MPI_Aint MPI_Aint_add(MPI_Aint base, MPI_Aint disp) { 
    MPI_Aint _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Aint_add(base, disp);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_flush_local_all ================== */
_EXTERN_C_ int PMPI_Win_flush_local_all(MPI_Win win);
_EXTERN_C_ int MPI_Win_flush_local_all(MPI_Win win) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_flush_local_all(win);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_create_keyval ================== */
_EXTERN_C_ int PMPI_Comm_create_keyval(MPI_Comm_copy_attr_function *comm_copy_attr_fn, MPI_Comm_delete_attr_function *comm_delete_attr_fn, int *comm_keyval, void *extra_state);
_EXTERN_C_ int MPI_Comm_create_keyval(MPI_Comm_copy_attr_function *comm_copy_attr_fn, MPI_Comm_delete_attr_function *comm_delete_attr_fn, int *comm_keyval, void *extra_state) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Comm_create_keyval(comm_copy_attr_fn, comm_delete_attr_fn, comm_keyval, extra_state);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_get_parent ================== */
_EXTERN_C_ int PMPI_Comm_get_parent(MPI_Comm *parent);
_EXTERN_C_ int MPI_Comm_get_parent(MPI_Comm *parent) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Comm_get_parent(parent);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Testany ================== */
_EXTERN_C_ int PMPI_Testany(int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status);
_EXTERN_C_ int MPI_Testany(int count, MPI_Request array_of_requests[], int *indx, int *flag, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Testany(count, array_of_requests, indx, flag, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_extent ================== */
_EXTERN_C_ int PMPI_Type_extent(MPI_Datatype datatype, MPI_Aint *extent);
_EXTERN_C_ int MPI_Type_extent(MPI_Datatype datatype, MPI_Aint *extent) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_extent(datatype, extent);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_cvar_handle_alloc ================== */
_EXTERN_C_ int PMPI_T_cvar_handle_alloc(int cvar_index, void *obj_handle, MPI_T_cvar_handle *handle, int *count);
_EXTERN_C_ int MPI_T_cvar_handle_alloc(int cvar_index, void *obj_handle, MPI_T_cvar_handle *handle, int *count) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_cvar_handle_alloc(cvar_index, obj_handle, handle, count);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_idup ================== */
_EXTERN_C_ int PMPI_Comm_idup(MPI_Comm comm, MPI_Comm *newcomm, MPI_Request *request);
_EXTERN_C_ int MPI_Comm_idup(MPI_Comm comm, MPI_Comm *newcomm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_idup(comm, newcomm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_set_name ================== */
_EXTERN_C_ int PMPI_Win_set_name(MPI_Win win, const char *win_name);
_EXTERN_C_ int MPI_Win_set_name(MPI_Win win, const char *win_name) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_set_name(win, win_name);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_dup ================== */
_EXTERN_C_ int PMPI_Type_dup(MPI_Datatype oldtype, MPI_Datatype *newtype);
_EXTERN_C_ int MPI_Type_dup(MPI_Datatype oldtype, MPI_Datatype *newtype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_dup(oldtype, newtype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_pvar_get_index ================== */
_EXTERN_C_ int PMPI_T_pvar_get_index(const char *name, int var_class, int *pvar_index);
_EXTERN_C_ int MPI_T_pvar_get_index(const char *name, int var_class, int *pvar_index) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_pvar_get_index(name, var_class, pvar_index);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Add_error_code ================== */
_EXTERN_C_ int PMPI_Add_error_code(int errorclass, int *errorcode);
_EXTERN_C_ int MPI_Add_error_code(int errorclass, int *errorcode) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Add_error_code(errorclass, errorcode);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_create_resized ================== */
_EXTERN_C_ int PMPI_Type_create_resized(MPI_Datatype oldtype, MPI_Aint lb, MPI_Aint extent, MPI_Datatype *newtype);
_EXTERN_C_ int MPI_Type_create_resized(MPI_Datatype oldtype, MPI_Aint lb, MPI_Aint extent, MPI_Datatype *newtype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_create_resized(oldtype, lb, extent, newtype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Get_address ================== */
_EXTERN_C_ int PMPI_Get_address(const void *location, MPI_Aint *address);
_EXTERN_C_ int MPI_Get_address(const void *location, MPI_Aint *address) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Get_address(location, address);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Iallgather ================== */
_EXTERN_C_ int PMPI_Iallgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Iallgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Iallgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Get_count ================== */
_EXTERN_C_ int PMPI_Get_count(const MPI_Status *status, MPI_Datatype datatype, int *count);
_EXTERN_C_ int MPI_Get_count(const MPI_Status *status, MPI_Datatype datatype, int *count) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Get_count(status, datatype, count);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Grequest_start ================== */
_EXTERN_C_ int PMPI_Grequest_start(MPI_Grequest_query_function *query_fn, MPI_Grequest_free_function *free_fn, MPI_Grequest_cancel_function *cancel_fn, void *extra_state, MPI_Request *request);
_EXTERN_C_ int MPI_Grequest_start(MPI_Grequest_query_function *query_fn, MPI_Grequest_free_function *free_fn, MPI_Grequest_cancel_function *cancel_fn, void *extra_state, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Grequest_start(query_fn, free_fn, cancel_fn, extra_state, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Cartdim_get ================== */
_EXTERN_C_ int PMPI_Cartdim_get(MPI_Comm comm, int *ndims);
_EXTERN_C_ int MPI_Cartdim_get(MPI_Comm comm, int *ndims) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Cartdim_get(comm, ndims);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Allgather ================== */
_EXTERN_C_ int PMPI_Allgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
_EXTERN_C_ int MPI_Allgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Cart_coords ================== */
_EXTERN_C_ int PMPI_Cart_coords(MPI_Comm comm, int rank, int maxdims, int coords[]);
_EXTERN_C_ int MPI_Cart_coords(MPI_Comm comm, int rank, int maxdims, int coords[]) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Cart_coords(comm, rank, maxdims, coords);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_split_type ================== */
_EXTERN_C_ int PMPI_Comm_split_type(MPI_Comm comm, int split_type, int key, MPI_Info info, MPI_Comm *newcomm);
_EXTERN_C_ int MPI_Comm_split_type(MPI_Comm comm, int split_type, int key, MPI_Info info, MPI_Comm *newcomm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_split_type(comm, split_type, key, info, newcomm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Rsend ================== */
_EXTERN_C_ int PMPI_Rsend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
_EXTERN_C_ int MPI_Rsend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Rsend(buf, count, datatype, dest, tag, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_get_amode ================== */
_EXTERN_C_ int PMPI_File_get_amode(MPI_File fh, int *amode);
_EXTERN_C_ int MPI_File_get_amode(MPI_File fh, int *amode) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_get_amode(fh, amode);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Neighbor_allgatherv ================== */
_EXTERN_C_ int PMPI_Neighbor_allgatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int recvcounts[], const int displs[], MPI_Datatype recvtype, MPI_Comm comm);
_EXTERN_C_ int MPI_Neighbor_allgatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int recvcounts[], const int displs[], MPI_Datatype recvtype, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Neighbor_allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Info_create ================== */
_EXTERN_C_ int PMPI_Info_create(MPI_Info *info);
_EXTERN_C_ int MPI_Info_create(MPI_Info *info) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Info_create(info);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_create_f90_complex ================== */
_EXTERN_C_ int PMPI_Type_create_f90_complex(int precision, int range, MPI_Datatype *newtype);
_EXTERN_C_ int MPI_Type_create_f90_complex(int precision, int range, MPI_Datatype *newtype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_create_f90_complex(precision, range, newtype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Status_set_elements_x ================== */
_EXTERN_C_ int PMPI_Status_set_elements_x(MPI_Status *status, MPI_Datatype datatype, MPI_Count count);
_EXTERN_C_ int MPI_Status_set_elements_x(MPI_Status *status, MPI_Datatype datatype, MPI_Count count) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Status_set_elements_x(status, datatype, count);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_set_name ================== */
_EXTERN_C_ int PMPI_Comm_set_name(MPI_Comm comm, const char *comm_name);
_EXTERN_C_ int MPI_Comm_set_name(MPI_Comm comm, const char *comm_name) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_set_name(comm, comm_name);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_remote_group ================== */
_EXTERN_C_ int PMPI_Comm_remote_group(MPI_Comm comm, MPI_Group *group);
_EXTERN_C_ int MPI_Comm_remote_group(MPI_Comm comm, MPI_Group *group) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_remote_group(comm, group);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Cart_shift ================== */
_EXTERN_C_ int PMPI_Cart_shift(MPI_Comm comm, int direction, int disp, int *rank_source, int *rank_dest);
_EXTERN_C_ int MPI_Cart_shift(MPI_Comm comm, int direction, int disp, int *rank_source, int *rank_dest) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Cart_shift(comm, direction, disp, rank_source, rank_dest);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_size ================== */
_EXTERN_C_ int PMPI_Comm_size(MPI_Comm comm, int *size);
_EXTERN_C_ int MPI_Comm_size(MPI_Comm comm, int *size) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_size(comm, size);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_pvar_get_num ================== */
_EXTERN_C_ int PMPI_T_pvar_get_num(int *num_pvar);
_EXTERN_C_ int MPI_T_pvar_get_num(int *num_pvar) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_pvar_get_num(num_pvar);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Request_free ================== */
_EXTERN_C_ int PMPI_Request_free(MPI_Request *request);
_EXTERN_C_ int MPI_Request_free(MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Request_free(request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_read_all_end ================== */
_EXTERN_C_ int PMPI_File_read_all_end(MPI_File fh, void *buf, MPI_Status *status);
_EXTERN_C_ int MPI_File_read_all_end(MPI_File fh, void *buf, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_read_all_end(fh, buf, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Irsend ================== */
_EXTERN_C_ int PMPI_Irsend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Irsend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Irsend(buf, count, datatype, dest, tag, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Address ================== */
_EXTERN_C_ int PMPI_Address(const void *location, MPI_Aint *address);
_EXTERN_C_ int MPI_Address(const void *location, MPI_Aint *address) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Address(location, address);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_compare ================== */
_EXTERN_C_ int PMPI_Comm_compare(MPI_Comm comm1, MPI_Comm comm2, int *result);
_EXTERN_C_ int MPI_Comm_compare(MPI_Comm comm1, MPI_Comm comm2, int *result) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm1);
HENSON_REPLACE_COMM_WORLD(comm2);

  _wrap_py_return_val = PMPI_Comm_compare(comm1, comm2, result);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Ineighbor_alltoall ================== */
_EXTERN_C_ int PMPI_Ineighbor_alltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Ineighbor_alltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Ineighbor_alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Query_thread ================== */
_EXTERN_C_ int PMPI_Query_thread(int *provided);
_EXTERN_C_ int MPI_Query_thread(int *provided) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Query_thread(provided);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_cvar_get_index ================== */
_EXTERN_C_ int PMPI_T_cvar_get_index(const char *name, int *cvar_index);
_EXTERN_C_ int MPI_T_cvar_get_index(const char *name, int *cvar_index) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_cvar_get_index(name, cvar_index);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Allgatherv ================== */
_EXTERN_C_ int PMPI_Allgatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int *recvcounts, const int *displs, MPI_Datatype recvtype, MPI_Comm comm);
_EXTERN_C_ int MPI_Allgatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int *recvcounts, const int *displs, MPI_Datatype recvtype, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Neighbor_allgather ================== */
_EXTERN_C_ int PMPI_Neighbor_allgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
_EXTERN_C_ int MPI_Neighbor_allgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Neighbor_allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_category_get_pvars ================== */
_EXTERN_C_ int PMPI_T_category_get_pvars(int cat_index, int len, int indices[]);
_EXTERN_C_ int MPI_T_category_get_pvars(int cat_index, int len, int indices[]) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_category_get_pvars(cat_index, len, indices);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_free_keyval ================== */
_EXTERN_C_ int PMPI_Comm_free_keyval(int *comm_keyval);
_EXTERN_C_ int MPI_Comm_free_keyval(int *comm_keyval) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Comm_free_keyval(comm_keyval);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Op_create ================== */
_EXTERN_C_ int PMPI_Op_create(MPI_User_function *user_fn, int commute, MPI_Op *op);
_EXTERN_C_ int MPI_Op_create(MPI_User_function *user_fn, int commute, MPI_Op *op) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Op_create(user_fn, commute, op);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_enum_get_info ================== */
_EXTERN_C_ int PMPI_T_enum_get_info(MPI_T_enum enumtype, int *num, char *name, int *name_len);
_EXTERN_C_ int MPI_T_enum_get_info(MPI_T_enum enumtype, int *num, char *name, int *name_len) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_enum_get_info(enumtype, num, name, name_len);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Ssend_init ================== */
_EXTERN_C_ int PMPI_Ssend_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Ssend_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Ssend_init(buf, count, datatype, dest, tag, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Rsend_init ================== */
_EXTERN_C_ int PMPI_Rsend_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Rsend_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Rsend_init(buf, count, datatype, dest, tag, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Igatherv ================== */
_EXTERN_C_ int PMPI_Igatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int recvcounts[], const int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Igatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int recvcounts[], const int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Igatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Mrecv ================== */
_EXTERN_C_ int PMPI_Mrecv(void *buf, int count, MPI_Datatype datatype, MPI_Message *message, MPI_Status *status);
_EXTERN_C_ int MPI_Mrecv(void *buf, int count, MPI_Datatype datatype, MPI_Message *message, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Mrecv(buf, count, datatype, message, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Errhandler_get ================== */
_EXTERN_C_ int PMPI_Errhandler_get(MPI_Comm comm, MPI_Errhandler *errhandler);
_EXTERN_C_ int MPI_Errhandler_get(MPI_Comm comm, MPI_Errhandler *errhandler) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Errhandler_get(comm, errhandler);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_test_inter ================== */
_EXTERN_C_ int PMPI_Comm_test_inter(MPI_Comm comm, int *flag);
_EXTERN_C_ int MPI_Comm_test_inter(MPI_Comm comm, int *flag) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_test_inter(comm, flag);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_read_all_begin ================== */
_EXTERN_C_ int PMPI_File_read_all_begin(MPI_File fh, void *buf, int count, MPI_Datatype datatype);
_EXTERN_C_ int MPI_File_read_all_begin(MPI_File fh, void *buf, int count, MPI_Datatype datatype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_read_all_begin(fh, buf, count, datatype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_attach ================== */
_EXTERN_C_ int PMPI_Win_attach(MPI_Win win, void *base, MPI_Aint size);
_EXTERN_C_ int MPI_Win_attach(MPI_Win win, void *base, MPI_Aint size) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_attach(win, base, size);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_read_at_all ================== */
_EXTERN_C_ int PMPI_File_read_at_all(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
_EXTERN_C_ int MPI_File_read_at_all(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_read_at_all(fh, offset, buf, count, datatype, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_read_ordered_end ================== */
_EXTERN_C_ int PMPI_File_read_ordered_end(MPI_File fh, void *buf, MPI_Status *status);
_EXTERN_C_ int MPI_File_read_ordered_end(MPI_File fh, void *buf, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_read_ordered_end(fh, buf, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_set_attr ================== */
_EXTERN_C_ int PMPI_Type_set_attr(MPI_Datatype datatype, int type_keyval, void *attribute_val);
_EXTERN_C_ int MPI_Type_set_attr(MPI_Datatype datatype, int type_keyval, void *attribute_val) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_set_attr(datatype, type_keyval, attribute_val);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Group_union ================== */
_EXTERN_C_ int PMPI_Group_union(MPI_Group group1, MPI_Group group2, MPI_Group *newgroup);
_EXTERN_C_ int MPI_Group_union(MPI_Group group1, MPI_Group group2, MPI_Group *newgroup) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Group_union(group1, group2, newgroup);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_get_contents ================== */
_EXTERN_C_ int PMPI_Type_get_contents(MPI_Datatype datatype, int max_integers, int max_addresses, int max_datatypes, int array_of_integers[], MPI_Aint array_of_addresses[], MPI_Datatype array_of_datatypes[]);
_EXTERN_C_ int MPI_Type_get_contents(MPI_Datatype datatype, int max_integers, int max_addresses, int max_datatypes, int array_of_integers[], MPI_Aint array_of_addresses[], MPI_Datatype array_of_datatypes[]) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_get_contents(datatype, max_integers, max_addresses, max_datatypes, array_of_integers, array_of_addresses, array_of_datatypes);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_lock ================== */
_EXTERN_C_ int PMPI_Win_lock(int lock_type, int rank, int assert, MPI_Win win);
_EXTERN_C_ int MPI_Win_lock(int lock_type, int rank, int assert, MPI_Win win) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_lock(lock_type, rank, assert, win);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_size_x ================== */
_EXTERN_C_ int PMPI_Type_size_x(MPI_Datatype datatype, MPI_Count *size);
_EXTERN_C_ int MPI_Type_size_x(MPI_Datatype datatype, MPI_Count *size) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_size_x(datatype, size);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_category_changed ================== */
_EXTERN_C_ int PMPI_T_category_changed(int *stamp);
_EXTERN_C_ int MPI_T_category_changed(int *stamp) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_category_changed(stamp);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_set_info ================== */
_EXTERN_C_ int PMPI_Comm_set_info(MPI_Comm comm, MPI_Info info);
_EXTERN_C_ int MPI_Comm_set_info(MPI_Comm comm, MPI_Info info) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_set_info(comm, info);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_pvar_stop ================== */
_EXTERN_C_ int PMPI_T_pvar_stop(MPI_T_pvar_session session, MPI_T_pvar_handle handle);
_EXTERN_C_ int MPI_T_pvar_stop(MPI_T_pvar_session session, MPI_T_pvar_handle handle) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_pvar_stop(session, handle);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_get_envelope ================== */
_EXTERN_C_ int PMPI_Type_get_envelope(MPI_Datatype datatype, int *num_integers, int *num_addresses, int *num_datatypes, int *combiner);
_EXTERN_C_ int MPI_Type_get_envelope(MPI_Datatype datatype, int *num_integers, int *num_addresses, int *num_datatypes, int *combiner) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_get_envelope(datatype, num_integers, num_addresses, num_datatypes, combiner);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Send ================== */
_EXTERN_C_ int PMPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
_EXTERN_C_ int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Send(buf, count, datatype, dest, tag, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_delete_attr ================== */
_EXTERN_C_ int PMPI_Win_delete_attr(MPI_Win win, int win_keyval);
_EXTERN_C_ int MPI_Win_delete_attr(MPI_Win win, int win_keyval) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_delete_attr(win, win_keyval);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_create_hindexed_block ================== */
_EXTERN_C_ int PMPI_Type_create_hindexed_block(int count, int blocklength, const MPI_Aint array_of_displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype);
_EXTERN_C_ int MPI_Type_create_hindexed_block(int count, int blocklength, const MPI_Aint array_of_displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_create_hindexed_block(count, blocklength, array_of_displacements, oldtype, newtype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Keyval_free ================== */
_EXTERN_C_ int PMPI_Keyval_free(int *keyval);
_EXTERN_C_ int MPI_Keyval_free(int *keyval) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Keyval_free(keyval);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Alloc_mem ================== */
_EXTERN_C_ int PMPI_Alloc_mem(MPI_Aint size, MPI_Info info, void *baseptr);
_EXTERN_C_ int MPI_Alloc_mem(MPI_Aint size, MPI_Info info, void *baseptr) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Alloc_mem(size, info, baseptr);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_get_atomicity ================== */
_EXTERN_C_ int PMPI_File_get_atomicity(MPI_File fh, int *flag);
_EXTERN_C_ int MPI_File_get_atomicity(MPI_File fh, int *flag) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_get_atomicity(fh, flag);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Dist_graph_create ================== */
_EXTERN_C_ int PMPI_Dist_graph_create(MPI_Comm comm_old, int n, const int sources[], const int degrees[], const int destinations[], const int weights[], MPI_Info info, int reorder, MPI_Comm *comm_dist_graph);
_EXTERN_C_ int MPI_Dist_graph_create(MPI_Comm comm_old, int n, const int sources[], const int degrees[], const int destinations[], const int weights[], MPI_Info info, int reorder, MPI_Comm *comm_dist_graph) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm_old);

  _wrap_py_return_val = PMPI_Dist_graph_create(comm_old, n, sources, degrees, destinations, weights, info, reorder, comm_dist_graph);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Probe ================== */
_EXTERN_C_ int PMPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status *status);
_EXTERN_C_ int MPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Probe(source, tag, comm, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_hvector ================== */
_EXTERN_C_ int PMPI_Type_hvector(int count, int blocklength, MPI_Aint stride, MPI_Datatype oldtype, MPI_Datatype *newtype);
_EXTERN_C_ int MPI_Type_hvector(int count, int blocklength, MPI_Aint stride, MPI_Datatype oldtype, MPI_Datatype *newtype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_hvector(count, blocklength, stride, oldtype, newtype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_close ================== */
_EXTERN_C_ int PMPI_File_close(MPI_File *fh);
_EXTERN_C_ int MPI_File_close(MPI_File *fh) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_close(fh);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_flush_local ================== */
_EXTERN_C_ int PMPI_Win_flush_local(int rank, MPI_Win win);
_EXTERN_C_ int MPI_Win_flush_local(int rank, MPI_Win win) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_flush_local(rank, win);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_create_errhandler ================== */
_EXTERN_C_ int PMPI_File_create_errhandler(MPI_File_errhandler_function *file_errhandler_fn, MPI_Errhandler *errhandler);
_EXTERN_C_ int MPI_File_create_errhandler(MPI_File_errhandler_function *file_errhandler_fn, MPI_Errhandler *errhandler) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_create_errhandler(file_errhandler_fn, errhandler);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Group_free ================== */
_EXTERN_C_ int PMPI_Group_free(MPI_Group *group);
_EXTERN_C_ int MPI_Group_free(MPI_Group *group) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Group_free(group);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Ialltoall ================== */
_EXTERN_C_ int PMPI_Ialltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Ialltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Ialltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_rank ================== */
_EXTERN_C_ int PMPI_Comm_rank(MPI_Comm comm, int *rank);
_EXTERN_C_ int MPI_Comm_rank(MPI_Comm comm, int *rank) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_rank(comm, rank);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Cancel ================== */
_EXTERN_C_ int PMPI_Cancel(MPI_Request *request);
_EXTERN_C_ int MPI_Cancel(MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Cancel(request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_post ================== */
_EXTERN_C_ int PMPI_Win_post(MPI_Group group, int assert, MPI_Win win);
_EXTERN_C_ int MPI_Win_post(MPI_Group group, int assert, MPI_Win win) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_post(group, assert, win);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_commit ================== */
_EXTERN_C_ int PMPI_Type_commit(MPI_Datatype *datatype);
_EXTERN_C_ int MPI_Type_commit(MPI_Datatype *datatype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_commit(datatype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_iwrite_shared ================== */
_EXTERN_C_ int PMPI_File_iwrite_shared(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
_EXTERN_C_ int MPI_File_iwrite_shared(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_iwrite_shared(fh, buf, count, datatype, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_get_extent_x ================== */
_EXTERN_C_ int PMPI_Type_get_extent_x(MPI_Datatype datatype, MPI_Count *lb, MPI_Count *extent);
_EXTERN_C_ int MPI_Type_get_extent_x(MPI_Datatype datatype, MPI_Count *lb, MPI_Count *extent) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_get_extent_x(datatype, lb, extent);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_write_at ================== */
_EXTERN_C_ int PMPI_File_write_at(MPI_File fh, MPI_Offset offset, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
_EXTERN_C_ int MPI_File_write_at(MPI_File fh, MPI_Offset offset, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_write_at(fh, offset, buf, count, datatype, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Cart_get ================== */
_EXTERN_C_ int PMPI_Cart_get(MPI_Comm comm, int maxdims, int dims[], int periods[], int coords[]);
_EXTERN_C_ int MPI_Cart_get(MPI_Comm comm, int maxdims, int dims[], int periods[], int coords[]) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Cart_get(comm, maxdims, dims, periods, coords);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_flush ================== */
_EXTERN_C_ int PMPI_Win_flush(int rank, MPI_Win win);
_EXTERN_C_ int MPI_Win_flush(int rank, MPI_Win win) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_flush(rank, win);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Intercomm_create ================== */
_EXTERN_C_ int PMPI_Intercomm_create(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm);
_EXTERN_C_ int MPI_Intercomm_create(MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(local_comm);
HENSON_REPLACE_COMM_WORLD(peer_comm);

  _wrap_py_return_val = PMPI_Intercomm_create(local_comm, local_leader, peer_comm, remote_leader, tag, newintercomm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Allreduce ================== */
_EXTERN_C_ int PMPI_Allreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
_EXTERN_C_ int MPI_Allreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_dup_with_info ================== */
_EXTERN_C_ int PMPI_Comm_dup_with_info(MPI_Comm comm, MPI_Info info, MPI_Comm *newcomm);
_EXTERN_C_ int MPI_Comm_dup_with_info(MPI_Comm comm, MPI_Info info, MPI_Comm *newcomm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_dup_with_info(comm, info, newcomm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Reduce ================== */
_EXTERN_C_ int PMPI_Reduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm);
_EXTERN_C_ int MPI_Reduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_hindexed ================== */
_EXTERN_C_ int PMPI_Type_hindexed(int count, const int *array_of_blocklengths, const MPI_Aint *array_of_displacements, MPI_Datatype oldtype, MPI_Datatype *newtype);
_EXTERN_C_ int MPI_Type_hindexed(int count, const int *array_of_blocklengths, const MPI_Aint *array_of_displacements, MPI_Datatype oldtype, MPI_Datatype *newtype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_hindexed(count, array_of_blocklengths, array_of_displacements, oldtype, newtype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_get_position ================== */
_EXTERN_C_ int PMPI_File_get_position(MPI_File fh, MPI_Offset *offset);
_EXTERN_C_ int MPI_File_get_position(MPI_File fh, MPI_Offset *offset) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_get_position(fh, offset);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Recv_init ================== */
_EXTERN_C_ int PMPI_Recv_init(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Recv_init(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Recv_init(buf, count, datatype, source, tag, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_group ================== */
_EXTERN_C_ int PMPI_Comm_group(MPI_Comm comm, MPI_Group *group);
_EXTERN_C_ int MPI_Comm_group(MPI_Comm comm, MPI_Group *group) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_group(comm, group);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Is_thread_main ================== */
_EXTERN_C_ int PMPI_Is_thread_main(int *flag);
_EXTERN_C_ int MPI_Is_thread_main(int *flag) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Is_thread_main(flag);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_iwrite_all ================== */
_EXTERN_C_ int PMPI_File_iwrite_all(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
_EXTERN_C_ int MPI_File_iwrite_all(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_iwrite_all(fh, buf, count, datatype, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_create_indexed_block ================== */
_EXTERN_C_ int PMPI_Type_create_indexed_block(int count, int blocklength, const int array_of_displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype);
_EXTERN_C_ int MPI_Type_create_indexed_block(int count, int blocklength, const int array_of_displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_create_indexed_block(count, blocklength, array_of_displacements, oldtype, newtype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Wait ================== */
_EXTERN_C_ int PMPI_Wait(MPI_Request *request, MPI_Status *status);
_EXTERN_C_ int MPI_Wait(MPI_Request *request, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Wait(request, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_create_dynamic ================== */
_EXTERN_C_ int PMPI_Win_create_dynamic(MPI_Info info, MPI_Comm comm, MPI_Win *win);
_EXTERN_C_ int MPI_Win_create_dynamic(MPI_Info info, MPI_Comm comm, MPI_Win *win) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Win_create_dynamic(info, comm, win);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Dist_graph_neighbors ================== */
_EXTERN_C_ int PMPI_Dist_graph_neighbors(MPI_Comm comm, int maxindegree, int sources[], int sourceweights[], int maxoutdegree, int destinations[], int destweights[]);
_EXTERN_C_ int MPI_Dist_graph_neighbors(MPI_Comm comm, int maxindegree, int sources[], int sourceweights[], int maxoutdegree, int destinations[], int destweights[]) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Dist_graph_neighbors(comm, maxindegree, sources, sourceweights, maxoutdegree, destinations, destweights);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Error_string ================== */
_EXTERN_C_ int PMPI_Error_string(int errorcode, char *string, int *resultlen);
_EXTERN_C_ int MPI_Error_string(int errorcode, char *string, int *resultlen) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Error_string(errorcode, string, resultlen);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_sync ================== */
_EXTERN_C_ int PMPI_File_sync(MPI_File fh);
_EXTERN_C_ int MPI_File_sync(MPI_File fh) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_sync(fh);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Ineighbor_allgatherv ================== */
_EXTERN_C_ int PMPI_Ineighbor_allgatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int recvcounts[], const int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Ineighbor_allgatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int recvcounts[], const int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Ineighbor_allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_free ================== */
_EXTERN_C_ int PMPI_Type_free(MPI_Datatype *datatype);
_EXTERN_C_ int MPI_Type_free(MPI_Datatype *datatype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_free(datatype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Compare_and_swap ================== */
_EXTERN_C_ int PMPI_Compare_and_swap(const void *origin_addr, const void *compare_addr, void *result_addr, MPI_Datatype datatype, int target_rank, MPI_Aint target_disp, MPI_Win win);
_EXTERN_C_ int MPI_Compare_and_swap(const void *origin_addr, const void *compare_addr, void *result_addr, MPI_Datatype datatype, int target_rank, MPI_Aint target_disp, MPI_Win win) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Compare_and_swap(origin_addr, compare_addr, result_addr, datatype, target_rank, target_disp, win);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_unlock_all ================== */
_EXTERN_C_ int PMPI_Win_unlock_all(MPI_Win win);
_EXTERN_C_ int MPI_Win_unlock_all(MPI_Win win) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_unlock_all(win);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_get_attr ================== */
_EXTERN_C_ int PMPI_Type_get_attr(MPI_Datatype datatype, int type_keyval, void *attribute_val, int *flag);
_EXTERN_C_ int MPI_Type_get_attr(MPI_Datatype datatype, int type_keyval, void *attribute_val, int *flag) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_get_attr(datatype, type_keyval, attribute_val, flag);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_disconnect ================== */
_EXTERN_C_ int PMPI_Comm_disconnect(MPI_Comm *comm);
_EXTERN_C_ int MPI_Comm_disconnect(MPI_Comm *comm) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Comm_disconnect(comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_pvar_readreset ================== */
_EXTERN_C_ int PMPI_T_pvar_readreset(MPI_T_pvar_session session, MPI_T_pvar_handle handle, void *buf);
_EXTERN_C_ int MPI_T_pvar_readreset(MPI_T_pvar_session session, MPI_T_pvar_handle handle, void *buf) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_pvar_readreset(session, handle, buf);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Attr_get ================== */
_EXTERN_C_ int PMPI_Attr_get(MPI_Comm comm, int keyval, void *attribute_val, int *flag);
_EXTERN_C_ int MPI_Attr_get(MPI_Comm comm, int keyval, void *attribute_val, int *flag) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Attr_get(comm, keyval, attribute_val, flag);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_cvar_handle_free ================== */
_EXTERN_C_ int PMPI_T_cvar_handle_free(MPI_T_cvar_handle *handle);
_EXTERN_C_ int MPI_T_cvar_handle_free(MPI_T_cvar_handle *handle) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_cvar_handle_free(handle);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_enum_get_item ================== */
_EXTERN_C_ int PMPI_T_enum_get_item(MPI_T_enum enumtype, int indx, int *value, char *name, int *name_len);
_EXTERN_C_ int MPI_T_enum_get_item(MPI_T_enum enumtype, int indx, int *value, char *name, int *name_len) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_enum_get_item(enumtype, indx, value, name, name_len);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_call_errhandler ================== */
_EXTERN_C_ int PMPI_File_call_errhandler(MPI_File fh, int errorcode);
_EXTERN_C_ int MPI_File_call_errhandler(MPI_File fh, int errorcode) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_call_errhandler(fh, errorcode);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Group_compare ================== */
_EXTERN_C_ int PMPI_Group_compare(MPI_Group group1, MPI_Group group2, int *result);
_EXTERN_C_ int MPI_Group_compare(MPI_Group group1, MPI_Group group2, int *result) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Group_compare(group1, group2, result);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Imrecv ================== */
_EXTERN_C_ int PMPI_Imrecv(void *buf, int count, MPI_Datatype datatype, MPI_Message *message, MPI_Request *request);
_EXTERN_C_ int MPI_Imrecv(void *buf, int count, MPI_Datatype datatype, MPI_Message *message, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Imrecv(buf, count, datatype, message, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_category_get_index ================== */
_EXTERN_C_ int PMPI_T_category_get_index(const char *name, int *cat_index);
_EXTERN_C_ int MPI_T_category_get_index(const char *name, int *cat_index) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_category_get_index(name, cat_index);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Test_cancelled ================== */
_EXTERN_C_ int PMPI_Test_cancelled(const MPI_Status *status, int *flag);
_EXTERN_C_ int MPI_Test_cancelled(const MPI_Status *status, int *flag) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Test_cancelled(status, flag);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Buffer_attach ================== */
_EXTERN_C_ int PMPI_Buffer_attach(void *buffer, int size);
_EXTERN_C_ int MPI_Buffer_attach(void *buffer, int size) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Buffer_attach(buffer, size);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_call_errhandler ================== */
_EXTERN_C_ int PMPI_Win_call_errhandler(MPI_Win win, int errorcode);
_EXTERN_C_ int MPI_Win_call_errhandler(MPI_Win win, int errorcode) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_call_errhandler(win, errorcode);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_struct ================== */
_EXTERN_C_ int PMPI_Type_struct(int count, const int *array_of_blocklengths, const MPI_Aint *array_of_displacements, const MPI_Datatype *array_of_types, MPI_Datatype *newtype);
_EXTERN_C_ int MPI_Type_struct(int count, const int *array_of_blocklengths, const MPI_Aint *array_of_displacements, const MPI_Datatype *array_of_types, MPI_Datatype *newtype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_struct(count, array_of_blocklengths, array_of_displacements, array_of_types, newtype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Graph_neighbors_count ================== */
_EXTERN_C_ int PMPI_Graph_neighbors_count(MPI_Comm comm, int rank, int *nneighbors);
_EXTERN_C_ int MPI_Graph_neighbors_count(MPI_Comm comm, int rank, int *nneighbors) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Graph_neighbors_count(comm, rank, nneighbors);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_get_position_shared ================== */
_EXTERN_C_ int PMPI_File_get_position_shared(MPI_File fh, MPI_Offset *offset);
_EXTERN_C_ int MPI_File_get_position_shared(MPI_File fh, MPI_Offset *offset) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_get_position_shared(fh, offset);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Cart_map ================== */
_EXTERN_C_ int PMPI_Cart_map(MPI_Comm comm, int ndims, const int dims[], const int periods[], int *newrank);
_EXTERN_C_ int MPI_Cart_map(MPI_Comm comm, int ndims, const int dims[], const int periods[], int *newrank) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Cart_map(comm, ndims, dims, periods, newrank);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_iread ================== */
_EXTERN_C_ int PMPI_File_iread(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
_EXTERN_C_ int MPI_File_iread(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_iread(fh, buf, count, datatype, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Dist_graph_neighbors_count ================== */
_EXTERN_C_ int PMPI_Dist_graph_neighbors_count(MPI_Comm comm, int *indegree, int *outdegree, int *weighted);
_EXTERN_C_ int MPI_Dist_graph_neighbors_count(MPI_Comm comm, int *indegree, int *outdegree, int *weighted) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Dist_graph_neighbors_count(comm, indegree, outdegree, weighted);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_cvar_get_info ================== */
_EXTERN_C_ int PMPI_T_cvar_get_info(int cvar_index, char *name, int *name_len, int *verbosity, MPI_Datatype *datatype, MPI_T_enum *enumtype, char *desc, int *desc_len, int *binding, int *scope);
_EXTERN_C_ int MPI_T_cvar_get_info(int cvar_index, char *name, int *name_len, int *verbosity, MPI_Datatype *datatype, MPI_T_enum *enumtype, char *desc, int *desc_len, int *binding, int *scope) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_cvar_get_info(cvar_index, name, name_len, verbosity, datatype, enumtype, desc, desc_len, binding, scope);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_pvar_read ================== */
_EXTERN_C_ int PMPI_T_pvar_read(MPI_T_pvar_session session, MPI_T_pvar_handle handle, void *buf);
_EXTERN_C_ int MPI_T_pvar_read(MPI_T_pvar_session session, MPI_T_pvar_handle handle, void *buf) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_pvar_read(session, handle, buf);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_seek ================== */
_EXTERN_C_ int PMPI_File_seek(MPI_File fh, MPI_Offset offset, int whence);
_EXTERN_C_ int MPI_File_seek(MPI_File fh, MPI_Offset offset, int whence) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_seek(fh, offset, whence);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Neighbor_alltoallw ================== */
_EXTERN_C_ int PMPI_Neighbor_alltoallw(const void *sendbuf, const int sendcounts[], const MPI_Aint sdispls[], const MPI_Datatype sendtypes[], void *recvbuf, const int recvcounts[], const MPI_Aint rdispls[], const MPI_Datatype recvtypes[], MPI_Comm comm);
_EXTERN_C_ int MPI_Neighbor_alltoallw(const void *sendbuf, const int sendcounts[], const MPI_Aint sdispls[], const MPI_Datatype sendtypes[], void *recvbuf, const int recvcounts[], const MPI_Aint rdispls[], const MPI_Datatype recvtypes[], MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Neighbor_alltoallw(sendbuf, sendcounts, sdispls, sendtypes, recvbuf, recvcounts, rdispls, recvtypes, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Neighbor_alltoallv ================== */
_EXTERN_C_ int PMPI_Neighbor_alltoallv(const void *sendbuf, const int sendcounts[], const int sdispls[], MPI_Datatype sendtype, void *recvbuf, const int recvcounts[], const int rdispls[], MPI_Datatype recvtype, MPI_Comm comm);
_EXTERN_C_ int MPI_Neighbor_alltoallv(const void *sendbuf, const int sendcounts[], const int sdispls[], MPI_Datatype sendtype, void *recvbuf, const int recvcounts[], const int rdispls[], MPI_Datatype recvtype, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Neighbor_alltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Publish_name ================== */
_EXTERN_C_ int PMPI_Publish_name(const char *service_name, MPI_Info info, const char *port_name);
_EXTERN_C_ int MPI_Publish_name(const char *service_name, MPI_Info info, const char *port_name) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Publish_name(service_name, info, port_name);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_get_extent ================== */
_EXTERN_C_ int PMPI_Type_get_extent(MPI_Datatype datatype, MPI_Aint *lb, MPI_Aint *extent);
_EXTERN_C_ int MPI_Type_get_extent(MPI_Datatype datatype, MPI_Aint *lb, MPI_Aint *extent) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_get_extent(datatype, lb, extent);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Graph_create ================== */
_EXTERN_C_ int PMPI_Graph_create(MPI_Comm comm_old, int nnodes, const int indx[], const int edges[], int reorder, MPI_Comm *comm_graph);
_EXTERN_C_ int MPI_Graph_create(MPI_Comm comm_old, int nnodes, const int indx[], const int edges[], int reorder, MPI_Comm *comm_graph) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm_old);

  _wrap_py_return_val = PMPI_Graph_create(comm_old, nnodes, indx, edges, reorder, comm_graph);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Put ================== */
_EXTERN_C_ int PMPI_Put(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
_EXTERN_C_ int MPI_Put(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_write_at_all ================== */
_EXTERN_C_ int PMPI_File_write_at_all(MPI_File fh, MPI_Offset offset, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
_EXTERN_C_ int MPI_File_write_at_all(MPI_File fh, MPI_Offset offset, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_write_at_all(fh, offset, buf, count, datatype, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Ibsend ================== */
_EXTERN_C_ int PMPI_Ibsend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Ibsend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Ibsend(buf, count, datatype, dest, tag, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_complete ================== */
_EXTERN_C_ int PMPI_Win_complete(MPI_Win win);
_EXTERN_C_ int MPI_Win_complete(MPI_Win win) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_complete(win);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Graph_map ================== */
_EXTERN_C_ int PMPI_Graph_map(MPI_Comm comm, int nnodes, const int indx[], const int edges[], int *newrank);
_EXTERN_C_ int MPI_Graph_map(MPI_Comm comm, int nnodes, const int indx[], const int edges[], int *newrank) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Graph_map(comm, nnodes, indx, edges, newrank);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Info_get_valuelen ================== */
_EXTERN_C_ int PMPI_Info_get_valuelen(MPI_Info info, const char *key, int *valuelen, int *flag);
_EXTERN_C_ int MPI_Info_get_valuelen(MPI_Info info, const char *key, int *valuelen, int *flag) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Info_get_valuelen(info, key, valuelen, flag);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Cart_sub ================== */
_EXTERN_C_ int PMPI_Cart_sub(MPI_Comm comm, const int remain_dims[], MPI_Comm *newcomm);
_EXTERN_C_ int MPI_Cart_sub(MPI_Comm comm, const int remain_dims[], MPI_Comm *newcomm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Cart_sub(comm, remain_dims, newcomm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Info_get ================== */
_EXTERN_C_ int PMPI_Info_get(MPI_Info info, const char *key, int valuelen, char *value, int *flag);
_EXTERN_C_ int MPI_Info_get(MPI_Info info, const char *key, int valuelen, char *value, int *flag) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Info_get(info, key, valuelen, value, flag);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_iwrite_at ================== */
_EXTERN_C_ int PMPI_File_iwrite_at(MPI_File fh, MPI_Offset offset, const void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
_EXTERN_C_ int MPI_File_iwrite_at(MPI_File fh, MPI_Offset offset, const void *buf, int count, MPI_Datatype datatype, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_iwrite_at(fh, offset, buf, count, datatype, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_spawn ================== */
_EXTERN_C_ int PMPI_Comm_spawn(const char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]);
_EXTERN_C_ int MPI_Comm_spawn(const char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_spawn(command, argv, maxprocs, info, root, comm, intercomm, array_of_errcodes);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Group_intersection ================== */
_EXTERN_C_ int PMPI_Group_intersection(MPI_Group group1, MPI_Group group2, MPI_Group *newgroup);
_EXTERN_C_ int MPI_Group_intersection(MPI_Group group1, MPI_Group group2, MPI_Group *newgroup) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Group_intersection(group1, group2, newgroup);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Iallgatherv ================== */
_EXTERN_C_ int PMPI_Iallgatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int recvcounts[], const int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Iallgatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int recvcounts[], const int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Iallgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Ibcast ================== */
_EXTERN_C_ int PMPI_Ibcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Ibcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Ibcast(buffer, count, datatype, root, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_read_at_all_begin ================== */
_EXTERN_C_ int PMPI_File_read_at_all_begin(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype);
_EXTERN_C_ int MPI_File_read_at_all_begin(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_read_at_all_begin(fh, offset, buf, count, datatype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Info_get_nthkey ================== */
_EXTERN_C_ int PMPI_Info_get_nthkey(MPI_Info info, int n, char *key);
_EXTERN_C_ int MPI_Info_get_nthkey(MPI_Info info, int n, char *key) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Info_get_nthkey(info, n, key);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_get_size ================== */
_EXTERN_C_ int PMPI_File_get_size(MPI_File fh, MPI_Offset *size);
_EXTERN_C_ int MPI_File_get_size(MPI_File fh, MPI_Offset *size) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_get_size(fh, size);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Rput ================== */
_EXTERN_C_ int PMPI_Rput(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win, MPI_Request *request);
_EXTERN_C_ int MPI_Rput(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Rput(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_indexed ================== */
_EXTERN_C_ int PMPI_Type_indexed(int count, const int *array_of_blocklengths, const int *array_of_displacements, MPI_Datatype oldtype, MPI_Datatype *newtype);
_EXTERN_C_ int MPI_Type_indexed(int count, const int *array_of_blocklengths, const int *array_of_displacements, MPI_Datatype oldtype, MPI_Datatype *newtype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_indexed(count, array_of_blocklengths, array_of_displacements, oldtype, newtype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Send_init ================== */
_EXTERN_C_ int PMPI_Send_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Send_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Send_init(buf, count, datatype, dest, tag, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_lb ================== */
_EXTERN_C_ int PMPI_Type_lb(MPI_Datatype datatype, MPI_Aint *displacement);
_EXTERN_C_ int MPI_Type_lb(MPI_Datatype datatype, MPI_Aint *displacement) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_lb(datatype, displacement);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_set_info ================== */
_EXTERN_C_ int PMPI_File_set_info(MPI_File fh, MPI_Info info);
_EXTERN_C_ int MPI_File_set_info(MPI_File fh, MPI_Info info) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_set_info(fh, info);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_set_atomicity ================== */
_EXTERN_C_ int PMPI_File_set_atomicity(MPI_File fh, int flag);
_EXTERN_C_ int MPI_File_set_atomicity(MPI_File fh, int flag) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_set_atomicity(fh, flag);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Graph_get ================== */
_EXTERN_C_ int PMPI_Graph_get(MPI_Comm comm, int maxindex, int maxedges, int indx[], int edges[]);
_EXTERN_C_ int MPI_Graph_get(MPI_Comm comm, int maxindex, int maxedges, int indx[], int edges[]) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Graph_get(comm, maxindex, maxedges, indx, edges);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_read_ordered ================== */
_EXTERN_C_ int PMPI_File_read_ordered(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
_EXTERN_C_ int MPI_File_read_ordered(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_read_ordered(fh, buf, count, datatype, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Pack_external_size ================== */
_EXTERN_C_ int PMPI_Pack_external_size(const char datarep[], int incount, MPI_Datatype datatype, MPI_Aint *size);
_EXTERN_C_ int MPI_Pack_external_size(const char datarep[], int incount, MPI_Datatype datatype, MPI_Aint *size) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Pack_external_size(datarep, incount, datatype, size);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_create_darray ================== */
_EXTERN_C_ int PMPI_Type_create_darray(int size, int rank, int ndims, const int array_of_gsizes[], const int array_of_distribs[], const int array_of_dargs[], const int array_of_psizes[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype);
_EXTERN_C_ int MPI_Type_create_darray(int size, int rank, int ndims, const int array_of_gsizes[], const int array_of_distribs[], const int array_of_dargs[], const int array_of_psizes[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_create_darray(size, rank, ndims, array_of_gsizes, array_of_distribs, array_of_dargs, array_of_psizes, order, oldtype, newtype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_set_errhandler ================== */
_EXTERN_C_ int PMPI_Win_set_errhandler(MPI_Win win, MPI_Errhandler errhandler);
_EXTERN_C_ int MPI_Win_set_errhandler(MPI_Win win, MPI_Errhandler errhandler) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_set_errhandler(win, errhandler);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_create_keyval ================== */
_EXTERN_C_ int PMPI_Type_create_keyval(MPI_Type_copy_attr_function *type_copy_attr_fn, MPI_Type_delete_attr_function *type_delete_attr_fn, int *type_keyval, void *extra_state);
_EXTERN_C_ int MPI_Type_create_keyval(MPI_Type_copy_attr_function *type_copy_attr_fn, MPI_Type_delete_attr_function *type_delete_attr_fn, int *type_keyval, void *extra_state) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_create_keyval(type_copy_attr_fn, type_delete_attr_fn, type_keyval, extra_state);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_accept ================== */
_EXTERN_C_ int PMPI_Comm_accept(const char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm);
_EXTERN_C_ int MPI_Comm_accept(const char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_accept(port_name, info, root, comm, newcomm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_create_struct ================== */
_EXTERN_C_ int PMPI_Type_create_struct(int count, const int array_of_blocklengths[], const MPI_Aint array_of_displacements[], const MPI_Datatype array_of_types[], MPI_Datatype *newtype);
_EXTERN_C_ int MPI_Type_create_struct(int count, const int array_of_blocklengths[], const MPI_Aint array_of_displacements[], const MPI_Datatype array_of_types[], MPI_Datatype *newtype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_create_struct(count, array_of_blocklengths, array_of_displacements, array_of_types, newtype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Gather ================== */
_EXTERN_C_ int PMPI_Gather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
_EXTERN_C_ int MPI_Gather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Gather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Close_port ================== */
_EXTERN_C_ int PMPI_Close_port(const char *port_name);
_EXTERN_C_ int MPI_Close_port(const char *port_name) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Close_port(port_name);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_sync ================== */
_EXTERN_C_ int PMPI_Win_sync(MPI_Win win);
_EXTERN_C_ int MPI_Win_sync(MPI_Win win) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_sync(win);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_create_subarray ================== */
_EXTERN_C_ int PMPI_Type_create_subarray(int ndims, const int array_of_sizes[], const int array_of_subsizes[], const int array_of_starts[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype);
_EXTERN_C_ int MPI_Type_create_subarray(int ndims, const int array_of_sizes[], const int array_of_subsizes[], const int array_of_starts[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_create_subarray(ndims, array_of_sizes, array_of_subsizes, array_of_starts, order, oldtype, newtype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_free_keyval ================== */
_EXTERN_C_ int PMPI_Win_free_keyval(int *win_keyval);
_EXTERN_C_ int MPI_Win_free_keyval(int *win_keyval) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_free_keyval(win_keyval);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_write_at_all_end ================== */
_EXTERN_C_ int PMPI_File_write_at_all_end(MPI_File fh, const void *buf, MPI_Status *status);
_EXTERN_C_ int MPI_File_write_at_all_end(MPI_File fh, const void *buf, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_write_at_all_end(fh, buf, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Rget_accumulate ================== */
_EXTERN_C_ int PMPI_Rget_accumulate(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, void *result_addr, int result_count, MPI_Datatype result_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win, MPI_Request *request);
_EXTERN_C_ int MPI_Rget_accumulate(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, void *result_addr, int result_count, MPI_Datatype result_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Rget_accumulate(origin_addr, origin_count, origin_datatype, result_addr, result_count, result_datatype, target_rank, target_disp, target_count, target_datatype, op, win, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Waitall ================== */
_EXTERN_C_ int PMPI_Waitall(int count, MPI_Request array_of_requests[], MPI_Status array_of_statuses[]);
_EXTERN_C_ int MPI_Waitall(int count, MPI_Request array_of_requests[], MPI_Status array_of_statuses[]) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Waitall(count, array_of_requests, array_of_statuses);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_delete_attr ================== */
_EXTERN_C_ int PMPI_Comm_delete_attr(MPI_Comm comm, int comm_keyval);
_EXTERN_C_ int MPI_Comm_delete_attr(MPI_Comm comm, int comm_keyval) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_delete_attr(comm, comm_keyval);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Testall ================== */
_EXTERN_C_ int PMPI_Testall(int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[]);
_EXTERN_C_ int MPI_Testall(int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[]) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Testall(count, array_of_requests, flag, array_of_statuses);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_create_errhandler ================== */
_EXTERN_C_ int PMPI_Comm_create_errhandler(MPI_Comm_errhandler_function *comm_errhandler_fn, MPI_Errhandler *errhandler);
_EXTERN_C_ int MPI_Comm_create_errhandler(MPI_Comm_errhandler_function *comm_errhandler_fn, MPI_Errhandler *errhandler) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Comm_create_errhandler(comm_errhandler_fn, errhandler);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Barrier ================== */
_EXTERN_C_ int PMPI_Barrier(MPI_Comm comm);
_EXTERN_C_ int MPI_Barrier(MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Barrier(comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_get_attr ================== */
_EXTERN_C_ int PMPI_Win_get_attr(MPI_Win win, int win_keyval, void *attribute_val, int *flag);
_EXTERN_C_ int MPI_Win_get_attr(MPI_Win win, int win_keyval, void *attribute_val, int *flag) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_get_attr(win, win_keyval, attribute_val, flag);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_get_byte_offset ================== */
_EXTERN_C_ int PMPI_File_get_byte_offset(MPI_File fh, MPI_Offset offset, MPI_Offset *disp);
_EXTERN_C_ int MPI_File_get_byte_offset(MPI_File fh, MPI_Offset offset, MPI_Offset *disp) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_get_byte_offset(fh, offset, disp);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Waitsome ================== */
_EXTERN_C_ int PMPI_Waitsome(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[]);
_EXTERN_C_ int MPI_Waitsome(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[]) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Waitsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_lock_all ================== */
_EXTERN_C_ int PMPI_Win_lock_all(int assert, MPI_Win win);
_EXTERN_C_ int MPI_Win_lock_all(int assert, MPI_Win win) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_lock_all(assert, win);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_get_name ================== */
_EXTERN_C_ int PMPI_Comm_get_name(MPI_Comm comm, char *comm_name, int *resultlen);
_EXTERN_C_ int MPI_Comm_get_name(MPI_Comm comm, char *comm_name, int *resultlen) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_get_name(comm, comm_name, resultlen);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Group_range_excl ================== */
_EXTERN_C_ int PMPI_Group_range_excl(MPI_Group group, int n, int ranges[][3], MPI_Group *newgroup);
_EXTERN_C_ int MPI_Group_range_excl(MPI_Group group, int n, int ranges[][3], MPI_Group *newgroup) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Group_range_excl(group, n, ranges, newgroup);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_split ================== */
_EXTERN_C_ int PMPI_Comm_split(MPI_Comm comm, int color, int key, MPI_Comm *newcomm);
_EXTERN_C_ int MPI_Comm_split(MPI_Comm comm, int color, int key, MPI_Comm *newcomm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_split(comm, color, key, newcomm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_pvar_handle_alloc ================== */
_EXTERN_C_ int PMPI_T_pvar_handle_alloc(MPI_T_pvar_session session, int pvar_index, void *obj_handle, MPI_T_pvar_handle *handle, int *count);
_EXTERN_C_ int MPI_T_pvar_handle_alloc(MPI_T_pvar_session session, int pvar_index, void *obj_handle, MPI_T_pvar_handle *handle, int *count) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_pvar_handle_alloc(session, pvar_index, obj_handle, handle, count);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_read_ordered_begin ================== */
_EXTERN_C_ int PMPI_File_read_ordered_begin(MPI_File fh, void *buf, int count, MPI_Datatype datatype);
_EXTERN_C_ int MPI_File_read_ordered_begin(MPI_File fh, void *buf, int count, MPI_Datatype datatype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_read_ordered_begin(fh, buf, count, datatype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_create_hindexed ================== */
_EXTERN_C_ int PMPI_Type_create_hindexed(int count, const int array_of_blocklengths[], const MPI_Aint array_of_displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype);
_EXTERN_C_ int MPI_Type_create_hindexed(int count, const int array_of_blocklengths[], const MPI_Aint array_of_displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_create_hindexed(count, array_of_blocklengths, array_of_displacements, oldtype, newtype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_write_all_begin ================== */
_EXTERN_C_ int PMPI_File_write_all_begin(MPI_File fh, const void *buf, int count, MPI_Datatype datatype);
_EXTERN_C_ int MPI_File_write_all_begin(MPI_File fh, const void *buf, int count, MPI_Datatype datatype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_write_all_begin(fh, buf, count, datatype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Attr_delete ================== */
_EXTERN_C_ int PMPI_Attr_delete(MPI_Comm comm, int keyval);
_EXTERN_C_ int MPI_Attr_delete(MPI_Comm comm, int keyval) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Attr_delete(comm, keyval);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_get_errhandler ================== */
_EXTERN_C_ int PMPI_Win_get_errhandler(MPI_Win win, MPI_Errhandler *errhandler);
_EXTERN_C_ int MPI_Win_get_errhandler(MPI_Win win, MPI_Errhandler *errhandler) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_get_errhandler(win, errhandler);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_get_info ================== */
_EXTERN_C_ int PMPI_File_get_info(MPI_File fh, MPI_Info *info_used);
_EXTERN_C_ int MPI_File_get_info(MPI_File fh, MPI_Info *info_used) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_get_info(fh, info_used);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Info_dup ================== */
_EXTERN_C_ int PMPI_Info_dup(MPI_Info info, MPI_Info *newinfo);
_EXTERN_C_ int MPI_Info_dup(MPI_Info info, MPI_Info *newinfo) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Info_dup(info, newinfo);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_write_shared ================== */
_EXTERN_C_ int PMPI_File_write_shared(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
_EXTERN_C_ int MPI_File_write_shared(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_write_shared(fh, buf, count, datatype, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_iread_at ================== */
_EXTERN_C_ int PMPI_File_iread_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
_EXTERN_C_ int MPI_File_iread_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_iread_at(fh, offset, buf, count, datatype, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_write_all_end ================== */
_EXTERN_C_ int PMPI_File_write_all_end(MPI_File fh, const void *buf, MPI_Status *status);
_EXTERN_C_ int MPI_File_write_all_end(MPI_File fh, const void *buf, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_write_all_end(fh, buf, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_remote_size ================== */
_EXTERN_C_ int PMPI_Comm_remote_size(MPI_Comm comm, int *size);
_EXTERN_C_ int MPI_Comm_remote_size(MPI_Comm comm, int *size) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_remote_size(comm, size);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_get_name ================== */
_EXTERN_C_ int PMPI_Type_get_name(MPI_Datatype datatype, char *type_name, int *resultlen);
_EXTERN_C_ int MPI_Type_get_name(MPI_Datatype datatype, char *type_name, int *resultlen) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_get_name(datatype, type_name, resultlen);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_get_group ================== */
_EXTERN_C_ int PMPI_File_get_group(MPI_File fh, MPI_Group *group);
_EXTERN_C_ int MPI_File_get_group(MPI_File fh, MPI_Group *group) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_get_group(fh, group);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_preallocate ================== */
_EXTERN_C_ int PMPI_File_preallocate(MPI_File fh, MPI_Offset size);
_EXTERN_C_ int MPI_File_preallocate(MPI_File fh, MPI_Offset size) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_preallocate(fh, size);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_iread_all ================== */
_EXTERN_C_ int PMPI_File_iread_all(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
_EXTERN_C_ int MPI_File_iread_all(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_iread_all(fh, buf, count, datatype, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_cvar_write ================== */
_EXTERN_C_ int PMPI_T_cvar_write(MPI_T_cvar_handle handle, const void *buf);
_EXTERN_C_ int MPI_T_cvar_write(MPI_T_cvar_handle handle, const void *buf) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_cvar_write(handle, buf);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Group_translate_ranks ================== */
_EXTERN_C_ int PMPI_Group_translate_ranks(MPI_Group group1, int n, const int ranks1[], MPI_Group group2, int ranks2[]);
_EXTERN_C_ int MPI_Group_translate_ranks(MPI_Group group1, int n, const int ranks1[], MPI_Group group2, int ranks2[]) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Group_translate_ranks(group1, n, ranks1, group2, ranks2);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Testsome ================== */
_EXTERN_C_ int PMPI_Testsome(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[]);
_EXTERN_C_ int MPI_Testsome(int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[]) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Testsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_create_hvector ================== */
_EXTERN_C_ int PMPI_Type_create_hvector(int count, int blocklength, MPI_Aint stride, MPI_Datatype oldtype, MPI_Datatype *newtype);
_EXTERN_C_ int MPI_Type_create_hvector(int count, int blocklength, MPI_Aint stride, MPI_Datatype oldtype, MPI_Datatype *newtype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_create_hvector(count, blocklength, stride, oldtype, newtype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_get_attr ================== */
_EXTERN_C_ int PMPI_Comm_get_attr(MPI_Comm comm, int comm_keyval, void *attribute_val, int *flag);
_EXTERN_C_ int MPI_Comm_get_attr(MPI_Comm comm, int comm_keyval, void *attribute_val, int *flag) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_get_attr(comm, comm_keyval, attribute_val, flag);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Initialized ================== */
_EXTERN_C_ int PMPI_Initialized(int *flag);
_EXTERN_C_ int MPI_Initialized(int *flag) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Initialized(flag);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_create_group ================== */
_EXTERN_C_ int PMPI_Comm_create_group(MPI_Comm comm, MPI_Group group, int tag, MPI_Comm *newcomm);
_EXTERN_C_ int MPI_Comm_create_group(MPI_Comm comm, MPI_Group group, int tag, MPI_Comm *newcomm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_create_group(comm, group, tag, newcomm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Grequest_complete ================== */
_EXTERN_C_ int PMPI_Grequest_complete(MPI_Request request);
_EXTERN_C_ int MPI_Grequest_complete(MPI_Request request) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Grequest_complete(request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Get_accumulate ================== */
_EXTERN_C_ int PMPI_Get_accumulate(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, void *result_addr, int result_count, MPI_Datatype result_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win);
_EXTERN_C_ int MPI_Get_accumulate(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, void *result_addr, int result_count, MPI_Datatype result_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Get_accumulate(origin_addr, origin_count, origin_datatype, result_addr, result_count, result_datatype, target_rank, target_disp, target_count, target_datatype, op, win);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_contiguous ================== */
_EXTERN_C_ int PMPI_Type_contiguous(int count, MPI_Datatype oldtype, MPI_Datatype *newtype);
_EXTERN_C_ int MPI_Type_contiguous(int count, MPI_Datatype oldtype, MPI_Datatype *newtype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_contiguous(count, oldtype, newtype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_create_errhandler ================== */
_EXTERN_C_ int PMPI_Win_create_errhandler(MPI_Win_errhandler_function *win_errhandler_fn, MPI_Errhandler *errhandler);
_EXTERN_C_ int MPI_Win_create_errhandler(MPI_Win_errhandler_function *win_errhandler_fn, MPI_Errhandler *errhandler) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_create_errhandler(win_errhandler_fn, errhandler);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_fence ================== */
_EXTERN_C_ int PMPI_Win_fence(int assert, MPI_Win win);
_EXTERN_C_ int MPI_Win_fence(int assert, MPI_Win win) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_fence(assert, win);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_category_get_info ================== */
_EXTERN_C_ int PMPI_T_category_get_info(int cat_index, char *name, int *name_len, char *desc, int *desc_len, int *num_cvars, int *num_pvars, int *num_categories);
_EXTERN_C_ int MPI_T_category_get_info(int cat_index, char *name, int *name_len, char *desc, int *desc_len, int *num_cvars, int *num_pvars, int *num_categories) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_category_get_info(cat_index, name, name_len, desc, desc_len, num_cvars, num_pvars, num_categories);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Group_difference ================== */
_EXTERN_C_ int PMPI_Group_difference(MPI_Group group1, MPI_Group group2, MPI_Group *newgroup);
_EXTERN_C_ int MPI_Group_difference(MPI_Group group1, MPI_Group group2, MPI_Group *newgroup) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Group_difference(group1, group2, newgroup);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_write ================== */
_EXTERN_C_ int PMPI_File_write(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
_EXTERN_C_ int MPI_File_write(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_write(fh, buf, count, datatype, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_pvar_start ================== */
_EXTERN_C_ int PMPI_T_pvar_start(MPI_T_pvar_session session, MPI_T_pvar_handle handle);
_EXTERN_C_ int MPI_T_pvar_start(MPI_T_pvar_session session, MPI_T_pvar_handle handle) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_pvar_start(session, handle);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Scatterv ================== */
_EXTERN_C_ int PMPI_Scatterv(const void *sendbuf, const int *sendcounts, const int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
_EXTERN_C_ int MPI_Scatterv(const void *sendbuf, const int *sendcounts, const int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Scatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_write_all ================== */
_EXTERN_C_ int PMPI_File_write_all(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
_EXTERN_C_ int MPI_File_write_all(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_write_all(fh, buf, count, datatype, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_connect ================== */
_EXTERN_C_ int PMPI_Comm_connect(const char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm);
_EXTERN_C_ int MPI_Comm_connect(const char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_connect(port_name, info, root, comm, newcomm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_flush_all ================== */
_EXTERN_C_ int PMPI_Win_flush_all(MPI_Win win);
_EXTERN_C_ int MPI_Win_flush_all(MPI_Win win) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_flush_all(win);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_get_group ================== */
_EXTERN_C_ int PMPI_Win_get_group(MPI_Win win, MPI_Group *group);
_EXTERN_C_ int MPI_Win_get_group(MPI_Win win, MPI_Group *group) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_get_group(win, group);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_set_size ================== */
_EXTERN_C_ int PMPI_File_set_size(MPI_File fh, MPI_Offset size);
_EXTERN_C_ int MPI_File_set_size(MPI_File fh, MPI_Offset size) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_set_size(fh, size);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Status_set_cancelled ================== */
_EXTERN_C_ int PMPI_Status_set_cancelled(MPI_Status *status, int flag);
_EXTERN_C_ int MPI_Status_set_cancelled(MPI_Status *status, int flag) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Status_set_cancelled(status, flag);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Reduce_local ================== */
_EXTERN_C_ int PMPI_Reduce_local(const void *inbuf, void *inoutbuf, int count, MPI_Datatype datatype, MPI_Op op);
_EXTERN_C_ int MPI_Reduce_local(const void *inbuf, void *inoutbuf, int count, MPI_Datatype datatype, MPI_Op op) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Reduce_local(inbuf, inoutbuf, count, datatype, op);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_allocate ================== */
_EXTERN_C_ int PMPI_Win_allocate(MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, void *baseptr, MPI_Win *win);
_EXTERN_C_ int MPI_Win_allocate(MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, void *baseptr, MPI_Win *win) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Win_allocate(size, disp_unit, info, comm, baseptr, win);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Graph_neighbors ================== */
_EXTERN_C_ int PMPI_Graph_neighbors(MPI_Comm comm, int rank, int maxneighbors, int neighbors[]);
_EXTERN_C_ int MPI_Graph_neighbors(MPI_Comm comm, int rank, int maxneighbors, int neighbors[]) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Graph_neighbors(comm, rank, maxneighbors, neighbors);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Dims_create ================== */
_EXTERN_C_ int PMPI_Dims_create(int nnodes, int ndims, int dims[]);
_EXTERN_C_ int MPI_Dims_create(int nnodes, int ndims, int dims[]) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Dims_create(nnodes, ndims, dims);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Scatter ================== */
_EXTERN_C_ int PMPI_Scatter(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);
_EXTERN_C_ int MPI_Scatter(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Scatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Ireduce_scatter_block ================== */
_EXTERN_C_ int PMPI_Ireduce_scatter_block(const void *sendbuf, void *recvbuf, int recvcount, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Ireduce_scatter_block(const void *sendbuf, void *recvbuf, int recvcount, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Ireduce_scatter_block(sendbuf, recvbuf, recvcount, datatype, op, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_category_get_categories ================== */
_EXTERN_C_ int PMPI_T_category_get_categories(int cat_index, int len, int indices[]);
_EXTERN_C_ int MPI_T_category_get_categories(int cat_index, int len, int indices[]) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_category_get_categories(cat_index, len, indices);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Ialltoallw ================== */
_EXTERN_C_ int PMPI_Ialltoallw(const void *sendbuf, const int sendcounts[], const int sdispls[], const MPI_Datatype sendtypes[], void *recvbuf, const int recvcounts[], const int rdispls[], const MPI_Datatype recvtypes[], MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Ialltoallw(const void *sendbuf, const int sendcounts[], const int sdispls[], const MPI_Datatype sendtypes[], void *recvbuf, const int recvcounts[], const int rdispls[], const MPI_Datatype recvtypes[], MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Ialltoallw(sendbuf, sendcounts, sdispls, sendtypes, recvbuf, recvcounts, rdispls, recvtypes, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Ialltoallv ================== */
_EXTERN_C_ int PMPI_Ialltoallv(const void *sendbuf, const int sendcounts[], const int sdispls[], MPI_Datatype sendtype, void *recvbuf, const int recvcounts[], const int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Ialltoallv(const void *sendbuf, const int sendcounts[], const int sdispls[], MPI_Datatype sendtype, void *recvbuf, const int recvcounts[], const int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Ialltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_free ================== */
_EXTERN_C_ int PMPI_Comm_free(MPI_Comm *comm);
_EXTERN_C_ int MPI_Comm_free(MPI_Comm *comm) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Comm_free(comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_call_errhandler ================== */
_EXTERN_C_ int PMPI_Comm_call_errhandler(MPI_Comm comm, int errorcode);
_EXTERN_C_ int MPI_Comm_call_errhandler(MPI_Comm comm, int errorcode) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_call_errhandler(comm, errorcode);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Neighbor_alltoall ================== */
_EXTERN_C_ int PMPI_Neighbor_alltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
_EXTERN_C_ int MPI_Neighbor_alltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Neighbor_alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Alltoall ================== */
_EXTERN_C_ int PMPI_Alltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
_EXTERN_C_ int MPI_Alltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_spawn_multiple ================== */
_EXTERN_C_ int PMPI_Comm_spawn_multiple(int count, char *array_of_commands[], char **array_of_argv[], const int array_of_maxprocs[], const MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]);
_EXTERN_C_ int MPI_Comm_spawn_multiple(int count, char *array_of_commands[], char **array_of_argv[], const int array_of_maxprocs[], const MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[]) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_spawn_multiple(count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm, intercomm, array_of_errcodes);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_free_keyval ================== */
_EXTERN_C_ int PMPI_Type_free_keyval(int *type_keyval);
_EXTERN_C_ int MPI_Type_free_keyval(int *type_keyval) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_free_keyval(type_keyval);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Reduce_scatter_block ================== */
_EXTERN_C_ int PMPI_Reduce_scatter_block(const void *sendbuf, void *recvbuf, int recvcount, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
_EXTERN_C_ int MPI_Reduce_scatter_block(const void *sendbuf, void *recvbuf, int recvcount, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Reduce_scatter_block(sendbuf, recvbuf, recvcount, datatype, op, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_pvar_handle_free ================== */
_EXTERN_C_ int PMPI_T_pvar_handle_free(MPI_T_pvar_session session, MPI_T_pvar_handle *handle);
_EXTERN_C_ int MPI_T_pvar_handle_free(MPI_T_pvar_session session, MPI_T_pvar_handle *handle) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_pvar_handle_free(session, handle);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_wait ================== */
_EXTERN_C_ int PMPI_Win_wait(MPI_Win win);
_EXTERN_C_ int MPI_Win_wait(MPI_Win win) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_wait(win);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_test ================== */
_EXTERN_C_ int PMPI_Win_test(MPI_Win win, int *flag);
_EXTERN_C_ int MPI_Win_test(MPI_Win win, int *flag) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_test(win, flag);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_write_at_all_begin ================== */
_EXTERN_C_ int PMPI_File_write_at_all_begin(MPI_File fh, MPI_Offset offset, const void *buf, int count, MPI_Datatype datatype);
_EXTERN_C_ int MPI_File_write_at_all_begin(MPI_File fh, MPI_Offset offset, const void *buf, int count, MPI_Datatype datatype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_write_at_all_begin(fh, offset, buf, count, datatype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Get_elements ================== */
_EXTERN_C_ int PMPI_Get_elements(const MPI_Status *status, MPI_Datatype datatype, int *count);
_EXTERN_C_ int MPI_Get_elements(const MPI_Status *status, MPI_Datatype datatype, int *count) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Get_elements(status, datatype, count);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_get_errhandler ================== */
_EXTERN_C_ int PMPI_Comm_get_errhandler(MPI_Comm comm, MPI_Errhandler *errhandler);
_EXTERN_C_ int MPI_Comm_get_errhandler(MPI_Comm comm, MPI_Errhandler *errhandler) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_get_errhandler(comm, errhandler);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Mprobe ================== */
_EXTERN_C_ int PMPI_Mprobe(int source, int tag, MPI_Comm comm, MPI_Message *message, MPI_Status *status);
_EXTERN_C_ int MPI_Mprobe(int source, int tag, MPI_Comm comm, MPI_Message *message, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Mprobe(source, tag, comm, message, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Info_get_nkeys ================== */
_EXTERN_C_ int PMPI_Info_get_nkeys(MPI_Info info, int *nkeys);
_EXTERN_C_ int MPI_Info_get_nkeys(MPI_Info info, int *nkeys) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Info_get_nkeys(info, nkeys);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Finalized ================== */
_EXTERN_C_ int PMPI_Finalized(int *flag);
_EXTERN_C_ int MPI_Finalized(int *flag) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Finalized(flag);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Get_elements_x ================== */
_EXTERN_C_ int PMPI_Get_elements_x(const MPI_Status *status, MPI_Datatype datatype, MPI_Count *count);
_EXTERN_C_ int MPI_Get_elements_x(const MPI_Status *status, MPI_Datatype datatype, MPI_Count *count) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Get_elements_x(status, datatype, count);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Op_commutative ================== */
_EXTERN_C_ int PMPI_Op_commutative(MPI_Op op, int *commute);
_EXTERN_C_ int MPI_Op_commutative(MPI_Op op, int *commute) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Op_commutative(op, commute);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Dist_graph_create_adjacent ================== */
_EXTERN_C_ int PMPI_Dist_graph_create_adjacent(MPI_Comm comm_old, int indegree, const int sources[], const int sourceweights[], int outdegree, const int destinations[], const int destweights[], MPI_Info info, int reorder, MPI_Comm *comm_dist_graph);
_EXTERN_C_ int MPI_Dist_graph_create_adjacent(MPI_Comm comm_old, int indegree, const int sources[], const int sourceweights[], int outdegree, const int destinations[], const int destweights[], MPI_Info info, int reorder, MPI_Comm *comm_dist_graph) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm_old);

  _wrap_py_return_val = PMPI_Dist_graph_create_adjacent(comm_old, indegree, sources, sourceweights, outdegree, destinations, destweights, info, reorder, comm_dist_graph);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Abort ================== */
_EXTERN_C_ int PMPI_Abort(MPI_Comm comm, int errorcode);
_EXTERN_C_ int MPI_Abort(MPI_Comm comm, int errorcode) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Abort(comm, errorcode);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Info_delete ================== */
_EXTERN_C_ int PMPI_Info_delete(MPI_Info info, const char *key);
_EXTERN_C_ int MPI_Info_delete(MPI_Info info, const char *key) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Info_delete(info, key);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_iread_at_all ================== */
_EXTERN_C_ int PMPI_File_iread_at_all(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
_EXTERN_C_ int MPI_File_iread_at_all(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_iread_at_all(fh, offset, buf, count, datatype, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Cart_rank ================== */
_EXTERN_C_ int PMPI_Cart_rank(MPI_Comm comm, const int coords[], int *rank);
_EXTERN_C_ int MPI_Cart_rank(MPI_Comm comm, const int coords[], int *rank) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Cart_rank(comm, coords, rank);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_get_name ================== */
_EXTERN_C_ int PMPI_Win_get_name(MPI_Win win, char *win_name, int *resultlen);
_EXTERN_C_ int MPI_Win_get_name(MPI_Win win, char *win_name, int *resultlen) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_get_name(win, win_name, resultlen);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Iallreduce ================== */
_EXTERN_C_ int PMPI_Iallreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Iallreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Iallreduce(sendbuf, recvbuf, count, datatype, op, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_size ================== */
_EXTERN_C_ int PMPI_Type_size(MPI_Datatype datatype, int *size);
_EXTERN_C_ int MPI_Type_size(MPI_Datatype datatype, int *size) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_size(datatype, size);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Fetch_and_op ================== */
_EXTERN_C_ int PMPI_Fetch_and_op(const void *origin_addr, void *result_addr, MPI_Datatype datatype, int target_rank, MPI_Aint target_disp, MPI_Op op, MPI_Win win);
_EXTERN_C_ int MPI_Fetch_and_op(const void *origin_addr, void *result_addr, MPI_Datatype datatype, int target_rank, MPI_Aint target_disp, MPI_Op op, MPI_Win win) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Fetch_and_op(origin_addr, result_addr, datatype, target_rank, target_disp, op, win);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Rget ================== */
_EXTERN_C_ int PMPI_Rget(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win, MPI_Request *request);
_EXTERN_C_ int MPI_Rget(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Rget(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_set_attr ================== */
_EXTERN_C_ int PMPI_Win_set_attr(MPI_Win win, int win_keyval, void *attribute_val);
_EXTERN_C_ int MPI_Win_set_attr(MPI_Win win, int win_keyval, void *attribute_val) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_set_attr(win, win_keyval, attribute_val);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_create_f90_integer ================== */
_EXTERN_C_ int PMPI_Type_create_f90_integer(int range, MPI_Datatype *newtype);
_EXTERN_C_ int MPI_Type_create_f90_integer(int range, MPI_Datatype *newtype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_create_f90_integer(range, newtype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Iscatterv ================== */
_EXTERN_C_ int PMPI_Iscatterv(const void *sendbuf, const int sendcounts[], const int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Iscatterv(const void *sendbuf, const int sendcounts[], const int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Iscatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_delete ================== */
_EXTERN_C_ int PMPI_File_delete(const char *filename, MPI_Info info);
_EXTERN_C_ int MPI_File_delete(const char *filename, MPI_Info info) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_delete(filename, info);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_read_all ================== */
_EXTERN_C_ int PMPI_File_read_all(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
_EXTERN_C_ int MPI_File_read_all(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_read_all(fh, buf, count, datatype, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Group_size ================== */
_EXTERN_C_ int PMPI_Group_size(MPI_Group group, int *size);
_EXTERN_C_ int MPI_Group_size(MPI_Group group, int *size) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Group_size(group, size);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Attr_put ================== */
_EXTERN_C_ int PMPI_Attr_put(MPI_Comm comm, int keyval, void *attribute_val);
_EXTERN_C_ int MPI_Attr_put(MPI_Comm comm, int keyval, void *attribute_val) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Attr_put(comm, keyval, attribute_val);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Iscatter ================== */
_EXTERN_C_ int PMPI_Iscatter(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Iscatter(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Iscatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_start ================== */
_EXTERN_C_ int PMPI_Win_start(MPI_Group group, int assert, MPI_Win win);
_EXTERN_C_ int MPI_Win_start(MPI_Group group, int assert, MPI_Win win) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_start(group, assert, win);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_free ================== */
_EXTERN_C_ int PMPI_Win_free(MPI_Win *win);
_EXTERN_C_ int MPI_Win_free(MPI_Win *win) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_free(win);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Alltoallw ================== */
_EXTERN_C_ int PMPI_Alltoallw(const void *sendbuf, const int sendcounts[], const int sdispls[], const MPI_Datatype sendtypes[], void *recvbuf, const int recvcounts[], const int rdispls[], const MPI_Datatype recvtypes[], MPI_Comm comm);
_EXTERN_C_ int MPI_Alltoallw(const void *sendbuf, const int sendcounts[], const int sdispls[], const MPI_Datatype sendtypes[], void *recvbuf, const int recvcounts[], const int rdispls[], const MPI_Datatype recvtypes[], MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Alltoallw(sendbuf, sendcounts, sdispls, sendtypes, recvbuf, recvcounts, rdispls, recvtypes, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Alltoallv ================== */
_EXTERN_C_ int PMPI_Alltoallv(const void *sendbuf, const int *sendcounts, const int *sdispls, MPI_Datatype sendtype, void *recvbuf, const int *recvcounts, const int *rdispls, MPI_Datatype recvtype, MPI_Comm comm);
_EXTERN_C_ int MPI_Alltoallv(const void *sendbuf, const int *sendcounts, const int *sdispls, MPI_Datatype sendtype, void *recvbuf, const int *recvcounts, const int *rdispls, MPI_Datatype recvtype, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Alltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Exscan ================== */
_EXTERN_C_ int PMPI_Exscan(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
_EXTERN_C_ int MPI_Exscan(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Exscan(sendbuf, recvbuf, count, datatype, op, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Op_free ================== */
_EXTERN_C_ int PMPI_Op_free(MPI_Op *op);
_EXTERN_C_ int MPI_Op_free(MPI_Op *op) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Op_free(op);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Iscan ================== */
_EXTERN_C_ int PMPI_Iscan(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Iscan(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Iscan(sendbuf, recvbuf, count, datatype, op, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Keyval_create ================== */
_EXTERN_C_ int PMPI_Keyval_create(MPI_Copy_function *copy_fn, MPI_Delete_function *delete_fn, int *keyval, void *extra_state);
_EXTERN_C_ int MPI_Keyval_create(MPI_Copy_function *copy_fn, MPI_Delete_function *delete_fn, int *keyval, void *extra_state) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Keyval_create(copy_fn, delete_fn, keyval, extra_state);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_vector ================== */
_EXTERN_C_ int PMPI_Type_vector(int count, int blocklength, int stride, MPI_Datatype oldtype, MPI_Datatype *newtype);
_EXTERN_C_ int MPI_Type_vector(int count, int blocklength, int stride, MPI_Datatype oldtype, MPI_Datatype *newtype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_vector(count, blocklength, stride, oldtype, newtype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_create_keyval ================== */
_EXTERN_C_ int PMPI_Win_create_keyval(MPI_Win_copy_attr_function *win_copy_attr_fn, MPI_Win_delete_attr_function *win_delete_attr_fn, int *win_keyval, void *extra_state);
_EXTERN_C_ int MPI_Win_create_keyval(MPI_Win_copy_attr_function *win_copy_attr_fn, MPI_Win_delete_attr_function *win_delete_attr_fn, int *win_keyval, void *extra_state) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_create_keyval(win_copy_attr_fn, win_delete_attr_fn, win_keyval, extra_state);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_match_size ================== */
_EXTERN_C_ int PMPI_Type_match_size(int typeclass, int size, MPI_Datatype *datatype);
_EXTERN_C_ int MPI_Type_match_size(int typeclass, int size, MPI_Datatype *datatype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_match_size(typeclass, size, datatype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Scan ================== */
_EXTERN_C_ int PMPI_Scan(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
_EXTERN_C_ int MPI_Scan(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Scan(sendbuf, recvbuf, count, datatype, op, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Startall ================== */
_EXTERN_C_ int PMPI_Startall(int count, MPI_Request array_of_requests[]);
_EXTERN_C_ int MPI_Startall(int count, MPI_Request array_of_requests[]) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Startall(count, array_of_requests);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_seek_shared ================== */
_EXTERN_C_ int PMPI_File_seek_shared(MPI_File fh, MPI_Offset offset, int whence);
_EXTERN_C_ int MPI_File_seek_shared(MPI_File fh, MPI_Offset offset, int whence) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_seek_shared(fh, offset, whence);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Wtime ================== */
_EXTERN_C_ double PMPI_Wtime();
_EXTERN_C_ double MPI_Wtime() { 
    double _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Wtime();
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Add_error_class ================== */
_EXTERN_C_ int PMPI_Add_error_class(int *errorclass);
_EXTERN_C_ int MPI_Add_error_class(int *errorclass) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Add_error_class(errorclass);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Igather ================== */
_EXTERN_C_ int PMPI_Igather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Igather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Igather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Info_free ================== */
_EXTERN_C_ int PMPI_Info_free(MPI_Info *info);
_EXTERN_C_ int MPI_Info_free(MPI_Info *info) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Info_free(info);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Open_port ================== */
_EXTERN_C_ int PMPI_Open_port(MPI_Info info, char *port_name);
_EXTERN_C_ int MPI_Open_port(MPI_Info info, char *port_name) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Open_port(info, port_name);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_pvar_write ================== */
_EXTERN_C_ int PMPI_T_pvar_write(MPI_T_pvar_session session, MPI_T_pvar_handle handle, const void *buf);
_EXTERN_C_ int MPI_T_pvar_write(MPI_T_pvar_session session, MPI_T_pvar_handle handle, const void *buf) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_pvar_write(session, handle, buf);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_set_info ================== */
_EXTERN_C_ int PMPI_Win_set_info(MPI_Win win, MPI_Info info);
_EXTERN_C_ int MPI_Win_set_info(MPI_Win win, MPI_Info info) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_set_info(win, info);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Unpublish_name ================== */
_EXTERN_C_ int PMPI_Unpublish_name(const char *service_name, MPI_Info info, const char *port_name);
_EXTERN_C_ int MPI_Unpublish_name(const char *service_name, MPI_Info info, const char *port_name) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Unpublish_name(service_name, info, port_name);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Group_rank ================== */
_EXTERN_C_ int PMPI_Group_rank(MPI_Group group, int *rank);
_EXTERN_C_ int MPI_Group_rank(MPI_Group group, int *rank) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Group_rank(group, rank);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Lookup_name ================== */
_EXTERN_C_ int PMPI_Lookup_name(const char *service_name, MPI_Info info, char *port_name);
_EXTERN_C_ int MPI_Lookup_name(const char *service_name, MPI_Info info, char *port_name) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Lookup_name(service_name, info, port_name);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Recv ================== */
_EXTERN_C_ int PMPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status);
_EXTERN_C_ int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Recv(buf, count, datatype, source, tag, comm, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_get_info ================== */
_EXTERN_C_ int PMPI_Comm_get_info(MPI_Comm comm, MPI_Info *info);
_EXTERN_C_ int MPI_Comm_get_info(MPI_Comm comm, MPI_Info *info) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_get_info(comm, info);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_pvar_session_create ================== */
_EXTERN_C_ int PMPI_T_pvar_session_create(MPI_T_pvar_session *session);
_EXTERN_C_ int MPI_T_pvar_session_create(MPI_T_pvar_session *session) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_pvar_session_create(session);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Pack ================== */
_EXTERN_C_ int PMPI_Pack(const void *inbuf, int incount, MPI_Datatype datatype, void *outbuf, int outsize, int *position, MPI_Comm comm);
_EXTERN_C_ int MPI_Pack(const void *inbuf, int incount, MPI_Datatype datatype, void *outbuf, int outsize, int *position, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Pack(inbuf, incount, datatype, outbuf, outsize, position, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_open ================== */
_EXTERN_C_ int PMPI_File_open(MPI_Comm comm, const char *filename, int amode, MPI_Info info, MPI_File *fh);
_EXTERN_C_ int MPI_File_open(MPI_Comm comm, const char *filename, int amode, MPI_Info info, MPI_File *fh) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_File_open(comm, filename, amode, info, fh);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Bsend ================== */
_EXTERN_C_ int PMPI_Bsend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
_EXTERN_C_ int MPI_Bsend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Bsend(buf, count, datatype, dest, tag, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Group_excl ================== */
_EXTERN_C_ int PMPI_Group_excl(MPI_Group group, int n, const int ranks[], MPI_Group *newgroup);
_EXTERN_C_ int MPI_Group_excl(MPI_Group group, int n, const int ranks[], MPI_Group *newgroup) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Group_excl(group, n, ranks, newgroup);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Ireduce_scatter ================== */
_EXTERN_C_ int PMPI_Ireduce_scatter(const void *sendbuf, void *recvbuf, const int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Ireduce_scatter(const void *sendbuf, void *recvbuf, const int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Ireduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_read ================== */
_EXTERN_C_ int PMPI_File_read(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
_EXTERN_C_ int MPI_File_read(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_read(fh, buf, count, datatype, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Issend ================== */
_EXTERN_C_ int PMPI_Issend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Issend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Issend(buf, count, datatype, dest, tag, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_iwrite_at_all ================== */
_EXTERN_C_ int PMPI_File_iwrite_at_all(MPI_File fh, MPI_Offset offset, const void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
_EXTERN_C_ int MPI_File_iwrite_at_all(MPI_File fh, MPI_Offset offset, const void *buf, int count, MPI_Datatype datatype, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_iwrite_at_all(fh, offset, buf, count, datatype, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_cvar_read ================== */
_EXTERN_C_ int PMPI_T_cvar_read(MPI_T_cvar_handle handle, void *buf);
_EXTERN_C_ int MPI_T_cvar_read(MPI_T_cvar_handle handle, void *buf) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_cvar_read(handle, buf);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_init_thread ================== */
_EXTERN_C_ int PMPI_T_init_thread(int required, int *provided);
_EXTERN_C_ int MPI_T_init_thread(int required, int *provided) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_init_thread(required, provided);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_iwrite ================== */
_EXTERN_C_ int PMPI_File_iwrite(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
_EXTERN_C_ int MPI_File_iwrite(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_iwrite(fh, buf, count, datatype, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Gatherv ================== */
_EXTERN_C_ int PMPI_Gatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int *recvcounts, const int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm);
_EXTERN_C_ int MPI_Gatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, const int *recvcounts, const int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Gatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_create ================== */
_EXTERN_C_ int PMPI_Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm);
_EXTERN_C_ int MPI_Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_create(comm, group, newcomm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_write_ordered ================== */
_EXTERN_C_ int PMPI_File_write_ordered(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
_EXTERN_C_ int MPI_File_write_ordered(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_write_ordered(fh, buf, count, datatype, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_dup ================== */
_EXTERN_C_ int PMPI_Comm_dup(MPI_Comm comm, MPI_Comm *newcomm);
_EXTERN_C_ int MPI_Comm_dup(MPI_Comm comm, MPI_Comm *newcomm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_dup(comm, newcomm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_finalize ================== */
_EXTERN_C_ int PMPI_T_finalize();
_EXTERN_C_ int MPI_T_finalize() { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_finalize();
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_iread_shared ================== */
_EXTERN_C_ int PMPI_File_iread_shared(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
_EXTERN_C_ int MPI_File_iread_shared(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_iread_shared(fh, buf, count, datatype, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_set_errhandler ================== */
_EXTERN_C_ int PMPI_File_set_errhandler(MPI_File file, MPI_Errhandler errhandler);
_EXTERN_C_ int MPI_File_set_errhandler(MPI_File file, MPI_Errhandler errhandler) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_set_errhandler(file, errhandler);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Register_datarep ================== */
_EXTERN_C_ int PMPI_Register_datarep(const char *datarep, MPI_Datarep_conversion_function *read_conversion_fn, MPI_Datarep_conversion_function *write_conversion_fn, MPI_Datarep_extent_function *dtype_file_extent_fn, void *extra_state);
_EXTERN_C_ int MPI_Register_datarep(const char *datarep, MPI_Datarep_conversion_function *read_conversion_fn, MPI_Datarep_conversion_function *write_conversion_fn, MPI_Datarep_extent_function *dtype_file_extent_fn, void *extra_state) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Register_datarep(datarep, read_conversion_fn, write_conversion_fn, dtype_file_extent_fn, extra_state);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Reduce_scatter ================== */
_EXTERN_C_ int PMPI_Reduce_scatter(const void *sendbuf, void *recvbuf, const int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
_EXTERN_C_ int MPI_Reduce_scatter(const void *sendbuf, void *recvbuf, const int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Reduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Ibarrier ================== */
_EXTERN_C_ int PMPI_Ibarrier(MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Ibarrier(MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Ibarrier(comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_create_f90_real ================== */
_EXTERN_C_ int PMPI_Type_create_f90_real(int precision, int range, MPI_Datatype *newtype);
_EXTERN_C_ int MPI_Type_create_f90_real(int precision, int range, MPI_Datatype *newtype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_create_f90_real(precision, range, newtype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_set_name ================== */
_EXTERN_C_ int PMPI_Type_set_name(MPI_Datatype datatype, const char *type_name);
_EXTERN_C_ int MPI_Type_set_name(MPI_Datatype datatype, const char *type_name) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_set_name(datatype, type_name);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_cvar_get_num ================== */
_EXTERN_C_ int PMPI_T_cvar_get_num(int *num_cvar);
_EXTERN_C_ int MPI_T_cvar_get_num(int *num_cvar) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_cvar_get_num(num_cvar);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Group_incl ================== */
_EXTERN_C_ int PMPI_Group_incl(MPI_Group group, int n, const int ranks[], MPI_Group *newgroup);
_EXTERN_C_ int MPI_Group_incl(MPI_Group group, int n, const int ranks[], MPI_Group *newgroup) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Group_incl(group, n, ranks, newgroup);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Get_version ================== */
_EXTERN_C_ int PMPI_Get_version(int *version, int *subversion);
_EXTERN_C_ int MPI_Get_version(int *version, int *subversion) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Get_version(version, subversion);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Pack_external ================== */
_EXTERN_C_ int PMPI_Pack_external(const char datarep[], const void *inbuf, int incount, MPI_Datatype datatype, void *outbuf, MPI_Aint outsize, MPI_Aint *position);
_EXTERN_C_ int MPI_Pack_external(const char datarep[], const void *inbuf, int incount, MPI_Datatype datatype, void *outbuf, MPI_Aint outsize, MPI_Aint *position) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Pack_external(datarep, inbuf, incount, datatype, outbuf, outsize, position);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Irecv ================== */
_EXTERN_C_ int PMPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Irecv(buf, count, datatype, source, tag, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_get_true_extent ================== */
_EXTERN_C_ int PMPI_Type_get_true_extent(MPI_Datatype datatype, MPI_Aint *true_lb, MPI_Aint *true_extent);
_EXTERN_C_ int MPI_Type_get_true_extent(MPI_Datatype datatype, MPI_Aint *true_lb, MPI_Aint *true_extent) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_get_true_extent(datatype, true_lb, true_extent);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Errhandler_create ================== */
_EXTERN_C_ int PMPI_Errhandler_create(MPI_Handler_function *function, MPI_Errhandler *errhandler);
_EXTERN_C_ int MPI_Errhandler_create(MPI_Handler_function *function, MPI_Errhandler *errhandler) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Errhandler_create(function, errhandler);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Cart_create ================== */
_EXTERN_C_ int PMPI_Cart_create(MPI_Comm comm_old, int ndims, const int dims[], const int periods[], int reorder, MPI_Comm *comm_cart);
_EXTERN_C_ int MPI_Cart_create(MPI_Comm comm_old, int ndims, const int dims[], const int periods[], int reorder, MPI_Comm *comm_cart) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm_old);

  _wrap_py_return_val = PMPI_Cart_create(comm_old, ndims, dims, periods, reorder, comm_cart);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Sendrecv ================== */
_EXTERN_C_ int PMPI_Sendrecv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status);
_EXTERN_C_ int MPI_Sendrecv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_shared_query ================== */
_EXTERN_C_ int PMPI_Win_shared_query(MPI_Win win, int rank, MPI_Aint *size, int *disp_unit, void *baseptr);
_EXTERN_C_ int MPI_Win_shared_query(MPI_Win win, int rank, MPI_Aint *size, int *disp_unit, void *baseptr) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_shared_query(win, rank, size, disp_unit, baseptr);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_set_attr ================== */
_EXTERN_C_ int PMPI_Comm_set_attr(MPI_Comm comm, int comm_keyval, void *attribute_val);
_EXTERN_C_ int MPI_Comm_set_attr(MPI_Comm comm, int comm_keyval, void *attribute_val) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_set_attr(comm, comm_keyval, attribute_val);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_allocate_shared ================== */
_EXTERN_C_ int PMPI_Win_allocate_shared(MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, void *baseptr, MPI_Win *win);
_EXTERN_C_ int MPI_Win_allocate_shared(MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, void *baseptr, MPI_Win *win) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Win_allocate_shared(size, disp_unit, info, comm, baseptr, win);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_get_true_extent_x ================== */
_EXTERN_C_ int PMPI_Type_get_true_extent_x(MPI_Datatype datatype, MPI_Count *lb, MPI_Count *extent);
_EXTERN_C_ int MPI_Type_get_true_extent_x(MPI_Datatype datatype, MPI_Count *lb, MPI_Count *extent) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_get_true_extent_x(datatype, lb, extent);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Bcast ================== */
_EXTERN_C_ int PMPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm);
_EXTERN_C_ int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Bcast(buffer, count, datatype, root, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Get_processor_name ================== */
_EXTERN_C_ int PMPI_Get_processor_name(char *name, int *resultlen);
_EXTERN_C_ int MPI_Get_processor_name(char *name, int *resultlen) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Get_processor_name(name, resultlen);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Info_set ================== */
_EXTERN_C_ int PMPI_Info_set(MPI_Info info, const char *key, const char *value);
_EXTERN_C_ int MPI_Info_set(MPI_Info info, const char *key, const char *value) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Info_set(info, key, value);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Ineighbor_allgather ================== */
_EXTERN_C_ int PMPI_Ineighbor_allgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Ineighbor_allgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Ineighbor_allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Raccumulate ================== */
_EXTERN_C_ int PMPI_Raccumulate(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win, MPI_Request *request);
_EXTERN_C_ int MPI_Raccumulate(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Raccumulate(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, op, win, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Pack_size ================== */
_EXTERN_C_ int PMPI_Pack_size(int incount, MPI_Datatype datatype, MPI_Comm comm, int *size);
_EXTERN_C_ int MPI_Pack_size(int incount, MPI_Datatype datatype, MPI_Comm comm, int *size) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Pack_size(incount, datatype, comm, size);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Intercomm_merge ================== */
_EXTERN_C_ int PMPI_Intercomm_merge(MPI_Comm intercomm, int high, MPI_Comm *newintracomm);
_EXTERN_C_ int MPI_Intercomm_merge(MPI_Comm intercomm, int high, MPI_Comm *newintracomm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(intercomm);

  _wrap_py_return_val = PMPI_Intercomm_merge(intercomm, high, newintracomm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_get_type_extent ================== */
_EXTERN_C_ int PMPI_File_get_type_extent(MPI_File fh, MPI_Datatype datatype, MPI_Aint *extent);
_EXTERN_C_ int MPI_File_get_type_extent(MPI_File fh, MPI_Datatype datatype, MPI_Aint *extent) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_get_type_extent(fh, datatype, extent);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Get_library_version ================== */
_EXTERN_C_ int PMPI_Get_library_version(char *version, int *resultlen);
_EXTERN_C_ int MPI_Get_library_version(char *version, int *resultlen) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Get_library_version(version, resultlen);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Isend ================== */
_EXTERN_C_ int PMPI_Isend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Isend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Isend(buf, count, datatype, dest, tag, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_pvar_reset ================== */
_EXTERN_C_ int PMPI_T_pvar_reset(MPI_T_pvar_session session, MPI_T_pvar_handle handle);
_EXTERN_C_ int MPI_T_pvar_reset(MPI_T_pvar_session session, MPI_T_pvar_handle handle) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_pvar_reset(session, handle);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Start ================== */
_EXTERN_C_ int PMPI_Start(MPI_Request *request);
_EXTERN_C_ int MPI_Start(MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Start(request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Unpack ================== */
_EXTERN_C_ int PMPI_Unpack(const void *inbuf, int insize, int *position, void *outbuf, int outcount, MPI_Datatype datatype, MPI_Comm comm);
_EXTERN_C_ int MPI_Unpack(const void *inbuf, int insize, int *position, void *outbuf, int outcount, MPI_Datatype datatype, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Unpack(inbuf, insize, position, outbuf, outcount, datatype, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_get_view ================== */
_EXTERN_C_ int PMPI_File_get_view(MPI_File fh, MPI_Offset *disp, MPI_Datatype *etype, MPI_Datatype *filetype, char *datarep);
_EXTERN_C_ int MPI_File_get_view(MPI_File fh, MPI_Offset *disp, MPI_Datatype *etype, MPI_Datatype *filetype, char *datarep) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_get_view(fh, disp, etype, filetype, datarep);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_pvar_session_free ================== */
_EXTERN_C_ int PMPI_T_pvar_session_free(MPI_T_pvar_session *session);
_EXTERN_C_ int MPI_T_pvar_session_free(MPI_T_pvar_session *session) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_pvar_session_free(session);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Iexscan ================== */
_EXTERN_C_ int PMPI_Iexscan(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Iexscan(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Iexscan(sendbuf, recvbuf, count, datatype, op, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Unpack_external ================== */
_EXTERN_C_ int PMPI_Unpack_external(const char datarep[], const void *inbuf, MPI_Aint insize, MPI_Aint *position, void *outbuf, int outcount, MPI_Datatype datatype);
_EXTERN_C_ int MPI_Unpack_external(const char datarep[], const void *inbuf, MPI_Aint insize, MPI_Aint *position, void *outbuf, int outcount, MPI_Datatype datatype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Unpack_external(datarep, inbuf, insize, position, outbuf, outcount, datatype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Errhandler_set ================== */
_EXTERN_C_ int PMPI_Errhandler_set(MPI_Comm comm, MPI_Errhandler errhandler);
_EXTERN_C_ int MPI_Errhandler_set(MPI_Comm comm, MPI_Errhandler errhandler) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Errhandler_set(comm, errhandler);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_unlock ================== */
_EXTERN_C_ int PMPI_Win_unlock(int rank, MPI_Win win);
_EXTERN_C_ int MPI_Win_unlock(int rank, MPI_Win win) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_unlock(rank, win);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Test ================== */
_EXTERN_C_ int PMPI_Test(MPI_Request *request, int *flag, MPI_Status *status);
_EXTERN_C_ int MPI_Test(MPI_Request *request, int *flag, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Test(request, flag, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Request_get_status ================== */
_EXTERN_C_ int PMPI_Request_get_status(MPI_Request request, int *flag, MPI_Status *status);
_EXTERN_C_ int MPI_Request_get_status(MPI_Request request, int *flag, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Request_get_status(request, flag, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Waitany ================== */
_EXTERN_C_ int PMPI_Waitany(int count, MPI_Request array_of_requests[], int *indx, MPI_Status *status);
_EXTERN_C_ int MPI_Waitany(int count, MPI_Request array_of_requests[], int *indx, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Waitany(count, array_of_requests, indx, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_read_at_all_end ================== */
_EXTERN_C_ int PMPI_File_read_at_all_end(MPI_File fh, void *buf, MPI_Status *status);
_EXTERN_C_ int MPI_File_read_at_all_end(MPI_File fh, void *buf, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_read_at_all_end(fh, buf, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_set_view ================== */
_EXTERN_C_ int PMPI_File_set_view(MPI_File fh, MPI_Offset disp, MPI_Datatype etype, MPI_Datatype filetype, const char *datarep, MPI_Info info);
_EXTERN_C_ int MPI_File_set_view(MPI_File fh, MPI_Offset disp, MPI_Datatype etype, MPI_Datatype filetype, const char *datarep, MPI_Info info) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_set_view(fh, disp, etype, filetype, datarep, info);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Type_ub ================== */
_EXTERN_C_ int PMPI_Type_ub(MPI_Datatype datatype, MPI_Aint *displacement);
_EXTERN_C_ int MPI_Type_ub(MPI_Datatype datatype, MPI_Aint *displacement) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Type_ub(datatype, displacement);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Status_set_elements ================== */
_EXTERN_C_ int PMPI_Status_set_elements(MPI_Status *status, MPI_Datatype datatype, int count);
_EXTERN_C_ int MPI_Status_set_elements(MPI_Status *status, MPI_Datatype datatype, int count) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Status_set_elements(status, datatype, count);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Pcontrol ================== */
_EXTERN_C_ int PMPI_Pcontrol(const int level, ...);
_EXTERN_C_ int MPI_Pcontrol(const int level, ...) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Pcontrol(level);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Group_range_incl ================== */
_EXTERN_C_ int PMPI_Group_range_incl(MPI_Group group, int n, int ranges[][3], MPI_Group *newgroup);
_EXTERN_C_ int MPI_Group_range_incl(MPI_Group group, int n, int ranges[][3], MPI_Group *newgroup) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Group_range_incl(group, n, ranges, newgroup);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Get ================== */
_EXTERN_C_ int PMPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
_EXTERN_C_ int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Get(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Iprobe ================== */
_EXTERN_C_ int PMPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status);
_EXTERN_C_ int MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Iprobe(source, tag, comm, flag, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_join ================== */
_EXTERN_C_ int PMPI_Comm_join(int fd, MPI_Comm *intercomm);
_EXTERN_C_ int MPI_Comm_join(int fd, MPI_Comm *intercomm) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Comm_join(fd, intercomm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_read_shared ================== */
_EXTERN_C_ int PMPI_File_read_shared(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
_EXTERN_C_ int MPI_File_read_shared(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_read_shared(fh, buf, count, datatype, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Win_detach ================== */
_EXTERN_C_ int PMPI_Win_detach(MPI_Win win, const void *base);
_EXTERN_C_ int MPI_Win_detach(MPI_Win win, const void *base) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Win_detach(win, base);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Improbe ================== */
_EXTERN_C_ int PMPI_Improbe(int source, int tag, MPI_Comm comm, int *flag, MPI_Message *message, MPI_Status *status);
_EXTERN_C_ int MPI_Improbe(int source, int tag, MPI_Comm comm, int *flag, MPI_Message *message, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Improbe(source, tag, comm, flag, message, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Ssend ================== */
_EXTERN_C_ int PMPI_Ssend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
_EXTERN_C_ int MPI_Ssend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Ssend(buf, count, datatype, dest, tag, comm);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Ineighbor_alltoallw ================== */
_EXTERN_C_ int PMPI_Ineighbor_alltoallw(const void *sendbuf, const int sendcounts[], const MPI_Aint sdispls[], const MPI_Datatype sendtypes[], void *recvbuf, const int recvcounts[], const MPI_Aint rdispls[], const MPI_Datatype recvtypes[], MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Ineighbor_alltoallw(const void *sendbuf, const int sendcounts[], const MPI_Aint sdispls[], const MPI_Datatype sendtypes[], void *recvbuf, const int recvcounts[], const MPI_Aint rdispls[], const MPI_Datatype recvtypes[], MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Ineighbor_alltoallw(sendbuf, sendcounts, sdispls, sendtypes, recvbuf, recvcounts, rdispls, recvtypes, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Ineighbor_alltoallv ================== */
_EXTERN_C_ int PMPI_Ineighbor_alltoallv(const void *sendbuf, const int sendcounts[], const int sdispls[], MPI_Datatype sendtype, void *recvbuf, const int recvcounts[], const int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Ineighbor_alltoallv(const void *sendbuf, const int sendcounts[], const int sdispls[], MPI_Datatype sendtype, void *recvbuf, const int recvcounts[], const int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Ineighbor_alltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Wtick ================== */
_EXTERN_C_ double PMPI_Wtick();
_EXTERN_C_ double MPI_Wtick() { 
    double _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Wtick();
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_category_get_cvars ================== */
_EXTERN_C_ int PMPI_T_category_get_cvars(int cat_index, int len, int indices[]);
_EXTERN_C_ int MPI_T_category_get_cvars(int cat_index, int len, int indices[]) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_category_get_cvars(cat_index, len, indices);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Errhandler_free ================== */
_EXTERN_C_ int PMPI_Errhandler_free(MPI_Errhandler *errhandler);
_EXTERN_C_ int MPI_Errhandler_free(MPI_Errhandler *errhandler) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Errhandler_free(errhandler);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Topo_test ================== */
_EXTERN_C_ int PMPI_Topo_test(MPI_Comm comm, int *status);
_EXTERN_C_ int MPI_Topo_test(MPI_Comm comm, int *status) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Topo_test(comm, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Ireduce ================== */
_EXTERN_C_ int PMPI_Ireduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Ireduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Ireduce(sendbuf, recvbuf, count, datatype, op, root, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_get_errhandler ================== */
_EXTERN_C_ int PMPI_File_get_errhandler(MPI_File file, MPI_Errhandler *errhandler);
_EXTERN_C_ int MPI_File_get_errhandler(MPI_File file, MPI_Errhandler *errhandler) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_get_errhandler(file, errhandler);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Bsend_init ================== */
_EXTERN_C_ int PMPI_Bsend_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
_EXTERN_C_ int MPI_Bsend_init(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Bsend_init(buf, count, datatype, dest, tag, comm, request);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Add_error_string ================== */
_EXTERN_C_ int PMPI_Add_error_string(int errorcode, const char *string);
_EXTERN_C_ int MPI_Add_error_string(int errorcode, const char *string) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Add_error_string(errorcode, string);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Accumulate ================== */
_EXTERN_C_ int PMPI_Accumulate(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win);
_EXTERN_C_ int MPI_Accumulate(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Accumulate(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, op, win);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_category_get_num ================== */
_EXTERN_C_ int PMPI_T_category_get_num(int *num_cat);
_EXTERN_C_ int MPI_T_category_get_num(int *num_cat) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_category_get_num(num_cat);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Comm_set_errhandler ================== */
_EXTERN_C_ int PMPI_Comm_set_errhandler(MPI_Comm comm, MPI_Errhandler errhandler);
_EXTERN_C_ int MPI_Comm_set_errhandler(MPI_Comm comm, MPI_Errhandler errhandler) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Comm_set_errhandler(comm, errhandler);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Aint_diff ================== */
_EXTERN_C_ MPI_Aint PMPI_Aint_diff(MPI_Aint addr1, MPI_Aint addr2);
_EXTERN_C_ MPI_Aint MPI_Aint_diff(MPI_Aint addr1, MPI_Aint addr2) { 
    MPI_Aint _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_Aint_diff(addr1, addr2);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_write_ordered_begin ================== */
_EXTERN_C_ int PMPI_File_write_ordered_begin(MPI_File fh, const void *buf, int count, MPI_Datatype datatype);
_EXTERN_C_ int MPI_File_write_ordered_begin(MPI_File fh, const void *buf, int count, MPI_Datatype datatype) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_write_ordered_begin(fh, buf, count, datatype);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_write_ordered_end ================== */
_EXTERN_C_ int PMPI_File_write_ordered_end(MPI_File fh, const void *buf, MPI_Status *status);
_EXTERN_C_ int MPI_File_write_ordered_end(MPI_File fh, const void *buf, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_write_ordered_end(fh, buf, status);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_Graphdims_get ================== */
_EXTERN_C_ int PMPI_Graphdims_get(MPI_Comm comm, int *nnodes, int *nedges);
_EXTERN_C_ int MPI_Graphdims_get(MPI_Comm comm, int *nnodes, int *nedges) { 
    int _wrap_py_return_val = 0;

  HENSON_REPLACE_COMM_WORLD(comm);

  _wrap_py_return_val = PMPI_Graphdims_get(comm, nnodes, nedges);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_T_pvar_get_info ================== */
_EXTERN_C_ int PMPI_T_pvar_get_info(int pvar_index, char *name, int *name_len, int *verbosity, int *var_class, MPI_Datatype *datatype, MPI_T_enum *enumtype, char *desc, int *desc_len, int *binding, int *readonly, int *continuous, int *atomic);
_EXTERN_C_ int MPI_T_pvar_get_info(int pvar_index, char *name, int *name_len, int *verbosity, int *var_class, MPI_Datatype *datatype, MPI_T_enum *enumtype, char *desc, int *desc_len, int *binding, int *readonly, int *continuous, int *atomic) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_T_pvar_get_info(pvar_index, name, name_len, verbosity, var_class, datatype, enumtype, desc, desc_len, binding, readonly, continuous, atomic);
    return _wrap_py_return_val;
}

/* ================== C Wrappers for MPI_File_read_at ================== */
_EXTERN_C_ int PMPI_File_read_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
_EXTERN_C_ int MPI_File_read_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status) { 
    int _wrap_py_return_val = 0;

  
  _wrap_py_return_val = PMPI_File_read_at(fh, offset, buf, count, datatype, status);
    return _wrap_py_return_val;
}


