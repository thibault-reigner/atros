/**
 * \file include/kernel/list.h
 * \brief Contains useful macros to manipulate simple, doubly-linked and doubly-linked circular lists.
 *
 * These macros are type-independant, the lists' elements are expected to
 * have "prev" (if recquired) and "next" fields.
 */
#ifndef KERNEL_LIST_H
#define KERNEL_LIST_H

#include <types.h>
#include <kernel/panic.h>

#ifndef __ASM__

/****************************************************************************
 * The following macros are used to manipulate simple lists: where each
 * element is linked to the next one (last one is linked to the empty list).
 * The previous and next fields'names can be specified.
 ****************************************************************************/

#define list_is_empty_generic(list, next)	\
  ((list) == NULL)

#define list_is_singleton_generic(list, next)	\
  ((list)->next == NULL)

#define list_push_head_generic(list, el, next) ({	\
      (el)->next = (list);				\
      (list) = (el);					\
    })

#define list_delete_head_generic(list,head, next) ({	\
      if(list_is_singleton_generic(list, next))		\
	{						\
	  (list) = NULL;				\
	}						\
      else						\
	{						\
	  (list) = (head)->next;			\
	  (head)->next = NULL;				\
	}						\
    })

#define list_pop_head_generic(list, next) ({		\
      typeof(list) __el2pop = (list);			\
      list_delete_head_generic(list,__el2pop, next);	\
      __el2pop;						\
    })

#define list_delete_el_generic(list, prev_el, el, next) ({	\
      KASSERT( !((prev_el) == NULL && (list) != (el)));		\
      if ((list) == (el) || (prev_el) == NULL)			\
	{							\
	  (list) = (el)->next;					\
	}							\
      else							\
	{							\
	  (prev_el)->next = (el)->next;				\
	}							\
      (el)->next = NULL;					\
    })

#define list_insert_after_generic(list, prev_el, el, next) ({	\
      if ((prev_el) != NULL)				\
	{						\
	  (el)->next = (prev_el)->next;			\
	  (prev_el)->next = (el);			\
	}						\
      else						\
	{						\
	  list_push_head_generic(list, el, next);	\
	}						\
    })

#define list_insert_ordered_generic(list, el, next, cmp) ({		\
      if (list_is_empty(list))						\
	{								\
	  list_push_head_generic(list, el, next);			\
	}								\
	else if (cmp(el, list) <= 0 )					\
	  {								\
	    list_push_head_generic(list, el, next);			\
	  }								\
	else								\
	  {								\
	    typeof(list) __prev = NULL;					\
	    typeof(list) __current = (list);				\
	    while (cmp(el, __current) > 0)				\
	      {								\
		__prev = __current;					\
		__current = __current->next;				\
	      }								\
	    list_insert_after_generic(list, __prev, el, next);		\
	  }								\
    })

#define list_length_generic(list, next) ({	\
      uint_t __length = 0;			\
      typeof(list) __current = (list);		\
      while (__current != NULL)			\
	{					\
	  __length++;				\
	  __current = __current->next;		\
	}					\
      __length;					\
    })

/*
 * Non-generic methods to operate on lists:
 * they expect the next field of the lists' elements 
 * to be named next.
 */
#define list_is_empty(list)			\
  (list_is_empty_generic(list, next))
#define list_is_singleton(list)			\
  (list_is_singleton_generic(list, next))
#define list_push_head(list, el)		\
  (list_push_head_generic(list, el, next)) 
#define list_delete_head(list, head)		\
  (list_delete_head_generic(list, head, next))
#define list_pop_head(list)			\
  (list_pop_head_generic(list, next))
#define list_delete_el(list, prev_el, el)		\
  (list_delete_el_generic(list, prev_el, el, next))
#define list_length(list)			\
  (list_length_generic(list, next))
#define list_insert_after(list, prev_el, el)		\
  (list_insert_after_generic(list, prev_el, el, next))
#define list_insert_ordered(list, el, cmp)		\
  (list_insert_ordered_generic(list, el, next, cmp)) 

/****************************************************************************
 * The following macros are used to manipulate non-circular doubly linked lists,
 * where each element is linked to the previous and next ones.
 ****************************************************************************/

#define dlist_is_empty_generic(list, prev, next)	\
  ((list) == NULL)

#define dlist_is_singleton_generic(list, prev, next)	\
  ((list)->next == NULL)

#define dlist_push_head_generic(list, el, prev, next) ({	\
      if(!dlist_is_empty_generic(list, prev, next))	\
	{							\
	  (list)->prev = (el);					\
	}							\
      (el)->prev = NULL;					\
      (el)->next = (list);					\
      (list) = (el);						\
    })

#define dlist_delete_head_generic(list, head, prev, next) ({	\
      if(dlist_is_singleton_generic(list, prev, next))	\
	{							\
	  (list) = NULL;					\
	}							\
      else							\
	{							\
	  (list) = (head)->next;				\
	  (list)->prev = NULL;					\
	  (head)->prev = NULL;					\
	  (head)->next = NULL;					\
	}							\
    })

#define dlist_pop_head_generic(list, prev, next) ({	\
      typeof(list) __el2pop = (list);			\
      dlist_delete_head_generic(list, __el2pop, next);	\
      __el2pop;						\
    })

#define dlist_delete_el_generic(list, el, prev, next) ({	\
      if((list) != (el))					\
	{							\
	  (el)->prev->next = (el)->next;			\
	  if((el)->next != NULL)				\
	    {							\
	      (el)->next->prev = (el)->prev;			\
	    }							\
	  (el)->prev = NULL;					\
	  (el)->next = NULL;					\
	}							\
      else{							\
	dlist_delete_head_generic(list,el,prev,next);		\
      }								\
    })

/*
 * Non-generic methods to operate on doubly-linked lists:
 * they expect the next field of the lists' elements 
 * to be named next.
 */
#define dlist_is_empty(list)			\
  (dlist_is_empty_generic(list, prev, next))
#define dlist_is_singleton(list)			\
  (dlist_is_singleton_generic(list, prev, next))
#define dlist_push_head(list, el)		\
  (dlist_push_head_generic(list, el, prev, next)) 
#define dlist_delete_head(list, head)		\
  (dlist_delete_head_generic(list, head, prev, next))
#define dlist_pop_head(list)			\
  (dlist_pop_head_generic(list, prev, next))
#define dlist_delete_el(list, prev_el, el)		\
  (dlist_delete_el_generic(list, prev_el, el, prev, next))
#define dlist_length(list)			\
  (dlist_length_generic(list, prev, next))

/****************************************************************************
 * The following macros are used to manipulate circular doubly linked lists,
 * where each element is linked to the previous and next ones.
 ****************************************************************************/

#define clist_is_empty_generic(list, prev, next)	\
  ((list) == NULL)

#define clist_is_singleton_generic(list, prev, next)		\
  (((list)->prev == (list)) && ((list)->next == (list)))

#define clist_push_head_generic(list, el, prev, next) ({	\
      if(!clist_is_empty(list))					\
	{							\
	  (el)->next = (list);					\
	  (el)->prev = (list)->prev;				\
	  ((list)->prev)->next = (el);				\
	  (list)->prev = (el);					\
	}							\
      else							\
	{							\
	  (el)->next = (el);					\
	  (el)->prev = (el);					\
	}							\
      (list) = (el);						\
    })

#define clist_push_tail_generic(list, el, prev, next) ({	\
      if(!clist_is_empty(list))					\
	{							\
	  (el)->next = (list);					\
	  (el)->prev = (list)->prev;				\
	  ((list)->prev)->next = (el);				\
	  ((list)->prev) = (el);				\
	}							\
      else							\
	{							\
	  (el)->next = (el);					\
	  (el)->prev = (el);					\
	  (list) = (el);					\
	}							\
    })

#define clist_delete_el_generic(list, el, prev, next) ({	\
      if(clist_is_singleton_generic(list, prev, next))		\
	{							\
	  (el)->prev = NULL;					\
	  (el)->next = NULL;					\
	  (list) = NULL;					\
	}							\
      else							\
	{							\
	  ((el)->prev)->next = (el)->next;			\
	  ((el)->next)->prev = (el)->prev;			\
	  if((list) == (el))					\
	    {							\
	      (list) = (list)->next;				\
	    }							\
	}							\
    })

#define clist_pop_head_generic(list, prev, next) ({	\
      typeof(list) el2pop = (list);			\
      clist_delete_el_generic(list,el2pop,prev,next);	\
      el2pop;						\
    })

/**
 * Insert an element in an ordered circular doubly-linked list.
 * cmp(a,b) is a function which compare two elements a and b, and returns:
 * a value < 0 if a < b
 * a value = 0 if a = b
 * a value > 0 if a > b
 */
#define clist_insert_ordered_generic(list, el, prev, next, cmp) ({	\
      if(clist_is_empty(list))						\
	{								\
	  clist_push_head_generic(list, el, prev, next);		\
	}								\
      else if(cmp(el, list) <= 0 )					\
	{								\
	  clist_push_head_generic(list, el, prev, next);		\
	}								\
      else if(cmp(el, list->prev) > 0)					\
	{								\
	  clist_push_tail_generic(list, el, prev, next);		\
	}								\
      else								\
	{								\
	  typeof(list) __current = (list);				\
	  while(cmp(el, __current) > 0)					\
	    {__current = __current->next;}				\
	  clist_push_head_generic(__current, el, prev, next);		\
	}								\
    })

/*
 * Search an element in a circular doubly-linked list.
 * Return TRUE if the element is in the circle list, FALSE otherwise.
 */
#define clist_search_el_generic(list, el, prev, next, cmp) ({	\
      typeof(list) __current = (list);				\
      bool_t __found = FALSE;					\
      if(!clist_is_empty(list))					\
	{							\
	  do{							\
	    if(cmp(__current,(el)) == 0)			\
	      {__found = TRUE;}					\
	    __current = __current->next;			\
	  }while(__currrent != (list) && __found == FALSE);	\
	}							\
      __found;							\
    })

/*
 * Non-generic methods to operate on circular doubly-linked lists:
 * they expect the prev and next fields of the lists' elements
 * to be named prev and next.
 */
#define clist_is_empty(list)			\
  clist_is_empty_generic(list, prev, next)
#define clist_is_singleton(list)		\
  clist_is_singleton_generic(list, prev, next)
#define clist_push_head(list, el)		\
  clist_push_head_generic(list, el, prev, next)
#define clist_push_tail(list, el)		\
  clist_push_tail_generic(list,el,prev,next)
#define clist_delete_el(list, el)		\
  clist_delete_el_generic(list,el,prev,next)
#define clist_pop_head(list)			\
  clist_pop_head_generic(list,prev,next)


#endif //__ASM__

#endif 
