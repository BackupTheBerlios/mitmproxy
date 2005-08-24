#include "conf.h"
#ifndef EXIT_FAILURE
#define EXIT_FAILURE -1
#endif
#define MAXBUFSIZ 256

struct proxybind *allocProxy (void)
{

  struct proxybind *localp;

  localp = malloc (sizeof (struct proxybind));
  if (localp == NULL)
    { 
      fprintf(stderr,"Allocation failure..Exiting!");
      exit(EXIT_FAILURE);
    }
    

  localp->next = NULL;
  localp->prev = NULL;

  return localp;
}


struct proxybind *
freeProxy (struct proxybind *newadd, struct proxybind *head)
{
  if (newadd->prev)
    newadd->prev->next = newadd->next;
    else 
      head = newadd->next;
  if (newadd->next)
	newadd->next->prev = newadd->prev;
  free (newadd);

  return head;
}

void
freeAll (struct proxybind **head)
{
  struct proxybind *my_head = *head;
  
  while (my_head != NULL)
 {
 my_head = freeProxy(my_head,my_head);
 } 
 
*head = my_head;
}
struct proxybind *
prev (struct proxybind *list)
{
  if (list == NULL)
    return NULL;
  return (list->prev);
}

struct proxybind *
next (struct proxybind *list)
{
  if (list == NULL)
    return NULL;
  return (list->next);
}


struct proxybind *
insertProxy (struct proxybind *head, struct proxybind *x)
{


x->next = head;

if (head)
    head->prev = x;
    
head = x;

x->prev = NULL;

return head;
}





