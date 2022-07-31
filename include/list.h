#ifndef _LIST_H_
#define _LIST_H_

typedef struct list_node {
	void *data;
	struct list_node *next;
	struct list_node *prev;
} list_node_t;

typedef struct list {
	list_node_t *head;
	list_node_t *tail;
	size_t count;
} list_t;

list_t *list_new();
void list_free(list_t * list, void (*free_data)(void *node_data));

#define list_for_each(list, current) \
    for ((current) = (list)->head; (current) != NULL; (current) = (current)->next)

#define list_for_each_backward(list, current) \
    for ((current) = (list)->tail; (current) != NULL; (current) = (current)->prev)

#define list_head(list) ((list)->head)
#define list_tail(list) ((list)->tail)

#define list_push(list, node) list_node_insert_after(list, list->tail, node);
#define list_pop(list) list_node_detach(list, list->tail);

#define list_shift(list) list_node_detach(list, list->head);
#define list_unshift(list, node) list_node_insert_before(list, list->head, node);

void list_node_print(list_node_t * node, void (*print_data)(void *node_data));
void list_print(list_t * list, void (*print_data)(void *node_data));
void list_reverse(list_t * list);

list_node_t *list_node_new(void *data);
void list_node_free(list_node_t * node, void (*free_data)(void *node_data));

void list_node_insert_after(list_t * list, list_node_t * current, list_node_t * new);
void list_node_insert_before(list_t * list, list_node_t * current, list_node_t * new);

list_node_t *list_node_detach(list_t * list, list_node_t * node);

list_t *list_merge_sorted(const list_t * list1, const list_t * list2,
						  long (*comparator)(void *node1_data, void *node2_data));
list_t *list_merge_k_sorted(int amount, list_t ** lists, long (*comparator)(void *node1_data, void *node2_data));

list_node_t *list_search(const list_t * list, int (*is_target)(void *node_data));

#endif
