// ============================================================================
//         PROJETO WAR ESTRUTURADO - DESAFIO DE CÓDIGO
// ============================================================================
//
// AUTOR: [Paulo Eduardo]
// DATA: [11/01/2026]
//
// DESCRIÇÃO:
// Jogo de estratégia simplificado baseado no clássico WAR, onde o jogador
// deve completar uma missão secreta para vencer o jogo.
//
// ============================================================================
// OBJETIVOS:
// - Modularizar completamente o código em funções especializadas.
// - Implementar um sistema de missões para um jogador.
// - Criar uma função para verificar se a missão foi cumprida.
// - Utilizar passagem por referência (ponteiros) para modificar dados e
//   passagem por valor/referência constante (const) para apenas ler.
// - Foco em: Design de software, modularização, const correctness, lógica de jogo.
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

// --- Constantes Globais ---
#define NUM_TERRITORIOS 12
#define NUM_MISSOES 6
#define TAM_MAX_NOME 30
#define TAM_MAX_COR 15

// Cores disponíveis para os exércitos
typedef enum {
    AZUL,      // Jogador humano
    VERMELHO,  // Exército inimigo 1
    VERDE,     // Exército inimigo 2
    AMARELO,   // Exército inimigo 3
    NUM_CORES
} CorExercito;

// Estrutura para representar um território
typedef struct {
    char nome[TAM_MAX_NOME];
    CorExercito exercito;
    int tropas;
} Territorio;

// Estrutura para representar uma missão
typedef struct {
    int id;
    char descricao[100];
    CorExercito exercitoAlvo;  // Para missões de destruir exército
    int territoriosNecessarios; // Para missões de conquistar territórios
} Missao;

// --- Protótipos das Funções ---

// Funções de setup e gerenciamento de memória
Territorio* alocarMapa();
void inicializarTerritorios(Territorio* mapa);
void liberarMemoria(Territorio* mapa);

// Funções de interface com o usuário
void exibirMenuPrincipal();
void exibirMapa(const Territorio* mapa);
void exibirMissao(int missaoId);
void limparBufferEntrada();

// Funções de lógica principal do jogo
void faseDeAtaque(Territorio* mapa);
void simularAtaque(Territorio* origem, Territorio* destino);
int sortearMissao();
int verificarVitoria(const Territorio* mapa, int missaoId);

// Função utilitária
const char* obterNomeCor(CorExercito cor);

// --- Função Principal ---
int main() {
    setlocale(LC_ALL, "Portuguese");
    
    printf("===========================================\n");
    printf("        GUERRA ESTRATÉGICA - WAR\n");
    printf("===========================================\n\n");
    
    // Inicializar gerador de números aleatórios
    srand(time(NULL));
    
    // Alocar memória para o mapa
    Territorio* mapa = alocarMapa();
    if (mapa == NULL) {
        printf("Erro: Não foi possível alocar memória para o mapa.\n");
        return 1;
    }
    
    // Inicializar territórios
    inicializarTerritorios(mapa);
    
    // Sorteiar missão para o jogador
    int missaoAtual = sortearMissao();
    
    // Configurar cor do jogador
    CorExercito jogador = AZUL;
    
    printf("Você controla o exército AZUL.\n");
    printf("Sua missão secreta é:\n");
    exibirMissao(missaoAtual);
    printf("\nPressione ENTER para continuar...");
    getchar();
    
    int opcao;
    int vitoria = 0;
    
    // Loop principal do jogo
    do {
        system("clear || cls"); // Limpar tela (Linux/Windows)
        
        printf("===========================================\n");
        printf("           MAPA ATUAL DO JOGO\n");
        printf("===========================================\n");
        exibirMapa(mapa);
        printf("\n");
        
        printf("===========================================\n");
        printf("              SUA MISSÃO\n");
        printf("===========================================\n");
        exibirMissao(missaoAtual);
        printf("\n");
        
        exibirMenuPrincipal();
        printf("\nEscolha uma opção: ");
        scanf("%d", &opcao);
        limparBufferEntrada();
        
        switch (opcao) {
            case 1:
                faseDeAtaque(mapa);
                break;
                
            case 2:
                vitoria = verificarVitoria(mapa, missaoAtual);
                if (vitoria) {
                    printf("\n===========================================\n");
                    printf("          PARABÉNS! VOCÊ VENCEU!\n");
                    printf("===========================================\n");
                    printf("Você cumpriu sua missão com sucesso!\n\n");
                } else {
                    printf("\n===========================================\n");
                    printf("            MISSÃO NÃO CUMPRIDA\n");
                    printf("===========================================\n");
                    printf("Continue tentando para cumprir sua missão.\n\n");
                }
                printf("Pressione ENTER para continuar...");
                getchar();
                break;
                
            case 0:
                printf("Encerrando o jogo...\n");
                break;
                
            default:
                printf("Opção inválida! Tente novamente.\n");
                printf("Pressione ENTER para continuar...");
                getchar();
                break;
        }
        
        // Verificar vitória após cada ação
        if (opcao != 0 && opcao != 2) {
            vitoria = verificarVitoria(mapa, missaoAtual);
        }
        
    } while (opcao != 0 && !vitoria);
    
    // Liberar memória alocada
    liberarMemoria(mapa);
    
    printf("\nObrigado por jogar!\n");
    return 0;
}

// --- Implementação das Funções ---

// Funções de setup e gerenciamento de memória
Territorio* alocarMapa() {
    return (Territorio*)calloc(NUM_TERRITORIOS, sizeof(Territorio));
}

void inicializarTerritorios(Territorio* mapa) {
    // Nomes dos territórios
    const char* nomesTerritorios[NUM_TERRITORIOS] = {
        "Amazônia", "Cerrado", "Mata Atlântica", "Caatinga",
        "Pampa", "Pantanal", "Alaska", "Groenlândia",
        "Sibéria", "Austrália", "África do Sul", "Antártida"
    };
    
    // Distribuir territórios entre os exércitos
    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        strcpy(mapa[i].nome, nomesTerritorios[i]);
        
        // Distribuição inicial: 4 para cada exército
        if (i < 3) mapa[i].exercito = AZUL;        // Jogador
        else if (i < 6) mapa[i].exercito = VERMELHO;
        else if (i < 9) mapa[i].exercito = VERDE;
        else mapa[i].exercito = AMARELO;
        
        // Tropas iniciais: entre 1 e 5
        mapa[i].tropas = (rand() % 5) + 1;
    }
}

void liberarMemoria(Territorio* mapa) {
    free(mapa);
}

// Funções de interface com o usuário
void exibirMenuPrincipal() {
    printf("===========================================\n");
    printf("                MENU PRINCIPAL\n");
    printf("===========================================\n");
    printf("1. Fase de Ataque\n");
    printf("2. Verificar Condição de Vitória\n");
    printf("0. Sair do Jogo\n");
    printf("===========================================\n");
}

void exibirMapa(const Territorio* mapa) {
    printf("\n%-25s %-15s %-10s\n", "TERRITÓRIO", "EXÉRCITO", "TROPAS");
    printf("%-25s %-15s %-10s\n", "-------------------------", "---------------", "----------");
    
    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        const char* nomeCor = obterNomeCor(mapa[i].exercito);
        
        // Destacar territórios do jogador
        if (mapa[i].exercito == AZUL) {
            printf("> %-23s %-15s %-10d\n", mapa[i].nome, nomeCor, mapa[i].tropas);
        } else {
            printf("  %-23s %-15s %-10d\n", mapa[i].nome, nomeCor, mapa[i].tropas);
        }
    }
}

void exibirMissao(int missaoId) {
    Missao missoes[NUM_MISSOES] = {
        {1, "Destruir completamente o exército VERMELHO", VERMELHO, 0},
        {2, "Destruir completamente o exército VERDE", VERDE, 0},
        {3, "Destruir completamente o exército AMARELO", AMARELO, 0},
        {4, "Conquistar pelo menos 8 territórios no total", NUM_CORES, 8},
        {5, "Conquistar pelo menos 10 territórios no total", NUM_CORES, 10},
        {6, "Conquistar pelo menos 6 territórios e destruir o exército VERMELHO", VERMELHO, 6}
    };
    
    if (missaoId >= 1 && missaoId <= NUM_MISSOES) {
        printf("%s\n", missoes[missaoId - 1].descricao);
    }
}

void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Funções de lógica principal do jogo
void faseDeAtaque(Territorio* mapa) {
    system("clear || cls");
    printf("===========================================\n");
    printf("              FASE DE ATAQUE\n");
    printf("===========================================\n\n");
    
    exibirMapa(mapa);
    
    int origem, destino;
    
    printf("\nSelecione o território de ORIGEM (1-%d): ", NUM_TERRITORIOS);
    scanf("%d", &origem);
    limparBufferEntrada();
    
    if (origem < 1 || origem > NUM_TERRITORIOS) {
        printf("Território inválido!\n");
        return;
    }
    
    // Ajustar para índice do array
    origem--;
    
    // Verificar se o território pertence ao jogador
    if (mapa[origem].exercito != AZUL) {
        printf("Você só pode atacar a partir de territórios que você controla!\n");
        printf("Pressione ENTER para continuar...");
        getchar();
        return;
    }
    
    // Verificar se tem tropas suficientes para atacar
    if (mapa[origem].tropas <= 1) {
        printf("Você precisa de pelo menos 2 tropas para atacar!\n");
        printf("Pressione ENTER para continuar...");
        getchar();
        return;
    }
    
    printf("\nSelecione o território de DESTINO (1-%d): ", NUM_TERRITORIOS);
    scanf("%d", &destino);
    limparBufferEntrada();
    
    if (destino < 1 || destino > NUM_TERRITORIOS) {
        printf("Território inválido!\n");
        return;
    }
    
    // Ajustar para índice do array
    destino--;
    
    // Verificar se não está atacando seu próprio território
    if (origem == destino) {
        printf("Você não pode atacar seu próprio território!\n");
        printf("Pressione ENTER para continuar...");
        getchar();
        return;
    }
    
    // Verificar se o destino não é do jogador
    if (mapa[destino].exercito == AZUL) {
        printf("Você não pode atacar seus próprios territórios!\n");
        printf("Pressione ENTER para continuar...");
        getchar();
        return;
    }
    
    // Executar o ataque
    simularAtaque(&mapa[origem], &mapa[destino]);
    
    printf("\nPressione ENTER para continuar...");
    getchar();
}

void simularAtaque(Territorio* origem, Territorio* destino) {
    printf("\n===========================================\n");
    printf("              SIMULAÇÃO DE ATAQUE\n");
    printf("===========================================\n");
    printf("%s (%s) ataca %s (%s)\n", 
           origem->nome, obterNomeCor(origem->exercito),
           destino->nome, obterNomeCor(destino->exercito));
    printf("\n");
    
    // Rolagem de dados
    int dadosAtacante[3] = {0};
    int dadosDefensor[2] = {0};
    
    // Atacante rola até 3 dados (dependendo do número de tropas -1)
    int numDadosAtacante = (origem->tropas - 1) > 3 ? 3 : (origem->tropas - 1);
    for (int i = 0; i < numDadosAtacante; i++) {
        dadosAtacante[i] = (rand() % 6) + 1;
    }
    
    // Defensor rola até 2 dados
    int numDadosDefensor = (destino->tropas > 2) ? 2 : destino->tropas;
    for (int i = 0; i < numDadosDefensor; i++) {
        dadosDefensor[i] = (rand() % 6) + 1;
    }
    
    // Ordenar dados em ordem decrescente
    for (int i = 0; i < numDadosAtacante - 1; i++) {
        for (int j = i + 1; j < numDadosAtacante; j++) {
            if (dadosAtacante[i] < dadosAtacante[j]) {
                int temp = dadosAtacante[i];
                dadosAtacante[i] = dadosAtacante[j];
                dadosAtacante[j] = temp;
            }
        }
    }
    
    for (int i = 0; i < numDadosDefensor - 1; i++) {
        for (int j = i + 1; j < numDadosDefensor; j++) {
            if (dadosDefensor[i] < dadosDefensor[j]) {
                int temp = dadosDefensor[i];
                dadosDefensor[i] = dadosDefensor[j];
                dadosDefensor[j] = temp;
            }
        }
    }
    
    // Exibir resultados dos dados
    printf("Dados do Atacante: ");
    for (int i = 0; i < numDadosAtacante; i++) {
        printf("[%d] ", dadosAtacante[i]);
    }
    printf("\n");
    
    printf("Dados do Defensor: ");
    for (int i = 0; i < numDadosDefensor; i++) {
        printf("[%d] ", dadosDefensor[i]);
    }
    printf("\n\n");
    
    // Comparar dados
    int comparacoes = (numDadosAtacante < numDadosDefensor) ? numDadosAtacante : numDadosDefensor;
    int perdasAtacante = 0;
    int perdasDefensor = 0;
    
    for (int i = 0; i < comparacoes; i++) {
        if (dadosAtacante[i] > dadosDefensor[i]) {
            perdasDefensor++;
        } else {
            perdasAtacante++;
        }
    }
    
    printf("RESULTADO:\n");
    printf("- Tropas perdidas pelo atacante: %d\n", perdasAtacante);
    printf("- Tropas perdidas pelo defensor: %d\n", perdasDefensor);
    printf("\n");
    
    // Atualizar tropas
    origem->tropas -= perdasAtacante;
    destino->tropas -= perdasDefensor;
    
    // Verificar se o território foi conquistado
    if (destino->tropas <= 0) {
        printf("VITÓRIA! %s foi conquistado!\n", destino->nome);
        
        // O conquistador move pelo menos 1 tropa para o novo território
        int tropasParaMover = (origem->tropas - 1) > 1 ? 1 : origem->tropas - 1;
        if (tropasParaMover > 0) {
            origem->tropas -= tropasParaMover;
            destino->tropas = tropasParaMover;
            destino->exercito = origem->exercito;
            printf("%d tropas foram movidas para o novo território.\n", tropasParaMover);
        }
    }
    
    printf("\nSITUAÇÃO ATUAL:\n");
    printf("%s: %d tropas\n", origem->nome, origem->tropas);
    printf("%s: %d tropas (%s)\n", destino->nome, destino->tropas, 
           obterNomeCor(destino->exercito));
}

int sortearMissao() {
    return (rand() % NUM_MISSOES) + 1;
}

int verificarVitoria(const Territorio* mapa, int missaoId) {
    int contagemTerritorios[NUM_CORES] = {0};
    int territoriosJogador = 0;
    
    // Contar territórios por exército
    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        contagemTerritorios[mapa[i].exercito]++;
        if (mapa[i].exercito == AZUL) {
            territoriosJogador++;
        }
    }
    
    // Verificar missões específicas
    switch (missaoId) {
        case 1: // Destruir exército VERMELHO
            return contagemTerritorios[VERMELHO] == 0;
            
        case 2: // Destruir exército VERDE
            return contagemTerritorios[VERDE] == 0;
            
        case 3: // Destruir exército AMARELO
            return contagemTerritorios[AMARELO] == 0;
            
        case 4: // Conquistar 8 territórios
            return territoriosJogador >= 8;
            
        case 5: // Conquistar 10 territórios
            return territoriosJogador >= 10;
            
        case 6: // Conquistar 6 territórios e destruir VERMELHO
            return territoriosJogador >= 6 && contagemTerritorios[VERMELHO] == 0;
            
        default:
            return 0;
    }
}

// Função utilitária
const char* obterNomeCor(CorExercito cor) {
    switch (cor) {
        case AZUL: return "AZUL";
        case VERMELHO: return "VERMELHO";
        case VERDE: return "VERDE";
        case AMARELO: return "AMARELO";
        default: return "DESCONHECIDO";
    }
}