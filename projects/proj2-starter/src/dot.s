.globl dot

.text
# =======================================================
# FUNCTION: Dot product of 2 int arrays
# Arguments:
#   a0 (int*) is the pointer to the start of arr0
#   a1 (int*) is the pointer to the start of arr1
#   a2 (int)  is the number of elements to use
#   a3 (int)  is the stride of arr0
#   a4 (int)  is the stride of arr1
# Returns:
#   a0 (int)  is the dot product of arr0 and arr1
# Exceptions:
#   - If the number of elements to use is less than 1,
#     this function terminates the program with error code 36
#   - If the stride of either array is less than 1,
#     this function terminates the program with error code 37
# =======================================================
dot:
    mv t0 a0
    mv t2 x0 #index of array0
    mv t1 a1
    mv t3 x0 #index of array1

    mv a0 x0

    addi t5 x0 1
    blt a3 t5 exception_stride
    blt a4 t5 exception_stride
    blt a2 t5 exception_elements

loop_start:
    blez a2 loop_end
    
    slli t4 t2 2
    mul t4 t4 a3
    add t4 t4 t0
    lw a5 0(t4)

    slli t5 t3 2
    mul t5 t5 a4
    add t5 t5 t1
    lw a6 0(t5)
    
    mul a7 a5 a6
    add a0 a0 a7

    addi a2 a2 -1
    addi t2 t2 1
    addi t3 t3 1
    j loop_start

loop_end:
    jr ra

exception_elements:
    li a0 36
    j exit

exception_stride:
    li a0 37
    j exit
