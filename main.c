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
        return 0;
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
    schemePath = tempPath;
    schemePath = concatN(schemePath, (char *)"scheme.pow");
    schemePath = (char *) realloc(schemePath, sizeof(char)*strlen(schemePath)+1);
    if(schemePath == NULL) {
        printf("Falha na realocacao de memoria!1\n");
        free(schemePath);
        system("pause");
        return 0;
    }
    cmdCommand = (char *)"C:\\Windows\\System32\\cmd.exe /c powercfg /IMPORT ";
    cmdCommand = concatN(cmdCommand, tempPath);
    cmdCommand = concatN(cmdCommand, (char *)"scheme.pow >> log.log");
    cmdCommand = (char *) realloc(cmdCommand, sizeof(char)*strlen(cmdCommand)+1);
    if(cmdCommand == NULL) {
        printf("Falha na realocacao de memoria!1\n");
        free(cmdCommand);
        system("pause");
        return 0;
    }
    if ((checkFile = fopen("log.log", "r"))) {
        fclose(checkFile);
        DeleteFileA("log.log");
    }
    if (!downloadF((char *)"http://pected.000webhostapp.com/scheme.pow")) {
        printf("Erro Ao Fazer Download Do Esquema De Energia!\n");
        system("pause");
        return 0;
    }
    myCreateProcess(NULL, NULL, 1, cmdCommand, 1);
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
    bufferFile = (char *) realloc(bufferFile, sizeof(char)*c);
    if(bufferFile == NULL) {
        printf("Falha na realocacao de memoria!2\n");
        free(bufferFile);
        system("pause");
        return 0;
    }
    fclose(pFile);
    if ((checkFile = fopen("log.log", "r"))) {
        fclose(checkFile);
        DeleteFileA("log.log");
    }
    cmdCommand = (char *) realloc(cmdCommand, sizeof(char)*200);
    if(cmdCommand == NULL) {
        printf("Falha na realocacao de memoria!3\n");
        free(cmdCommand);
        system("pause");
        return 0;
    }
    bufferFile = subStrN(bufferFile, 46, strlen(bufferFile)-1);
    cmdCommand = (char *)"C:\\Windows\\System32\\cmd.exe /c powercfg -SETACTIVE ";
    cmdCommand = concatN(cmdCommand, bufferFile);
    cmdCommand = (char *) realloc(cmdCommand, sizeof(char)*strlen(cmdCommand)+1);
    if(cmdCommand == NULL) {
        printf("Falha na realocacao de memoria!4\n");
        free(cmdCommand);
        system("pause");
        return 0;
    }
    myCreateProcess(NULL, NULL, 1, cmdCommand, 1);
    DeleteFileA(schemePath);
    free(bufferFile);
    free(cmdCommand);
    free(schemePath);
    system("pause");
    return 0;
}
