//
// Created by gyankos on 13/10/26.
//

#include <iostream>
#include <chrono>
#include <unordered_map>
#include <list>
#include <string>
#include <vector>
#include <iomanip>

// Inclusione degli header no-lock
#include "../../include/yaucl/structures/gamma_context.h"


// Modello di riferimento standard: LinkedHashMap basata su STL nativa (con clonazione totale)
template <typename K, typename V>
struct StandardLinkedHashMap {
    std::list<std::pair<K, V>> linear_list;
    std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator> lookup;

    StandardLinkedHashMap() {}
    StandardLinkedHashMap(const StandardLinkedHashMap<K,V>& x) : linear_list(x.linear_list) {
        for (auto it = linear_list.begin(); it != linear_list.end(); ++it) {
            lookup.emplace(it->first, it);
        }
    }

    void emplace(K key, V val) {
        auto* ptr = find(key);
        if (ptr != nullptr) {
            update_value(key, val);
        } else {
            linear_list.emplace_back(key, val);
            auto it = linear_list.end();
            --it;
            lookup[key] = it;
        }
    }

    V* find(const K& key) {
        auto it = lookup.find(key);
        if (it == lookup.end()) return nullptr;
        return &(it->second->second);
    }

    void update_value(const K& key, V new_val) {
        auto it = lookup.find(key);
        if (it == lookup.end()) return;

        linear_list.erase(it->second);
        linear_list.emplace_back(key, new_val);

        auto new_it = linear_list.end();
        --new_it;
        lookup[key] = new_it;
    }
};

int main() {
    // Configurazione fissa dei fattori di ramificazione per garantire l'equità
    const std::size_t RAMI_BACKTRACKING  = 50;     // Numero costante di sotto-contesti per passo
    const std::size_t LOOKUP_PER_RAMO    = 20000;  // Numero costante di risoluzioni di tipo
    const std::size_t SHADOWING_PER_RAMO = 100;    // Numero costante di ridefinizioni locali (Shadowing)

    // Generiamo in anticipo le chiavi per evitare che il costo di std::to_string sporchi il benchmark
    const std::size_t MAX_ELEMENTI = 10000000;
    std::vector<std::string> precomputed_keys;
    precomputed_keys.reserve(MAX_ELEMENTI);
    for (std::size_t i = 0; i < MAX_ELEMENTI; ++i) {
        precomputed_keys.push_back("var_global_" + std::to_string(i));
    }

    // Intestazione dell'output in formato CSV per un plot immediato
    std::cout << "Dimensione_Gamma,Tempo_Vista_Ibrida_ms,Tempo_Mappa_Flat_ms,Rapporto_Velocita" << std::endl;

    // Incrementiamo gradualmente la quantità di dati nel contesto originale (da 10k a 100k, passi da 10k)
    for (std::size_t dimensione_gamma = 10; dimensione_gamma <= MAX_ELEMENTI; dimensione_gamma *= 10) {



        // ----------------------------------------------------------------====
        // ESPERIMENTO 2: CLONAZIONE COMPLETA DELLA UNORDERED_MAP (Dati identici)
        // ----------------------------------------------------------------====
        auto t2_start = std::chrono::high_resolution_clock::now();

        StandardLinkedHashMap<std::string, std::string> Gamma_global_flat;
        for (std::size_t i = 0; i < dimensione_gamma; ++i) {
            Gamma_global_flat.emplace(precomputed_keys[i], "Type_Int");
        }

        for (std::size_t b = 0; b < RAMI_BACKTRACKING; ++b) {
            StandardLinkedHashMap<std::string, std::string> Gamma_child_flat = Gamma_global_flat;

            // Identica sequenza di lookup
            for (std::size_t l = 0; l < LOOKUP_PER_RAMO; ++l) {
                std::size_t target = (l * 13) % dimensione_gamma;
                Gamma_child_flat.find(precomputed_keys[target]);
            }

            // Identica sequenza di shadowing
            for (std::size_t s = 0; s < SHADOWING_PER_RAMO; ++s) {
                Gamma_child_flat.update_value(precomputed_keys[s], "Type_Overloaded_Float");
            }
        }

        auto t2_end = std::chrono::high_resolution_clock::now();
        auto durata_flat = std::chrono::duration_cast<std::chrono::milliseconds>(t2_end - t2_start).count();

        // ========================================================================
        // ESPERIMENTO 1: ARCHITETTURA POLIMORFICA ADATTIVA (Euristica Predittiva)
        // ========================================================================
        auto t1_start = std::chrono::high_resolution_clock::now();


        logds::yaucl::gamma_context::ViewFactory<std::string, std::string> factory;

        // Creazione della vista radice (Senza interfacce o variant)
        std::unique_ptr<logds::yaucl::gamma_context::HierarchicalListView<std::string, std::string>> Gamma_global =
            factory.create_root_view();

        for (std::size_t i = 0; i < dimensione_gamma; ++i) {
            Gamma_global->emplace(precomputed_keys[i], "Type_Int");
        }

        for (std::size_t b = 0; b < RAMI_BACKTRACKING; ++b) {
            // La vista figlia nasce e decide autonomamente nel costruttore se fare detach immediato
            std::unique_ptr<logds::yaucl::gamma_context::HierarchicalListView<std::string, std::string>> Gamma_child =
                factory.create_child_view(*Gamma_global);

            // Esecuzione lookup diretti nativi (Nessun salto condizionale da Variant o VTable)
            for (std::size_t l = 0; l < LOOKUP_PER_RAMO; ++l) {
                std::size_t target = (l * 13) % dimensione_gamma;
                Gamma_child->find_value(precomputed_keys[target]);
            }

            for (std::size_t s = 0; s < SHADOWING_PER_RAMO; ++s) {
                Gamma_child->update_value(precomputed_keys[s], "Type_Overloaded_Float");
            }
        }
        auto t1_end = std::chrono::high_resolution_clock::now();
        auto durata_ibrida = std::chrono::duration_cast<std::chrono::milliseconds>(t1_end - t1_start).count();

        // Calcolo del fattore di accelerazione (es. 4.5x più veloce)
        double speedup = static_cast<double>(durata_flat) / (durata_ibrida > 0 ? durata_ibrida : 1);

        // Stampa la riga di dati CSV
        std::cout << dimensione_gamma << ","
                  << durata_ibrida << ","
                  << durata_flat << ","
                  << std::fixed << std::setprecision(2) << speedup << std::endl;
    }

    return 0;
}
