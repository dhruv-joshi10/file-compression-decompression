#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define MAX_TREE_HT 100

struct MinHeapNode {
    char data;
    unsigned freq;
    struct MinHeapNode *left, *right;
};

struct MinHeap {
    unsigned size;
    unsigned capacity;
    struct MinHeapNode** array;
};

struct MinHeapNode* newNode(char data, unsigned freq) {
    struct MinHeapNode* temp = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

struct MinHeap* createMinHeap(unsigned capacity) {
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (struct MinHeapNode**)malloc(minHeap->capacity * sizeof(struct MinHeapNode*));
    return minHeap;
}

void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b) {
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

void minHeapify(struct MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

struct MinHeapNode* extractMin(struct MinHeap* minHeap) {
    struct MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

void insertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode) {
    ++minHeap->size;
    int i = minHeap->size - 1;
    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}

int isSizeOne(struct MinHeap* minHeap) {
    return (minHeap->size == 1);
}

struct MinHeap* createAndBuildMinHeap(char data[], int freq[], int size) {
    struct MinHeap* minHeap = createMinHeap(size);
    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(data[i], freq[i]);
    minHeap->size = size;
    for (int i = (minHeap->size - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
    return minHeap;
}

struct MinHeapNode* buildHuffmanTree(char data[], int freq[], int size) {
    struct MinHeapNode *left, *right, *top;
    struct MinHeap* minHeap = createAndBuildMinHeap(data, freq, size);
    while (!isSizeOne(minHeap)) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);
        top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }
    return extractMin(minHeap);
}

int isLeaf(struct MinHeapNode* root) {
    return !(root->left) && !(root->right);
}

void storeCodes(struct MinHeapNode* root, int arr[], int top, char codes[][MAX_TREE_HT]) {
    if (root->left) {
        arr[top] = 0;
        storeCodes(root->left, arr, top + 1, codes);
    }
    if (root->right) {
        arr[top] = 1;
        storeCodes(root->right, arr, top + 1, codes);
    }
    if (isLeaf(root)) {
        int i;
        for (i = 0; i < top; i++) {
            codes[(unsigned char)root->data][i] = '0' + arr[i];
        }
        codes[(unsigned char)root->data][top] = '\0';
    }
}

void printCodes(struct MinHeapNode* root, int arr[], int top) {
    if (root->left) {
        arr[top] = 0;
        printCodes(root->left, arr, top + 1);
    }
    if (root->right) {
        arr[top] = 1;
        printCodes(root->right, arr, top + 1);
    }
    if (isLeaf(root)) {
        printf("%c", root->data);
        // for (int i = 0; i < top; ++i)
            // printf("%d", arr[i]);
        // printf("\n");
    }
    // printf("\n");
}

void HuffmanCodes(char data[], int freq[], int size, char codes[][MAX_TREE_HT]) {
    struct MinHeapNode* root = buildHuffmanTree(data, freq, size);
    int arr[MAX_TREE_HT], top = 0;
    storeCodes(root, arr, top, codes);
    printf("Compressed text: ");
    printCodes(root, arr, top);
    printf("\n\n");
}

void calculateFrequencies(char* str, char* data, int* freq, int* size) {
    int count[256] = {0};
    for (int i = 0; str[i]; i++) {
        count[(unsigned char)str[i]]++;
    }
    int index = 0;
    for (int i = 0; i < 256; i++) {
        if (count[i] > 0) {
            data[index] = (char)i;
            freq[index] = count[i];
            index++;
        }
    }
    *size = index;
}

char* compressData(char* input, char codes[][MAX_TREE_HT]) {
    char* encoded = (char*)malloc(1024 * sizeof(char)); 
    encoded[0] = '\0';
    
    for (int i = 0; input[i]; i++) {
        strcat(encoded, codes[(unsigned char)input[i]]);
    }
    return encoded;
}

void decompress(struct MinHeapNode* root, char* encodedData) {
    struct MinHeapNode* currentNode = root;

    for (int i = 0; encodedData[i] != '\0'; i++) {
        if (encodedData[i] == '0')
            currentNode = currentNode->left;
        else
            currentNode = currentNode->right;

        if (isLeaf(currentNode)) {
            printf("%c", currentNode->data);
            currentNode = root;
        }
    }
    printf("\n");
}

char* readFileIntoArray(const char *filename, size_t *fileSize) {
    FILE *file = fopen(filename, "rb"); 
    if (!file) {
        fprintf(stderr, "Could not open file: %s\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *fileSize = ftell(file);
    fseek(file, 0, SEEK_SET); 

    char *buffer = (char *)malloc(*fileSize + 1); 
    if (!buffer) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    size_t bytesRead = fread(buffer, 1, *fileSize, file);
    buffer[bytesRead] = '\0'; 

    fclose(file);
    return buffer;
}

int main() {
    const char *filename = "testfile"; 
    size_t fileSize = 0;

    char *fileContent = readFileIntoArray(filename, &fileSize);

    if (fileContent) {
        char data[256];
        int freq[256], size;

        printf("TEXT: %s\n\n", fileContent);

        calculateFrequencies(fileContent, data, freq, &size);

        char codes[256][MAX_TREE_HT];
        HuffmanCodes(data, freq, size, codes);

        char* encodedData = compressData(fileContent, codes);
        printf("Encoded Data Binary: %s\n", encodedData);

        struct MinHeapNode* root = buildHuffmanTree(data, freq, size);
        printf("\nDecoded Data: ");
        decompress(root, encodedData);

        free(encodedData);
        free(fileContent);
    }

    return 0;

}

