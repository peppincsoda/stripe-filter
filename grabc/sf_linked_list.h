#ifndef _SF_LINKED_LIST_H_
#define _SF_LINKED_LIST_H_

typedef struct _SfLinkedListItem SfLinkedListItem;

struct _SfLinkedListItem
{
  SfLinkedListItem *prev;
  SfLinkedListItem *next;
};

void
sf_linked_list_init (SfLinkedListItem *item);

void
sf_linked_list_add_after (SfLinkedListItem *after_this,
                          SfLinkedListItem *item);

void
sf_linked_list_add_before (SfLinkedListItem *before_this,
                           SfLinkedListItem *item);

#endif  //  _SF_LINKED_LIST_H_
