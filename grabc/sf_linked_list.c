#include "sf_linked_list.h"

void
sf_linked_list_init (SfLinkedListItem *item)
{
  item->next = item;
  item->prev = item;
}

void
sf_linked_list_add_after (SfLinkedListItem *after_this,
                          SfLinkedListItem *item)
{
  item->next = after_this->next;
  after_this->next = item;
  item->prev = after_this;
}

void
sf_linked_list_add_before (SfLinkedListItem *before_this,
                           SfLinkedListItem *item)
{
  item->prev = before_this->prev;
  before_this->prev = item;
  item->next = before_this;
}
