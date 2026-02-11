#pragma once
#ifndef TWARE_LIST_H
#define TWARE_LIST_H

#include <cstddef>

class tNode
{
public:
    tNode() 
    {
        Next = NULL;
        Prev = NULL;
    }
    ~tNode() {}
    tNode* GetNext() 
    {
        return Next;
    }
    tNode* GetPrev() 
    {
        return Prev;
    }

    tNode* Remove()
    {
        tNode* next_node = Next;
        tNode* prev_node = Prev;
        prev_node->Next = next_node;
        next_node->Prev = prev_node;
        return this;
    }

    tNode* AddAfter(tNode* insert_point)
    {
        tNode* new_prev = insert_point;
        tNode* new_next = insert_point->Next;
        new_prev->Next = this;
        new_next->Prev = this;
        Prev = new_prev;
        Next = new_next;
        return this;
    }

    tNode* AddBefore(tNode* insert_point)
    {
        tNode* new_prev = insert_point->Prev;
        tNode* new_next = insert_point;
        new_prev->Next = this;
        new_next->Prev = this;
        Prev = new_prev;
        Next = new_next;
        return this;
    }

    tNode* Next;
    tNode* Prev;
};



template <class T>
class tTNode : public tNode
{
public:
    tTNode() : tNode() {}
    ~tTNode() {}

    T* GetNext() 
    {
        return (T*)(tNode::GetNext());
    }
    T* GetPrev() 
    {
        return (T*)(tNode::GetPrev());
    }

    T* Remove() 
    {
        return (T*)(tNode::Remove());
    }

    T* AddAfter(T* insert_point) 
    {
        return (T*)(tNode::AddAfter(insert_point));
    }
    T* AddBefore(T* insert_point) 
    {
        return (T*)(tNode::AddBefore(insert_point));
    }
};



class tList
{
public:
    tList() 
    {
        HeadNode.Next = &this->HeadNode; 
        HeadNode.Prev = &this->HeadNode;
    }
    void InitList() 
    {
        HeadNode.Next = &this->HeadNode; 
        HeadNode.Prev = &this->HeadNode;
    }

    int IsEmpty() 
    {
        return (HeadNode.GetNext() == &this->HeadNode);
    }

    tNode* EndOfList() 
    {
        return &this->HeadNode;
    }

    tNode* GetHead() 
    {
        return HeadNode.GetNext();
    }
    tNode* GetTail() 
    {
        return HeadNode.GetPrev();
    }

    tNode* GetNextCircular(tNode* node) 
    {
        node = node->GetNext(); 
        if (node == EndOfList())
            node = GetHead(); 
        return node;
    }
    tNode* GetPrevCircular(tNode* node) 
    {
        node = node->GetPrev(); 
        if (node == EndOfList()) 
            node = GetTail(); 
        return node;
    }

    tNode* AddHead(tNode* node) 
    {
        return node->AddAfter(&this->HeadNode);
    }
    tNode* AddTail(tNode* node) 
    {
        return node->AddBefore(&this->HeadNode);
    }

    tNode* AddBefore(tNode* insert_point, tNode* node) 
    {
        return node->AddBefore(insert_point);
    }
    tNode* AddAfter(tNode* insert_point, tNode* node) 
    {
        return node->AddAfter(insert_point);
    }

    tNode* Remove(tNode* node) 
    {
        return node->Remove();
    }
    tNode* RemoveHead() 
    {
        return GetHead()->Remove();
    }
    tNode* RemoveTail() 
    {
        return GetTail()->Remove();
    }

    void AddTail(tList* list);
    void AddHead(tList* list);
    tNode* GetNode(int ordinal_number);

    int GetNodeNumber(tNode* node) 
    {
        return TraversetList(node) - 1;
    }

    int IsInList(tNode* node) 
    {
        return TraversetList(node);
    }

    int CountElements() 
    {
        return TraversetList(0);
    }

private:
    int TraversetList(tNode* match_node);

    tNode HeadNode;
};



template <class T>
class tTList : public tList
{
public:
    ~tTList() 
    {
        while (!IsEmpty()) 
            delete RemoveHead();
    }

    void DeleteAllElements() 
    {
        while (!IsEmpty()) 
            delete RemoveHead();
    }

    T* EndOfList() 
    {
        return (T*)(tList::EndOfList());
    }
    T* GetHead() 
    {
        return (T*)(tList::GetHead());
    }
    T* GetTail() 
    {
        return (T*)(tList::GetTail());
    }
    T* GetNextCircular(tNode* node) 
    {
        return (T*)(tList::GetNextCircular(node));
    }
    T* GetPrevCircular(tNode* node) 
    {
        return (T*)(tList::GetPrevCircular(node));
    }
    T* AddHead(tNode* node) 
    {
        return (T*)(tList::AddHead(node));
    }
    T* AddTail(tNode* node) 
    {
        return (T*)(tList::AddTail(node));
    }
    T* AddBefore(tNode* insert_point, tNode* node) 
    {
        return (T*)(tList::AddBefore(insert_point, node));
    }
    T* AddAfter(tNode* insert_point, tNode* node) 
    {
        return (T*)(tList::AddAfter(insert_point, node));
    }
    T* Remove(tNode* node) 
    {
        return (T*)(tList::Remove(node));
    }
    T* RemoveHead() 
    {
        return (T*)(tList::RemoveHead());
    }
    T* RemoveTail() 
    {
        return (T*)(tList::RemoveTail());
    }
    T* GetNode(int ordinal_number) 
    {
        return (T*)(tList::GetNode(ordinal_number));
    }
    void AddHead(tList* list) 
    {
        tList::AddHead(list);
    }
    void AddTail(tList* list) 
    {
        tList::AddTail(list);
    }
};

#endif // TWARE_LIST_H
