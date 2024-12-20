.globl factorial

.data
n: .word 7

.text
# Don't worry about understanding the code in main
# You'll learn more about function calls in lecture soon
main:
    la t0, n
    lw a0, 0(t0)
    jal ra, factorial

    addi a1, a0, 0
    addi a0, x0, 1
    ecall # Print Result

    addi a1, x0, '\n'
    addi a0, x0, 11
    ecall # Print newline

    addi a0, x0, 10
    ecall # Exit


# factorial takes one argument:
# a0 contains the number which we want to compute the factorial of
# The return value should be stored in a0
factorial:
    # YOUR CODE HERE
    addi a1 x0 1
    addi a2 x0 0

    mv a2 a0
    bge a2 a1 loop

    bge a0 a1 exit

    addi a0 x0 1
    # This is how you return from a function. You'll learn more about this later.
    # This should be the last line in your program.
    jr ra

loop:
    bge a1 a2 exit
    addi a2 a2 -1
    mul a0 a0 a2
    j loop
exit:
    jr ra