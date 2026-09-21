#if !defined(BASE_LIST_H)
#define BASE_LIST_H

#define sll_push_front_n(head, tail, node, next)    ((node)->next = (head), ((head) == 0 ? (tail) = (node) : 0), (head) = (node))
#define sll_push_back_n(head, tail, node, next)     ((node)->next = 0, ((head) == 0 ? ((head) = (node)) : ((tail)->next = (node))), (tail) = (node))
#define sll_pop_front_n(head, tail, next)           ((head) == (tail) ? ((head) = (tail) = 0) : ((head) = (head)->next))

#define sll_push_front(head, tail, node)    sll_push_front_n(head, tail, node, next)
#define sll_push_back(head, tail, node)     sll_push_back_n(head, tail, node, next)
#define sll_pop_front(head, tail)           sll_pop_front_n(head, tail, next)

#define sll_stack_push(head, tail, node)    sll_push_front(head, tail, node)
#define sll_stack_pop(head, tail)           sll_pop_front(head, tail)

#define sll_queue_push(head, tail, node)    sll_push_back(head, tail, node)
#define sll_queue_pop(head, tail)           sll_pop_front(head, tail)

#define dll_push_back_np(head, tail, node, next, prev)                         \
    ((head) == 0 ? ((head) = (tail) = (node), (node)->next = (node)->prev = 0) \
                 : ((node)->prev = (tail), (tail)->next = (node), (tail) = (node), (node)->next = 0))

#define dll_remove_np(head, tail, node, next, prev)                                                            \
    ((head) == (node) ? ((head) == (tail) ? ((head) = (tail) = 0) : ((head) = (head)->next, (head)->prev = 0)) \
                      : ((tail) == (node) ? ((tail) = (tail)->prev, (tail)->next = 0)                          \
                                          : ((node)->next->prev = (node)->prev, (node)->prev->next = (node)->next)))

#define dll_push_front_np(head, tail, node, next, prev) dll_push_back_np(tail, head, node, prev, next)

#define dll_push_back(head, tail, node) dll_push_back_np(head, tail, node, next, prev)
#define dll_push_front(head, tail, node) dll_push_front_np(head, tail, node, next, prev)
#define dll_remove(head, tail, node) dll_remove_np(head, tail, node, next, prev)

#endif /* BASE_LIST_H */