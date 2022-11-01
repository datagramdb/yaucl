package it.giacomobergami.knobab.gui;

import com.fasterxml.jackson.dataformat.csv.CsvMapper;
import com.fasterxml.jackson.dataformat.csv.CsvSchema;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import it.giacomobergami.knobab.http.Chooser;
import it.giacomobergami.knobab.http.PostQuery;
import it.giacomobergami.knobab.records.*;
import utils.external.jtables.ButtonColumn;
import utils.external.jtables.PagingModel;

import javax.swing.*;
import javax.swing.event.*;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.io.*;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.util.*;
import java.util.List;

public class EntryPoint extends JFrame {
    private JButton logsButton;
    private JPanel mainPanel;
    private JLabel logname;
    private JTabbedPane Visuals;
    private JCheckBox withDataCheckBox;
    private JCheckBox withMissingCheckBox;
    private JCheckBox noStatsCheckBox;
    private JTextField filename;
    private JButton fileChooser;
    private JCheckBox manualInput;
    private JTextField logFileName;
    private JComboBox comboBox1;
    private JTextArea manualInputText;
    private JButton sendButton;
    private JButton dropLogButton;
    private JButton dumpCSVButton;
    private JList activityLabels;
    private JList attributeList;
    private JPanel CountTablePanel;
    private JSlider theSlider;
    private JLabel pageCount;
    private JLabel totalPages;
    private JPanel ActTableTAB;
    private JLabel DisplayedAttributeName;
    private JPanel TableForDisplayedAttribute;
    private JTree tree1;
    String currentLog = null;
    private String url;
    CsvMapper csvMapper;
    CsvSchema loggerSchema;
    Gson gson;
    public List<LoggerInformation> logs;
    private String attributesArr[];
    private String activityLabelsArr[];
    ChangeListener prevListener = null;
    AbstractAction bogusAction;
    String selectedAttribute = null;
    Integer displayOffset = 0;

    private static void eradicate(JComponent ChildPanel2) {
        ChildPanel2.removeAll();
        ChildPanel2.revalidate();
        ChildPanel2.repaint();
    }

    private String[] listLogs() {
        String x = PostQuery.post(url, "list LOGS for \"\"");
        String options[] = null;
        if (!x.isEmpty()) {
            options = x.split("\\r?\\n");
        }
        return options;
    }

    private String[] listAttributes(String currentLog) {
        String x = PostQuery.post(url, "list ATTRIBUTES for "+escapeForJava(currentLog, true));
        String options[] = null;
        if (!x.isEmpty()) {
            options = x.split("\\r?\\n");
        }
        return options;
    }

    private String[] listActivityLabels(String currentLog) {
        String x = PostQuery.post(url, "list ACTIVITYLABEL for "+escapeForJava(currentLog, true));
        String options[] = null;
        if (!x.isEmpty()) {
            options = x.split("\\r?\\n");
        }
        return options;
    }

    private String displayCountTable(String currentLog) {
        return PostQuery.post(url, "display COUNTTABLE for "+escapeForJava(currentLog, true));
    }

    private String displayActTable(String currentLog) {
        return PostQuery.post(url, "display ACTTABLE for "+escapeForJava(currentLog, true));
    }

    private String displayAttTable(String currentLog, String attribute) {
        return PostQuery.post(url, "display ATTRIBUTETABLE "+escapeForJava(attribute, true) +" for "+escapeForJava(currentLog, true));
    }

    public void listLogs(ActionEvent e) {
        String choice = Chooser.stringOptions(listLogs(), "List of logs", "Please select a log: ").run();
        if (choice != null) {
            if (!Objects.equals(choice, currentLog))
                selectedAttribute = null;
            currentLog = choice;
            logname.setText(currentLog);
        }
    }

    public void onManualInput(ActionEvent e) {
        boolean isManualInput = manualInput.isSelected();
        manualInputText.setEnabled(isManualInput);
        fileChooser.setEnabled(!isManualInput);
        filename.setEnabled(!isManualInput);
    }

    public static String escapeForJava( String value, boolean quote )
    {
        StringBuilder builder = new StringBuilder();
        if( quote )
            builder.append( "\"" );
        for( char c : value.toCharArray() )
        {
            if( c == '\'' )
                builder.append( "\\'" );
            else if ( c == '\"' )
                builder.append( "\\\"" );
            else if( c == '\r' )
                builder.append( "\\r" );
            else if( c == '\n' )
                builder.append( "\\n" );
            else if( c == '\t' )
                builder.append( "\\t" );
            else if( c < 32 || c >= 127 )
                builder.append( String.format( "\\u%04x", (int)c ) );
            else
                builder.append( c );
        }
        if( quote )
            builder.append( "\"" );
        return builder.toString();
    }

    public void onSendLog(ActionEvent e) {
        StringBuilder sb = new StringBuilder("load ");
        boolean isManualInput = manualInput.isSelected();
        if (isManualInput) {
            sb.append("log {").append(manualInputText.getText()).append("} \"\" ");
        } else {
            int idx = comboBox1.getSelectedIndex();
            if (idx == 0) {
                sb.append("HRF  ");
            } else if (idx == 1) {
                sb.append("TAB  ");
            } else if (idx == 2) {
                sb.append("XES  ");
            }
            sb.append(escapeForJava(filename.getText(), true));
        }
        if (withDataCheckBox.isSelected())
            sb.append(" with data ");
        if (noStatsCheckBox.isSelected())
            sb.append(" no stats ");
        if (withMissingCheckBox.isSelected())
            sb.append(" with missing ");
        sb.append(" as ");
        sb.append(escapeForJava(logFileName.getText(), true));
        String json = PostQuery.post(url, sb.toString());
        LoggerInformation log = gson.fromJson(json, LoggerInformation.class);
        logs.add(log);
    }

    public void onDropLog(ActionEvent e) {
        String choice = Chooser.stringOptions(listLogs(), "List of logs", "Please select a log: ").run();
        if (choice != null) {
            PostQuery.post(url, "droplog " + escapeForJava(choice, true));
            if (Objects.equals(choice, currentLog)) {
                currentLog = null;
                logname.setText("<no log>");
                activityLabelsArr = null;
                attributesArr = null;
            }
        }
    }

    public void dumpCSV(ActionEvent e) {

        JFileChooser fileChooser = new JFileChooser();
        fileChooser.setDialogTitle("Specify a file to save the CSV done so far");
        int userSelection = fileChooser.showSaveDialog(this);

        if (userSelection == JFileChooser.APPROVE_OPTION) {
            File fileToSave = fileChooser.getSelectedFile();
            FileOutputStream fos = null;
            try {
                fos = new FileOutputStream(fileToSave);
                DataOutputStream outStream = new DataOutputStream(new BufferedOutputStream(fos));
                outStream.writeUTF(csvMapper.writer(loggerSchema).writeValueAsString(logs));
                outStream.close();;
            } catch (IOException ex) {
                ex.printStackTrace();
            }
            System.out.println("Saved as file: " + fileToSave.getAbsolutePath());
        }
    }

    public void setVisualTabChanged(ChangeEvent e) {
        if (currentLog != null) {
            JTabbedPane tabbedPane = (JTabbedPane) e.getSource();
            int selectedIndex = tabbedPane.getSelectedIndex();
            JList list = null;
            String x = "";
            String options[] = new String[0];
            if (prevListener != null) {
                theSlider.removeChangeListener(prevListener);
                prevListener = null;
                pageCount.setText("<na>");
                totalPages.setText("");
            }
            if (selectedIndex == 1) {
                if (attributesArr == null) {
                    attributesArr = listAttributes(currentLog);
                }
                options = attributesArr;
                list = attributeList;
                list.addMouseListener(new MouseInputListener() {
                    @Override
                    public void mouseDragged(MouseEvent e) {

                    }

                    @Override
                    public void mouseMoved(MouseEvent e) {

                    }

                    @Override
                    public void mouseClicked(MouseEvent e) {

                    }

                    @Override
                    public void mousePressed(MouseEvent e) {
                        JList source = (JList)e.getSource();
                        selectedAttribute = source.getSelectedValue().toString();
                    }

                    @Override
                    public void mouseReleased(MouseEvent e) {

                    }

                    @Override
                    public void mouseEntered(MouseEvent e) {

                    }

                    @Override
                    public void mouseExited(MouseEvent e) {

                    }
                });
            } else if (selectedIndex == 2) {
                if (activityLabelsArr == null) {
                    activityLabelsArr = listActivityLabels(currentLog);
                }
                options = activityLabelsArr;
                list = activityLabels;
            } else if (selectedIndex == 3) {
                visualizeCountTable();
            } else if (selectedIndex == 4) {
                visualizeActTable();
            } else if (selectedIndex == 5) {
                visualizeAttTable();
            }
            if (list != null) {
                DefaultListModel<String> listModel = new DefaultListModel<>();
                for (String option : options) {
                    listModel.addElement(option);
                }
                list.setModel(listModel);
            }
        }
    }

    private void visualizeAttTable() {
        if      (currentLog!= null && selectedAttribute != null) {
            DisplayedAttributeName.setText(selectedAttribute);
            var ls = displayAttTable(currentLog, selectedAttribute);
            var local = new PagingModel<>(new SplitterUtil<>(ls, ValueRecord.class) {
                @Override
                public Object cell(int row, int col) {
                    switch (col) {
                        case 0:
                            if (activityLabelsArr == null) {
                                activityLabelsArr = listActivityLabels(currentLog);
                            }
                            return activityLabelsArr[get(row).Act.intValue()];
                        case 1:
                            return get(row).Value;
                        case 2:
                            return String.valueOf(get(row).TableOffset);
                    }
                    return null;
                }

            }, new String[]{"Act", "Value", "TableOffset"}, 50);
            JTable jtlo = new JTable(local);
            ButtonColumn prevCol = new ButtonColumn(jtlo, bogusAction, 2);
            jtlo.addMouseListener(new MouseInputListener() {
                @Override
                public void mouseClicked(MouseEvent e) {}
                @Override
                public void mousePressed(MouseEvent e) {
                    if (jtlo.getSelectedColumn() >= 2) {
                        displayOffset = Integer.valueOf(local.getValueAt(jtlo.getSelectedRow(), jtlo.getSelectedColumn()).toString());
                        if (displayOffset > -1)
                            Visuals.setSelectedIndex(4);
                    }
                }
                @Override
                public void mouseReleased(MouseEvent e) {}
                @Override
                public void mouseEntered(MouseEvent e) {}
                @Override
                public void mouseExited(MouseEvent e) {}
                @Override
                public void mouseDragged(MouseEvent e) {}
                @Override
                public void mouseMoved(MouseEvent e) {}
            });
            eradicate(TableForDisplayedAttribute);
            var jsp2 = PagingModel.createPagingScrollPaneForTable(jtlo);
            totalPages.setText("/"+local.getPageCount());
            theSlider.setMaximum(local.getPageCount());
            theSlider.setMinimum(0);
            theSlider.setValue(0);
            prevListener = e1 -> {
                var i = theSlider.getValue();
                pageCount.setText(String.valueOf(i));
                local.setPage(i);
            };
            theSlider.addChangeListener(prevListener);
            TableForDisplayedAttribute.add(jsp2);
        } else {
            eradicate(TableForDisplayedAttribute);
            DisplayedAttributeName.setText("<no attribute>");
        }

    }

    private void visualizeActTable() {
        var ls = displayActTable(currentLog);
        PagingModel<ActTableRecord> pm;
        pm = new PagingModel<>(new SplitterUtil<>(ls, ActTableRecord.class) {
            @Override
            public Object cell(int row, int col) {
                switch (col) {
                    case 0:
                        if (activityLabelsArr == null) {
                            activityLabelsArr = listActivityLabels(currentLog);
                        }
                        return activityLabelsArr[get(row).Act.intValue()];
                    case 1:
                        return String.valueOf(get(row).TraceId);
                    case 2:
                        return String.valueOf(get(row).EventId);
                    case 3:
                        return String.valueOf(get(row).PREVPTR);
                    case 4:
                        return String.valueOf(get(row).NEXTPTR);
                }
                return null;
            }

        }, new String[]{"Act", "TraceId", "EventId", "Prev", "Next"}, 50);

        JTable jt;
        jt = new JTable(pm);
        ButtonColumn prevCol = new ButtonColumn(jt, bogusAction, 3);
        ButtonColumn nextCol = new ButtonColumn(jt, bogusAction, 4);
        jt.addMouseListener(new MouseInputListener() {
            @Override
            public void mouseClicked(MouseEvent e) {}
            @Override
            public void mousePressed(MouseEvent e) {
                if (jt.getSelectedColumn() >= 3) {
                    Integer rowIdx = Integer.valueOf(pm.getValueAt(jt.getSelectedRow(), jt.getSelectedColumn()).toString());
                    if (rowIdx > -1)
                        pm.navigateToRow(rowIdx, jt);
                }
            }
            @Override
            public void mouseReleased(MouseEvent e) {}
            @Override
            public void mouseEntered(MouseEvent e) {}
            @Override
            public void mouseExited(MouseEvent e) {}
            @Override
            public void mouseDragged(MouseEvent e) {}
            @Override
            public void mouseMoved(MouseEvent e) {}
        });
        eradicate(ActTableTAB);
        var jsp2 = PagingModel.createPagingScrollPaneForTable(jt);
        totalPages.setText("/"+pm.getPageCount());
        theSlider.setMaximum(pm.getPageCount());
        theSlider.setMinimum(0);
        theSlider.setValue(0);
        prevListener = e1 -> {
            var i = theSlider.getValue();
            pageCount.setText(String.valueOf(i));
            pm.setPage(i);
        };
        theSlider.addChangeListener(prevListener);
        ActTableTAB.add(jsp2);
        if ((displayOffset != null) && (displayOffset < 0)) displayOffset = null;
        if (displayOffset != null) {
            pm.navigateToRow(displayOffset, jt);
            displayOffset = null;
        }
    }

    private void visualizeCountTable() {
        var ls = displayCountTable(currentLog);
        PagingModel<CountTableRecord> pm = new PagingModel<>(new SplitterUtil<>(ls, CountTableRecord.class) {
            @Override
            public Object cell(int row, int col) {
                switch (col) {
                    case 0:
                        if (activityLabelsArr == null) {
                            activityLabelsArr = listActivityLabels(currentLog);
                        }
                        return activityLabelsArr[get(row).Act.intValue()];
                    case 1:
                        return String.valueOf(get(row).TraceId);
                    case 2:
                        return String.valueOf(get(row).Count);
                }
                return null;
            }

        }, new String[]{"Act", "TraceId", "Count"}, 50);
        JTable jt = new JTable(pm);
        eradicate(CountTablePanel);
        var jsp = PagingModel.createPagingScrollPaneForTable(jt);
        totalPages.setText("/"+pm.getPageCount());
        theSlider.setMaximum(pm.getPageCount());
        theSlider.setMinimum(0);
        theSlider.setValue(0);
        prevListener = e1 -> {
            var i = theSlider.getValue();
            pageCount.setText(String.valueOf(i));
            pm.setPage(i);
        };
        theSlider.addChangeListener(prevListener);
        CountTablePanel.add(jsp);
    }

    public EntryPoint(String url) {
        this.url = url;
        prevListener = null;
        csvMapper = new CsvMapper();
        logs = new ArrayList<>();
        gson = new GsonBuilder().create();
        setContentPane(mainPanel);
        setTitle("KnoBAB - GUI");
        setSize(400, 500);
        pageCount.setText("<na>");
        totalPages.setText("");
        setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        loggerSchema = csvMapper.schemaFor(LoggerInformation.class).withHeader();
        loggerSchema = csvMapper.schemaFor(CountTableRecord.class).withHeader();
        logsButton.addActionListener(this::listLogs);
        manualInput.addActionListener(this::onManualInput);
        sendButton.addActionListener(this::onSendLog);
        dropLogButton.addActionListener(this::onDropLog);
        dumpCSVButton.addActionListener(this::dumpCSV);
        Visuals.addChangeListener(this::setVisualTabChanged);
        onManualInput(null);
        setVisible(true);
        bogusAction = new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent e) {

            }
        };
    }

    private void createUIComponents() {
        // TODO: place custom component creation code here



    }
}
