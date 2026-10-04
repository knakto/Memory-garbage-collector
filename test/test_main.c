#include "mem_gbc.h"
#include <stdio.h>
#include <assert.h>

void mem_gbc_allocate(void)
{
	void *item5 = gbc_malloc(100);
	void *item6 = gbc_malloc(100);
	void *item7 = gbc_malloc(100);
	void *item8 = gbc_malloc(100);
	(void)item5;
	(void)item6;
	(void)item7;
	(void)item8;

	//free test
	gbc_free(item5);
	gbc_free(item6);
	gbc_free(item7);
}

void mem_gbc_test(void)
{
	void *item1 = gbc_malloc(100);
	void *item2 = gbc_malloc(100);
	void *item3 = gbc_malloc(100);
	void *item4 = gbc_malloc(100);
	(void)item1;
	(void)item2;
	(void)item3;
	(void)item4;
	mem_gbc_allocate();
	gbc_clear();
}

void linked_list_test(void)
{
	// lst_new test
	{
		void *ptr = "hello world";
		t_lst *node = lst_new(&ptr);
		assert(node != NULL);
		assert(node->content == &ptr);
		assert(node->next == NULL);
		free(node);
	}

	// lst_addback test
	{
		void *ptr1 = "hello";
		void *ptr2 = "world";
		t_lst *head = NULL;
		t_lst *node1 = lst_new(&ptr1);
		t_lst *node2 = lst_new(&ptr2);

		lst_addback(&head, node1);
		assert(head == node1);
		lst_addback(&head, node2);
		assert(head->next == node2);
		free(node1);
		free(node2);
	}

	// lst_clear test
	{
		void *ptr = "hello world";
		t_lst *head = NULL;
		lst_addback(&head, lst_new(ptr));
		lst_addback(&head, lst_new(ptr));
		lst_addback(&head, lst_new(ptr));
		lst_addback(&head, lst_new(ptr));
		lst_clear(&head, NULL);
		assert(head == NULL);
	}
}

int main(void)
{
	linked_list_test();
	return 0;
}
