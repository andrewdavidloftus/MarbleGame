#include <stdlib.h>
#include <stdio.h>
#include "doublely_linked_list.h"

DLinkedList* create_dlinkedlist(void) {
    DLinkedList* newList = (DLinkedList*)malloc(sizeof(DLinkedList));
    newList->head = NULL;
    newList->tail = NULL;
    newList->current = NULL;
    newList->size = 0;
    return newList;
}

LLNode* create_llnode(void* data) {
    LLNode* newNode = (LLNode*)malloc(sizeof(LLNode));
    newNode->data = data;
    newNode->previous = NULL;
    newNode->next = NULL;
    return newNode;
}


void insertHead(DLinkedList* dLinkedList, void* data){
  LLNode* newNode = create_llnode(data);
  dLinkedList->size++;
  if(dLinkedList->head == NULL){ // Inserting into empty list
    dLinkedList->head = newNode;
    dLinkedList->tail = newNode;
  }else{
    newNode->next = dLinkedList->head;
    (dLinkedList->head)->previous = newNode;
    dLinkedList->head = newNode;
  }
}



void insertTail(DLinkedList* dLinkedList, void* data){
  LLNode* newNode = create_llnode(data);
  dLinkedList->size++;
  if(dLinkedList->head == NULL){ // Inserting into empty list
    dLinkedList->head = newNode;
    dLinkedList->tail = newNode;
  }else{
    newNode->previous = dLinkedList->tail;
    (dLinkedList->tail)->next = newNode;
    dLinkedList->tail = newNode;
  }
}

int insertAfter(DLinkedList* dLinkedList, void* newData){
    if(dLinkedList->current == NULL){
        return 0;
    }
    dLinkedList->size++;
    if (dLinkedList->current == dLinkedList->tail) { // Handles adding after tail
        LLNode* newNode = create_llnode(newData);
        newNode->previous = dLinkedList->current;
        (dLinkedList->current)->next = newNode;
        dLinkedList->tail = (dLinkedList->current)->next;
        return 1;
    } else {
        LLNode* newNode = create_llnode(newData);
        newNode->previous = dLinkedList->current;
        newNode->next = (dLinkedList->current)->next;
        ((dLinkedList->current)->next)->previous = newNode;
        (dLinkedList->current)->next = newNode;
        return 1;
    }
}


int insertBefore(DLinkedList* dLinkedList, void* newData){
    if(dLinkedList->current == NULL){
        return 0;
    }
    dLinkedList->size++;
    if (dLinkedList->current == dLinkedList->head) { // Handles adding before head
        LLNode* newNode = create_llnode(newData);
        newNode->next = dLinkedList->current;
        (dLinkedList->current)->previous = newNode;
        dLinkedList->head = (dLinkedList->current)->previous;
        return 1;
    } else {
        LLNode* newNode = create_llnode(newData);
        newNode->next = dLinkedList->current;
        newNode->previous = (dLinkedList->current)->previous;
        ((dLinkedList->current)->previous)->next = newNode;
        (dLinkedList->current)->previous = newNode;
        return 1;
    }
}


void* deleteBackward(DLinkedList* dLinkedList){
    if (dLinkedList->current == NULL) {
        return NULL;
    }
    dLinkedList->size--;
    LLNode* current = dLinkedList->current;
    if (current == dLinkedList->head){
        if (current == dLinkedList->tail) {
            dLinkedList->head = NULL;
            dLinkedList->tail = NULL;
            dLinkedList->current = NULL;
            free(current->data);
            free(current);
            return NULL;
        }
        dLinkedList->head = current->next;
        (dLinkedList->head)->previous = NULL;
        dLinkedList->current = NULL;
        free(current->data);
        free(current);
        return NULL;
    } else {
        (current->previous)->next = current->next;
        if (current != dLinkedList->tail) {
            (current->next)->previous = current->previous;
        } else {
            dLinkedList->tail = current->previous;
        }
        dLinkedList->current = current->previous;
        free(current->data);
        free(current);
        return (dLinkedList->current)->data;
    }
}



void* deleteForward(DLinkedList* dLinkedList){
    if (dLinkedList->current == NULL) {
        return NULL;
    }
    dLinkedList->size--;
    LLNode* current = dLinkedList->current;
    if (current == dLinkedList->tail){
        if (current == dLinkedList->head) {
            dLinkedList->tail = NULL;
            dLinkedList->head = NULL;
            dLinkedList->current = NULL;
            free(current->data);
            free(current);
            return NULL;
        }
        dLinkedList->tail = current->previous;
        (dLinkedList->tail)->next = NULL;
        dLinkedList->current = NULL;
        free(current->data);
        free(current);
        return NULL;
    } else {
        (current->next)->previous = current->previous;
        if (current != dLinkedList->head) {
            (current->previous)->next = current->next;
        } else {
            dLinkedList->head = current->next;
        }
        dLinkedList->current = current->next;
        free(current->data);
        free(current);
        return (dLinkedList->current)->data;
    }
}


void* removeBackward(DLinkedList* dLinkedList){
    if (dLinkedList->current == NULL) {
        return NULL;
    }
    dLinkedList->size--;
    LLNode* current = dLinkedList->current;
    if (current == dLinkedList->head){
        if (current == dLinkedList->tail) {
            dLinkedList->head = NULL;
            dLinkedList->tail = NULL;
            dLinkedList->current = NULL;
            void* value = current->data;
            free(current);
            return value;
        }
        dLinkedList->head = current->next;
        (dLinkedList->head)->previous = NULL;
        dLinkedList->current = NULL;
        void* value = current->data;
        free(current);
        return value;
    } else {
        (current->previous)->next = current->next;
        if (current != dLinkedList->tail) {
            (current->next)->previous = current->previous;
        } else {
            dLinkedList->tail = current->previous;
        }
        dLinkedList->current = current->previous;
        void* value = current->data;
        free(current);
        return value;
    }
}

void* removeForward(DLinkedList* dLinkedList){
    if (dLinkedList->current == NULL) {
        return NULL;
    }
    dLinkedList->size--;
    LLNode* current = dLinkedList->current;
    if (current == dLinkedList->tail){
        if (current == dLinkedList->head) {
            dLinkedList->tail = NULL;
            dLinkedList->head = NULL;
            dLinkedList->current = NULL;
            void* value = current->data;
            free(current);
            return value;
        }
        dLinkedList->tail = current->previous;
        (dLinkedList->tail)->next = NULL;
        dLinkedList->current = NULL;
        void* value = current->data;
        free(current);
        return value;
    } else {
        (current->next)->previous = current->previous;
        if (current != dLinkedList->head) {
            (current->previous)->next = current->next;
        } else {
            dLinkedList->head = current->next;
        }
        dLinkedList->current = current->next;
        void* value = current->data;
        free(current);
        return value;
    }
}


void destroyList(DLinkedList* dLinkedList){
  if(dLinkedList->head != NULL){
    getHead(dLinkedList);
    dLinkedList->current = dLinkedList->head;
    while(deleteForward(dLinkedList)){};
  }
  free(dLinkedList);
}


void* getHead(DLinkedList* dLinkedList){
    if (dLinkedList->head == NULL) {
        return NULL;
    } else {
        dLinkedList->current = dLinkedList->head;
        return (dLinkedList->current)->data;
    }
}



void* getTail(DLinkedList* dLinkedList){
    if (dLinkedList->tail == NULL) {
        return NULL;
    } else {
        dLinkedList->current = dLinkedList->tail;
        return (dLinkedList->current)->data;
    }
}



void* getCurrent(DLinkedList* dLinkedList){
    if (dLinkedList->current == NULL) {
        return NULL;
    } else {
        return (dLinkedList->current)->data;
    }
}



void* getNext(DLinkedList* dLinkedList){
    if (dLinkedList->current == NULL) {
        return NULL;
    } else if (dLinkedList->current == dLinkedList->tail) {
        dLinkedList->current = NULL;
        return NULL;
    } else {
        dLinkedList->current = (dLinkedList->current)->next;
        return (dLinkedList->current)->data;
    }
}



void* getPrevious(DLinkedList* dLinkedList){
    if (dLinkedList->current == dLinkedList->head) {
        dLinkedList->current = NULL;
        return NULL;
    } else {
        dLinkedList->current = (dLinkedList->current)->previous;
        return (dLinkedList->current)->data;
    }
}


int getSize(DLinkedList* dLinkedList){
    return dLinkedList->size;
}




