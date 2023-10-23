/*
 * RelationalTables.h
 * This file is part of yaucl_structures
 *
 * Copyright (C) 2023 - Giacomo Bergami
 *
 *  EquiJoin-Tables is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 *  EquiJoin-Tables is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with  EquiJoin-Tables. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef AIRLINE_RELATIONALTABLES_H
#define AIRLINE_RELATIONALTABLES_H

//
// Created by giacomo on 06/01/23.
//

#include <iostream>
#include <utility>
#include <vector>

/**
 * Abstract representation of a row for a table supporting simple equi-join operations
 *
 * @tparam S        Schema type
 * @tparam D        Data type
 */
template <typename S, typename D>
struct AbstractEquiJoinRow {
    /**
     * Number of cells associated to the table
     * @return
     */
    virtual size_t nCells() const = 0;
    /**
     * Returning the datum associated to the i-th cell
     * @param i     n° of cell
     * @return Cell reference
     */
    virtual const D& getCell(size_t i) const = 0;

    friend std::ostream &operator<<(std::ostream &os, const AbstractEquiJoinRow &row) {
        for (size_t i = 0, N = row.nCells(); i<N; i++) {
            os << row.getCell(i);
            if (i != (N-1)) os << ",";
        }
        return os;
    }
};

/**
 * Abstract representation of a table supporting efficient natural join operations
 * @tparam S    Schema type
 * @tparam D    Data type
 */
template <typename S, typename D>
struct AbstractEquiJoinTable {
    virtual ~AbstractEquiJoinTable() {}
    /**
     * Number of rows within the table
     * @return
     */
    virtual size_t size() const = 0;
    /**
     * Returning the schema associated to the table
     * @return
     */
    virtual const std::vector<S>& getSchema() const = 0;
    /**
     * Returning the row of the table
     * @param i
     * @return
     */
    virtual AbstractEquiJoinRow<S,D>* getRow(size_t i) = 0;

    /**
     * Providing a concrete/materialised row
     *
     * @param i Row id
     * @return
     */
    virtual std::vector<D> asConcreteRow(size_t i ) = 0;

    /**
     * Whether this element shall be disposed if a child of a other component
     * (i.e., avoiding deletion of the memory multiple times )
     * @return
     */
    virtual bool isDisposable() const = 0;

    /**
     * Setting the disposability
     * @param dispose disposability value
     */
    virtual void setDisposability(bool dispose) = 0;
};

#include <memory>

template <typename S, typename D>
struct ConcreteEquiJoinTable;

template <typename S, typename D>
struct ConcreteEquiJoinRow : public AbstractEquiJoinRow<S,D> {
    ConcreteEquiJoinTable<S,D>* parent;
    size_t rowId, nCell;
    ConcreteEquiJoinRow(ConcreteEquiJoinTable<S, D> *parent, size_t rowId, size_t ncells) : parent(parent), rowId(rowId), nCell(ncells) {}

    size_t nCells() const override { return nCell; }

    const D &getCell(size_t i) const override {
        return parent->data.at(rowId).at(i);
    }
};

/**
 * Representing a table by row
 * @tparam S    Schema type
 * @tparam D    Data type
 */
template <typename S, typename D>
struct ConcreteEquiJoinTable : public AbstractEquiJoinTable<S,D> {
    std::vector<S> schema;
    std::vector<std::vector<D>> data;
    ConcreteEquiJoinRow<S,D> row;

    ~ConcreteEquiJoinTable() override = default;
    ConcreteEquiJoinTable(const std::vector<S> &schema, const std::vector<std::vector<D>> &data)
    : schema(schema), data(data), row{this,0, schema.size()}, dispose{true} {}
    /**
     * Copying the abstract table into a concrete one
     * @param table:     the original table
     */
    void fromTable(AbstractEquiJoinTable<S,D>* table) {
        schema.clear();
        data.clear();
        row.nCell = 0;
        if (!table) return;
        schema = table->getSchema();
        row.ncell = schema.size();
        std::vector<D> tmp;
        for (size_t i = 0, N = table->size(); i<N; i++) {
            tmp.clear();
            tmp.reserve(row.nCell);
            auto& ROW = table->getRow(i);
            for (size_t j = 0; j<row.ncell; j++) {
                tmp.emplace_back(ROW->getCell(j));
            }
            data.emplace_back(tmp);
        }
    }
    ConcreteEquiJoinTable(AbstractEquiJoinTable<S,D>* table) {
        fromTable(table);
    }
    ConcreteEquiJoinTable& operator=(AbstractEquiJoinTable<S,D>* table) {
        fromTable(table);
        return *this;
    }

    size_t size() const override { return data.size(); }
    const std::vector<S>& getSchema() const override { return schema; }
    AbstractEquiJoinRow<S, D> *getRow(size_t i) override {
        row.rowId = i;
        return &row;
    }

    std::vector<D> asConcreteRow(size_t i) override {
        return data.at(i);
    }

    void setDisposability(bool dispose) override { this->dispose = dispose; }
    bool isDisposable() const override { return dispose; }

private:
    bool dispose;
    int concrete;
};


template<typename S, typename D>
struct CrossProductTable;

/**
 * Representing the row of a cross-product of two tables
 *
 * @tparam S    Schema type
 * @tparam D    Data type
 */
template <typename S, typename D>
struct CrossProductRow : public AbstractEquiJoinRow<S,D> {
    AbstractEquiJoinTable<S,D>* l, *r; size_t ncols, lSize, i, j;

    CrossProductRow(AbstractEquiJoinTable<S,D>* l,
                    AbstractEquiJoinTable<S,D>* r, size_t ncols, size_t lSize)
            : l(l), r(r), ncols(ncols), lSize(lSize) { }

    size_t nCells() const override { return ncols; }

    const D &getCell(size_t k) const override {
        return (k<lSize) ?
               l->getRow(i)->getCell(k) :
               r->getRow(j)->getCell(k-lSize);
    }
};

/**
 * Table computing the cross product of two tables
 *
 * @tparam S    Schema type
 * @tparam D    Data type
 */
template <typename S, typename D>
struct CrossProductTable : public AbstractEquiJoinTable<S,D> {
    size_t size_;
    std::vector<S> vv;
    CrossProductRow<S,D> row;

    ~CrossProductTable() override {
        if (row.l && (row.l->isDisposable())) delete row.l;
        if (row.r && (row.r->isDisposable())) delete row.r;
    }

    CrossProductTable(AbstractEquiJoinTable<S,D>* l,
                      AbstractEquiJoinTable<S,D>* r) : row{l,r,0,0}, disp{false} {
        size_ = l->size() * r->size();
        const auto& refL = l->getSchema();
        vv.insert(vv.end(), refL.begin(), refL.end());
        row.lSize = refL.size();
        const auto& refR = r->getSchema();
        vv.insert(vv.end(), refR.begin(), refR.end());
        row.ncols = vv.size();
    }

    size_t size() const override { return size_; }

    const std::vector<S> &getSchema() const override { return vv; }

    AbstractEquiJoinRow<S, D> *getRow(size_t k) override {
        row.i = k % row.lSize;
        row.j = k / row.lSize;
        return &row;
    }

    std::vector<D> asConcreteRow(size_t k) override {
        size_t i = k % row.lSize;
        size_t j = k / row.lSize;
        std::vector<D> v;
        v.reserve(row.ncols);
        for (size_t h = 0; h<row.ncols; h++) {
            v.emplace_back((h<row.lSize) ?
                                    row.l->getRow(i)->getCell(k) :
                                    row.r->getRow(j)->getCell(k-row.lSize));
        }
        return v;
    }

    bool isDisposable() const override { return disp; }
    void setDisposability(bool dispose) override { disp = dispose; }

private:
    bool disp;
};


template <typename S, typename D>
struct TableColumnCombinations;

/**
 * Row for a cross-product combination of multiple rows
 * @tparam S    Schema type
 * @tparam D    Data type
 */
template <typename S, typename D>
struct TableColumnCombinationRow : public AbstractEquiJoinRow<S,D> {
    TableColumnCombinations<S,D>* parent;
    std::vector<size_t> replace;
    size_t nCols;
    TableColumnCombinationRow(TableColumnCombinations<S, D> *parent) : parent(parent) {}

    size_t nCells() const override { return nCols; }

    const D &getCell(size_t i) const override {
        return parent->columnar_data.at(i).at(replace.at(i));
    }
};

/**
 * Given a set of columns, it generates a table of the cross product of all the
 * cells among each other
  * @tparam S    Schema type
 * @tparam D    Data type
 */
template <typename S, typename D>
struct TableColumnCombinations : public AbstractEquiJoinTable<S,D> {
    std::vector<S> schema;
    std::vector<std::vector<D>> columnar_data;
    std::vector<size_t> columns;
    size_t total_rows;
    TableColumnCombinationRow<S, D> row;

    ~TableColumnCombinations() override = default;
    TableColumnCombinations(const std::vector<S> &Schema,
                            const std::vector<std::vector<D>> &columnarData) : row{this}, disp{false} {
        total_rows = 1;
        for (size_t i = 0, N = std::min(Schema.size(), columnarData.size()); i<N; i++) {
            const auto& x = columnarData.at(i);
            if (!x.empty()) {
                schema.emplace_back(Schema.at(i));
                columnar_data.emplace_back(x);
                columns.emplace_back(total_rows);
                total_rows *= x.size();
            }
        }
        row.nCols = columns.size();
        std::reverse(columns.begin(), columns.end());
    }

    size_t size() const override { return total_rows; }

    const std::vector<S> &getSchema() const override { return schema; }

    AbstractEquiJoinRow<S, D> *getRow(size_t k) override {
        row.replace.clear();
        for (size_t i = 0, N = columns.size(); i<N-1; i++) {
            size_t div = k / columns.at(i);
            row.replace.emplace_back(div);
            k = k % columns.at(i);
        }
        row.replace.emplace_back(k);
        std::reverse(row.replace.begin(), row.replace.end());
        return &row;
    }

    std::vector<D> asConcreteRow(size_t k) override {
        std::vector<size_t> replace;
        std::vector<D> result;
        replace.reserve(row.nCols);
        result.reserve(row.nCols);
        for (size_t i = 0, N = columns.size(); i<N-1; i++) {
            size_t div = k / columns.at(i);
            replace.emplace_back(div);
            k = k % columns.at(i);
        }
        replace.emplace_back(k);
        std::reverse(replace.begin(), replace.end());
        for (size_t i = 0; i<row.nCols; i++) {
            result.emplace_back(columnar_data.at(i).at(replace.at(i)));
        }
        return result;
    }

    bool isDisposable() const override { return disp; }
    void setDisposability(bool dispose) override { disp = dispose; }
private:
    bool disp;
};

/**
 * Computes the union among multiple tables
 * @tparam S    Schema type
 * @tparam D    Data type
 */
template <typename S, typename D>
struct TableUnion : public AbstractEquiJoinTable<S,D> {
    std::vector<AbstractEquiJoinTable<S,D>*> unione;
    size_t nRows;
    std::vector<S> origSchema;
    std::vector<size_t> elements;

    ~TableUnion() override {
        for (const auto ptr: unione)
            if (ptr && (ptr->isDisposable()))
                delete ptr;
    }
    TableUnion(const std::vector<AbstractEquiJoinTable<S,D>*> &union_) : disp{false} {
        nRows = 0; bool firstInsertion = true;
        for (const auto& x : union_) {
            if (x->size() > 0) {
                if (firstInsertion) {
                    origSchema = x->getSchema();
                    unione.emplace_back(x);
                    nRows += x->size();
                    elements.emplace_back(nRows);
                    firstInsertion = false;
                } else {
                    if (origSchema == x->getSchema()) {
                        unione.emplace_back(x);
                        nRows += x->size();
                        elements.emplace_back(nRows);
                    }
                }
            }
        }
    }

    size_t size() const override { return nRows; }
    const std::vector<S> &getSchema() const override { return origSchema; }
    AbstractEquiJoinRow<S, D> *getRow(size_t i) override {
        for (size_t k = 0, N = elements.size(); k<N; k++) {
            if (i<elements.at(k)) {
                return unione[k]->getRow(i-(k == 0 ? 0 : elements.at(k-1)));
            }
        }
        return nullptr;
    }

    std::vector<D> asConcreteRow(size_t i) override {
        for (size_t k = 0, N = elements.size(); k<N; k++) {
            if (i<elements.at(k)) {
                return unione[k]->asConcreteRow(i-(k == 0 ? 0 : elements.at(k-1)));
            }
        }
        return {};
    }

    bool isDisposable() const override { return disp; }
    void setDisposability(bool dispose) override {
        disp = dispose;
    }
private:
    bool disp;
};

/**
 * Representing a row where a new cell column is added at the end
 * @tparam S    Schema type
 * @tparam D    Data type
 */
template <typename S, typename D>
struct ExtendedRowWithCell : public AbstractEquiJoinRow<S,D> {
    std::vector<D> newData;
    size_t rowId, nCols, nData;
    AbstractEquiJoinTable<S,D>* table;

    ExtendedRowWithCell(const std::vector<D>& newData,
                        size_t rowId,
                        size_t nCols,
                        AbstractEquiJoinTable<S,D>* table) :
            newData(newData), rowId(rowId), nCols(nCols), table(table) {}

    size_t nCells() const override { return nCols+nData; }

    const D &getCell(size_t i) const override {
        if (i<(nCols)) return table->getRow(rowId)->getCell(i);
        else return newData.at(i-nCols);
    }
};

/**
 * Extending the table by adding all the time the same cell with the same value
 * @tparam S    Schema type
 * @tparam D    Data type
 */
template <typename S, typename D>
struct ExtendTableWithData : public AbstractEquiJoinTable<S,D> {
    ExtendedRowWithCell<S,D> row;
    std::vector<S> schema;
    size_t nrows;

    ~ExtendTableWithData() override {
        if (row.table && (row.table->isDisposable())) delete row.table;
    }
    ExtendTableWithData(AbstractEquiJoinTable<S,D>* table,
                        const std::vector<S>& newCell,
                        const std::vector<D>& newData) :
            row{newData, 0, table->getSchema().size(), table}, disp{false} {
        const std::vector<S>& s = table->getSchema();
        schema = s;
        size_t N = std::min(newCell.size(), newData.size());
        schema.insert(schema.end(), newCell.begin(), newCell.begin()+N);
        nrows = table->size();
        row.nData = N;
    }

    size_t size() const override { return nrows; }
    const std::vector<S> &getSchema() const override { return schema; }
    AbstractEquiJoinRow<S, D> *getRow(size_t i) override {
        row.rowId = i;
        return &row;
    }

    std::vector<D> asConcreteRow(size_t k) override {
        std::vector<D> result = row.table->asConcreteRow(k);
        result.insert(result.end(), row.newData.begin(), row.newData.end());
        return result;
    }

    bool isDisposable() const override { return disp; }
    void setDisposability(bool dispose) override {
        disp = dispose;
    }
private:
    bool disp;
};

#include <unordered_map>

template <typename S, typename D>
struct ProjectAndSelectTable;

/**
 * Rows associated to a table where rows are selected, and column projected
 *
 * @tparam S    Schema type
 * @tparam D    Data type
 */
template <typename S, typename D>
struct ProjectAndSelectRows : public AbstractEquiJoinRow<S,D> {
    std::vector<size_t>      schemaFieldIdxs;
    size_t row_id, nCell;
    AbstractEquiJoinTable<S,D>* table;
    ProjectAndSelectRows(AbstractEquiJoinTable<S,D>* table) : table(table), row_id{0} {}

    size_t nCells() const override { return nCell; }
    const D &getCell(size_t i) const override {
        return table->getRow(row_id)->getCell(schemaFieldIdxs.at(i));
    }
};

/**
 * Table where the columns are projected, and rows selected
 *
 * @tparam S
 * @tparam D
 */
template <typename S, typename D>
struct ProjectAndSelectTable : public AbstractEquiJoinTable<S,D> {
    std::vector<size_t> rowIds;
    std::vector<S>      schemaFieldNames;
    size_t nrows;
    ProjectAndSelectRows<S,D> row;

    ~ProjectAndSelectTable() override {
        if ((row.table) && row.table->isDisposable())
            delete row.table;
    }
    ProjectAndSelectTable(AbstractEquiJoinTable<S,D>* table,
                          const std::vector<size_t>& row_ids,
                          const std::vector<S>& schema_field_names) : row{table}, disp{false} {
        size_t origNRows = table->size();
        for (size_t idx : row_ids)
            if (idx < origNRows)
                rowIds.emplace_back(idx);
        nrows = rowIds.size();
        const std::vector<S>& orig_schema = table->getSchema();
        std::unordered_map<S, size_t> map;
        for (const S& key : schema_field_names) {
            auto it = map.find(key);
            size_t offset = 0;
            if (it != map.end()) {
                offset = it->second+1;
            }
            ptrdiff_t pos = std::distance(orig_schema.begin(),
                                          std::find(orig_schema.begin()+offset, orig_schema.end(), key));
            if (pos >= 0) {
                schemaFieldNames.emplace_back(key);
                map[key] = pos;
                row.schemaFieldIdxs.emplace_back(pos);
            }
        }
        row.nCell = row.schemaFieldIdxs.size();
    }

    size_t size() const override { return nrows; }
    const std::vector<S> &getSchema() const override { return schemaFieldNames; }
    AbstractEquiJoinRow<S, D> *getRow(size_t i) override {
        row.row_id = i;
        return &row;
    }

    std::vector<D> asConcreteRow(size_t k) override {
        std::vector<D> result;
        result.reserve(row.nCell);
        for (size_t i = 0; i<row.nCell; i++) {
            result.emplace_back(row.table->getRow(k)->getCell(row.schemaFieldIdxs.at(i)));
        }
        return result;
    }

    bool isDisposable() const override { return disp; }
    void setDisposability(bool dispose) override { disp = dispose; }
private:
    bool disp;
};

template <typename S, typename D>
struct MaterialisedView : public ConcreteEquiJoinTable<S,D> {
    MaterialisedView(AbstractEquiJoinTable<S,D>* table)
    : ConcreteEquiJoinTable<S,D>( std::vector<S>{}, std::vector<std::vector<D>>{}),
      orig{table}, beingCompiled{false}, nrows{orig->size()},  disp{false}  {
        ConcreteEquiJoinTable<S,D>::schema = orig->getSchema();
        ConcreteEquiJoinTable<S,D>::row.parent = this;
        ConcreteEquiJoinTable<S,D>::row.rowId = 0;
        ConcreteEquiJoinTable<S,D>::row.nCell = ConcreteEquiJoinTable<S,D>::schema.size();
    }

    ~MaterialisedView() override {
       if (orig && (orig->isDisposable())) delete orig;
    }

    size_t size() const override {
        return nrows;
    }

    const std::vector<S> &getSchema() const override {
        return ConcreteEquiJoinTable<S,D>::schema;
    }

    AbstractEquiJoinRow<S, D> *getRow(size_t i) override {
        compileConcrete();
        return ConcreteEquiJoinTable<S,D>::getRow(i);
    }

    std::vector<D> asConcreteRow(size_t i) override {
        compileConcrete();
        return ConcreteEquiJoinTable<S,D>::data.at(i);
    }

    bool isDisposable() const override { return disp; }
    void setDisposability(bool dispose) override { disp = dispose; }

private:
    void compileConcrete() {
        if (!beingCompiled) {
            for (size_t i = 0; i<nrows; i++) {
                ConcreteEquiJoinTable<S,D>::data.emplace_back(orig->asConcreteRow(i));
            }
            if (orig->isDisposable()) {
                delete orig;
                orig = nullptr;
            }
            beingCompiled = true;
        }
    }
    AbstractEquiJoinTable<S,D> *orig;
    bool beingCompiled = false;
    size_t nrows;
    bool disp;
};

#include <cassert>
#include <map>

template <typename S>
struct natural_join_hash_for_vector {
    size_t operator()(const std::vector<S>& v) const {
        std::hash<S> h;
        size_t u = 17;
        for (S x : v) u = h(x)*13+u;
        return u;
    }
};

/**
 * Computes the natural join between two tables
 * @tparam S        Schema type
 * @tparam D        Data type
 * @param left  Left table
 * @param right Right table
 * @return      Table with natural join
 */
template <typename S, typename D>
AbstractEquiJoinTable<S,D>*
natural_join(AbstractEquiJoinTable<S,D>* left,
             AbstractEquiJoinTable<S,D>* right) {
    if (left->size() > right->size()) return natural_join(right, left);
    std::vector<S> toJoin;
    std::vector<AbstractEquiJoinTable<S,D>*> W;
    {
        std::vector<S> Lschema = left->getSchema();
        std::vector<S> Rschema = right->getSchema();
        std::sort(Lschema.begin(), Lschema.end());
        std::sort(Rschema.begin(), Rschema.end());
        std::set_intersection(Lschema.begin(), Lschema.end(), Rschema.begin(), Rschema.end(), std::back_inserter(toJoin));
    }
    if (toJoin.empty()) {
        return new CrossProductTable<S,D>(left, right);
    } else {
        std::vector<size_t> idxL, idxR;
        std::vector<S> projL, projR;
        const auto& Lschema = left->getSchema();
        const auto& Rschema = right->getSchema();
        std::unordered_map<std::vector<D>, std::vector<size_t>, natural_join_hash_for_vector<D>> mapL;
        std::map<std::vector<D>, std::vector<size_t>> mapR;
        for (const S& x : toJoin) {
            ptrdiff_t pos = std::distance(Lschema.begin(),
                                          std::find(Lschema.begin(), Lschema.end(), x));
            if (pos >= 0) idxL.emplace_back(pos);
            pos = std::distance(Rschema.begin(),
                                std::find(Rschema.begin(), Rschema.end(), x));
            if (pos >= 0) idxR.emplace_back(pos);
        }
        std::sort(idxL.begin(), idxL.end());
        std::sort(idxR.begin(), idxR.end());
        for (size_t i = 0, N = Lschema.size(); i<N; i++) {
            if (!std::binary_search(idxL.begin(), idxL.end(), i))
                projL.emplace_back(Lschema.at(i));
//            if (!std::binary_search(idxR.begin(), idxR.end(), i))
//                projR.emplace_back(Lschema.at(i));
        }
        for (size_t i = 0, N = Rschema.size(); i<N; i++) {
//            if (!std::binary_search(idxL.begin(), idxL.end(), i))
//                projL.emplace_back(Lschema.at(i));
            if (!std::binary_search(idxR.begin(), idxR.end(), i))
                projR.emplace_back(Rschema.at(i));
        }
        assert(idxL.size() == idxR.size());
        std::vector<D> u;
        for (size_t i = 0, N = left->size(); i<N; i++) {
            u.clear();
            auto ptrRow = left->getRow(i);
            for (size_t idx : idxL)
                u.emplace_back(ptrRow->getCell(idx));
            mapL[u].emplace_back(i);
        }
        for (size_t i = 0, N = right->size(); i<N; i++) {
            u.clear();
            auto ptrRow = right->getRow(i);
            for (size_t idx : idxR)
                u.emplace_back(ptrRow->getCell(idx));
            auto it = mapL.find(u);
            if (it != mapL.end()) {
                mapR[u].emplace_back(i);
            }
        }
        for (const auto& [key, rowsIdsR] : mapR) {
            auto it = mapL.find(key);
            assert(it != mapL.end());
            const auto& rowsIdsL = it->second;

            auto projsL = new ProjectAndSelectTable<S, D>(left, rowsIdsL, projL);
            auto projsR = new ProjectAndSelectTable<S, D>(right, rowsIdsR, projR);
            auto cross = new CrossProductTable<S,D>(projsL, projsR);
            auto extension = new ExtendTableWithData<S, D>(cross, toJoin, key);
            W.emplace_back(extension);
        }
        return new TableUnion<S, D>(W);
    }
}

#endif //AIRLINE_RELATIONALTABLES_H
