#pragma once

#include <functional>
#include <string>

// Functii template libere
// Implementarile raman in .h!

// cautaLiniar: O(n) parcurgere simpla
template<typename T>
int cautaLiniar(T* const* arr, int n, const std::function<bool(const T*)>& pred) {
    for (int i = 0; i < n; i++) {
        if (arr[i] != nullptr && pred(arr[i])) {
            return i;
        }
    }
    return -1;
}

// Specializare pentru ResursaCloud* pt cautat ID mai usor
#include "ResursaCloud.h"

template<>
inline int cautaLiniar<ResursaCloud>(ResursaCloud* const* arr, int n,
                                     const std::function<bool(const ResursaCloud*)>& pred) {
    for (int i = 0; i < n; i++) {
        if (arr[i] != nullptr && pred(arr[i])) {
            return i;
        }
    }
    return -1;
}

// Cautare dupa ID direct
inline int cautaDupaId(ResursaCloud* const* arr, int n, int idCautat) {
    auto pred = [idCautat](const ResursaCloud* r) -> bool {
        return r->getId() == idCautat;
    };
    return cautaLiniar<ResursaCloud>(arr, n, pred);
}

// sorteaza: Bubble sort generic, O(n^2)
template<typename T, typename Cheie>
void sorteaza(T** arr, int n, const std::function<Cheie(const T*)>& extractor) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            auto cheie_j = extractor(arr[j]);
            auto cheie_j1 = extractor(arr[j + 1]);
            if (cheie_j1 < cheie_j) {
                T* temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

// numara: parcurge si numara, O(n)
template<typename T>
int numara(T* const* arr, int n, const std::function<bool(const T*)>& pred) {
    int contor = 0;
    for (int i = 0; i < n; i++) {
        if (arr[i] != nullptr && pred(arr[i])) {
            contor++;
        }
    }
    return contor;
}
