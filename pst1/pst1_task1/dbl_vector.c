
#include <stdio.h>
#include <stdlib.h>
#include "dbl_vector.h"
#define min(a,b) ((a) < (b) ? (a) : (b)) // define the min function
#define max(a,b) ((a) > (b) ? (a) : (b)) // define the max function

void dv_init( dbl_vector_t* vec ) {
    vec->capacity = DV_INITIAL_CAPACITY;
    vec->size = 0;
    //vec->data = (double*)malloc(sizeof(double));
    vec->data = (double*)malloc(vec->capacity * sizeof(double));
}

void dv_ensure_capacity(dbl_vector_t* vec, size_t new_size) {
    size_t old_capacity = vec->capacity;
    
    if (new_size > old_capacity) {
        // Calculate new capacity
        size_t new_capacity = old_capacity * DV_GROWTH_FACTOR;
        if (new_capacity < new_size) {
            new_capacity = new_size;
        }
        
        // Calculate the number of bytes needed for the new capacity
        size_t new_mem_size = new_capacity * sizeof(double);
        
        // Reallocate memory for the vector data
        vec->data = (double*)realloc(vec->data, new_mem_size);
        vec->capacity = new_capacity;
    }
    
    // If new_size <= old_capacity, do nothing
}

void dv_destroy( dbl_vector_t* vec ) {
    vec->capacity = 0;
    vec->size = 0;
    free(vec->data); // free the data
    vec->data = NULL;
}

void dv_copy( dbl_vector_t* vec, dbl_vector_t* dest ) {
    dest->size = vec->size; // set the size of the destination to the size of the source
    dv_ensure_capacity(dest, vec->size); // ensure the capacity of the destination
    for (size_t i = 0; i < vec->size; i++) {
        dest->data[i] = vec->data[i]; // copy the data from the source to the destination
    }
}

void dv_clear( dbl_vector_t* vec ) {
    vec->size = 0; // clear the size of the vector
}

void dv_push( dbl_vector_t* vec, double new_item ) { //
    size_t old_size = vec->size; // store the old size of the vector
    vec->size = old_size + 1; // increase the size of the vector for the new item
    dv_ensure_capacity(vec, old_size + 1); // ensure the capacity of the vector
    for(int i =0; i < old_size; i++) {
        vec->data[i] = vec->data[i]; // add the new item to the vector
    }
    vec->data[(int)old_size] = new_item; // add the new item to the vector

}

void dv_pop( dbl_vector_t* vec ) { //Remove the last item (if any) from the end of the vector.
    size_t old_capacity = vec->capacity; // store the old capacity of the vector
    size_t old_size = vec->size; // store the old size of the vector
    double *old_data = vec->data; // store the old data of the vector
    if(old_size > 0){
        vec->size = old_size - 1; // decrease the size of the vector
        for(int i = 0; i < old_size - 1; i++) {
            vec->data[i] = old_data[i]; // Copy the reset of data
        }
    } else {
        vec->size = 0; // set the size of the vector to 0
    }

}

double dv_last( dbl_vector_t* vec ) { //Get the last item from the end of the vector, or NaN if the vector is empty.
    double result = NAN;
    size_t old_size = vec->size; // store the old size of the vector
    size_t old_capacity = vec->capacity; // store the old capacity of the vector
    double *old_data = vec->data; // store the old data of the vector
    for(int i = 0; i < old_size; i++) {
        result = old_data[i]; // get the last item from the end of the vector
    }
    if (vec->size > 0){
        return result;
    } else {
        return NAN; // return NaN if the vector is empty
    }
    return result;
}

void dv_insert_at(dbl_vector_t* vec, size_t pos, double new_item) {
    size_t old_size = vec->size;  // Store the old size of the vector
    // Calculate the location to insert, which is the minimum of pos and old_size
    size_t loc = min(pos, old_size);
    // Ensure the vector has enough capacity to accommodate the new item
    dv_ensure_capacity(vec, old_size + 1);

    // vec->data[loc] = new_item; // Insert the new item at the calculated location
    // vec->data[loc+1] = old_data[0];
    // vec->data[loc+2] = old_data[1];
    // vec->data[loc+3] = old_data[2];
    // vec->data[loc+4] = old_data[3];
    // vec->data[loc+5] = old_data[4];
    // vec->data[loc+6] = old_data[5];
    // vec->data[loc+7] = old_data[6];
    // vec->data[loc+8] = old_data[7];
    // vec->data[loc+9] = old_data[8];

/*Because old_data is a pointer points to the address of vec->data, 
so when we change the value of vec->data, the value of old_data will also change.
Shifting the elements of the vector to the left will cause overwriting the new item.
*/
    for (size_t i = old_size; i > loc; i--) {
        vec->data[i] = vec->data[i - 1];
    }
    vec->data[loc] = new_item;
    vec->size = old_size + 1; // Increase the size of the vector by 1
}


void dv_remove_at( dbl_vector_t* vec, size_t pos ) {
    size_t old_size = vec->size; // store the old size of the vector
    double loc = min(pos, old_size);
    size_t old_capacity = vec->capacity; // store the old capacity of the vector
    for(size_t i = loc; i < old_size; i++) {
        vec->data[i] = vec->data[i + 1]; // remove the item at the designated position in the vector
    }
    if(pos >= old_size){
        vec->size = old_size; // set the size of the vector to the old size
    } else {
        vec->size = old_size - 1; // decrease the size of the vector
    }
}

void dv_foreach( dbl_vector_t* vec, void (*callback)(double, void*), void* info ) {
    size_t old_size = vec->size; // store the old size of the vector
    double *old_data = vec->data; // store the old data of the vector
    size_t old_capacity = vec->capacity; // store the old capacity of the vector
    vec->capacity = old_capacity; // set the capacity of the vector
    vec->size = old_size; // set the size of the vector
    vec->data = old_data; // set the data of the vector
    for(int i = 0; i < old_size; i++) {
        callback(old_data[i], info); // call the callback function
    }
}