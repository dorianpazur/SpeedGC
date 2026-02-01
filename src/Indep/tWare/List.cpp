#include <tWare/List.h>

tNode *tList::GetNode(int ordinal_number)
{
    int n = 0;
    tNode *node = GetHead();
    while ((node != EndOfList()) && (n != ordinal_number))
    {
        n++;
        node = node->GetNext();
    }
    return node;
}

int tList::TraversetList(tNode *match_node)
{
    tNode *node = GetHead();
    int n = 0;
    while (node != EndOfList())
    {
        if (node == match_node) 
            return n + 1;
        n++;
        node = node->GetNext();
    }
    if (match_node) 
        n = 0;
    return n;
}


void tList::AddTail(tList *list)
{
    tNode *that_head = list->GetHead();
    tNode *that_tail = list->GetTail();
    if (that_head != list->EndOfList())
    {
        tNode *this_tail = GetTail();
        that_tail->Next = &this->HeadNode;
        that_head->Prev = this_tail;
        this_tail->Next = that_head;
        HeadNode.Prev = that_tail;
        list->HeadNode.Next = &list->HeadNode;
        list->HeadNode.Prev = &list->HeadNode;
    }
}


void tList::AddHead(tList *list)
{
    tNode *that_head = list->GetHead();
    tNode *that_tail = list->GetTail();
    if (that_head != list->EndOfList())
    {
        tNode *this_head = GetHead();
        that_head->Prev = &this->HeadNode;
        that_tail->Next = this_head;
        this_head->Prev = that_tail;
        HeadNode.Next = that_head;
        list->HeadNode.Next = &list->HeadNode;
        list->HeadNode.Prev = &list->HeadNode;
    }
}
