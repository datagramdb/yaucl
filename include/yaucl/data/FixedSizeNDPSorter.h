//
// Created by giacomo on 30/04/2022.
//

#ifndef KNOBAB_SERVER_FIXEDSIZENDPSORTER_H
#define KNOBAB_SERVER_FIXEDSIZENDPSORTER_H

#include <yaucl/data/MemoryMappingFile.h>
#include <functional>
#include <filesystem>
#include <algorithm>
#include <queue>
#include <cmath>
#include "new_iovec.h"
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

template <typename T, typename Comparator = std::less<T>> class FixedSizeNDPSorter {
    size_t runs_size;
    Comparator Tcomp;

    int partition(std::vector<T> &array, const int low, const int high) {
        // select the rightmost element as pivot
        T& pivot = array[high];

        // pointer for greater element
        int i = (low - 1);

        // traverse each element of the array
        // compare them with the pivot
        for (int j = low; j < high; j++) {
            if (!Tcomp(pivot, array[j])) { // array[j] <= pivot

                // if element smaller than pivot is found
                // swap it with the greater element pointed by i
                i++;

                // swap element at i with element at j
                std::swap(array[i], array[j]);
            }
        }

        // swap pivot with the greater element at i
        std::swap(array[i + 1], array[high]);

        // return the partition point
        return (i + 1);
    }

    void do_quicksort(std::vector<T> &array, const int low, const int high) {
        if (low < high) {

            // find the pivot element such that
            // elements smaller than pivot are on left of pivot
            // elements greater than pivot are on righ of pivot
            int pi = partition(array, low, high);

            // recursive call on the left of pivot
            do_quicksort(array, low, pi - 1);

            // recursive call on the right of pivot
            do_quicksort(array, pi + 1, high);
        }
    }

    void do_quicksort(std::vector<T>& arr) {
        do_quicksort(arr, 0, arr.size()-1);
    }

public:

    FixedSizeNDPSorter(size_t size_runs) : runs_size{size_runs} {}
    FixedSizeNDPSorter(size_t size_runs, Comparator less) : runs_size{size_runs}, Tcomp{less} {}

    /**
     * Sorts the file, while removing the similar data up to V1 and clustering it, where V2 just
     * remarks which are the differences.
     *
     * @tparam V1
     * @tparam V2
     * @param filename
     * @param tmp_path
     * @param projectorEq
     * @param projectorRest
     */
    template <typename V1, typename V2>
    std::filesystem::path sort_with_clusteredIndex(const std::filesystem::path& filename,
                                  const std::filesystem::path& tmp_path,
                                  std::function<V1(T)> projectorEq,
                                  std::function<V2(T)> projectorRest) {
        constexpr size_t Tsize = sizeof(T);
        yaucl::data::MemoryMappedFile file{filename};
        size_t full = file.file_size();
        size_t min_runs_size = std::min(file.file_size(), runs_size);
        size_t num_ways = (size_t)std::ceil((double)file.file_size() / (double)min_runs_size);
        size_t N = full / Tsize;
        size_t Nblock = min_runs_size / Tsize;

        std::vector<yaucl::data::MemoryMappedFile> externalFiles(num_ways);
        std::vector<size_t> externalFileCounter;
        std::vector<T> in_memory_file_copy;

        size_t i = 0;
        for (i = 0; i<num_ways; i++) {
            size_t M = (((i+1)*Nblock) > N) ? (N)-i*Nblock : Nblock;
            for (size_t j = 0; j<M; j++) {
                auto& ref = file.at<T>(i*Nblock+j);
                if (j >= in_memory_file_copy.size())
                    in_memory_file_copy.emplace_back(ref);
                else
                    in_memory_file_copy[j] = ref;
            }
            in_memory_file_copy.resize(M);
            do_quicksort(in_memory_file_copy);
            auto tmpFile = tmp_path / (std::to_string(i)+".tmp");
            {
                std::fstream toSort = std::fstream(tmpFile, std::ios::out | std::ios::binary);
                for (size_t pos = 0, O = in_memory_file_copy.size(); pos<O; pos++) {
                    toSort.write((char*)&in_memory_file_copy[pos], Tsize);
                }
            }
            externalFiles[i].open(tmpFile);
            externalFileCounter.emplace_back(0);
        }

        std::priority_queue<std::pair<T, size_t>,
                std::vector<std::pair<T, size_t>>,
                std::greater<std::pair<T, size_t>>> minheap;
        auto k = externalFiles.size();
        for (i = 0; i < k; i++){
            auto count = externalFileCounter.at(i);
            auto& val = externalFiles.at(i).at<T>(count);
            minheap.emplace(val, i);
        }

        auto out = tmp_path / ("result.tmp");
        auto clIndex = filename.string() + ("_clIndex.bin");
        yaucl::data::VariadicSizeArrayElementsReaderWriter varWriter{clIndex};
        std::fstream tmpSort = std::fstream(out, std::ios::out | std::ios::binary);
        new_iovec data;

        // Now one by one get the minimum element from min
        // heap and replace it with next element.
        // run till all filled input files reach EOF
        bool first = true;
        V1 prev;
        std::vector<V2> current;
        while (!minheap.empty()) {
            // Get the minimum element and store it in output file
            auto& ref = minheap.top();
            if (first) {
                prev = projectorEq(ref.first);
                current.emplace_back(projectorRest(ref.first));
                first = false;
            } else {
                auto currEq = projectorEq(ref.first);
                if (currEq == prev) {
                    // If it is equivalent to the previous, then add the projected value to the overflow vector
                    current.emplace_back(projectorRest(ref.first));
                } else {
                    // Otherwise, flush to write the current vector, ...
                    tmpSort.write((char*)&prev, sizeof(V1));
                    data.iov_base = (void*)current.data();
                    data.iov_len = current.size() * sizeof(V2);
                    varWriter.put(data);
                    varWriter.flush();
                    current.clear();
                    // ... and put the novel information.
                    current.emplace_back(projectorRest(ref.first));
                    prev = currEq;
                }
            }
            size_t currFileId = ref.second;
            auto& currFile = externalFiles.at(currFileId);
            size_t currFileOffset = (++externalFileCounter[ref.second]);
            minheap.pop();

            // Find the next element that will replace current
            // root of heap. The next element belongs to same
            // input file as the current min element.
            if (currFileOffset != currFile.cast_size<T>()) {
                minheap.emplace(currFile.at<T>(currFileOffset), currFileId);
            } else {
                currFile.close();
            }
        }
        // Finalising the cluster
        tmpSort.write((char*)&prev, sizeof(V1));
        data.iov_base = (void*)current.data();
        data.iov_len = current.size() * sizeof(V2);
        varWriter.put(data);
        varWriter.close();
        current.clear();

        tmpSort.close();
        file.close();
        std::filesystem::remove(filename);
        std::filesystem::rename(out, filename);

        return clIndex;
    }

    void sort(const std::filesystem::path& filename, const std::filesystem::path& tmp_path) {
        constexpr size_t Tsize = sizeof(T);
        yaucl::data::MemoryMappedFile file{filename};
        size_t full = file.file_size();
        size_t min_runs_size = std::min(file.file_size(), runs_size);
        size_t num_ways = (size_t)std::ceil((double)file.file_size() / (double)min_runs_size);
        size_t N = full / Tsize;
        size_t Nblock = min_runs_size / Tsize;

        std::vector<yaucl::data::MemoryMappedFile> externalFiles(num_ways);
        std::vector<size_t> externalFileCounter;
        std::vector<T> in_memory_file_copy;

        size_t i = 0;
        for (i = 0; i<num_ways; i++) {
            size_t M = (((i+1)*Nblock) > N) ? (N)-i*Nblock : Nblock;
            for (size_t j = 0; j<M; j++) {
                auto& ref = file.at<T>(i*Nblock+j);
                if (j >= in_memory_file_copy.size())
                    in_memory_file_copy.emplace_back(ref);
                else
                    in_memory_file_copy[j] = ref;
            }
            in_memory_file_copy.resize(M);
            do_quicksort(in_memory_file_copy);
            auto tmpFile = tmp_path / (std::to_string(i)+".tmp");
            {
                std::fstream toSort = std::fstream(tmpFile, std::ios::out | std::ios::binary);
                for (size_t pos = 0, O = in_memory_file_copy.size(); pos<O; pos++) {
                    toSort.write((char*)&in_memory_file_copy[pos], Tsize);
                }
            }
            externalFiles[i].open(tmpFile);
            externalFileCounter.emplace_back(0);
        }

        std::priority_queue<std::pair<T, size_t>,
                            std::vector<std::pair<T, size_t>>,
                            std::greater<std::pair<T, size_t>>> minheap;
        auto k = externalFiles.size();
        for (i = 0; i < k; i++){
            auto count = externalFileCounter.at(i);
            auto& val = externalFiles.at(i).at<T>(count);
            minheap.emplace(val, i);
        }

        auto out = tmp_path / ("result.tmp");
        std::fstream tmpSort = std::fstream(out, std::ios::out | std::ios::binary);

        // Now one by one get the minimum element from min
        // heap and replace it with next element.
        // run till all filled input files reach EOF
        while (!minheap.empty()) {
            // Get the minimum element and store it in output file
            auto& ref = minheap.top();
            size_t currFile = ref.second;
            auto& currF = externalFiles.at(currFile);
            tmpSort.write((char*)&ref.first, Tsize);
            size_t currFileOffset = (++externalFileCounter[ref.second]);
            minheap.pop();

            // Find the next element that will replace current
            // root of heap. The next element belongs to same
            // input file as the current min element.
            if (currFileOffset != currF.cast_size<T>()) {
                minheap.emplace(currF.at<T>(currFileOffset), currFile);
            } else {
                currF.close();
            }
        }

        tmpSort.close();
        file.close();
        std::filesystem::remove(filename);
        std::filesystem::rename(out, filename);
    }
};

template <typename T> class FixedSizeReaderWriter {
    std::fstream file;
    std::filesystem::path filename;
    bool isWrite, isRead;
    yaucl::data::FixedSizeArrayElements<T> reader;
    std::filesystem::path tmp_path;

    void prepareWrite() {
        if (isRead) {
            reader.close();
            isRead = false;
        }
        if (!isWrite) {
            file = std::fstream(filename, std::ios::out | std::ios::binary);
            isWrite = true;
        }
    }
    void prepareRead() {
        if (isWrite) {
            file.close();
            isWrite = false;
        }
        if (!isRead) {
            reader.open(filename);
            isRead = true;
        }
    }


public:
    FixedSizeReaderWriter(const std::filesystem::path& file,
                          const std::filesystem::path& tmp_path) : tmp_path{tmp_path}, filename{file}, isWrite{false}, isRead{false}{}
    FixedSizeReaderWriter() : isWrite{false}, isRead{false}{}
    FixedSizeReaderWriter(const FixedSizeReaderWriter&x) {
        if ((!x.isRead) && (!x.isWrite)) {
            isWrite = isRead = false;
            tmp_path = x.tmp_path;
            filename = x.filename;
        } else throw std::runtime_error("ERROR: cannot copy a FixedSizeReaderWriter when opened!");
    }
    FixedSizeReaderWriter(FixedSizeReaderWriter&&) = default;
    FixedSizeReaderWriter& operator=(const FixedSizeReaderWriter&x ) {
        if ((!x.isRead) && (!x.isWrite)) {
            isWrite = isRead = false;
            tmp_path = x.tmp_path;
            filename = x.filename;
        } else throw std::runtime_error("ERROR: cannot copy a FixedSizeReaderWriter when opened!");
    }
    FixedSizeReaderWriter& operator=(FixedSizeReaderWriter&&) = default;
    ~FixedSizeReaderWriter() { close(); }

    const T* begin() {
        prepareRead();
        return reader.begin();
    }
    const T* end() {
        prepareRead();
        return reader.end();
    }
    void open(const std::filesystem::path& f,
              const std::filesystem::path& t)  {
        filename = f;
        tmp_path = t;
        isWrite = false;
        isRead = false;
    }
    void put(const T& data) {
        prepareWrite();
        file.write((char*)&data, sizeof(T));
    }
    void put(const new_iovec& data) {
        prepareWrite();
        file.write((char*)data.iov_base, data.iov_len);
    }
    size_t size() {
        prepareRead();
        return reader.size();
    }
    T& get(size_t i) {
        prepareRead();
        return reader[i];
    }
    void removeById(const std::unordered_set<size_t>& ids) {
        size_t N = size(); // prepareRead
        auto tmpFile = tmp_path / filename.filename();
        {
            auto cpy = std::fstream(tmpFile, std::ios::out | std::ios::binary);
            for (size_t i = 0; i<N; i++) {
                if (!ids.contains(i)) {
                    cpy.write((char*)&reader[i], sizeof(T));
                }
            }
        }
        std::filesystem::remove(filename);
        std::filesystem::rename(tmpFile, filename);
        close();
    }
    void removeByValue(const std::unordered_set<T>& values) {
        size_t N = size(); // prepareRead
        auto tmpFile = tmp_path / filename.filename();
        {
            auto cpy = std::fstream(tmpFile, std::ios::out | std::ios::binary);
            for (size_t i = 0; i<N; i++) {
                T& ref = reader[i];
                if (!values.contains(ref)) {
                    cpy.write((char*)&ref, sizeof(T));
                }
            }
        }
        std::filesystem::remove(filename);
        std::filesystem::rename(tmpFile, filename);
        close();
    }
    void sort(size_t size_runs) {
        close();
        FixedSizeNDPSorter<T> sorter(size_runs);
        sorter.sort(filename, tmp_path);
    }
    template <typename Comparator> void sort(size_t size_runs, Comparator less) {
        close();
        FixedSizeNDPSorter<T, Comparator> sorter(size_runs, less);
        sorter.sort(filename, tmp_path);
    }
    template <typename Comparator> void sort(size_t size_runs) {
        close();
        FixedSizeNDPSorter<T, Comparator> sorter(size_runs);
        sorter.sort(filename, tmp_path);
    }

    /**
     *
     * @tparam V1               This should be a fixed-size record, too!
     * @tparam V2               This should be a fixed-size record, too!
     * @param size_runs
     * @param projectorEq
     * @param projectorRest
     */
    template <typename V1, typename V2> std::pair<FixedSizeReaderWriter<V1>,
            yaucl::data::VariadicSizeArrayElementsReader>  sortAndGroupViaClusteredIndex(size_t size_runs, std::function<V1(T)> projectorEq,
                                                                                                std::function<V2(T)> projectorRest) {
        close();
        FixedSizeNDPSorter<T> sorter(size_runs);
        auto pathIndex = sorter.sort_with_clusteredIndex(filename, tmp_path, projectorEq, projectorRest);
        return {{filename, tmp_path}, {pathIndex}};
    }

    /**
 *
 * @tparam V1               This should be a fixed-size record, too!
 * @tparam V2               This should be a fixed-size record, too!
 * @param size_runs
 * @param projectorEq
 * @param projectorRest
 */
    template <typename Comparator, typename V1, typename V2> std::pair<FixedSizeReaderWriter<V1>,
                                                                        yaucl::data::VariadicSizeArrayElementsReader>
                                                             sortAndGroupViaClusteredIndex(size_t size_runs, Comparator less, std::function<V1(T)> projectorEq,
                                                                                                std::function<V2(T)> projectorRest) {
        close();
        FixedSizeNDPSorter<T, Comparator> sorter(size_runs, less);
        auto pathIndex = sorter.sort_with_clusteredIndex(filename, tmp_path, projectorEq, projectorRest);
        return {{filename, tmp_path}, {pathIndex}};
    }

    /**
 *
 * @tparam V1               This should be a fixed-size record, too!
 * @tparam V2               This should be a fixed-size record, too!
 * @param size_runs
 * @param projectorEq
 * @param projectorRest
 */
    template <typename Comparator, typename V1, typename V2> std::pair<FixedSizeReaderWriter<V1>,
            yaucl::data::VariadicSizeArrayElementsReader>  sortAndGroupViaClusteredIndex(size_t size_runs, std::function<V1(T)> projectorEq,
                                                                                                std::function<V2(T)> projectorRest) {
        close();
        FixedSizeNDPSorter<T, Comparator> sorter(size_runs);
        auto pathIndex = sorter.sort_with_clusteredIndex(filename, tmp_path, projectorEq, projectorRest);
        return {{filename, tmp_path}, {pathIndex}};
    }


    void close() {
        if (isWrite) {
            file.close();
            isWrite = false;
        }
        if (isRead) {
            reader.close();
            isRead = false;
        }
    }
};


#endif //KNOBAB_SERVER_FIXEDSIZENDPSORTER_H
