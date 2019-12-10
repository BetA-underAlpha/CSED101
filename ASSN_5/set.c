#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "set.h"

void addSet(Set *baseSet, FILE *fp) {
    int i = 0, setSize = 0, tmp = 0;
    char setName[21] = "";
    Set *newSet;

    fscanf(fp, "%s", setName);
    fscanf(fp, "%d", &(setSize));

    newSet = appendSet(baseSet, setName);
    if (newSet != 0 && 0 <= setSize) {
        for (i = 0; i < setSize; i++) {
            fscanf(fp, "%d", &tmp);
            addElement(newSet, tmp);
        }
    }
}

void showSet(Set *baseSet, char *setName) {
    Set *setTarget = baseSet->next;
    int isPrint;
    while (setTarget != NULL) {
        isPrint = (strcmp(setName, "") == 0) ? (1) : (strcmp(setName, setTarget->set_name) == 0);
        (isPrint) && (showElements(setTarget));
        setTarget = setTarget->next;
    }
}

int showElements(Set *set) {
    Element *eleTarget = set->ele_head;
    printf("%s : ", set->set_name);
    while (eleTarget != NULL) {
        printf("%d ", eleTarget->data);
        eleTarget = eleTarget->next;
    }
    printf("\n");

    return 1;
}

int isElement(Set *set, int target) {
    Element *e = set->ele_head;
    while (e != NULL) {
        if (e->data == target) {
            return 1;
        }
        e = e->next;
    }
    return 0;
}

int addElement(Set *set, int data) {
    Element *prevEle = set->ele_head;
    Element *tmp;
    if (!isElement(set, data)) {
        tmp = (Element *) malloc(sizeof(Element));
        tmp->data = data;
        if (set->set_size == 0 || data < set->ele_head->data) {
            tmp->next = set->ele_head;
            set->ele_head = tmp;
        } else {
            while (prevEle->next != NULL && (prevEle->next->data < data)) {
                prevEle = prevEle->next;
            }
            tmp->next = prevEle->next;
            prevEle->next = tmp;
        }
        (set->set_size)++;
    }
    return 1;
}

int popElement(Set *set, int data) {
    Element *cur = set->ele_head;
    Element *tmp;

    if (cur->data == data) {
        tmp = set->ele_head;
        set->ele_head = tmp->next;
        free(tmp);
        (set->set_size)--;
    } else {
        while (cur != NULL && cur->next != NULL) {
            if (cur->next->data == data) {
                tmp = cur->next;
                cur->next = tmp->next;
                free(tmp);
                (set->set_size)--;
                break;
            }
            cur = cur->next;
        }
    }
    return 1;
}

int clearSet(Set *baseSet, Set *set) {
    Set *cur = baseSet->next;
    Set *tmp;
    if (set == baseSet->next) {
        tmp = baseSet->next;
        baseSet->next = tmp->next;
        clearElements(tmp->ele_head);
        free(tmp);
        return 1;
    } else {
        while (cur->next != NULL) {
            if (cur->next == set) {
                tmp = cur->next;
                cur->next = tmp->next;
                clearElements(tmp->ele_head);
                free(tmp);
                return 1;
            }
            cur = cur->next;
        }
    }
    return 0;
}

void clearElements(Element *baseEle) {
    Element *cur = baseEle;
    Element *tmp;
    while (cur != NULL) {
        tmp = cur;
        cur = cur->next;
        free(tmp);
    }
}

int clearSetIn(Set *baseSet, char *setName) {
    Set *set = getSetByName(baseSet, setName);
    if (set == 0) return 0;
    return clearSet(baseSet, set);
}

void clearAllSet(Set *baseSet) {
    Set *set = baseSet->next, *tmp;
    while (set != NULL) {
        tmp = set->next;
        clearElements(set->ele_head);
        free(set);
        set = tmp;
    }
}

void initSet(Set *set) {
    set->set_size = 0;
    set->next = NULL;
    set->ele_head = NULL;
}

int executer(int (*f)(Set *, int), Set *baseSet, char *setName, int target) {
    Set *set = getSetByName(baseSet, setName);
    if (set != 0) {
        return f(set, target);
    }
    return 0;
}

Set *getSetByName(Set *baseSet, char *setName) {
    Set *cur = baseSet->next;
    while (cur != NULL) {
        if (strcmp(cur->set_name, setName) == 0) {
            return cur;
        }
        cur = cur->next;
    }
    return 0;
}

Set *appendSet(Set *baseSet, char *setName) {
    Set *newSet, *prevSet = baseSet;
    if (getSetByName(baseSet, setName) != 0) {
        return 0;
    } else {
        newSet = (Set *) malloc(sizeof(Set));
        while (prevSet->next != NULL) {
            prevSet = prevSet->next;
        }
        prevSet->next = newSet;
        newSet->set_size = 0;
        newSet->next = NULL;
        newSet->ele_head = NULL;
        strcpy(newSet->set_name, setName);
        return newSet;
    }
}

/////////

Set *setUnion(Set *baseSet, char *targetName_1, char *targetName_2, char *resultName) {
    Set *targetSet_1 = getSetByName(baseSet, targetName_1), *targetSet_2 = getSetByName(baseSet,
                                                                                        targetName_2), *resultSet;
    Element *ele1, *ele2, **minEle;
    if ((targetSet_1 == 0) || (targetSet_2 == 0)) return 0;
    ele1 = targetSet_1->ele_head;
    ele2 = targetSet_2->ele_head;
    resultSet = appendSet(baseSet, resultName);
    if (resultSet == 0) return 0;
    while (ele1 != NULL || ele2 != NULL) {
        minEle = minElement(&ele1, &ele2);
        addElement(resultSet, (*minEle)->data);
        (*minEle) = (*minEle)->next;
    }
    return resultSet;
}

Set *setDiffInter(Set *baseSet, char *srcName, char *targetName, char *resultName, int isDiff) {
    Set *srcSet = getSetByName(baseSet, srcName), *targetSet = getSetByName(baseSet, targetName), *resultSet;
    Element *srcEle, *targetEle;
    int isFit;
    if ((srcSet == 0) || (targetSet == 0)) return 0;
    targetEle = targetSet->ele_head;
    resultSet = appendSet(baseSet, resultName);
    if(resultSet == 0) return 0;
    for (srcEle = srcSet->ele_head; srcEle != NULL; srcEle = srcEle->next) {
        if(targetEle == NULL){
            isFit = 1;
        } else {
            while ((targetEle->next != NULL) && targetEle->data < srcEle->data) targetEle = targetEle->next;
            isFit = ((isDiff) ? (srcEle->data != targetEle->data) : (srcEle->data == targetEle->data));
        }
        (isFit) && (addElement(resultSet, srcEle->data));

    }

    return resultSet;
}

int isSubset_Disjoint(Set *baseSet, char *setName_1, char *setName_2, int isSub) {
    int tmp = 0;
    char setName[31] = "";
    makeUniqName(baseSet, setName);
    Set *resultSet = setDiffInter(baseSet, setName_1, setName_2, setName, isSub);
    if (resultSet == 0) return 0;
    tmp = (resultSet->set_size == 0);
    clearSet(baseSet, resultSet);
    return tmp;
}

void makeUniqName(Set *baseSet, char *setName) {
    int i = 0;
    do {
        i = 0;
        while (i < 21 && (setName[i++] = (char) (rand() % 90 + 33)));
    } while (getSetByName(baseSet, setName) != 0);
}

Element ** minElement(Element **a, Element **b) {
    if (*a == NULL && *b != NULL) return b;
    else if (*a != NULL && *b == NULL) return a;
    else if (*a == NULL && *b == NULL) return NULL;
    else return ((*a)->data <= (*b)->data) ? (a) : (b);
}