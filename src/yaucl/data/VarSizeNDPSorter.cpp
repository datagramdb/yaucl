//
// Created by giacomo on 05/06/2022.
//

#include <yaucl/data/VarSizeNDPSorter.h>
#include <cmath>
#include <fstream>
#include <queue>

VarSizeNDPSorter::VarSizeNDPSorter(size_t size_runs, const std::function<bool(const new_iovec&, const new_iovec&, char*)>& p) : runs_size{size_runs}, pred{p} {}

void VarSizeNDPSorter::sort(const std::filesystem::path &f, const std::filesystem::path &tmp_path) {
    constexpr size_t Tsize = sizeof(size_t)*2;
    yaucl::data::MemoryMappedFile index_file{f.string()+"_idx"};
    yaucl::data::MemoryMappedFile file{f};
    char* data = file.data();

    size_t full = file.file_size();
    size_t min_runs_size = std::min(full, runs_size);
    size_t records = index_file.file_size() / Tsize;
    //size_t num_ways = (size_t)std::ceil((double)full / (double)min_runs_size);
    //size_t N = full / Tsize;
    //size_t Nblock = min_runs_size / Tsize;
    size_t idx = 0;

    std::vector<size_t> externalFileCounter;
    std::vector<new_iovec> in_memory_file_copy;
    size_t i = 0;

    while (idx < records) {
        size_t currWritten = min_runs_size;
        size_t j = 0;

        while ((currWritten > 0) && (idx < records)) {
            const auto& ref = index_file.at<idx_record>(idx);
            if ((currWritten>=ref.len)) {
                if (j >= in_memory_file_copy.size())
                    in_memory_file_copy.emplace_back((char*)ref.offset, ref.len);
                else
                    in_memory_file_copy[j] = {(char*)ref.offset, ref.len};
                j++;
                idx++;
                currWritten -= ref.len;
            } else {
                break;
            }
        }
        in_memory_file_copy.resize(j);
        do_quicksort(in_memory_file_copy, data);
        auto tmpFile = tmp_path / (std::to_string(i)+".tmp");
        {
            std::fstream toSort = std::fstream(tmpFile, std::ios::out | std::ios::binary);
            for (size_t pos = 0, O = in_memory_file_copy.size(); pos<O; pos++) {
//                auto valu = data+(size_t)in_memory_file_copy[pos].iov_base;
                toSort.write((char*)&in_memory_file_copy[pos].iov_base, sizeof (size_t));
                toSort.write((char*)&in_memory_file_copy[pos].iov_len, sizeof (size_t));
            }
        }
        externalFileCounter.emplace_back(0);
        i++;
    }
    std::vector<yaucl::data::MemoryMappedFile> externalFiles(i);
    for (size_t j = 0; j<i; j++) {
        auto tmpFile = tmp_path / (std::to_string(j)+".tmp");
        externalFiles[j].open(tmpFile);
    }

    std::priority_queue<std::pair<new_iovec, size_t>,
            std::vector<std::pair<new_iovec, size_t>>,
            std::function<bool(const std::pair<new_iovec, size_t>&,
                               const std::pair<new_iovec, size_t>&)>> minheap([&](const std::pair<new_iovec, size_t>& a,
                                                                                  const std::pair<new_iovec, size_t>& b) {
        return (pred(b.first, a.first, data)) || ((!pred(a.first, b.first, data)) && (a.second > b.second));
    });

    auto k = externalFiles.size();
    std::pair<new_iovec, size_t> cp{{nullptr,0}, 0};
    for (i = 0; i < k; i++){
        auto& val = externalFiles.at(i).at<idx_record>(0);
//        auto valu = data+val.offset;
        cp.first.iov_base = (void*)val.offset;
        cp.first.iov_len = val.len;
        cp.second = i;
        minheap.emplace(cp);
    }

    auto out = tmp_path / ("result.tmp");
    auto outIdx = tmp_path / ("result.idx");
    std::fstream tmpSort = std::fstream(out, std::ios::out | std::ios::binary);
    std::fstream idxSort = std::fstream(outIdx, std::ios::out | std::ios::binary);
    size_t offset = 0;

    // Now one by one get the minimum element from min
    // heap and replace it with next element.
    // run till all filled input files reach EOF
    while (!minheap.empty()) {
        // Get the minimum element and store it in output file
        const std::pair<new_iovec, size_t>& ref = minheap.top();
        size_t currFile = ref.second;
        auto& currF = externalFiles.at(currFile);
        tmpSort.write((char*)(data+(size_t)ref.first.iov_base), ref.first.iov_len);
        idxSort.write((char*)&offset, sizeof(size_t));
        idxSort.write((char*)&ref.first.iov_len, sizeof(size_t));
        offset+=ref.first.iov_len;
        size_t currFileOffset = (++externalFileCounter[ref.second]);
        minheap.pop();

        // Find the next element that will replace current
        // root of heap. The next element belongs to same
        // input file as the current min element.
        if (currFileOffset != currF.cast_size<idx_record>()) {
            auto& val = currF.at<idx_record>(currFileOffset);
            cp.first.iov_base = (void*)val.offset;
            cp.first.iov_len = val.len;
            cp.second = currFile;
            minheap.emplace(cp);
        } else {
            currF.close();
        }
    }

    tmpSort.close();
    idxSort.close();
    std::filesystem::remove(f);
    std::filesystem::remove(f.string()+"_idx");
    std::filesystem::rename(out, f);
    std::filesystem::rename(outIdx, f.string()+"_idx");
}

void VarSizeNDPSorter::do_quicksort(std::vector<new_iovec> &arr, char* data) {
    do_quicksort(arr, 0, arr.size()-1, data);
}

void VarSizeNDPSorter::do_quicksort(std::vector<new_iovec> &array, const int low, const int high, char* data) {
    if (low < high) {

        // find the pivot element such that
        // elements smaller than pivot are on left of pivot
        // elements greater than pivot are on righ of pivot
        int pi = partition(array, low, high, data);

        // recursive call on the left of pivot
        do_quicksort(array, low, pi - 1, data);

        // recursive call on the right of pivot
        do_quicksort(array, pi + 1, high, data);
    }
}

int VarSizeNDPSorter::partition(std::vector<new_iovec> &array, const int low, const int high, char* data) {
    // select the rightmost element as pivot
    const new_iovec& pivot = array[high];

    // pointer for greater element
    int i = (low - 1);

    // traverse each element of the array
    // compare them with the pivot
    for (int j = low; j < high; j++) {
        if (!(pred(pivot, array[j], data))) {

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

VarSizeNDPReader::VarSizeNDPReader(const std::filesystem::path &p) : file{p}, idx{p.string()+"_idx"}{}
