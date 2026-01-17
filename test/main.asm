global _start
section .text
extern print
_start:
sub rsp, 8
mov rax, 10
push rax
mov rax, 2
push rax
pop rax
pop rbx
mov rcx, rax
mov rax, rbx 
xor rdx, rdx
div rcx
push rax
pop rax
mov [rsp + 0], rax
mov rdi, rax
call print
mov rax, 9
push rax
mov rax, 4
push rax
pop rax
pop rbx
mov rcx, rax
mov rax, rbx
xor rdx, rdx
div rcx
push rdx
pop rax
mov [rsp + 0], rax
mov rdi, rax
call print
add rsp, 8
mov rax, 60
xor rdi, rdi
syscall
