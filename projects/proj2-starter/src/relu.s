.globl relu

.text
# ==============================================================================
# FUNCTION: Performs an inplace element-wise ReLU on an array of ints
# Arguments:
#   a0 (int*) is the pointer to the array
#   a1 (int)  is the # of elements in the array
# Returns:
#   None
# Exceptions:
#   - If the length of the array is less than 1,
#     this function terminates the program with error code 36
# ==============================================================================
relu:
    mv t0 a0
    mv t3 x0
    addi t1 x0 0
    addi a2 x0 1
    blt a1 a2 exception
    bge a1 a2 loop_start
    j exit

loop_start:
    bge t1 a1 loop_end
    slli t2 t1 2
    add t3 t2 t0
    lw t2 0(t3)
    jal x1 loop_continue
    addi t1 t1 1
    j loop_start

loop_continue:
    blez t2 negitive_to_zero
    jr ra

negitive_to_zero:
    mv t2 x0
    sw t2 0(t3)
    jr ra

loop_end:
    j exit

exception:
    li a0 36
    j exit