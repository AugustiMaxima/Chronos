#include <stdlib.h>
#include <stdbool.h>
#include <map.h>
#include <bwio.h>

int max(int a, int b){
    return a>b? a: b;
}

void initializeNode(Node* node, int key, void* value){
    node->key = key;
    node->value = value;
    node->height = 1;
    node->left = NULL;
    node->right = NULL;
}

void freeNode(Map* map, Node* node){
    push(&(map->freeQueue), node);
}

void initializeMap(Map* map){
    map->root = NULL;
    initializeQueue(&(map->freeQueue));
    int i=0;
    for(i=0;i<MAP_SIZE;i++){
        push(&(map->freeQueue), map->nodes + i);
    }
}

void updateHeight(Node* node){
    node->height = max(node->left? node->left->height : 0, node->right? node->right->height : 0) + 1;
    //bwprintf(COM2, "Height update:%x %d\r\n",node, node->height);
}

Node* rotation(Node* position){

    int lh = position->left ? position->left->height : 0;
    int rh = position->right? position->right->height : 0;

    int bal = lh - rh;
    if(bal>1){
        //zig zag left
        //      o top
        //  o outlier
        //      o root
        int lrh = position->left->right? position->left->right->height : 0;
        int llh = position->left->left? position->left->left->height : 0;
        if(lrh > llh){
            Node* outlier = position->left;
            Node* root = outlier->right;
            outlier->right = root->left;
            position->left = root->right;
            root->left = outlier;
            root->right = position;
            updateHeight(outlier);
            updateHeight(position);
            updateHeight(root);
            return root;
        } else {//straight line
            Node* median  = position->left;
            position->left = median->right;
            median->right = position;
            updateHeight(position);
            updateHeight(median);
            return median;
        }
    } else if(bal<-1){
        int rlh = position->right->left? position->right->left->height : 0;
        int rrh = position->right->right? position->right->right->height : 0;
        if(rlh > rrh){
            Node* outlier = position->right;
            Node* root = outlier->left;
            outlier->left = root->right;
            position->right = root->left;
            root->right = outlier;
            root->left = position;
            updateHeight(outlier);
            updateHeight(position);
            updateHeight(root);
            return root;

        } else {
            Node* median  = position->right;
            position->right = median->left;
            median->left = position;
            updateHeight(position);
            updateHeight(median);
            return median;
        }
    }
    updateHeight(position);
    return position;
}


Node* insertNode(Node* position, Node* node){
    if(!position)
        return node;
    if(position->key>node->key){
        position->left = insertNode(position->left, node);
    } else {
        position->right = insertNode(position->right, node);
    }
    return rotation(position);
}

Node* promote(Node* position, bool left){
    if(!position)
        return NULL;
    Node* chosen = NULL;
    if(left){
        if(position->left){
            if(position->left->left){
                chosen = promote(position->left, left);
            } else {
                //end of the line, the chosen one
                chosen = position->left;
                position->left = chosen->right;
            }
        }
    } else {
        if(position->right){
            if(position->right->right){
                chosen = promote(position->right, left);
            } else {
                //end of the line, the chosen one
                chosen = position->right;
                position->right = chosen->left;
            }
        }
    }
    updateHeight(position);
    return chosen;
}

Node* successor(Map* map, Node* position){
    Node* candidate = NULL;
    if(position->right){
        if(!position->right->left){
            candidate = position->right;
            candidate->left = position->left;
        } else {
            candidate = promote(position->right, true);
            candidate->left = position->left;
            candidate->right = position->right;
        }
    } else if(position->left){
        if(!position->left->right){
            candidate = position->left;
            candidate->right = position->right;
        } else {
            candidate = promote(position->left, false);
            candidate->left = position->left;
            candidate->right = position->right;
        }
    }
    freeNode(map, position);
    updateHeight(candidate);
    return candidate;
}

Node* removeNode(Map* map, Node* position, int key){
    if(!position)
        return NULL;
    if(position->key>key){
        position->left = removeNode(map, position->left, key);

    } else if(position->key == key) {
        position = successor(map, position);
    } else {
        position->right = removeNode(map, position->right, key);
    }
    updateHeight(position);
    return rotation(position);
}

void* search(Node* node, int key){
    if(!node)
        return NULL;
    if(node->key>key){
        return search(node->left, key);
    } else if (node->key==key) {
        return node->value;
    } else{
        return search(node->right, key);
    }
}

int insertMap(Map* map, int key, void* value){
    Node* node = pop(&(map->freeQueue));
    if(!node)
        return -1;
    initializeNode(node, key, value);
    map->root = insertNode(map->root, node);
}

void* getMap(Map* map, int key){
    return search(map->root, key);
}

void removeMap(Map* map, int key){
    map->root = removeNode(map, map->root, key);
}

void debugTree(Node* node){
    if(!node) return;
    bwprintf(COM2, " %d " ,node->key);
    debugTree(node->left);
    debugTree(node->right);
}
void printTree(Map* map){
    bwprintf(COM2,"Printing trees\r\n");
    debugTree(map->root);
    bwprintf(COM2,"Doneish\r\n");
}
