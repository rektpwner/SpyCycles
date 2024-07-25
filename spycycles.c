#define _GNU_SOURCE
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/uio.h>
#include <stdint.h>
#include <capstone/capstone.h>

static inline uint64_t rdtsc()
{
    unsigned int low = 0, high = 0;
    __asm__ __volatile__ ("rdtsc" : "=a"(low), "=d"(high));
    return ((unsigned long long)high << 32) | low;
}

static inline void disassemble_instruction(csh handle, cs_insn *insn, pid_t pid, uint64_t address)
{
    unsigned char opcode[16];
    struct iovec local = { opcode, sizeof(opcode) };
    struct iovec remote = { (void*)address, sizeof(opcode) };
    const uint8_t *code = NULL;
    size_t code_size = 0;

    ssize_t opcode_size = process_vm_readv(pid, &local, 1, &remote, 1, 0);
    if (opcode_size == -1) {
        perror("process_vm_readv");
        return;
    }

    code = opcode;
    code_size = opcode_size;

    if (cs_disasm_iter(handle, &code, &code_size, &address, insn)) {
        for (size_t i = 0; i < insn->size; ++i) {
            printf("%02x ", insn->bytes[i]);
        }
        printf("%s %s", insn->mnemonic, insn->op_str);
    }
    else {
        printf("<unknown>");
    }
}

static inline void process_parent(const pid_t child_pid)
{
    int status = 0;
    csh handle = 0;
    cs_insn *insn = NULL;
    struct user_regs_struct regs = {0};
    uint64_t start_ticks = 0, end_ticks = 0;

    waitpid(child_pid, &status, 0);

    if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK) {
        fprintf(stderr, "Failed to initialize Capstone\n");
        return;
    }

    insn = cs_malloc(handle);

    while (WIFSTOPPED(status)) {
        ptrace(PTRACE_GETREGS, child_pid, NULL, &regs);

        start_ticks = rdtsc();
        ptrace(PTRACE_SINGLESTEP, child_pid, NULL, NULL);
        waitpid(child_pid, &status, 0);
        end_ticks = rdtsc();

        printf("[%lu] [0x%llx] ", end_ticks - start_ticks, regs.rip);

        disassemble_instruction(handle, insn, child_pid, regs.rip);

        printf("\n");

        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            break;
        }
    }

    cs_free(insn, 1);
    cs_close(&handle);

    ptrace(PTRACE_DETACH, child_pid, NULL, NULL);
}

static inline void process_child(char *argv[])
{
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    execvp(argv[1], &argv[1]);
    perror("execve");
    exit(1);
}

int main(int argc, char *argv[])
{
    pid_t child_pid = 0;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <program> [args]\n", argv[0]);
        return EXIT_FAILURE;
    }

    child_pid = fork();
    if (child_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (child_pid == 0) {
        process_child(argv);
    }
    else {
        process_parent(child_pid);
    }

    return EXIT_SUCCESS;
}
