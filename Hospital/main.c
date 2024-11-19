#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PACIENTES 100
#define MAX_MEDICOS 50
#define MAX_SALAS 10
#define MAX_ESPECIALIDADES 10
#define MAX_NOME 50

typedef struct {
    char nome[MAX_NOME];
    int idade;
    char telefone[MAX_NOME];
    int idUnico;
    float peso;
    float altura;
    char sintomas[MAX_NOME];
    char medicacao[MAX_NOME];
    int prioridade;
    char especialidadeNecessaria[MAX_NOME];
    int faltou;
    int atendido;
    int retorno;
} Paciente;

typedef struct {
    char nome[MAX_NOME];
    char especialidade[MAX_NOME];
    int horasTrabalhadas;
} Medico;

typedef struct {
    char nome[MAX_NOME];
} Especialidade;

typedef struct {
    int sala;
    int hora;
    Medico medico;
    Paciente paciente;
} Atendimento;

// Função para comparar pacientes por prioridade
int compararPacientes(const void *a, const void *b) {
    Paciente *pacienteA = (Paciente *)a;
    Paciente *pacienteB = (Paciente *)b;

    if (pacienteA->retorno != pacienteB->retorno)
        return pacienteB->retorno - pacienteA->retorno;

    return pacienteB->prioridade - pacienteA->prioridade;
}

// Função para comparar médicos por horas trabalhadas (decrescente)
int compararMedicos(const void *a, const void *b) {
    Medico *medicoA = (Medico *)a;
    Medico *medicoB = (Medico *)b;

    return medicoB->horasTrabalhadas - medicoA->horasTrabalhadas;
}

// Lê os dados do arquivo de entrada
void lerDados(FILE *entrada, int *numPacientes, int *numSalas, int *numEspecialidades, int *numMedicos,
              Especialidade especialidades[], Medico medicos[], Paciente pacientes[]) {
    fscanf(entrada, "%d %d %d %d", numPacientes, numSalas, numEspecialidades, numMedicos);

    // Lendo especialidades
    for (int i = 0; i < *numEspecialidades; i++) {
        fscanf(entrada, "%s", especialidades[i].nome);
    }

    // Lendo médicos
    for (int i = 0; i < *numMedicos; i++) {
        fscanf(entrada, "%s %s", medicos[i].nome, medicos[i].especialidade);
        medicos[i].horasTrabalhadas = 0;
    }

    // Lendo pacientes
    for (int i = 0; i < *numPacientes; i++) {
        fscanf(entrada, "%s %d %s %d %f %f %s %s %d %s",
               pacientes[i].nome,
               &pacientes[i].idade,
               pacientes[i].telefone,
               &pacientes[i].idUnico,
               &pacientes[i].peso,
               &pacientes[i].altura,
               pacientes[i].sintomas,
               pacientes[i].medicacao,
               &pacientes[i].prioridade,
               pacientes[i].especialidadeNecessaria);

        pacientes[i].faltou = 0;
        pacientes[i].atendido = 0;
        pacientes[i].retorno = 0;
    }
}

// Função principal
int main() {
    srand(time(NULL));
    FILE *entrada = fopen("dados_pacientes.txt", "r");
    if (entrada == NULL) {
        printf("Erro ao abrir o arquivo dados_pacientes.\n");
        return 1;
    }

    int numPacientes, numSalas, numEspecialidades, numMedicos;
    Especialidade especialidades[MAX_ESPECIALIDADES];
    Medico medicos[MAX_MEDICOS];
    Paciente pacientes[MAX_PACIENTES];
    Atendimento atendimentos[MAX_PACIENTES * MAX_SALAS];

    lerDados(entrada, &numPacientes, &numSalas, &numEspecialidades, &numMedicos, especialidades, medicos, pacientes);
    fclose(entrada);

    int totalAtendimentos = 0, semanas = 0;

    while (1) {
        int atendidosNaSemana = 0;

        // Ordenar pacientes por prioridade
        qsort(pacientes, numPacientes, sizeof(Paciente), compararPacientes);

        for (int hora = 8; hora <= 17; hora++) { // Horário comercial: 8h às 17h
            for (int sala = 0; sala < numSalas; sala++) {
                int pacienteAtendido = 0;

                for (int i = 0; i < numPacientes; i++) {
                    if (pacientes[i].faltou < 2 && pacientes[i].prioridade > 0 && !pacientes[i].atendido) {
                        for (int j = 0; j < numMedicos; j++) {
                            if (strcmp(medicos[j].especialidade, pacientes[i].especialidadeNecessaria) == 0) {
                                // Alocar atendimento
                                atendimentos[totalAtendimentos].sala = sala + 1;
                                atendimentos[totalAtendimentos].hora = hora;
                                atendimentos[totalAtendimentos].medico = medicos[j];
                                atendimentos[totalAtendimentos].paciente = pacientes[i];
                                totalAtendimentos++;
                                atendidosNaSemana++;

                                // Atualizar status do paciente
                                pacientes[i].atendido = 1;
                                pacientes[i].retorno = 1;
                                medicos[j].horasTrabalhadas++;

                                // Simular falta
                                if (rand() % 100 < 5) {
                                    pacientes[i].faltou++;
                                    pacientes[i].prioridade--;
                                    pacientes[i].atendido = 0;
                                } else {
                                    pacientes[i].prioridade = 0;
                                }
                                pacienteAtendido = 1;
                                break;
                            }
                        }
                    }
                    if (pacienteAtendido) break;
                }
            }
        }

        semanas++;
        if (atendidosNaSemana == 0) break;
    }

    // Ordenar médicos por horas trabalhadas
    qsort(medicos, numMedicos, sizeof(Medico), compararMedicos);

    // Relatório de atendimentos
    FILE *saida = fopen("relatorio_atendimentos.txt", "w");
    if (saida == NULL) {
        printf("Erro ao criar o arquivo de relatorio.\n");
        return 1;
    }

    fprintf(saida, "Número de semanas necessárias: %d\n", semanas);
    fprintf(saida, "Total de pacientes atendidos: %d\n", totalAtendimentos);
    fprintf(saida, "\nDetalhamento dos atendimentos:\n");

    for (int i = 0; i < totalAtendimentos; i++) {
        fprintf(saida, "Sala %d, Hora %d: Médico %s, Paciente %s\n",
                atendimentos[i].sala,
                atendimentos[i].hora,
                atendimentos[i].medico.nome,
                atendimentos[i].paciente.nome);
    }

    fprintf(saida, "\nHoras trabalhadas por médico:\n");
    for (int i = 0; i < numMedicos; i++) {
        fprintf(saida, "Médico: %s, Horas: %d\n", medicos[i].nome, medicos[i].horasTrabalhadas);
    }

    fclose(saida);
    printf("Relatório gerado com sucesso!\n");

    return 0;
}
