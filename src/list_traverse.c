#include <stdio.h>

#include <mastik/list.h>
#include <mastik/list_traverse.h>
#include <mastik/low.h>

inline
void
traverse_list_skylake(node *ptr)
{
	while (ptr && ptr->next && ptr->next->next)
	{
		memaccess (ptr);
		memaccess (ptr->next);
		memaccess (ptr->next->next);
		memaccess (ptr);
		memaccess (ptr->next);
		memaccess (ptr->next->next);
		ptr = ptr->next;
	}
}

inline
void
traverse_list_asm_skylake(node *ptr)
{
	__asm__ volatile
	(
		"test %%rcx, %%rcx;"	
		"jz out;"
		"loop:"
		"movq (%%rcx), %%rax;" // memread 0
		"test %%rax, %%rax;" // 
		"jz out;"
		"movq (%%rax), %%rax;" // memread 1
		"test %%rax, %%rax;" // 
		"jz out;"
		"movq (%%rax), %%rax;" // memread 2
		"movq (%%rcx), %%rcx;" // memread 0, ptr is now 1
		"movq (%%rcx), %%rax;" // memread 1
		"movq (%%rax), %%rax;" // memread 2
		"test %%rcx, %%rcx;"
		"jnz loop;"
		"out:"
		: // no output
		: "c" (ptr) // ptr in rcx
		: "cc", "memory"
	);
}

inline
void
traverse_list_asm_haswell(node *ptr)
{
	__asm__ volatile
	(
		"test %%rcx, %%rcx;"
		"jz out2;"
		"loop2:"
		"movq (%%rcx), %%rax;"
		"test %%rax, %%rax;"
		"jz out2;"
		"movq (%%rax), %%rax;"
		"movq (%%rcx), %%rcx;"
		"movq (%%rcx), %%rax;"
		"test %%rcx, %%rcx;"
		"jnz loop2;"
		"out2:"
		: // no output
		: "c" (ptr)
		: "cc", "memory"
	);
}

inline
void
traverse_list_asm_simple(node *ptr)
{
	__asm__ volatile
	(
		"loop3:"
		"test %%rcx, %%rcx;"
		"jz out3;"
		"movq (%%rcx), %%rcx;"
		"jmp loop3;"
		"out3:"
		: // no output
		: "c" (ptr)
		: "cc", "memory"
	);
}

inline
void
traverse_list_haswell(node *ptr)
{
	while (ptr && ptr->next)
	{
		memaccess (ptr);
		memaccess (ptr->next);
		memaccess (ptr);
		memaccess (ptr->next);
		ptr = ptr->next;
	}
}

inline
void
traverse_list_simple(node *ptr)
{
	while (ptr)
	{
		memaccess (ptr);
		ptr = ptr->next;
	}
}

inline
void
traverse_list_to_n(node *ptr, int n)
{
	while (ptr && n-- > 0)
	{
		memaccess (ptr);
		ptr = ptr->next;
	}
}

inline
void
traverse_list_to_n_skylake(node *ptr, int n)
{
	while (n > 2 && ptr && ptr->next && ptr->next->next)
	{
		memaccess (ptr);	memaccess (ptr->next);	memaccess (ptr->next->next);
		memaccess (ptr);	memaccess (ptr->next);	memaccess (ptr->next->next);
		ptr = ptr->next;
		n--;
	}

	while (n > 1 && ptr && ptr->next)
	{
		memaccess (ptr);	memaccess (ptr->next);
		memaccess (ptr);	memaccess (ptr->next);
		ptr = ptr->next;
		n--;
	}

	while (n > 0 && ptr)
	{
		memaccess (ptr);
		ptr = ptr->next;
		n--;
	}
}

inline void traverse_zigzag_victim(node *ptr, void *victim)
{
    while (ptr)
    {
        memaccess (ptr);
        memaccess (victim);
        ptr = ptr->next;
    }
}
