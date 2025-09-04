#include <stdio.h>
#include <stdlib.h>

/*
 * Este programa implementa a multiplicação de matrizes usando o
 * algoritmo de Strassen.
 *
 * OBSERVAÇÕES IMPORTANTES:
 * - O código assume que n é par em todos os níveis de recursão (ideal: n potência de 2).
 *   Para n ímpar, seria preciso fazer "padding" (preencher com zeros até a próxima potência de 2)
 *   antes de chamar strassen().
 * - Strassen reduz 8 multiplicações de blocos para 7 (P1..P7), compensando com somas/subtrações.
 * - Caso-base: n == 1 (multiplicação de escalares).
 */

/* ===================== Funções auxiliares ===================== */

// Aloca uma matriz n×n de int, inicializada com zeros (calloc).
int** alocarMatriz(int n) {
    int** m = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) m[i] = (int*)calloc(n, sizeof(int));
    return m;
}

// Libera a matriz n×n previamente alocada com alocarMatriz.
void liberarMatriz(int n, int** m) {
    for (int i = 0; i < n; i++) free(m[i]);
    free(m);
}

// C = A + B (soma elemento a elemento).
void somarMatrizes(int n, int** A, int** B, int** C) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) C[i][j] = A[i][j] + B[i][j];
}

// C = A - B (subtração elemento a elemento).
void subtrairMatrizes(int n, int** A, int** B, int** C) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) C[i][j] = A[i][j] - B[i][j];
}

// Apenas imprime a matriz n×n (tabulada).
void imprimirMatriz(int n, int** C) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) printf("%d\t", C[i][j]);
        printf("\n");
    }
}

/* ===================== Algoritmo de Strassen =====================
 *
 * Ideia: dividir A e B em 4 quadrantes de tamanho (n/2)×(n/2):
 *
 *   A = | A11  A12 |      B = | B11  B12 |
 *       | A21  A22 |          | B21  B22 |
 *
 * Em vez de calcular C = A×B com 8 multiplicações de blocos,
 * calculamos 7 produtos P1..P7 usando combinações de somas/subtrações:
 *
 *  S1  = B12 − B22         P1 = A11 × S1
 *  S2  = A11 + A12         P2 = S2 × B22
 *  S3  = A21 + A22         P3 = S3 × B11
 *  S4  = B21 − B11         P4 = A22 × S4
 *  S5  = A11 + A22
 *  S6  = B11 + B22         P5 = S5 × S6
 *  S7  = A12 − A22
 *  S8  = B21 + B22         P6 = S7 × S8
 *  S9  = A11 − A21
 *  S10 = B11 + B12         P7 = S9 × S10
 *
 * Recombinação para obter os quadrantes de C:
 *
 *  C11 = P5 + P4 − P2 + P6
 *  C12 = P1 + P2
 *  C21 = P3 + P4
 *  C22 = P5 + P1 − P3 − P7
 *
 * Observação: essa forma é equivalente à formulação canônica de Strassen
 * (que costuma usar M1..M7). A diferença de sinais em P7 é compensada na recombinação.
 */

void strassen(int n, int** A, int** B, int** C) {
    // Caso-base: matriz 1×1 → multiplicação de escalares.
    if (n == 1) {
        C[0][0] = A[0][0] * B[0][0];
        return;
    }

    // Tamanho dos subproblemas (quadrantes)
    int novo_n = n / 2;

    // Alocação das submatrizes (quadrantes de A e B)
    int** A11 = alocarMatriz(novo_n); int** A12 = alocarMatriz(novo_n);
    int** A21 = alocarMatriz(novo_n); int** A22 = alocarMatriz(novo_n);
    int** B11 = alocarMatriz(novo_n); int** B12 = alocarMatriz(novo_n);
    int** B21 = alocarMatriz(novo_n); int** B22 = alocarMatriz(novo_n);
    
    // Divide A e B em quadrantes (copia os blocos correspondentes)
    for (int i = 0; i < novo_n; i++) {
        for (int j = 0; j < novo_n; j++) {
            A11[i][j] = A[i][j];
            A12[i][j] = A[i][j + novo_n];
            A21[i][j] = A[i + novo_n][j];
            A22[i][j] = A[i + novo_n][j + novo_n];

            B11[i][j] = B[i][j];
            B12[i][j] = B[i][j + novo_n];
            B21[i][j] = B[i + novo_n][j];
            B22[i][j] = B[i + novo_n][j + novo_n];
        }
    }
    
    // ===== 1) Preparação das somas/subtrações (S1..S10) e 2) chamadas recursivas (P1..P7) =====

    // S1 = B12 - B22;   P1 = A11 * S1
    int** S1 = alocarMatriz(novo_n); subtrairMatrizes(novo_n, B12, B22, S1);
    int** P1 = alocarMatriz(novo_n); strassen(novo_n, A11, S1, P1);

    // S2 = A11 + A12;   P2 = S2 * B22
    int** S2 = alocarMatriz(novo_n); somarMatrizes(novo_n, A11, A12, S2);
    int** P2 = alocarMatriz(novo_n); strassen(novo_n, S2, B22, P2);

    // S3 = A21 + A22;   P3 = S3 * B11
    int** S3 = alocarMatriz(novo_n); somarMatrizes(novo_n, A21, A22, S3);
    int** P3 = alocarMatriz(novo_n); strassen(novo_n, S3, B11, P3);

    // S4 = B21 - B11;   P4 = A22 * S4
    int** S4 = alocarMatriz(novo_n); subtrairMatrizes(novo_n, B21, B11, S4);
    int** P4 = alocarMatriz(novo_n); strassen(novo_n, A22, S4, P4);

    // S5 = A11 + A22;  S6 = B11 + B22;  P5 = S5 * S6
    int** S5 = alocarMatriz(novo_n); somarMatrizes(novo_n, A11, A22, S5);
    int** S6 = alocarMatriz(novo_n); somarMatrizes(novo_n, B11, B22, S6);
    int** P5 = alocarMatriz(novo_n); strassen(novo_n, S5, S6, P5);

    // S7 = A12 - A22;  S8 = B21 + B22;  P6 = S7 * S8
    int** S7 = alocarMatriz(novo_n); subtrairMatrizes(novo_n, A12, A22, S7);
    int** S8 = alocarMatriz(novo_n); somarMatrizes(novo_n, B21, B22, S8);
    int** P6 = alocarMatriz(novo_n); strassen(novo_n, S7, S8, P6);

    // S9 = A11 - A21;  S10 = B11 + B12; P7 = S9 * S10
    int** S9  = alocarMatriz(novo_n); subtrairMatrizes(novo_n, A11, A21, S9);
    int** S10 = alocarMatriz(novo_n); somarMatrizes(novo_n, B11, B12, S10);
    int** P7  = alocarMatriz(novo_n); strassen(novo_n, S9, S10, P7);

    // As S1..S10 não são mais necessárias (libera memória).
    liberarMatriz(novo_n, S1);  liberarMatriz(novo_n, S2);  liberarMatriz(novo_n, S3);
    liberarMatriz(novo_n, S4);  liberarMatriz(novo_n, S5);  liberarMatriz(novo_n, S6);
    liberarMatriz(novo_n, S7);  liberarMatriz(novo_n, S8);  liberarMatriz(novo_n, S9);
    liberarMatriz(novo_n, S10);

    // ===== 3) Recombinação: monta os quadrantes de C a partir dos P’s =====

    int** C11 = alocarMatriz(novo_n); int** C12 = alocarMatriz(novo_n);
    int** C21 = alocarMatriz(novo_n); int** C22 = alocarMatriz(novo_n);

    // C11 = P5 + P4 − P2 + P6
    somarMatrizes(novo_n, P5, P4, C11);
    subtrairMatrizes(novo_n, C11, P2, C11);
    somarMatrizes(novo_n, C11, P6, C11);

    // C12 = P1 + P2
    somarMatrizes(novo_n, P1, P2, C12);

    // C21 = P3 + P4
    somarMatrizes(novo_n, P3, P4, C21);

    // C22 = P5 + P1 − P3 − P7
    somarMatrizes(novo_n, P5, P1, C22);
    subtrairMatrizes(novo_n, C22, P3, C22);
    subtrairMatrizes(novo_n, C22, P7, C22);
    
    // Copia os quadrantes C11..C22 para as posições corretas de C (matriz final)
    for (int i = 0; i < novo_n; i++) {
        for (int j = 0; j < novo_n; j++) {
            C[i][j]                   = C11[i][j];          // canto superior esquerdo
            C[i][j + novo_n]          = C12[i][j];          // canto superior direito
            C[i + novo_n][j]          = C21[i][j];          // canto inferior esquerdo
            C[i + novo_n][j + novo_n] = C22[i][j];          // canto inferior direito
        }
    }
    
    // Libera toda a memória temporária alocada neste nível
    liberarMatriz(novo_n, P1);  liberarMatriz(novo_n, P2);  liberarMatriz(novo_n, P3);
    liberarMatriz(novo_n, P4);  liberarMatriz(novo_n, P5);  liberarMatriz(novo_n, P6);
    liberarMatriz(novo_n, P7);
    liberarMatriz(novo_n, A11); liberarMatriz(novo_n, A12); liberarMatriz(novo_n, A21); liberarMatriz(novo_n, A22);
    liberarMatriz(novo_n, B11); liberarMatriz(novo_n, B12); liberarMatriz(novo_n, B21); liberarMatriz(novo_n, B22);
    liberarMatriz(novo_n, C11); liberarMatriz(novo_n, C12); liberarMatriz(novo_n, C21); liberarMatriz(novo_n, C22);
}

/* ===================== Exemplo mínimo de uso ===================== */

int main() {
    int n = 2;  // Para testes maiores, use n = 4, 8, 16... (ideal: potência de 2)

    // Aloca A, B e C (n×n)
    int** A = alocarMatriz(n);
    int** B = alocarMatriz(n);
    int** C = alocarMatriz(n);

    // Preenche A e B com um caso de teste simples
    // A = {{1, 2}, {3, 4}}
    A[0][0] = 1; A[0][1] = 2; A[1][0] = 3; A[1][1] = 4;
    // B = {{5, 6}, {7, 8}}
    B[0][0] = 5; B[0][1] = 6; B[1][0] = 7; B[1][1] = 8;
    
    // Calcula C = A × B via Strassen
    strassen(n, A, B, C);

    // Exibe o resultado
    printf("Matriz Resultante C (Strassen):\n");
    imprimirMatriz(n, C); // Esperado para o caso: [[19,22],[43,50]]

    // Libera memória
    liberarMatriz(n, A);
    liberarMatriz(n, B);
    liberarMatriz(n, C);
    return 0;
}

