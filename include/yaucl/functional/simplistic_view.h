#pragma once

#include <list>
#include <vector>

namespace yaucl {
    namespace functional {

        template <typename T, typename Data>
        struct simplistic_view_iterator;

        template <typename T, typename Data>
        struct simplistic_view {
            Data* original_data;
            std::list<size_t> indices;

            std::vector<size_t> resolveIndices() const {
                return {indices.begin(), indices.end()};
            }

            simplistic_view(Data* ptr, const std::list<size_t>& idx) : original_data{ptr}, indices{idx} {}
            simplistic_view(Data& original) {
                original_data = &original;
                for (size_t i = 0, N = original.size(); i<N; i++)
                    indices.emplace_back(i);
            }
            simplistic_view() : original_data{nullptr} {}
            simplistic_view(const simplistic_view<T, Data>& ) = default;
            simplistic_view(simplistic_view<T, Data>&& x) = default;
            simplistic_view& operator=(const simplistic_view<T, Data>& ) = default;
            simplistic_view& operator=(simplistic_view<T, Data>&& x) = default;

            simplistic_view_iterator<T, Data> begin() { auto it = indices.begin(); return simplistic_view_iterator<T, Data>{this, it}; }
            simplistic_view_iterator<T, Data> end() { auto it = indices.end();  return simplistic_view_iterator<T, Data>{this, it}; }
        };

        template <typename T, typename Data>
        struct simplistic_view_iterator {
            simplistic_view<T, Data>* original_view;
            std::list<size_t>::iterator it;
            simplistic_view_iterator(struct simplistic_view<T, Data>* ptr, std::list<size_t>::iterator iteratore) : original_view{ptr}, it{iteratore} {}
            simplistic_view_iterator() : original_view{nullptr} {}
            simplistic_view_iterator(const simplistic_view_iterator<T, Data>& x) : original_view{x.original_view}, it{x.it} {}
            simplistic_view_iterator(simplistic_view_iterator<T, Data>&& x) : original_view{x.original_view}, it{x.it}  { }
            simplistic_view_iterator& operator=(const simplistic_view_iterator<T, Data>& ) = default;
            simplistic_view_iterator<T, Data>& operator=(simplistic_view_iterator<T, Data>&& x) = default;
            const T* get() const { return (const T*)&original_view->original_data[*it]; }
            T& operator*() { return (*original_view->original_data)[*it]; }
            T& operator->() { return (*original_view->original_data)[*it]; }
            simplistic_view_iterator<T, Data>& operator++() {
                it++;
                return *this;
            }
            simplistic_view_iterator<T, Data>& operator++(int) {
                it++;
                return *this;
            }
            simplistic_view_iterator<T, Data>& operator--() {
                it--;
                return *this;
            }
            simplistic_view_iterator<T, Data>& operator--(int) {
                it--;
                return *this;
            }
            bool operator==(const simplistic_view_iterator<T, Data> &rhs) const {
                return original_view == rhs.original_view &&
                       it == rhs.it;
            }
            bool operator!=(const simplistic_view_iterator<T, Data> &rhs) const {
                return !(rhs == *this);
            }
        };

        /**
         * Erasing the current element from the view
         * @tparam T        Datatype
         * @param data      View container
         * @param it        Iterator to the current point of the structure
         * @return          An updated view (data) where the element pointed by it was removed
         */
        template <typename T, typename Data> simplistic_view_iterator<T, Data>& erase(struct simplistic_view<T, Data>& data, simplistic_view_iterator<T, Data>& it) {
            it.it = data.indices.erase(it.it);
            return it;
        }

        /**
         * Inserting an element in a given position of the data view by extending the underlying representation
         * @tparam T                Datatype
         * @param data              Dataview to be updated
         * @param obj               Object to be inserted
         * @param at                Position in the view where to insert the object
         * @param check_presence    If set to false (default), the value is always inserted at the end of it. Otherwise, it is inserted only if not already present
         * @return An pointer to the location where the element was added within the view
         */
        template <typename T, typename Data> simplistic_view_iterator<T, Data> insert(struct simplistic_view<T, Data>& data, const T& obj, simplistic_view_iterator<T, Data>& at, bool check_presence = false) {
            if (check_presence) {
                for (auto it = data.begin(), en = data.end(); it != en; it++) {
                    if (*it == obj) return it;
                }
            }
            data.original_data->emplace_back(obj);
            size_t N = data.original_data->size()-1;
            return simplistic_view_iterator<T, Data>{&data,
                                               (std::list<size_t>::iterator)data.indices.insert(at.it, N)};
        }

        /**
         * Inserting an element at the end of the data view by extending the underlying representation
         * @tparam T                Datatype
         * @param data              Dataview to be updated
         * @param obj               Object to be inserted
         * @param check_presence    If set to false (default), the value is always inserted at the end of it. Otherwise, it is inserted only if not already present
         * @return An pointer to the location where the element was added within the view
         */
        template <typename T, typename Data> simplistic_view_iterator<T, Data> back_insert(struct simplistic_view<T, Data>& data, const T& obj, bool check_presence = false) {
            if (check_presence) {
                for (auto it = data.begin(), en = data.end(); it != en; it++) {
                    if (*it == obj) return it;
                }
            }
            data.original_data->emplace_back(obj);
            size_t N = data.original_data->size()-1;
            return simplistic_view_iterator<T, Data>{&data,
                                               (std::list<size_t>::iterator)data.indices.insert(data.indices.end(), N)};
        }

        /**
         * Sorting the elements in the view! (not the actual underlying data)
         * @tparam T    Datatype
         * @param data  View to be sorted
         */
        template <typename T, typename Data> void sort(struct simplistic_view<T, Data>& data) {
            std::vector<size_t> tosort{data.indices.begin(), data.indices.end()};
            std::sort(tosort.begin(), tosort.end(), [data](const size_t l, const size_t r) {
                return data.original_data->at(l) < data.original_data->at(r);
            });
            size_t i = 0;
            for (auto it = data.indices.begin(), en = data.indices.end(); it != en; it++) {
                *it = tosort.at(i++);
            }
        }


    }
}