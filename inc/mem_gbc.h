#ifndef MEM_GBC_H
# define MEM_GBC_H

# include <stdlib.h>

//##############################
// Linked list function
//##############################

typedef struct s_lst
{
	void			*content;
	struct s_lst	*next;
} t_lst;

// Definition
t_lst	*lst_new(void *content);
void	lst_addback(t_lst **head, t_lst *node);
void	lst_clear(t_lst **head, void (*f)(void *));

//implimentation
t_lst	*lst_new(void *content)
{
	t_lst *node;

	node = malloc(sizeof(t_lst));
	if (!node)
		return NULL;
	node->content = content;
	node->next = NULL;
	return node;
}

void	lst_addback(t_lst **head, t_lst *new_node)
{
	if (!new_node || !head)
		return ;
	if (!*head)
	{
		*head = new_node;
		return ;
	}

	t_lst *node = *head;
	while (node->next)
		node = node->next;
	node->next = new_node;
}

void	lst_clear(t_lst **head, void (*f)(void *))
{
	if (!head)
		return ;
	t_lst *node = *head;
	t_lst *prev = NULL;
	while (node)
	{
		if (f)
			f(node->content);
		prev = node;
		node = node->next;
		free(prev);
	}
	*head = NULL;
}

//##############################
// Memory garbage collector
//##############################

// Definition
t_lst	**global_pointer_storage(void);
void	*gbc_malloc(size_t n);
void	gbc_clear(void);

//implimentation
t_lst	**global_pointer_storage(void)
{
	static t_lst *storage;

	return &storage;
}

void	*gbc_malloc(size_t n)
{
	void *block = malloc(n);
	if (!block)
		return NULL;
	lst_addback(global_pointer_storage(), lst_new(block));
	return block;
}

void	gbc_clear(void)
{
	lst_clear(global_pointer_storage(), free);
}

#endif
