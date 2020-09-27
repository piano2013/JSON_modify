#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include "cJSON.h"

cJSON* open_cfgfile(const char* fname)
{
    FILE* fp;
    cJSON* root;
    int nbyte, size;

    fp = fopen(fname, "rb");
    if (fp == NULL) {
        perror("fopen");
        return NULL;
    }
    char* src;
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    src = malloc(size + 1);
    nbyte = fread(src, 1, size, fp);
    if (nbyte < 0) {
        free(src);
        return NULL;
    }
    cJSON_Minify(src);
    // 解析数据包
    root = cJSON_Parse(src);
    if (!root)
    {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
        return NULL;
    }

    free(src);
    fclose(fp);

    return root;
}

int close_cfgfile(cJSON* json)
{
    // 释放内存空间
    cJSON_Delete(json);

    return 0;
}

cJSON* replace_value_from_cfgfile_with_class(cJSON* json, char* node, char* item, int replaceint)
{
    cJSON* json_value, * root;
    root = cJSON_GetObjectItem(json, node);
    if (!root)
        return NULL;

    json_value = cJSON_GetObjectItem(root, item);
    if (json_value && json_value->type == cJSON_Number)
    {
        json_value->valueint = replaceint;
        json_value->valuedouble = replaceint;
    }
    else
        return NULL;

    return json;
}

cJSON* replace_value_from_cfgfile(cJSON* json, char* item, int replaceint)
{
    cJSON* json_value;
    json_value = cJSON_GetObjectItem(json, item);
    if (!json_value)
        return NULL;

    if (json_value && json_value->type == cJSON_Number)
    {
        json_value->valueint = replaceint;
        json_value->valuedouble = replaceint;
    }
    else
        return NULL;

    return json;
}

int main(void)
{
    int numvalue = 110;
    cJSON* json_before = open_cfgfile("create.json");
    char* node = cJSON_Print(json_before);
    printf("modify before:");
    printf(node);
    printf("\n");
    
    cJSON* json_after = replace_value_from_cfgfile(json_before, "PCI", numvalue);
    char* node1 = cJSON_Print(json_after);
    printf("modify1 after:");
    printf(node1);
    printf("\n");
    cJSON* json_after1 = replace_value_from_cfgfile(json_after, "SCS", numvalue);
    char* node2 = cJSON_Print(json_after1);
    printf("modify2 after:");
    printf(node2);

    FILE* fp = fopen("create.json", "w+");
    char* buff = cJSON_Print(json_after1);
    printf("%d",strlen(buff));
    fwrite(buff, strlen(buff), 1, fp);
    //清理工作
    fclose(fp);
    close_cfgfile(json_before);
    
    return 0;
}