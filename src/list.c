#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "list.h"

void list_node_print(list_node_t * node, void (*print_data)(void *node_data))
{
	if(print_data != NULL) {
		print_data(node->data);
		printf(" :: ");
	}

	printf("%p <~ %p ~> %p\n", (void *) node->prev, (void *) node, (void *) node->next);
}

void list_print(list_t * list, void (*print_data)(void *node_data))
{
	printf("list_t:  %p\n",(void *) list);
	printf("head:    %p\n", (void *) list->head);
	printf("tail:    %p\n", (void *) list->tail);
	printf("count:   %ld\n", list->count);

	list_node_t *current;

	list_for_each(list, current) {
		list_node_print(current, print_data);
	}

	printf("\n");
}

list_t *list_new()
{
	list_t *list = malloc(sizeof(list_t));

	list->head = NULL;
	list->tail = NULL;
	list->count = 0;

	return list;
}

void list_free(list_t * list, void (*free_data)(void *node_data))
{
	list_node_t *current = list->head;

	while (current) {
		list_node_t *next = current->next;

		list_node_free(current, free_data);
		current = next;
	}

	list->head = NULL;
	list->tail = NULL;

	free(list);
}

void list_reverse(list_t * list)
{
	list_node_t *current = list->head;
	list_node_t *temp;

	while (current) {
		temp = current->prev;

		current->prev = current->next;
		current->next = temp;

		current = current->prev;
	}

	temp = list->head;

	list->head = list->tail;
	list->tail = temp;
}

void list_node_free(list_node_t * node, void (*free_data)(void *node_data))
{
	if( (*free_data) != NULL && node != NULL) {
		(*free_data)(node->data);
	}

	if (node != NULL && node->data != NULL) {
		free(node->data);
	}

	free(node);
}

list_node_t *list_node_new(void *data)
{
	list_node_t *node = malloc(sizeof(list_node_t));

	if (node == NULL) {
		return NULL;
	}

	node->data = data;

	node->next = NULL;
	node->prev = NULL;

	return node;
}

void list_node_insert_after(list_t * list, list_node_t * current, list_node_t * new)
{
	assert(list != NULL);
	assert(new != NULL);

	if (list->head == NULL && list->tail == NULL) {
		list->head = new;
		list->tail = new;
		list->count += 1;

		return;
	}

	assert(current != NULL);

	new->next = current->next;
	new->prev = current;

	if (current->next == NULL) {
		list->tail = new;
	} else {
		current->next->prev = new;
	}

	current->next = new;

	list->count += 1;
}

void list_node_insert_before(list_t * list, list_node_t * current, list_node_t * new)
{
	assert(list != NULL);
	assert(new != NULL);

	if (list->head == NULL && list->tail == NULL) {
		list->head = new;
		list->tail = new;
		list->count += 1;

		return;
	}

	assert(current != NULL);

	new->prev = current->prev;
	new->next = current;

	if (current->prev == NULL) {
		list->head = new;
	} else {
		current->prev->next = new;
	}

	current->prev = new;

	list->count += 1;
}

list_node_t *list_node_detach(list_t * list, list_node_t * node)
{
	assert(list != NULL);
	assert(node != NULL);

	if (node == list->head) {
		list->head = node->next;
	} else {
		node->prev->next = node->next;
	}

	if (node == list->tail) {
		list->tail = node->prev;
	} else {
		node->next->prev = node->prev;
	}

	node->next = NULL;
	node->prev = NULL;

	list->count -= 1;

	return node;
}

static list_node_t *merge_nodes(list_t * list, const list_node_t * node1, const list_node_t * node2,
								long (*comparator)(void *node1_data, void *node2_data))
{
	list_node_t *min = list_node_new(NULL);

	min->prev = list->tail;

	list->tail = min;
	list->count += 1;

	if (node1 == NULL) {
		min->data = node2->data;
		min->next = node2->next;

		list_node_t *tail = min->next;

		while (tail != NULL) {
			tail = tail->next;
			list->count += 1;
		}
		list->tail = tail;

		return min;
	} else if (node2 == NULL) {
		min->data = node1->data;
		min->next = node1->next;

		list_node_t *tail = min->next;

		while (tail != NULL) {
			tail = tail->next;
			list->count += 1;
		}
		list->tail = tail;

		return min;
	}

	if (comparator(node1->data, node2->data) > 0) {
		min->data = node1->data;
		min->next = merge_nodes(list, node1->next, node2, comparator);
	} else {
		min->data = node2->data;
		min->next = merge_nodes(list, node1, node2->next, comparator);
	}

	return min;
}

list_t *list_merge_sorted(const list_t * list1,
						  const list_t * list2, long (*comparator)(void *node1_data, void *node2_data))
{
	list_t *sorted = list_new();

	sorted->head = merge_nodes(sorted, list1->head, list2->head, comparator);

	return sorted;
}

list_t *list_merge_k_sorted(int amount, list_t ** lists, long (*comparator)(void *node1_data, void *node2_data))
{
	int interval = 1;

	while (interval < amount) {
		for (int i = 0; i < amount - interval; i += interval * 2) {
			lists[i] = list_merge_sorted(lists[i], lists[i + interval], comparator);
		}

		interval *= 2;
	}

	return amount > 0 ? lists[0] : NULL;
}

list_node_t *list_search(const list_t * list, int (*is_target)(void *node_data))
{
	list_node_t *current;

	list_for_each(list, current) {
		if (is_target(current->data)) {
			return current;
		}
	}

	return NULL;
}
