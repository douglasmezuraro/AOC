/*
  TERCEIRA AVALIAÇÃO DE ARQUITETURA E ORGANIZAÇÃO DE COMPUTADORES
  PROFESSOR: RODRIGO CALVO
  AUTOR: DOUGLAS MEZURARO RA: 95676
  AUTOR: LUCAS BORTOLANZA RA: 95672
  DATA: 13/08/2016
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* VARIÁVEIS, CONSTANTES E DEFINIÇÕES GLOBAIS */

#define MESMA_STRING 0
#define RAM_LENGTH 16
#define STRING_LENGTH 16
#define BASE_HEXADECIMAL 16

#define INDEX_LOAD  0
#define INDEX_ADD   1
#define INDEX_MOV   2
#define INDEX_MULT  3
#define INDEX_STORE 4
#define INDEX_DIV   5

const STR_LOAD  = "LOAD";
const STR_ADD   = "ADD";
const STR_MOV   = "MOV";
const STR_MULT  = "MULT";
const STR_STORE = "STORE";
const STR_DIV   = "DIV";

const STR_A   = "A";
const STR_B   = "B";
const STR_M1  = "M1";
const STR_M2  = "M2";
const STR_D1  = "D1";
const STR_D2  = "D2";
const STR_R   = "R";
const STR_C   = "C";
const STR_Z   = "Z";
const STR_PC  = "PC";
const STR_IR  = "IR";
const STR_MAR = "MAR";
const STR_MBR = "MBR";

// RAM
int RAM[RAM_LENGTH];

// REGISTRADORES
int A, B, M1, M2, D1, D2, R, C, Z, PC, IR, MBR;
char MAR[STRING_LENGTH];

/* DEFINIÇÕES DAS FUNÇÕES E PROCEDIMENTOS */

int main(void);
bool FileIsOpened(FILE *Arquivo);
void LoadValues(FILE *Arquivo);
void SetValueByAddress(char AValue[STRING_LENGTH], char AAddress[STRING_LENGTH]);
void ExecuteInstrunctions(FILE *Arquivo);
int GetIndexByName(char AName[STRING_LENGTH]);
int * GetAddressByName(char AName[STRING_LENGTH]);
int GetRAMValueByAdress(char AAdress[STRING_LENGTH]);
int GetRegisterValueByAddress(char AAdress[STRING_LENGTH]);
bool IsAddress(char AString[STRING_LENGTH]);

// IMPLEMENTAÇÕES DAS FUNÇÕES ASSEMBLY EM C
void LOAD(int * ARegister, int AValue);
void ADD(int * ARegister, int AValue);
void MOV(int * ARegisterA, int * ARegisterB);
void MULT(int * ARegister, int AValue);
void STORE(int * ARegiter, char AAddress[STRING_LENGTH]);
void DIV(int * ARegister, int AValue);

// SETS DOS REGISTRADORES
void SetPC(void);
void SetM1(int AValue);
void SetM2(int AValue);
void SetD1(int AValue);
void SetD2(int AValue);
void SetR(int AValue);
void SetZ(int AValue);
void SetIR(void);
void SetMAR(char AAddress[STRING_LENGTH]);
void SetMBR(int AValue);

// METÓDOS UTEIS
void CleanConsole(void);
void WaitPressKey(void);
void PrintRAMValues(void);
void PrintRegisterValues(void);
void PrintConsoleBeforeLoadFiles (void);
void PrintConsoleBeforeRunInstrunction(char AMethodName[], char ARegisterName[], char AString[]);
void PrintConsoleAfterInstrunction(void);


/* IMPLEMENTAÇÕES DAS FUNÇÕES E PROCEDIMENTOS */

bool FileIsOpened(FILE *Arquivo){
  return Arquivo != NULL;
}

void LoadValues(FILE *Arquivo){
  char aValue[STRING_LENGTH],
       aAddress[STRING_LENGTH],
       aFlag;

  /*
    MODIFIQUEI O ARQUIVO ORIGINAL COLOCANDO UM CARACTER QUALQUER
    NA PRIMEIRA POSIÇÃO PARA USAR COMO UMA FLAG PORQUE ESTOU UTILIZANDO
    A FUNÇÃO GETC PARA SAIR DO LAÇO QUANDO CHEGAR AO FIM DO ARQUIVO
    E ESSA FUNÇÃO GET "GASTA" O PRIMEIRO CARACTERE.
  */

  while(aFlag = fgetc(Arquivo) != EOF){
    fscanf(Arquivo, "%s %s", &aValue, &aAddress);
    SetValueByAddress(aValue, aAddress);
  }
}

void SetValueByAddress(char AValue[STRING_LENGTH], char AAddress[STRING_LENGTH]){
  /*
    ESSA FUNÇÃO RECEBE DUAS STRINGS, QUE SÃO AAVALUE E AADDRESS, E FAZ A RAM
    NA POSIÇÃO AADDRESS RECEBER O AVALUE.
  */

  RAM[strtol(AAddress, NULL, BASE_HEXADECIMAL)] = atoi(AValue);
}

void ExecuteInstrunctions(FILE *Arquivo){
  char aMethodName[STRING_LENGTH],
       aRegisterName[STRING_LENGTH],
       aString[STRING_LENGTH],
       aFlag;
  int aValue;
  int * aRegisterValue;

  while(aFlag = fgetc(Arquivo) != EOF){
    fscanf(Arquivo, "%s", &aMethodName);
    fscanf(Arquivo, "%s", &aRegisterName);
    fscanf(Arquivo, "%s", &aString);

    if(IsAddress(aString)){
      aValue = GetRAMValueByAdress(aString);
      SetMAR(aString);
      SetMBR(aValue);
    }
    else
      aValue = GetRegisterValueByAddress(aString);

    PrintConsoleBeforeRunInstrunction(aMethodName, aRegisterName, aString);

    switch(GetIndexByName(aMethodName)){
      case INDEX_LOAD:
        LOAD(GetAddressByName(aRegisterName), aValue);
        break;
      case INDEX_ADD:
        ADD(GetAddressByName(aRegisterName), aValue);
        break;
      case INDEX_MOV:
        MOV(GetAddressByName(aRegisterName), GetAddressByName(aString));
        break;
      case INDEX_MULT:
        MULT(GetAddressByName(aRegisterName), aValue);
        break;
      case INDEX_STORE:
        STORE(GetAddressByName(aRegisterName), aString);
        break;
      case INDEX_DIV:
        DIV(GetAddressByName(aRegisterName), aValue);
        break;
      default:
        CleanConsole();
        printf("O COMANDO ""%s"" NAO EH VALIDO.\n", aMethodName);
        system("pause");
    }

    SetPC(); // ATUALIZA O PC
    SetIR(); // ATUALIZA O IR

    PrintConsoleAfterInstrunction();
  }
}

int GetIndexByName(char AName[STRING_LENGTH]){
  /*
    ESSA FUNÇÃO COMPARA O PARÂMETRO ANAME QUE É O NOME DO COMANDO LIDO NO ARQUIVO
    COM AS CONSTANTES E ENTÃO RETORNA UMA CONSTANTE INDEX DE SEU RESPECTIVO COMANDO.
    FIZ ESSA FUNÇÃO PORQUE NÃO DA PARA USAR O SWITCH CASE COM STING EM C.
  */

  if(strncmp(AName, STR_LOAD, sizeof(STR_LOAD)) == MESMA_STRING)
    return INDEX_LOAD;
  else if(strncmp(AName, STR_ADD, sizeof(STR_ADD)) == MESMA_STRING)
    return INDEX_ADD;
  else if(strncmp(AName, STR_MOV, sizeof(STR_MOV)) == MESMA_STRING)
    return INDEX_MOV;
  else if(strncmp(AName, STR_MULT, sizeof(STR_MULT)) == MESMA_STRING)
    return INDEX_MULT;
  else if(strncmp(AName, STR_STORE, sizeof(STR_STORE)) == MESMA_STRING)
    return INDEX_STORE;
  else if(strncasecmp(AName, STR_DIV, sizeof(STR_DIV)) == MESMA_STRING)
    return INDEX_DIV;
  //RETORNA -1 PARA NAO SER RECONHECIDO PELO SWITCH CASE
  else return -1;
}

int * GetAddressByName(char AName[STRING_LENGTH]){
  /*
    ESSA FUNÇÃO COMPARA O PARÂMETRO ANAME QUE É O NOME DO REGISTRADOR LIDO NO ARQUIVO
    COM AS CONSTANTES E RETORNA O ENDEREÇO DE SEU RESPECTIVO REGISTRADOR.
  */

  if(strncmp(AName, STR_A, sizeof(STR_A)) == MESMA_STRING)
    return &A;
  else if(strncmp(AName, STR_B, sizeof(STR_B)) == MESMA_STRING)
    return &B;
  else if(strncmp(AName, STR_M1, sizeof(STR_M1)) == MESMA_STRING)
    return &M1;
  else if(strncmp(AName, STR_M2, sizeof(STR_M2)) == MESMA_STRING)
    return &M2;
  else if(strncmp(AName, STR_D1, sizeof(STR_D1)) == MESMA_STRING)
    return &D1;
  else if(strncmp(AName, STR_D2, sizeof(STR_D2)) == MESMA_STRING)
    return &D2;
  else if(strncmp(AName, STR_R, sizeof(STR_R)) == MESMA_STRING)
    return &R;
  else if(strncmp(AName, STR_C, sizeof(STR_C)) == MESMA_STRING)
    return &C;
  else if(strncmp(AName, STR_Z, sizeof(STR_Z)) == MESMA_STRING)
    return &Z;
  else if(strncmp(AName, STR_PC, sizeof(STR_PC)) == MESMA_STRING)
    return &PC;
  else if(strncmp(AName, STR_IR, sizeof(STR_IR)) == MESMA_STRING)
    return &IR;
  else if(strncmp(AName, STR_MAR, sizeof(STR_MAR)) == MESMA_STRING)
    return &MAR;
  else if(strncmp(AName, STR_MBR, sizeof(STR_MBR)) == MESMA_STRING)
    return &MBR;
}

int GetRAMValueByAdress(char AAdress[STRING_LENGTH]){
  /*
   ESSA FUNÇÃO CONVERTE O PARÂMETRO AADRESS QUE É UMA STRING EM UM INTEIRO
   ATRAVÉS DA FUNÇÃO STRTOL NA BASE 16 E POR FIM RETORNA O VALOR DO VETOR RAM.
  */

  return RAM[strtol(AAdress, NULL, BASE_HEXADECIMAL)];
}

int GetRegisterValueByAddress(char AAdress[STRING_LENGTH]){
  if(strncmp(AAdress, STR_A, sizeof(STR_A)) == MESMA_STRING)
    return A;
  else if(strncmp(AAdress, STR_B, sizeof(STR_B)) == MESMA_STRING)
    return B;
  else if(strncmp(AAdress, STR_M1, sizeof(STR_M1)) == MESMA_STRING)
    return M1;
  else if(strncmp(AAdress, STR_M2, sizeof(STR_M2)) == MESMA_STRING)
    return M2;
  else if(strncmp(AAdress, STR_D1, sizeof(STR_D1)) == MESMA_STRING)
    return D1;
  else if(strncmp(AAdress, STR_D2, sizeof(STR_D2)) == MESMA_STRING)
    return D2;
  else if(strncmp(AAdress, STR_R, sizeof(STR_R)) == MESMA_STRING)
    return R;
  else if(strncmp(AAdress, STR_C, sizeof(STR_C)) == MESMA_STRING)
    return C;
  else if(strncmp(AAdress, STR_Z, sizeof(STR_Z)) == MESMA_STRING)
    return Z;
  else if(strncmp(AAdress, STR_PC, sizeof(STR_PC)) == MESMA_STRING)
    return PC;
  else if(strncmp(AAdress, STR_IR, sizeof(STR_IR)) == MESMA_STRING)
    return IR;
  else if(strncmp(AAdress, STR_MAR, sizeof(STR_MAR)) == MESMA_STRING)
    return MAR;
  else if(strncmp(AAdress, STR_MBR, sizeof(STR_MBR)) == MESMA_STRING)
    return MBR;

  /*
    CASO NAO ACHE UM REGISTRADOR COM ESSE NOME É PORQUE É UM VALOR
    ENTÃO CONVERTE O PARÂMETRO EM INTEIRO E O RETORNA
  */

  else return atoi(AAdress);
}

void LOAD(int * ARegister, int AValue){
  * ARegister = AValue;
}

void ADD(int * ARegister, int AValue){
  * ARegister = * ARegister + AValue;
}

void MOV(int * ARegisterA, int * ARegisterB){
  // SINTAX: MOV REGISTRADOR FONTE, REGISTRADOR DESTINO
  * ARegisterB = * ARegisterA;
}

void MULT(int * ARegister, int AValue){
  SetM1(* ARegister);
  SetM2(AValue);
  * ARegister = M1 * M2;
  SetZ(* ARegister);
}

void STORE(int * ARegiter, char AAddress[STRING_LENGTH]){
  /*
    REUTILIZEI A O PROCEDIMENTO SETVALUEBYADDRESS CONVERTENDO
    O VALOR DO REGISTRADOR PARA STRING UTILIZANDO A FUNCAO ITOA NA BASE 10
  */

  char Aux[STRING_LENGTH];
  itoa(* ARegiter, Aux, 10);
  SetValueByAddress(Aux, AAddress);
}

void DIV(int * ARegister, int AValue){
  SetD1(* ARegister);
  SetD2(AValue);
  SetR(D1 % D2);
  * ARegister = D1 / D2;
  SetZ(* ARegister);
}

void PrintRAMValues(void){
  int i;

  for(i = 0; i < RAM_LENGTH; i++){
    printf("[0x%x] = %i\n", i, RAM[i]);
  }
}

void PrintRegisterValues(void){
  printf("  A = %i\n", A);
  printf("  B = %i\n", B);
  printf(" M1 = %i\n", M1);
  printf(" M2 = %i\n", M2);
  printf(" D1 = %i\n", D1);
  printf(" D2 = %i\n", D2);
  printf("  R = %i\n", R);
  printf("  C = %i\n", C);
  printf("  Z = %i\n", Z);
  printf(" PC = %i\n", PC);
  printf(" IR = %i\n", IR);
  printf("MAR = %s\n", MAR);
  printf("MBR = %i\n", MBR);
}

bool IsAddress(char AString[STRING_LENGTH]){
  return AString[1] == 'x';
}

void CleanConsole(void){
  system("cls");
}

void WaitPressKey(void){
  getchar();
}

void SetPC(void){
  /*
    ESTE METODO INICIA O GERISTRADOR PC COM 1 CASO NAO ESTEJA
    INICIADO E INCREMENTA O REGISTRADOR PC
  */

  if(PC < 1)
    PC = 1;
  else
    PC = PC + 1;
}

void SetM1(int AValue){
  M1 = AValue;  // Altera valor do registrador M1
}

void SetM2(int AValue){
  M2 = AValue;  // Altera valor do registrador M2
}

void SetD1(int AValue){
  D1 = AValue; // Altera valor do registrador D1
}

void SetD2(int AValue){
  D2 = AValue; // Altera valor do registrador D2
}

void SetR(int AValue){
  R = AValue; // Altera valor do registrador R
}

void SetZ(int AValue){
  if(AValue < 0)
    Z = -1;
  else if(AValue == 0)
    Z = 0;
  else
    Z = 1;
}

void SetIR(void){
  /*
    COMO O REGISTRADOR IR GUARDA A PROXIMA INSTRUÇÃO E NESSE CASO
    NÃO EXISTE DESVIO EU DEIXEI O IR COMO O PC + 1;
  */

  IR = PC + 1;
}

void SetMAR(char AAddress[STRING_LENGTH]){
  /*
    PASSA O ENDEREÇO A SER LIDO OU ESCRITO NA RAM PARA O REGISTRADOR MAR
  */

  strcpy(MAR, AAddress);
}

void SetMBR(int AValue){
  /*
    PASSA O VALOR A SER LIDO OU ESCRITO NA RAM PARA O REGISTRADOR MBR
  */

  MBR = AValue;
}

void PrintConsoleBeforeLoadFiles(void){
  /*
    CRIEI ESSE PROCEDIMENTO SÓ PARA PRINTAR OS VALORES INICIAIS
    SEM NENHUMA ALTERAÇÃO.
  */

  printf("********************************************************\n");
  printf("VALORES DOS REGISTRADORES ANTES DE LER OS ARQUIVOS      \n");
  printf("--------------------------------------------------------\n");
  PrintRegisterValues();
  printf("\n--------------------------------------------------------\n");
  printf("VALORES DA MEMORIA RAM ANTES DE LER OS ARQUIVOS         \n");
  printf("--------------------------------------------------------\n");
  PrintRAMValues();
  printf("\n********************************************************\n\n\n\n\n");
}

void PrintConsoleBeforeRunInstrunction(char AMethodName[], char ARegisterName[], char AString[]){
  /*
    CRIEI ESSE PROCEDIMENTO PARA NÃO "POLUIR" O PROCEDIMENTO DE EXECUTAR
    OS MÉTODOS LIDOS DO ARQUIVO
  */

  printf("********************************************************\n");
  printf("INSTRUCAO: %s %s, %s\n", AMethodName, ARegisterName, AString);
  printf("--------------------------------------------------------\n");
  printf("VALORES DOS REGISTRADORES ANTES DE EXECUTAR A INSTRUCAO \n");
  printf("--------------------------------------------------------\n");
  PrintRegisterValues();
  printf("\n--------------------------------------------------------\n");
  printf("VALORES DA MEMORIA RAM ANTES DE EXECUTAR A INSTRUCAO      \n");
  printf("--------------------------------------------------------\n");
  PrintRAMValues();
}

void PrintConsoleAfterInstrunction(void){
  /*
    CRIEI ESSE PROCEDIMENTO PARA NÃO "POLUIR" O PROCEDIMENTO DE EXECUTAR
    OS MÉTODOS LIDOS DO ARQUIVO
  */

  printf("\n--------------------------------------------------------\n");
  printf("VALORES DOS REGISTRADORES DEPOIS DE EXECUTAR A INSTRUCAO  \n");
  printf("--------------------------------------------------------\n");
  PrintRegisterValues();
  printf("\n--------------------------------------------------------\n");
  printf("VALORES DA MEMORIA RAM DEPOIS DE EXECUTAR A INSTRUCAO     \n");
  printf("--------------------------------------------------------\n");
  PrintRAMValues();
  printf("********************************************************\n\n");
}

int main(void){
  FILE *Data = fopen("Data.txt", "r");
  FILE *Instrunctions = fopen("Instrunctions.txt", "r");

  if((!FileIsOpened(Data)) || (!FileIsOpened(Instrunctions))){
    printf("Erro ao tentar abrir os arquivos especificados.\n");
    system("pause");
    return 1;
  }

  SetMAR("0x00");                      // INICIANDO O MAR COM 0x00
  SetMBR(0);                           // INICIANDO O MBR COM 0

  PrintConsoleBeforeLoadFiles();       // PRINTA NO CONSOLE OS VALORES INICIAIS

  LoadValues(Data);                    // CARREGA OS VALORES QUE ESTÃO NO ARQUIVO DATA.TXT NA RAM
  ExecuteInstrunctions(Instrunctions); // EXECUTA OS MÉTODOS QUE ESTÃO No ARQUIVO ROTINAS.TXT

  fclose(Data);
  fclose(Instrunctions);

  return 0;
}
