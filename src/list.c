#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <mastik/list.h>

int
list_to_array(node *ptr, uint64_t *array)
{
	int l = 0;
	int index=0;
	while (ptr)
	{
		array[index++] = (uint64_t)ptr;
		l = l + 1;
		ptr = ptr->next;
	}
	return l;
}

int
list_length(node *ptr)
{
	int l = 0;
	while (ptr)
	{
		l = l + 1;
		ptr = ptr->next;
	}
	return l;
}

/* add element to the head of the list */
void
list_push(node **ptr, node *e)
{
	if (!e)
	{
		return;
	}
	e->prev = NULL;
	e->next = *ptr;
	if (*ptr)
	{
		(*ptr)->prev = e;
	}
	*ptr = e;
}

/* add element to the end of the list */
int
list_append(node **ptr, node *e)
{
	node *tmp = *ptr;
	if (!e)
	{
		return -1;
	}
	if (!tmp)
	{
		*ptr = e;
		return 1;
	}
	int length = 1;
	int already_exist=0;
	while (tmp->next)
	{
		if (tmp->next == e)
		{
			already_exist=1;
		}
		tmp = tmp->next;
		length++;
	}
	if (!already_exist) {
		tmp->next = e;
		e->prev = tmp;
		e->next = NULL;
		return length+1;
	}
	return length;
}

/* set the fist element of the list*/
int
list_set_first(node **ptr, node *e)
{
	if (!e)
	{
		return -1;
	}
	*ptr = e;
	e->prev = NULL;
	e->next = NULL;
	return 1;
}

/* remove and return first element of list */
node*
list_pop(node **ptr)
{
	node *tmp = (ptr) ? *ptr : NULL;
	if (!tmp)
	{
		return NULL;
	}
	if (tmp->next)
	{
		tmp->next->prev = NULL;
	}
	*ptr = tmp->next;
	tmp->next = NULL;
	tmp->prev = NULL;
	return tmp;
}

void
list_print(node *ptr)
{
	if (!ptr)
	{
		printf("(empty)\n");
		return;
	}
	while (ptr != NULL)
	{
		printf("%p ", (void*)ptr);
		ptr = ptr->next;
	}
	printf("\n");
}
