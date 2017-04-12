#include <sfcore/sf_linked_list.h>

SF_CORE_API
void
sf_linked_list_init (SfLinkedListItem *item)
{
  item->next = item;
  item->prev = item;
}

SF_CORE_API
void
sf_linked_list_add_after (SfLinkedListItem *after_this,
                          SfLinkedListItem *item)
{
  item->prev = after_this;
  item->next = after_this->next;
  after_this->next->prev = item;
  after_this->next = item;
}

SF_CORE_API
void
sf_linked_list_add_before (SfLinkedListItem *before_this,
                           SfLinkedListItem *item)
{
  item->prev = before_this->prev;
  item->next = before_this;
  before_this->prev->next = item;
  before_this->prev = item;
}
