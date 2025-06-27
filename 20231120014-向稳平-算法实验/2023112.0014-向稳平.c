#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int idx, w, v;
    double ratio;
} Item;

int maxBrute = 0;
int maxBacktrack = 0;

// 蛮力法前向声明
static void dfs(int i, int cw, int cv, int w[], int v[], int n, int C, clock_t start_cpu, int *timeout);

void brute_force(int w[], int v[], int n, int C) {
    time_t start_real, end_real;
    clock_t start_cpu, end_cpu;

    time(&start_real);
    printf("\n蛮力法 开始时间: %s", asctime(localtime(&start_real)));
    start_cpu = clock();

    maxBrute = 0;
    int timeout = 0;

    dfs(0, 0, 0, w, v, n, C, start_cpu, &timeout);

    end_cpu = clock();
    time(&end_real);
    printf("蛮力法 结束时间: %s", asctime(localtime(&end_real)));
    double diff = (double)(end_cpu - start_cpu) * 1000 / CLOCKS_PER_SEC;
    printf("蛮力法 时间差: %.2lf ms\n", diff);
    if (timeout)
        printf("蛮力法 超时！计算时间超过20秒，自动终止。\n");
    else
        printf("蛮力法 最大价值: %d\n", maxBrute);
}

// 蛮力法递归DFS实现
static void dfs(int i, int cw, int cv, int w[], int v[], int n, int C, clock_t start_cpu, int *timeout) {
    if (*timeout) return;
    if (i == n) {
        if (cw <= C && cv > maxBrute) maxBrute = cv;
        return;
    }
    clock_t now = clock();
    double elapsed_ms = (double)(now - start_cpu) * 1000 / CLOCKS_PER_SEC;
    if (elapsed_ms > 20000) {
        *timeout = 1;
        return;
    }
    dfs(i + 1, cw, cv, w, v, n, C, start_cpu, timeout);
    dfs(i + 1, cw + w[i], cv + v[i], w, v, n, C, start_cpu, timeout);
}

// 动态规划
void dp(int w[], int v[], int n, int C) {
    time_t start_real, end_real;
    clock_t start_cpu, end_cpu;

    time(&start_real);
    printf("\n动态规划 开始时间: %s", asctime(localtime(&start_real)));
    start_cpu = clock();

    int **f = (int **)malloc((n+1) * sizeof(int *));
    for (int i = 0; i <= n; i++) f[i] = (int *)calloc(C+1, sizeof(int));
    int *selected = (int *)calloc(n, sizeof(int));

    int timeout = 0;

    for (int i = 1; i <= n && !timeout; i++) {
        for (int j = 0; j <= C; j++) {
            f[i][j] = (w[i-1] > j) ? f[i-1][j] : (f[i-1][j] > f[i-1][j-w[i-1]] + v[i-1] ? f[i-1][j] : f[i-1][j-w[i-1]] + v[i-1]);
        }
        clock_t now = clock();
        double elapsed_ms = (double)(now - start_cpu) * 1000 / CLOCKS_PER_SEC;
        if (elapsed_ms > 20000) {
            timeout = 1;
            break;
        }
    }

    end_cpu = clock();
    time(&end_real);
    printf("动态规划 结束时间: %s", asctime(localtime(&end_real)));
    double diff = (double)(end_cpu - start_cpu) * 1000 / CLOCKS_PER_SEC;
    printf("动态规划 时间差: %.2lf ms\n", diff);

    if (timeout) {
        printf("动态规划 超时！计算时间超过20秒，自动终止。\n");
    } else {
        int ans = f[n][C], j = C;
        for (int i = n; i >= 1; i--)
            if (f[i][j] != f[i-1][j]) {
                selected[i-1] = 1;
                j -= w[i-1];
            }
        printf("动态规划 最大价值: %d\n", ans);

      
        int totalW = 0;
        for (int i = 0; i < n; i++)
            if (selected[i]) {
                totalW += w[i];
            }
        printf("\n动态规划 总重量: %d\n", totalW);
    }

    for (int i = 0; i <= n; i++) free(f[i]);
    free(f); free(selected);
}

// 贪心法
int cmp(const void *a, const void *b) {
    double r1 = ((Item*)a)->ratio;
    double r2 = ((Item*)b)->ratio;
    return (r2 > r1) - (r2 < r1);
}

void greedy(Item items[], int n, int C) {
    time_t start_real, end_real;
    clock_t start_cpu, end_cpu;

    time(&start_real);
    printf("\n贪心法 开始时间: %s", asctime(localtime(&start_real)));
    start_cpu = clock();

    qsort(items, n, sizeof(Item), cmp);

    int *selected = (int *)calloc(n, sizeof(int));
    int totalV = 0, totalW = 0;

    int timeout = 0;

    for (int i = 0; i < n; i++) {
        if (totalW + items[i].w <= C) {
            totalW += items[i].w;
            totalV += items[i].v;
            selected[items[i].idx] = 1;
        }
        clock_t now = clock();
        double elapsed_ms = (double)(now - start_cpu) * 1000 / CLOCKS_PER_SEC;
        if (elapsed_ms > 30000) {
            timeout = 1;
            break;
        }
    }

    end_cpu = clock();
    time(&end_real);
    printf("贪心法 结束时间: %s", asctime(localtime(&end_real)));
    double diff = (double)(end_cpu - start_cpu) * 1000 / CLOCKS_PER_SEC;
    printf("贪心法 时间差: %.2lf ms\n", diff);

    if (timeout) {
        printf("贪心法 超时！计算时间超过30秒，自动终止。\n");
    } else {
        printf("贪心法 近似最大价值: %d\n", totalV);

        int totalW = 0;
        for (int i = 0; i < n; i++)
            if (selected[i]) {
                totalW += items[i].w;
            }
        printf("贪心法 总重量: %d\n", totalW);
    }

    free(selected);
}

// 回溯法
static void bt(int i, int cw, int cv, int w[], int v[], int n, int C, clock_t start_cpu, int *timeout, int *maxBacktrack) {
    if (*timeout) return;
    if (cw > C) return;
    if (i == n) {
        if (cv > *maxBacktrack) *maxBacktrack = cv;
        return;
    }
    clock_t now = clock();
    double elapsed_ms = (double)(now - start_cpu) * 1000 / CLOCKS_PER_SEC;
    if (elapsed_ms > 20000) {
        *timeout = 1;
        return;
    }
    bt(i + 1, cw, cv, w, v, n, C, start_cpu, timeout, maxBacktrack);
    bt(i + 1, cw + w[i], cv + v[i], w, v, n, C, start_cpu, timeout, maxBacktrack);
}

void backtrack(int w[], int v[], int n, int C) {
    time_t start_real, end_real;
    clock_t start_cpu, end_cpu;

    time(&start_real);
    printf("\n回溯法 开始时间: %s", asctime(localtime(&start_real)));
    start_cpu = clock();

    maxBacktrack = 0;
    int timeout = 0;

    bt(0, 0, 0, w, v, n, C, start_cpu, &timeout, &maxBacktrack);

    end_cpu = clock();
    time(&end_real);
    double diff = (double)(end_cpu - start_cpu) * 1000 / CLOCKS_PER_SEC;

    printf("回溯法 结束时间: %s", asctime(localtime(&end_real)));
    printf("回溯法 时间差: %.2lf ms\n", diff);

    if (timeout)
        printf("回溯法 超时！计算时间超过30秒，自动终止。\n");
    else
        printf("回溯法 最大价值: %d\n", maxBacktrack);
}

int main() {
    int n, C, choice;

    printf("输入物品数量 n: ");
    scanf("%d", &n);
    printf("输入背包容量 C: ");
    scanf("%d", &C);

    srand(time(NULL));
    int *w = (int *)malloc(n * sizeof(int));
    int *v = (int *)malloc(n * sizeof(int));
    Item *items = (Item *)malloc(n * sizeof(Item));

    for (int i = 0; i < n; i++) {
        w[i] = rand() % 100 + 1;
        v[i] = rand() % 901 + 100;
        items[i].idx = i;
        items[i].w = w[i];
        items[i].v = v[i];
        items[i].ratio = (double)v[i] / w[i];
    }

    while (1) {
        printf("\n选择算法:\n 1-蛮力法\n 2-动态规划\n 3-贪心法\n 4-回溯法\n 0-退出\n输入编号: ");
        scanf("%d", &choice);
        if (choice == 0) break;
        if (choice == 1) brute_force(w, v, n, C);
        else if (choice == 2) dp(w, v, n, C);
        else if (choice == 3) greedy(items, n, C);
        else if (choice == 4) backtrack(w, v, n, C);
        else printf("无效选择\n");
    }

    free(w); free(v); free(items);
    return 0;
}
