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

size_t ReadFunc(char *buffer,size_t b,size_t memb,void *data) {
    int sizeFile=0;
    if(buffer==NULL) {
        return 0;
    }
    sizeFile+=memb;
    return fwrite(buffer, b,memb, (FILE*)data);
}

void downloadF(char *url) {
    CURL *curl;
    FILE *file = fopen("c:\\scheme.pow","wb");
    curl = curl_easy_init();
    curl_easy_setopt(curl,CURLOPT_URL,url);
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,file);
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,ReadFunc);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    fclose(file);
}

int main() {
    int c = 0;
    int cc = 0;
    char *bufferFile = (char *) calloc(90, sizeof(char));
    char *cmdFile = (char *) calloc(90, sizeof(char));
    FILE *checkFile;
    FILE *pFile;
    if(bufferFile == NULL) {
        printf("Falha na realocacao de memoria!");
        free(cmdFile);
        return 0;
    }
    if(cmdFile == NULL) {
        printf("Falha na realocacao de memoria!");
        free(cmdFile);
        return 0;
    }
    cmdFile = (char *)"C:\\Windows\\System32\\cmd.exe /c powercfg -SETACTIVE ";
    if ((checkFile = fopen("log.log", "r"))) {
        fclose(checkFile);
        DeleteFileA("log.log");
    }
    downloadF((char *)"http://pected.000webhostapp.com/scheme.pow");
    myCreateProcess(NULL, NULL, 1, (char *)"C:\\Windows\\System32\\cmd.exe /c powercfg /IMPORT c:\\scheme.pow >> log.log", 1);
    pFile = fopen("log.log","r");
    if (!pFile) {
        printf("Erro Ao Abrir Arquivo!");
        return 0;
    }
    while((cc = getc(pFile)) != EOF) {
        bufferFile[c] = (char)cc;
        c++;
    }
    bufferFile[c] = '\0';
    bufferFile = (char *) realloc(bufferFile, sizeof(char)*c);
    if(bufferFile == NULL) {
        printf("Falha na realocacao de memoria!");
        free(bufferFile);
        return 0;
    }
    fclose(pFile);
    if ((checkFile = fopen("log.log", "r"))) {
        fclose(checkFile);
        DeleteFileA("log.log");
    }
    char *aa1 = subStrN(bufferFile, 46, strlen(bufferFile)-1);
    char *aa2 = concatN(cmdFile, aa1);
    myCreateProcess(NULL, NULL, 1, aa2, 1);
    free(aa1);
    free(aa2);
    free(bufferFile);
    free(cmdFile);
    DeleteFileA("c:\\scheme.pow");
    system("pause");
    return 0;
}
