//
// Created by giacomo on 30/04/2022.
//

#ifndef KNOBAB_SERVER_FIXEDSIZENDPSORTER_H
#define KNOBAB_SERVER_FIXEDSIZENDPSORTER_H

#include <yaucl/data/MemoryMappingFile.h>
#include <filesystem>
#include <algorithm>
#include <queue>

template <typename T> class FixedSizeNDPSorter {

    size_t runs_size;
    int partition(std::vector<T> &arr, const int left, const int right) {
        T& pivotElement = arr[right];
        uint_fast64_t i = left - 1;
        for (uint_fast64_t j = left; j < right; j++) {
            if ((arr[j] < pivotElement)) {
                i++;
                std::swap(arr[i],arr[j]);
            }
        }
        if ((arr[right] < arr[i+1])) {
            std::swap(arr[(i+1)],arr[right]);
        }
        return i+1;
    }

    void do_quicksort(std::vector<T> &arr, const int left, const int right) {
        if (left < right) {
            uint_fast64_t part = partition(arr, left, right);
            if (part > 0) do_quicksort(arr, left, part - 1);
            do_quicksort(arr, part + 1, right);
        }
    }

    void do_quicksort(std::vector<T>& arr) {
        do_quicksort(arr, 0, arr.size()-1);
    }

public:

    FixedSizeNDPSorter(size_t size_runs) : runs_size{size_runs} {}

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

        for (size_t i = 0; i<num_ways; i++) {
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
        size_t i;
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
        std::filesystem::remove(filename);
        std::filesystem::rename(out, filename);
    }
};


#endif //KNOBAB_SERVER_FIXEDSIZENDPSORTER_H
