#ifndef _SF_LINKED_LIST_H_
#define _SF_LINKED_LIST_H_

#include <sfcore/sf_core_header.h>

typedef struct _SfLinkedListItem SfLinkedListItem;

struct _SfLinkedListItem
{
  SfLinkedListItem *prev;
  SfLinkedListItem *next;
};

SF_CORE_API
void
sf_linked_list_init (SfLinkedListItem *item);

SF_CORE_API
void
sf_linked_list_add_after (SfLinkedListItem *after_this,
                          SfLinkedListItem *item);

SF_CORE_API
void
sf_linked_list_add_before (SfLinkedListItem *before_this,
                           SfLinkedListItem *item);

#endif  //  _SF_LINKED_LIST_H_
