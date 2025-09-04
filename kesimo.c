#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/* =========================
   Utilitários básicos
   ========================= */

// Troca os valores apontados por a e b
void trocar(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/*
 * insertionSort: ordena arr[0..n-1] em ordem crescente.
 * Usamos essa ordenação "simples" nos grupinhos de 5 elementos,
 * porque é barata e suficiente para formar boas medianas.
 */
void insertionSort(int arr[], int n) {
    for (int i = 1; i < n; i++) {
        int chave = arr[i];
        int j = i - 1;
        // move elementos maiores que 'chave' uma posição à frente
        while (j >= 0 && arr[j] > chave) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = chave;
    }
}

/*
 * particionar (variação de Lomuto):
 *  - Reorganiza arr[l..r] em torno de um valor 'pivo' (valor, não índice).
 *  - Todos <= pivo vão para a esquerda, > pivo para a direita.
 *  - Retorna a posição final do pivo (índice 'i').
 * Observação: primeiro localizamos UMA ocorrência do valor do pivô
 * e a movemos para o fim (posição r) para facilitar o particionamento.
 */
int particionar(int arr[], int l, int r, int pivo) {
    // 1) acha o pivo no intervalo e move para o fim
    int i;
    for (i = l; i <= r; i++) {
        if (arr[i] == pivo) break;  // para na primeira ocorrência
    }
    trocar(&arr[i], &arr[r]);       // pivô fica em arr[r]

    // 2) particiona usando Lomuto com comparação <=
    i = l;
    for (int j = l; j <= r - 1; j++) {
        if (arr[j] <= pivo) {
            trocar(&arr[i], &arr[j]);
            i++;
        }
    }
    // coloca o pivô na posição correta (entre as partes)
    trocar(&arr[i], &arr[r]);
    return i; // índice final do pivô
}

/* =========================
   Seleção determinística:
   k-ésimo menor (1-based)
   =========================
   kesimoMinimo(arr, l, r, k):
   - Encontra o k-ésimo menor em arr[l..r], com k iniciando em 1.
   - Usa "mediana das medianas" como pivô => O(n) no pior caso.
*/
int kesimoMinimo(int arr[], int l, int r, int k) {
    // Verifica se k está dentro do número de elementos do subarray atual
    if (k > 0 && k <= r - l + 1) {

        int n = r - l + 1; // quantidade de elementos em arr[l..r]

        /* ===== 1) DIVIDIR EM GRUPOS DE 5 E PEGAR MEDIANAS =====
           - Para cada grupo de até 5 elementos:
             a) ordena o grupinho com insertion sort
             b) pega a mediana daquele grupinho
           - Armazena todas as medianas no vetor 'medians'.
        */
        int i; 
        int medians[(n + 4) / 5]; // quantidade de grupos = ceil(n/5)
        // grupos "cheios" de 5
        for (i = 0; i < n / 5; i++) {
            insertionSort(arr + l + i * 5, 5);
            medians[i] = arr[l + i * 5 + 2]; // posição 2 (0-based) é a mediana do grupo de 5
        }
        // último grupo (se sobrar < 5 elementos)
        if (i * 5 < n) {
            int resto = n % 5;                     // tamanho do grupo final (1..4)
            insertionSort(arr + l + i * 5, resto); // ordena esse grupo menor
            medians[i] = arr[l + i * 5 + resto / 2]; // mediana do grupo menor
            i++; // total de medianas
        }
        
        /* ===== 2) CONQUISTAR: MEDIANA DAS MEDIANAS =====
           - Se só existe uma mediana, ela é o pivô.
           - Caso contrário, seleciona a mediana do vetor 'medians'
             recursivamente (isso garante um "bom pivô").
           IMPORTANTE: 'k' da seleção é 1-based; logo, a mediana de i itens
           é k = ceil(i/2) = (i+1)/2
        */
        int medOfMed;
        if (i == 1) {
            medOfMed = medians[0];
        } else {
            medOfMed = kesimoMinimo(medians, 0, i - 1, (i + 1) / 2);
        }

        /* ===== 3) PARTICIONAR EM TORNO DO PIVÔ =====
           - Rearranja arr[l..r] usando 'medOfMed' como pivô
           - 'pos' é a posição final do pivô no array particionado
        */
        int pos = particionar(arr, l, r, medOfMed);

        /* ===== 4) DECIDIR O LADO =====
           - Se pos-l == k-1, o pivô é exatamente o k-ésimo menor (1-based).
           - Se pos-l > k-1: o k-ésimo está à ESQUERDA.
           - Senão: está à DIREITA; ajusta k para o subarray direito.
        */
        if (pos - l == k - 1)
            return arr[pos];
        if (pos - l > k - 1)
            return kesimoMinimo(arr, l, pos - 1, k);
        return kesimoMinimo(arr, pos + 1, r, k - (pos - l) - 1);
    }

    // k inválido para o intervalo atual
    return INT_MAX;
}

/* =========================
   Demonstração de uso
   ========================= */
int main() {
    int D[] = {25, 21, 98, 100, 76, 22, 43, 60, 89, 42};
    int n = sizeof(D) / sizeof(D[0]);
    int k = 5; // queremos o 5º menor (k é 1-based)

    printf("Array original: ");
    for(int i = 0; i < n; i++) printf("%d ", D[i]);
    printf("\n");

    // Para referência, array ordenado seria:
    // 21, 22, 25, 42, 43, 60, 76, 89, 98, 100
    // O 5º menor é 43.

    int resultado = kesimoMinimo(D, 0, n - 1, k);

    if (resultado != INT_MAX) {
        printf("O %d-esimo menor elemento e: %d\n", k, resultado);
    } else {
        printf("k e invalido.\n");
    }
    
    return 0;
}
