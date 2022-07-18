package utils.external.jtables;

//A larger table model that performs "paging" of its data. This model
//reports a small number of rows (like 100 or so) as a "page" of data. You
//can switch pages to view all of the rows as needed using the pageDown()
//and pageUp() methods. Presumably, access to the other pages of data is
//dictated by other GUI elements such as up/down buttons, or maybe a text
//field that allows you to enter the page number you want to display.
//

import it.giacomobergami.knobab.records.SplitterUtil;

import javax.swing.*;
import javax.swing.table.AbstractTableModel;
import javax.swing.table.TableModel;
import java.awt.*;
import java.util.List;

public class PagingModel<T> extends AbstractTableModel {

    protected int pageSize;
    protected int pageOffset;
    protected SplitterUtil<T> data;
    protected String[] columns;

    public PagingModel(SplitterUtil<T> ls, String[] colNames, int pageSize) {
        data = ls;
        this.pageSize = pageSize;
        columns = colNames;
        pageOffset = 0;
    }

    // Return values appropriate for the visible table part.
    public int getRowCount() {
        return Math.min(pageSize, data.size());
    }

    public int getColumnCount() {
        return columns.length;
    }

    // Work only on the visible part of the table.
    public Object getValueAt(int row, int col) {
        int realRow = row + (pageOffset * pageSize);
        return data.cell(realRow,col);
    }

    public String getColumnName(int col) {
        return columns[col];
    }

    // Use this method to figure out which page you are on.
    public int getPageOffset() {
        return pageOffset;
    }

    public int getPageCount() {
        return (int) Math.ceil((double) data.size() / pageSize);
    }

    // Use this method if you want to know how big the real table is . . . we
    // could also write "getRealValueAt()" if needed.
    public int getRealRowCount() {
        return data.size();
    }

    public int getPageSize() {
        return pageSize;
    }

    public void navigateToRow(int row, JTable jTable1) {
        int pageId = (int)Math.floor(((double) row) / pageSize);
        int physicalRow = row - pageId*pageSize;
        setPage(pageId);
        jTable1.getSelectionModel().setSelectionInterval(physicalRow, physicalRow);
//        jTable1.scrollRectToVisible(new Rectangle(jTable1.getCellRect(physicalRow, 0, true)));
//        jTable1.changeSelection(physicalRow, 0, false, false);
    }

    public void setPageSize(int s) {
        if (s == pageSize) {
            return;
        }
        int oldPageSize = pageSize;
        pageSize = s;
        pageOffset = (oldPageSize * pageOffset) / pageSize;
        fireTableDataChanged();
        /*
         * if (pageSize < oldPageSize) { fireTableRowsDeleted(pageSize,
         * oldPageSize - 1); } else { fireTableRowsInserted(oldPageSize,
         * pageSize - 1); }
         */
    }

    // Update the page offset and fire a data changed (all rows).
    public void pageDown() {
        if (pageOffset < getPageCount() - 1) {
            pageOffset++;
            fireTableDataChanged();
        }
    }

    public void setPage(int x) {
        if ((x >= 0) && (x < getPageCount() -1)) {
            pageOffset = x;
            fireTableDataChanged();
        }
    }

    // Update the page offset and fire a data changed (all rows).
    public void pageUp() {
        if (pageOffset > 0) {
            pageOffset--;
            fireTableDataChanged();
        }
    }

    // We provide our own version of a scrollpane that includes
    // the page up and page down buttons by default.
    public static JScrollPane createPagingScrollPaneForTable(JTable jt) {
        JScrollPane jsp = new JScrollPane(jt);
        TableModel tmodel = jt.getModel();

        // Don't choke if this is called on a regular table . . .
        if (!(tmodel instanceof PagingModel)) {
            return jsp;
        }

        // Okay, go ahead and build the real scrollpane
//        final PagingModel model = (PagingModel) tmodel;
//        final JButton upButton = new JButton(new ArrowIcon(ArrowIcon.NORTH));
//        upButton.setEnabled(false); // starts off at 0, so can't go up
//        final JButton downButton = new JButton(new ArrowIcon(ArrowIcon.SOUTH));
//        if (model.getPageCount() <= 1) {
//            downButton.setEnabled(false); // One page...can't scroll down
//        }

//        upButton.addActionListener(ae -> {
//            model.pageUp();
//            // If we hit the top of the data, disable the up button.
//            if (model.getPageOffset() == 0) {
//                upButton.setEnabled(false);
//            }
//            downButton.setEnabled(true);
//        });
//
//        downButton.addActionListener(ae -> {
//            model.pageDown();
//            // If we hit the bottom of the data, disable the down button.
//            if (model.getPageOffset() == (model.getPageCount() - 1)) {
//                downButton.setEnabled(false);
//            }
//            upButton.setEnabled(true);
//        });

        // Turn on the scrollbars; otherwise we won't get our corners.
        jsp
                .setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS);
        jsp
                .setHorizontalScrollBarPolicy(ScrollPaneConstants.HORIZONTAL_SCROLLBAR_ALWAYS);
//
//        // Add in the corners (page up/down).
//        jsp.setCorner(ScrollPaneConstants.UPPER_RIGHT_CORNER, upButton);
//        jsp.setCorner(ScrollPaneConstants.LOWER_RIGHT_CORNER, downButton);

        return jsp;
    }
}