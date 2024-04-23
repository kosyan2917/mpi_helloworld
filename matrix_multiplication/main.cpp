#include <iostream>
#include <vector>
#include <random>
#include <omp.h>
#include <sys/time.h>
#include <chrono>
#include <matplot/matplot.h>

typedef std::vector<std::vector<int>> Matrix;

Matrix generateMatrix(int n) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 10);

    Matrix matrix(n, std::vector<int>(n));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = dis(gen);
        }
    }

    return matrix;
}

void sequentialSolve(const Matrix &A, const Matrix &B, Matrix &result) {
    int n = A.size();

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }     
}

void parallelSolve(const Matrix &A, const Matrix &B, Matrix &result) {
    int n = A.size();

    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void transpose(Matrix &matrix) {
    int n = matrix.size();
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            std::swap(matrix[i][j], matrix[j][i]);
        }
    }
}


void optimizedParallelSolve(const Matrix &A, Matrix &B, Matrix &result) {
    int n = A.size();
    transpose(B);
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                result[i][j] += A[i][k] * B[j][k];
            }
        }
    }
    
}

void printMatrix(Matrix matrix) {
    int n = matrix.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

int main() {
    // Example usage
    omp_set_num_threads(10);
    setbuf(stdout, NULL);
    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    std::vector<int> sizes = {};
    std::vector<int> times = {};
    std::vector<int> times_optimized = {};
    for (int n = 100; n <= 2000; n += 100) {
        sizes.push_back(n);
        Matrix A = generateMatrix(n);
        Matrix B = generateMatrix(n);
        Matrix C(n, std::vector<int>(n));
        start = std::chrono::high_resolution_clock::now();
        parallelSolve(A, B, C);
        end = std::chrono::high_resolution_clock::now();
        times.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
        start = std::chrono::high_resolution_clock::now();
        optimizedParallelSolve(A, B, C);
        end = std::chrono::high_resolution_clock::now();
        times_optimized.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
    }
    printf("%d %d", times[times.size()-1], times_optimized[times_optimized.size()-1]);
    matplot::plot(sizes, times);
    matplot::hold(matplot::on);
    matplot::plot(sizes, times_optimized);
    matplot::show();
    


    // printf("Sequential test started");
    // gettimeofday(&tbefore, NULL);
    // sequentialSolve(A, B, C);
    // gettimeofday(&tafter, NULL);
    // timersub(&tafter, &tbefore, &telapsed);
    // printf(" Elapsed: %ld.%06ld\n", (long int)telapsed.tv_sec, (long int)telapsed.tv_usec);
    

    // printf("parallel test started");
    // auto start = std::chrono::high_resolution_clock::now();
    // parallelSolve(A, B, C);
    // auto end = std::chrono::high_resolution_clock::now();
    // printf(" Elapsed: %ld\n", std::chrono::duration_cast<std::chrono::milliseconds>(end - start));

    // printf("optimized parallel test started");
    // start = std::chrono::high_resolution_clock::now();
    // optimizedParallelSolve(A, B, C);
    // end = std::chrono::high_resolution_clock::now();
    // printf(" Elapsed: %ld\n", std::chrono::duration_cast<std::chrono::milliseconds>(end - start));

    return 0;
}