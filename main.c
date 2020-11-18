#include <windows.h>
#include <string.h> /*strlen, */
#include <stdio.h> /* printf, scanf, NULL */
#include <stdlib.h> /* malloc, calloc, exit, free */

#include "include/curl/curl.h"

#define INITIAL_BUFFER (MAX_PATH * 5)

void myCreateProcess (LPCTSTR lpApplicationName, LPCTSTR lpCurrentDirectory, int wShow, char *args, int isCMD) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    if (wShow == 1)
        si.wShowWindow = SW_MINIMIZE;
    ZeroMemory(&pi, sizeof(pi));
    if (!CreateProcessA(lpApplicationName, args, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, lpCurrentDirectory, &si, &pi)) {
        MessageBoxA(NULL, "Erro ao criar processo", "Error", MB_ICONEXCLAMATION|MB_OK);
    } else if(isCMD) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
}

char *subStrN(char *str, int n1, int n2) {
    int c = 0;
    char *newSTR = (char*)malloc(sizeof (char) * 150);
    if(newSTR == NULL) {
        printf("Falha na realocacao de memoria!");
        free(newSTR);
        return NULL;
    }
    for(int i = 0; i < (signed int)strlen(str); i++) {
        if(i >= n1 && i <= n2) {
            newSTR[c] = str[i];
            c++;
        }
    }
    newSTR[c] = '\0';
    return newSTR;
}

char *concatN(char *str, char *str2) {
    int c = 0;
    int c1 = 0;
    char *newSTR = (char*)malloc(sizeof (char) * 150);
    do {
        newSTR[c] = str[c];
        c++;
    } while(str[c] != '\0');
    do {
        newSTR[c] = str2[c1];
        c1++;
        c++;
    } while(str2[c1] != '\0');
    newSTR[c] = '\0';
    return newSTR;
}

int Search_in_File(char *fname, char *str) {
    FILE *fp;
    int find_result = 0;
    char temp[512];
    if((fp = fopen(fname, "r")) == NULL) {
        return 0;
    }
    while(fgets(temp, 512, fp) != NULL) {
        if((strstr(temp, str)) != NULL) {
            find_result++;
            break;
        }
    }
    if(fp) {
        fclose(fp);
    }
    if(find_result == 0) {
        return 0;
    } else {
        return 1;
    }
}

char *getGuidStrFile(char *fname, char *str) {
    FILE *fp;
    char temp[512];
    char *newStr = (char *)calloc(200, sizeof(char));
    if(newStr == NULL) {
        printf("Falha na alocacao de memoria!");
        free(newStr);
        system("pause");
        return NULL;
    }
    if((fp = fopen(fname, "r")) == NULL) {
        return NULL;
    }
    while(fgets(temp, 512, fp) != NULL) {
        if((strstr(temp, str)) != NULL) {
            newStr = temp;
            break;
        }
    }
    if(fp) {
        fclose(fp);
    }
    newStr = subStrN(newStr, 28, 64);
    newStr = (char *)realloc(newStr, (strlen(newStr)+1) * sizeof(char));
    if(newStr == NULL) {
        printf("Falha na (re)alocacao de memoria!");
        free(newStr);
        system("pause");
        return NULL;
    }
    return newStr;
}

size_t ReadFunc(char *buffer, size_t b, size_t memb, void *data) {
    int sizeFile=0;
    if(buffer==NULL) {
        return 0;
    }
    sizeFile+=memb;
    return fwrite(buffer, b,memb, (FILE*)data);
}

int downloadF(char *url) {
    char tempPath[MAX_PATH];
    char *tempFullPath = (char *) calloc(MAX_PATH, sizeof(char));;
    if(tempFullPath == NULL) {
        printf("Falha na alocacao de memoria!");
        free(tempFullPath);
        return 0;
    }
    GetTempPathA(sizeof(tempPath), tempPath);
    tempFullPath = tempPath;
    tempFullPath = concatN(tempFullPath, (char *)"scheme.pow");
    tempFullPath = (char *) realloc(tempFullPath, sizeof(char)*strlen(tempFullPath)+1);
    if(tempFullPath == NULL) {
        printf("Falha na realocacao de memoria!");
        free(tempFullPath);
        return 0;
    }
    CURL *curl;
    CURLcode res;
    FILE *file = fopen(tempFullPath, "wb");
    curl = curl_easy_init();
    curl_easy_setopt(curl,CURLOPT_URL, url);
    curl_easy_setopt(curl,CURLOPT_WRITEDATA, file);
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION, ReadFunc);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    fclose(file);
    free(tempFullPath);
    if(res == CURLE_OK) {
        return 1;
    } else {
        printf("%i: ", res);
        return 0;
    }
}

int main() {
    char tempPath[MAX_PATH];
    GetTempPathA(sizeof(tempPath), tempPath);
    int c = 0;
    int cc = 0;
    char *bufferFile = (char *) calloc(200, sizeof(char));
    char *cmdCommand = (char *) calloc(200, sizeof(char));
    char *schemePath = (char *) calloc(200, sizeof(char));
    FILE *checkFile;
    FILE *pFile;
    if(bufferFile == NULL) {
        printf("Falha na alocacao de memoria!\n");
        free(cmdCommand);
        system("pause");
        return 0;
    }
    if(cmdCommand == NULL) {
        printf("Falha na alocacao de memoria!\n");
        free(cmdCommand);
        system("pause");
        return 0;
    }

    //desativa hibernação
    myCreateProcess(NULL, NULL, 1, (char *)"C:\\Windows\\System32\\cmd.exe /c powercfg /hibernate off", 1);

    //apaga arquivo de log
    if ((checkFile = fopen("log.log", "r"))) {
        fclose(checkFile);
        DeleteFileA("log.log");
    }

    //gera uma lista com todos planos de energia
    myCreateProcess(NULL, NULL, 1, (char *)"C:\\Windows\\System32\\cmd.exe /c powercfg /list >> log.log", 1);

    //percorre lista de planos de energia, e apaga todas ocorrencias que contem >> (Meu Plano Personalizado 1)
    if ((checkFile = fopen("log.log", "r"))) {
        fclose(checkFile);
        //altera para o plano balanceado padrão do windows...
        if(Search_in_File((char *)"log.log", (char *)"Equilibrado") || Search_in_File((char *)"log.log", (char *)"Balanced")) {
            bufferFile = getGuidStrFile((char *)"log.log", (char *)"Equilibrado");
            bufferFile = (char *)realloc(bufferFile, (strlen(bufferFile)+1)*sizeof(char));
            if(bufferFile == NULL) {
                printf("Falha na (re)alocacao de memoria!\n");
                free(bufferFile);
                system("pause");
                return 0;
            }
            cmdCommand = (char *)"C:\\Windows\\System32\\cmd.exe /c powercfg -SETACTIVE ";
            cmdCommand = concatN(cmdCommand, bufferFile);
            myCreateProcess(NULL, NULL, 1, cmdCommand, 1);
        }
        //apaga todas ocorrencias que contem >> (Meu Plano Personalizado 1)
        do {
            bufferFile = getGuidStrFile((char *)"log.log", (char *)"(Meu Plano Personalizado 1)");
            bufferFile = (char *)realloc(bufferFile, (strlen(bufferFile)+1)*sizeof(char));
            if(bufferFile == NULL) {
                printf("Falha na (re)alocacao de memoria!\n");
                free(bufferFile);
                system("pause");
                return 0;
            }
            cmdCommand = (char *)"C:\\Windows\\System32\\cmd.exe /c powercfg /delete ";
            cmdCommand = concatN(cmdCommand, bufferFile);
            myCreateProcess(NULL, NULL, 1, cmdCommand, 1);
            DeleteFileA("log.log");
            myCreateProcess(NULL, NULL, 1, (char *)"C:\\Windows\\System32\\cmd.exe /c powercfg /list >> log.log", 1);
        } while(Search_in_File((char *)"log.log", (char *)"(Meu Plano Personalizado 1)"));
        DeleteFileA("log.log");
        //realoca os ponteiros usados...
        bufferFile = (char *) realloc(bufferFile, 200*sizeof(char));
        if(bufferFile == NULL) {
            printf("Falha na realocacao de memoria!\n");
            free(bufferFile);
            system("pause");
            return 0;
        }
        cmdCommand = (char *) realloc(cmdCommand, 200*sizeof(char));
        if(cmdCommand == NULL) {
            printf("Falha na realocacao de memoria!\n");
            free(cmdCommand);
            system("pause");
            return 0;
        }
    }
    //obtem o path completo do arquivo de esquema
    schemePath = tempPath;
    schemePath = concatN(schemePath, (char *)"scheme.pow");
    schemePath = (char *) realloc(schemePath, sizeof(char)*strlen(schemePath)+1);
    if(schemePath == NULL) {
        printf("Falha na realocacao de memoria!\n");
        free(schemePath);
        system("pause");
        return 0;
    }
    //faz download do arquivo de esquema
    if (!downloadF((char *)"http://pected.000webhostapp.com/scheme.pow")) {
        printf("Erro Ao Fazer Download Do Esquema De Energia!\n");
        system("pause");
        return 0;
    }

    //importa o arquivo de esquema e gera um arquivo de log
    cmdCommand = (char *)"C:\\Windows\\System32\\cmd.exe /c powercfg /IMPORT ";
    cmdCommand = concatN(cmdCommand, tempPath);
    cmdCommand = concatN(cmdCommand, (char *)"scheme.pow >> log.log");
    cmdCommand = (char *) realloc(cmdCommand, sizeof(char)*strlen(cmdCommand)+1);
    if(cmdCommand == NULL) {
        printf("Falha na realocacao de memoria!\n");
        free(cmdCommand);
        system("pause");
        return 0;
    }
    myCreateProcess(NULL, NULL, 1, cmdCommand, 1);

    //obtem o log para uma variavel bufferFile
    pFile = fopen("log.log","r");
    if (!pFile) {
        printf("Erro Ao Abrir Arquivo!\n");
        return 0;
    }
    while((cc = getc(pFile)) != EOF) {
        bufferFile[c] = (char)cc;
        c++;
    }
    bufferFile[c] = '\0';
    bufferFile = (char *) realloc(bufferFile, c*sizeof(char));
    if(bufferFile == NULL) {
        printf("Falha na realocacao de memoria!\n");
        free(bufferFile);
        system("pause");
        return 0;
    }
    fclose(pFile);

    //deleta o arquivo de log
    if ((checkFile = fopen("log.log", "r"))) {
        fclose(checkFile);
        DeleteFileA("log.log");
    }

    //realoca a variavel cmdCommand
    cmdCommand = (char *) realloc(cmdCommand, sizeof(char)*200);
    if(cmdCommand == NULL) {
        printf("Falha na realocacao de memoria!\n");
        free(cmdCommand);
        system("pause");
        return 0;
    }

    //ativa o esquema de energia importado
    bufferFile = subStrN(bufferFile, 46, strlen(bufferFile)-1);
    cmdCommand = (char *)"C:\\Windows\\System32\\cmd.exe /c powercfg -SETACTIVE ";
    cmdCommand = concatN(cmdCommand, bufferFile);
    cmdCommand = (char *) realloc(cmdCommand, sizeof(char)*strlen(cmdCommand)+1);
    if(cmdCommand == NULL) {
        printf("Falha na realocacao de memoria!\n");
        free(cmdCommand);
        system("pause");
        return 0;
    }
    myCreateProcess(NULL, NULL, 1, cmdCommand, 1);

    //deleta o arquivo de esquema da pasta %temp%
    DeleteFileA(schemePath);

    //libera memoria
    free(bufferFile);
    free(cmdCommand);
    free(schemePath);
    return 0;
}
