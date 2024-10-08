#define MAXLEN 100
#define NB_ROUTES 1138554

// Création de la structure de noeud pour construire un Patricia Trie
typedef struct MyAlgoNode
{
    unsigned int bit;
    unsigned int netmask;
    unsigned int gw;
    struct MyAlgoNode *left;
    struct MyAlgoNode *right;
} MyAlgoNode;

unsigned int convertAddrtoBit(char *ip);
void printBinaryIP(unsigned int ip);
int is_leaf(MyAlgoNode *node);
unsigned int rechercher(MyAlgoNode *arbre, unsigned int addr);
void ajouter(MyAlgoNode **arbre, unsigned int addr, unsigned int gw, unsigned int netmask, unsigned int i);
void insertMyAlgo(unsigned int addr, unsigned int netmask, unsigned int gw);
void initMyAlgo();
unsigned int lookupMyAlgo(unsigned int addr);
int loadFile(char *path);
