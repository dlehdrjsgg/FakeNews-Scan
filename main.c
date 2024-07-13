#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif

void clearScreen() {
    system(CLEAR);
}

void PythonScript(const char *url, int index, FILE *OutFile, int *errorFlag, int *errorIndex) {
    char command[512];
    snprintf(command, sizeof(command), "python3 FakeNews.py \"%s\" %d", url, index);
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        perror("> 파이썬 스크립트를 실행할 수 없습니다");
        *errorFlag = 1;
        *errorIndex = index;
        return;
    }

    char result[512];
    while (fgets(result, sizeof(result), fp) != NULL) {
        printf("%s", result);
        fprintf(OutFile, "%s", result);
        if (strstr(result, "오류가 발생했습니다") != NULL) {
            *errorFlag = 1;
            *errorIndex = index;
        }
    }
    pclose(fp);
}

int main() {
    char filename[256];
    printf("[ 가짜 뉴스 판독 프로그램 - AirDrop ]\n");
    printf("> 기사 파일 이름을 입력하세요: ");
    scanf("%s", filename);
    printf("==================================================\n");

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("> 파일을 열 수 없습니다");
        return 1;
    }

    int total = 0;
    char url[256];
    while (fgets(url, sizeof(url), file) && total < 10) {
        if (strlen(url) > 1) {
            total++;
        }
    }
    fseek(file, 0, SEEK_SET);

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char OutFilename[256];
    strftime(OutFilename, sizeof(OutFilename), "%m%d_%H%M%S.txt", t);
    FILE *OutFile = fopen(OutFilename, "w");
    if (OutFile == NULL) {
        perror("> 출력 파일을 생성할 수 없습니다");
        fclose(file);
        return 1;
    }

    int index = 1;
    int errorFlag = 0;
    int errorIndex = -1;
    
    clearScreen();

    for (int i = 1; i <= total; i++) {
        printf("%d번 기사 검사 중...\n", i);
    }
    printf("ChatGPT에 전달 중...\n");
    printf("==================================================\n");

    fprintf(OutFile, "< 탐지 내용 >\n");
    while (fgets(url, sizeof(url), file) && index <= total) {
        url[strcspn(url, "\n")] = '\0';
        if (strlen(url) > 0) {
            fprintf(OutFile, "%d. %s\n", index, url);
            index++;
        }
    }

    fseek(file, 0, SEEK_SET); 
    index = 1;

    fprintf(OutFile, "\n< 탐지 결과 >\n");

    while (fgets(url, sizeof(url), file) && index <= total) {
        url[strcspn(url, "\n")] = '\0';
        if (strlen(url) > 0) {
            PythonScript(url, index, OutFile, &errorFlag, &errorIndex);
            index++;
        }
    }

    fprintf(OutFile, "\n< 탐지 총 결과 >\n");
    if (errorFlag) {
        fprintf(OutFile, "%d번 URL 오류발생, 나머지 정상 탐지\n", errorIndex);
        printf("> %d번 URL 오류발생, 나머지 정상 탐지\n", errorIndex);
    } else {
        fprintf(OutFile, "모두 정상 탐지완료\n");
        printf("> 모두 정상 탐지완료\n");
    }
    fprintf(OutFile, "총 %d개의 기사를 검사하였습니다.\n", total);
    printf("> 총 %d개의 기사를 검사하였습니다.\n", total);

    fclose(OutFile);
    fclose(file);

    return 0;
}