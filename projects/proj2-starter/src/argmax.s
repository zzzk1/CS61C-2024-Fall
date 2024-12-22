.globl argmax

.text
# =================================================================
# FUNCTION: Given a int array, return the index of the largest
#   element. If there are multiple, return the one
#   with the smallest index.
# Arguments:
#   a0 (int*) is the pointer to the start of the array
#   a1 (int)  is the # of elements in the array
# Returns:
#   a0 (int)  is the first index of the largest element
# Exceptions:
#   - If the length of the array is less than 1,
#     this function terminates the program with error code 36
# =================================================================
argmax:
    mv t0 a0 #address
    mv t1 x0 #index
    addi t5 x0 1
    mv t6 x0 #max value
    blt a1 t5 exception
    jal x1 loop_start
    j exit

loop_start:
    bge t1 a1 loop_end
    slli t2 t1 2 #offset
    add t2 t2 t0 # t2 point to array[i]
    lw t3 0(t2)
    jal x1 loop_continue
    addi t1 t1 1
    j loop_start

max_one:
    mv t6 t2
    mv a0 t2
    jr ra

loop_continue:
    bge t3 t6 max_one # if array[i] > max value, save the address
    jr ra 

loop_end:
    j exit

exception:
    li a0 36
    j exit