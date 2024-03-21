#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TAMANHO_TABELA 75

typedef struct ItemDados {
    char nome[50];
    char email[100];
    char telefone[15];
    int chave;
} ItemDados;

ItemDados *tabelaHash[TAMANHO_TABELA];

int multiplicacao(ItemDados *d) {
    long long int chave = (long long int)d->chave;
    chave = chave * 2654435761LL;
    return (int)(chave % TAMANHO_TABELA);
}

int segundoHash(ItemDados *d) {
    long long int chave = (long long int)d->chave;
    chave = chave * 31415 + 1;
    return (int)(chave % (TAMANHO_TABELA - 1)) + 1; // Garantir que o segundo hash seja diferente de zero
}

void inicializarTabelaHash() {
    for (int i = 0; i < TAMANHO_TABELA; i++) {
        tabelaHash[i] = NULL;
    }
}

void inserir(ItemDados *item) {
    int chave = item->chave;
    int indiceHash = multiplicacao(item);
    int tamanhoPasso = 7 - (chave % 7);
    int segundoPasso = segundoHash(item);
    int tentativas = 0;

    while (tabelaHash[indiceHash] != NULL && tentativas < TAMANHO_TABELA) {
        indiceHash = (indiceHash + segundoPasso) % TAMANHO_TABELA;
        tentativas++;
    }

    if (tabelaHash[indiceHash] == NULL) {
        tabelaHash[indiceHash] = item;
    } else {
        printf("Tentativas esgotadas. Nao e possivel inserir o item.\n");
    }
}

void listarContatos() {
    printf("\nLista de Contatos:\n");
    for (int i = 0; i < TAMANHO_TABELA; i++) {
        if (tabelaHash[i] != NULL) {
            printf("Chave: %d\n", tabelaHash[i]->chave);
            printf("Nome: %s\n", tabelaHash[i]->nome);
            printf("Email: %s\n", tabelaHash[i]->email);
            printf("Telefone: %s\n\n", tabelaHash[i]->telefone);
        }
    }
}

void editarContato(int chave) {
    for (int i = 0; i < TAMANHO_TABELA; i++) {
        if (tabelaHash[i] != NULL && tabelaHash[i]->chave == chave) {
            printf("Novo nome: ");
            scanf("%s", tabelaHash[i]->nome);
            printf("Novo email: ");
            scanf("%s", tabelaHash[i]->email);
            printf("Novo telefone: ");
            scanf("%s", tabelaHash[i]->telefone);
            printf("Contato editado com sucesso!\n");
            return;
        }
    }
    printf("Contato nao encontrado.\n");
}

void excluirContato(int chave) {
    for (int i = 0; i < TAMANHO_TABELA; i++) {
        if (tabelaHash[i] != NULL && tabelaHash[i]->chave == chave) {
            free(tabelaHash[i]);
            tabelaHash[i] = NULL;
            printf("Contato excluido com sucesso!\n");
            return;
        }
    }
    printf("Contato nao encontrado.\n");
}

void visualizarColisoes() {
    printf("\nColisoes de Entrada:\n");
    for (int i = 0; i < TAMANHO_TABELA; i++) {
        if (tabelaHash[i] != NULL) {
            int indiceHash = multiplicacao(tabelaHash[i]);
            int segundoPasso = segundoHash(tabelaHash[i]);
            if (indiceHash != i) {
                printf("\nChave: %d, houve uma colisao na posicao: %d\n", tabelaHash[i]->chave, i);
                printf("Metodo do duplo hash, indices investigados: ");
                int tentativas = 0;
                while (tabelaHash[indiceHash] != NULL && tentativas < TAMANHO_TABELA) {
                    printf("%d ", indiceHash);
                    indiceHash = (indiceHash + segundoPasso) % TAMANHO_TABELA;
                    tentativas++;
                }
                printf("\n");
            }
        }
    }
}

void criarArquivoContatos() {
    FILE *arquivo = fopen("contatos.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao criar o arquivo de contatos.\n");
        return;
    }

    for (int i = 0; i < TAMANHO_TABELA; i++) {
        if (tabelaHash[i] != NULL) {
            fprintf(arquivo, "Chave: %d\n", tabelaHash[i]->chave);
            fprintf(arquivo, "Nome: %s\n", tabelaHash[i]->nome);
            fprintf(arquivo, "Email: %s\n", tabelaHash[i]->email);
            fprintf(arquivo, "Telefone: %s\n\n", tabelaHash[i]->telefone);
        }
    }

    fclose(arquivo);
    printf("Arquivo de contatos criado com sucesso.\n");
}

void lerContatosDoArquivo() {
    FILE *arquivo = fopen("todosOsContatos.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de contatos.\n");
        return;
    }

    ItemDados *novoItem;
    char buffer[100];

    while (fgets(buffer, sizeof(buffer), arquivo) != NULL) {
        if (strstr(buffer, "Nome: ") != NULL) {
            novoItem = (ItemDados *)malloc(sizeof(ItemDados));
            sscanf(buffer, "Nome: %[^\n]", novoItem->nome); // Ler o nome ate a quebra de linha

            fgets(buffer, sizeof(buffer), arquivo); // Ler a linha "Telefone: "
            sscanf(buffer, "Telefone: %[^\n]", novoItem->telefone); // Ler o telefone ate a quebra de linha

            fgets(buffer, sizeof(buffer), arquivo); // Ler a linha "Email: "
            sscanf(buffer, "Email: %[^\n]", novoItem->email); // Ler o email ate a quebra de linha

            // Le a chave do contato a partir do nome
            novoItem->chave = 0;
            for (int i = 0; novoItem->nome[i] != '\0'; i++) {
                novoItem->chave += novoItem->nome[i];
            }

            inserir(novoItem);
        }
    }

    fclose(arquivo);
}

int main() {
    inicializarTabelaHash();
    lerContatosDoArquivo();
    int escolha;

    do {
        printf("\nMenu:\n");
        printf("1. Adicionar novo contato\n");
        printf("2. Listar todos os contatos\n");
        printf("3. Editar um contato\n");
        printf("4. Excluir um contato\n");
        printf("5. Visualizar colisoes de entrada\n");
        printf("6. Criar arquivo de contatos\n");
        printf("7. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &escolha);

        switch (escolha) {
            case 1:
                {
                    ItemDados *novoItem = (ItemDados *)malloc(sizeof(ItemDados));
                    printf("Chave: ");
                    scanf("%d", &novoItem->chave);
                    printf("Nome: ");
                    scanf("%s", novoItem->nome);
                    printf("Email: ");
                    scanf("%s", novoItem->email);
                    printf("Telefone: ");
                    scanf("%s", novoItem->telefone);
                    inserir(novoItem);
                }
                break;
            case 2:
                listarContatos();
                break;
            case 3:
                {
                    int chave;
                    printf("Digite a chave do contato que deseja editar: ");
                    scanf("%d", &chave);
                    editarContato(chave);
                }
                break;
            case 4:
                {
                    int chave;
                    printf("Digite a chave do contato que deseja excluir: ");
                    scanf("%d", &chave);
                    excluirContato(chave);
                }
                break;
            case 5:
                visualizarColisoes();
                break;
            case 6:
                criarArquivoContatos();
                break;
            case 7:
                printf("Saindo do programa...\n");
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
                break;
        }
    } while (escolha != 7);

    return 0;
}
