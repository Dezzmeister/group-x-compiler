/*
    Handwritten factorial function to figure some of this stuff out

    https://refspecs.linuxfoundation.org/LSB_5.0.0/LSB-Core-AMD64/LSB-Core-AMD64/callingsequence.html
    https://ftp.gnu.org/old-gnu/Manuals/gas-2.9.1/html_chapter/as_7.html
*/
.section    .rodata # "read only data" for compile time constants, str literals, etc.
.ERR_STR:
    .asciz  "Usage: %s (num)\n" # asciz: null terminated ascii string
.FMT_STR:
    .asciz  "%d\n"
.text
/*
    factorial function. accepts long argument (4 bytes) on stack and computes factorial,
    leaving result in eax.

    When this function is called, the stack will look like this:
              | qword argument    |
      rsp ->  | qword return addr |
    Then we push old rbp on the stack (pushq %rbp):
              | qword argument    |
              | qword return addr |
      rsp ->  | qword rbp         |
    Then we set rbp <- rsp to set up the new stack frame. This function follows the
    cdecl convention which means that the caller will need to clean up stack arguments
    when the function returns. I think this is the default when you compile with gcc.
    The argument will be at rbp + 16 and the first four bytes will be zero, but if you try
    to movl from 12(%rbp) you will get nonsense because of endianness (I think), so you have
    to get the entire 8 byte qword. Then the lower 4 bytes will be in the corresponding 32-bit
    register (rdx -> edx).

    This function has 1 local variable, and that's the argument that was passed in. It copies
    the argument to its stack frame (pushq %rdx). This is unnecessary but I think it's ok
    for our compiler to handle arguments like this.
*/
fac:
    pushq   %rbp
    movq    %rsp, %rbp
    movq    16(%rbp), %rdx
    pushq   %rdx
    cmpl    $0, %edx    # check if arg == 0 and jump to FAC_BASE if so
    je      .FAC_BASE
    subl    $1, %edx    # edx <- edx - 1 (preparing to call fac again)
    pushq   %rdx        # push argument to fac onto the stack
    call    fac         # call fac, result will be in eax (push rip and jmp to fac)
    addq    $8, %rsp    # clean up the stack (cdecl convention)
    popq    %rdx        # get our (arg - 1)
    mul     %rdx        # multiply by rax and store the result in rax. rax has the result from calling fac
    popq    %rbp        # restore old rbp. no need to reset rsp because we popped the local var already
    ret                 # pop return address into rip
.FAC_BASE:
    movl    $1, %eax    # prepare to return 1
    movq    %rbp, %rsp  # invalidate local stack frame which contains a local variable
    popq    %rbp        # restore old rbp
    ret                 # pop return address into rip. rsp now points at the argument to fac

.globl  main            # .globl/.global means that this symbol should be visible to the linker
/*
    For command line args, the OS will put argc in edi and argv in rsi.
*/
main:
    pushq   %rbp
    movq    %rsp, %rbp
    cmpl    $2, %edi        # error checking, make sure there are only 2 args by checking argc
    jne     .BAD_ARGS
    addq    $8, %rsi        # rsi now points to argv[1]
    movq    (%rsi), %rdi    # get argv[1] which is a pointer to the first char in the arg, prepare to call atoi
    call    atoi@PLT        # call atoi thru "procedure linkage table" - some kind of dynamic linkage which I don't fully understand
    pushq   %rax            # atoi returns to eax, push result as arg to fac
    call    fac                # call fac
    addq    $8, %rsp        # clean up stack because we pushed an argument before call
    leaq    .FMT_STR(%rip), %rdi    # load addr of FMT_STR into rdi, preparing to call printf
    movq    %rax, %rsi      # the second argument to printf, the int to substitute in the fmt string
    movq    $0, %rax        # the number of floating point arguments to printf
    call    printf@PLT      # call printf thru PLT
    leave                   # does movq %rbp, %rsp and popq %rbp. apparently leave is fast but enter is not
    ret
.BAD_ARGS:
    leaq    .ERR_STR(%rip), %rdi    # calling printf with error fmt string
    movq    (%rsi), %rsi            # at this point rsi points to argv[0], so deref it for str arg to printf
    movq    $0, %rax                # 0 floating point args
    call    printf@PLT              # call printf
    leave
    ret
