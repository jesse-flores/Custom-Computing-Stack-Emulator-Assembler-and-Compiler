; Program that prints a character using a syscall
start:
    LOAD_A 1        ; Syscall number for PRINT_CHAR
    LOAD_B 72       ; ASCII value for 'H'
    SYSCALL
    LOAD_A 1
    LOAD_B 101      ; 'e'
    SYSCALL
    LOAD_A 1
    LOAD_B 108      ; 'l'
    SYSCALL
    LOAD_A 1
    LOAD_B 108      ; 'l'
    SYSCALL
    LOAD_A 1
    LOAD_B 111      ; 'o'
    SYSCALL
    HALT