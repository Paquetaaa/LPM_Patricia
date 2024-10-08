#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include "func.h"

MyAlgoNode *arbre = NULL; // Racine de l'arbre

// Fonction qui permet de convertir mon adresse IP en adresse 32 bits.
unsigned int convertAddrtoBit(char *ip)
{
    struct in_addr addr;

    // Converti l'adresse IP en forme binaire
    if (inet_pton(AF_INET, ip, &addr) != 1)
    {
        printf("Erreur lors de la conversion de l'adresse IP\n");
        return 0;
    }

    // Retourne l'adresse IP en forme binaire, passage en big endian avec ntohl.
    return ntohl(addr.s_addr);
}

// Affiche l'@ IP en binaire, utilisé pour le débuggage
void printBinaryIP(unsigned int ip)
{
    for (int i = 31; i >= 0; i--)
    {
        printf("%d", (ip >> i) & 1);
        if (i % 8 == 0)
        {
            printf(" ");
        }
    }
    printf("\n");
}

// Pour savoir si c'est une feuille, on regarde les deux fils
int is_leaf(MyAlgoNode *node)
{
    if ((node->left == NULL) && (node->right == NULL))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// On parcourt l'arbre tant que notre netmask a des 1. On regarde alors le bit d'@ pour savoir si on doit aller a droite ou a gauche. Enfin on retourne la derniere GW trouvée.
unsigned int rechercher(MyAlgoNode *arbre, unsigned int addr)
{
    unsigned int lastGateway = 0;
    for (int j = 0; j < 32; j++)
    {
        if (arbre == NULL)
        {
            return lastGateway;
        }
        if (arbre->gw != 0)
        {
            lastGateway = arbre->gw;
        }
        if ((addr & (1 << (31 - j))))
        {

            arbre = arbre->right;
        }
        else
        {

            arbre = arbre->left;
        }
    }
    return lastGateway;
}

// Permet d'ajouter toutes les routes dans l'arbre de recherche, si le noeud n'existe pas, on le crée puis on ajoute des noeud tant qu'on a des bit a 1 sur notre netmask
void ajouter(MyAlgoNode **arbre, unsigned int addr, unsigned int gw, unsigned int netmask, unsigned int i)
{

    if (*arbre == NULL)
    {
        *arbre = (MyAlgoNode *)malloc(sizeof(MyAlgoNode));
        (*arbre)->bit = (addr >> (31 - i)) & 1;
        // printf("%d bit ajouté\n",(*arbre)->bit);
        (*arbre)->gw = 0;
        (*arbre)->netmask = 0;
        (*arbre)->left = NULL;
        (*arbre)->right = NULL;
    }
    if ((netmask & i) == 0)
    {
        (*arbre)->gw = gw;
        (*arbre)->netmask = netmask;
        return;
    }
    else if ((addr & i) != 0)
    {
        i = i >> 1;
        ajouter(&(*arbre)->right, addr, gw, netmask, i);
    }
    else
    {
        i = i >> 1;
        ajouter(&(*arbre)->left, addr, gw, netmask, i);
    }
}

void initMyAlgo()
{
    printf("Début insertion des routes\n");
}

void insertMyAlgo(unsigned int addr, unsigned int netmask, unsigned int gw)
{
    unsigned int i = 1 << 31;
    ajouter(&arbre, addr, gw, netmask, i);
}

unsigned int lookupMyAlgo(unsigned int addr)
{
    return rechercher(arbre, addr);
}

// This function must not be modified !!!
int loadFile(char *path)
{
    FILE *fi;
    char s[MAXLEN], *p, *start;
    struct in_addr c;
    unsigned int addr, netmask, gw;

    // first sanity check
    if (!path)
        return 0;
    if ((fi = fopen(path, "r")) == NULL)
        return 0;

    // read file line by line (max = MAXLEN )
    while (fgets(s, MAXLEN, fi) != NULL)
    {
        s[MAXLEN - 1] = 0;
        p = start = s;
        // seek for blank char
        while (*p && (*p != ' '))
            p++;
        if (p > start)
        {
            *p = 0;
            p++;
            if (inet_aton(start, &c) == 0)
                continue;
            addr = htonl(c.s_addr);
        }
        // skip remaining blank char
        while (*p && (*p == ' '))
            p++;

        // stat a new search for blank char
        start = p;
        while (*p && (*p != ' '))
            p++;
        if (p > start)
        {
            *p = 0;
            p++;
            if (inet_aton(start, &c) == 0)
                continue;
            netmask = htonl(c.s_addr);
        }
        // skip remaining blank char
        while (*p && (*p == ' '))
            p++;

        // stat a new search for blank '\n' char
        start = p;
        while (*p && (*p != '\n'))
            p++;
        if (p > start)
        {
            *p = 0;
            p++;
            if (inet_aton(start, &c) == 0)
                continue;
            gw = htonl(c.s_addr);
        }

        // call your function to insert entry to routing table
        insertMyAlgo(addr, netmask, gw);
    }
    printf("Insertion terminée \n");
    fclose(fi);
    return 1;
}

// // only for debug purpose might be modified !
// int main(int argc, char *argv[])
// {
//     char s[MAXLEN];
//     int addr;
//     struct in_addr a;
//     FILE *inputFile = fopen("iptest.", "r");
//     FILE *outputFile = fopen("output.txt", "w");

//     initMyAlgo();
//     if ((argc > 1) && (loadFile(argv[1]))) // Toutes les routes sont loadées
//     {
//         if (inputFile == NULL || outputFile == NULL)
//         {
//             printf("Erreur d'ouverture du fichier\n");
//             return -1;
//         }

//         while (fgets(s, MAXLEN, inputFile) != NULL)
//         {
//             s[MAXLEN - 1] = 0;
//             // printf("adresse cherchée = %s \n", s);
//             if (inet_aton(s, &a) == 0)
//                 continue;
//             addr = htonl(a.s_addr);
//             a.s_addr = htonl(lookupMyAlgo(addr));
//             // printf("GW found = %s\n", inet_ntoa(a));
//             fprintf(outputFile, "%s%s\n", s, inet_ntoa(a));
//         }

//         fclose(inputFile);
//         fclose(outputFile);
//     }
//     return 0;
// }

// int t(int argc, char *argv[])
// {
//     char s[MAXLEN];
//     int addr;
//     struct in_addr a;

//     initMyAlgo();
//     if ((argc > 1) && (loadFile(argv[1])))
//     {

//         while (fgets(s, MAXLEN, stdin) != NULL)
//         {
//             s[MAXLEN - 1] = 0;
//             FILE *retour = fopen("retour", "w");
//             printf("adresse cherchée = %s \n", s);
//             if (inet_aton(s, &a) == 0)
//                 continue;
//             addr = htonl(a.s_addr);
//             a.s_addr = htonl(lookupMyAlgo(addr));
//             printf("GW found = %s\n", inet_ntoa(a));
//             // fprintf(retour, "%s", inet_ntoa(a));
//         }
//     }
//     return 0;
// }